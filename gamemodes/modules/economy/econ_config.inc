// =============================================================================
// economy/econ_config.inc - TUTTI i valori economici del gamemode, in TETRI (₾1 = 100).
//
// Modello: "economia di gioco ispirata alla Georgia 2026" (docs/economia/PIANO_ECONOMIA_GEL.md).
//   Riferimento reale: salario medio lordo Q2 2026 ₾2.389/mese (Geostat), benzina ₾4,03-4,09/L (Gulf, ott. 2026),
//   pasto economico ₾30, pane ~₾1,98/500 g (Numbeo Tbilisi, ott. 2026), auto e case da annunci/Geostat.
//   Calendario RP: 1 ora di gioco attivo = 1 settimana lavorativa RP. Gli stipendi orari di gioco derivano dagli
//   stipendi mensili reali (mensile x 12 / 52, netto dell'imposta sul reddito del 20%); i beni hanno prezzi reali
//   o "di gameplay" dichiarati tali. I numeri qui sotto sono i valori di GIOCO.
// Cambiare un valore qui cambia tutto il gamemode (interfaccia compresa). I valori della "vita in citta'"
// (consegne, taxi, noleggi, progetti di quartiere) stanno nel database (ld_config, migrazione 017).
// =============================================================================

// ---------------------------------------------------------------- inizio del personaggio
#define ECON_START_TOTAL        (65000)      // ₾650 tra contanti e banca (il DB applica la stessa cifra: story_start_total)
#define ECON_START_CASH         (20000)      // ₾200 in contanti (il resto in banca)
// contanti iniziali secondo il motivo scelto nella storia (= SQL story_motive_cash)
#define ECON_START_CASH_M2      (15000)      // ₾150
#define ECON_START_CASH_M3      (30000)      // ₾300
#define ECON_START_CASH_M4      (25000)      // ₾250

// ---------------------------------------------------------------- reddito di base ogni 15 minuti di gioco attivo
// (sostegno cittadino; riferimento: pensione minima ₾370/mese ~ ₾85/settimana)
#define ECON_PAYCHECK_BASE      (2000)       // ₾20
#define ECON_PAYCHECK_PER_LEVEL (100)        // +₾1 per livello
#define ECON_PAYCHECK_MAX       (3000)       // massimo ₾30 ogni 15 min (₾120/h)

// ---------------------------------------------------------------- lavori del Job Center (paga per tappa + bonus a fine giro)
// obiettivo netto: autista bus ~₾330/h, corriere ~₾360/h, netturbino ~₾270/h (stime con i minuti medi per giro)
#define ECON_JOB_BUS_STEP       (500)        // ₾5,00 x 6 fermate
#define ECON_JOB_BUS_BONUS      (850)        // ₾8,50
#define ECON_JOB_COURIER_STEP   (840)        // ₾8,40 x 5 consegne
#define ECON_JOB_COURIER_BONUS  (1200)       // ₾12,00
#define ECON_JOB_CLEANER_STEP   (375)        // ₾3,75 x 8 cassonetti
#define ECON_JOB_CLEANER_BONUS  (600)        // ₾6,00
#define ECON_JOB_HOURLY_CAP     (45000)      // ₾450/h per personaggio (anti-abuso)

// ---------------------------------------------------------------- fazioni (paga ogni 15 min in servizio, per grado 1..6)
// riferimento: stipendio base pubblico ₾1.600/mese (grado 1) fino a ~₾4.000 (grado 6)
#define ECON_FAC_PAY_R1         (7500)       // ₾75   (~₾300/h)
#define ECON_FAC_PAY_R2         (9000)
#define ECON_FAC_PAY_R3         (10500)
#define ECON_FAC_PAY_R4         (12500)
#define ECON_FAC_PAY_R5         (15000)
#define ECON_FAC_PAY_R6         (18500)      // ₾185  (~₾740/h)
#define ECON_FAC_REVIVE_BONUS   (2500)       // ₾25 al medico per ogni rianimazione (max 6 per ora, come prima)
#define ECON_HOSPITAL_FEE       (4000)       // ₾40 di ricovero
#define ECON_FINE_MIN           (2000)       // ₾20 (multe reali per infrazioni minori ₾40-100: gioco piu' morbido)
#define ECON_FINE_HOURLY_CAP    (300000)     // ₾3.000 di multe emesse per agente all'ora
// tetto di una multa per grado (rif. multe stradali georgiane ₾50-500, guida in stato di ebbrezza ₾500-2.000)
#define ECON_FINE_CAP_R2        (15000)      // ₾150
#define ECON_FINE_CAP_R3        (30000)      // ₾300
#define ECON_FINE_CAP_R4        (60000)      // ₾600
#define ECON_FINE_CAP_R5        (100000)     // ₾1.000
#define ECON_FINE_CAP_R6        (150000)     // ₾1.500

// ---------------------------------------------------------------- trasferimenti tra giocatori (/pay)
#define ECON_PAY_MAX            (500000)     // ₾5.000 in contanti per volta
#define ECON_ADMIN_GIVE_MAX     (1000000)    // /givemoney: al massimo ₾10.000 per volta (registrato nel log admin)

// ---------------------------------------------------------------- banca (limiti per operazione / giorno, commissioni)
#define ECON_ATM_DEPOSIT_MAX    (200000)     // ₾2.000 per versamento al bancomat
#define ECON_ATM_WITHDRAW_MAX   (100000)     // ₾1.000 per prelievo al bancomat
#define ECON_ATM_WITHDRAW_DAY   (300000)     // ₾3.000 al giorno dal bancomat
#define ECON_DESK_WITHDRAW_MAX  (1000000)    // ₾10.000 per prelievo allo sportello
#define ECON_DESK_WITHDRAW_DAY  (2500000)    // ₾25.000 al giorno allo sportello
#define ECON_XFER_FEE_PCT       (1)          // 1% di commissione sui bonifici (banche georgiane: 0-1% tra conti)
#define ECON_XFER_FEE_MIN       (50)         // ₾0,50 minimo
#define ECON_XFER_MIN           (100)        // ₾1 minimo per bonifico
#define ECON_XFER_MAX           (2500000)    // ₾25.000 per bonifico
#define ECON_XFER_OUT_DAY       (5000000)    // ₾50.000 in uscita al giorno
#define ECON_XFER_IN_DAY        (10000000)   // ₾100.000 in entrata al giorno

// ---------------------------------------------------------------- veicoli (concessionario: prezzi dell'usato georgiano come riferimento)
#define ECON_VEH_FAGGIO         (220000)     // ₾2.200   scooter usato
#define ECON_VEH_PREVION        (750000)     // ₾7.500   utilitaria vecchia (rif. Opel Astra 2002-06 ₾7.800-8.800)
#define ECON_VEH_MANANA         (690000)     // ₾6.900   la piu' economica
#define ECON_VEH_VIRGO          (890000)     // ₾8.900
#define ECON_VEH_GLENDALE       (950000)     // ₾9.500
#define ECON_VEH_PRIMO          (1450000)    // ₾14.500  berlina (rif. Toyota Prius 2013 ₾14.500-18.900)
#define ECON_VEH_WILLARD        (1590000)    // ₾15.900
#define ECON_VEH_SADLER         (1350000)    // ₾13.500  pick-up vecchio
#define ECON_VEH_PCJ600         (1750000)    // ₾17.500  moto sportiva usata
#define ECON_VEH_BOBCAT         (1950000)    // ₾19.500  pick-up
#define ECON_VEH_TAMPA          (1650000)    // ₾16.500
#define ECON_VEH_MESA           (2900000)    // ₾29.000  4x4
#define ECON_VEH_SENTINEL       (3100000)    // ₾31.000  berlina premium (rif. Mercedes E400 2013 ₾27.785)
#define ECON_VEH_RESALE_PCT     (60)         // rivendita al concessionario: 60% del prezzo
#define ECON_VEH_RECOVER_PCT    (5)          // recupero/assicurazione dopo distruzione: 5% del prezzo...
#define ECON_VEH_RECOVER_MIN    (6000)       // ...minimo ₾60
#define ECON_FUEL_PER_LITER     (405)        // ₾4,05/L (benzina regular, Gulf ott. 2026: ₾4,03-4,09)

// ---------------------------------------------------------------- case (le 10 case di base; quelle di Arizona hanno il prezzo nel DB)
#define ECON_HOUSE_T0           (5200000)    // ₾52.000
#define ECON_HOUSE_T1           (8500000)    // ₾85.000
#define ECON_HOUSE_T2           (15000000)   // ₾150.000
#define ECON_HOUSE_T3           (38000000)   // ₾380.000
#define ECON_HOUSE_RESALE_PCT   (70)         // rivendita: 70%
// bollette per ciclo (fino a 180 min di consumo): rif. utenze 85 m2 ₾228/mese (Numbeo Tbilisi)
#define ECON_BILL_LUCE_T0       (3800)
#define ECON_BILL_LUCE_T1       (5500)
#define ECON_BILL_LUCE_T2       (8000)
#define ECON_BILL_LUCE_T3       (15000)
#define ECON_BILL_GAS_T0        (2200)
#define ECON_BILL_GAS_T1        (3200)
#define ECON_BILL_GAS_T2        (4800)
#define ECON_BILL_GAS_T3        (9000)
#define ECON_BILL_RESTORE_FEE   (2500)       // ₾25 di riattivazione dopo il distacco

// ---------------------------------------------------------------- negozio (supermercato): prezzi reali di Tbilisi dove esistono
#define ECON_ITEM_FOOD          (250)        // ₾2,50  razione (pane 500 g ~₾1,98)
#define ECON_ITEM_MEAL          (1400)       // ₾14,00 piatto pronto (pasto al ristorante economico ₾30)
#define ECON_ITEM_WATER         (120)        // ₾1,20
#define ECON_ITEM_SNACK         (280)        // ₾2,80
#define ECON_ITEM_KIT           (4500)       // ₾45,00 kit di pronto soccorso (stima)
#define ECON_ITEM_BANDAGE       (650)        // ₾6,50
#define ECON_ITEM_CIGARETTES    (900)        // ₾9,00 (Marlboro ₾9,00)
#define ECON_ITEM_LIGHTER       (150)        // ₾1,50
#define ECON_ITEM_PHONE         (38000)      // ₾380   telefono base (stima)
#define ECON_ITEM_BACKPACK      (8500)       // ₾85    zaino (stima)

// ---------------------------------------------------------------- "vita in citta'": valori della tabella ld_config
// Ogni ECON_LD_<CHIAVE> e' la riga <CHIAVE> di ld_config: scripts/gen_econ_sql.py la scrive in sql/migrations/017_economia_gel.sql.
// Il database decide gli addebiti; il Pawn usa le stesse cifre per stime e messaggi.
#define ECON_LD_MONEY_MAX           (ECON_MONEY_MAX)
// taxi: rif. Tbilisi partenza ₾3 + ~₾2/km (Bolt/Yandex, ott. 2026). In gioco ₾5 + ₾10/km: le distanze di LS sono
// compresse (~1,5 km a corsa) e il tassista deve guadagnare ~₾200-350/h tra clienti NPC e giocatori
#define ECON_LD_TAXI_BASE           (500)        // ₾5,00
#define ECON_LD_TAXI_PER_M_X100     (100)        // ₾0,01 al metro (₾10/km)
#define ECON_LD_TAXI_RENT_HOUR      (1500)       // ₾15/h affitto del taxi
#define ECON_LD_TAXI_DEP            (6000)       // ₾60 cauzione
// consegne (camion): obiettivo ~₾500-700/h a pieno ritmo (rif. trasporti ₾2.322/mese lordi)
#define ECON_LD_FRT_BASE            (3500)       // ₾35 a consegna
#define ECON_LD_FRT_PER_M_X100      (350)        // ₾0,035 al metro
#define ECON_LD_LD_WORK_HOURLY_CAP  (70000)      // ₾700/h tra consegne e taxi NPC
// noleggi: per blocco di 20 minuti (rif. noleggio scooter/auto a ore ₾10-30/h)
#define ECON_LD_RENT_FEE_FAGGIO     (400)        // ₾4
#define ECON_LD_RENT_FEE_PREVION    (800)        // ₾8
#define ECON_LD_RENT_FEE_MANANA     (900)        // ₾9
#define ECON_LD_RENT_DEP_FAGGIO     (4000)       // ₾40 cauzione
#define ECON_LD_RENT_DEP_PREVION    (8000)       // ₾80
#define ECON_LD_RENT_DEP_MANANA     (9000)       // ₾90
#define ECON_LD_RENT_FEE_STREET     (2900)       // ₾29 per 60 min (car sharing)
#define ECON_LD_RENT_DEP_STREET     (0)
// benvenuto e progetti di quartiere
#define ECON_LD_ONB_REWARD          (3000)       // ₾30 alla prima consegna di benvenuto
#define ECON_LD_PRJ_GOAL            (150000)     // ₾1.500 obiettivo di un progetto
#define ECON_LD_PRJ_DON_MIN         (1000)       // ₾10
#define ECON_LD_PRJ_DON_MAX         (10000)      // ₾100
#define ECON_LD_PRJ_DON_DAY         (25000)      // ₾250 al giorno per personaggio
#define ECON_LD_PRJ_DON_REP_PER     (2500)       // 1 punto reputazione ogni ₾25 donati
#define ECON_LD_PRJ_FUND_RIDE_X10   (375)        // +₾0,37 al fondo per corsa

// nomi brevi usati dal Pawn
#define ECON_STREET_RENT        (ECON_LD_RENT_FEE_STREET)
#define ECON_TAXI_RENT_HOUR     (ECON_LD_TAXI_RENT_HOUR)
#define ECON_TAXI_DEPOSIT       (ECON_LD_TAXI_DEP)
#define ECON_PRJ_DON_MIN        (ECON_LD_PRJ_DON_MIN)
#define ECON_PRJ_DON_MAX        (ECON_LD_PRJ_DON_MAX)
#define ECON_PRJ_DON_DAY        (ECON_LD_PRJ_DON_DAY)
