#!/usr/bin/env python3
"""Allarga l'ultima dimensione degli array globali di stringhe che dopo la traduzione non bastano (errore 018)."""
import re, sys, os
ROOT = '/home/claude/lssc-2027/gamemodes'
errs = [l for l in open(sys.argv[1]) if 'error 018' in l]
done = 0
for e in errs:
    m = re.match(r'(.+?)\((\d+)\) : error 018', e)
    p = os.path.join(ROOT, m.group(1)); ln = int(m.group(2)) - 1
    L = open(p, encoding='utf-8').read().split('\n')
    i = ln
    while i >= 0 and not re.match(r'^\s*(new|static)\b.*\]\s*=\s*\{?', L[i]): i -= 1
    j = i; txt = ''
    while j < len(L):
        txt += L[j] + '\n'
        if re.search(r'\};\s*$', L[j]): break
        j += 1
    strs = re.findall(r'"((?:[^"\\]|\\.)*)"', txt)
    need = max((len(re.sub(r'\\(.)', r'\1', s).encode('utf-8')) for s in strs), default=0) + 1
    dm = re.search(r'(\[[^\]]*\])\s*=', L[i])
    dims = re.findall(r'\[([^\]]*)\]', L[i].split('=')[0])
    last = dims[-1]
    if last.isdigit() and int(last) < need:
        newv = ((need + 7) // 8) * 8
        head = L[i].split('=')[0]
        k = head.rfind('[' + last + ']')
        L[i] = head[:k] + '[' + str(newv) + ']' + head[k + len(last) + 2:] + '=' + '='.join(L[i].split('=')[1:])
        open(p, 'w', encoding='utf-8').write('\n'.join(L)); done += 1
        print(p.split('modules/')[1], i + 1, last, '->', newv)
print('allargati', done)
