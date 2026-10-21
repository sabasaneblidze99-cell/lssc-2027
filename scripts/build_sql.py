#!/usr/bin/env python3
"""Rigenera sql/lsrp_completo.sql (installazione nuova in un solo file) da sql/schema_full_v8.sql + sql/migrations/*.sql.
Non contiene utenti ne' password: l'utente si crea a parte con sql/crea_utente.sql."""
import glob, os

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SQL = os.path.join(ROOT, 'sql')
migs = sorted(glob.glob(os.path.join(SQL, 'migrations', '[0-9][0-9][0-9]_*.sql')))
names = [os.path.basename(m)[:-4] for m in migs]
out = [
    "-- LSSC 2027 - DATABASE COMPLETO IN UN SOLO FILE (installazione nuova). GENERATO da scripts/build_sql.py: non modificare a mano.",
    "-- = schema_full_v8.sql + migrazioni %s." % ', '.join(n.split('_')[0] for n in names),
    "-- phpMyAdmin: pagina iniziale (NON dentro un database) -> Importa -> questo file -> Esegui. Poi sql/crea_utente.sql.",
    "-- Testato su MySQL 8.0 (log binario attivo) e MariaDB 10.11. Se il database lsrp esiste gia': BACKUP, poi solo le migrazioni mancanti.",
    "",
]
out.append(open(os.path.join(SQL, 'schema_full_v8.sql'), encoding='utf-8').read().rstrip('\n'))
for m, n in zip(migs, names):
    out.append('\n-- ================================================================ migrazione %s\nUSE lsrp;' % n)
    out.append(open(m, encoding='utf-8').read().rstrip('\n'))
open(os.path.join(SQL, 'lsrp_completo.sql'), 'w', encoding='utf-8').write('\n'.join(out) + '\n')
print('sql/lsrp_completo.sql: base + %d migrazioni' % len(migs))
