# LS Crime's: audit pre-lancio

Data: 21 ottobre 2026. Branch `prelaunch`. Punto di ripristino: `main` al commit `6aaec52`, cioè lo stato prima dell'audit.

Questo audit **non** ha avviato il server di gioco né fatto test in gioco. Tutto quello che riguarda il rendering, il client o la posizione in gioco è marcato come da verificare.

## A. Executive summary

**Stato generale.** Il gamemode (`lsrp.amx`, con CEF) compila con 0 errori e 0 warning con pawncc 3.10.10 e 3.10.11, sia dai moduli sia dal file unico per Qawno, e i due producono lo stesso bytecode. Il database si installa da zero in un solo file su MySQL 8.0 e su MariaDB 10.11. Tutte le 156 query del gamemode sono state controllate contro lo schema reale con un utente a permessi minimi.

**Problemi trovati e risolti (10):**

1. Su un database nuovo, le prime 10 case Arizona venivano sovrascritte dalle case di base del gamemode: entrata spostata, interno sbagliato.
2. Il file unico `lsrp_completo.pwn` non compilava in Qawno (`error 075`), perché gli escape del georgiano rendono le righe troppo lunghe.
3. La password di test era scritta nello schema SQL del repository.
4. Una query (valutazioni taxi) dava errore 1054 su MariaDB.
5. 8 business senza interno teletrasportavano il giocatore a 0,0,0.
6. Il mapping conteneva 801 oggetti copiati due volte.
7. Due case Arizona avevano la stessa porta.
8. La password del login CEF veniva cancellata dalla memoria solo in parte.
9. Il fumetto di `/ame` mostrava caratteri rotti con il georgiano della chat CEF.
10. Il modello di `config.json` non conteneva `streamer`, senza il quale il gamemode non si carica.

**Ancora aperti.** Il GPS non esiste. I garage e la gestione dei business non sono implementati. Va sostituita la `streamer.dll` 2.9.4, di origine non verificata, con la 2.9.6 ufficiale. Il CEF client e il server devono venire dallo stesso commit. Restano da decidere i comandi in italiano e la sovrapposizione tra la casa di base 4 e la casa Arizona 86. Tutti i dettagli sono nella sezione H.

**Prontezza.** Il progetto è pronto per un **primo test isolato e controllato** di registrazione, login, personaggi, spawn, HUD CEF, case, interni, veicoli, salvataggio e riconnessione. **Non** è pronto per il test del GPS, che manca, né per un'apertura pubblica: mancano un anti-cheat e un RCON sicuro sul server reale.

### Readiness gates

| Gate | Stato | Motivo |
|---|---|---|
| 1. Integrità del progetto | PASS | Git pulito. Branch `prelaunch` su GitHub, `main` invariato come punto di ripristino. Backup del database di sviluppo e delle configurazioni. Il tag `pre-audit-6aaec52` non è stato pubblicato (il proxy ha rifiutato il push dei tag), ma esiste in locale e `main` coincide. |
| 2. Dipendenze | NOT VERIFIED | Tutte le dipendenze sono individuate dalle native compilate e i link sono verificati. I binari Windows sul PC dell'utente non si possono ispezionare da qui: versione di bcrypt sconosciuta, streamer 2.9.4 da aggiornare, cef.dll compilata dall'utente. |
| 3. Build | PASS | 0 errori e 0 warning: 3.10.10 e 3.10.11, `lsrp.pwn`, `lsrp_completo.pwn`, `lsrp_test`, `lsrp_voce`. |
| 4. Database | PASS | Import da zero su MySQL 8.0.46 (log binario attivo) e MariaDB 10.11.14. 156 query preparate, 36 CALL con il numero giusto di argomenti, 218 letture di colonne tutte presenti, 47 INSERT con le colonne obbligatorie. |
| 5. Integrazione dei sistemi | BLOCKED | Account, personaggi, spawn, inventario, banca, case, veicoli, lavori e fazioni sono collegati e controllati. **GPS assente**. Garage e gestione dei business non implementati. |
| 6. World data | BLOCKED | Case, porte, business, veicoli e mapping hanno loader e relazioni coerenti (controllo statico). Il **GPS** non ha né dati né loader. |
| 7. CEF | PASS (statico) | 69 eventi server→pagina, tutti con un listener; 25 eventi pagina→server, tutti sottoscritti. Font georgiano su ogni testo. Il rendering in gioco è NOT VERIFIED. |
| 8. Sicurezza | PASS (per un test isolato) | Nessuna SQL injection trovata e transazioni lato server con op_id. Restano senza anti-cheat e con l'RCON dell'utente da cambiare: due punti da chiudere prima dell'apertura pubblica. |
| 9. Deployment | PASS | Modelli di `config.json` (server privato) e `mysql.ini`. Utente SQL con permessi minimi. Log del server, del plugin MySQL e del client CEF (`cef_client.log`). |

## B. Plugin report

| Plugin | Serve? | Installato | Versione | Compatibile | Problema | Link ufficiale | Azione |
|---|---|---|---|---|---|---|---|
| open.mp server | sì | sì (utente) | 1.6.7.3218 | sì | — | https://github.com/openmultiplayer/open.mp/releases/tag/v1.6.7.3218 | READY |
| MySQL R41-4 | sì (17 native) | sì | R41-4 | sì, include R41-4 | — | https://github.com/pBlueG/SA-MP-MySQL/releases/tag/R41-4 | READY |
| samp-bcrypt | sì (3 native) | sì | sconosciuta | funziona nei test dell'utente | versione non letta | https://github.com/Sreyas-Sreelal/samp-bcrypt/releases/tag/0.4.1 | VERSION UNKNOWN |
| Streamer | sì (4 native, 14.956 oggetti) | dll 2.9.4 del donatore | 2.9.4 contro l'include 2.9.6 | le native ci sono | dll di fonte non verificata, versione diversa dall'include | https://github.com/samp-incognito/samp-streamer-plugin/releases/tag/v2.9.6 | OUTDATED BUT WORKING → sostituire |
| samp-cef (componente open.mp) | sì (7 native) | build dell'utente | commit sconosciuto | include identico al sorgente f14fe4f | la release v1.4.1 non contiene il componente open.mp | https://github.com/ZOTTCE/samp-cef | REQUIRES MANUAL INSTALLATION |
| SampVoice -omp | no (solo il filterscript della voce) | build Linux di prova | omp | sì | — | https://github.com/AmyrAhmady/sampvoice/releases/tag/v3.2.0-omp | OPZIONALE |
| sscanf, Pawn.CMD, Pawn.RakNet, YSF, Nex-AC, FCNPC, MapAndreas, TOTP, Weapon Config, YSI, PawnPlus | no | — | — | — | nessuna nativa usata | — | NOT NEEDED |
| Crashdetect | consigliato per il debug | no | — | da verificare con open.mp | — | https://github.com/Zeex/samp-plugin-crashdetect | facoltativo |

Dettagli completi (cartelle, librerie runtime, porte, verifica) in `docs/PRELAUNCH_DEPENDENCIES.md`.

## C. Compilation report

- **Compilatori**: pawncc 3.10.10 (Zeex) e 3.10.11 (open.mp, quello di Qawno), sempre con `-d3`. Include in `toolchain/include/`.
- **Errore iniziale trovato**: `lsrp_completo.pwn(35253) : error 075: input line too long`. Le tabelle con i nomi georgiani dei quartieri superavano i 4.000 caratteri per riga una volta convertite in `\xHH;`. Correzione in `scripts/flatten.py`: le righe sopra i 2.000 caratteri vengono spezzate dopo le virgole fuori dalle stringhe. Risultato: stesso codice e stessi dati di `lsrp.amx` (confronto delle sezioni del file .amx).
- **Finale**: 0 errori e 0 warning con entrambi i compilatori per `lsrp.amx` (CEF), `lsrp_test.amx` (senza CEF), `lsrp_completo.pwn` e `filterscripts/lsrp_voce.amx`. Nessun warning soppresso: le direttive `#pragma unused` già presenti riguardano solo parametri davvero inutilizzati.
- **Native importate**: 184 in `lsrp.amx` (152 open.mp, 17 MySQL, 7 CEF, 4 streamer, 3 bcrypt, 1 Create3DTextLabel) e 15 in `lsrp_voce.amx` (10 SampVoice). Nessuna nativa senza plugin.

## D. Database report

- **Schema**: 35 tabelle InnoDB, tutte `utf8mb4_unicode_ci`; 68 tra procedure e funzioni; 28 foreign key; ogni tabella ha una chiave primaria. Le funzioni dichiarano `READS SQL DATA` o `DETERMINISTIC`, quindi l'import funziona anche con il log binario attivo (MySQL 8 di default).
- **Installazione nuova**: `sql/lsrp_completo.sql`, rigenerato da `scripts/build_sql.py` (base v8 + migrazioni 013–016), seguito da `sql/crea_utente.sql`. Non contiene più password. L'utente ha solo SELECT, INSERT, UPDATE, DELETE, EXECUTE, CREATE TEMPORARY TABLES e LOCK TABLES su `lsrp.*`, e le 156 query sono state provate proprio con questo utente.
- **Migrazioni registrate** in `schema_migrations`: schema_full_v8, 012, 013, 014, 015, 016.
- **Loader collegati**: account (`DB_OnAccountLoaded`), personaggi (`DB_OnCharLoaded`, lista di 3), case (`DB_OnHousesLoaded`, LIMIT 400; 10 di base + 336 Arizona = 346), veicoli dei giocatori (`db_vehicle`), inventario e banca (procedure con op_id idempotenti), lavori e fazioni (`db_jobfac`), vita in città e contratti (`db_living`), storia (`db_story`), admin e ban (`db_admin`).
- **Callback async**: 52 callback che ricevono `playerid`. Tutte controllano che il giocatore sia ancora lo stesso, tramite nome e token o generazione, prima di usare i dati: nessuna corsa dopo una disconnessione.
- **Problemi corretti**:
  - `db_house.inc` (seme delle case di base): `ON DUPLICATE KEY UPDATE` → `INSERT IGNORE`. Su un database nuovo sovrascriveva le case Arizona 1–10.
  - `015` / `gen.py`: `ALTER TABLE houses AUTO_INCREMENT = 11` prima delle case Arizona.
  - Nuova `016_arizona_houses_fix.sql`: ripara i database creati con la versione precedente di `lsrp_completo.sql`. Provata su una copia danneggiata: 0 differenze dopo l'import.
  - `db_living.inc` (riga 737): la media delle ultime 20 valutazioni taxi usava una tabella derivata con riferimento esterno, che dà l'errore 1054 su MariaDB. Riscritta con una sottoquery `LIMIT 1 OFFSET 19`: stessi risultati su MySQL 8 e MariaDB.
- **Residui**: nessuno bloccante. Il messaggio "combat_logout gia presente" che appare durante l'import è informativo.

## E. System integration report

| Collegamento | Stato | Note |
|---|---|---|
| Account → Login → Personaggi → Spawn | OK (statico + test NPC del 21/10 su 6aaec52) | bcrypt, blocco dopo password errate, 3 personaggi, scelta dello spawn, regola delle 3 ore. |
| Personaggio → Inventario → Oggetti → DB | OK | Procedure `inv_*` con op_id. |
| Personaggio → Soldi → Banca → Transazioni | OK | Procedure `bank_*` con limiti giornalieri e op_id; estratto conto in CEF non collegato (`lssc:bank_row`). |
| Personaggio → Veicolo → Proprietà → Garage | INCOMPLETE | Veicoli personali salvati. **Nessun sistema di garage**: 86 case Arizona hanno dati di garage non usati. |
| Personaggio → Casa → Entrata → Interno | OK | Mondo virtuale `1000 + id`. Le case Arizona usano l'interno originale (`int_game`, `int_x/y/z`). |
| Personaggio → Lavoro → Checkpoint → Paga | OK | Job Center, checkpoint, paycheck; veicoli Arizona dei lavori netturbino e pizza collegati. |
| Personaggio → GPS → Destinazione → Checkpoint | **BLOCKED** | Il GPS non esiste. `teleports.json` (41 edifici) e i business potrebbero fare da destinazioni. |
| Fazione → Gradi → Permessi → Veicoli | OK per LSPD ed EMS | Gli altri veicoli di fazione del donatore (esercito, gang, radio, banca…) sono chiusi, tranne che per gli admin, finché le fazioni non esistono. |
| Business | INCOMPLETE | 46 business visitabili, di cui 8 senza interno (solo l'etichetta). Proprietà e gestione non implementate. |
| Pawn → CEF bridge → JavaScript → UI | OK (statico) | Un solo browser per giocatore, creato su `OnCefInitialize(success)`, distrutto dal componente alla disconnessione. Se la pagina non risponde entro 3 secondi, si torna ai dialog. |
| Pulizia alla disconnessione | OK | 25 chiamate di pulizia in `OnPlayerDisconnect`; timer per giocatore chiusi con `KillTimer`. |

## F. CEF report

- **Server**: componente open.mp di samp-cef (`components/cef.dll`), 7 native verificate nel sorgente. Porta UDP **7779** (7777 + 2).
- **Client**: `cef.asi` più l'intera cartella `cef/` dallo **stesso commit** del componente. SA-MP 0.3.7 R1/R3 o 0.3.DL R1; la R2 non è supportata.
- **Asset**: `<GTA>/cef/assets/index.html` con HUD, login, personaggi, spawn, bacheca e telefono (660 KB). `preview.html` e `preview-server.js` servono solo come anteprima nel browser e non vanno nel client.
- **Eventi**: 69 server→pagina, tutti con un listener. 25 pagina→server, tutti con `cef_subscribe`. Listener presenti ma mai usati dal server: `lssc:msg`, `lssc:msg_clear`, `lssc:bank_row`, `lssc:chat_open`, `lssc:chat_clear`. Il server non li manda ancora e non è stato inventato un backend finto.
- **Validazione**: ogni evento della pagina passa dagli stessi controlli di comandi e dialog. Chat CEF con pausa minima tra i messaggi, password in esadecimale e cancellate dalla memoria dopo l'uso, testo verso la pagina ripulito da UTF-8 non valido (`cef_safe.inc`).
- **Da provare in gioco**: focus, ESC, tasti F6/Y/N, scala a 1080p e 1440p, prestazioni in guida, eventuali crash allo spawn.

### Georgiano

| Area | Stato |
|---|---|
| Pagine CEF (HUD, login, personaggi, spawn, bacheca, telefono) | Georgian READY IN CEF, REQUIRES CLIENT TEST. Noto Sans Georgian in ogni elenco di font. |
| Chat, dialog, 3D text, TextDraw, fumetti (interfaccia SA-MP) | Georgian UNSUPPORTED IN LEGACY UI: i font di SA-MP non hanno l'alfabeto georgiano, quindi il testo viene traslitterato in lettere latine da `core/lang.inc`. Nessuna chiamata diretta con testo georgiano non traslitterato (controllo su tutte le native di testo). |
| Database | READY: `utf8mb4` ovunque, connessione `SET NAMES utf8mb4`. |
| Gamemode | Tradotto: 1.886 testi più i frammenti. TRANSLATION MISSING (scelta voluta, da confermare): i nomi dei comandi e delle opzioni restano in italiano (`/casa`, `/lavoro`, `/noleggio`, `/compra cibo`…). Restano in italiano anche i log del server, che legge solo chi gestisce il server. |
| Quartieri | Da decidere: georgiano nell'HUD, misto altrove (`Idlewood-ის`). |

## G. Repair changelog

| File | Problema | Modifica | Test | Rischi |
|---|---|---|---|---|
| `gamemodes/modules/database/db_house.inc` | Seme delle case che sovrascrive le case Arizona 1–10 | `INSERT IGNORE`, tolto `ON DUPLICATE KEY UPDATE` | Import da zero: id Arizona 11–346, id 1–10 liberi | Le case di base non si "riallineano" più da sole se cambi le loro coordinate nel codice |
| `recovery/tools/arizona/gen.py`, `sql/migrations/015_arizona_houses.sql` | Come sopra; casa 192 con la porta doppia | `AUTO_INCREMENT = 11`; doppioni di porta esclusi | MySQL 8 e MariaDB | Nessuno sui database esistenti (INSERT IGNORE su `uq_houses_src`) |
| `sql/migrations/016_arizona_houses_fix.sql` (nuovo) | Database già danneggiati | UPDATE delle case Arizona senza proprietario ai valori originali | Copia danneggiata → 0 differenze | Rimette il prezzo originale anche se un admin l'aveva cambiato a mano su case Arizona senza proprietario |
| `gamemodes/modules/database/db_living.inc` | Errore 1054 su MariaDB | Sottoquery con OFFSET; buffer da 1024 a 1280 | Stessi risultati su MySQL 8 e MariaDB (dati di prova) | — |
| `sql/schema_full_v8.sql`, `sql/crea_utente.sql`, `scripts/build_sql.py`, `sql/lsrp_completo.sql` | Password nel repository; file unico assemblato a mano | Utente a parte con segnaposto e permessi minimi; file unico generato | Import e utente su MySQL 8 e MariaDB | Chi installa deve eseguire anche `crea_utente.sql` |
| `scripts/flatten.py`, `gamemodes/lsrp_completo.pwn` | `error 075` in Qawno | Righe lunghe spezzate | Stesso bytecode di `lsrp.pwn` | — |
| `gamemodes/modules/world/az_world.inc` | 8 business senza interno → 0,0,0 | Niente pickup di teletrasporto; id pickup -1 | Compilazione; da provare in gioco | — |
| `gamemodes/modules/world/az_map.inc` (generato) | 801 oggetti doppi | Tolti i doppioni identici in tutti i campi | 14.956 oggetti | Gli altri 2.586 con stessa posizione ma materiali diversi sono rimasti |
| `gamemodes/modules/cef/cef_bridge.inc` | Password cancellata solo in parte | Azzerato tutto il buffer (`sizeof`) | Compilazione | — |
| `gamemodes/modules/commands/rp_commands.inc` | Fumetto `/ame` con il georgiano rotto | Traslitterazione, massimo 139 caratteri | Compilazione; da provare in gioco | — |
| `gamemodes/lsrp.pwn` | Nome della modalità "LSRP 0.1.0-proto" | "LS Crime's RP" | — | — |
| `gamemodes/modules/database/db_core.inc` | Il suggerimento indicava un file inesistente | Punta a `lsrp_completo.sql` e `crea_utente.sql` | — | — |
| `deploy/server/*.esempio`, `cef/assets/PROTOCOLLO.md`, `scripts/build.sh` | Configurazione incompleta, documentazione vecchia | Modelli privati, porta 7779, `build_sql` nella build | — | — |

## H. Azioni manuali richieste

1. **Streamer 2.9.6**: scarica `samp-streamer-plugin-2.9.6.zip` da https://github.com/samp-incognito/samp-streamer-plugin/releases/tag/v2.9.6 e metti `plugins/streamer.dll` al posto di quella del donatore. Verifica: il log non deve mostrare avvisi di versione.
2. **CEF**: client (`cef.asi` + `cef/`) e `components/cef.dll` devono venire dallo stesso commit di https://github.com/ZOTTCE/samp-cef. Apri la porta UDP 7779 nel firewall di Windows.
3. **Database del PC di prova**: se l'hai creato con il vecchio `lsrp_completo.sql`, fai un backup e poi importa `016_arizona_houses_fix.sql` dentro `lsrp`. La casa Arizona 192 (doppione della 143) resta nel tuo database: dimmi se vuoi una migrazione che la disattivi.
4. **Password**: la password di test `LsrpTest2026` è nella storia del repository. Cambiala sull'utente MySQL che la usa e in `mysql.ini`. Cancellarla dalla storia di git richiede un force-push e serve la tua autorizzazione.
5. **RCON**: il tuo `config.json` aveva una password RCON corta. Usane una lunga, oppure `"enable": false` come nel modello.
6. **Decisioni di gioco**: la casa di base 4 (2065,0 / −1703,5) è a 0,8 m dalla casa Arizona 86; le due porte si sovrappongono. Restano da decidere anche i comandi in italiano e i nomi dei quartieri.
7. **Visual C++ Redistributable x86**: https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist, se streamer non si carica.

## I. Piano del primo test (preparato, NON eseguito)

Server privato sul tuo PC: `announce` spento, query spenta, password di ingresso (vedi `deploy/server/config.json.esempio`).

1. **Database**: avvia MySQL/MariaDB. Su un database nuovo importa `lsrp_completo.sql`, poi `crea_utente.sql` con la tua password. In phpMyAdmin controlla: `SELECT COUNT(*) FROM lsrp.houses` → 336 prima del primo avvio, 346 dopo; `SELECT * FROM lsrp.schema_migrations` → 6 righe.
2. **Dipendenze**: in `plugins/` servono `mysql.dll`, `streamer.dll` (2.9.6) e `samp_bcrypt.dll`; accanto all'exe `log-core.dll` e `libmariadb.dll`; in `components/` `cef.dll`; `mysql.ini` nella cartella del server.
3. **Avvio**: `omp-server.exe` con `main_scripts: ["lsrp 1"]`.
4. **Log**: nessun `Failed` né `Function not registered`. Devi vedere `[DB] connesso (utf8mb4)`, `[MONDO] Arizona Los Santos: 14956 oggetti, 41 porte, 46 business (8 senza interno), 227 veicoli` e il componente CEF caricato.
5. **Client**: SA-MP 0.3.7 R1 o R3 con `cef.asi` e `cef/assets` di questo pacchetto; connessione a `127.0.0.1:7777`.
6. **Registrazione e login**: account nuovo, poi uscita e login. La pagina CEF deve comparire entro 3 secondi, altrimenti si apre il dialog.
7. **Spawn**: crea il personaggio, sceglie lo spawn, compare l'HUD. Annota se il gioco crasha e copia `cef_client.log` e il log del server.
8. **CEF**: F6 chat, Y interazione, N azioni rapide, telefono; ESC chiude e rilascia il mouse.
9. **Casa**: vai a una casa Arizona libera (per esempio la casa Arizona n. 1, a 1885,9 / −1113,6). Devi vedere l'etichetta, `/casa` deve funzionare, l'acquisto con abbastanza soldi deve riuscire.
10. **Interno**: entra ed esci da una casa e da un edificio pubblico (freccia), controllando mondo virtuale e interno. Prova anche un business con interno e uno senza: quello senza non deve teletrasportarti.
11. **Veicolo**: motorino libero (N = motore), auto di strada a noleggio da $179, auto LSPD chiusa se non sei in servizio.
12. **GPS**: non eseguibile, il sistema non esiste (BLOCKED).
13. **Salvataggio**: soldi e posizione cambiati, `/q`, poi controllo in `characters` (pos_x, money).
14. **Riconnessione**: rientro, lista dei personaggi, spawn nell'ultima posizione con gli stessi dati.
15. **Arresto**: chiudi la console del server con Ctrl+C. Nel log deve esserci `[DB] connessione chiusa`, senza errori.

**Mi fermo qui: aspetto la tua autorizzazione per il test in gioco.**
