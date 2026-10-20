# Flusso completo: registrazione → personaggio → prologo → mondo libero

Stato: **PROGETTO**. (✓ = esiste già e si riusa · ✚ = nuovo · ✎ = da modificare)

```
CONNESSIONE (nome del launcher = username dell'account)
│
├─ ✓ controllo ban (per account) · ✓ attesa decisione CEF (max 9 s)
│
├─ account NON esiste → ✎ REGISTRAZIONE ACCOUNT
│      password + conferma (✓ bcrypt) · email facoltativa ✚ (non verificata) · regolamento ✓
│
├─ account esiste → ✓ ACCESSO (3 tentativi, poi kick; ✚ blocco 10 min dopo 3 kick in 1 h)
│
▼
✚ SELEZIONE PERSONAGGIO (3 slot)
│   [Personaggio 1: Giorgi Beridze · Citizen · Liv. 4 · ultimo accesso ieri]   → GIOCA
│   [Nuovo personaggio]                                                       → CREAZIONE
│   [Slot libero]
│
▼ (nuovo)
✚ IDENTITÀ: Nome · Cognome (latino, unico) · Data di nascita RP (18–80 anni)
✎ GENERE: uomo / donna                     (oggi: esiste)
✚ PERCORSO: OUTLAW · CITIZEN · RECRUIT     (carta con archetipo, tema e "cosa ti aspetta")
✎ ASPETTO: skin del percorso e del genere  (oggi: 7 skin uguali per tutti)
✓ solo CITIZEN: origine + motivo           (sistema storia esistente)
│
▼
✚ INTRO comune "LOS SANTOS — THE CITY NEVER FORGETS" (saltabile, ripetibile)
│
├─ CITIZEN → ✎ arrivo in bus (esistente) → telefono → ✓ onboarding Casa del Quartiere → motel
├─ OUTLAW  → ✚ Ganton: telefonata di Dre → debito di Miss Lou → scelta → reputazione
└─ RECRUIT → ✚ Verdant Meadows: livelli 1-2-3 → certificato → trasferimento a Los Santos
│
▼
MONDO LIBERO (un'unica economia, un'unica città)
   ✓ lavori · ✓ contratti · ✓ banca · ✓ case · ✓ LSPD/EMS · ✚ City Memory · ✚ cambio personaggio (/cambiapg)
```

## Rientro di un personaggio esistente
Selezione → `SetPlayerName` → spawn **dove si era fermato**, oppure al passo del prologo se è ancora in corso → HUD.

## Account esistenti (migrazione)
- L'username resta il nome attuale (es. `Mario_Rossi`).
- Il personaggio in slot 0 riceve nome e cognome da quel nome, percorso **Citizen** (legacy) e prologo **completato**. Nessuna perdita di dati.
- Al primo accesso dopo l'aggiornamento: selezione con il personaggio già presente e due slot liberi.

## UI
- Con CEF: tutto nella schermata animata già fatta (`auth.js`), con nuovi passi `select`, `identity`, `path`.
- Senza CEF: dialog equivalenti, testi traslitterati.
- Tutte le scelte sono **rivalidate dal server** (nome unico, data valida, skin del percorso, slot libero).
