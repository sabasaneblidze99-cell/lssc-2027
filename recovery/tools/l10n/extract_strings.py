#!/usr/bin/env python3
"""Estrae i testi per i giocatori (non ancora in georgiano) dai moduli Pawn -> l10n/strings_<gruppo>.json.
Esclude: log del server (print/printf/Log_Write), SQL, nomi di callback/eventi/timer, nomi di file, texture del mapping,
nomi dei comandi, nomi dei modelli dei veicoli."""
import re, os, json, sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
G = os.path.join(ROOT, 'gamemodes')
OUT = os.path.join(ROOT, 'l10n')
os.makedirs(OUT, exist_ok=True)
SKIP_FILES = {'world/az_map.inc', 'world/az_data.inc', 'core/vehicle_names.inc', 'core/enums.inc', 'core/forwards.inc'}
LOG_CALL = re.compile(r'\b(print|printf|Log_Write|mysql_tquery|mysql_format|mysql_query|SetTimer|SetTimerEx|CallLocalFunction|CallRemoteFunction|funcidx|cef_subscribe|fexist|fopen|cache_get_value_name\w*|DB_LivingMakeOpId|DB_\w*MakeOpId|strcmp|ApplyAnimation|PreloadAnimLib|cef_emit_event|CefUI_Open|CefUI_Close|CefUI_AuthStep|SvCreate\w*|LD_StrEqI|bcrypt_\w+)\s*\(')
STR = re.compile(r'"((?:[^"\\\n]|\\.)*)"')

def is_text(t):
    if re.search(r'[Ⴀ-ჿ]', t):
        return False
    core = re.sub(r'\{[0-9A-Fa-f]{6}\}|%[-+ 0-9.]*[a-zA-Z]|\\n|\\t|\\\\', ' ', t)
    if len(re.findall(r'[A-Za-zàèéìòù]', core)) < 3:
        return False
    if re.fullmatch(r'[a-z0-9_:./-]+', t):        # identificatori, chiavi, eventi, file
        return False
    if re.fullmatch(r'/?[a-z]+( [a-z|\[\]<>]+)*', t) and len(t) < 14:   # nomi di comandi
        return False
    if re.search(r'\b(SELECT|INSERT|UPDATE|DELETE|FROM|WHERE|CALL|COALESCE)\b', t):
        return False
    return True

items = []
for dp, _, fs in os.walk(os.path.join(G, 'modules')):
    for f in sorted(fs):
        if not f.endswith('.inc'):
            continue
        p = os.path.join(dp, f)
        rel = os.path.relpath(p, os.path.join(G, 'modules'))
        if rel in SKIP_FILES:
            continue
        for n, line in enumerate(open(p, encoding='utf-8'), 1):
            code = re.sub(r'//.*$', '', line) if '"' not in line.split('//')[0] else line
            if re.match(r'\s*//', line) or re.match(r'\s*#(define|include|emit)', line) and 'define' not in line:
                continue
            for m in STR.finditer(code):
                t = m.group(1)
                if not is_text(t):
                    continue
                before = code[:m.start()]
                # stringa dentro una chiamata di log/SQL/sistema?
                depth = 0; owner = None
                for k in range(len(before) - 1, -1, -1):
                    ch = before[k]
                    if ch == ')': depth += 1
                    elif ch == '(':
                        if depth == 0:
                            mm = re.search(r'([A-Za-z_]\w*)\s*$', before[:k])
                            owner = mm.group(1) if mm else None
                            break
                        depth -= 1
                if owner and LOG_CALL.match(owner + '('):
                    continue
                items.append({'id': len(items), 'file': rel, 'line': n, 'it': t, 'ctx': line.strip()[:200], 'ka': ''})

# gruppi di circa 250 testi, per cartella
groups = {}
for it in items:
    groups.setdefault(it['file'].split('/')[0], []).append(it)
batches = []; cur = []
for k in sorted(groups):
    for it in groups[k]:
        cur.append(it)
        if len(cur) >= 260:
            batches.append(cur); cur = []
if cur: batches.append(cur)
for i, b in enumerate(batches):
    json.dump(b, open(os.path.join(OUT, 'strings_%02d.json' % i), 'w'), ensure_ascii=False, indent=1)
print(len(items), 'testi in', len(batches), 'file')
