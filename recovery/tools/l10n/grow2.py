#!/usr/bin/env python3
"""Allarga i buffer locali di testo (8..200 celle) usati come destinazione di format/strcat/strcopy/strmid o passati
con la loro dimensione a un'altra funzione: dopo la traduzione i testi georgiani sono ~3 volte piu' lunghi in byte."""
import re, glob, os
MOD = '/home/claude/lssc-2027/gamemodes/modules'
HDR = re.compile(r'^(?:stock |public |static )*(?:[A-Za-z_]\w*:)?[A-Za-z_]\w*\(.*\)\s*$')
SKIP = {'core/chat.inc', 'cef/cef_safe.inc', 'core/lang.inc', 'world/az_map.inc', 'world/az_data.inc'}
total = 0
for p in glob.glob(MOD + '/**/*.inc', recursive=True):
    rel = os.path.relpath(p, MOD)
    if rel in SKIP: continue
    L = open(p, encoding='utf-8').read().split('\n'); changed = False
    i = 0
    while i < len(L):
        if HDR.match(L[i]) and i + 1 < len(L) and L[i + 1].strip() == '{':
            depth = 0; j = i + 1
            while j < len(L):
                depth += L[j].count('{') - L[j].count('}')
                if depth == 0: break
                j += 1
            body = '\n'.join(L[i + 1:j + 1])
            for dm in re.finditer(r'\bnew\b([^;\n]*)', body):
                for nm in re.finditer(r'\b([A-Za-z_]\w*)\[(\d+)\](?!\s*\[)', dm.group(1)):
                    name, n = nm.group(1), int(nm.group(2))
                    if not (8 <= n <= 200): continue
                    if re.search(r'\bFloat:\s*' + name + r'\b', dm.group(1)): continue
                    used = re.search(r'\b(format|strcat|strcopy|strmid|strins|valstr)\s*\(\s*' + name + r'\s*,', body) or \
                           re.search(r'\b' + name + r'\s*,\s*' + str(n) + r'\s*[,)]', body)
                    if not used: continue
                    new = n * 3
                    for k in range(i + 1, j + 1):
                        s = L[k]
                        s2 = re.sub(r'(\bnew\b[^;]*?\b' + name + r')\[' + str(n) + r'\](?!\s*\[)', r'\g<1>[' + str(new) + ']', s)
                        s2 = re.sub(r'\b' + name + r'(\s*,\s*)' + str(n) + r'(\s*[,)])', name + r'\g<1>' + str(new) + r'\2', s2)
                        s2 = re.sub(r'(\b(?:format|strcat|strmid|strcopy|strins)\s*\(\s*' + name + r'\s*,[^;]*?,\s*)' + str(n) + r'(\s*\))', r'\g<1>' + str(new) + r'\2', s2)
                        if s2 != s: L[k] = s2; changed = True
                    total += 1
            i = j + 1; continue
        i += 1
    if changed: open(p, 'w', encoding='utf-8').write('\n'.join(L))
print('buffer allargati', total)
