# LSSC 2027: runtime test environment results (2026-10-20)

Everything lives under `/home/claude/testenv`. The repo `/home/claude/lssc-2027` was only read: nothing was modified or committed.
The gamemode built here (`lsrp_senza_cef.amx`, pawncc 3.10.10 `-d3 LSRP_NO_CEF=1`) is **byte-identical** to the one in the repo.

## Summary

| Step | Result |
|---|---|
| 1. MariaDB 10.11 + schema_full_v8 + migrations 013/014 (applied twice) | OK. The only errors are 4 in the schema header (MySQL-8 `CREATE USER` syntax, see E1). The migrations are idempotent. |
| 2. open.mp 1.6.7.3218 + mysql R41-4 + samp_bcrypt 0.4.1 (32-bit) | OK. The server starts, the DB connects, the gamemode boots, there are no runtime errors and `logs/errors.log` is empty. |
| 3. Real headless SA-MP 0.3.7 client | **Not available**: no Linux client could be cloned (see "Client search"). |
| 3b. Fallback: open.mp NPC + harness filterscript | OK. Register, character creation (name, birth date, gender, skin), the story + intro path (with and without skip), login, the character list, spawn-select 9810, commands, the inventory dialogs and disconnect/save all work. |

## Reproduce

```bash
/home/claude/testenv/start_db.sh                 # private MariaDB (datadir db/data, socket db/mysqld.sock, 127.0.0.1:3306)
/home/claude/testenv/start_server.sh 30          # omp-server headless for 30 s (no argument = background); output in server/console.out
/home/claude/testenv/bot_smoke.sh --fresh        # NPC test: new account + character (intro skipped via flag)
/home/claude/testenv/bot_smoke.sh                # NPC test: login + character list + spawn-select 9810
/home/claude/testenv/bot_smoke.sh --fresh --intro  # as above but with the intro cinematic (removes lsrp_intro_skip.flag)
/home/claude/testenv/build_plugins.sh            # rebuild plugins + gamemode + harness (documents the patches)
```

Applying the SQL (already done):
```bash
S=/home/claude/testenv/db/mysqld.sock
mariadb --socket=$S -uroot --force < /home/claude/lssc-2027/sql/schema_full_v8.sql
# user created manually (see E1) with a random local test password, stored in /home/claude/testenv/.dbpass (chmod 600)
for f in /home/claude/lssc-2027/sql/migrations/*.sql; do mariadb --socket=$S -uroot --force lsrp < $f; done   # run twice
```
SQL outputs: `db/sqllogs/schema.out` and `db/sqllogs/migrations.out`.

Server layout (`server/`):
- `config.json` is based on `recovery/reference/config.json`. Changes: `main_scripts: ["lsrp_senza_cef 1"]`, a random RCON password (in `.rconpass`), the `cef` key removed, and `max_bots: 10`. `bot_smoke.sh` adds `side_scripts: ["filterscripts/bot_harness"]` only while the test runs.
- `mysql.ini` (hostname/username/password/database/server_port) sits in the server root, chmod 600.
- `plugins/mysql.so`, `plugins/samp_bcrypt.so`, plus `log-core.so` in the root (`LD_LIBRARY_PATH=server`).

## Building the 32-bit plugins (notes)

- **mysql R41-4**:
  - The `libs/cmake` submodule (Zeex/cmake-modules) can no longer be cloned (GitHub asks for credentials, so the repo was removed or made private). I replaced it with 3 minimal modules in `cmake-shim/`.
  - Ubuntu has no `libmariadb-dev:i386`, so I used `libmysqlclient-dev:i386` (MySQL 8.0.46 client) instead.
  - Patches to the plugin source (testenv only):
    - `my_bool` changed to `bool` in `src/CConnection.cpp:73`.
    - Computed-goto cast at `libs/samp-log-core/src/amx/amx.c:1714`.
  - The MySQL 8 client prints the harmless line `WARNING: MYSQL_OPT_RECONNECT is deprecated` 4 times at startup.
- **samp_bcrypt**:
  - `rustup target add i686-unknown-linux-gnu` fails because static.rust-lang.org is blocked by the proxy.
  - Built instead with the apt toolchain: `rustc/cargo 1.75` + `libstd-rust-dev:i386`, using `cargo build --release --target i686-unknown-linux-gnu`.
- **Other natives**: the gamemode only includes `open.mp`, `a_mysql` and `samp_bcrypt` (CEF/sampvoice are excluded in the no-CEF build). The server reported no missing natives.

## Server log (30 s headless boot)

```
[Info] Loading plugin: mysql
[Info]  >> plugin.mysql: R41-4 successfully loaded.
[Info] Loading plugin: samp_bcrypt
[Info] [SampBcrypt] [info]: Version: 0.4.1
WARNING: MYSQL_OPT_RECONNECT is deprecated and will be removed in a future version.   (x4, client library)
[Info] [DB] connesso (utf8mb4, via mysql_connect_file)
[Info] LSRP 0.1.0-proto
[Info] Legacy Network started on port 7777
[Info] [DISTRICT] boot living completato: 3 quartieri, 4 contratti per bucket
[Info] [HOUSE] caricate 10 case
```
- No `Run time error`, no missing natives, no query errors.
- `logs/errors.log` and `logs/warnings.log` (mysql plugin) are empty in every run.
- `logs/log-core.log` only contains `signal 2 (SIGINT) catched`, which is my Ctrl-C shutdown.

## NPC test: what was exercised

The harness is `harness/bot_harness.pwn`, built to `server/filterscripts/bot_harness.amx`. How it works:
- It creates the NPC `Test_Bot` with `NPC_Create`.
- It simulates the `OnPlayerRequestClass` that a real client sends. Without it `Account_ShowPrompt` never shows anything, because it waits for `pClassRequested`.
- It reads each dialog with `GetPlayerDialogID`/`GetPlayerDialogData` and answers by calling the gamemode's `OnDialogResponse` through `CallRemoteFunction`.
- After the spawn dialog it calls `NPC_Spawn`, then sends the commands, then `NPC_Destroy`, which triggers the disconnect.

Observed flow (from `runs_fresh.log`, `runs_login.log`):
```
dialog 9001 registracia -> password            [ACCOUNT] login account N (Test_Bot)   (bcrypt $2b$11$, hash_algo=1)
dialog 9006 name -> Test_Bot ; 9007 birth -> 14/05/1998 ; 9003 gender -> 0 ; 9004 skin -> 0
[CHAR] nuovo personaggio N (Test_Bot)          -> spawn (NPC_Spawn) -> OnPlayerSpawn
(with --intro: 9600/9601 story, 9602 confirm, cinematic in VW 20000+id, /salta accepted after ~15 s:
 "[INTRO] intro terminata (skip) per Test_Bot", accounts.intro_seen=1)
login: dialog 9002 -> password ; 9005 character list (slot 1 Test_Bot + 2 "new") -> 0 ; 9810 spawn-select (TABLIST) -> 0
OnPlayerCommandText /help,/stats,/me -> 1 ; /comandoinesistente -> 0 ; Cmd_inventario -> dialog 9400 -> 9401
disconnect -> characters.last_save updated
```
Rows written to the DB:
- `accounts`: bcrypt hash.
- `characters`: name, birth_date, phone 555000N, money 2500 / bank 5000, skin 7.
- `inventory_items` + `inventory_log`: ID card issued.
- `character_story` + `story_ops`: written when the intro runs.

**Limits of the NPC approach** (these are not gamemode bugs):
- The output of `SendClientMessage` to the NPC cannot be observed.
- `Util_IsPlaying()` returns 0 for NPCs (`gamemodes/modules/core/utils.inc:15`). Because of that, `Cmd_Dispatch` only answers "Devi aver effettuato l'accesso…" (I invoked the `Cmd_*` publics directly to compensate), and `played_ms` stays 0 (`economy/paycheck.inc:57`).
- `SpawnPlayer` has no effect on an NPC, and `NPC_Spawn` ignores `SetSpawnInfo`: the NPC appears at the class position 1481,-1749.5. At disconnect the saved position is `0,0,3.5`. This is almost certainly an NPC artifact; to be checked with the real client.

## Errors / anomalies found

**E1: schema_full_v8.sql does not run as-is on MariaDB** (the rest of the file is fine; with `--force` all tables and procedures get created):
```
ERROR 1064 (42000) at line 8: You have an error in your SQL syntax; ... near 'BY '<password di test>'' at line 1
ERROR 1064 (42000) at line 9: ... near 'BY '<password di test>''
ERROR 1133 (28000) at line 10: Can't find any matching row in the user table
ERROR 1133 (28000) at line 11: Can't find any matching row in the user table
```
- Location: `sql/schema_full_v8.sql:8-9`. `IDENTIFIED WITH mysql_native_password BY '...'` is MySQL 8 syntax. MariaDB needs `IDENTIFIED BY '...'` or `IDENTIFIED VIA mysql_native_password USING PASSWORD('...')`.
- Side issue: the file contains a test password that is committed to git.
- Without `--force`, the import stops at line 8.

**E2: the 9810 spawn-select text is cut in the middle of a Georgian character** (it renders as `�`):
```
dialog id=9810 ... body="{FFFFFF}bolo adgili|{AAAAAA}los-santosi - gamoxvedi 1 wutis �|{FFFFFF}qalaqi|{AAAAAA}Unity Station"
```
- Location: `gamemodes/modules/character/spawn_select.inc:312`, `new away[48]`.
- "გამოხვედი 1 წუთის წინ" takes about 49 bytes in UTF-8 (3 bytes per Georgian letter), so it does not fit. The "საათის"/"დღის" variants (`SpawnSel_Away`, lines 278-299) overflow too.
- `zone[40]` (line 312) and `sub[96]`, `line[180]` (lines 360, 373) are also tight for longer district names. Suggested fix: about 3x the buffers.

**E3: on open.mp the primary mysql.ini reader never finds the file in the server root**
- Location: `gamemodes/modules/database/db_core.inc:174,177`.
- Pawn `fopen` resolves paths relative to `scriptfiles/`, and `"../mysql.ini"` is rejected. So `DB_IniRead` fails and the code falls back to `mysql_connect_file` (line 181), which works but skips the port option and the `DB_ConnectFinish` diagnostics.
- Verified: with a copy at `scriptfiles/mysql.ini` the log prints `[DB] connesso (utf8mb4)` instead of `... via mysql_connect_file`.
- The messages on lines 190-191 ("atteso nella stessa cartella di omp-server") are therefore misleading.

**E4 (observation): `characters.last_played_at` is never written.** There is no query in `gamemodes/` that references it, so it stays NULL after each session. Is this intended?

**Other notes**
- `schema_migrations` stays empty: neither the schema nor 013/014 record their name in it.
- 014 is already a no-op on a fresh v8 schema (`combat_logout gia presente`).
- pawncc reports 3363 warnings; this is known and does not block anything.

## Client search (step 3)

- Reachable via `git ls-remote`:
  - `enginestein/SAMP.py`, `samp-client`/`samp-python`/`samp-query` (pip): query/RCON only, no game protocol.
  - `Cheaterman/PySAMP`, `pysamp/PySAMP`: server-side Python scripting.
  - `Brunoo16/samp-packet-list`: documentation only.
  - `openmultiplayer/open.mp`.
- Not clonable (anonymous HTTPS asks for credentials, so they are missing/private): `P3ti/RakSAMP`, `BlastHackNet/RakSAMP`, `RD42/RakSAMP-Lite`, `povargek/RakSAMP-Lite`, `kalcor/samp-npc`, plus a few other guesses.
- `gh search` is blocked (the session is restricted to its configured repos). `samp-npc` from sa-mp.com cannot be downloaded (only registries + GitHub are allowed).
- Writing a client from scratch (RakNet 2.x + SA-MP encryption + auth) was judged out of scope, so I used the open.mp NPC component, which works.

## Files

- `start_db.sh`, `start_server.sh`, `bot_smoke.sh`, `build_plugins.sh`: scripts.
- `harness/bot_harness.pwn`: NPC harness.
- `cmake-shim/`: replacement cmake modules.
- `server/`: ready-to-run open.mp server.
- `runs_fresh.log`, `runs_login.log`: logs of the last runs.
- `db/`: MariaDB datadir and logs.
- `.dbpass`, `.rconpass`: local test credentials, chmod 600.

## Aggiornamento (dopo le correzioni)
- E2 corretto (buffer dei testi georgiani in spawn_select/char_select/street_rental): dialog 9810 ora completo.
- E4 corretto: `last_played_at` scritto a ogni salvataggio.
- E3: messaggio d'errore chiarito (mysql.ini nella radice o in scriptfiles/); la connessione via mysql_connect_file funziona.
- E1 non toccato: lo schema e' scritto per MySQL 8 (phpMyAdmin dell'utente); su MariaDB creare l'utente a mano con `IDENTIFIED BY`.
- Ripetuto: registrazione -> nome 9006 -> nascita 9007 -> genere -> skin -> spawn; secondo accesso: lista 9005 -> spawn 9810 -> comandi. Nessun errore nei log.
