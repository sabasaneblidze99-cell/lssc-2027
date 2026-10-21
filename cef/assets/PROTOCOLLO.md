# CityOS (samp-cef): protocollo eventi

Shell unica `index.html` (un solo browser per giocatore, come consigliato da samp-cef). Si crea con
`cef_create_browser(playerid, LSSC_BROWSER_ID, "index.html", false, false)` (percorso relativo risolto da `<GTA>/cef/assets/`)
nascosto al focus: l'HUD e' sempre visibile, il focus si da' solo alle viste interattive.

La pagina **non decide nulla di gioco**: mostra i dati ricevuti e invia richieste. Il server rivalida tutto (prossimita' alla
bacheca, requisiti, fondi, tetti, cooldown) esattamente come per i dialog di `modules/ui/living_ui.inc`, che restano il
**fallback equivalente** per chi non ha il plugin (`OnCefInitialize(playerid, 0)`).

## Tipi degli argomenti (verificati nel sorgente samp-cef)

- **Server → pagina**: `cef_emit_event(playerid, "nome", CEFSTR(s), CEFINT(n), CEFFLOAT(f) ...)`. La pagina riceve una lista di
  argomenti tipizzati (`cef.on(nome, function (a, b, ...) {})`). Solo `string`, `integer`, `float`: niente array/oggetti,
  quindi le liste si inviano con il pattern *begin / add / end* (come `UI_CityBoardBegin/Add/Show`).
- **Pagina → server**: `cef.emit(nome, ...)`; sul server `cef_subscribe("nome", "Callback")` riceve `Callback(playerid, const arguments[])`
  con **tutti gli argomenti uniti da spazi in una sola stringa**. Per questo la pagina invia solo interi o parole senza spazi.
- **Codifica**: componente open.mp, stringhe passate byte per byte. Il gamemode invia testo **UTF-8** (georgiano incluso):
  `cef/cef_safe.inc` ripulisce ogni argomento stringa prima dell'invio (una sequenza UTF-8 rotta farebbe chiudere il server).
  La pagina -> server: i testi liberi (chat, password, nomi) viaggiano in **esadecimale** (`bridge.js`) e il server li decodifica.
- Booleani: interi `0/1`. Assenza di un valore: `-1` dove indicato.

## Server → pagina

| Evento | Argomenti | Note |
|---|---|---|
| `lssc:hud` | money, bank, level, expIn, expStep, hour, minute | `expStep` 0 = livello massimo; `hour` -1 = ora nascosta. Inviare al cambio, non a ogni tick |
| `lssc:hud_visible` | show (0/1) | nasconde l'HUD (login, intro, cutscene) |
| `lssc:contract` | contractId, state, objective(s), destination(s), distanceM, secondsLeft, integrityPct, fromDistrict, toDistrict, cargo, reward, repPenalty | `contractId` 0 = nessun contratto. `state` 1 accettato, 2 in consegna. -1 = non mostrato (distanza, timer, integrita'). Il conto alla rovescia e' locale (1 s): reinviare solo quando cambia qualcosa (throttle >= 1 s) |
| `lssc:toast` | text(s), kind | kind 0 info, 1 ok, 2 attenzione, 3 errore (= `UI_LV_TOAST_*`), 4 s |
| `lssc:reject` | text(s) | esito negativo (= `UI_LivingReject`): toast rosso, chiude le conferme e lo stato "in attesa" |
| `lssc:open` | view(s), arg(s) | `board` oppure `phone` con app `home`, `contracts`, `messages`, `district`, `bank`, `job`, `profile`. **Chiede il focus** |
| `lssc:close` | view(s) | `""` = qualunque vista aperta. **Rilascia il focus** |
| `lssc:board_begin` | atBoard (0/1), placeName(s) | svuota la cache (max 12 righe) |
| `lssc:board_add` | contractId, kind, fromDistrict, toDistrict, cargo, reward, distanceM, secondsLeft, premium, minLevel, minRep, minLocalRep, blockCode | stessi parametri di `UI_CityBoardAdd`; `blockCode` = `UI_LV_BLOCK_*` (0 = idoneo) |
| `lssc:board_end` | - | ridisegna; poi `lssc:open board` |
| `lssc:district_begin` | districtId, activity, security, crime, localRep, festivalHours, prosperity, localTier, discountPct, hazard | `prosperity` 0 in difficolta', 1 stabile, 2 fiorente; `localTier` -1 da riconquistare, 0 sconosciuto, 1 conosciuto, 2 di casa; `hazard` 0/1. Soglie decise dal server (`gameplay_config.inc`) |
| `lssc:district_rumor` | text(s) | facoltativo |
| `lssc:district_event` | type, secondsAgo, value | max 8, dal piu' recente; `type` 1..9 come `UI_CityDistrictEvent` |
| `lssc:district_end` | - | ridisegna la scheda |
| `lssc:project` | projectId, districtId, name(s), goal, fund, hoursLeft, donMin, donMax, dayLeft, blockCode | `projectId` 0 = nessun progetto; `blockCode` = `UI_LV_PRJ_*` |
| `lssc:rep_begin` / `lssc:rep_end` | - | delimitano la reputazione |
| `lssc:rep_track` | track, points, completed, failed, ratingSum, ratingCnt, dayGain, tier, nextAt, bonusPct | `track` 1 trasporti, 2 taxi; `nextAt` 0 = grado massimo |
| `lssc:rep_local` | districtId, rep, localTier, discountPct, premium | |
| `lssc:bank_begin` / `lssc:bank_end` | - | estratto conto (sola lettura) |
| `lssc:bank_row` | when(s), kind(s), other(s), amount, balanceAfter | max 10, come `UI_BankStatementAdd` |
| `lssc:msg` | sender(s), text(s), secondsAgo, unread | max 20 in memoria; `lssc:msg_clear` svuota |
| `lssc:profile` | name(s), jobName(s) | |

## Pagina → server

| Evento | Argomenti | Equivalente Pawn |
|---|---|---|
| `lssc:ready` | - | pagina caricata e in ascolto: solo dopo si inviano `lssc:hud`, `lssc:contract`, `lssc:profile` |
| `lssc:close` | view | il giocatore ha chiuso (ESC o X): il server chiama `cef_focus_browser(..., false)` |
| `lssc:accept` | contractId | `UI_OnContractAccept` (dopo conferma). Risposta: `lssc:contract` + `lssc:close board` + toast, oppure `lssc:reject` |
| `lssc:abandon` | contractId | `UI_OnContractAbandon` (dopo conferma) |
| `lssc:board_refresh` | - | `UI_OnContractBoardRefresh`: ricaricare e inviare board_begin/add/end + `lssc:open board` |
| `lssc:district` | districtId (0 = dove si trova il giocatore) | `UI_OnDistrictPick` |
| `lssc:project_open` | districtId | `UI_OnProjectOpen` |
| `lssc:contribute` | projectId, amount | `UI_OnProjectContribute` (dopo conferma; importo rivalidato) |
| `lssc:bank_req` | - | inviare `lssc:hud` aggiornato + bank_begin/row/end |
| `lssc:rep_req` | - | come `/reputazione` |
| `lssc:job_waypoint` | - | come `/lavoro`: checkpoint verso il Job Center |

Esempio Pawn: `cef_subscribe("lssc:accept", "Cef_OnAccept");` e
`public Cef_OnAccept(playerid, const arguments[]) { UI_OnContractAccept(playerid, strval(arguments)); return 1; }`
(con piu' argomenti: separarli sugli spazi; non fidarsi mai dei valori).

## Collegamento nel gamemode (stato attuale)

- Build: `scripts/build.sh` = `lsrp.amx` con CEF (unico gamemode ufficiale); `--test` = senza CEF, solo per il server di prova Linux (stub in `ui/cef_ui.inc`).
- `modules/ui/cef_ui.inc`: stato del browser per giocatore, apertura/chiusura viste e focus, HUD. La pagina sostituisce dialog e TextDraw
  **solo dopo `lssc:ready`**; chi non ha il plugin (`OnCefInitialize(..., 0)`) continua con dialog e TextDraw.
- Rami CEF in `ui/hud.inc` (HUD) e `ui/living_ui.inc` (bacheca, contratto, quartiere, reputazione, progetto, toast, rifiuti).
- `modules/cef/cef_bridge.inc`: eventi pagina -> server, che passano dagli stessi ingressi di comandi e dialog.
- Il telefono si apre con `/telefono`. Se e' gia' aperto il server non lo riapre (le app chiedono i dati da sole: si eviterebbe un ciclo).
- `lssc:bank_req` per ora invia solo il saldo: l'estratto conto dettagliato resta allo sportello/ATM (`/estratto`).
- Non ancora collegati (la pagina ha gia' il listener, il server non li invia): `lssc:msg`/`lssc:msg_clear` (messaggi), `lssc:bank_row` (estratto conto),
  `lssc:chat_open`/`lssc:chat_clear`. Noleggio e taxi restano dialog.
- Rete: il componente usa UDP porta del server + 2 (7777 -> **7779**, QUIC cifrato): va aperta nel firewall insieme alla 7777.

## Focus e input

- **Chiedono il focus** (mouse e tastiera alla pagina): `lssc:open board`, `lssc:open phone`. La pagina chiama `cef.set_focus(true)`;
  il server, che resta l'autorita', chiama anche `cef_focus_browser(playerid, id, true)` subito dopo l'evento.
- **Rilasciano il focus**: `lssc:close` (in entrambi i versi). La pagina chiama `cef.set_focus(false)`; il server `cef_focus_browser(..., false)`.
- **Mai focus**: HUD, contratto, toast, tutti gli eventi dati.
- **ESC** chiude prima la conferma aperta, poi la vista (ed emette `lssc:close`). **Backspace** nel telefono torna indietro.
  Con la vista chiusa la pagina non riceve tasti: il telefono si apre da comando/tasto lato server (`lssc:open phone`).

## Prestazioni (samp-cef: rendering software OSR, GPU disattivata)

- HUD sempre visibile **senza `backdrop-filter`**: sfondo traslucido piu' opaco (`--glass-hud`), nessuna ombra, nessuna animazione;
  si aggiorna solo il testo che cambia; unico timer 1 s per la scadenza del contratto.
- Blur (14-16 px) solo su bacheca, telefono e conferme, cioe' quando il giocatore e' fermo nell'interfaccia.
- Animazioni solo `opacity` dei toast (180 ms). Nessuna animazione infinita.
- UI in `rem` legati all'altezza (`1rem = 100vh/67.5`): identica a 720p e 1080p.

## File

`index.html` shell · `styles.css` stile condiviso · `bridge.js` (cef, viste, focus, formattazione) · `hud.js` · `bacheca.js` · `telefono.js` ·
`fonts/` Barlow e Barlow Condensed (woff2 latin da @fontsource 5.3.0, licenza SIL OFL 1.1 in `fonts/OFL.txt`).
Solo anteprima/test (non servono in gioco): `preview.html`, `preview-server.js`, `_screenshots/`.
Anteprima in un browser normale: aprire `preview.html` (parametri `?res=720|1080&scene=...`).

## HUD Glass UI (cef/assets/hud) - eventi aggiunti

Server (`gamemodes/modules/ui/hud_cef.inc`) → pagina. Tutti inviati **solo quando il valore cambia**; timer unico 250 ms
(tachimetro 4 Hz solo per chi guida, il resto 1 Hz).

| Evento | Argomenti | Note |
|---|---|---|
| `lssc:identity` | name(s), playerid | all'apertura dell'HUD |
| `lssc:clock` | hour, minute, day, month, year | ora del server, al cambio di minuto |
| `lssc:status` | health, armor, hunger, thirst | 0-100; `thirst` -1 = il gamemode non ha la sete (indicatore nascosto) |
| `lssc:veh_show` | show, modelName(s) | 1 quando il giocatore guida, 0 quando scende/muore |
| `lssc:veh` | speedKmh, fuelPct, engine, lights, healthPct, locked | `fuelPct` -1 = veicolo senza carburante gestito (non personale). Niente marcia: il gamemode non simula un cambio |
| `lssc:zone` | districtId, danger, indoor | `districtId` 0 = fuori dai quartieri Living; `danger` -1 ignoto, 0 sicura (sicurezza >= 60), 1 normale, 2 pericolosa (crimine >= 70) |
| `lssc:voice` | state, key(s) | 0 SampVoice assente, 1 nessun microfono, 2 pronto |
| `lssc:interact` | show, key(s), label(s), progress | punto interattivo vicino; `progress` -1 = nessuna barra |
| `lssc:chat_mode` | on | 1 = la chat CEF sostituisce quella di SA-MP per questo giocatore |
| `lssc:chat` | category, text(s) | 0 sistema, 1 IC, 2 OOC, 3 fazione, 4 lavoro, 5 annunci, 6 emergenze, 7 admin; testo UTF-8 con colori `{RRGGBB}` |
| `lssc:chat_open` | prefill(s) | apre l'input della chat |
| `lssc:notify` | kind, title(s), text(s), ms | 0 successo, 1 info, 2 avviso, 3 errore, 4 soldi ricevuti, 5 soldi spesi, 6 missione, 7 lavoro, 8 RP |
| `lssc:money_delta` | delta, isBank | dopo un cambio reale di `lssc:hud` |
| `lssc:levelup` | level | |
| `lssc:quick_open` | mask | bit 0 inventario, 1 telefono, 2 mappa, 3 animazioni, 4 veicoli (impostazioni sempre). Chiede il focus |
| `lssc:quick_close` | - | |

Pagina → server:

| Evento | Argomenti | Equivalente |
|---|---|---|
| `lssc:chat_send` | testo UTF-8 in esadecimale | `/...` = `OnPlayerCommandText` (Cmd_Dispatch), altrimenti `OnPlayerText` (stesse verifiche, anti-flood 700 ms) |
| `lssc:quick` | inventory / phone / vehicles | `/inventario`, `/telefono`, `/v list` |
| `lssc:quick_close` | - | menu chiuso con Esc |

Tasti: **Y** (lato server, `KEY_YES`) esegue il comando del punto interattivo; **N** (`KEY_NO`, a piedi) apre le azioni rapide;
**F6** (lato pagina, grazie a `cef_always_listen_keys`) apre l'input della chat CEF. `/chatsamp` e `/chatcef` scelgono la chat.

## Personaggi (M15) e scelta dello spawn

| Verso | Evento | Argomenti | Note |
|---|---|---|---|
| server → pagina | `lssc:chars_begin` | slots(i) | inizio lista (3) |
| server → pagina | `lssc:chars_add` | slot(i) used(i) name(s) level(i) cash(i) skin(i) last(s) | una per slot |
| server → pagina | `lssc:chars_new` | slot(i) | mostra il modulo di creazione |
| server → pagina | `lssc:chars_err` | code(i) | 1 nome non valido, 2 occupato, 3 data non valida, 4 riprova |
| pagina → server | `lssc:char_pick` | slot | personaggio esistente → caricamento |
| pagina → server | `lssc:char_create` | slot Nome Cognome giorno mese anno | nomi solo latini (nessuno spazio) |
| pagina → server | `lssc:char_back` | - | dal modulo alla lista |
| server → pagina | `lssc:spawn_begin` | forced(i) seconds(i) | forced 1 = uscito in combattimento |
| server → pagina | `lssc:spawn_add` | idx(i) icon(s) title(s) sub(s) | icone: pin home badge medic briefcase city |
| pagina → server | `lssc:spawn_pick` | idx | |

Viste: `lssc:open chars`, `lssc:open spawn` (chiedono il focus; ESC non le chiude).
