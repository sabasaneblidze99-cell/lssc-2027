#!/usr/bin/env python3
"""Genera le migrazioni dell'economia in lari dai valori di gamemodes/modules/economy/econ_config.inc
(unica fonte dei numeri) e da world/arizona/house_prices_gel.json (prezzi delle case Arizona, da gen.py).

  sql/migrations/017_economia_gel.sql      NON distruttiva: prezzi preconfigurati, config della citta', funzioni
                                           della storia, tabella economy_meta. Va anche in lsrp_completo.sql.
  sql/economia/018_conversione_saldi.sql   conversione dei SALDI esistenti (con backup). Solo con approvazione.
  sql/economia/018_rollback.sql            ripristino dei saldi dal backup di 018.
"""
import os, re, json

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
CFG = os.path.join(ROOT, 'gamemodes', 'modules', 'economy', 'econ_config.inc')
CUR = os.path.join(ROOT, 'gamemodes', 'modules', 'economy', 'currency.inc')

defs = {}
for path in (CUR, CFG):
    for m in re.finditer(r'^#define\s+(ECON_\w+)\s+\(([^)]*)\)', open(path, encoding='utf-8').read(), re.M):
        defs[m.group(1)] = m.group(2).strip()
def val(name):
    v = defs[name]
    if re.fullmatch(r'-?\d+', v):
        return int(v)
    return val(v)

# ---------------------------------------------------------------- 017
L = ['-- 017_economia_gel.sql - GENERATO da scripts/gen_econ_sql.py (valori: gamemodes/modules/economy/econ_config.inc).',
     '-- Economia in lari georgiani: tutti gli importi del database sono interi in TETRI (₾1 = 100).',
     '-- NON distruttiva: aggiorna solo prezzi preconfigurati (case senza proprietario), la configurazione della citta\' e le',
     '-- funzioni dei soldi iniziali. NON tocca i saldi dei giocatori (vedi sql/economia/018_conversione_saldi.sql). Idempotente.',
     '',
     'CREATE TABLE IF NOT EXISTS `economy_meta` (',
     '  `k` VARCHAR(32) NOT NULL,',
     '  `v` VARCHAR(64) NOT NULL,',
     '  `updated_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,',
     '  PRIMARY KEY (`k`)',
     ') ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;',
     "INSERT INTO economy_meta (k, v) VALUES ('currency', 'GEL'), ('unit', 'tetri'), ('unit_per_currency', '100'), ('model', 'ka-2026-v1')",
     '  ON DUPLICATE KEY UPDATE v = VALUES(v);',
     "INSERT IGNORE INTO economy_meta (k, v) VALUES ('balances', 'non convertiti');",
     '',
     '-- soldi iniziali (= ECON_START_TOTAL / ECON_START_CASH_M*)',
     'DELIMITER //',
     'DROP FUNCTION IF EXISTS story_start_total//',
     'CREATE FUNCTION story_start_total() RETURNS INT DETERMINISTIC',
     'BEGIN',
     '  RETURN %d;' % val('ECON_START_TOTAL'),
     'END//',
     'DROP FUNCTION IF EXISTS story_motive_cash//',
     'CREATE FUNCTION story_motive_cash(p_motive INT) RETURNS INT DETERMINISTIC',
     'BEGIN',
     '  RETURN CASE p_motive WHEN 2 THEN %d WHEN 3 THEN %d WHEN 4 THEN %d ELSE %d END;' % (
         val('ECON_START_CASH_M2'), val('ECON_START_CASH_M3'), val('ECON_START_CASH_M4'), val('ECON_START_CASH')),
     'END//',
     'DELIMITER ;',
     '',
     '-- configurazione della vita in citta\' (il gamemode la riscrive comunque a ogni avvio con gli stessi valori)']
ld = sorted((k[len('ECON_LD_'):], val(k)) for k in defs if k.startswith('ECON_LD_'))
L.append('INSERT INTO ld_config (k, v) VALUES ' + ', '.join("('%s', %d)" % kv for kv in ld) + ' ON DUPLICATE KEY UPDATE v = VALUES(v);')
L += ['',
      '-- le 10 case di base (id 1-10, senza origine) ancora in vendita: prezzo della loro classe',
      'UPDATE houses SET price = CASE tier WHEN 0 THEN %d WHEN 1 THEN %d WHEN 2 THEN %d ELSE %d END WHERE src IS NULL AND owner_id IS NULL;' % (
          val('ECON_HOUSE_T0'), val('ECON_HOUSE_T1'), val('ECON_HOUSE_T2'), val('ECON_HOUSE_T3')),
      '-- case Arizona ancora in vendita: prezzo di gioco in lari (classe, zona, garage; world/arizona/house_prices_gel.json)']
hp = json.load(open(os.path.join(ROOT, 'world', 'arizona', 'house_prices_gel.json')))
for h in hp:
    L.append("UPDATE houses SET price = %d WHERE src = 'arizona' AND src_id = %d AND owner_id IS NULL;" % (h['price_tetri'], h['src_id']))
L.append("INSERT IGNORE INTO schema_migrations (name) VALUES ('017_economia_gel');")
open(os.path.join(ROOT, 'sql', 'migrations', '017_economia_gel.sql'), 'w', encoding='utf-8').write('\n'.join(L) + '\n')

# ---------------------------------------------------------------- 018 (saldi, solo con approvazione)
os.makedirs(os.path.join(ROOT, 'sql', 'economia'), exist_ok=True)
cat = re.search(r'new g_VehCatModel\[13\] = \{([^}]*)\}', open(os.path.join(ROOT, 'gamemodes', 'modules', 'vehicle', 'vehicle.inc'), encoding='utf-8').read()).group(1)
models = [int(x) for x in cat.split(',')]
names = ['FAGGIO', 'PREVION', 'MANANA', 'VIRGO', 'GLENDALE', 'PRIMO', 'WILLARD', 'SADLER', 'PCJ600', 'BOBCAT', 'TAMPA', 'MESA', 'SENTINEL']
vcase = ' '.join('WHEN %d THEN %d' % (m, val('ECON_VEH_' + n)) for m, n in zip(models, names))
R = ['-- 018_conversione_saldi.sql - GENERATO da scripts/gen_econ_sql.py. DA APPLICARE SOLO CON APPROVAZIONE, dopo 017 e dopo un backup.',
     '-- Converte i saldi esistenti dai "$" del vecchio gamemode ai TETRI. Non e\' un cambio valuta reale: e\' il rapporto tra',
     '-- il potere d\'acquisto nel vecchio gioco e nel nuovo (vecchio ~$3.000/h di gioco, nuovo ~₾400/h => 1 $ = 0,12 ₾ = 12 tetri).',
     '-- Cambia il rapporto qui sotto se decidi diversamente (0 = tutti i personaggi ripartono dai soldi iniziali).',
     'SET @tetri_per_dollaro := 12;',
     'SET @inizio := story_start_total();',
     '',
     "-- blocca una seconda esecuzione",
     "SET @gia := (SELECT COUNT(*) FROM economy_meta WHERE k = 'balances' AND v = 'convertiti');",
     "SET @q := IF(@gia > 0, 'SELECT ''saldi gia convertiti: niente da fare'' AS esito', 'SELECT ''conversione in corso'' AS esito');",
     'PREPARE st FROM @q; EXECUTE st; DEALLOCATE PREPARE st;',
     '',
     '-- backup (una tabella per esecuzione, mai sovrascritta)',
     'CREATE TABLE IF NOT EXISTS econ_backup_018_characters AS SELECT id, money, bank, NOW() AS saved_at FROM characters;',
     'CREATE TABLE IF NOT EXISTS econ_backup_018_houses AS SELECT id, price, price_paid, NOW() AS saved_at FROM houses;',
     'CREATE TABLE IF NOT EXISTS econ_backup_018_vehicles AS SELECT id, model, price_paid, NOW() AS saved_at FROM vehicles;',
     '',
     'START TRANSACTION;',
     '-- contanti e banca: rapporto di potere d\'acquisto, con tetto ₾10.000.000; con rapporto 0 si riparte dai soldi iniziali',
     'UPDATE characters SET',
     '  money = IF(@tetri_per_dollaro = 0, LEAST(%d, @inizio), LEAST(ROUND(money * @tetri_per_dollaro), %d)),' % (val('ECON_START_CASH'), val('ECON_MONEY_MAX')),
     '  bank  = IF(@tetri_per_dollaro = 0, GREATEST(@inizio - %d, 0), LEAST(ROUND(bank * @tetri_per_dollaro), %d))' % (val('ECON_START_CASH'), val('ECON_MONEY_MAX')),
     "WHERE @gia = 0;",
     '-- case gia\' possedute: listino in lari (come 017 per quelle in vendita) e prezzo pagato = listino (rimborso coerente)',
     'UPDATE houses SET price = CASE tier WHEN 0 THEN %d WHEN 1 THEN %d WHEN 2 THEN %d ELSE %d END WHERE src IS NULL AND owner_id IS NOT NULL AND @gia = 0;' % (val('ECON_HOUSE_T0'), val('ECON_HOUSE_T1'), val('ECON_HOUSE_T2'), val('ECON_HOUSE_T3')),
     ] + ["UPDATE houses SET price = %d WHERE src = 'arizona' AND src_id = %d AND owner_id IS NOT NULL AND @gia = 0;" % (h['price_tetri'], h['src_id']) for h in hp] + [
     'UPDATE houses SET price_paid = price WHERE owner_id IS NOT NULL AND @gia = 0;',
     '-- veicoli: prezzo pagato = listino in lari del modello (gli altri modelli: rapporto)',
     'UPDATE vehicles SET price_paid = CASE model %s ELSE LEAST(ROUND(price_paid * @tetri_per_dollaro), %d) END WHERE @gia = 0;' % (vcase, val('ECON_MONEY_MAX')),
     "INSERT INTO economy_meta (k, v) VALUES ('balances', 'convertiti') ON DUPLICATE KEY UPDATE v = IF(@gia = 0, 'convertiti', v);",
     "INSERT INTO economy_meta (k, v) VALUES ('balances_ratio', CAST(@tetri_per_dollaro AS CHAR)) ON DUPLICATE KEY UPDATE v = IF(@gia = 0, VALUES(v), v);",
     'COMMIT;',
     '-- verifica: nessun saldo negativo o oltre il tetto',
     'SELECT COUNT(*) AS personaggi, SUM(money < 0 OR bank < 0) AS negativi, SUM(money > %d OR bank > %d) AS oltre_tetto, ROUND(SUM(money + bank) / 100, 2) AS totale_lari FROM characters;' % (val('ECON_MONEY_MAX'), val('ECON_MONEY_MAX'))]
open(os.path.join(ROOT, 'sql', 'economia', '018_conversione_saldi.sql'), 'w', encoding='utf-8').write('\n'.join(R) + '\n')
B = ['-- 018_rollback.sql - GENERATO da scripts/gen_econ_sql.py. Rimette saldi e prezzi pagati com\'erano prima di 018.',
     'START TRANSACTION;',
     'UPDATE characters c JOIN econ_backup_018_characters b ON b.id = c.id SET c.money = b.money, c.bank = b.bank;',
     'UPDATE houses h JOIN econ_backup_018_houses b ON b.id = h.id SET h.price = IF(h.owner_id IS NOT NULL, b.price, h.price), h.price_paid = b.price_paid;',
     'UPDATE vehicles v JOIN econ_backup_018_vehicles b ON b.id = v.id SET v.price_paid = b.price_paid;',
     "UPDATE economy_meta SET v = 'non convertiti' WHERE k = 'balances';",
     'COMMIT;']
open(os.path.join(ROOT, 'sql', 'economia', '018_rollback.sql'), 'w', encoding='utf-8').write('\n'.join(B) + '\n')
print('017: %d chiavi ld_config, %d case Arizona; 018 + rollback in sql/economia/' % (len(ld), len(hp)))
