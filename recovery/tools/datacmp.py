import cmp, decomp
exec(open('/home/claude/tools/amx/env.sh').read().replace('G=', 'G="').strip() + '"')
a1, d1 = cmp.load(G + '/lsrp.amx')
a2, d2 = cmp.load('/home/claude/rec/gamemodes/lsrp.amx')
g2 = {}
for s in d2.glob:
    g2.setdefault((s.name, s.vclass, s.cs if s.vclass == 2 else 0), s)
bad = 0; n = 0
by2 = collections = None
names2 = {}
for s in d2.glob:
    names2.setdefault(s.name, []).append(s)
for s in d1.glob:
    cands = names2.get(s.name, [])
    t = None
    for c in cands:
        if [z for _, z in c.dims] == [z for _, z in s.dims] and c.vclass == s.vclass:
            t = c; break
    if t is None:
        print('MISSING', s.name); bad += 1; continue
    sz = d1.gsize(s)
    if s.vclass == 2 and len(cands) > 1:
        continue
    c1 = [a1.dcell(s.addr + 4 * k) for k in range(sz // 4)]
    c2 = [a2.dcell(t.addr + 4 * k) for k in range(sz // 4)]
    n += 1
    if c1 != c2:
        bad += 1
        k = next(i for i in range(len(c1)) if c1[i] != c2[i])
        print('DIFF', s.name, s.dims, 'at cell', k, c1[k], c2[k])
print('globals compared', n, 'bad', bad)
