# HUD "Street Crime's Glass UI" (CEF)

Stato: implementato e testato nella pagina (browser headless, 5 risoluzioni) e compilato in Pawn (3 varianti).
**Da provare in gioco con il client GTA + samp-cef**: tasti F6/Y/N, focus, prestazioni in guida.

## 1. Audit (cosa c'era)

| Area | Prima | Ora |
|---|---|---|
| CEF | samp-cef (componente open.mp `cef.dll`, client `cef.asi`), un solo browser `index.html`, bridge `bridge.js` (`window.cef.on/emit/set_focus`), API Pawn `cef_emit_event`/`cef_subscribe`/`cef_focus_browser`/`cef_always_listen_keys` (verificate nel sorgente samp-cef) | invariato, riusato |
| HUD soldi/livello | TextDraw (`ui/hud.inc`) per chi non ha CEF; `hud.js` (pannello giallo) per chi ha CEF | TextDraw invariati; `hud.js` sostituito dal nuovo HUD |
| Veicolo | TextDraw (`ui/vehicle_ui.inc`): velocita', benzina, motore, solo veicoli personali | CEF: tachimetro per ogni veicolo guidato; TextDraw solo senza CEF |
| Fame | TextDraw (`ui/house_ui.inc`) | CEF nel pannello stato; TextDraw solo senza CEF |
| Sete | **non esiste** nel gamemode | non mostrata (l'evento la prevede: -1 = assente) |
| Cintura, marcia | **non esistono** | non mostrate (nessun dato inventato) |
| Quartiere/sicurezza | Living Los Santos: `District_At`, crimine/sicurezza per quartiere | pannello accanto al radar con dati reali |
| Chat | SendClientMessage (~260 chiamate) + chat di default SA-MP | adapter `core/chat.inc` + chat CEF (vedi 4) |
| Notifiche | `lssc:toast` / `lssc:reject` | motore notifiche con coda (stessi eventi + `lssc:notify`) |
| Voce | assente | SampVoice opzionale (`voice/voice.inc`, `LSRP_VOICE`) |

## 2. Design system

`cef/assets/hud/css/variables.css`: palette ufficiale (#111126, #A855F7, #FF3674, #FF914D, ...), gradiente viola→rosa→arancione,
font locali: Rajdhani (numeri), Barlow (testo latino, gia' del login), Noto Sans Georgian (georgiano). Nessuna risorsa da Internet.
samp-cef disegna in software (OSR): **nessun backdrop-filter sui pannelli sempre visibili**, blur solo su azioni rapide/impostazioni.
Animazioni brevi su transform/opacity, nessuna animazione infinita.

## 3. Struttura

```
cef/assets/hud/
  css/variables.css layout.css components.css animations.css
  js/strings.js    testi georgiani
     icons.js      icone SVG inline originali
     bridge.js     unico accesso a window.LSSC/samp-cef (focus, tasti, testo in esadecimale)
     state.js      stato + impostazioni (localStorage protetto da try/catch)
     events.js     mappatura eventi server -> stato
     app.js        zone dello schermo e montaggio moduli
     modules/player-status.js contract.js notifications.js vehicle-hud.js zone.js chat.js interactions.js quick-actions.js
gamemodes/modules/ui/hud_cef.inc   lato server (timer 250 ms, invio solo dei cambiamenti)
gamemodes/modules/core/chat.inc    adapter chat (SendClientMessage ridefinito)
gamemodes/modules/core/vehicle_names.inc nomi modelli 400-611
gamemodes/modules/voice/voice.inc  SampVoice (opzionale)
```
Protocollo eventi: `cef/assets/PROTOCOLLO.md` (sezione HUD Glass UI).

## 4. Chat

- Chi ha la pagina CEF riceve **tutti** i messaggi nella chat CEF (testo originale, anche georgiano), la chat SA-MP non riceve
  nulla: nessun doppione. Per nasconderla del tutto il giocatore preme **F7** (funzione del client SA-MP).
- Scrivere: **F6** (CEF, si puo' scrivere in georgiano) oppure **T** (SA-MP, solo latino): tutti e due passano dalle stesse verifiche.
- Messaggi verso chi non ha CEF: georgiano traslitterato in latino come prima.
- Categorie: riconosciute dal contesto (`Util_SendNear` = IC/OOC, `[RADIO]`/`[DIP]` = fazione, `[STAFF]` = admin, ...).
  Le schede Lavoro/Annunci/911 compaiono solo quando arriva un messaggio di quel tipo (oggi il gamemode non ha annunci ne' 911).
- `/chatsamp` torna alla chat SA-MP, `/chatcef` alla chat CEF.
- Sicurezza: testo mostrato solo con textContent (niente HTML), codici colore `{RRGGBB}` resi come colori.

## 5. Interazioni e azioni rapide

Tasto **Y** vicino a: banca, ATM, Job Center, noleggio, deposito taxi, bacheca contratti, ingresso/uscita casa, negozio,
concessionario, distributore (da guidatore). Esegue lo stesso comando che il giocatore scriverebbe (`/banca`, `/atm`, ...).
Tasto **N** (a piedi): azioni rapide. Disponibili: inventario, telefono, veicoli, impostazioni HUD. Mappa e animazioni sono
disattivate perche' il gamemode non ha (ancora) quei sistemi.

## 6. Voce (SampVoice)

- Versione: **SampVoice per open.mp** (https://github.com/AmyrAhmady/sampvoice/releases, file *x.x.x-omp*), MIT. Il plugin originale di CyberMor viene rifiutato da open.mp ("Skipping legacy plugin").
- Server: plugin `control` + servizio `voice` (vedi README della release). Il gamemode con voce si compila con `LSRP_VOICE=1`
  (`scripts/build.sh --voice` -> `lsrp_voce.amx`). Senza il plugin caricato **non** usare `lsrp_voce.amx`.
- Client: SampVoice supporta **SA-MP 0.3.7 R1 e R3**. Con altre versioni del client la voce non funziona (l'HUD mostra
  "ხმოვანი ჩატი არ არის").
- B = voce locale (25 m), Z = radio di fazione (solo in servizio).
- I binari non sono inclusi qui: si scaricano dalla pagina release (questa sessione non puo' scaricare i file di release).

## 7. Compilazione

| File | Comando | Server |
|---|---|---|
| `lsrp.amx` | `scripts/build.sh` / Qawno | componente CEF |
| `lsrp_senza_cef.amx` | `scripts/build.sh --nocef` (`LSRP_NO_CEF=1`) | nessun componente |
| `lsrp_voce.amx` | `scripts/build.sh --voice` (`LSRP_VOICE=1`) | CEF + SampVoice |

## 8. Test eseguiti

- `tests/cef/hudshot.py`: 1280x720, 1366x768, 1600x900, 1920x1080, 2560x1440; nessuna sovrapposizione tra pannelli,
  nessun pannello fuori schermo, nessun errore JS (solo `img/login.jpg` facoltativa).
- `tests/cef/hudfunc.py` (samp-cef simulato): eventi -> DOM, HTML del server non eseguito, F6 -> `lssc:chat_send` con UTF-8
  georgiano corretto, focus preso e rilasciato, maschera azioni rapide, coda notifiche (max 4), +$ e livello.
- Pawn: le tre varianti compilano con 0 errori.

## 9. Checklist in gioco (da fare con il client)

- [ ] login → HUD compare solo dopo lo spawn; logout/riconnessione: niente pannelli rimasti
- [ ] salute/armatura/fame cambiano con i valori reali; morte e respawn
- [ ] sali/scendi da un veicolo: il tachimetro appare/sparisce; motore/luci/serratura; benzina solo su veicoli personali
- [ ] F6 apre la chat CEF, Enter invia, Esc chiude e restituisce mouse e tastiera al gioco
- [ ] nessun messaggio doppio tra chat CEF e chat SA-MP
- [ ] Y vicino a banca/ATM/casa esegue l'azione; N apre il menu
- [ ] fluidita' in guida e in zone affollate (FPS prima/dopo)

## 10. Limitazioni note

- L'HUD nativo di GTA (soldi, vita, arma in alto a destra) non si puo' togliere dal server: il pannello stato e' posizionato sotto.
- Velocita' aggiornata dal server 4 volte al secondo (interpolata in CSS): piccolo ritardo rispetto al tachimetro nativo.
- Molti messaggi del gamemode sono ancora in italiano (traduzione L2 in `docs/LOCALIZATION_KA.md`): l'HUD e' tutto in georgiano.
- Il tasto F6 arriva alla pagina solo se la chat/dialog di SA-MP non e' aperta (comportamento del client samp-cef).
