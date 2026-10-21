#!/usr/bin/env python3
"""Controlla le traduzioni in l10n/strings_*.json: stessi specificatori di format (stesso ordine), stessi colori {RRGGBB},
stessi \\n \\t, stessi comandi /xxx, virgolette bilanciate. Uso: check.py [file ...] (default: tutti)."""
import re, json, sys, glob, os

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
files = sys.argv[1:] or sorted(glob.glob(os.path.join(ROOT, 'l10n', 'strings_*.json')))
SPEC = re.compile(r'%[-+ 0-9.]*[a-zA-Z%]')
COL = re.compile(r'\{[0-9A-Fa-f]{6}\}')
ESC = re.compile(r'\\[nt"\\]')
CMD = re.compile(r'(?<![A-Za-z0-9])/[a-z]{2,}')
bad = 0; todo = 0; done = 0
for f in files:
    for it in json.load(open(f)):
        ka = it.get('ka')
        if ka is None:
            done += 1; continue
        if ka == '':
            todo += 1; continue
        done += 1
        src = it['it']; errs = []
        if SPEC.findall(src) != SPEC.findall(ka): errs.append('format %s -> %s' % (SPEC.findall(src), SPEC.findall(ka)))
        if sorted(COL.findall(src)) != sorted(COL.findall(ka)): errs.append('colori')
        if ESC.findall(src) != ESC.findall(ka): errs.append('escape \\n \\t')
        if sorted(CMD.findall(src)) != sorted(CMD.findall(ka)): errs.append('comandi %s -> %s' % (CMD.findall(src), CMD.findall(ka)))
        if re.search(r'(?<!\\)"', ka): errs.append('virgolette non escapate')
        if errs:
            bad += 1
            print('%s #%d %s:%d: %s\n   IT: %s\n   KA: %s' % (os.path.basename(f), it['id'], it['file'], it['line'], '; '.join(errs), src, ka))
print('fatte %d, da fare %d, errori %d' % (done, todo, bad))
sys.exit(1 if bad else 0)
