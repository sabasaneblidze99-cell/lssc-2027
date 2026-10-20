"""Structured decompiler on top of decomp.Decomp: recovers if/else, while, do, for, switch,
break/continue following pawncc 3.10 -d3 (unoptimised) code generation patterns."""
import sys, collections
import amxload
from decomp import *

CJ = ('jzer', 'jnz')


class Decomp2(Decomp):
    # ------------------------------------------------------------------ setup
    def decompile(self, f):
        a = self.a
        self.f = f
        self.cur_locals = [s for s in a.syms if s.vclass == 1 and f.cs <= s.cs <= f.ce]
        insns = list(a.disasm(f.cs, f.ce))
        self.insns = insns
        self.idx = {ad: i for i, (ad, _, _) in enumerate(insns)}
        self.N = len(insns)
        # merges: AND form  const.pri 1 ; jump E ; zero.pri ; E
        #         OR  form  zero.pri ; jump E ; const.pri 1 ; E
        self.merges = {}
        for i in range(len(insns) - 2):
            o0, o1, o2 = insns[i], insns[i + 1], insns[i + 2]
            if o1[1] == 'jump' and o1[2][0] == o2[0] + (8 if o2[1] == 'const.pri' else 4):
                if o0[1] == 'const.pri' and o0[2] == [1] and o2[1] == 'zero.pri':
                    self.merges[o0[0]] = (o0[0], o0[0], o2[0], o1[2][0])  # first, T, F, end
                elif o0[1] == 'zero.pri' and o2[1] == 'const.pri' and o2[2] == [1]:
                    self.merges[o0[0]] = (o0[0], o2[0], o0[0], o1[2][0])
        self.mT = {m[1]: m for m in self.merges.values()}
        self.mF = {m[2]: m for m in self.merges.values()}
        self.sc_jumps = {}   # insn index -> merge
        for k, (ad, op, ar) in enumerate(insns):
            if op in CJ:
                m = self.find_merge2(k, ar[0])
                if m is not None:
                    self.sc_jumps[k] = m
        self.jtargets = set()
        for ad, op, ar in insns:
            if op in ('jump', 'jzer', 'jnz', 'switch'):
                self.jtargets.add(ar[0])
            if op == 'casetbl':
                self.jtargets.add(ar[1])
                for v, t in ar[2:]:
                    self.jtargets.add(t)
        # backward jumps: target -> list of source indices
        self.back = collections.defaultdict(list)
        for k, (ad, op, ar) in enumerate(insns):
            if op in ('jump',) + CJ and ar[0] <= ad:
                self.back[ar[0]].append(k)
        self.out = []
        self.ind = 1
        self.stmt_line = self.lineof(f.cs)
        self.heapn = 0
        self.pending = None
        self.sc = collections.defaultdict(list)
        self.tern = []
        self.internal = set()
        self.stmt_out_mark = 0
        self.labels_needed = set()
        self.inloop = set()
        self.tern_pts = set()
        self.decl = set()
        self.last_decl = None
        st = {'pri': C(0), 'alt': C(0), 'stk': [], 'sp': 0}
        self.walk(st, 1 if insns and insns[0][1] == 'proc' else 0, self.N, None)
        self.flush(0)
        # strip trailing implicit 'return 0;' handled by caller
        return self.out

    def find_merge2(self, k, t):
        pc = self.insns[k][0]
        nb = self.next_break(k)
        for m in sorted(self.merges.values()):
            if m[0] <= pc or m[0] > nb:
                continue
            if t in (m[1], m[2]):
                return m
            if pc < t <= m[0]:
                kk = self.idx.get(t)
                if kk is not None and self.insns[kk - 1][1] == 'jump':
                    return None
                return m
        return None

    def expr_target(self, addr, upto):
        for k, (ad, op, ar) in enumerate(self.insns):
            if op in ('jump', 'jzer', 'jnz') and ar[0] == addr and k < upto:
                if not any(self.insns[j][1] == 'break' for j in range(k + 1, upto)):
                    return True
        return False

    def emit(self, pc, text):
        ln = self.stmt_line
        if text.startswith('new ') and self.out and self.out[-1][3].startswith('new ') and len(self.out) - 1 >= self.stmt_out_mark \
                and self.out[-1][2] == self.ind and getattr(self, 'decl_merge_ok', True):
            t = self.out[-1]
            self.out[-1] = (t[0], t[1], t[2], t[3].rstrip(';') + ', ' + text[4:])
            if self.last_decl is not None:
                self.last_decl = (len(self.out) - 1, self.last_decl[1])
            return
        self.out.append((pc, ln, self.ind, text))
        if not text.startswith('new '):
            self.last_decl = None

    def store_local(self, st, off, v, pc):
        ld = self.last_decl
        if ld is not None and ld[1] == off and ld[0] == len(self.out) - 1:
            s = self.localname(off, pc)
            t = self.out[ld[0]]
            self.out[ld[0]] = (t[0], t[1], t[2], t[3].rstrip(';') + ' = ' + self.ex(v, pc, 0, True) + ';')
            self.last_decl = None
            return
        Decomp.store_local(self, st, off, v, pc)

    def step(self, st, i):
        pc, op, ar = self.insns[i]
        if op == 'stack' and ar[0] < 0:
            r = Decomp.step(self, st, i)
            self.last_decl = (len(self.out) - 1, st['sp'])
            return r
        if op == 'movs' and isinstance(st['alt'], LocAddr) and self.last_decl is not None and self.last_decl[1] == st['alt'].off \
                and isinstance(st['pri'], C) and self.literal(st['pri'].v, ar[0] // 4) is not None:
            ld = self.last_decl
            t = self.out[ld[0]]
            self.out[ld[0]] = (t[0], t[1], t[2], t[3].rstrip(';') + ' = ' + self.str_lit(self.literal(st['pri'].v, ar[0] // 4)) + ';')
            self.last_decl = None
            return None
        if op == 'fill' and isinstance(st['alt'], LocAddr) and isinstance(st['pri'], C) and st['pri'].v == 0:
            ld = self.last_decl
            return None
        return Decomp.step(self, st, i)

    # ------------------------------------------------------------------ walker
    def walk(self, st, i0, i1, ctx):
        """ctx: dict(exit=addr, cont=addr) of innermost loop or None"""
        insns = self.insns
        i = i0
        while i < i1:
            pc, op, ar = insns[i]
            # short-circuit merge reached
            if pc in self.merges:
                m = self.merges[pc]
                nodes = self.sc.pop(m[0], [])
                if nodes:
                    st['pri'] = self.reduce2(nodes, m, pc)
                    i = self.idx[m[3]]
                    continue
            # loop head?
            if pc in self.back and op == 'break' and pc not in self.inloop:
                bl = [b for b in self.back[pc] if i < b < i1 and insns[b][1] == 'jump']
                if bl:
                    r = self.loop(st, i, max(bl), ctx)
                    if r is not None:
                        i = r
                        continue
            if pc in self.labels_needed:
                self.flush(pc)
                self.emit(pc, 'L_%x:' % pc)
            if op == 'break':
                self.flush(pc)
                self.declare_pending(st, pc)
                self.stmt_line = self.lineof(pc)
                self.stmt_out_mark = len(self.out)
                self.stmt_stk = (list(st['stk']), st['sp'])
                i += 1
                continue
            if op in CJ and i in self.sc_jumps:
                m = self.sc_jumps[i]
                nodes = self.sc[m[0]]
                start = nodes[-1][4] if nodes else pc
                nodes.append((start, st['pri'], op, ar[0], insns[i + 1][0]))
                self.pending = None
                i += 1
                continue
            if op in CJ:
                t = ar[0]
                nb = self.next_break(i)
                if pc < t < nb and t not in self.mF and t not in self.mT:
                    i = self.ternary2(st, i, t, ctx)
                    continue
                i = self.ifstmt(st, i, i1, ctx)
                continue
            if op == 'jump':
                i = self.jumpstmt(st, i, i1, ctx)
                continue
            if op == 'switch':
                i = self.switchstmt(st, i, i1, ctx)
                continue
            if op == 'retn':
                if self.pending is st['pri']:
                    self.pending = None
                self.flush(pc)
                self.ret_pc = pc
                za = insns[i - 1][0] if i >= 1 else -1
                if i == self.N - 1 and i >= 2 and insns[i - 1][1] == 'zero.pri' and (insns[i - 2][1] != 'break' or (i >= 3 and insns[i - 3][1] == 'proc')) \
                        and za not in self.mF and za not in self.merges and za not in self.tern_pts:
                    i += 1
                    continue
                if self.pending is st['pri']:
                    self.pending = None
                self.emit(pc, self.retstmt(st))
                i += 1
                continue
            r = self.step(st, i)
            i = i + 1 if r is None else r
        return st

    def retstmt(self, st):
        P = st['pri']
        return 'return %s;' % self.ex(P, self.insns[0][0] if False else self.cur_pc(), 0, True)

    def cur_pc(self):
        return self.out[-1][0] if self.out else 0

    # ------------------------------------------------------------------ ternary
    def ternary2(self, st, i, t, ctx):
        pc, op, ar = self.insns[i]
        cond = st['pri'] if op == 'jzer' else Un('!', st['pri'])
        jidx = self.idx[t] - 1
        if self.insns[jidx][1] != 'jump':
            self.emit(pc, '/*TERNARY? %x*/' % pc)
            return i + 1
        end = self.insns[jidx][2][0]
        self.tern_pts.add(t); self.tern_pts.add(end)
        saved = (list(st['stk']), st['sp'])
        self.pending = None
        self.walk_expr(st, i + 1, jidx)
        a = st['pri']
        self.pending = None
        st['stk'] = list(saved[0]); st['sp'] = saved[1]
        self.walk_expr(st, self.idx[t], self.idx[end])
        b = st['pri']
        st['pri'] = Tern(cond, a, b)
        self.pending = None
        return self.idx[end]

    def walk_expr(self, st, i0, i1):
        # expression-only walk (no statements expected)
        self.walk(st, i0, i1, None)

    # ------------------------------------------------------------------ if / else
    def cond_text(self, op, P, pc):
        if op == 'jzer':
            return self.ex(P, pc)
        return self.ex(Un('!', P), pc)

    def ifstmt(self, st, i, i1, ctx):
        pc, op, ar = self.insns[i]
        t = ar[0]
        P = st['pri']
        self.pending = None
        ti = self.idx.get(t)
        if ti is None or t <= pc or ti > i1:
            # do-while tail or weird: emit goto
            self.flush(pc)
            self.emit(pc, 'if (%s) goto L_%x;' % (self.ex(Un('!', P) if op == 'jzer' else P, pc), t))
            self.labels_needed.add(t)
            return i + 1
        cond = self.cond_text(op, P, pc)
        saved = (list(st['stk']), st['sp'])
        # else part?
        prev = self.insns[ti - 1]
        has_else = False
        if prev[1] == 'jump' and ti - 1 > i:
            X = prev[2][0]
            if X > t and (ctx is None or X not in (ctx.get('exit'), ctx.get('cont'))) and X <= self.addr(i1):
                has_else = True
        line = self.stmt_line
        self.emit(pc, 'if (%s)' % cond)
        self.emit(pc, '{')
        self.ind += 1
        if has_else:
            self.walk(st, i + 1, ti - 1, ctx)
        else:
            self.walk(st, i + 1, ti, ctx)
        self.flush(pc)
        self.ind -= 1
        self.stmt_line = line
        self.emit(pc, '}')
        st['stk'] = list(saved[0]); st['sp'] = saved[1]
        if has_else:
            X = prev[2][0]
            xi = self.idx.get(X, i1)
            self.emit(pc, 'else')
            self.emit(pc, '{')
            self.ind += 1
            self.walk(st, ti, xi, ctx)
            self.flush(pc)
            self.ind -= 1
            self.emit(pc, '}')
            st['stk'] = list(saved[0]); st['sp'] = saved[1]
            return xi
        return ti

    def addr(self, i):
        return self.insns[i][0] if i < self.N else self.f.ce

    # ------------------------------------------------------------------ jumps
    def jumpstmt(self, st, i, i1, ctx):
        pc, op, ar = self.insns[i]
        t = ar[0]
        self.flush(pc)
        if ctx is not None and t == ctx.get('exit'):
            self.emit(pc, 'break;')
            return i + 1
        if ctx is not None and t == ctx.get('cont'):
            self.emit(pc, 'continue;')
            return i + 1
        # for loop:  init ; jump Ltest ; Linc: ... ; Ltest: cond ; jzer Lexit ; body ; jump Linc ; Lexit
        if t > pc and i + 1 < self.N:
            linc = self.insns[i + 1][0]
            bl = [b for b in self.back.get(linc, []) if b > i and self.insns[b][1] == 'jump']
            if bl:
                return self.forloop(st, i, max(bl), ctx)
        self.emit(pc, 'goto L_%x;' % t)
        self.labels_needed.add(t)
        return i + 1

    def forloop(self, st, i, b, ctx):
        insns = self.insns
        pc = insns[i][0]
        t = insns[i][2][0]
        ti = self.idx[t]
        lexit = insns[b + 1][0] if b + 1 < self.N else self.f.ce
        # init: statements emitted since statement start
        init_lines = self.out[self.stmt_out_mark:]
        del self.out[self.stmt_out_mark:]
        il = [x[3].rstrip(';') for x in init_lines]
        if len(il) > 1 and all(x.startswith('new ') for x in il):
            init = il[0] + ', ' + ', '.join(x[4:] for x in il[1:])
        else:
            init = ', '.join(il)
        line = self.stmt_line
        # inc: [i+1, ti)
        inc = self.capture(st, i + 1, ti, ctx)
        # cond: from ti to jzer -> lexit
        ci = ti
        cond = ''
        j = ti
        k = None
        while j < b:
            if insns[j][1] in CJ and j not in self.sc_jumps and insns[j][2][0] == lexit:
                k = j
                break
            if insns[j][1] == 'break' and j > ti:
                break
            j += 1
        saved = (list(st['stk']), st['sp'])
        if k is not None:
            sub = self.capture_expr(st, ti, k)
            cond = self.cond_text(insns[k][1], st['pri'], insns[k][0])
            body0 = k + 1
        else:
            body0 = ti
        incs = ', '.join(x.rstrip(';') for x in inc)
        self.stmt_line = line
        self.emit(pc, ('for (%s; %s; %s)' % (init, cond, incs)).replace('; ;', ';;').replace('; )', ';)').replace('(; ', '(;'))
        self.emit(pc, '{')
        self.ind += 1
        self.walk(st, body0, b, {'exit': lexit, 'cont': insns[i + 1][0]})
        self.flush(pc)
        self.ind -= 1
        self.emit(pc, '}')
        st['stk'] = list(saved[0]); st['sp'] = saved[1]
        return b + 1

    def capture(self, st, i0, i1, ctx):
        mark = len(self.out)
        ind = self.ind
        saved = (list(st['stk']), st['sp'])
        sl = self.stmt_line
        self.walk(st, i0, i1, ctx)
        self.flush(0)
        lines = [x[3] for x in self.out[mark:]]
        del self.out[mark:]
        st['stk'] = list(saved[0]); st['sp'] = saved[1]
        self.stmt_line = sl
        return lines

    def capture_expr(self, st, i0, i1):
        mark = len(self.out)
        sl = self.stmt_line
        self.walk(st, i0, i1, None)
        extra = self.out[mark:]
        del self.out[mark:]
        self.stmt_line = sl
        self.pending = None
        return extra

    # ------------------------------------------------------------------ loops
    def loop(self, st, i, b, ctx):
        """insn i is 'break' at loop head (target of back jump b)."""
        insns = self.insns
        pc = insns[i][0]
        lexit = insns[b + 1][0] if b + 1 < self.N else self.f.ce
        self.inloop.add(pc)
        saved = (list(st['stk']), st['sp'])
        line = self.lineof(pc)
        # do-while: insns[b-1] is jzer -> lexit
        if insns[b - 1][1] == 'jzer' and insns[b - 1][2][0] == lexit and (b - 1) not in self.sc_jumps:
            # find condition start: last 'break' before b-1
            cs = b - 1
            while cs > i and insns[cs][1] != 'break':
                cs -= 1
            if cs > i:
                self.flush(pc)
                self.emit(pc, 'do')
                self.emit(pc, '{')
                self.ind += 1
                self.walk(st, i, cs, {'exit': lexit, 'cont': insns[cs][0]})
                self.flush(pc)
                self.ind -= 1
                self.stmt_line = self.lineof(insns[cs][0])
                self.capture_expr(st, cs + 1, b - 1)
                c = self.cond_text('jzer', st['pri'], insns[cs][0])
                self.emit(pc, '}')
                self.emit(pc, 'while (%s);' % c)
                st['stk'] = list(saved[0]); st['sp'] = saved[1]
                return b + 1
        # while(cond): from i+1 to jzer -> lexit before next break
        j = i + 1
        k = None
        while j < b:
            if insns[j][1] == 'break':
                break
            if insns[j][1] in CJ and j not in self.sc_jumps and insns[j][2][0] == lexit:
                k = j
                break
            j += 1
        self.flush(pc)
        self.stmt_line = line
        if k is not None:
            self.capture_expr(st, i + 1, k)
            c = self.cond_text(insns[k][1], st['pri'], insns[k][0])
            self.emit(pc, 'while (%s)' % c)
            body0 = k + 1
        else:
            self.emit(pc, 'while (true)')
            body0 = i + 1
        self.emit(pc, '{')
        self.ind += 1
        self.walk(st, body0, b, {'exit': lexit, 'cont': pc})
        self.flush(pc)
        self.ind -= 1
        self.emit(pc, '}')
        st['stk'] = list(saved[0]); st['sp'] = saved[1]
        return b + 1

    # ------------------------------------------------------------------ switch
    def switchstmt(self, st, i, i1, ctx):
        insns = self.insns
        pc, op, ar = insns[i]
        P = st['pri']
        if self.pending is P:
            self.pending = None
        self.flush(pc)
        ti = self.idx[ar[0]]
        ct = insns[ti]
        cnt, dflt = ct[2][0], ct[2][1]
        cases = ct[2][2:]
        lexit = insns[ti + 1][0] if ti + 1 < self.N else self.f.ce
        # group case values by target
        bytgt = collections.OrderedDict()
        for v, t in cases:
            bytgt.setdefault(t, []).append(v)
        tgts = sorted(set(list(bytgt.keys()) + ([dflt] if dflt != lexit else [])))
        line = self.stmt_line
        self.emit(pc, 'switch (%s)' % self.ex(P, pc))
        self.emit(pc, '{')
        saved = (list(st['stk']), st['sp'])
        for n, t in enumerate(tgts):
            s0 = self.idx[t]
            s1 = self.idx[tgts[n + 1]] if n + 1 < len(tgts) else ti
            # strip final jump lexit
            e1 = s1
            if insns[s1 - 1][1] == 'jump' and insns[s1 - 1][2][0] == lexit:
                e1 = s1 - 1
            if t in bytgt:
                vals = bytgt[t]
                lab = 'case %s:' % ', '.join(self.case_vals(vals))
                if t == dflt:
                    lab += ' default:'
            else:
                lab = 'default:'
            self.stmt_line = self.lineof(t)
            self.ind += 1
            self.emit(t, lab)
            self.emit(t, '{')
            self.ind += 1
            st['stk'] = list(saved[0]); st['sp'] = saved[1]
            self.walk(st, s0, e1, ctx)
            self.flush(t)
            self.ind -= 1
            self.emit(t, '}')
            self.ind -= 1
        self.stmt_line = line
        self.emit(pc, '}')
        st['stk'] = list(saved[0]); st['sp'] = saved[1]
        return ti + 1

    def case_vals(self, vals):
        vals = sorted(vals)
        out = []
        k = 0
        while k < len(vals):
            j = k
            while j + 1 < len(vals) and vals[j + 1] == vals[j] + 1:
                j += 1
            if j - k >= 2:
                out.append('%d .. %d' % (vals[k], vals[j]))
            else:
                out.extend(str(v) for v in vals[k:j + 1])
            k = j + 1
        return out

    # ------------------------------------------------------------------ short circuit
    def reduce2(self, nodes, m, pc):
        first, T, F, end = m
        N = []
        for k, (start, cond, kind, target, nxt) in enumerate(nodes):
            nx = nodes[k + 1][0] if k + 1 < len(nodes) else first
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
                refs = sum(1 for z in N if z is not a and z is not b and (z[2] == b[0] or z[3] == b[0]))
                if refs:
                    continue
                if a[2] == b[0] and a[3] == b[3]:
                    N[k] = [a[0], Bin('&&', a[1], b[1]), b[2], b[3]]
                elif a[3] == b[0] and a[2] == b[2]:
                    N[k] = [a[0], Bin('||', a[1], b[1]), b[2], b[3]]
                else:
                    continue
                del N[k + 1]
                changed = True
                break
        if len(N) == 1:
            n = N[0]
            single = len(nodes) == 1
            try:
                n[1].merged = True
            except Exception:
                pass
            if n[2] == 'T' and n[3] == 'F':
                return Tern(n[1], C(1), C(0)) if single else n[1]
            if n[2] == 'F' and n[3] == 'T':
                return Tern(n[1], C(0), C(1)) if single else Un('!', n[1])
        return Raw('/*SC?*/(' + ' ; '.join('%s?%s:%s' % (self.ex(n[1], pc), n[2], n[3]) for n in N) + ')')


def render(d, f, out):
    lines = ['// %s:%d' % (d.fileof(f.addr), d.lineof(f.addr)), header(d, f), '{']
    for pc, ln, ind, t in out:
        lines.append('%s%s%s' % ('\t' * ind, t, '' if t.endswith(('{', '}')) or t.startswith(('if', 'else', 'for', 'while', 'switch', 'do', 'case', 'default')) else '  //' + str(ln) if False else ''))
    lines.append('}')
    return '\n'.join(lines)


if __name__ == '__main__':
    amx = amxload.Amx(sys.argv[1], sys.argv[2])
    d = Decomp2(amx)
    only = sys.argv[3] if len(sys.argv) > 3 else None
    for f in d.funcs:
        if only and f.name != only:
            continue
        out = d.decompile(f)
        print(render(d, f, out))
        print()
