#!/usr/bin/env python3
"""Genera i moduli Pawn e la migrazione SQL del mondo Arizona (Los Santos) da world/arizona/*.json.
  gamemodes/modules/world/az_map.inc    mapping (streamer) + edifici GTA rimossi
  gamemodes/modules/world/az_data.inc   porte/edifici, business, veicoli (tabelle costanti)
  sql/migrations/015_arizona_houses.sql case nel DB (idempotente)"""
import json, os, re

# ------------------------------------------------------------------ prezzi in lari (tetri) - economia GEL (docs/economia)
# Prezzo di GIOCO, non conversione del prezzo russo: classe della casa (dimensione/qualita' dell'interno), zona di LS, garage.
# Riferimento reale (Geostat Q2 2026): case private a Tbilisi ₾2.778-4.656/m2, nuove costruzioni ₾3.680-6.730/m2;
# le case di gioco costano circa la meta' di un immobile reale equivalente (progressione).
GEL_CLASS = {1: 6800000, 2: 11500000, 5: 26000000, 6: 42000000}
def ls_zone(x, y):
    if x < 1450 and y > -1300: return 'alta'    # Richman, Mulholland, Vinewood, Rodeo, Temple
    if x > 1800 and y < -1050: return 'bassa'   # Ganton, Idlewood, Jefferson, East LS, Willowfield, Playa del Seville
    return 'media'                              # Downtown, Commerce, Verona, Santa Maria, Marina, Las Colinas, Glen Park
GEL_ZONE = {'alta': 145, 'media': 100, 'bassa': 85}
def gel_price(h):
    if h['cost'] >= 10000000:
        return 185000000                        # villa unica del server di origine: ₾1.850.000
    base = GEL_CLASS.get(h['class'], 6800000) * GEL_ZONE[ls_zone(h['enter'][0], h['enter'][1])] // 100
    if h.get('garage'):
        base += 900000                          # garage: +₾9.000
    return (base + 25000) // 50000 * 50000      # arrotondato a ₾500
GEL_BIZ_TYPE = {1: 18000000, 2: 15000000, 3: 22000000, 4: 12000000, 5: 35000000, 6: 20000000, 7: 45000000, 8: 16000000, 9: 40000000, 10: 90000000}


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
W = os.path.join(ROOT, 'world', 'arizona')
def load(n): return json.load(open(os.path.join(W, n + '.json')))

# ------------------------------------------------------------------ traslitterazione cirillico -> latino (testi sugli oggetti)
CYR = dict(zip('абвгдеёжзийклмнопрстуфхцчшщъыьэюя', ['a','b','v','g','d','e','e','zh','z','i','y','k','l','m','n','o','p','r','s','t','u','f','h','ts','ch','sh','sch','','y','','e','yu','ya']))
def translit(s):
    out = ''
    for ch in s:
        lo = ch.lower()
        if lo in CYR:
            t = CYR[lo]
            out += t.capitalize() if ch != lo else t
        else:
            out += ch
    return out

def fl(v):
    s = ('%.6f' % float(v)).rstrip('0')
    return s + '0' if s.endswith('.') else s

def pstr(s):
    s = translit(s)
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'

# ------------------------------------------------------------------ mapping
mapping_src = load('mapping')
# copie identiche (stesso modello, posizione, rotazione, mondo, interno, distanza e materiali) presenti nel server di origine:
# si tiene solo la prima. Non cambia nulla a vista, libera slot dello streamer.
_seen = set(); mapping = []
for _o in mapping_src:
    _k = json.dumps(_o, sort_keys=True)
    if _k not in _seen:
        _seen.add(_k); mapping.append(_o)
MAP_DUP = len(mapping_src) - len(mapping)
removed = load('removed_buildings')
L = ['// GENERATO da recovery/tools/arizona/gen.py - non modificare a mano.',
     '// Mapping del mondo Arizona Prime (v2), solo Los Santos e interni degli edifici di Los Santos.',
     '// %d oggetti (%d copie identiche del server di origine tolte), %d edifici GTA rimossi. Richiede il plugin streamer.' % (len(mapping), MAP_DUP, len(removed)), '',
     '#define AZMAP_OBJECTS (%d)' % len(mapping), '']
CH = 500    # pezzi da 500 oggetti: uno per tick di avvio (core/boot.inc), nessuna callback lunga
parts = [mapping[i:i + CH] for i in range(0, len(mapping), CH)]
for pi, part in enumerate(parts):
    L.append('static AzMap_Part%d()' % pi)
    L.append('{')
    if any(o['mat'] or o['mattext'] for o in part):
        L.append('\tnew STREAMER_TAG_OBJECT:o;')
    for o in part:
        dist = fl(min(max(o.get('dist', 300.0), 50.0), 600.0))
        call = 'CreateDynamicObject(%d, %s, %s, %s, %s, %s, %s, %d, %d, -1, %s, %s)' % (
            o['model'], fl(o['pos'][0]), fl(o['pos'][1]), fl(o['pos'][2]), fl(o['rot'][0]), fl(o['rot'][1]), fl(o['rot'][2]),
            o['world'], o['interior'], dist, dist)
        if o['mat'] or o['mattext']:
            L.append('\to = ' + call + ';')
            for m in o['mat']:
                if len(m) < 4: continue
                color = m[4] if len(m) > 4 else '0'
                L.append('\tSetDynamicObjectMaterial(o, %s, %s, %s, %s, %s);' % (m[0], m[1], pstr(m[2].strip('"')), pstr(m[3].strip('"')), color))
            for m in o['mattext']:
                a = (m + ['0'] * 10)[:9]
                txt = pstr(a[1].strip('"').replace('\\n', '\n')).replace('\n', '\\n')
                L.append('\tSetDynamicObjectMaterialText(o, %s, %s, %s, %s, %s, %s, %s, %s, %s);' % (
                    a[0], txt, a[2] if re.fullmatch(r'-?\d+', a[2]) else '90', pstr(a[3].strip('"')), a[4], a[5], a[6], a[7], a[8]))
        else:
            L.append('\t' + call + ';')
    L.append('}')
    L.append('')
L.append('#define AZMAP_PARTS (%d)' % len(parts))
L.append('')
L.append('// crea il pezzo p (0 .. AZMAP_PARTS-1) del mapping; chiamata a tappe dall\'avvio')
L.append('AzMap_CreatePart(p)')
L.append('{')
L.append('\tswitch (p)')
L.append('\t{')
for pi in range(len(parts)):
    L.append('\t\tcase %d: AzMap_Part%d();' % (pi, pi))
L.append('\t}')
L.append('\tif (p == 0)')
L.append('\t{')
L.append('\t\tStreamer_SetVisibleItems(STREAMER_TYPE_OBJECT, 600);   // 0.3.7 + CEF nello stesso processo: memoria del client limitata')
L.append('\t}')
L.append('\tif (p == AZMAP_PARTS - 1)')
L.append('\t{')
L.append('\t\tprintf("[MAPPA] Arizona Los Santos: %d oggetti", AZMAP_OBJECTS);')
L.append('\t}')
L.append('}')
L.append('')
L.append('// edifici originali di GTA da togliere dove c\'e\' il mapping (va chiamata a ogni connessione)')
L.append('AzMap_RemoveBuildings(playerid)')
L.append('{')
for r in removed:
    L.append('\tRemoveBuildingForPlayer(playerid, %d, %s, %s, %s, %s);' % (r['model'], fl(r['pos'][0]), fl(r['pos'][1]), fl(r['pos'][2]), fl(r['radius'])))
L.append('}')
open(os.path.join(ROOT, 'gamemodes', 'modules', 'world', 'az_map.inc'), 'w').write('\n'.join(L) + '\n')

# ------------------------------------------------------------------ porte, business, veicoli
KA_TP = {'Больница LS': 'საავადმყოფო', 'Мэрия': 'მერია', 'Выход на улицу': 'ქუჩაში გასვლა', 'Army LS': 'არმია (LS)',
         'Полицейский участок LS': 'პოლიციის განყოფილება', 'Выход в гараж': 'ავტოფარეხში გასვლა', 'Sport Club LS': 'სპორტული კლუბი',
         'Los Santos': 'ქუჩაში გასვლა', 'Завод Армия LS': 'არმიის ქარხანა', 'Выход': 'გასასვლელი', 'Radio студия LS': 'რადიო სტუდია',
         'Наркопритон': 'ნარკობუნაგი', 'The Ghetto': 'ქუჩაში გასვლა', 'Крыша': 'სახურავი', 'Вход': 'შესასვლელი'}
KA_BIZ = {'Магазин 24/7': ('მაღაზია 24/7', 1), 'Закусочная': ('სასადილო', 2), 'Бар': ('ბარი', 3), 'Аренда транспорта': ('ტრანსპორტის ქირაობა', 4),
          'Амуниция/Тир': ('იარაღის მაღაზია', 5), 'Магазин одежды': ('ტანსაცმლის მაღაზია', 6), 'АЗС/Магазин механики': ('ბენზინგასამართი', 7),
          'Магазин аксессуаров': ('აქსესუარების მაღაზია', 8), 'Тюнинг салон': ('ტიუნინგ სალონი', 9), 'Концертный зал': ('საკონცერტო დარბაზი', 10),
          'Магазин Тюнинга LS': ('ტიუნინგის მაღაზია', 9)}
def ka_name(ru):
    return KA_TP.get(ru, ru)   # nomi delle gang restano latini

tps = load('teleports'); biz = load('businesses'); veh = load('vehicles'); parked = load('parked_vehicles')
D = ['// GENERATO da recovery/tools/arizona/gen.py - non modificare a mano.',
     '// Porte degli edifici, business e veicoli del mondo Arizona (solo Los Santos).', '']
D.append('#define AZ_DOORS (%d)' % len(tps))
D.append('// nome, x, y, z, mondo, interno, -> x, y, z, angolo, interno, mondo')
D.append('new const s_AzDoorName[AZ_DOORS][96] = {')
D.append(',\n'.join('\t"%s"' % ka_name(t['name_ru']) for t in tps))
D.append('};')
D.append('new const Float:s_AzDoorPos[AZ_DOORS][7] = {')
D.append(',\n'.join('\t{%s, %s, %s, %s, %s, %s, %s}' % (fl(t['enter'][0]), fl(t['enter'][1]), fl(t['enter'][2]), fl(t['exit'][0]), fl(t['exit'][1]), fl(t['exit'][2]), fl(t['exit_a'])) for t in tps))
D.append('};')
D.append('new const s_AzDoorWI[AZ_DOORS][4] = {   // mondo/interno di ingresso, interno/mondo di arrivo')
D.append(',\n'.join('\t{%d, %d, %d, %d}' % (t['enter_world'], t['enter_int'], t['exit_int'], t['exit_world']) for t in tps))
D.append('};')
D.append('')
D.append('#define AZ_BIZ (%d)' % len(biz))
D.append('new const s_AzBizName[AZ_BIZ][96] = {')
D.append(',\n'.join('\t"%s"' % KA_BIZ.get(b['name_ru'], (b['name_ru'], 0))[0] for b in biz))
D.append('};')
D.append('stock const s_AzBizType[AZ_BIZ] = {' + ', '.join(str(KA_BIZ.get(b['name_ru'], ('', 0))[1]) for b in biz) + '};')
D.append('new const s_AzBizInt[AZ_BIZ] = {' + ', '.join(str(b['interior']) for b in biz) + '};')
D.append('// prezzo di gioco in tetri per tipo (per una futura proprieta\' dei business; quello di origine resta in world/arizona/businesses.json)')
D.append('stock const s_AzBizPrice[AZ_BIZ] = {' + ', '.join(str(GEL_BIZ_TYPE.get(KA_BIZ.get(b['name_ru'], ('', 0))[1], 15000000)) for b in biz) + '};')
D.append('new const Float:s_AzBizPos[AZ_BIZ][6] = {')
D.append(',\n'.join('\t{%s, %s, %s, %s, %s, %s}' % tuple(fl(v) for v in b['enter'] + b['exit']) for b in biz))
D.append('};')
D.append('')
# veicoli: tipo 1 fazione, 2 lavoro, 3 motorino libero, 4 auto parcheggiata (chiusa)
KIND = {'faction': 1, 'job': 2, 'rent': 3}
allv = [(KIND[v['kind']], v) for v in veh] + [(4, v) for v in parked]
D.append('#define AZ_VEH (%d)' % len(allv))
D.append('// modello, colore1, colore2, tipo (1 fazione, 2 lavoro, 3 motorino, 4 parcheggiata), gruppo (fazione/lavoro di Arizona)')
D.append('new const s_AzVehInfo[AZ_VEH][5] = {')
tid = load('team_ids'); jid = load('job_ids')
def grp(kind, v):
    g = v.get('group', '')
    if kind == 1: return tid.get(g, 0)
    if kind == 2: return jid.get(g, int(g) if g.isdigit() else 0)
    return 0
D.append(',\n'.join('\t{%d, %d, %d, %d, %d}' % (v['model'], v['colors'][0], v['colors'][1], k, grp(k, v)) for k, v in allv))
D.append('};')
D.append('new const Float:s_AzVehPos[AZ_VEH][4] = {')
D.append(',\n'.join('\t{%s, %s, %s, %s}' % tuple(fl(x) for x in v['pos']) for k, v in allv))
D.append('};')
# ------------------------------------------------------------------ GPS
# destinazioni del GPS di Arizona (world/arizona/gps.json, estratto da extract_gps.py) e porte pubbliche, scelte a mano:
# solo luoghi fisici di Los Santos; esclusi i punti dei sistemi di Arizona che da noi non esistono (mercato nero, lotteria, lavori assenti).
GPS_AZ = [('[LS] Вокзал', 'რკინიგზის სადგური Unity'), ('[LS] Центральный банк', 'ცენტრალური ბანკი'),
          ('[LS] Пирс', 'სანტა-მარიას პირსი'), ('[LS] Колесо обозрения', 'ეშმაკის ბორბალი'),
          ('[LS] Центральный рынок', 'ცენტრალური ბაზარი'), ('[LS] Концертный ЗАЛ', 'საკონცერტო დარბაზი'),
          ('[LS] Свалка', 'ნაგავსაყრელი'), ('[LS] Лодочная', 'ნავსადგური'),
          ('[LS] Салон TransFender', 'ტიუნინგ სალონი TransFender'), ('[LS] Loco Low Co.', 'ტიუნინგ სალონი Loco Low Co.')]
GPS_DOOR = [(1, 'მერია'), (0, 'საავადმყოფო All Saints'), (5, 'პოლიციის განყოფილება'), (9, 'სპორტული კლუბი'), (13, 'რადიო სტუდია')]
gps_src = {g['name_ru']: g for g in load('gps')}
gps = [(ka, tps[i]['enter']) for i, ka in GPS_DOOR] + [(ka, gps_src[ru]['pos']) for ru, ka in GPS_AZ]
D.append('// GPS: luoghi importanti di Los Santos (porte pubbliche + destinazioni del GPS di origine)')
D.append('#define AZ_GPS (%d)' % len(gps))
D.append('new const s_AzGpsName[AZ_GPS][96] = {')
D.append(',\n'.join('\t"%s"' % ka for ka, _ in gps))
D.append('};')
D.append('new const Float:s_AzGpsPos[AZ_GPS][3] = {')
D.append(',\n'.join('\t{%s, %s, %s}' % tuple(fl(v) for v in p) for _, p in gps))
D.append('};')
D.append('')
open(os.path.join(ROOT, 'gamemodes', 'modules', 'world', 'az_data.inc'), 'w').write('\n'.join(D) + '\n')

# ------------------------------------------------------------------ case -> SQL
houses_src = load('houses')
# case con la stessa identica entrata di una precedente (doppioni del server di origine): si tiene la prima
houses = []; _doors = set()
for _h in houses_src:
    _k = tuple(round(c, 2) for c in _h['enter'])
    if _k not in _doors:
        _doors.add(_k); houses.append(_h)
def tier(h):
    c = h['class']
    return 0 if c <= 1 and h['cost'] < 1000000 else (1 if c <= 1 else (2 if c == 2 else 3))
S = ['-- 015_arizona_houses.sql - case del mondo Arizona (solo Los Santos), GENERATO da recovery/tools/arizona/gen.py.',
     '-- Idempotente: si puo\' rilanciare. Aggiunge alla tabella houses il punto interno personalizzato e l\'origine.',
     '-- Tutte le case sono SENZA proprietario (in vendita). Prezzi in tetri (lari x 100) dell\'economia GEL; livelli come nel server di origine.',
     "SET @c := (SELECT COUNT(*) FROM information_schema.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'houses' AND COLUMN_NAME = 'int_game');",
     "SET @q := IF(@c = 0, 'ALTER TABLE houses ADD COLUMN int_game TINYINT NULL DEFAULT NULL, ADD COLUMN int_x FLOAT NULL DEFAULT NULL, ADD COLUMN int_y FLOAT NULL DEFAULT NULL, ADD COLUMN int_z FLOAT NULL DEFAULT NULL, ADD COLUMN src VARCHAR(16) NULL DEFAULT NULL, ADD COLUMN src_id INT NULL DEFAULT NULL, ADD COLUMN min_level SMALLINT UNSIGNED NOT NULL DEFAULT 0, ADD UNIQUE KEY uq_houses_src (src, src_id)', 'SELECT 1');",
     'PREPARE st FROM @q; EXECUTE st; DEALLOCATE PREPARE st;',
     '-- Su un database nuovo gli id 1-10 restano alle 10 case di base del gamemode (le crea il gamemode al primo avvio):',
     '-- senza questa riga le case Arizona prenderebbero gli id 1-10 e il gamemode le sovrascriverebbe. Su una tabella gia piena non cambia nulla.',
     'ALTER TABLE houses AUTO_INCREMENT = 11;',
     'INSERT IGNORE INTO houses (owner_id, tier, interior_idx, ext_x, ext_y, ext_z, ext_a, price, locked, int_game, int_x, int_y, int_z, src, src_id, min_level) VALUES']
vals = []
for h in houses:
    vals.append("(NULL, %d, 0, %s, %s, %s, 0, %d, 1, %d, %s, %s, %s, 'arizona', %d, %d)" % (
        tier(h), fl(h['enter'][0]), fl(h['enter'][1]), fl(h['enter'][2]), gel_price(h), h['interior'], fl(h['exit'][0]), fl(h['exit'][1]), fl(h['exit'][2]), h['src_id'], h['level']))
S.append(',\n'.join(vals) + ';')
S.append('INSERT IGNORE INTO house_utilities (house_id) SELECT id FROM houses;')
S.append("INSERT IGNORE INTO schema_migrations (name) VALUES ('015_arizona_houses');")
open(os.path.join(ROOT, 'sql', 'migrations', '015_arizona_houses.sql'), 'w').write('\n'.join(S) + '\n')
json.dump([{'src_id': h['src_id'], 'class': h['class'], 'zone': ls_zone(h['enter'][0], h['enter'][1]), 'garage': bool(h.get('garage')), 'price_src': h['cost'], 'price_tetri': gel_price(h)} for h in houses],
          open(os.path.join(ROOT, 'world', 'arizona', 'house_prices_gel.json'), 'w'), indent=0)

# 016: riparazione dei database installati con la prima versione di lsrp_completo.sql, dove le case Arizona avevano gli id 1-10
# e il gamemode le sovrascriveva con le sue 10 case di base. Rimette entrata, prezzo e classe del server di origine alle case
# Arizona SENZA proprietario. Idempotente; su un database corretto non cambia nulla.
F = ['-- 016_arizona_houses_fix.sql - GENERATO da recovery/tools/arizona/gen.py. Idempotente, non cancella nulla.',
     '-- Ripara le case Arizona sovrascritte (id 1-10) quando il database era stato creato con la prima versione di lsrp_completo.sql.',
     '-- Tocca solo case Arizona senza proprietario; rimette entrata, classe e prezzo del server di origine.']
for h in houses:
    F.append("UPDATE houses SET tier = %d, interior_idx = 0, ext_x = %s, ext_y = %s, ext_z = %s, ext_a = 0, price = %d WHERE src = 'arizona' AND src_id = %d AND owner_id IS NULL;" % (
        tier(h), fl(h['enter'][0]), fl(h['enter'][1]), fl(h['enter'][2]), gel_price(h), h['src_id']))
F.append("INSERT IGNORE INTO schema_migrations (name) VALUES ('016_arizona_houses_fix');")
open(os.path.join(ROOT, 'sql', 'migrations', '016_arizona_houses_fix.sql'), 'w').write('\n'.join(F) + '\n')
print('mapping', len(mapping), 'parti', len(parts), 'porte', len(tps), 'business', len(biz), 'veicoli', len(allv), 'case', len(houses))
