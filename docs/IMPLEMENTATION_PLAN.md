# Chapter Zero — piano di implementazione per incrementi

Stato: **PROGETTO, in attesa di conferma**. Ogni milestone si chiude con: `check.sh` PASS, test runtime con bot, commit e tag, pacchetto Windows di prova.
Numerazione: si continua dopo le milestone esistenti (M6 inventario, M7 banca, Living Los Santos).

| # | Milestone | Contenuto | Dipende da | Verifica massima ottenibile qui |
|---|---|---|---|---|
| **M13** | Lingua georgiana (base) | `lang_ka.inc`, `Lang_Translit()` + test unitari, font georgiano nel CEF, testi dell'accesso già in georgiano | — | [UNIT] (traslitterazione), pagina in Chromium |
| **M14** | Account e separazione dati | migrazione 012 (parte account/personaggi), username ≠ nome personaggio, email facoltativa non verificata, blocco dopo kick ripetuti | M13 | [DB] + [RUNTIME] bot |
| **M15** | Slot personaggio | selezione 3 slot (CEF + dialog), creazione con nome/cognome/nascita, `SetPlayerName`, migrazione dei personaggi esistenti | M14 | [RUNTIME] bot |
| **M16** | Sesso e archetipi | scelta percorso (carte archetipo), skin per percorso, numero di telefono | M15 | [RUNTIME] bot; skin da controllare in gioco |
| **M17** | Intro "THE CITY NEVER FORGETS" | 7 scene sul motore dell'intro esistente, testi in georgiano, `/intro` | M16 | [RUNTIME] parziale (camera non verificabile dal bot) |
| **M18** | Prologo Citizen | sul bus e onboarding esistenti + telefono + motel | M17 | [RUNTIME] bot |
| **M19** | Prologo Outlaw | Dre e Miss Lou, scelta, reputazione Ganton, contatti | M17 | [RUNTIME] bot; attori e animazioni in gioco |
| **M20** | Prologo Recruit | 3 livelli, certificato, `/candidatura` | M17 | [RUNTIME] bot; guida e collaborazione in gioco |
| **M21** | Cambio personaggio e regole | `/cambiapg`, blocchi, cooldown, guardia sui trasferimenti, log | M15 | [RUNTIME] bot |
| **M22** | City Memory | tabella, scritture dagli eventi esistenti (`LD_Emit`), comando staff di consultazione; "Cronaca" CityOS più avanti | M14 | [DB] + [RUNTIME] |
| L2 | Traduzione dei moduli esistenti | un modulo per volta (accesso → HUD/telefono → banca → lavori → …) | M13 | per modulo |

Ordine richiesto rispettato (1 account → … → 9 City Memory). Ho aggiunto **M13 lingua in testa**: senza di essa ogni testo nuovo andrebbe riscritto due volte.

## Prima milestone implementabile: **M13 + M14** ("fondamenta")

1. `lang_ka.inc`: chiavi + testi georgiani (bozza) dell'accesso e dei messaggi di sistema più frequenti.
2. `Lang_Translit()` (UTF-8 georgiano → latino), con test unitari: tutte le 33 lettere, testo misto, ASCII invariato, buffer pieno.
3. Pagina CEF: font Noto Sans Georgian locale, testi dell'accesso in georgiano.
4. Migrazione 012 (solo account e colonne personaggio), provata su MariaDB dev **e** MySQL 8.
5. Accesso: username libero (anche non `Nome_Cognome`), email facoltativa in registrazione (CEF + dialog), blocco temporaneo.
6. Test: registrazione, login, password errata, blocco, email non valida, account esistente migrato, compatibilità con tutti i test runtime esistenti (oggi 502).

## Piano di test (quarta fase)

| Area | Test |
|---|---|
| Creazione account | username libero; username uguale al nome di un personaggio → rifiutato; email facoltativa e formato; password; blocco dopo kick ripetuti |
| Tre personaggi | crea slot 1, 2, 3; quarto rifiutato; nome duplicato rifiutato (anche tra account); data di nascita fuori intervallo rifiutata |
| Salvataggio e riconnessione | soldi, posizione, inventario per ogni slot; disconnessione durante la creazione; riavvio server |
| Cambio personaggio | cooldown; bloccato ferito / ammanettato / in combattimento / con contratto attivo; log scritto |
| Separazione | `/pay`, `/bonifico`, `/dai` tra personaggi dello stesso account rifiutati; inventari e conti indipendenti |
| Prologhi | ripresa dopo disconnessione a ogni passo; Outlaw: entrambe le scelte con effetti in DB; Recruit: fallimento e ripetizione, certificato unico |
| Compatibilità | suite runtime completa esistente (502) senza regressioni; build con e senza CEF |

Limiti onesti: il bot di test non vede la camera né il CEF. Cinematiche, attori, animazioni e pagine in gioco li verifica il Game Director sul suo client.
