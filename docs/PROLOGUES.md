# Intro e prologhi giocabili — progetto tecnico

Stato: **PROGETTO**. Coordinate, skin e animazioni sono tutte **DA VERIFICARE IN GIOCO**. Le cinematiche non si dichiarano funzionanti senza prova nel client.
Vincolo: **nessuna dipendenza obbligatoria dal CEF**. Tutto funziona con camera, TextDraw e dialog; il CEF aggiunge soltanto grafica e musica.

## 0. Riuso dell'esistente

| Serve | Esiste già |
|---|---|
| Camera interpolata, dissolvenze, letterbox, sottotitoli, skip con blocco iniziale, versione lunga/breve | `intro/intro.inc` + `ui/intro_ui.inc` (`InterpolateCameraPos/LookAt`, `UI_IntroFade*`) |
| Attori di scena | `CreateActor` nell'intro del bus |
| Checkpoint con permanenza, controlli antiteletrasporto | `freight/freight.inc` (stesso schema per le prove a tempo) |
| Reputazione di quartiere, eventi della città | `district.inc`, `LD_Emit` |
| Telefono, messaggi, waypoint | CityOS (CEF) + `/aiuto`, `Hub_SetTarget` |
| Onboarding a passi persistente | `ld_onboarding` + `hub.inc` |

Nuovo modulo: `modules/prologue/` con una macchina a stati per personaggio (`prologue_progress`: percorso, passo, dati del passo, tentativi). Si riprende **dal passo salvato** dopo una disconnessione o un riavvio.

## 1. Intro comune — „LOS SANTOS — THE CITY NEVER FORGETS"

Durata: ~45 s alla prima volta (saltabile dopo 8 s), ~12 s dalla seconda. Si può rivedere: `/intro` o "Rivedi intro" nel telefono.

| # | Camera (interpolata) | Testo (sottotitolo, georgiano) |
|---|---|---|
| 1 | Alba sull'oceano verso Santa Maria Beach | „Una volta questa città prometteva tutto." |
| 2 | Downtown, grattacieli | „Poi gli affitti sono saliti più in fretta dei sogni." |
| 3 | Negozi chiusi a Idlewood, con un attore commerciante | „Le piccole imprese resistono, una consegna alla volta." |
| 4 | Ganton al tramonto, lampeggianti lontani (oggetto luce) | „In alcuni quartieri la notte arriva prima." |
| 5 | Municipio / Pershing Square | „Le istituzioni fanno quello che possono. Non sempre basta." |
| 6 | Unity Station, gente con valigie (attori) | „E ogni giorno arriva qualcuno che ci crede ancora." |
| 7 | Panoramica, titolo grande | **LOS SANTOS — THE CITY NEVER FORGETS** |

Poi stacco sul percorso scelto, con il **titolo del prologo**.

## 2. Percorso CITIZEN — „ONE WAY TICKET" (il primo da realizzare: riusa di più)

1. **Arrivo**: la scena del bus a Unity Station (esistente), con origine e motivo dal sistema storia.
2. **Telefono**: SMS di benvenuto da "Casa del Quartiere" + suggerimento `/telefono` (con CEF) o `/aiuto` (senza).
3. **Servizi**: tre punti sulla mappa (Casa del Quartiere, Job Center, noleggio), che è già l'onboarding a 5 passi.
4. **Alloggio temporaneo**: indicazione per il motel di Jefferson (3 notti gratuite come *spawn point*, non come casa). **Nuovo, piccolo.**
5. **Fine**: messaggio „Los Santos è tua". Nessun obbligo: il prologo si può chiudere in qualunque momento con `/saltaprologo` (resta registrato come "saltato").

## 3. Percorso OUTLAW — „NO ONE GETS OUT CLEAN"

Spawn: una via di Ganton (non Grove Street come territorio di gang). Ora: sera.

| Passo | Cosa succede | Insegna |
|---|---|---|
| 1 | Telefonata (SMS + suono) da **"Dre"**, un vecchio amico: „Sei tornato. Ci vediamo dietro il market." | telefono, contatti |
| 2 | Checkpoint a piedi verso il market (attore Dre con animazione) | movimento |
| 3 | Dialogo con Dre (tasto **Y** o `/interagisci`): un vecchio debito di famiglia, 300$ da restituire a **"Miss Lou"**, che gestisce il negozio | interazione |
| 4 | **Scelta**: (a) restituire i soldi di persona; (b) „Dre dice che può sistemare lui" (scorciatoia) | conseguenze |
| 5a | Miss Lou ringrazia: **+reputazione Ganton**, contatto „Miss Lou" nel telefono (sconti futuri al negozio) | reputazione locale |
| 5b | Dre trattiene una parte, la voce gira: **−reputazione Ganton**, ma il contatto „Dre" sblocca lavori di strada futuri (non armati) | conseguenze, scelte grigie |
| 6 | Fine: „A Ganton tutti si ricordano tutto." Registrato nella City Memory come traguardo personale | City Memory |

Mai armi, mai appartenenza a gang, nessun reato contro altri giocatori. Il debito è narrativo e i 300$ fanno parte del denaro iniziale del percorso (neutro per l'economia).
Redenzione (futuro): il flag `background_outlaw` resta. Un percorso di riabilitazione potrà alleggerire i suoi effetti con requisiti RP.

## 4. Percorso RECRUIT — „BEFORE THE BADGE"

Luogo: campo di addestramento a **Verdant Meadows** (aeroporto abbandonato, ampio e isolato). Mondo virtuale separato **per gruppo di reclute**, così non si mischia con il traffico: le reclute si vedono tra loro, il resto del server no.
Durata obiettivo: **15–20 minuti** in tutto, con riprese libere. Niente grinding: ogni livello si supera una volta.

| Livello | Contenuto | Superamento |
|---|---|---|
| 1 — Disciplina | Istruttore (attore "Sgt. Hale"): adunata, percorso a piedi tra checkpoint a tempo generoso, 3 domande su comunicazioni e procedure (radio, codici base) | percorso entro il tempo + almeno 2 risposte su 3 corrette |
| 2 — Guida e collaborazione | Percorso di guida con un mezzo militare (Patriot) tra coni, con integrità del mezzo (come il trasporto merci). Collaborazione: se c'è un'altra recluta, uno guida e l'altro naviga (comando radio); da soli, la naviga l'istruttore | integrità ≥ 70% ed entro il tempo |
| 3 — Prova finale | Percorso misto (corsa, guida, 3 domande nuove) | punteggio ≥ 70/100 |

Al superamento:
- `character_certificates`: certificato **"Formazione di base completata"** (persistente, data, punteggio), mostrabile con `/documento`;
- trasferimento cinematografico a Los Santos (camera + elicottero, oggetto di scena) e spawn al municipio;
- requisito utile per **candidarsi** all'LSPD (`/candidatura`). L'assunzione resta dei vertici LSPD in RP;
- le carriere civile o militare restano aperte.

Fallire non punisce: si ripete il livello dopo 60 s. La guida è senza armi. Il poligono di tiro è escluso dalla prima versione.

## 5. Ripresa e casi limite

- Disconnessione: si riprende dal passo salvato (posizione di inizio passo, non a metà di un checkpoint).
- Riavvio del server: idem (stato in DB).
- Cambio personaggio durante un prologo: **bloccato**. Si può uscire dal gioco, ma non cambiare.
- Prologo saltato: registrato. Il percorso resta nel background, ma senza i bonus narrativi (contatti, certificato).
- Giocatori senza CEF: stesse scene con TextDraw e dialog. Testi traslitterati in latino.
