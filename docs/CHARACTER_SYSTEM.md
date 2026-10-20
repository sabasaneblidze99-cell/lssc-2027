# Sistema account e personaggi (multi-character) — progetto

Stato (ottobre 2026): **M15 implementato** in `character/char_select.inc` + vista CEF `cef/assets/chars/` (selezione 3 slot, nome/cognome, data di nascita,
`SetPlayerName`), compilato e provato nella pagina; **da provare in gioco**. Cambio personaggio (`/cambiapg`, M21) e archetipi (M16) non ancora fatti.
Dopo la selezione: scelta dello spawn (`character/spawn_select.inc`, vista `cef/assets/spawn/`).

## 1. Cosa esiste oggi (analisi)

| Area | Oggi | File |
|---|---|---|
| Account | 1 account = **nome del launcher** (`Nome_Cognome` obbligatorio), password bcrypt (costo 11, migrazione trasparente da SHA256), ban per account, admin_level, mute | `account/account.inc`, `db_account.inc` |
| Personaggio | **1 per account** (`slot` 0); lo schema ha già `UNIQUE(account_id, slot)` | `character/character.inc`, `db_character.inc` |
| Creazione | genere → 7 skin per genere (`g_ValidSkins`) → storia (5 provenienze × 4 motivi) → intro del bus → spawn | `dialogs.inc`, `intro/intro.inc` |
| UI | dialog **e** pagina CEF animata (accesso, registrazione, genere, aspetto, storia) | `ui/cef_ui.inc`, `cef/assets/auth.js` |
| Nome in gioco | è il nome del launcher (`GetPlayerName` usato ovunque come nome RP) | tutti i moduli |
| Telefono | CityOS (CEF) + comandi; **nessun numero di telefono** | `cef/assets/telefono.js` |
| Tutorial | onboarding Casa del Quartiere (5 passi, `/aiuto`) | `hub/hub.inc` |
| Fazioni | LSPD, EMS (gradi 1–6, servizio, manette, ferito) | `faction/faction.inc` |

Conseguenza principale: oggi **account = personaggio = nome del launcher**. Con tre personaggi questo cambia.

## 2. Separazione Account / Personaggio

**ACCOUNT** (chi gioca):
- `username` = nome del launcher. **Non** deve essere per forza `Nome_Cognome` (es. `saba99`). Per gli account esistenti resta il nome attuale.
- `email` facoltativa, `email_verified` sempre **0** finché non esiste un vero servizio di verifica (codice inviato per email). Non si finge una verifica.
- password bcrypt (già così), `password_changed_at`, blocco temporaneo dopo troppi errori (`locked_until`), log degli accessi.
- **Nessun numero di telefono reale.** Non serve, quindi non si raccoglie (principio di minimizzazione).

**PERSONAGGIO** (chi vive a Los Santos): nome e cognome RP (latino, unico in tutto il server), data di nascita RP (età 18–80), genere, skin, **archetipo/percorso**, numero di telefono **in gioco** generato dal server (`555-XXXX`, unico), posizione, stato, progressione, denaro, proprietà, inventario, reputazione. Esiste già tutto per `char_id`, tranne identità, archetipo e numero.

## 3. Flusso di accesso

```
launcher (username) → [CEF o dialog] Accesso / Registrazione account
  → SELEZIONE PERSONAGGIO (3 slot: personaggio esistente | "Nuovo personaggio" | slot vuoto)
       esistente → anteprima (aspetto, livello, soldi, ultimo accesso, percorso) → GIOCA
       nuovo → Nome, Cognome, Data di nascita → Uomo/Donna → Percorso (Outlaw/Citizen/Recruit)
              → Aspetto (skin del percorso) → [Citizen: origine e motivo, sistema storia esistente]
              → Intro "THE CITY NEVER FORGETS" → Prologo del percorso → mondo libero
  → SetPlayerName(Nome_Cognome del personaggio) prima dello spawn
```

Regole sul nome:
- Formato `Nome_Cognome`, solo lettere latine, 3–20 caratteri, unico (`characters.char_name UNIQUE`).
- Un **username di account non può coincidere** con il nome di un personaggio di un altro (e viceversa): evita lo scambio di identità.
- Se `SetPlayerName` fallisce (nome già in uso da un altro giocatore online), si torna alla selezione con un messaggio.

## 4. Cambio personaggio

`/cambiapg` (oppure "Cambia personaggio" nel telefono) → salvataggio completo → torna alla **selezione**, senza disconnettersi.

Bloccato se (verifica server, motivo mostrato al giocatore):
- **cooldown** non scaduto (`GP_CHAR_SWITCH_COOLDOWN_MIN`, proposta **30 min** dall'ultimo cambio dell'account);
- ferito/in rianimazione, ammanettato, **in arresto o carcere** (futuro LSPD), ricercato o inseguito (futuro);
- danni subiti o inflitti negli ultimi **120 s** (in combattimento);
- in un veicolo in movimento, in un interno di proprietà altrui;
- contratto, turno di lavoro, noleggio, corsa taxi o prologo **attivi**: va prima chiuso, senza penalità dove il sistema lo prevede già;
- operazione bancaria o di inventario in volo (i lock esistono già).

Salvataggio sicuro: si usa `Char_Save` esistente, si attende la conferma DB e solo dopo si scarica lo stato. Uscita e rientro durante il cambio: idempotente.
Log: tabella `char_switch_log` (account, da/a, motivo del blocco, ip, ora).

## 5. Regole anti-abuso fra personaggi dello stesso account (controlli server)

| Regola | Controllo |
|---|---|
| Niente soldi fra personaggi propri | `/pay`, `/bonifico`, `/dai`, vendita veicoli/case: **rifiutati** se mittente e destinatario hanno lo stesso `account_id` (anche offline per i bonifici) |
| Niente "staffetta" via un terzo | log di trasferimenti A→X→B dello stesso account entro 24 h, segnalati allo staff (non bloccati: troppi falsi positivi) |
| Conflitto gang/polizia | un account non può avere **contemporaneamente** un personaggio in LSPD e uno in un'organizzazione criminale (quando esisteranno). Controllo all'invito |
| Conoscenze | non verificabile dal server: **regolamento** (no metagaming), contatti e telefono separati per personaggio, nessuna informazione dell'altro personaggio mostrata in UI |
| Duplicazioni | tutte le operazioni economiche restano per `char_id` con op_id idempotenti (già così) |
| Log | `admin_log` per le azioni staff, `char_switch_log`, log dei trasferimenti bloccati |

## 6. Archetipi, skin e percorsi

Skin **reali** di GTA SA (nomi modello dalla lista ufficiale degli ID). Va controllato in gioco l'aspetto e la coerenza (**DA VERIFICARE IN GIOCO**).
Esclusi: skin "speciali" e dei personaggi della storia (0–6, 265+, ecc.), skin delle gang (102–127, 173–175), divise di polizia e servizi.

| Percorso | Uomo (ID: modello) | Donna (ID: modello) |
|---|---|---|
| Outlaw | 21 `bmycr`, 22 `bmyst`, 143 `sbmycr`, 47 `hmycr` | 13 `bfyst`, 69 `sbfyst`, 195 `gangrl3`, 41 `hfyst` |
| Citizen | 7 `male01`, 101 `wmyst`, 46 `hmyri`, 60 `omyst` | 12 `bfyri`, 93 `wfyst`, 40 `hfyri`, 56 `ofyst` |
| Recruit | 96 `wmyjg`, 20 `bmyri`, 48 `hmyst` (in addestramento: 287 `army`) | 90 `wfyjg`, 91 `wfyri`, 39 `hfost` (in addestramento: **non esiste una divisa militare femminile** in GTA SA: si usa la tuta sportiva 90 o 191 `gungrl3`, da decidere) |

Gli archetipi (Malik, Kiara, Daniel, Sofia, Ethan, Alexandra) sono le **presentazioni**: testo, scena dell'intro e skin proposta come prima scelta. Non sono nomi obbligatori.

## 7. Cosa NON fa il multi-character

- Non annulla conseguenze: arresto, ferite, debiti, ricerca restano sul personaggio e bloccano il cambio.
- Non condivide inventario, soldi, case, veicoli, reputazione, contatti, certificati.
- Non permette due personaggi dello stesso account online insieme (una sola connessione).
