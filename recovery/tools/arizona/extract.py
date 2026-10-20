#!/usr/bin/env python3
"""Estrae i DATI DEL MONDO di Arizona Prime (v2) limitati a Los Santos e li salva in world/arizona/*.json.
Solo dati (coordinate, interni, modelli, prezzi): nessun codice dei sistemi, nessun dato personale
(i proprietari diventano "nessuno"; account, password, email, IP non vengono letti).

Uso: extract.py <cartella "Arizona Prime(v2)"> <cartella di uscita>"""
import re, os, sys, json, subprocess

SRC, OUT = sys.argv[1], sys.argv[2]
os.makedirs(OUT, exist_ok=True)

# Los Santos (zona di GTA SA "Los Santos": x 44.6..2997, y -2892.9..-768)
def in_ls(x, y):
    return 44.6 <= x <= 2997.0 and -2892.9 <= y <= -768.0

def read_cp1251(p):
    return open(p, 'rb').read().decode('cp1251', errors='replace')

pwn = read_cp1251(os.path.join(SRC, 'gamemodes', 'arizona.pwn')).replace('\r\n', '\n')
L = pwn.split('\n')
sqlp = [os.path.join(dp, f) for dp, _, fs in os.walk(SRC) for f in fs if f.endswith('.sql')][0]
sql = open(sqlp, 'rb').read().decode('utf-8', errors='replace')

# ------------------------------------------------------------------ SQL
def rows(table):
    out = []
    for m in re.finditer(r'INSERT INTO `' + table + r'`\s*\(([^)]*)\)\s*VALUES\s*(.*?);\s*\n', sql, re.S):
        cols = re.findall(r'`(\w+)`', m.group(1))
        for r in re.findall(r"\((.*?)\)(?=\s*,\s*\(|\s*$)", m.group(2), re.S):
            vals = re.findall(r"'((?:[^'\\]|\\.)*)'|(-?[\d.]+)|(NULL)", r)
            vals = [a if a or not b else b for a, b, c in vals]
            out.append(dict(zip(cols, vals)))
    return out

def f(v):
    try: return round(float(v), 4)
    except: return 0.0

def i(v):
    try: return int(float(v))
    except: return 0

houses = []
seen = set()
for h in rows('houses'):
    ex, ey = f(h['Enter_X']), f(h['Enter_Y'])
    if not in_ls(ex, ey) or h['ID'] in seen:
        continue
    seen.add(h['ID'])
    houses.append({
        'src_id': i(h['ID']), 'enter': [ex, ey, f(h['Enter_Z'])], 'exit': [f(h['Exit_X']), f(h['Exit_Y']), f(h['Exit_Z'])],
        'interior': i(h['Interior']), 'cost': i(h['Cost']), 'level': i(h['Level']), 'class': i(h.get('Klass', 0)),
        'garage': [f(h.get('Garage_X', 0)), f(h.get('Garage_Y', 0)), f(h.get('Garage_Z', 0))] if i(h.get('Garage', 0)) else None,
    })

biz = []
for b in rows('businesses'):
    ex, ey = f(b['Enter_X']), f(b['Enter_Y'])
    if not in_ls(ex, ey):
        continue
    biz.append({'src_id': i(b['ID']), 'name_ru': b['Name'], 'enter': [ex, ey, f(b['Enter_Z'])],
                'exit': [f(b['Exit_X']), f(b['Exit_Y']), f(b['Exit_Z'])], 'interior': i(b['Interior']),
                'cost': i(b['Cost']), 'level': i(b['Level']), 'enter_price': i(b.get('EnterPrice', 0))})

famhouses = []
for h in rows('familyhouse'):
    ex, ey = f(h['Enter_X']), f(h['Enter_Y'])
    if in_ls(ex, ey):
        famhouses.append({'src_id': i(h['ID']), 'name_ru': h['FamHouseName'], 'enter': [ex, ey, f(h['Enter_Z'])],
                          'exit': [f(h['Exit_X']), f(h['Exit_Y']), f(h['Exit_Z'])], 'interior': i(h['INT']), 'cost': i(h['Cost'])})

gangzones = [{'src_id': i(z['ID']), 'gang': i(z['GangID']), 'min': [f(z['Min_X']), f(z['Min_Y'])], 'max': [f(z['Max_X']), f(z['Max_Y'])]}
             for z in rows('zonesinfo') if in_ls((f(z['Min_X']) + f(z['Max_X'])) / 2, (f(z['Min_Y']) + f(z['Max_Y'])) / 2)]

# ------------------------------------------------------------------ porte / teletrasporti (TeleportInfo)
teleports = []
m = re.search(r'new TeleportInfo\[\d+\]\[tpInfo\]\s*=\s*\{(.*?)\n\};', pwn, re.S)
for row in re.findall(r'\{\s*"([^"]*)"\s*,([^{}]*)\}', m.group(1)):
    v = [x.strip() for x in row[1].split(',')]
    if len(v) < 11: continue
    nums = [float(x) for x in v[:11]]
    teleports.append({'name_ru': row[0], 'enter': nums[0:3], 'enter_world': int(nums[3]), 'enter_int': int(nums[4]),
                      'exit': nums[5:8], 'exit_a': nums[8], 'exit_int': int(nums[9]), 'exit_world': int(nums[10])})
# coppie: si tiene il teletrasporto se l'ingresso o l'uscita e' su una strada di Los Santos
def street_ls(p, world, interior):
    return interior == 0 and world == 0 and in_ls(p[0], p[1]) and p[2] < 300.0
teleports = [t for t in teleports if street_ls(t['enter'], t['enter_world'], t['enter_int']) or street_ls(t['exit'], t['exit_world'], t['exit_int'])]

# ------------------------------------------------------------------ veicoli
NUM = r'\s*(-?\d+(?:\.\d+)?)\s*'
TEAM = {mm.group(1): int(mm.group(2)) for mm in re.finditer(r'#define\s+(TEAM_\w+)\s+\((\d+)\)', pwn)}
JOB = {mm.group(1): int(mm.group(2)) for mm in re.finditer(r'#define\s+(JOB_\w+)\s+\(?(\d+)\)?', pwn)}
vehicles = []
for mm in re.finditer(r'CreateTeamVehicle\(\s*(\w+)\s*,\s*(\w+)\s*,' + ','.join([NUM] * 7), pwn):
    x, y = float(mm.group(4)), float(mm.group(5))
    if in_ls(x, y):
        vehicles.append({'kind': 'faction', 'group': mm.group(1), 'rank': mm.group(2), 'model': int(float(mm.group(3))),
                         'pos': [x, y, float(mm.group(6)), float(mm.group(7))], 'colors': [int(float(mm.group(8))), int(float(mm.group(9)))]})
for mm in re.finditer(r'CreateJobVehicle\(\s*(\w+)\s*,' + ','.join([NUM] * 7), pwn):
    x, y = float(mm.group(3)), float(mm.group(4))
    if in_ls(x, y):
        vehicles.append({'kind': 'job', 'group': mm.group(1), 'model': int(float(mm.group(2))),
                         'pos': [x, y, float(mm.group(5)), float(mm.group(6))], 'colors': [int(float(mm.group(7))), int(float(mm.group(8)))]})
for mm in re.finditer(r'MopedVehicle\[\d+\]\s*=\s*J_AddStaticVehicleEx\(' + ','.join([NUM] * 7), pwn):
    x, y = float(mm.group(2)), float(mm.group(3))
    if in_ls(x, y):
        vehicles.append({'kind': 'rent', 'group': 'MOPED', 'model': int(float(mm.group(1))),
                         'pos': [x, y, float(mm.group(4)), float(mm.group(5))], 'colors': [int(float(mm.group(6))), int(float(mm.group(7)))]})
# veicoli personali del DB (ownable): si importano come veicoli in vendita / parcheggiati, SENZA proprietario
parked = []
for c in rows('ownable'):
    x, y = f(c['Pos_X']), f(c['Pos_Y'])
    if in_ls(x, y) and i(c.get('VirtualWorld', 0)) == 0 and i(c.get('Interior', 0)) == 0:
        parked.append({'model': i(c['Model']), 'pos': [x, y, f(c['Pos_Z']), f(c['Pos_A'])], 'colors': [i(c['Color_1']), i(c['Color_2'])], 'cost': i(c['Cost'])})

# ------------------------------------------------------------------ mapping (oggetti statici)
# blocchi statici: OnGameModeInit (mappa della citta') e Loadtest228
def func_range(header_re):
    for k, l in enumerate(L):
        if re.match(header_re, l):
            depth = 0; started = False
            for j in range(k, len(L)):
                depth += L[j].count('{') - L[j].count('}')
                if '{' in L[j]: started = True
                if started and depth == 0:
                    return k, j
    return None
ranges = [func_range(r'^public OnGameModeInit\(\)'), func_range(r'^stock Loadtest228\(\)')]
# file di mapping inclusi da Loadmap(): pawno/include/cod/*.inc
for inc in ('pososi.inc', 'ffbi.inc'):
    ip = os.path.join(SRC, 'pawno', 'include', 'cod', inc)
    if os.path.exists(ip):
        base = len(L)
        L.extend(read_cp1251(ip).replace('\r\n', '\n').split('\n'))
        ranges.append((base, len(L) - 1))
LIT = r'(?:0x[0-9A-Fa-f]+|-?\d+(?:\.\d+)?|"(?:[^"\\]|\\.)*"|[A-Za-z_]\w*)'
ARGS = re.compile(r'^\s*' + LIT + r'(\s*,\s*' + LIT + r')*\s*$')
objs = []
cur = None
for a, b in ranges:
    for l in L[a:b + 1]:
        s = l.strip()
        mm = re.match(r'^(?:([A-Za-z_]\w*(?:\[\d+\])?)\s*=\s*)?(CreateDynamicObject|CreateObject)\((.*)\);\s*(//.*)?$', s)
        if mm and ARGS.match(mm.group(3)):
            args = [x.strip() for x in re.findall(LIT, mm.group(3))]
            cur = {'model': int(args[0]), 'pos': [float(x) for x in args[1:4]], 'rot': [float(x) for x in args[4:7]],
                   'world': int(args[7]) if len(args) > 7 and re.fullmatch(r'-?\d+', args[7]) else -1,
                   'interior': int(args[8]) if len(args) > 8 and re.fullmatch(r'-?\d+', args[8]) else -1,
                   'dist': float(args[11]) if len(args) > 11 and re.fullmatch(r'-?\d+(\.\d+)?', args[11]) else 300.0,
                   'mat': [], 'mattext': []}
            objs.append(cur)
            continue
        mm = re.match(r'^(SetDynamicObjectMaterial|SetObjectMaterial)\(\s*[A-Za-z_]\w*(?:\[\d+\])?\s*,(.*)\);', s)
        if mm and cur is not None and ARGS.match(mm.group(2)):
            cur['mat'].append([x.strip() for x in re.findall(LIT, mm.group(2))])
            continue
        mm = re.match(r'^(SetDynamicObjectMaterialText|SetObjectMaterialText)\(\s*[A-Za-z_]\w*(?:\[\d+\])?\s*,(.*)\);', s)
        if mm and cur is not None and ARGS.match(mm.group(2)):
            cur['mattext'].append([x.strip() for x in re.findall(LIT, mm.group(2))])
            continue
        if re.match(r'^(for|while|if|switch)\b', s):
            cur = None
# interni personalizzati da tenere: uscite di case, business, case famiglia e porte tenute
anchors = [h['exit'] for h in houses] + [b['exit'] for b in biz] + [h['exit'] for h in famhouses] + [t['exit'] for t in teleports] + [t['enter'] for t in teleports]
def near_anchor(p, r=120.0):
    return any((p[0] - a[0]) ** 2 + (p[1] - a[1]) ** 2 + (p[2] - a[2]) ** 2 <= r * r for a in anchors)
mapping = [o for o in objs if in_ls(o['pos'][0], o['pos'][1]) or near_anchor(o['pos'])]

removed = []
for mm in re.finditer(r'RemoveBuildingForPlayer\(\s*playerid\s*,' + ','.join([NUM] * 5) + r'\)', pwn):
    v = [float(x) for x in mm.groups()]
    if in_ls(v[1], v[2]):
        removed.append({'model': int(v[0]), 'pos': v[1:4], 'radius': v[4]})

data = {'houses': houses, 'businesses': biz, 'family_houses': famhouses, 'teleports': teleports, 'gangzones': gangzones,
        'vehicles': vehicles, 'parked_vehicles': parked, 'mapping': mapping, 'removed_buildings': removed,
        'team_ids': TEAM, 'job_ids': JOB}
for k, v in data.items():
    json.dump(v, open(os.path.join(OUT, k + '.json'), 'w'), ensure_ascii=False, indent=1)
print({k: len(v) for k, v in data.items()}, 'oggetti totali statici:', len(objs))
