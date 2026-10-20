"""Generate compilable Pawn source tree from lsrp.amx (+debug info)."""
import sys, os, re, struct, collections
import amxload, natives
from decomp import *
from decomp2 import Decomp2, CJ

INCDIRS = ['/home/claude/tools/omp-stdlib', '/home/claude/tools/inc']


class Gen(Decomp2):
    def __init__(self, amx):
        Decomp2.__init__(self, amx)
        self.protos = natives.parse_includes(INCDIRS)
        self.inc_forwards = natives.parse_forwards(INCDIRS)
        self.fparams = {}
        for f in self.funcs:
            args = sorted([s for s in amx.syms if s.vclass == 1 and s.addr >= 12 and s.cs == f.cs], key=lambda s: s.addr)
            self.fparams[f.name] = args
        self.fbyname = {f.name: f for f in self.funcs}
        self.enum_tags = {k: v for k, v in amx.dtags.items() if v.startswith('E_') and v not in (
            'E_ORM_ERROR', 'E_MYSQL_GLOBAL_OPTION', 'E_MYSQL_OPTION', 'E_MYSQL_FIELD_TYPE', 'E_MYSQL_EXECTIME_UNIT', 'E_LOGLEVEL')}
        self.uses = collections.defaultdict(lambda: collections.defaultdict(set))  # tag -> off -> kinds
        self.subsz = collections.defaultdict(lambda: collections.defaultdict(int))  # tag->off->size from bounds/const
        self.enum_size = {}
        for s in amx.syms:
            for t, sz in s.dims:
                if t in self.enum_tags and sz:
                    self.enum_size[t] = sz
        self.fields = None  # tag -> {off: (name, size, isarray)}
        self.maxargs = collections.defaultdict(int)
        self.callctx = None
        self.ref_defaults = {}
        self.pass2 = False

    # ---------------------------------------------------------------- address parsing with tags
    def addr_parse(self, e, pc):
        r = self.addr_parse_t(e, pc)
        if r is None:
            return None
        return (r[0], r[1], len(r[2]), r[3])

    def addr_parse_t(self, e, pc):
        # returns (base, [idx strings], [dimtags], state, extra)
        if isinstance(e, C):
            g = self.global_at(e.v)
            if g is None or not g.dims:
                return None
            if g.addr == e.v:
                return (g.name, [], [t for t, _ in g.dims], 'start', [s for _, s in g.dims])
            if len(g.dims) > 1 and e.v < g.addr + 4 * g.dims[0][1]:
                k = (e.v - g.addr) // 4
                t = g.dims[0][0]
                return (g.name, [self.fieldidx(t, str(k), k, None)], [t for t, _ in g.dims], 'ind', [z for _, z in g.dims])
            if len(g.dims) == 1:
                k = (e.v - g.addr) // 4
                t = g.dims[0][0]
                return (g.name, [self.idxname(t, k, None)], [t], 'elem', [g.dims[0][1]], ) if False else \
                       (g.name, [self.fieldidx(t, str(k), k, 'elem')], [t], 'elem', [g.dims[0][1]])
            return None
        if isinstance(e, LocAddr):
            s = self.localname(e.off, pc)
            if s is None:
                return ('_loc%d' % -e.off if e.off < 0 else '_arg%d' % ((e.off - 12) // 4), [], [0], 'start', [0])
            return (s.name, [], [t for t, _ in s.dims], 'start', [z for _, z in s.dims])
        if isinstance(e, Loc):
            s = self.localname(e.off, pc)
            if s is not None and s.ident in (3, 4):
                d = s.dims or [(0, 0)]
                return (s.name, [], [t for t, _ in d], 'start', [z for _, z in d])
            return None
        if isinstance(e, Bin) and e.op == '+':
            for x, y in ((e.a, e.b), (e.b, e.a)):
                if isinstance(x, Ld) and self.same(x.addr, y):
                    r = self.addr_parse_t(y, pc)
                    if r is not None and r[3] == 'ind':
                        return (r[0], r[1], r[2], 'start', r[4])
            for x, y in ((e.a, e.b), (e.b, e.a)):
                r = self.addr_parse_t(x, pc)
                if r is None or r[3] != 'start':
                    continue
                k = len(r[1])
                tag = r[2][k] if k < len(r[2]) else 0
                if isinstance(y, C):
                    dsz = r[4][k] if k < len(r[4]) else 0
                    if y.v % 4 or y.v < 0 or (dsz and y.v // 4 >= dsz):
                        continue
                idx = self.index_of_t(y, pc, tag)
                if idx is None:
                    continue
                st = 'ind' if k + 1 < len(r[2]) else 'elem'
                return (r[0], r[1] + [idx], r[2], st, r[4])
            for x, y in ((e.a, e.b), (e.b, e.a)):
                r = self.addr_parse_t(x, pc)
                if r is None or r[3] != 'elem':
                    continue
                if not r[1]:
                    continue
                tag = r[2][len(r[1]) - 1]
                if tag not in self.enum_tags:
                    continue
                sub = self.index_of_t(y, pc, 0, sub_of=(tag, r))
                if sub is None:
                    continue
                return (r[0], r[1] + [sub], r[2], 'elem2', r[4])
        return None

    def index_of_t(self, y, pc, tag, sub_of=None):
        if isinstance(y, C):
            if y.v % 4:
                return None
            k = y.v // 4
            if sub_of is not None:
                self.note_sub(sub_of, k + 1)
                return str(k)
            return self.fieldidx(tag, str(k), k, None)
        if isinstance(y, Bin) and y.op == '<<' and isinstance(y.b, C) and y.b.v == 2:
            b = getattr(y.a, 'bnd', None)
            if sub_of is not None and b is not None:
                self.note_sub(sub_of, b + 1)
            return self.ex(y.a, pc, 0)
        return None

    def note_sub(self, sub_of, size):
        tag, r = sub_of
        last = r[1][-1]
        off = self.cur_field_off.get(last)
        if off is None:
            return
        self.uses[tag][off].add('sub')
        if size > self.subsz[tag][off]:
            self.subsz[tag][off] = size

    def fieldidx(self, tag, s, k, kind):
        if tag in self.enum_tags:
            nm = self.fieldname(tag, k)
            self.cur_field_off[nm] = k
            return nm
        return s

    def fieldname(self, tag, k):
        if self.fields is not None and tag in self.fields:
            f = self.fields[tag].get(k)
            if f is not None:
                return f[0]
        base = self.enum_tags[tag][2:]
        return '%s_%d' % (base, k)

    # record use kind at lval resolution
    def lval(self, e, pc, elem=False):
        r = self.addr_parse_t(e, pc)
        if r is None:
            return None
        base, idx, tags, state, sizes = r
        if idx:
            t = tags[len(idx) - 1] if state != 'elem2' else tags[len(idx) - 2]
            if t in self.enum_tags:
                fld = idx[-1] if state != 'elem2' else idx[-2]
                off = self.cur_field_off.get(fld)
                if off is not None:
                    if state == 'elem2':
                        self.uses[t][off].add('sub')
                    elif elem:
                        self.uses[t][off].add('scalar')
                        if self.pass2 and self.fields and self.fields[t].get(off, (0, 1, False))[2]:
                            idx = idx + ['0']
                    else:
                        ctx = self.callctx
                        self.uses[t][off].add(('addr', ctx))
        if not elem and state in ('start', 'ind') and tags and len(idx) < len(tags) and len(idx) > 0:
            nt = tags[len(idx)]
            if self.callctx == 'ref' or (self.callctx == 'vararg' and nt in self.enum_tags):
                elem = True
        if elem and state in ('start', 'ind') and tags and len(idx) < len(tags):
            t = tags[len(idx)]
            if t in self.enum_tags:
                nm = self.fieldidx(t, '0', 0, None)
                self.uses[t][0].add('scalar')
                idx = idx + [nm]
                if self.pass2 and self.fields and self.fields[t].get(0, (0, 1, False))[2]:
                    idx = idx + ['0']
            else:
                idx = idx + ['0']
        s = base
        for i in idx:
            s += '[' + i + ']'
        return s

    # ---------------------------------------------------------------- calls / operators
    def step(self, st, i):
        pc, op, ar = self.insns[i]
        if self.pending is not None and self.pending is st['pri'] and op in (
                'load.s.pri', 'load.pri', 'const.pri', 'zero.pri', 'addr.pri', 'lref.s.pri', 'move.pri', 'pop.pri'):
            self.flush(pc)
        if op == 'bounds':
            P = st['pri']
            try:
                P.bnd = ar[0]
            except Exception:
                pass
            return None
        if op == 'movs' and self.step_movs_decl(st, i):
            return None
        if op in ('inc.s', 'dec.s', 'inc', 'dec'):
            d = '++' if op.startswith('inc') else '--'
            tgt = Loc(ar[0]) if op.endswith('.s') else Glob(ar[0])
            prev = self.insns[i - 1] if i > 0 else None
            lop = 'load.s.pri' if op.endswith('.s') else 'load.pri'
            nxt = self.insns[i + 1] if i + 1 < self.N else None
            if prev is not None and prev[1] == lop and prev[2][0] == ar[0]:
                self.flush(pc)
                e = Inc(tgt, d, False)
                st['pri'] = e
                self.pending = e
                e.pc = pc
                return None
            if nxt is not None and nxt[1] == lop and nxt[2][0] == ar[0]:
                self.flush(pc)
                e = Inc(tgt, d, True)
                st['pri'] = e
                self.pending = e
                e.pc = pc
                return i + 2
            self.flush(pc)
            self.emit(pc, self.ex(tgt, pc) + d + ';')
            return None
        if op in ('inc.pri', 'dec.pri') and isinstance(st['pri'], Ref):
            nxt = self.insns[i + 1] if i + 1 < self.N else None
            P = st['pri']
            if nxt is not None and nxt[1] == 'sref.s.pri' and nxt[2][0] == P.off:
                d = '++' if op == 'inc.pri' else '--'
                n2 = self.insns[i + 2] if i + 2 < self.N else None
                self.flush(pc)
                if n2 is not None and n2[1] == 'pop.pri' and st['stk'] and isinstance(st['stk'][-1], Ref) and st['stk'][-1].off == P.off:
                    self.pop(st)
                    e = Inc(P, d, False); st['pri'] = e; self.pending = e; e.pc = pc
                    return i + 3
                e = Inc(P, d, True); st['pri'] = e; self.pending = e; e.pc = pc
                return i + 2
        if op in ('inc.i', 'dec.i'):
            d = '++' if op == 'inc.i' else '--'
            nxt = self.insns[i + 1] if i + 1 < self.N else None
            P = st['pri']
            if nxt is not None and nxt[1] == 'pop.pri' and st['stk'] and isinstance(st['stk'][-1], Ld):
                top = self.pop(st)
                self.flush(pc)
                e = Inc(Ld(P), d, False)
                st['pri'] = e; self.pending = e; e.pc = pc
                return i + 2
            if nxt is not None and nxt[1] == 'load.i':
                self.flush(pc)
                e = Inc(Ld(P), d, True)
                st['pri'] = e; self.pending = e; e.pc = pc
                return i + 2
            self.flush(pc)
            self.emit(pc, self.ex(Ld(P), pc) + d + ';')
            return None
        if op in ('call', 'sysreq.c'):
            opform = (i >= 3 and self.insns[i - 1][1] == 'push.c' and self.insns[i - 1][2][0] == 8 and
                      self.insns[i - 2][1] == 'push.alt' and self.insns[i - 3][1] == 'push.pri')
            r = Decomp2.step(self, st, i)
            e = st['pri']
            if isinstance(e, Call):
                e.opform = opform
                e.pc = pc
                if not e.native:
                    self.maxargs[e.name] = max(self.maxargs[e.name], len(e.args))
            return r
        return Decomp2.step(self, st, i)

    def dimstr(self, s):
        out = ''
        for t, sz in s.dims:
            tn = self.tagname.get(t, '_')
            out += '[%s]' % (tn if t in self.enum_tags else (sz if sz else ''))
        return out

    def data_init(self, addr, dims, tags=None):
        if self._builder is not None:
            return self._builder.nd_init(addr, dims, tags or [0] * len(dims))
        a = self.a
        if len(dims) == 1:
            n = dims[0]
            cells = [a.dcell(addr + 4 * k) for k in range(n)]
            return self.cells_init(cells, tags[0] if tags else 0)
        n = dims[0]
        rows = []
        for r in range(n):
            ind = a.dcell(addr + 4 * r)
            start = addr + 4 * r + ind
            if dims[1]:
                cells = [a.dcell(start + 4 * k) for k in range(dims[1])]
            else:
                cells = []
                p = start
                while True:
                    c = a.dcell(p)
                    cells.append(c)
                    p += 4
                    if c == 0:
                        break
            rows.append(self.cells_init(cells, tags[1] if tags else 0))
        return '{' + ', '.join(rows) + '}'

    def cells_init(self, cells, tag=0):
        if self._builder is not None:
            return self._builder.row_init(cells, tag)
        return '{' + ', '.join(fmt_const(c) for c in cells) + '}'

    _builder = None

    def step_movs_decl(self, st, i):
        pc, op, ar = self.insns[i]
        ld = self.last_decl
        if ld is None or not isinstance(st['alt'], LocAddr) or ld[1] != st['alt'].off or not isinstance(st['pri'], C):
            return False
        sym = self.localname(ld[1], pc)
        if sym is None or not sym.dims:
            return False
        dims = [z for _, z in sym.dims]
        tags = [t for t, _ in sym.dims]
        if len(dims) > 1 and ar[0] == 4 * dims[0]:
            self.last_decl = None
            return True
        if len(dims) == 1:
            n = ar[0] // 4
            cells = [self.a.dcell(st['pri'].v + 4 * k) for k in range(n)]
            init = self.cells_init(cells + [0] * (dims[0] - n) if dims[0] > n else cells, tags[0])
        elif len(dims) == 2 and dims[1] == 0:
            base = st['pri'].v
            ncell = ar[0] // 4
            starts = [k + self.a.dcell(base + 4 * k) // 4 for k in range(dims[0])]
            rows = []
            for k in range(dims[0]):
                end = starts[k + 1] if k + 1 < dims[0] else ncell
                cells = [self.a.dcell(base + 4 * j) for j in range(starts[k], end)]
                if 0 in cells:
                    cells = cells[:cells.index(0)]
                rows.append(self.str_lit(bytes(c & 255 for c in cells).decode('utf-8', 'replace')) if all(0 < c < 256 for c in cells) else '{' + ', '.join(fmt_const(c) for c in cells + [0]) + '}')
            init = '{' + ', '.join(rows) + '}'
        else:
            init = self.data_init(st['pri'].v, dims, tags)
        t = self.out[ld[0]]
        self.out[ld[0]] = (t[0], t[1], t[2], t[3].rstrip(';') + ' = ' + init + ';')
        self.last_decl = None
        return True

    def ex(self, e, pc, prec=0, asarg=False):
        if isinstance(e, Heap):
            if e.val is None:
                return '0'
            sv = self.callctx
            self.callctx = 'value'
            r = self.ex(e.val, pc, prec, True)
            self.callctx = sv
            return r
        if isinstance(e, C) and asarg:
            v = e.v
            kind = self.callctx
            if kind == 'value':
                return fmt_const(v)
            if kind == 'int':
                if v == -2147483648:
                    return 'cellmin'
                return '0x%08X' % (v & 0xffffffff) if abs(v) >= 0x01000000 else str(v)
            g = self.global_at(v) if v > 0 else None
            if kind in ('ref', 'vararg'):
                if g is not None:
                    if g.addr == v:
                        return g.name
                    lv = self.lval(e, pc, True)
                    if lv:
                        return lv
                s = self.literal(v)
                if s is not None:
                    return self.str_lit(s)
                return fmt_const(v)
            s = self.literal(v)
            if s is not None and (v > 64 or kind == 'array'):
                return self.str_lit(s)
            if g is not None and g.dims and g.addr == v:
                return g.name
            return fmt_const(v)
        if isinstance(e, C) and not asarg:
            v = e.v
            if v == -2147483648:
                return 'cellmin'
            if abs(v) >= 0x01000000:
                return '0x%08X' % (v & 0xffffffff)
            return str(v)
        if isinstance(e, Call):
            return self.ex_call(e, pc, prec)
        if isinstance(e, Inc):
            t = self.ex(e.t, pc, 14)
            return (e.d + t) if e.pre else (t + e.d)
        if isinstance(e, Bin) and e.op in ('&&', '||'):
            p = PREC[e.op]
            def side(x, pp):
                t = self.ex(x, pc, pp)
                if getattr(x, 'merged', False) and isinstance(x, Bin) and x.op in ('&&', '||') and not t.startswith('('):
                    t = '(' + t + ')'
                return t
            s_ = '%s %s %s' % (side(e.a, p), e.op, side(e.b, p + 1))
            return '(' + s_ + ')' if p < prec else s_
        if isinstance(e, Bin) and e.op not in ('&&', '||'):
            lv = self.lval(e, pc) if e.op == '+' else None
            if lv is not None:
                return lv
            p = PREC.get(e.op, 1)
            a = self.intwrap(e.a, pc, p)
            b = self.intwrap(e.b, pc, p + 1)
            s_ = '%s %s %s' % (a, e.op, b)
            return '(' + s_ + ')' if p < prec else s_
        return Decomp2.ex(self, e, pc, prec, asarg)

    # ---------------------------------------------------------------- Float tag handling
    def tag_of_expr(self, e, pc):
        if isinstance(e, Loc) or isinstance(e, Ref):
            s_ = self.localname(e.off, pc)
            return self.tagname.get(s_.tag) if s_ is not None else None
        if isinstance(e, Glob):
            g_ = self.global_at(e.a)
            return self.tagname.get(g_.tag) if g_ is not None and not g_.dims else None
        if isinstance(e, Ld):
            r = self.addr_parse_t(e.addr, pc)
            if r is not None and r[3] in ('elem', 'start') :
                # element of array variable: use variable tag
                base = r[0]
                return self.sym_tag(base, pc)
            return None
        if isinstance(e, Call):
            if e.native:
                if getattr(e, 'opform', False) and e.name in FLOATOPS:
                    return 'Float'
                return natives.RTAGS.get(e.name)
            if e.name.startswith('operator'):
                f_ = [x for x in self.funcs if x.name == e.name]
                return self.tagname.get(f_[0].tag) if f_ else None
            f_ = self.fbyname.get(e.name)
            return self.tagname.get(f_.tag) if f_ is not None else None
        if isinstance(e, Tern):
            ta = self.tag_of_expr(e.a, pc); tb = self.tag_of_expr(e.b, pc)
            return ta if ta == tb else None
        if isinstance(e, Heap) and e.val is not None:
            return self.tag_of_expr(e.val, pc)
        return None

    def sym_tag(self, name, pc):
        for s_ in self.cur_locals:
            if s_.name == name and s_.cs <= pc <= s_.ce + 8:
                return self.tagname.get(s_.tag)
        for s_ in self.glob:
            if s_.name == name:
                return self.tagname.get(s_.tag)
        return None

    def fwrap(self, e, pc):
        if isinstance(e, Heap) and e.val is not None:
            e = e.val
        if isinstance(e, C):
            if e.v == 0:
                return '0.0'
            nf = nice_float(e.v)
            if nf is not None:
                return nf if ('.' in nf) else nf + '.0'
            return 'Float:0x%08X' % (e.v & 0xffffffff)
        sv = self.callctx
        self.callctx = 'value'
        s_ = self.ex(e, pc, 14, True)
        self.callctx = sv
        if self.tag_of_expr(e, pc) == 'Float':
            return s_
        return 'Float:%s' % s_ if re.match(r'^[\w\[\]\. ]+$', s_) and '(' not in s_ else 'Float:(%s)' % s_

    def intwrap(self, e, pc, prec):
        s_ = self.ex(e, pc, prec)
        if self.tag_of_expr(e, pc) == 'Float':
            return '_:%s' % s_ if re.match(r'^[\w\[\]]+$', s_) else '_:(%s)' % s_
        return s_

    def ex_call(self, e, pc, prec):
        name = e.name
        if name.startswith('operator') and '(' in name:
            o = name[8:name.index('(')]
            tags = name[name.index('(') + 1:-1].split(',')
            if len(e.args) == 2:
                p = PREC.get(o, 9)
                a0 = self.tagged(e.args[0], tags[0], pc)
                a1 = self.tagged(e.args[1], tags[1], pc)
                s = '%s %s %s' % (a0, o, a1)
                return '(' + s + ')'
        if e.native and getattr(e, 'opform', False) and name in FLOATOPS and len(e.args) == 2:
            o = FLOATOPS[name]
            s = '%s %s %s' % (self.tagged(e.args[0], 'Float:', pc), o, self.tagged(e.args[1], 'Float:', pc))
            return '(' + s + ')'
        kinds = self.protos.get(name) if e.native else None
        params = None if e.native else self.fparams.get(name)
        out = []
        for k, x in enumerate(e.args):
            kind = None
            if kinds is not None:
                kind = kinds[k] if k < len(kinds) else (kinds[-1] if kinds and kinds[-1] == 'vararg' else None)
            elif params is not None:
                if k < len(params):
                    kind = {2: 'ref', 3: 'array', 4: 'array'}.get(params[k].ident, 'value')
                else:
                    kind = 'vararg'
            self.callctx = kind
            ptag = None
            if e.native:
                pt = natives.PTAGS.get(name)
                if pt is not None and k < len(pt):
                    ptag = pt[k]
            elif params is not None and k < len(params):
                ptag = self.tagname.get(params[k].tag)
                if params[k].ident != 1:
                    ptag = None
            if ptag == 'Float' and kind == 'value':
                out.append((self.fwrap(x, pc), kind, x))
            else:
                out.append((self.ex(x, pc, 0, True), kind, x))
            self.callctx = None
        # drop trailing default reference args (heap temporaries)
        while out and out[-1][1] == 'ref' and isinstance(out[-1][2], Heap):
            if not e.native:
                self.ref_defaults[(name, len(out) - 1)] = out[-1][0]
            out.pop()
        return '%s(%s)' % (name, ', '.join(o[0] for o in out))

    def flush(self, pc):
        if self.pending is not None:
            e = self.pending
            self.pending = None
            p = getattr(e, 'pc', pc)
            self.emit(p, self.ex(e, p) + ';')

    def retstmt(self, st):
        P = st['pri']
        tg = self.tagname.get(self.f.tag, '_')
        sv = self.callctx
        self.callctx = 'value' if tg == 'Float' else 'int'
        r = 'return %s;' % self.ex(P, getattr(P, 'pc', self.ret_pc), 0, True)
        self.callctx = sv
        return r

    def store_local(self, st, off, v, pc):
        s_ = self.localname(off, pc)
        ld = self.last_decl
        if s_ is not None and self.tagname.get(s_.tag) == 'Float' and ld is not None and ld[1] == off and ld[0] == len(self.out) - 1:
            t = self.out[ld[0]]
            self.out[ld[0]] = (t[0], t[1], t[2], t[3].rstrip(';') + ('' if (isinstance(v, C) and v.v == 0) else ' = ' + self.fwrap(v, pc)) + ';')
            self.last_decl = None
            return
        sv = self.callctx
        self.callctx = 'value' if (s_ is not None and self.tagname.get(s_.tag) == 'Float') else 'int'
        Decomp2.store_local(self, st, off, v, pc)
        self.callctx = sv

    def emit_assign(self, name, v, pc, target):
        ls = getattr(self, 'last_store', None)
        if ls is not None and ls[1] is v and ls[0] == len(self.out) - 1 and ls[2] == self.stmt_out_mark and not isinstance(v, (C,)) or \
           (ls is not None and ls[1] is v and ls[0] == len(self.out) - 1 and ls[2] == self.stmt_out_mark):
            t = self.out[ls[0]]
            self.out[ls[0]] = (t[0], t[1], t[2], '%s = %s' % (name, t[3]))
            self.last_store = (ls[0], v, self.stmt_out_mark)
            return
        self._emit_assign(name, v, pc, target)
        self.last_store = (len(self.out) - 1, v, self.stmt_out_mark)

    def _emit_assign(self, name, v, pc, target):
        sv = self.callctx
        fl = False
        if isinstance(target, (Loc, Ref)):
            s_ = self.localname(target.off, pc)
            fl = s_ is not None and self.tagname.get(s_.tag) == 'Float'
        elif isinstance(target, Ld):
            fl = self.tag_of_expr(target, pc) == 'Float'
        elif isinstance(target, Glob):
            g_ = self.global_at(target.a)
            fl = g_ is not None and self.tagname.get(g_.tag) == 'Float'
        self.callctx = 'value' if fl else 'int'
        if fl and isinstance(v, Call) and len(v.args) == 2 and self.same(v.args[0], target) and (
                (v.native and getattr(v, 'opform', False) and v.name in FLOATOPS) or v.name.startswith('operator')):
            o = FLOATOPS[v.name] if v.name in FLOATOPS else v.name[8:v.name.index('(')]
            rt = v.name[v.name.index('(') + 1:-1].split(',')[1] if v.name.startswith('operator') else 'Float:'
            self.emit(pc, '%s %s= %s;' % (name, o, self.fwrap(v.args[1], pc) if rt == 'Float:' else self.tagged(v.args[1], rt, pc)))
            self.callctx = sv
            return
        if fl and not (isinstance(v, Bin) and self.same(v.a, target)):
            self.emit(pc, '%s = %s;' % (name, self.fwrap(v, pc)))
        else:
            Decomp2.emit_assign(self, name, v, pc, target)
        self.callctx = sv

    def tagged(self, x, tag, pc):
        s = self.ex(x, pc, 14, True)
        if tag in ('_:', '_'):
            return '_:(%s)' % s if not re.match(r'^-?\d+$', s) else s
        if re.match(r'^-?\d+\.\d+$', s):
            return s
        return '%s(%s)' % (tag if tag.endswith(':') else tag + ':', s)


def gsize(s):
    if not s.dims:
        return 4
    m = 1; t = 0
    for _, sz in s.dims:
        m *= sz; t += m
    return t * 4


class Builder:
    def __init__(self, amxpath, imgpath, outdir):
        self.amx = amxload.Amx(amxpath, imgpath)
        self.g = Gen(self.amx)
        self.g._builder = self
        self.out = outdir

    def decompile_all(self):
        g = self.g
        self.bodies = {}
        for f in g.funcs:
            if not g.fileof(f.addr).startswith('gamemodes/'):
                continue
            g.cur_field_off = {}
            self.bodies[f.name] = g.decompile(f)

    def infer_enums(self):
        g = self.g
        fields = {}
        for tag, name in g.enum_tags.items():
            total = g.enum_size.get(tag)
            if total is None:
                continue
            used = sorted(o for o in g.uses[tag].keys() if 0 <= o < total)
            fl = {}
            for n, o in enumerate(used):
                nxt = used[n + 1] if n + 1 < len(used) else total
                gap = nxt - o
                kinds = g.uses[tag][o]
                isarr = 'sub' in kinds or g.subsz[tag][o] > 1
                for k in kinds:
                    if isinstance(k, tuple) and k[1] == 'array':
                        isarr = True
                if not isarr and 'scalar' not in kinds and gap > 1:
                    if not any(isinstance(k, tuple) and k[1] == 'ref' for k in kinds):
                        isarr = True
                size = gap if isarr else 1
                if isarr and g.subsz[tag][o] and g.subsz[tag][o] < gap:
                    size = max(g.subsz[tag][o], 1)
                fl[o] = (g.fieldname(tag, o), size, isarr)
            fields[tag] = fl
        g.fields = fields
        return fields

    def enum_text(self):
        g = self.g
        lines = []
        for tag in sorted(g.fields):
            name = g.enum_tags[tag]
            total = g.enum_size[tag]
            fl = g.fields[tag]
            lines.append('enum %s' % name)
            lines.append('{')
            items = []
            o = 0
            for off in sorted(fl):
                if off > o:
                    items.append('\t_%s_pad%d[%d]' % (name[2:], o, off - o))
                nm, size, isarr = fl[off]
                items.append('\t%s%s' % (nm, '[%d]' % size if isarr else ''))
                o = off + size
            if o < total:
                items.append('\t_%s_pad%d[%d]' % (name[2:], o, total - o))
            lines.append(',\n'.join(items))
            lines.append('}')
            lines.append('')
        return '\n'.join(lines)

    # ---------------------------------------------------------------- globals
    def ginit(self, s):
        g = self.g
        a = self.amx
        base = s.addr
        if not s.dims:
            v = a.dcell(base)
            return '' if v == 0 else ' = %s' % fmt_const(v)
        dims = [sz for _, sz in s.dims]
        tags = [t for t, _ in s.dims]
        if len(dims) == 1:
            cells = [a.dcell(base + 4 * k) for k in range(dims[0])]
            if not any(cells):
                return ''
            return ' = ' + self.row_init(cells, tags[0])
        if not any(a.dcell(base + 4 * k) for k in range(gsize(s) // 4)):
            return ''
        return ' = ' + self.nd_init(base, dims, tags, multiline=True)

    def nd_init(self, addr, dims, tags, multiline=False):
        a = self.amx
        if len(dims) == 1:
            if dims[0]:
                cells = [a.dcell(addr + 4 * k) for k in range(dims[0])]
            else:
                cells = []
                p = addr
                while True:
                    c = a.dcell(p); cells.append(c); p += 4
                    if c == 0:
                        break
            return self.row_init(cells, tags[0])
        parts = []
        for r in range(dims[0]):
            ind = a.dcell(addr + 4 * r)
            parts.append(self.nd_init(addr + 4 * r + ind, dims[1:], tags[1:]))
        if multiline:
            return '{\n' + ',\n'.join('\t' + p for p in parts) + '\n}'
        return '{' + ', '.join(parts) + '}'

    def row_init(self, cells, tag):
        g = self.g
        if tag in g.enum_tags and g.fields and tag in g.fields:
            parts = []
            o = 0
            fl = g.fields[tag]
            total = len(cells)
            items = []
            for off in sorted(fl):
                if off > o:
                    items.append(('pad', o, off - o))
                nm, size, isarr = fl[off]
                items.append((nm, off, size if isarr else 0))
                o = off + (size if isarr else 1)
            if o < total:
                items.append(('pad', o, total - o))
            for nm, off, size in items:
                if nm == 'pad' or size:
                    sz = size
                    parts.append(self.arr_init(cells[off:off + sz]))
                else:
                    parts.append(fmt_const(cells[off]))
            return '{' + ', '.join(parts) + '}'
        return self.arr_init(cells)

    def arr_init(self, cells):
        # string?
        nz = len(cells)
        while nz > 0 and cells[nz - 1] == 0:
            nz -= 1
        if nz == 0:
            return '{0}'
        body = cells[:nz]
        if all(32 <= c < 256 or c in (9, 10, 13) for c in body) and nz < len(cells):
            try:
                s = bytes(body).decode('utf-8')
                return self.g.str_lit(s)
            except UnicodeDecodeError:
                pass
        return '{' + ', '.join(fmt_const(c) for c in body) + '}'

    def gdecl(self, s, static=False):
        g = self.g
        tp = g.tagpref(s)
        dims = ''
        for t, sz in s.dims:
            tn = g.tagname.get(t, '_')
            dims += '[%s]' % (tn if tn in g.enum_tags.values() else (sz if sz else ''))
        return '%s %s%s%s%s;' % ('static' if static else 'new', tp, s.name, dims, self.ginit(s))

    # ---------------------------------------------------------------- headers
    def header(self, f):
        g = self.g
        args = g.fparams[f.name]
        parts = []
        for s in args:
            if s.ident in (3, 4):
                dd = ''
                for t, sz in (s.dims or [(0, 0)]):
                    tn = g.tagname.get(t, '_')
                    dd += '[%s]' % (tn if tn in g.enum_tags.values() else '')
                parts.append(g.tagpref(s) + s.name + dd)
            elif s.ident == 2:
                parts.append('&' + g.tagpref(s) + s.name)
            else:
                parts.append(g.tagpref(s) + s.name)
        if g.maxargs.get(f.name, 0) > len(args):
            parts.append('{Float,_}:...')
        tg = g.tagname.get(f.tag, '_')
        tp = tg + ':' if tg not in ('_', None) else ''
        pub = f.addr in self.amx.publics
        if pub and f.name in g.inc_forwards and len(g.inc_forwards[f.name]) == len(args):
            parts = [natives.rename_param(p, s.name) for p, s in zip(g.inc_forwards[f.name], args)]
            return pub, tp, f.name, ', '.join(parts), True
        # default values for reference params (from call sites)
        for k, s in enumerate(args):
            if s.ident == 2 and (f.name, k) in g.ref_defaults:
                parts[k] = parts[k] + ' = ' + g.ref_defaults[(f.name, k)]
        return pub, tp, f.name, ', '.join(parts), False

    # ---------------------------------------------------------------- emit
    def build(self):
        g = self.g
        amx = self.amx
        self.decompile_all()
        self.infer_enums()
        g.pass2 = True
        g.uses = collections.defaultdict(lambda: collections.defaultdict(set))
        self.decompile_all()
        files = collections.OrderedDict()
        for ad, n in sorted(amx.files):
            if n.startswith('gamemodes/'):
                files[n] = []
        # globals
        for s in g.glob:
            if s.vclass != 0 or s.name.startswith('__'):
                continue
            fn = g.fileof(s.cs)
            if fn not in files:
                fn = 'gamemodes/modules/core/player_data.inc' if s.cs < 0x14fc else list(files)[0]
            files[fn].append((s.cs, 0, 'g', s))
        for f in g.funcs:
            fn = g.fileof(f.addr)
            if fn in files:
                files[fn].append((f.addr, 1, 'f', f))
        forwards = []
        texts = {}
        for fn, items in files.items():
            items.sort(key=lambda x: (x[0], x[1]))
            out = []
            for _, _, kind, obj in items:
                if kind == 'g':
                    out.append(self.gdecl(obj))
                else:
                    pub, tp, name, params, incfw = self.header(obj)
                    if pub and not incfw:
                        forwards.append('forward %s%s(%s);' % (tp, name, params))
                    out.append('')
                    out.append('%s%s%s(%s)' % ('public ' if pub else '', tp, name, params))
                    out.append('{')
                    # static locals (one declaration statement)
                    sts = [s for s in g.glob if s.vclass == 2 and obj.cs <= s.cs < obj.ce]
                    sts.sort(key=lambda x: x.addr)
                    if sts:
                        out.append('\tstatic ' + ', '.join(self.gdecl(s, True)[len('static '):-1] for s in sts) + ';')
                    body = self.bodies.get(name, [])
                    for pc, ln, ind, t in body:
                        if t.startswith('L_') and t.endswith(':'):
                            out.append('%s%s' % ('\t' * max(ind - 1, 0), t))
                        else:
                            out.append('%s%s' % ('\t' * ind, t))
                    out.append('}')
            texts[fn] = '\n'.join(out) + '\n'
        return files, texts, forwards


if __name__ == '__main__':
    b = Builder(sys.argv[1], sys.argv[2], sys.argv[3])
    files, texts, forwards = b.build()
    od = sys.argv[3]
    for fn, t in texts.items():
        p = os.path.join(od, fn)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        open(p, 'w').write(t)
    open(os.path.join(od, 'enums.txt'), 'w').write(b.enum_text())
    open(os.path.join(od, 'forwards.txt'), 'w').write('\n'.join(forwards) + '\n')
    open(os.path.join(od, 'order.txt'), 'w').write('\n'.join(files.keys()) + '\n')
    print('ok', len(texts))
