# LSSC 2027 — guida per sessioni di sviluppo

Gamemode Roleplay open.mp + MySQL (Los Santos 2027). Lingua del gioco: georgiano
(CEF in alfabeto georgiano, chat/dialog traslitterati in latino da `core/lang.inc`).
Report all'utente: italiano, conciso.

## Regola n.1: niente lavoro fuori da git
Il workspace cloud può essere azzerato. **Commit + push su GitHub dopo ogni passo
significativo** (repo `sabasaneblidze99-cell/lssc-2027`). Ogni pacchetto consegnato
all'utente deve includere anche il sorgente.

## Struttura
- `gamemodes/lsrp.pwn` — entry point: include + callback open.mp che delegano ai moduli.
- `gamemodes/modules/<area>/*.inc` — moduli (core, ui, database, account, character,
  economy, admin, vehicle, inventory, bank, intro, house, job, faction, living,
  district, director, rental, taxi, freight, cityos, hub, cef, commands).
- `gamemodes/modules/core/enums.inc` — tutti gli enum dati (E_PLAYER_DATA …).
- `gamemodes/modules/core/forwards.inc` — forward delle public.
- `cef/assets/` — interfaccia CEF (login, HUD, telefono, bacheca).
- `sql/` — schema MySQL completo + migrazioni. `sql/lsrp_completo.sql` e' GENERATO da `scripts/build_sql.py` (lo chiama build.sh):
  non modificarlo a mano. Nessuna password in sql/: l'utente si crea con `sql/crea_utente.sql` (segnaposto).
- `deploy/server/` — modelli di config.json (server privato) e mysql.ini.
- `docs/PRELAUNCH_REPORT.md`, `docs/PRELAUNCH_DEPENDENCIES.md` — audit pre-lancio, plugin con link ufficiali.
- `docs/` — Chapter Zero, localizzazione, piani.
- `toolchain/include/` — include usati per compilare.
- `recovery/tools/` — strumenti che hanno ricostruito il sorgente dalla v8.

## Compilazione
- `gamemodes/lsrp_completo.pwn` = tutto in un file (per Qawno), rigenerato da `scripts/build.sh`: va committato **a ogni aggiornamento** e messo in ogni pacchetto (l'utente lo vuole sempre).
- UN SOLO gamemode ufficiale: `lsrp.amx` (CEF). La voce e' nel filterscript `filterscripts/lsrp_voce.pwn` (SampVoice -omp): il gamemode non dipende da plugin opzionali. Niente varianti per l'utente.
- Linux: `PAWNCC=/percorso/pawncc scripts/build.sh`; `--test` = senza CEF per il server di prova (tests/server, con componente sampvoice).
- Windows: `compila.bat`.
- Compiler: pawncc 3.10.10, sempre con `-d3`. Provare SEMPRE anche con pawncc 3.10.11 (quello di Qawno/open.mp, piu severo: es. array const a parametro non const = errore), compilato in /home/claude/tools/omp-compiler (repo openmultiplayer/compiler). `LSRP_CEF=1` attiva il CEF; senza, gli
  stub in `ui/cef_ui.inc` rendono il gamemode autonomo (solo dialog/TextDraw).

## Stato del sorgente (ottobre 2026)
Il sorgente è stato ricostruito dal compilato v8: entrambe le build (CEF e senza CEF)
ricompilano in bytecode identico agli originali. Nomi dei campi enum ricostruiti
dall'uso; i numeri magici (ID dialog, colori, stati) sono ancora letterali: sostituirli
con `#define` quando si tocca un modulo.

## Principi
- Non riscrivere sistemi funzionanti; integrare.
- Operazioni sensibili validate lato server; DB async con op_id idempotenti.
- Nessuna modifica a DB o server di produzione senza autorizzazione.
- Credenziali mai in git; password mai nei log.
- Non dichiarare funzionante ciò che non è stato testato (il client GTA lo testa l'utente).

## Mondo Arizona (solo Los Santos)
- Dati in `world/arizona/*.json`, estratti da `recovery/tools/arizona/extract.py`; i moduli `gamemodes/modules/world/az_map.inc`,
  `az_data.inc` e `sql/migrations/015_arizona_houses.sql` sono GENERATI da `recovery/tools/arizona/gen.py` (non modificarli a mano).
- Solo dati del mondo (coordinate, interni, modelli, prezzi), nessun codice dei sistemi Arizona, nessun dato personale.
- Richiede il plugin streamer (include in toolchain/include/streamer).
