"""Pawn AMX -> pseudo-Pawn decompiler (expression level, goto-based control flow).

Produces, for every function, source-like code annotated with original line numbers.
"""
import sys, bisect, struct, collections
import amxload

# ---------------------------------------------------------------- expressions
class E:
    pass

class C(E):          # constant
    def __init__(s, v): s.v = v
class Loc(E):        # value of local/arg at frame offset
    def __init__(s, off): s.off = off
class LocAddr(E):    # address of local at frame offset
    def __init__(s, off): s.off = off
class Glob(E):       # value of global scalar at data address
    def __init__(s, a): s.a = a
class Ld(E):         # load from address expr
    def __init__(s, addr): s.addr = addr
class Ref(E):        # deref of reference arg (lref.s)
    def __init__(s, off): s.off = off
class Bin(E):
    def __init__(s, op, a, b): s.op = op; s.a = a; s.b = b
class Un(E):
    def __init__(s, op, a): s.op = op; s.a = a
class Call(E):
    def __init__(s, name, args, native=False): s.name = name; s.args = args; s.native = native
class Heap(E):       # heap temp: value stored into it
    def __init__(s, n): s.n = n; s.val = None
class Tern(E):
    def __init__(s, c, a, b): s.c = c; s.a = a; s.b = b
class Raw(E):
    def __init__(s, t): s.t = t
class Assign(E):     # assignment used as value
    def __init__(s, lhs, rhs): s.lhs = lhs; s.rhs = rhs
class Inc(E):
    def __init__(s, target, d, pre=True): s.t = target; s.d = d; s.pre = pre

PREC = {'*': 13, '/': 13, '%': 13, '+': 12, '-': 12, '<<': 11, '>>': 11, '>>>': 11,
        '<': 10, '<=': 10, '>': 10, '>=': 10, '==': 9, '!=': 9, '&': 8, '^': 7, '|': 6,
        '&&': 5, '||': 4}


def nice_float(v):
    b = struct.pack('<i', v)
    f = struct.unpack('<f', b)[0]
    if f != f or f in (float('inf'), float('-inf')) or f == 0:
        return None
    if abs(f) < 1e-4 or abs(f) > 1e8:
        return None
    for p in range(1, 10):
        r = '%.*g' % (p, f)
        if struct.pack('<f', float(r)) == b:
            if 'e' in r:
                return None
            if p > 7:
                return None
            return r
    return None


def fmt_const(v):
    if v == -2147483648:
        return 'cellmin'
    if abs(v) >= 0x01000000:
        nf = nice_float(v)
        if nf is not None:
            return nf if '.' in nf else nf + '.0'
        return '0x%08X' % (v & 0xffffffff)
    return str(v)


FLOATOPS = {'floatadd': '+', 'floatsub': '-', 'floatmul': '*', 'floatdiv': '/'}


class Decomp:
    def __init__(self, amx):
        self.a = amx
        a = amx
        self.funcs = sorted([s for s in a.syms if s.ident == 9], key=lambda s: s.addr)
        self.fbyaddr = {s.addr: s for s in self.funcs}
        gl = sorted([s for s in a.syms if s.vclass in (0, 2) and s.ident != 9], key=lambda s: (s.addr, -self.gsize(s)))
        self.glob = gl
        self.gstarts = [s.addr for s in gl]
        self.gbyaddr = {}
        for s in gl:
            self.gbyaddr.setdefault(s.addr, s)
        self.lines = sorted(a.lines)
        self.laddr = [l[0] for l in self.lines]
        self.files = sorted(a.files)
        self.faddr = [f[0] for f in self.files]
        self.tagname = {}
        for k, v in a.dtags.items():
            self.tagname[k] = v

    def gsize(self, s):
        if not s.dims:
            return 4
        m = 1; t = 0
        for _, sz in s.dims:
            m *= sz; t += m
        return t * 4

    def global_at(self, v):
        i = bisect.bisect_right(self.gstarts, v) - 1
        cands = []
        j = i
        while j >= 0 and j > i - 6:
            s = self.glob[j]
            if s.addr <= v < s.addr + self.gsize(s):
                cands.append(s)
            j -= 1
        if not cands:
            return None
        cf = getattr(self, 'f', None)
        for s in cands:
            if s.vclass == 2 and cf is not None and cf.cs <= s.cs < cf.ce:
                return s
        for s in cands:
            if s.vclass == 0:
                return s
        return cands[0]

    def literal(self, v, maxlen=None):
        if v < 8 or v >= len(self.a.data) or v % 4:
            return None
        g = self.global_at(v)
        if g is not None:
            return None
        bs = []
        p = v
        while p + 4 <= len(self.a.data):
            c = self.a.dcell(p)
            if c == 0:
                break
            if c < 0 or c > 0x10FFFF:
                return None
            bs.append(c)
            p += 4
            if maxlen is not None and len(bs) >= maxlen:
                break
            if len(bs) > 4000:
                return None
        try:
            if all(c < 256 for c in bs):
                s = bytes(bs).decode('utf-8')
            else:
                s = ''.join(chr(c) for c in bs)
        except UnicodeDecodeError:
            s = bytes(c & 255 for c in bs).decode('latin1')
        return s

    def lineof(self, addr):
        i = bisect.bisect_right(self.laddr, addr) - 1
        return self.lines[i][1] + 1 if i >= 0 else 0

    def fileof(self, addr):
        i = bisect.bisect_right(self.faddr, addr) - 1
        return self.files[i][1] if i >= 0 else '?'

    # ------------------------------------------------------------ naming
    def localname(self, off, pc):
        best = None
        for s in self.cur_locals:
            if s.addr == off and s.cs <= pc < s.ce + 8:
                if best is None or s.cs > best.cs:
                    best = s
        if best is None:
            for s in self.cur_locals:
                if s.addr == off and s.cs >= pc and (best is None or s.cs < best.cs):
                    best = s
        if best is None:
            for s in self.cur_locals:
                if s.addr == off:
                    best = s
        return best

    def str_lit(self, s):
        out = '"'
        for ch in s:
            if ch == '"':
                out += '\\"'
            elif ch == '\\':
                out += '\\\\'
            elif ch == '\n':
                out += '\\n'
            elif ch == '\t':
                out += '\\t'
            elif ch == '\r':
                out += '\\r'
            elif ord(ch) < 32:
                out += '\\%d;' % ord(ch)
            else:
                out += ch
        return out + '"'

    # ------------------------------------------------------------ address decomposition
    def lval(self, e, pc, elem=False):
        """Return string lvalue name for an address expression, or None."""
        r = self.addr_parse(e, pc)
        if r is None:
            return None
        base, idx, ndims, state = r
        if elem and state == 'start' and ndims > 0 and len(idx) < ndims:
            idx = idx + ['0']
        s = base
        for i in idx:
            s += '[' + i + ']'
        return s

    def addr_parse(self, e, pc):
        # returns (basename, [idx strings], ndims, state) ; state 'elem' or 'ind' or 'sub'
        if isinstance(e, C):
            g = self.global_at(e.v)
            if g is None or not g.dims:
                return None
            if g.addr == e.v:
                return (g.name, [], len(g.dims), 'start')
            if len(g.dims) == 1:
                return (g.name, [str((e.v - g.addr) // 4)], 1, 'elem')
            return None
        if isinstance(e, LocAddr):
            s = self.localname(e.off, pc)
            if s is None:
                return ('_loc%d' % e.off if e.off < 0 else '_arg%d' % ((e.off - 12) // 4), [], 1, 'start')
            return (s.name, [], len(s.dims) if s.dims else 0, 'start')
        if isinstance(e, Loc):
            s = self.localname(e.off, pc)
            if s is not None and s.ident in (3, 4) :
                return (s.name, [], len(s.dims) if s.dims else 1, 'start')
            return None
        if isinstance(e, Glob):
            return None
        if isinstance(e, Bin) and e.op == '+':
            # row deref pattern: Ld(R) + R
            for x, y in ((e.a, e.b), (e.b, e.a)):
                if isinstance(x, Ld) and self.same(x.addr, y):
                    r = self.addr_parse(y, pc)
                    if r is not None:
                        return (r[0], r[1], r[2], 'start')
            for x, y in ((e.a, e.b), (e.b, e.a)):
                r = self.addr_parse(x, pc)
                if r is None or r[3] != 'start':
                    continue
                idx = self.index_of(y, pc)
                if idx is None:
                    continue
                return (r[0], r[1] + [idx], r[2], 'start' if len(r[1]) + 1 < r[2] else 'elem')
            for x, y in ((e.a, e.b), (e.b, e.a)):
                r = self.addr_parse(x, pc)
                if r is None or r[3] != 'elem':
                    continue
                idx = self.index_of(y, pc)
                if idx is None:
                    continue
                return (r[0], r[1] + [idx], r[2], 'elem2')
        return None

    def index_of(self, y, pc):
        if isinstance(y, C):
            if y.v % 4 == 0:
                return str(y.v // 4)
            return None
        if isinstance(y, Bin) and y.op == '<<' and isinstance(y.b, C) and y.b.v == 2:
            return self.ex(y.a, pc, 0)
        if isinstance(y, Bin) and y.op == '*' and isinstance(y.b, C) and y.b.v == 4:
            return self.ex(y.a, pc, 0)
        return None

    def same(self, x, y):
        if type(x) is not type(y):
            return False
        if x is y:
            return True
        if isinstance(x, C):
            return x.v == y.v
        if isinstance(x, (Loc, LocAddr, Ref)):
            return x.off == y.off
        if isinstance(x, Glob):
            return x.a == y.a
        if isinstance(x, Ld):
            return self.same(x.addr, y.addr)
        if isinstance(x, Bin):
            return x.op == y.op and self.same(x.a, y.a) and self.same(x.b, y.b)
        if isinstance(x, Un):
            return x.op == y.op and self.same(x.a, y.a)
        return False

    # ------------------------------------------------------------ printing
    def ex(self, e, pc, prec=0, asarg=False):
        if isinstance(e, C):
            v = e.v
            if asarg:
                s = self.literal(v)
                if s is not None and v > 64:
                    return self.str_lit(s)
                g = self.global_at(v)
                if g is not None and g.addr == v and v > 0:
                    return g.name
                if g is not None and v > 0:
                    lv = None
            return fmt_const(v)
        if isinstance(e, Loc):
            s = self.localname(e.off, pc)
            return s.name if s else ('_loc%d' % -e.off if e.off < 0 else '_arg%d' % ((e.off - 12) // 4))
        if isinstance(e, Ref):
            s = self.localname(e.off, pc)
            return s.name if s else '_ref%d' % e.off
        if isinstance(e, LocAddr):
            r = self.lval(e, pc)
            return ('&' if not asarg else '') + r if r else '&?'
        if isinstance(e, Glob):
            g = self.global_at(e.a)
            if g is not None:
                if g.dims and len(g.dims) == 1:
                    return '%s[%d]' % (g.name, (e.a - g.addr) // 4)
                if g.addr == e.a:
                    return g.name
                return '%s@+%d' % (g.name, e.a - g.addr)
            return 'DATA[%d]' % e.a
        if isinstance(e, Ld):
            lv = self.lval(e.addr, pc, True)
            if lv is not None:
                return lv
            return '*(' + self.ex(e.addr, pc, 0) + ')'
        if isinstance(e, Heap):
            if e.val is not None:
                return self.ex(e.val, pc, prec, asarg)
            return '/*heap%d*/' % e.n
        if isinstance(e, Call):
            if e.name.startswith('operator') and '(' in e.name and len(e.args) in (1, 2):
                o = e.name[8:e.name.index('(')]
                if len(e.args) == 2:
                    p = PREC.get(o, 9)
                    s = '%s %s %s' % (self.ex(e.args[0], pc, p), o, self.ex(e.args[1], pc, p + 1))
                    return '(' + s + ')' if p < prec else s
                return o + self.ex(e.args[0], pc, 14)
            if e.name in FLOATOPS and len(e.args) == 2:
                o = FLOATOPS[e.name]
                p = PREC[o]
                s = '%s %s %s' % (self.ex(e.args[0], pc, p), o, self.ex(e.args[1], pc, p + 1))
                return '(' + s + ')' if p < prec else s
            args = ', '.join(self.ex(x, pc, 0, True) for x in e.args)
            return '%s(%s)' % (e.name, args)
        if isinstance(e, Un):
            if e.op == '!' and isinstance(e.a, Bin) and e.a.op in ('==', '!=') and False:
                pass
            inner = self.ex(e.a, pc, 14)
            return e.op + inner
        if isinstance(e, Bin):
            if e.op in ('+',) and asarg is not None:
                lv = self.lval(e, pc)
                if lv is not None:
                    return lv if asarg else lv + '/*addr*/'
            p = PREC.get(e.op, 1)
            s = '%s %s %s' % (self.ex(e.a, pc, p), e.op, self.ex(e.b, pc, p + 1))
            return '(' + s + ')' if p < prec else s
        if isinstance(e, Tern):
            s = '%s ? %s : %s' % (self.ex(e.c, pc, 4), self.ex(e.a, pc, 4, asarg), self.ex(e.b, pc, 4, asarg))
            return '(' + s + ')' if prec > 0 else s
        if isinstance(e, Assign):
            s = '%s = %s' % (self.ex(e.lhs, pc, 0), self.ex(e.rhs, pc, 0))
            return '(' + s + ')'
        if isinstance(e, Raw):
            return e.t
        return '/*?%s*/' % type(e).__name__

    # ------------------------------------------------------------ function decompile
    def decompile(self, f):
        a = self.a
        self.cur_locals = [s for s in a.syms if s.vclass == 1 and f.cs <= s.cs <= f.ce]
        insns = list(a.disasm(f.cs, f.ce))
        self.insns = insns
        self.idx = {ad: i for i, (ad, _, _) in enumerate(insns)}
        # jump targets
        targets = set()
        for ad, op, ar in insns:
            if op in ('jump', 'jzer', 'jnz', 'jeq', 'jneq', 'jsless', 'jsleq', 'jsgrtr', 'jsgeq', 'switch'):
                targets.add(ar[0])
            if op == 'casetbl':
                targets.add(ar[1])
                for v, t in ar[2:]:
                    targets.add(t)
        self.targets = targets
        # merge patterns: const.pri 1 ; jump X ; zero.pri  (X = after zero.pri)
        merges = {}
        for i in range(len(insns) - 2):
            if insns[i][1] == 'const.pri' and insns[i][2] == [1] and insns[i + 1][1] == 'jump' and insns[i + 2][1] == 'zero.pri' \
                    and insns[i + 1][2][0] == insns[i + 2][0] + 4:
                merges[insns[i][0]] = (insns[i][0], insns[i + 2][0], insns[i + 1][2][0])
        self.merges = merges
        self.mtrue = {m[0]: m for m in merges.values()}
        self.mfalse = {m[1]: m for m in merges.values()}
        self.out = []
        self.stmt_line = self.lineof(f.cs)
        self.internal = set()
        for k, (ad, op, ar) in enumerate(insns):
            if op in ('jzer', 'jnz'):
                m = self.find_merge(ad, ar[0])
                if m is not None and ar[0] not in (m[0], m[1]):
                    self.internal.add(ar[0])
        self.heapn = 0
        self.lbl_state = {}
        self.decl = set()
        st = {'pri': C(0), 'alt': C(0), 'stk': [], 'sp': 0}
        self.pending = None
        self.sc = collections.defaultdict(list)  # merge true addr -> nodes
        self.tern = []  # stack of (cond, endaddr?)
        self.run(st, 0, len(insns))
        return self.out

    def emit(self, pc, text):
        ln = self.stmt_line if text[:2] != 'L_' else self.lineof(pc)
        self.out.append((pc, ln, text))

    def flush(self, pc):
        if self.pending is not None:
            e = self.pending
            self.pending = None
            self.emit(pc, self.ex(e, pc) + ';')

    def use(self, st):
        self.pending = None
        return st['pri']

    def next_break(self, i):
        j = i + 1
        while j < len(self.insns) and self.insns[j][1] not in ('break',):
            j += 1
        return self.insns[j][0] if j < len(self.insns) else 1 << 40

    def find_merge(self, pc, t):
        """merge (true,false,end) that this conditional jump at pc to t belongs to, if any"""
        nb = self.next_break(self.idx[pc])
        best = None
        for m in sorted(self.merges.values()):
            if m[0] <= pc or m[0] > nb:
                continue
            if t in (m[0], m[1]):
                return m
            if pc < t <= m[0]:
                k = self.idx.get(t)
                if k is not None and self.insns[k - 1][1] == 'jump':
                    return None  # ternary branch, not short-circuit
                return m
        return None

    def run(self, st, i0, i1):
        insns = self.insns
        i = i0
        while i < i1:
            pc, op, ar = insns[i]
            # labels
            if pc in self.targets and pc not in self.mfalse and pc not in self.mtrue and not self.in_expr(pc):
                self.flush(pc)
                self.emit(pc, 'L_%x:' % pc)
                if pc in self.lbl_state:
                    stk, sp = self.lbl_state[pc]
                    st['stk'] = list(stk); st['sp'] = sp
            # merge reached
            if pc in self.mtrue:
                m = self.mtrue[pc]
                nodes = self.sc.pop(m[0], [])
                e = self.reduce(nodes, m, pc)
                st['pri'] = e
                i = self.idx[m[2]]
                continue
            r = self.step(st, i)
            if r is None:
                i += 1
            else:
                i = r
        return st

    def in_expr(self, pc):
        if pc in self.internal:
            return True
        for k, nodes in self.sc.items():
            if any(n[4] == pc for n in nodes):
                return True
        if self.tern and any(pc in (t[1], t[2]) for t in self.tern):
            return True
        return False

    def reduce(self, nodes, m, pc):
        # nodes: list of (start, cond, kind, target, nextstart)
        T, F = m[0], m[1]
        N = []
        for k, (start, cond, kind, target, nxt) in enumerate(nodes):
            nx = nodes[k + 1][0] if k + 1 < len(nodes) else T
            if kind == 'jzer':
                N.append([start, cond, nx, target])
            else:
                N.append([start, cond, target, nx])
        def norm(x):
            return 'T' if x == T else 'F' if x == F else x
        for n in N:
            n[2] = norm(n[2]); n[3] = norm(n[3])
        changed = True
        while len(N) > 1 and changed:
            changed = False
            for k in range(len(N) - 1):
                a, b = N[k], N[k + 1]
                refs = sum(1 for z in N if z is not a and (z[2] == b[0] or z[3] == b[0]))
                if refs:
                    continue
                if a[2] == b[0] and a[3] == b[3]:
                    N[k] = [a[0], Bin('&&', a[1], b[1]), b[2], b[3]]
                elif a[3] == b[0] and a[2] == b[2]:
                    N[k] = [a[0], Bin('||', a[1], b[1]), b[2], b[3]]
                elif a[2] == b[0] and a[3] == b[2]:
                    N[k] = [a[0], Bin('&&', a[1], Un('!', b[1])), b[3], b[2]]
                elif a[3] == b[0] and a[2] == b[3]:
                    N[k] = [a[0], Bin('||', a[1], Un('!', b[1])), b[3], b[2]]
                else:
                    continue
                del N[k + 1]
                changed = True
                break
        if len(N) == 1:
            n = N[0]
            if n[2] == 'T' and n[3] == 'F':
                return n[1]
            if n[2] == 'F' and n[3] == 'T':
                return Un('!', n[1])
        return Raw('/*SC?*/(' + ' ; '.join('%s?%s:%s' % (self.ex(n[1], pc), n[2], n[3]) for n in N) + ')')

    def push(self, st, e):
        st['stk'].append(e)
        st['sp'] -= 4

    def pop(self, st):
        st['sp'] += 4
        if not st['stk']:
            return Raw('/*underflow*/')
        return st['stk'].pop()

    def nlocals(self, st):
        return len([1 for x in st['stk'] if isinstance(x, Raw) and x.t.startswith('<local')])

    def step(self, st, i):
        pc, op, ar = self.insns[i]
        P = st['pri']; A = st['alt']
        def setpri(e):
            st['pri'] = e
        if op == 'proc':
            return None
        if op == 'break':
            self.flush(pc)
            self.declare_pending(st, pc)
            self.stmt_line = self.lineof(pc)
            return None
        if op == 'load.pri':
            setpri(Glob(ar[0]))
        elif op == 'load.alt':
            st['alt'] = Glob(ar[0])
        elif op == 'load.s.pri':
            setpri(Loc(ar[0]))
        elif op == 'load.s.alt':
            st['alt'] = Loc(ar[0])
        elif op == 'lref.s.pri':
            setpri(Ref(ar[0]))
        elif op == 'lref.s.alt':
            st['alt'] = Ref(ar[0])
        elif op == 'load.i':
            self.pending = None
            setpri(Ld(P))
        elif op == 'const.pri':
            setpri(C(ar[0]))
        elif op == 'const.alt':
            st['alt'] = C(ar[0])
        elif op == 'zero.pri':
            setpri(C(0))
        elif op == 'zero.alt':
            st['alt'] = C(0)
        elif op == 'addr.pri':
            setpri(LocAddr(ar[0]))
        elif op == 'addr.alt':
            st['alt'] = LocAddr(ar[0])
        elif op == 'push.pri':
            self.pending = None
            self.push(st, P)
        elif op == 'push.alt':
            self.push(st, A)
        elif op == 'push.c':
            self.push(st, C(ar[0]))
        elif op == 'pop.pri':
            setpri(self.pop(st))
        elif op == 'pop.alt':
            st['alt'] = self.pop(st)
        elif op == 'move.pri':
            setpri(A)
        elif op == 'xchg':
            self.pending = None
            st['pri'], st['alt'] = A, P
        elif op == 'swap.pri':
            self.pending = None
            top = self.pop(st)
            self.push(st, P)
            setpri(top)
        elif op == 'shl.c.pri':
            self.pending = None
            setpri(Bin('<<', P, C(ar[0])))
        elif op in ('add', 'smul', 'and', 'or', 'xor', 'eq', 'neq', 'sless', 'sleq', 'sgrtr', 'sgeq'):
            self.pending = None
            o = {'add': '+', 'smul': '*', 'and': '&', 'or': '|', 'xor': '^', 'eq': '==', 'neq': '!=',
                 'sless': '<', 'sleq': '<=', 'sgrtr': '>', 'sgeq': '>='}[op]
            if op in ('add', 'smul', 'and', 'or', 'xor', 'eq', 'neq'):
                # PRI op ALT ; operand order: alt was pushed first usually (left operand) -> ALT op PRI
                setpri(Bin(o, P, A) if (isinstance(A, C) and not isinstance(P, C)) else Bin(o, A, P))
            else:
                setpri(Bin(o, P, A))
        elif op == 'sub.alt':
            self.pending = None
            setpri(Bin('-', A, P))
        elif op == 'sdiv.alt':
            self.pending = None
            setpri(Bin('/', A, P))
            st['alt'] = Bin('%', A, P)
        elif op in ('shl', 'shr', 'sshr'):
            self.pending = None
            setpri(Bin({'shl': '<<', 'shr': '>>>', 'sshr': '>>'}[op], P, A))
        elif op == 'neg':
            self.pending = None
            setpri(Un('-', P))
        elif op == 'invert':
            self.pending = None
            setpri(Un('~', P))
        elif op == 'not':
            self.pending = None
            setpri(Un('!', P))
        elif op == 'inc.pri':
            setpri(Bin('+', P, C(1)))
        elif op == 'bounds':
            pass
        elif op == 'stor.pri':
            self.pending = None
            self.store(st, Glob(ar[0]), P, pc)
        elif op == 'stor.s.pri':
            self.pending = None
            self.store_local(st, ar[0], P, pc)
        elif op == 'sref.s.pri':
            self.pending = None
            self.store(st, Ref(ar[0]), P, pc)
        elif op == 'stor.i':
            self.pending = None
            if isinstance(A, Heap):
                A.val = P
            else:
                self.store(st, Ld(A), P, pc)
        elif op in ('inc.s', 'dec.s'):
            self.flush(pc)
            self.emit(pc, self.ex(Loc(ar[0]), pc) + ('++;' if op == 'inc.s' else '--;'))
        elif op in ('inc', 'dec'):
            self.flush(pc)
            self.emit(pc, self.ex(Glob(ar[0]), pc) + ('++;' if op == 'inc' else '--;'))
        elif op in ('inc.i', 'dec.i'):
            self.flush(pc)
            self.emit(pc, self.ex(Ld(P), pc) + ('++;' if op == 'inc.i' else '--;'))
        elif op == 'heap':
            if ar[0] > 0:
                self.heapn += 1
                h = Heap(self.heapn)
                st['alt'] = h
            # negative: release
        elif op == 'fill':
            self.pending = None
            lv = self.lval(A, pc) or self.ex(A, pc)
            if not (isinstance(P, C) and P.v == 0 and self.is_decl_fill(A, st)):
                self.emit(pc, '__fill(%s, %s, %d);' % (lv, self.ex(P, pc), ar[0] // 4))
        elif op == 'movs':
            self.pending = None
            src = P; dst = A
            s = self.literal(src.v) if isinstance(src, C) else None
            lv = self.lval(dst, pc) or self.ex(dst, pc)
            if s is not None:
                self.emit(pc, '%s = %s; /*movs %d*/' % (lv, self.str_lit(s), ar[0] // 4))
            else:
                self.emit(pc, '%s = %s; /*movs %d*/' % (lv, self.lval(src, pc) or self.ex(src, pc, 0, True), ar[0] // 4))
        elif op == 'stack':
            n = ar[0]
            if n < 0:
                for k in range(-n // 4):
                    st['stk'].append(Raw('<local>'))
                st['sp'] += n
                off = st['sp']
                s = self.localname(off, pc)
                if s is not None:
                    self.emit(pc, 'new %s%s;' % (self.tagpref(s), s.name + self.dimstr(s)))
                    self.decl.add((off, s.name))
                else:
                    self.emit(pc, 'new _loc%d[%d];' % (-off, -n // 4))
            else:
                for k in range(n // 4):
                    if st['stk']:
                        st['stk'].pop()
                st['sp'] += n
        elif op == 'call':
            self.flush(pc)
            nb = self.pop(st)
            n = nb.v // 4 if isinstance(nb, C) else 0
            args = [self.pop(st) for _ in range(n)]
            f = self.fbyaddr.get(ar[0])
            name = f.name if f else 'func_%x' % ar[0]
            e = Call(name, args)
            setpri(e)
            self.pending = e
        elif op == 'sysreq.c':
            self.flush(pc)
            # args: top of stack is byte count
            nb = st['stk'][-1] if st['stk'] else C(0)
            n = nb.v // 4 if isinstance(nb, C) else 0
            args = [st['stk'][-2 - k] for k in range(n)] if len(st['stk']) > n else []
            e = Call(self.a.natives[ar[0]], args, True)
            setpri(e)
            self.pending = e
            # following 'stack n+4' pops
        elif op == 'retn':
            self.flush(pc)
            self.emit(pc, 'return %s;' % self.ex(P, pc))
        elif op == 'halt':
            self.emit(pc, '/*halt*/')
        elif op == 'jump':
            t = ar[0]
            # ternary: inside expression?
            if self.tern and self.tern[-1][1] is None and t > pc and self.insns[i + 1][0] in (self.tern[-1][2],):
                tc = self.tern[-1]
                tc[1] = t
                tc[3] = P
                return None
            self.flush(pc)
            self.lbl_state.setdefault(t, (list(st['stk']), st['sp']))
            self.emit(pc, 'goto L_%x;' % t)
        elif op in ('jzer', 'jnz'):
            t = ar[0]
            m = self.find_merge(pc, t)
            if m is not None:
                nodes = self.sc[m[0]]
                start = nodes[-1][4] if nodes else None
                nodes.append((start, P, op, t, self.insns[i + 1][0]))
                # node start for next = next insn
                lst = list(nodes[-1]); nodes[-1] = (start if nodes[:-1] else pc, P, op, t, self.insns[i + 1][0])
                # fix starts: node start is first insn after previous node's jump
                if len(nodes) > 1:
                    prev = nodes[-2]
                    nodes[-1] = (prev[4], P, op, t, self.insns[i + 1][0])
                return None
            nb = self.next_break(i)
            if pc < t < nb and t not in self.mfalse:
                # ternary: cond ? a : b
                return self.ternary(st, i, t)
            self.flush(pc)
            self.lbl_state.setdefault(t, (list(st['stk']), st['sp']))
            if op == 'jzer':
                c = P
                if isinstance(c, Un) and c.op == '!':
                    self.emit(pc, 'if (%s) goto L_%x;' % (self.ex(c.a, pc), t))
                else:
                    self.emit(pc, 'if (!(%s)) goto L_%x;' % (self.ex(c, pc), t))
            else:
                self.emit(pc, 'if (%s) goto L_%x;' % (self.ex(P, pc), t))
        elif op == 'switch':
            self.flush(pc)
            ct = self.insns[self.idx[ar[0]]]
            cnt, dflt = ct[2][0], ct[2][1]
            for v, tt in ct[2][2:] + [(None, dflt)]:
                self.lbl_state.setdefault(tt, (list(st['stk']), st['sp']))
            cases = ' '.join('case %d: goto L_%x;' % (v, tt) for v, tt in ct[2][2:])
            self.emit(pc, 'switch (%s) { %s default: goto L_%x; }' % (self.ex(P, pc), cases, dflt))
        elif op == 'casetbl':
            pass
        else:
            self.emit(pc, '/*UNHANDLED %s %s*/' % (op, ar))
        return None

    def ternary(self, st, i, t):
        pc, op, ar = self.insns[i]
        cond = st['pri'] if op == 'jzer' else Un('!', st['pri'])
        # run true branch until 'jump end' just before t
        j = self.idx[t] - 2 if self.insns[self.idx[t] - 1][1] != 'jump' else self.idx[t] - 1
        jidx = self.idx[t] - 1
        if self.insns[jidx][1] != 'jump':
            self.emit(pc, '/*TERNARY? at %x*/' % pc)
            return None
        end = self.insns[jidx][2][0]
        saved_stk = list(st['stk']); saved_sp = st['sp']
        self.run(st, i + 1, jidx)
        a = st['pri']
        st['stk'] = list(saved_stk); st['sp'] = saved_sp
        self.run(st, self.idx[t], self.idx[end])
        b = st['pri']
        st['pri'] = Tern(cond, a, b)
        self.pending = None
        return self.idx[end]

    def is_decl_fill(self, A, st):
        return isinstance(A, LocAddr)

    def tagpref(self, s):
        t = self.tagname.get(s.tag, '_')
        if t in ('_',) or t is None:
            return ''
        return t + ':'

    def dimstr(self, s):
        out = ''
        for t, sz in s.dims:
            tn = self.tagname.get(t, '_')
            out += '[%s]' % (tn if tn != '_' else sz) if tn != '_' else '[%d]' % sz
        return out

    def store_local(self, st, off, v, pc):
        # declaration via stack -4 already emitted; this is an assignment
        s = self.localname(off, pc)
        name = s.name if s else '_loc%d' % -off
        self.emit_assign(name, v, pc, Loc(off))

    def store(self, st, target, v, pc):
        name = self.ex(target, pc)
        self.emit_assign(name, v, pc, target)

    def emit_assign(self, name, v, pc, target):
        # compound assignment detection
        if isinstance(v, Bin) and v.op in ('+', '-', '*', '/', '|', '&', '^', '<<', '>>') and self.same(v.a, target):
            self.emit(pc, '%s %s= %s;' % (name, v.op, self.ex(v.b, pc)))
            return
        if isinstance(v, Bin) and v.op in ('+', '*', '|', '&', '^') and self.same(v.b, target):
            self.emit(pc, '%s %s= %s;' % (name, v.op, self.ex(v.a, pc)))
            return
        self.emit(pc, '%s = %s;' % (name, self.ex(v, pc, 0, True)))

    def declare_pending(self, st, pc):
        # items pushed (not <local>) remaining at statement boundary are local declarations
        for k, x in enumerate(st['stk']):
            if not (isinstance(x, Raw) and x.t.startswith('<local')):
                off = -4 * (k + 1)
                s = self.localname(off, pc)
                name = s.name if s else '_loc%d' % -off
                tp = self.tagpref(s) if s else ''
                self.emit(pc, 'new %s%s = %s;' % (tp, name, self.ex(x, pc, 0, True)))
                st['stk'][k] = Raw('<local %s>' % name)


def header(d, f):
    a = d.a
    args = sorted([s for s in a.syms if s.vclass == 1 and s.addr >= 12 and s.cs == f.cs], key=lambda s: s.addr)
    parts = []
    for s in args:
        pre = '&' if s.ident == 2 else ''
        if s.ident == 4 or s.ident == 3:
            parts.append(d.tagpref(s) + s.name + '[]' * max(1, len(s.dims)))
        else:
            parts.append(pre + d.tagpref(s) + s.name)
    kind = 'public ' if f.addr in a.publics else ''
    tg = d.tagname.get(f.tag, '_')
    tp = tg + ':' if tg not in ('_', None) else ''
    return '%s%s%s(%s)' % (kind, tp, f.name.split('(')[0] if not f.name.startswith('operator') else f.name, ', '.join(parts))


if __name__ == '__main__':
    amx = amxload.Amx(sys.argv[1], sys.argv[2])
    d = Decomp(amx)
    only = sys.argv[3] if len(sys.argv) > 3 else None
    for f in d.funcs:
        if only and f.name != only:
            continue
        out = d.decompile(f)
        print('// %s:%d' % (d.fileof(f.addr), d.lineof(f.addr)))
        print(header(d, f))
        print('{')
        for pc, ln, t in out:
            print('  /*%d*/ %s' % (ln, t))
        print('}\n')
