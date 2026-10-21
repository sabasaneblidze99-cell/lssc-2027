#!/usr/bin/env python3
"""Estrae le destinazioni GPS di Arizona Prime (v2) -> world/arizona/gps.json. Solo DATI (nome russo, coordinate, categoria),
nessun codice. Uso: extract_gps.py <arizona.pwn>"""
import re, sys, json, os
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
src = open(sys.argv[1], 'rb').read().decode('cp1251')
# menu principale /gps: listitem -> dialog del sottomenu
root = re.search(r'case 705:\s*\{(.*?)case 7:', src, re.S).group(1)
subs = []
for m in re.finditer(r'case (\d+): ShowPlayerDialogEx\(playerid, (\d+), DIALOG_STYLE_LIST, !"([^"]*)", !"((?:[^"\\]|\\.)*)"', root):
    items = [re.sub(r'\{[0-9A-Fa-f]{6}\}', '', x).strip() for x in m.group(4).replace('\\n', '\n').split('\n')]
    subs.append((int(m.group(1)), int(m.group(2)), m.group(3), items))
def handler(did):
    m = re.search(r'\n[ \t]*case %d:\s*\{' % did, src)
    if not m: return {}
    i = m.end(); depth = 1
    while depth and i < len(src):
        depth += {'{': 1, '}': -1}.get(src[i], 0); i += 1
    body = src[m.end():i]
    out = {}
    for c in re.finditer(r'case (\d+):\s*(?:\{[^}]*?)?(?:EnableGPSForPlayer|SetPlayerCheckpoint\w*)\(playerid,\s*([-\d.]+),\s*([-\d.]+),\s*([-\d.]+)', body):
        out.setdefault(int(c.group(1)), [float(c.group(2)), float(c.group(3)), float(c.group(4))])
    return out
LS = lambda p: 44.6 < p[0] < 2997 and -2892 < p[1] < -768
res = []
for li, did, title, items in subs:
    h = handler(did)
    for k, name in enumerate(items):
        if k in h:
            p = h[k]
            res.append({'cat': li, 'cat_ru': title, 'name_ru': name, 'pos': p, 'ls': LS(p)})
json.dump([r for r in res if r['ls']], open(os.path.join(ROOT, 'world', 'arizona', 'gps.json'), 'w'), ensure_ascii=False, indent=1)
print('voci', len(res), 'a Los Santos', sum(r['ls'] for r in res))
for r in res:
    if r['ls']: print(r['cat'], r['cat_ru'], '|', r['name_ru'], r['pos'])
