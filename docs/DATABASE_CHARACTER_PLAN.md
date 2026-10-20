# Piano database: account, personaggi, prologhi, City Memory

Stato: **PROGETTO**. Migrazione proposta: `sql/migrations/012_characters_v2.sql` (additiva, nessun DROP; provata prima sul DB dev e su MySQL 8 come per 010/011).
**Nessuna modifica a database di produzione.** Per i server reali: backup → importazione manuale da parte del Game Director.

## 1. Modifiche a tabelle esistenti

```sql
-- ACCOUNT: sicurezza e email facoltativa (mai considerata verificata senza verifica reale)
ALTER TABLE accounts
  ADD COLUMN email               VARCHAR(254) NULL,
  ADD COLUMN email_verified      TINYINT UNSIGNED NOT NULL DEFAULT 0,
  ADD COLUMN email_token_hash    CHAR(64) NULL,               -- solo l'hash del codice, quando esisterà l'invio
  ADD COLUMN password_changed_at TIMESTAMP NULL,
  ADD COLUMN failed_logins       SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  ADD COLUMN locked_until        INT UNSIGNED NOT NULL DEFAULT 0,
  ADD COLUMN last_char_switch    INT UNSIGNED NOT NULL DEFAULT 0, -- UNIX ts, cooldown cambio personaggio
  ADD COLUMN lang                CHAR(5) NOT NULL DEFAULT 'ka',
  ADD UNIQUE KEY uq_accounts_email (email);                   -- NULL multipli ammessi

-- PERSONAGGI: identita' e percorso (slot 0..2 gia' supportati da UNIQUE(account_id, slot))
ALTER TABLE characters
  ADD COLUMN char_name       VARCHAR(24) NULL,                -- "Nome_Cognome", latino, unico
  ADD COLUMN first_name      VARCHAR(20) NULL,
  ADD COLUMN last_name       VARCHAR(20) NULL,
  ADD COLUMN display_name_ka VARCHAR(64) NULL,                -- facoltativo, solo CEF (utf8mb4)
  ADD COLUMN birth_date      DATE NULL,
  ADD COLUMN gender          TINYINT UNSIGNED NOT NULL DEFAULT 0,
  ADD COLUMN archetype       TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- 0 legacy, 1 outlaw, 2 citizen, 3 recruit
  ADD COLUMN background_flags INT UNSIGNED NOT NULL DEFAULT 0,     -- bit: outlaw, prologo saltato, redenzione...
  ADD COLUMN phone_number    INT UNSIGNED NULL,               -- 5550000..5559999, generato dal server
  ADD COLUMN status_flags    INT UNSIGNED NOT NULL DEFAULT 0, -- bit: arrestato, ricercato, ferito... (blocco cambio)
  ADD COLUMN last_played_at  TIMESTAMP NULL,
  ADD UNIQUE KEY uq_characters_name (char_name),
  ADD UNIQUE KEY uq_characters_phone (phone_number);
```

## 2. Nuove tabelle

```sql
CREATE TABLE prologue_progress (            -- 1 riga per personaggio
  char_id INT UNSIGNED PRIMARY KEY, path TINYINT UNSIGNED NOT NULL, step SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  step_data INT NOT NULL DEFAULT 0, attempts SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  state TINYINT UNSIGNED NOT NULL DEFAULT 0,             -- 0 in corso, 1 completato, 2 saltato
  started_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, finished_at TIMESTAMP NULL,
  FOREIGN KEY (char_id) REFERENCES characters(id));

CREATE TABLE character_certificates (
  id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY, char_id INT UNSIGNED NOT NULL, kind SMALLINT UNSIGNED NOT NULL,  -- 1 formazione di base
  score TINYINT UNSIGNED NOT NULL, issued_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, revoked TINYINT UNSIGNED NOT NULL DEFAULT 0,
  UNIQUE KEY uq_cert (char_id, kind), FOREIGN KEY (char_id) REFERENCES characters(id));

CREATE TABLE character_contacts (           -- contatti narrativi (Dre, Miss Lou...) e, in futuro, tra giocatori
  char_id INT UNSIGNED NOT NULL, contact_key VARCHAR(32) NOT NULL, added_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (char_id, contact_key), FOREIGN KEY (char_id) REFERENCES characters(id));

CREATE TABLE char_switch_log (
  id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY, account_id INT UNSIGNED NOT NULL, from_char INT UNSIGNED NULL, to_char INT UNSIGNED NULL,
  result TINYINT NOT NULL,                                 -- 1 ok, <0 codice del blocco
  ip VARCHAR(45) NULL, at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, KEY ix_switch_acc (account_id, at));

CREATE TABLE city_memory (                  -- solo eventi verificati dal server; mai chat o messaggi privati
  id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY, at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  kind SMALLINT UNSIGNED NOT NULL,                         -- impresa aperta, progetto completato, traguardo, prologo...
  district TINYINT UNSIGNED NOT NULL DEFAULT 0, char_id INT UNSIGNED NULL, ref_id INT UNSIGNED NULL,
  text_key VARCHAR(48) NOT NULL, params VARCHAR(255) NOT NULL DEFAULT '',  -- testo composto dalla UI nella lingua del giocatore
  visibility TINYINT UNSIGNED NOT NULL DEFAULT 1,           -- 0 staff, 1 pubblico, 2 solo il personaggio
  op_id VARCHAR(64) NULL, UNIQUE KEY uq_cm_op (op_id), KEY ix_cm_at (at), KEY ix_cm_district (district, at));
```

## 3. Procedure (stile già usato: op_id idempotenti, esiti numerici)

- `sp_char_create(account, slot, first, last, birth, gender, archetype, skin, op_id)`: controlla slot libero (≤ 3), nome unico (anche contro gli username degli account), genera `phone_number` unico, crea `prologue_progress`. Risposta: id / codice errore.
- `sp_char_switch_check(account, char)`: cooldown e flag di blocco salvati. I blocchi di sessione (combattimento, veicolo) li controlla il Pawn.
- `sp_transfer_guard(from_char, to_char)`: 1 se stesso account → bonifici, `/pay`, `/dai` e vendite rifiutati.
- `sp_prologue_step(char, step, data, op_id)`, `sp_cert_issue(char, kind, score, op_id)`, `sp_city_memory_add(...)`.

## 4. Migrazione dei dati esistenti (nella stessa 012)

- `characters.char_name` ← `accounts.name` per lo slot 0. `first_name`/`last_name` ← divisione su `_`. `archetype` = 0 (legacy). `prologue_progress.state` = 1 (completato).
- `phone_number` generato per ogni personaggio esistente.
- Conflitti (nomi non validi): `char_name` resta NULL e il personaggio sceglie un nome al primo accesso (dialog/CEF dedicato).
- Test: schema nuovo da zero = schema + migrazioni (stesso controllo già in `scripts/db.sh test`).
