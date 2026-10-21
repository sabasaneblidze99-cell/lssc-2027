#!/usr/bin/env python3
"""Applica le traduzioni di l10n/strings_*.json ai moduli Pawn e allarga i buffer locali che ricevono testo georgiano.
Il georgiano occupa 3 byte per lettera: un buffer di 128 per un testo italiano diventa troppo piccolo."""
import re, json, glob, os, collections

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
MOD = os.path.join(ROOT, 'gamemodes', 'modules')
by_file = collections.defaultdict(list)
for f in sorted(glob.glob(os.path.join(ROOT, 'l10n', 'strings_*.json'))):
    for it in json.load(open(f)):
        if it.get('ka'):
            by_file[it['file']].append(it)

applied = missed = 0
touched = set()
for rel, items in by_file.items():
    p = os.path.join(MOD, rel)
    L = open(p, encoding='utf-8').read().split('\n')
    for it in items:
        n = it['line'] - 1
        old = '"' + it['it'] + '"'
        if old in L[n]:
            L[n] = L[n].replace(old, '"' + it['ka'] + '"', 1)
            applied += 1
        else:
            missed += 1
            print('NON TROVATO %s:%d %s' % (rel, it['line'], it['it'][:60]))
    open(p, 'w', encoding='utf-8').write('\n'.join(L))
    touched.add(p)

# ------------------------------------------------------------------ buffer locali
GEO = re.compile(r'[Ⴀ-ჿ]')
HDR = re.compile(r'^(?:stock |public |static )*(?:[A-Za-z_]\w*:)?[A-Za-z_]\w*\(.*\)\s*$')
def grow(n):
    return min(4096, n * 3 if n <= 256 else n * 2)
grown = 0
for p in glob.glob(os.path.join(MOD, '**', '*.inc'), recursive=True):
    L = open(p, encoding='utf-8').read().split('\n')
    i = 0
    changed = False
    while i < len(L):
        if HDR.match(L[i]) and i + 1 < len(L) and L[i + 1].strip() == '{':
            # corpo della funzione
            depth = 0; j = i + 1
            while j < len(L):
                depth += L[j].count('{') - L[j].count('}')
                if depth == 0: break
                j += 1
            body = '\n'.join(L[i + 1:j + 1])
            # buffer che ricevono testo georgiano: format/strcat/strcopy con un letterale georgiano
            targets = set()
            for m in re.finditer(r'\b(format|strcat|strcopy|strins)\s*\(\s*([A-Za-z_]\w*)\s*,\s*(\d+|sizeof\s*\(?\s*\w+\s*\)?)?[^;]*"[^"]*"', body):
                seg = body[m.start():body.find(';', m.start())]
                if GEO.search(seg):
                    targets.add(m.group(2))
            for m in re.finditer(r'\bstrcat\s*\(\s*([A-Za-z_]\w*)\s*,\s*"[^"]*[Ⴀ-ჿ][^"]*"', body):
                targets.add(m.group(1))
            for name in targets:
                dm = re.search(r'\bnew\b[^;\n]*?\b' + name + r'\[(\d+)\]', body)
                if not dm:
                    continue
                old = int(dm.group(1)); new = grow(old)
                if new == old:
                    continue
                for k in range(i + 1, j + 1):
                    s = L[k]
                    s2 = re.sub(r'(\bnew\b[^;]*?\b' + name + r')\[' + str(old) + r'\]', r'\g<1>[' + str(new) + ']', s)
                    s2 = re.sub(r'\b(format|strcat|strmid|strcopy|strins|Lang_Translit|Util_\w+|UI_\w+)\s*\(\s*' + name + r'\s*,\s*' + str(old) + r'\b', lambda mm: mm.group(0)[:-len(str(old))] + str(new), s2)
                    s2 = re.sub(r'(\bstrcat\s*\(\s*' + name + r'\s*,[^;]*,\s*)' + str(old) + r'(\s*\))', r'\g<1>' + str(new) + r'\2', s2)
                    s2 = re.sub(r'(\bformat\s*\(\s*' + name + r'\s*,\s*)' + str(old) + r'\b', r'\g<1>' + str(new), s2)
                    if s2 != s:
                        L[k] = s2; changed = True
                grown += 1
            i = j + 1
            continue
        i += 1
    if changed:
        open(p, 'w', encoding='utf-8').write('\n'.join(L))
print('applicate %d, non trovate %d, buffer allargati %d' % (applied, missed, grown))
