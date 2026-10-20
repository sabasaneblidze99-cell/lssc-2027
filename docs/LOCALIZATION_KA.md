# Lingua georgiana (ka) — strategia di localizzazione

Stato: **PROPOSTA**, da confermare dal Game Director. Nessun codice modificato.

## 1. Il problema

| Canale | Testo georgiano (ქართული) | Perché |
|---|---|---|
| Chat SA-MP, dialog, TextDraw, 3D label, game text | **NO** | il client 0.3.7/0.3.DL usa una codepage Windows a 1 byte (1251/1252) e i font di GTA SA non hanno glifi georgiani. Nessuna codepage Windows contiene il Mkhedruli (U+10D0–U+10FF) |
| Input della chat SA-MP (quello che scrive il giocatore) | **NO** | la casella di input converte nella codepage del client |
| Nome del giocatore (launcher) | **NO** | SA-MP ammette solo `A-Z a-z 0-9 [ ] ( ) $ @ . _ =` |
| Pagine CEF (HTML) | **SÌ** | Chromium usa Unicode. Serve solo un font con i glifi georgiani |
| Database (utf8mb4) | **SÌ** | |

Verificato in sandbox (**[UNIT]**): `pawncc` 3.10.10 conserva i byte UTF-8 delle stringhe georgiane (`"გამარჯობა"` = 27 celle, `E1 83 92 …`).
Verificato nel sorgente di samp-cef: il componente open.mp passa la stringa Pawn così com'è alla pagina (`std::string` → `rust::Str`).
Quindi il server può mandare testo georgiano UTF-8 alla pagina CEF.
**Non verificato in GTA**: la visualizzazione reale nel client.
Attenzione: una stringa con byte non UTF-8 (per esempio `è` in Windows-1252) mandata a `cef_emit_event` può far fallire la conversione. Oggi tutte le stringhe inviate sono ASCII.

## 2. Soluzione proposta: una sola fonte, due scritture

1. **Tabella testi unica in georgiano** (`modules/lang/lang_ka.inc`, UTF-8), con chiavi (`TXT_LOGIN_TITLE`, …).
   Testi lunghi, archetipi e prologhi stanno in un catalogo condiviso con la pagina CEF (`cef/assets/i18n/ka.js`).
2. **CEF**: mostra il georgiano vero, con il font **Noto Sans Georgian** (licenza SIL OFL, incluso in `cef/assets/fonts`).
   Windows ha comunque *Sylfaen* come font di riserva.
3. **Chat, dialog e TextDraw**: traslitterazione automatica in latino con `Lang_Translit()`, sullo stile con cui scrivono i giocatori georgiani online.
   Esempio: `გამარჯობა` → `gamarjoba`, `შენ` → `shen`, `ჭ` → `ch`.
   Si fa nel server, al momento dell'invio: un testo scritto una sola volta, nessuna doppia traduzione da mantenere.
4. **Nomi RP**: obbligatoriamente in latino (limite del client), per esempio `Giorgi_Beridze`. La pagina CEF può mostrarli anche in georgiano, se il giocatore li inserisce (campo facoltativo `display_name_ka`).
5. **Chat CEF (fase successiva)**: una chat disegnata dalla pagina (stile Arizona) permette di **scrivere e leggere in georgiano**. La chat SA-MP resta per chi non ha il CEF, con traslitterazione automatica dei messaggi georgiani in arrivo dalla chat CEF.

Sconsigliato: modificare i font di GTA per "rimappare" le lettere su una codepage. Richiede una mod per ogni giocatore, rompe gli accenti di tutti gli altri testi ed è fragile.

## 3. Lavoro necessario

| Passo | Contenuto | Note |
|---|---|---|
| L0 | `lang_ka.inc` + `Lang_Translit()` + test unitari della traslitterazione; font georgiano nel CEF | prima di tutto il resto: tutto il nuovo testo nasce già in georgiano |
| L1 | Testi di accesso, personaggio, intro e prologhi in georgiano (nuovi) | insieme a M13–M16 |
| L2 | Traduzione dei moduli esistenti (circa 950 punti del codice che mandano chat, dialog e avvisi, oggi in italiano), un modulo alla volta | lavoro lungo: va pianificato |
| L3 | Chat CEF con input in georgiano | facoltativa, dopo i prologhi |

Revisione: i testi georgiani scritti da me vanno **riletti da un madrelingua** prima dell'apertura. Indico come "bozza" ogni testo non ancora riletto.
