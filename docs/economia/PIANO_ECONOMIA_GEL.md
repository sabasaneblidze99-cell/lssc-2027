# LS Crime's: economia in lari georgiani (GEL)

Piano di ribilanciamento, branch `economy-gel` del 21 ottobre 2026. Lo stato è **preparato e testato, ma non applicato al tuo database**: la conversione dei saldi aspetta la tua approvazione.

I dati reali e le loro fonti sono in [`RICERCA_GEORGIA_2026.md`](RICERCA_GEORGIA_2026.md): Geostat, Banca nazionale della Georgia, Gulf, Numbeo e annunci georgiani, ognuno con data e grado di affidabilità. Tutti i valori di gioco sono in un solo file, `gamemodes/modules/economy/econ_config.inc`.

## 1. Il modello

**Valuta.** La valuta è il lari (GEL, ₾, ლარი, suddiviso in თეთრი). Gioco e database contano in **tetri interi**: ₾5,50 si scrive 550. Non ci sono float nella contabilità.

**Limiti e overflow.** Un saldo, contanti o banca, arriva al massimo a **₾10.000.000**, cioè 1.000.000.000 tetri. È lo stesso numero che il gamemode usava già come tetto. Anche contanti più banca restano sotto il massimo di un intero Pawn a 32 bit, e il database (INT) non cambia tipo.
- La casa più cara costa ₾1.850.000, cioè 185 milioni di tetri: nessun overflow.
- Le somme dell'intero server (`/economia`) le calcola MySQL in BIGINT.

**Calendario RP.** **1 ora di gioco attivo vale 1 settimana lavorativa RP.** Per ogni lavoro: stipendio orario di gioco = stipendio mensile reale × 12 / 52, al netto dell'imposta sul reddito del 20% (la quota reale georgiana).
- Esempio: salario medio lordo ₾2.389/mese → ₾551 a settimana lordi → **₾441 netti all'ora di gioco**.
- I lavori d'ingresso stanno sotto la media (₾270-360/h), i camionisti sopra (₾550-700/h), le fazioni da ₾300 a ₾740/h.

**Due livelli di prezzo.** Ogni valore del gioco ha un riferimento reale e un valore di gioco:
- i consumi (cibo, acqua, sigarette, carburante) hanno **prezzi reali**;
- i beni durevoli (auto, case) sono vicini ai prezzi reali ma con uno sconto di gioco dichiarato, perché la progressione non duri anni.

**Cosa non si fa.** Non c'è nessuna conversione automatica dai prezzi russi o dai vecchi "$": ogni prezzo è stato riassegnato.

## 2. Inventario dei sistemi e dei prezzi

| Sistema | Prima ($ del vecchio gamemode) | Fonte nel codice | Ora (GEL) | Riferimento reale |
|---|---|---|---|---|
| Soldi iniziali | $7.500 | `intro.inc`, `character.inc`, `db_story.inc`, SQL `story_start_total` | **₾650** (₾200 contanti + banca; ₾150-300 secondo il motivo della storia) | ~1 settimana di salario minimo pubblico |
| Reddito di base ogni 15 min | $300 + $25×livello | `economy/paycheck.inc` | **₾20 + ₾1×livello**, massimo ₾30 (₾84-120/h) | pensione minima ₾370/mese |
| Autista bus | $28×6 + $40 a giro | `job.inc`, `job_ui.inc` | ₾5×6 + ₾8,50 (**₾330/h**) | trasporti ₾2.322/mese (settore) |
| Corriere | $45×5 + $30 | idem | ₾8,40×5 + ₾12 (**₾360/h**) | — |
| Netturbino | $24×8 + $40 | idem | ₾3,75×8 + ₾6 (**₾270/h**) | servizi ₾1.500/mese circa |
| Tetto orario Job Center | $2.200/h | idem | **₾450/h** | anti-abuso |
| Consegne camion (città) | $180 + $0,14/m | `ld_config`, scritto dal gamemode a ogni avvio | **₾35 + ₾0,035/m** (~₾88 a consegna, ~₾550/h) | trasporti/logistica |
| Tetto lavoro città/h | $2.800 | `ld_config` | **₾700/h** | anti-abuso |
| Taxi: tariffa | $15 + $0,12/m | `ld_config`, `living_calc.inc` | **₾5 + ₾10/km** | Tbilisi ₾3 + ~₾2/km: in gioco ×5 perché LS è compressa |
| Taxi: affitto e cauzione | $60/h + $300 | `taxi.inc`, `ld_config` | **₾15/h + ₾60** | — |
| Paga fazione per grado (15 min) | $150 … $500 | `faction.inc`, `faction_ui.inc` | **₾75 / 90 / 105 / 125 / 150 / 185** (₾300-740/h) | stipendio base pubblico ₾1.600 |
| Multa: minimo e tetto per grado | $50; $500-5.000 | `faction.inc` | **₾20; ₾150 / 300 / 600 / 1.000 / 1.500** | multe stradali georgiane ₾50-2.000 |
| Tetto multe per agente/h | $10.000 | idem | ₾3.000 | — |
| Ricovero / bonus rianimazione | $200 / $100 | idem | ₾40 / ₾25 | sanità pubblica |
| /pay massimo | $50.000 | `rp_commands.inc` | ₾5.000 | — |
| /givemoney admin | $100.000 | `admin_commands.inc` | ₾10.000 | registrato nel log admin |
| Bancomat: versamento / prelievo / giorno | $20.000 / $5.000 / $15.000 | `bank.inc`, `db_bank.inc` | ₾2.000 / ₾1.000 / ₾3.000 | bancomat georgiani ~₾1.000-3.000 |
| Sportello: prelievo / giorno | $50.000 / $100.000 | idem | ₾10.000 / ₾25.000 | — |
| Bonifico: commissione, min, max, giorno | 2% (min $5), $10-25.000, $50.000/$100.000 | idem | **1% (min ₾0,50)**, ₾1-25.000, ₾50.000 uscita / ₾100.000 entrata | banche georgiane 0-1% |
| Auto del concessionario (13) | $12.000-38.000 | `vehicle.inc`, `vehicle_ui.inc` | **₾2.200 (Faggio) … ₾31.000 (Sentinel)** | Opel Astra 2002-06 ₾7.800-8.800; Prius 2013 ₾14.500-18.900; Mercedes E400 2013 ₾27.785 |
| Rivendita auto / recupero | 60% / 5% (min $500) | `vehicle.inc` | 60% / 5% (min **₾60**) | — |
| Benzina | $3/L | `vehicle.inc` | **₾4,05/L** | Gulf regular ₾4,03 (8/10) e ₾4,09 (19/10) |
| Case di base (4 classi) | $14.000-130.000 | `db_house.inc`, `house_ui.inc` | **₾52.000 / 85.000 / 150.000 / 380.000** | case private a Tbilisi ₾2.778-4.656/m² |
| Case Arizona (336) | 750.000 / 1.000.000 (donatore) | `015`, `017`, `gen.py` | **₾58.000-1.850.000** (mediana ₾98.000) per classe × zona × garage | prezzi di gioco ≈ metà del reale |
| Rivendita casa | 70% | `house.inc` | 70% | — |
| Bollette luce + gas a ciclo (4 classi) | $75-360 | `house.inc` | **₾60 / 87 / 128 / 240**; riattivazione ₾25 (prima $40) | utenze 85 m² ₾228/mese |
| Negozio: razione, piatto, acqua, snack | $9 / 14 / 3 / 4 | `inventory.inc` | **₾2,50 / 14 / 1,20 / 2,80** | pane 500 g ₾1,98; pasto economico ₾30 |
| Kit, benda, sigarette, accendino | $60 / 12 / 8 / 5 | idem | ₾45 / 6,50 / **9** / 1,50 | Marlboro ₾9,00 |
| Telefono, zaino | $250 / 350 | idem | ₾380 / ₾85 | stime |
| Noleggio per 20 min (Faggio, Previon, Manana) | $30 / 60 / 70 + cauzione $200-450 | `ld_config`, `living_calc.inc` | **₾4 / 8 / 9** + cauzione ₾40-90 | noleggi a ore ₾10-30/h |
| Auto di strada (60 min) | $179 | `street_rental.inc`, `ld_config` | **₾29** | car sharing |
| Benvenuto, progetti di quartiere | $120; $50-500 (al giorno $1.000), obiettivo $6.000 | `ld_config`, `hub.inc` | ₾30; ₾10-100 (al giorno ₾250), obiettivo ₾1.500 | — |
| Business Arizona (non acquistabili) | 1,75-19 M (donatore) | `az_data.inc` | ₾120.000-900.000 per tipo (solo dati, per il futuro) | — |

**Dove il client potrebbe mostrare un saldo diverso dal server.**
- Il contatore dei soldi di GTA mostra **lari interi**, solo per vista. Il gamemode non legge mai il denaro del client (`GetPlayerMoney` non è usato).
- L'HUD CEF formatta i tetri che riceve dal server e non calcola nessun saldo.
- Le stime del telefono e della bacheca usano gli stessi numeri del database (`econ_config.inc`, scritti in `ld_config` a ogni avvio), ma l'addebito lo decide sempre il database.

## 3. Simulazione (`tests/economy/simulate.py`, con i valori veri)

**Ipotesi.** Si gioca 1-4 ore al giorno, il 70-85% del tempo lavorando. Si mangia/beve circa 3 volte l'ora. Con l'auto si guida metà del tempo, con il consumo del gamemode di 21,6 L/h. Si risparmia tutto il resto. Non è un benchmark.

**Redditi orari di riferimento** (lari per ora di gioco):

| Fonte | Reddito |
|---|---|
| Reddito di base (livello 1 / 10) | ₾84 / ₾120 |
| Netturbino / autista bus / corriere | ₾270 / ₾330 / ₾360 |
| Consegne camion (6 l'ora, 1,5 km) | ₾551 |
| Taxi (10 NPC + 2 giocatori l'ora) | ₾231 |
| Fazione grado 1 / 6 | ₾300 / ₾740 |
| Costo della vita, senza auto / con auto | ₾6 / ₾49 |

**Saldo nel tempo:**

| Profilo | Ore/giorno | Netto/h (g. 1) | Netto settimana | Saldo 30 g | Saldo 90 g | Saldo 180 g |
|---|---|---|---|---|---|---|
| Nuovo giocatore | 1.0 | ₾271 | ₾1.900 | ₾8.793 | ₾25.081 | ₾49.511 |
| Occasionale | 1.5 | ₾342 | ₾3.596 | ₾16.060 | ₾46.881 | ₾93.112 |
| Attivo | 3.0 | ₾468 | ₾9.831 | ₾42.783 | ₾127.050 | ₾253.450 |
| Lavoratore specializzato | 3.0 | ₾602 | ₾12.636 | ₾54.804 | ₾163.112 | ₾325.573 |
| Fazione (LSPD/EMS) | 2.0 | ₾286 | ₾4.000 | ₾18.813 | ₾70.098 | ₾186.466 |
| Giocatore ricco | 4.0 | ₾620 | ₾17.361 | ₾75.055 | ₾223.866 | ₾447.081 |

**Quando si raggiunge un obiettivo** (giorno, tra parentesi le ore di gioco):

| Profilo | Faggio | Prima auto (Manana) | Primo | Casa ₾58.000 | Casa ₾98.000 | Business ₾150.000 | Sentinel |
|---|---|---|---|---|---|---|---|
| Nuovo giocatore | g. 6 (6 h) | g. 24 (24 h) | g. 52 (52 h) | g. 212 (212 h) | g. 359 (359 h) | oltre 1 anno | g. 112 (112 h) |
| Occasionale | g. 4 (6 h) | g. 13 (20 h) | g. 27 (40 h) | g. 112 (168 h) | g. 190 (285 h) | g. 291 (436 h) | g. 60 (90 h) |
| Attivo | g. 2 (6 h) | g. 5 (15 h) | g. 10 (30 h) | g. 41 (123 h) | g. 70 (210 h) | g. 107 (321 h) | g. 22 (66 h) |
| Specializzato | g. 1 (3 h) | g. 4 (12 h) | g. 8 (24 h) | g. 32 (96 h) | g. 54 (162 h) | g. 83 (249 h) | g. 17 (51 h) |
| Fazione | g. 3 (6 h) | g. 11 (22 h) | g. 24 (48 h) | g. 79 (158 h) | g. 113 (226 h) | g. 153 (306 h) | g. 48 (96 h) |
| Ricco | g. 1 (4 h) | g. 3 (12 h) | g. 6 (24 h) | g. 24 (96 h) | g. 40 (160 h) | g. 61 (244 h) | g. 13 (52 h) |

**Cosa dice la simulazione:**
- **Prima auto in 12-24 ore di gioco.** Prima del vecchio sistema bastavano ~4 ore.
- **Prima casa in 100-200 ore**, cioè 1-3 mesi a 2-3 ore al giorno: un obiettivo di medio periodo.
- **Business e auto di lusso** sono obiettivi di lungo periodo.
- Un nuovo giocatore non riceve patrimoni: parte da ₾650 e con lo scooter (6 ore) e i noleggi si muove subito.
- I veterani guadagnano circa 2,3 volte un nuovo giocatore all'ora, non 10 volte. I tetti orari (₾450 al Job Center, ₾700 per consegne e taxi) impediscono la fuga in avanti.

**Fonti di inflazione.**
- Il reddito di base è denaro creato dal nulla, ma piccolo: ~25% del reddito di un principiante, ~15% di un veterano.
- Paghe dei lavori, delle fazioni e dei clienti taxi NPC.

**Pozzi di denaro.**
- Carburante (~₾44/h con l'auto), bollette, cibo, multe, ricoveri, commissioni bancarie dell'1%.
- La perdita alla rivendita: 40% sulle auto, 30% sulle case.
- Affitti di taxi e noleggi, tassa municipale del 10% sui taxi.

**Nota.** I costi orari sono bassi rispetto al reddito: è voluto. Le proprietà non obbligano a giocare ogni giorno: le bollette maturano solo mentre il proprietario è in gioco e attivo (`House_OnTick`), non quando è offline.

## 4. Database

**`017_economia_gel.sql`**. È NON distruttiva e fa parte di `lsrp_completo.sql`. Fa queste cose:
- crea la tabella `economy_meta` (valuta GEL, unità tetri, modello `ka-2026-v1`);
- riscrive le funzioni dei soldi iniziali;
- aggiorna `ld_config`;
- rimette i prezzi in lari alle case **senza proprietario**.

È generata da `scripts/gen_econ_sql.py` leggendo `econ_config.inc` e i prezzi delle case prodotti da `gen.py`.

**`sql/economia/018_conversione_saldi.sql`**. Converte i saldi esistenti. È **SOLO CON LA TUA APPROVAZIONE** e non fa parte del file unico.
- Fa prima un backup: `econ_backup_018_*`.
- Converte contanti e banca con un rapporto di **potere d'acquisto** (non di cambio): vecchio gioco ~$3.000/h di reddito, nuovo ~₾400/h, quindi 1 $ = 12 tetri.
- Con `@tetri_per_dollaro = 0` tutti ripartono dai soldi iniziali.
- Rimette il listino in lari alle case possedute e imposta il prezzo pagato = listino.
- Prezzo pagato delle auto = listino del modello.
- Blocca una seconda esecuzione.
- Il ripristino è `018_rollback.sql`.

Il tuo database contiene solo i dati dei tuoi test, quindi consiglio di ripartire da zero (`@tetri_per_dollaro = 0`) oppure di convertire con 12.

**Test eseguiti** su MySQL 8.0.46 (log binario attivo) e MariaDB 10.11:
- import da zero: 0 errori; case da ₾58.000 a ₾1.850.000; 7 migrazioni registrate;
- 017 + 018 su una copia del vecchio schema con personaggi di prova (anche al tetto massimo): risultati corretti, nessun negativo né valore oltre il tetto;
- seconda esecuzione: bloccata; rollback: valori identici a quelli di prima.
- 158 query del gamemode preparate contro lo schema.

## 5. Pawn

**Moduli nuovi:**
- `economy/currency.inc` contiene:
  - `Money_Format` / `Money_FormatSym` / `Money_Str`: "₾1.250,50", e "₾85.000" senza decimali quando sono zero;
  - `Money_Parse`: legge "25", "12,50" o "12.50" e rifiuta il separatore delle migliaia, i negativi e più di 2 decimali;
  - `Money_GtaDisplay`, `Money_Pct`.
- `economy/econ_config.inc` raccoglie **tutti** i valori.
- `economy/econ_admin.inc` è il comando `/economia` (admin 2+): massa monetaria e flussi bancari delle ultime 24 ore per tipo.

**Modifiche:**
- 88 testi `$%d` → `%s` + `Money_Str(importo)`; 88 `$%s` → `₾%s`.
- I 6 formattatori duplicati usano lo stesso motore.
- Tutti gli input di denaro accettano lari con decimali: banca, /pay, /multa, /contribuisci, admin.
- Nell'interfaccia SA-MP (font senza ₾) il simbolo diventa "GEL 1.250,50".
- **Correzioni lungo la strada:**
  - il menu delle case mostrava il prezzo della classe invece di quello vero della casa (le case Arizona apparivano a $14.000-130.000);
  - le tabelle di prezzi erano duplicate tra modulo e interfaccia;
  - il calcolo della commissione e il rimborso della casa sono ora senza overflow.
- I valori di `ld_config` che il gamemode scrive a ogni avvio ora vengono da `econ_config.inc`.

**Test:**
- compilazione 0 errori e 0 warning con 3.10.10 e 3.10.11, anche `lsrp_completo.pwn`;
- `tests/economy/currency_test.pwn` gira nella VM Pawn reale: 25/25 ok;
- `translit_test`: "GEL 1.250,50 lari".

## 6. CEF

- `U.money` (unico formattatore di HUD, telefono, bacheca e personaggi): tetri → "₾1.250,50".
- `U.parseMoney` per le donazioni dal telefono.
- Nessun calcolo di saldo nel browser.
- Simbolo ₾: nessuno dei font aveva U+20BE. È aggiunto da Noto Sans Georgian (sottoinsieme latin-ext, OFL, 22 KB), solo per quel carattere.
- `tests/economy/money_js_test.js`: tutto ok.

## 7. Cosa resta da verificare o decidere

1. **In gioco** (client vero):
   - "GEL" nei dialog;
   - ₾ nell'HUD CEF;
   - acquisto di una casa Arizona al prezzo nuovo;
   - pieno di benzina;
   - stipendio del Job Center;
   - una multa;
   - un bonifico con decimali.
2. **Approvare 018**, scegliendo il rapporto: 12 oppure 0 (si riparte).
3. **Sistemi che non esistono ancora**, quindi senza prezzi da ribilanciare: affitto di case, mutui, rate, prestiti, interessi, tassa sulla proprietà, assicurazione periodica, proprietà dei business, contabilità d'impresa, meccanico, lavori agricoli e minatore. Il modello è pronto ad accoglierli: consiglio affitto ≈ 1/150 del prezzo per ora RP e tassa sulla proprietà 0,2%/settimana RP sopra ₾100.000, come in Georgia.
4. **Pannello admin.** `/economia` mostra le statistiche. Per cambiare i prezzi si modifica `econ_config.inc`, oppure `/ldset` per la città. Un pannello CEF con modifiche registrate è la fase 2.
5. I movimenti solo in contanti (negozio, /pay, benzina) non finiscono nel registro bancario: per statistiche complete serve un registro anche per questi.
