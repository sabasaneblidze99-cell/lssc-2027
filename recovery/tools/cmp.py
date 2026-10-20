"""Compare two AMX files function by function (normalised)."""
import sys, os, subprocess, collections
import amxload, decomp

DIS = '/home/claude/tools/pawn-src/build/pawndisasm'


def load(amxpath):
    d = os.path.dirname(os.path.abspath(amxpath))
    img = os.path.join(d, 'image.bin')
    subprocess.run([DIS, os.path.basename(amxpath)], cwd=d, stdout=subprocess.DEVNULL)
    a = amxload.Amx(amxpath, img)
    return a, decomp.Decomp(a)


def norm_funcs(a, d):
    res = {}
    for f in d.funcs:
        fn = d.fileof(f.addr)
        if not ('modules/' in fn or fn.endswith('lsrp.pwn')):
            continue
        seq = []
        for ad, op, ar in a.disasm(f.cs, f.ce):
            if op == 'break':
                seq.append('break'); continue
            if op == 'call':
                g = d.fbyaddr.get(ar[0]); seq.append('call ' + (g.name if g else '?')); continue
            if op == 'sysreq.c':
                seq.append('sysreq ' + a.natives[ar[0]]); continue
            if op in ('jump', 'jzer', 'jnz', 'switch'):
                seq.append('%s %+d' % (op, ar[0] - f.cs)); continue
            if op == 'casetbl':
                seq.append('casetbl %d %+d %s' % (ar[0], ar[1] - f.cs, sorted((v, t - f.cs) for v, t in ar[2:]))); continue
            if op in ('load.pri', 'load.alt', 'stor.pri', 'inc', 'dec'):
                seq.append('%s %s' % (op, sym(d, ar[0]))); continue
            if op in ('const.pri', 'const.alt', 'push.c'):
                seq.append('%s %s' % (op, cval(d, ar[0]))); continue
            seq.append(op + (' ' + ' '.join(str(x) for x in ar) if ar else ''))
        res[f.name] = seq
    return res


def sym(d, v):
    g = d.global_at(v)
    if g is not None:
        return 'G:%s+%d' % (g.name, v - g.addr)
    s = d.literal(v)
    if s is not None:
        return 'S:' + repr(s[:60])
    return 'D:%d' % v


def cval(d, v):
    if v <= 64 or v >= len(d.a.data):
        return str(v)
    g = d.global_at(v)
    if g is not None:
        return '%d|G:%s+%d' % (v, g.name, v - g.addr)
    s = d.literal(v)
    if s is not None:
        return '%d|S:%r' % (v, s[:80])
    return str(v)


def same_val(x, y):
    if x == y:
        return True
    xp = x.split(' ', 1); yp = y.split(' ', 1)
    if xp[0] != yp[0] or len(xp) != 2 or len(yp) != 2:
        return False
    xa = xp[1].split('|', 1); ya = yp[1].split('|', 1)
    if xa[0] == ya[0]:
        return True
    if len(xa) == 2 and len(ya) == 2 and xa[1] == ya[1]:
        return True
    return False


if __name__ == '__main__':
    a1, d1 = load(sys.argv[1])
    a2, d2 = load(sys.argv[2])
    f1 = norm_funcs(a1, d1)
    f2 = norm_funcs(a2, d2)
    ok = 0; bad = []
    missing = [n for n in f1 if n not in f2]
    for n, s1 in f1.items():
        s2 = f2.get(n)
        if s2 is None:
            continue
        if len(s1) == len(s2) and all(same_val(x, y) for x, y in zip(s1, s2)):
            ok += 1
        else:
            k = 0
            while k < min(len(s1), len(s2)) and same_val(s1[k], s2[k]):
                k += 1
            bad.append((n, k, len(s1), len(s2)))
    print('functions: %d  identical: %d  different: %d  missing: %d' % (len(f1), ok, len(bad), len(missing)))
    cat = collections.Counter()
    for n, k, l1, l2 in bad:
        s1, s2 = f1[n], f2[n]
        a_ = s1[k].split(' ')[0] if k < l1 else 'END'
        b_ = s2[k].split(' ')[0] if k < l2 else 'END'
        cat[(a_, b_)] += 1
    for c, v in cat.most_common(25):
        print('  %4d  %s -> %s' % (v, c[0], c[1]))
    out = sys.argv[3] if len(sys.argv) > 3 else None
    if out:
        with open(out, 'w') as fo:
            for n, k, l1, l2 in bad:
                fo.write('%s first-diff@%d len %d/%d\n' % (n, k, l1, l2))
                s1, s2 = f1[n], f2[n]
                for j in range(max(0, k - 6), min(max(l1, l2), k + 6)):
                    fo.write('   %-50s | %s\n' % (s1[j] if j < l1 else '', s2[j] if j < l2 else ''))
            fo.write('MISSING: %s\n' % missing)
