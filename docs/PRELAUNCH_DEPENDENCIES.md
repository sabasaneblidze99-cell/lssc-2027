# LS Crime's: dipendenze per il primo avvio

Aggiornato il 21 ottobre 2026, branch `prelaunch`.

Le native usate sono quelle realmente importate dai file compilati (`lsrp.amx`: 184 native, `lsrp_voce.amx`: 15), non una lista teorica.

Ambiente di riferimento: server di prova dell'utente su **Windows** (`C:\Users\sabas\Desktop\Server`), **open.mp 1.6.7.3218**, x86 (32 bit). Il client è GTA San Andreas con SA-MP 0.3.7 R1 o R3, oppure 0.3.DL R1. Tutti i plugin e i componenti sono a 32 bit.

## Server

| Dipendenza | Versione richiesta | Versione attuale | Stato | Link ufficiale | Dove va | Note |
|---|---|---|---|---|---|---|
| Server open.mp | 1.6.7.3218 | 1.6.7.3218 (log dell'utente) | READY | https://github.com/openmultiplayer/open.mp/releases/tag/v1.6.7.3218 (`open.mp-win-x86.zip`) | cartella del server | I componenti standard (Pawn, Dialogs, Objects, Pickups, TextLabels, Vehicles…) sono già nel pacchetto open.mp. |
| MySQL R41-4 (pBlueG) | R41-4 | R41-4 (log-core nel log MySQL dell'utente) | READY | https://github.com/pBlueG/SA-MP-MySQL/releases/tag/R41-4 (`mysql-R41-4-win32.zip`) | `plugins/mysql.dll`, `log-core.dll` e `libmariadb.dll` accanto a `omp-server.exe` | 17 native usate (`mysql_connect_file`, `mysql_tquery`, `mysql_format`, cache…). Include `a_mysql.inc` R41-4. La R41-4 corregge la `libmariadb.dll` difettosa della release precedente (note della release): usa la `libmariadb.dll` contenuta nello zip R41-4, non una presa altrove. |
| samp-bcrypt (Sreyas-Sreelal) | 0.4.x | sconosciuta (nel server dell'utente funziona) | VERSION UNKNOWN | https://github.com/Sreyas-Sreelal/samp-bcrypt/releases/tag/0.4.1 (`samp-bcrypt-windows-x86.zip`) | `plugins/samp_bcrypt.dll` | 3 native: `bcrypt_hash`, `bcrypt_get_hash`, `bcrypt_verify`. Login e registrazione funzionano nei test dell'utente. |
| Streamer (samp-incognito) | 2.9.6 (l'include è 0x296) | 2.9.4 (dll del donatore, fonte non verificata) | OUTDATED BUT WORKING → da sostituire | https://github.com/samp-incognito/samp-streamer-plugin/releases/tag/v2.9.6 (`samp-streamer-plugin-2.9.6.zip`) | `plugins/streamer.dll` + `"streamer"` in `legacy_plugins` | 4 native, tutte presenti anche nella 2.9.4. **Obbligatorio**: senza streamer il gamemode non si carica (14.956 oggetti del mondo). Su Windows richiede Visual C++ 2015 Redistributable (x86). La dll del donatore è stata tolta dal pacchetto. |
| samp-cef, componente open.mp (ZOTTCE) | stesso commit del client | build dell'utente (`C:\lsrp-build\samp-cef\build-win32\Release\cef.dll`) | REQUIRES MANUAL INSTALLATION | https://github.com/ZOTTCE/samp-cef (sorgente; il pacchetto client `cef.zip` è tra gli artifact di GitHub Actions) | `components/cef.dll` | 7 native, verificate nel sorgente del componente (`openmp-component/`). L'include `cef.inc` del progetto è identico a quello del repository (commit f14fe4f). La release v1.4.1 contiene solo il plugin SA-MP classico (`server.dll`/`libserver.so`), **non** il componente open.mp: va usata la build dal sorgente, con client e server dello stesso commit. |
| SampVoice per open.mp (AmyrAhmady) | v3.1.x-omp o v3.2.0-omp | build -omp dal sorgente (test Linux) | OPZIONALE / REQUIRES MANUAL INSTALLATION | https://github.com/AmyrAhmady/sampvoice/releases/tag/v3.2.0-omp (`sampvoice-win.zip`) | `components/` + `"side_scripts": ["filterscripts/lsrp_voce"]` (nel modello è vuoto: aggiungilo solo con il componente installato) | Solo il filterscript della voce lo usa (10 native Sv*). Il gamemode parte anche senza. Il plugin originale di CyberMor viene rifiutato da open.mp ("Skipping legacy plugin"). |
| MySQL Server 8.0 o MariaDB 10.4+ | 8.0.x / 10.4+ | sconosciuta (phpMyAdmin dell'utente) | VERSION UNKNOWN | MySQL: https://dev.mysql.com/downloads/mysql/ · MariaDB: sito ufficiale mariadb.org (link non verificabile da qui) | servizio locale, porta 3306 | Lo schema è stato importato da zero su **MySQL 8.0.46**, con il log binario attivo come nell'installazione standard, e su **MariaDB 10.11.14**. MySQL 8.4: attivare `mysql_native_password=ON`. |
| Visual C++ Redistributable x86 | 2015–2022 | non verificata | NOT VERIFIED | https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist | sistema | Serve a streamer (e di norma a mysql/log-core). |

## Strumenti di compilazione

| Dipendenza | Versione | Stato | Link | Note |
|---|---|---|---|---|
| pawncc (Qawno / open.mp) | 3.10.11 | READY | https://github.com/openmultiplayer/compiler/releases | 0 errori e 0 warning su `lsrp.pwn` e `lsrp_completo.pwn`. |
| pawncc (Zeex) | 3.10.10 | READY | https://github.com/pawn-lang/compiler/releases | 0 errori e 0 warning; si compila sempre con `-d3`. |
| Include | omp-stdlib, a_mysql R41-4, cef (samp-cef master), samp_bcrypt, streamer 2.9.6, sampvoice | READY | cartella `toolchain/include/` | Per Qawno sono copiati in `qawno_include/` del pacchetto. |

## Client (GTA San Andreas)

| Dipendenza | Versione | Stato | Link | Dove va |
|---|---|---|---|---|
| SA-MP client | 0.3.7 R1 o R3, oppure 0.3.DL R1 (la R2 **non** è supportata da samp-cef) | REQUIRES CLIENT TEST | launcher open.mp: https://github.com/openmultiplayer/launcher/releases | cartella di GTA |
| samp-cef client | stesso commit del componente server | REQUIRES MANUAL INSTALLATION | https://github.com/ZOTTCE/samp-cef (artifact `cef.zip` delle Actions, oppure build) | `<GTA>/cef.asi` + l'intera cartella `<GTA>/cef/` |
| Interfaccia LS Crime's | questo commit | READY | pacchetto `3_GTA_CLIENT/cef/assets` | `<GTA>/cef/assets/` (font georgiano Noto Sans Georgian incluso, licenza OFL) |
| SampVoice client | stessa release del componente | OPZIONALE | https://github.com/AmyrAhmady/sampvoice/releases | cartella di GTA |

## Rete

| Porta | Protocollo | Chi la usa |
|---|---|---|
| 7777 | UDP | server open.mp |
| 7779 | UDP | CEF (porta del server + 2, QUIC cifrato): da aprire nel firewall, altrimenti il client resta senza interfaccia CEF e usa i dialog |
| porta SampVoice | UDP | solo con la voce attiva (vedi il log del componente) |
| 3306 | TCP | MySQL, **solo locale** (`127.0.0.1`), mai aperta all'esterno |

## Plugin controllati e non necessari

Nel codice non c'è nessuna nativa di questi plugin: sscanf, Pawn.CMD, Pawn.RakNet, YSF, Nex-AC, FCNPC, MapAndreas, TOTP, Weapon Config, YSI, PawnPlus. Stato: **NOT NEEDED**. I comandi passano da un parser interno, l'anti-cheat non è implementato.

Crashdetect non serve per far girare il server, ma aiuta a diagnosticare i crash del gamemode. Si trova nel repository ufficiale https://github.com/Zeex/samp-plugin-crashdetect. La compatibilità con open.mp 1.6.7 non è stata verificata: è consigliato per il debug, non è installato.

## Come verificare l'installazione

Avvia `omp-server.exe` e controlla nel log queste righe:

1. `Loading plugin mysql`, `streamer`, `samp_bcrypt`: nessun "Failed".
2. `Successfully loaded component` per CEF (e per SampVoice, se c'è).
3. `[DB] connesso (utf8mb4)`.
4. `[MONDO] Arizona Los Santos: 14956 oggetti, 41 porte, 46 business (8 senza interno), 227 veicoli`.
5. Nessuna riga `Function not registered`.
