-- LSSC 2027 - installazione nuova del database (MySQL 8 / MariaDB)
-- Importare in phpMyAdmin dalla pagina iniziale (NON dentro un database): scheda "Importa", questo file, "Esegui".
-- Crea il database lsrp e tutte le tabelle. L'utente MySQL del server NON viene creato qui (nessuna password nel repository):
-- crealo con sql/crea_utente.sql scegliendo tu la password, la stessa che metterai in mysql.ini.
-- ATTENZIONE: se il database lsrp esiste gia', prima rinominalo o cancellalo tu da phpMyAdmin (questo file non cancella nulla).

CREATE DATABASE IF NOT EXISTS lsrp CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE lsrp;
-- NB: sql/schema.sql e' GENERATO (scripts/db.sh build-sql) da sql/src/schema_tables.sql + sql/procedures_inventory.sql.
-- LSRP schema base. Idempotente (CREATE TABLE IF NOT EXISTS), utf8mb4, InnoDB.
-- Le modifiche successive vanno in sql/migrations/NNN_nome.sql (additive).
-- I default sono NEUTRI: i valori iniziali di gioco arrivano dal Pawn (DB_CreateCharacter).

CREATE TABLE IF NOT EXISTS `schema_migrations` (
  `name`       VARCHAR(128) NOT NULL,
  `applied_at` TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `accounts` (
  `id`            INT UNSIGNED     NOT NULL AUTO_INCREMENT,
  `name`          VARCHAR(24)      NOT NULL,
  `password_hash` CHAR(64)         NOT NULL,
  `salt`          CHAR(16)         NOT NULL,
  `hash_algo`     TINYINT UNSIGNED NOT NULL DEFAULT 0,   -- 0 = SHA256+salt legacy, 1 = bcrypt (003)
  `admin_level`   TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `muted_until`   INT UNSIGNED     NOT NULL DEFAULT 0,   -- UNIX ts, 0 = non mutato (002)
  `registered_at` TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_login`    TIMESTAMP        NULL DEFAULT NULL,
  `last_ip`       VARCHAR(15)      NULL DEFAULT NULL,
  `intro_seen`    TINYINT UNSIGNED NOT NULL DEFAULT 0,   -- 1 = l'account ha gia' visto/saltato l'intro cinematica (010)
  `email`         VARCHAR(254)     NULL DEFAULT NULL,     -- facoltativa (012); mai considerata verificata senza verifica reale
  `email_verified` TINYINT UNSIGNED NOT NULL DEFAULT 0,   -- 1 solo dopo un vero codice di verifica (012)
  `email_token_hash` CHAR(64)      NULL DEFAULT NULL,     -- hash del codice di verifica, quando esistera' l'invio (012)
  `password_changed_at` TIMESTAMP  NULL DEFAULT NULL,     -- (012)
  `failed_logins` SMALLINT UNSIGNED NOT NULL DEFAULT 0,   -- tentativi errati consecutivi fra sessioni (012)
  `locked_until`  INT UNSIGNED     NOT NULL DEFAULT 0,    -- UNIX ts: login rifiutato fino a questo momento (012)
  `last_char_switch` INT UNSIGNED  NOT NULL DEFAULT 0,    -- UNIX ts dell'ultimo cambio personaggio (012)
  `lang`          CHAR(5)          NOT NULL DEFAULT 'ka', -- lingua dell'interfaccia (012)
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_accounts_name` (`name`),
  UNIQUE KEY `uq_accounts_email` (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `characters` (
  `id`         INT UNSIGNED     NOT NULL AUTO_INCREMENT,
  `account_id` INT UNSIGNED     NOT NULL,
  `slot`       TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `money`      INT              NOT NULL DEFAULT 0,
  `bank`       INT              NOT NULL DEFAULT 0,
  `level`      INT              NOT NULL DEFAULT 1,
  `exp`        INT              NOT NULL DEFAULT 0,
  `health`     FLOAT            NOT NULL DEFAULT 100,
  `armor`      FLOAT            NOT NULL DEFAULT 0,
  `pos_x`      FLOAT            NOT NULL DEFAULT 0,
  `pos_y`      FLOAT            NOT NULL DEFAULT 0,
  `pos_z`      FLOAT            NOT NULL DEFAULT 0,
  `angle`      FLOAT            NOT NULL DEFAULT 0,
  `interior`   INT              NOT NULL DEFAULT 0,
  `world`      INT              NOT NULL DEFAULT 0,
  `skin`       INT              NOT NULL DEFAULT 0,
  `played_ms`  BIGINT UNSIGNED  NOT NULL DEFAULT 0,
  `hunger`     TINYINT UNSIGNED NOT NULL DEFAULT 80,   -- sazieta' 0-100 (005)
  `groceries`  TINYINT UNSIGNED NOT NULL DEFAULT 0,    -- DEPRECATA da 008 (porzioni -> inventory_items item 1); resta come rollback e per l'eventuale residuo non migrato
  `job`          TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- lavoro, 0 = nessuno (007)
  `faction`      TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- fazione, 0 = nessuna (007)
  `faction_rank` TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- grado 1..6, 0 se senza fazione (007)
  `inv_bag`      TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- zaino acquistato (+8 slot), solo inv_set_bag lo modifica (008)
  `char_name`    VARCHAR(24)      NULL DEFAULT NULL,   -- "Nome_Cognome" RP, latino, unico (012)
  `first_name`   VARCHAR(20)      NULL DEFAULT NULL,   -- (012)
  `last_name`    VARCHAR(20)      NULL DEFAULT NULL,   -- (012)
  `display_name_ka` VARCHAR(64)   NULL DEFAULT NULL,   -- nome in georgiano, facoltativo, solo CEF (012)
  `birth_date`   DATE             NULL DEFAULT NULL,   -- data di nascita RP (012)
  `gender`       TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- 0 uomo, 1 donna (012)
  `archetype`    TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- 0 legacy, 1 outlaw, 2 citizen, 3 recruit (012)
  `background_flags` INT UNSIGNED NOT NULL DEFAULT 0,  -- bit narrativi permanenti (012)
  `phone_number` INT UNSIGNED     NULL DEFAULT NULL,   -- numero in gioco 5550000..5559999, unico (012)
  `status_flags` INT UNSIGNED     NOT NULL DEFAULT 0,  -- bit di stato che bloccano il cambio personaggio (012)
  `last_played_at` TIMESTAMP      NULL DEFAULT NULL,   -- (012)
  `created_at` TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_save`  TIMESTAMP        NULL DEFAULT NULL,
  `combat_logout` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_characters_account_slot` (`account_id`, `slot`),
  UNIQUE KEY `uq_characters_name` (`char_name`),
  UNIQUE KEY `uq_characters_phone` (`phone_number`),
  KEY `ix_characters_faction` (`faction`, `faction_rank`),
  CONSTRAINT `fk_characters_account` FOREIGN KEY (`account_id`)
    REFERENCES `accounts` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sistema admin (migrazione 002): tabelle replicate qui per i DB creati da zero.
CREATE TABLE IF NOT EXISTS `admin_log` (
  `id`                INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `staff_account_id`  INT UNSIGNED NOT NULL DEFAULT 0,
  `target_account_id` INT UNSIGNED NULL DEFAULT NULL,
  `action`            VARCHAR(24)  NOT NULL,
  `reason`            VARCHAR(128) NOT NULL DEFAULT '',
  `created_at`        TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `ix_admin_log_target` (`target_account_id`),
  KEY `ix_admin_log_staff` (`staff_account_id`),
  KEY `ix_admin_log_created` (`created_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `warns` (
  `id`               INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `account_id`       INT UNSIGNED NOT NULL,
  `staff_account_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `reason`           VARCHAR(64)  NOT NULL DEFAULT '',
  `created_at`       TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `active`           TINYINT UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`),
  KEY `ix_warns_account_active` (`account_id`, `active`),
  CONSTRAINT `fk_warns_account` FOREIGN KEY (`account_id`)
    REFERENCES `accounts` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- expires_at NULL = permanente; altrimenti UNIX timestamp di scadenza.
CREATE TABLE IF NOT EXISTS `bans` (
  `id`               INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `account_id`       INT UNSIGNED NULL DEFAULT NULL,
  `account_name`     VARCHAR(24)  NOT NULL,
  `ip`               VARCHAR(15)  NOT NULL DEFAULT '',
  `staff_account_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `reason`           VARCHAR(64)  NOT NULL DEFAULT '',
  `created_at`       TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `expires_at`       INT UNSIGNED NULL DEFAULT NULL,
  `active`           TINYINT UNSIGNED NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`),
  KEY `ix_bans_name_active` (`account_name`, `active`),
  KEY `ix_bans_ip_active` (`ip`, `active`),
  KEY `ix_bans_account` (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Veicoli personali (migrazione 004).
CREATE TABLE IF NOT EXISTS `vehicles` (
  `id`         INT UNSIGNED      NOT NULL AUTO_INCREMENT,
  `owner_id`   INT UNSIGNED      NOT NULL,
  `model`      SMALLINT UNSIGNED NOT NULL,
  `plate`      CHAR(7)           NOT NULL,
  `color1`     TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `color2`     TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `price_paid` INT UNSIGNED      NOT NULL DEFAULT 0,
  `pos_x`      FLOAT             NOT NULL DEFAULT 0,
  `pos_y`      FLOAT             NOT NULL DEFAULT 0,
  `pos_z`      FLOAT             NOT NULL DEFAULT 0,
  `pos_a`      FLOAT             NOT NULL DEFAULT 0,
  `park_x`     FLOAT             NOT NULL DEFAULT 0,
  `park_y`     FLOAT             NOT NULL DEFAULT 0,
  `park_z`     FLOAT             NOT NULL DEFAULT 0,
  `park_a`     FLOAT             NOT NULL DEFAULT 0,
  `interior`   SMALLINT          NOT NULL DEFAULT 0,
  `world`      SMALLINT          NOT NULL DEFAULT 0,
  `health`     FLOAT             NOT NULL DEFAULT 1000,
  `fuel_cl`    INT UNSIGNED      NOT NULL DEFAULT 0,
  `locked`     TINYINT(1)        NOT NULL DEFAULT 1,
  `destroyed`  TINYINT(1)        NOT NULL DEFAULT 0,
  `deleted`    TINYINT(1)        NOT NULL DEFAULT 0,
  `created_at` TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_vehicles_plate` (`plate`),
  KEY `ix_vehicles_owner_deleted` (`owner_id`, `deleted`),
  CONSTRAINT `fk_vehicles_owner` FOREIGN KEY (`owner_id`)
    REFERENCES `characters` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Case, utenze, chiavi, bollette (migrazione 005). Le case sono seminate dal gamemode (DB_SeedHouses).
-- owner_id NULL = in vendita. price_paid NULL = mai acquistata / tornata libera.
CREATE TABLE IF NOT EXISTS `houses` (
  `id`          INT UNSIGNED     NOT NULL AUTO_INCREMENT,
  `owner_id`    INT UNSIGNED     NULL DEFAULT NULL,
  `tier`        TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `interior_idx` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `ext_x`       FLOAT            NOT NULL DEFAULT 0,
  `ext_y`       FLOAT            NOT NULL DEFAULT 0,
  `ext_z`       FLOAT            NOT NULL DEFAULT 0,
  `ext_a`       FLOAT            NOT NULL DEFAULT 0,
  `price`       INT UNSIGNED     NOT NULL DEFAULT 0,
  `price_paid`  INT UNSIGNED     NULL DEFAULT NULL,
  `locked`      TINYINT(1)       NOT NULL DEFAULT 1,
  `created_at`  TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at`  TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `ix_houses_owner` (`owner_id`),
  CONSTRAINT `fk_houses_owner` FOREIGN KEY (`owner_id`)
    REFERENCES `characters` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `house_utilities` (
  `house_id`       INT UNSIGNED      NOT NULL,
  `luce_on`        TINYINT(1)        NOT NULL DEFAULT 1,
  `gas_on`         TINYINT(1)        NOT NULL DEFAULT 1,
  `luce_cut`       TINYINT(1)        NOT NULL DEFAULT 0,   -- staccata per morosita'
  `gas_cut`        TINYINT(1)        NOT NULL DEFAULT 0,
  `luce_min`       SMALLINT UNSIGNED NOT NULL DEFAULT 0,   -- minuti ON nel ciclo corrente
  `gas_min`        SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  `cycle_min`      SMALLINT UNSIGNED NOT NULL DEFAULT 0,   -- minuti del ciclo di fatturazione
  `fridge_units`   SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  `fridge_off_min` SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`house_id`),
  CONSTRAINT `fk_hutil_house` FOREIGN KEY (`house_id`)
    REFERENCES `houses` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Chiavi ospite: max 3 per casa (imposto da DB_AddKey).
CREATE TABLE IF NOT EXISTS `house_keys` (
  `house_id`   INT UNSIGNED NOT NULL,
  `char_id`    INT UNSIGNED NOT NULL,
  `created_at` TIMESTAMP    NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`house_id`, `char_id`),
  KEY `ix_hkeys_char` (`char_id`),
  CONSTRAINT `fk_hkeys_house` FOREIGN KEY (`house_id`)
    REFERENCES `houses` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT,
  CONSTRAINT `fk_hkeys_char` FOREIGN KEY (`char_id`)
    REFERENCES `characters` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- kind: 0 luce, 1 gas. cycles_age = cicli trascorsi dall'emissione.
CREATE TABLE IF NOT EXISTS `house_bills` (
  `id`         INT UNSIGNED     NOT NULL AUTO_INCREMENT,
  `house_id`   INT UNSIGNED     NOT NULL,
  `kind`       TINYINT UNSIGNED NOT NULL,
  `amount`     INT UNSIGNED     NOT NULL,
  `cycles_age` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `paid`       TINYINT(1)       NOT NULL DEFAULT 0,
  `created_at` TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `paid_at`    TIMESTAMP        NULL DEFAULT NULL,
  `pay_id`     INT UNSIGNED     NULL DEFAULT NULL,   -- id pagamento (006): min(id) delle bollette pagate insieme
  PRIMARY KEY (`id`),
  KEY `ix_hbills_house_paid` (`house_id`, `paid`),
  KEY `ix_hbills_pay` (`house_id`, `kind`, `pay_id`),
  CONSTRAINT `fk_hbills_house` FOREIGN KEY (`house_id`)
    REFERENCES `houses` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Lavori, multe e log di fazione (migrazione 007).
-- Statistiche per personaggio e lavoro: upsert a ogni turno completato (DB_JobPayout).
CREATE TABLE IF NOT EXISTS `job_stats` (
  `character_id` INT UNSIGNED     NOT NULL,
  `job`          TINYINT UNSIGNED NOT NULL,
  `runs`         INT UNSIGNED     NOT NULL DEFAULT 0,
  `earned`       BIGINT UNSIGNED  NOT NULL DEFAULT 0,
  `last_run_at`  TIMESTAMP        NULL DEFAULT NULL,
  PRIMARY KEY (`character_id`, `job`),
  CONSTRAINT `fk_job_stats_char` FOREIGN KEY (`character_id`)
    REFERENCES `characters` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Multe (denaro che sparisce: sink). Solo INSERT.
CREATE TABLE IF NOT EXISTS `fines` (
  `id`          INT UNSIGNED     NOT NULL AUTO_INCREMENT,
  `faction`     TINYINT UNSIGNED NOT NULL,
  `issuer_char` INT UNSIGNED     NOT NULL,
  `target_char` INT UNSIGNED     NOT NULL,
  `amount`      INT UNSIGNED     NOT NULL,
  `reason`      VARCHAR(64)      NOT NULL DEFAULT '',
  `created_at`  TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `ix_fines_target` (`target_char`, `id`),
  KEY `ix_fines_issuer` (`issuer_char`, `created_at`),
  CONSTRAINT `fk_fines_issuer` FOREIGN KEY (`issuer_char`)
    REFERENCES `characters` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT,
  CONSTRAINT `fk_fines_target` FOREIGN KEY (`target_char`)
    REFERENCES `characters` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Log di fazione (invite, join, promote, demote, fire, leave, set, cuff, ...). Solo INSERT. actor/target NULL = sistema/staff.
CREATE TABLE IF NOT EXISTS `faction_log` (
  `id`         INT UNSIGNED     NOT NULL AUTO_INCREMENT,
  `faction`    TINYINT UNSIGNED NOT NULL,
  `actor_char` INT UNSIGNED     NULL DEFAULT NULL,
  `target_char` INT UNSIGNED    NULL DEFAULT NULL,
  `action`     VARCHAR(24)      NOT NULL,
  `detail`     VARCHAR(64)      NOT NULL DEFAULT '',
  `created_at` TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `ix_faclog_faction` (`faction`, `created_at`),
  KEY `ix_faclog_actor` (`actor_char`),
  KEY `ix_faclog_target` (`target_char`),
  CONSTRAINT `fk_faclog_actor` FOREIGN KEY (`actor_char`)
    REFERENCES `characters` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT,
  CONSTRAINT `fk_faclog_target` FOREIGN KEY (`target_char`)
    REFERENCES `characters` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Inventario personale (migrazione 008). Le routine inv_* (inv_add, inv_remove, inv_transfer, inv_fridge, inv_set_bag, inv_undo, ...)
-- e la migrazione dati groceries -> inventory_items stanno in sql/procedures_inventory.sql (fonte unica), concatenato in coda a questo file da `scripts/db.sh build-sql`.
-- NON modificare sql/schema.sql a mano: la sorgente delle tabelle e' sql/src/schema_tables.sql.
CREATE TABLE IF NOT EXISTS `inventory_items` (
  `id`           BIGINT UNSIGNED   NOT NULL AUTO_INCREMENT,
  `character_id` INT UNSIGNED      NOT NULL,
  `slot`         TINYINT UNSIGNED  NOT NULL,
  `item_id`      SMALLINT UNSIGNED NOT NULL,
  `quantity`     SMALLINT UNSIGNED NOT NULL,
  `created_at`   TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at`   TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_invitems_slot` (`character_id`, `slot`),
  KEY `ix_invitems_item` (`character_id`, `item_id`),
  CONSTRAINT `ck_invitems_qty`  CHECK (`quantity` > 0 AND `quantity` <= 1000),
  CONSTRAINT `ck_invitems_item` CHECK (`item_id` > 0),
  CONSTRAINT `ck_invitems_slot` CHECK (`slot` < 64),
  CONSTRAINT `fk_invitems_char` FOREIGN KEY (`character_id`)
    REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `inventory_log` (
  `id`                 BIGINT UNSIGNED   NOT NULL AUTO_INCREMENT,
  `op_id`              CHAR(32)          NOT NULL,
  `ts`                 TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `kind`               VARCHAR(12)       NOT NULL,
  `character_id`       INT UNSIGNED      NOT NULL,
  `other_character_id` INT UNSIGNED      NULL DEFAULT NULL,
  `ref_id`             INT UNSIGNED      NULL DEFAULT NULL,
  `item_id`            INT UNSIGNED      NOT NULL DEFAULT 0,
  `quantity`           INT UNSIGNED      NOT NULL DEFAULT 0,
  `result`             TINYINT           NOT NULL,
  `stack_max`          SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  `slots_base`         TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `slots_bag`          TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `undone`             TINYINT(1)        NOT NULL DEFAULT 0,
  `undo_result`        TINYINT           NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_invlog_op` (`op_id`),
  KEY `ix_invlog_char` (`character_id`, `id`),
  KEY `ix_invlog_other` (`other_character_id`),
  KEY `ix_invlog_ts` (`ts`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- FONTE UNICA delle tabelle della banca (migrazione 009, docs/design/bank.md). NON includere a mano: 'scripts/db.sh build-sql' lo concatena in
-- sql/schema.sql e in sql/migrations/009_bank.sql. Additivo e idempotente (IF NOT EXISTS). Nessuna colonna nuova su `characters`.

-- Estratto conto: UNA riga per movimento di un personaggio (op_id UNIQUE = idempotenza). Un bonifico scrive 3 righe: <op>-o (xfer_out, -importo, mittente),
-- <op>-f (fee, -commissione, mittente, solo se > 0), <op>-i (xfer_in, +importo, destinatario). Lo storno di deposito/prelievo scrive <op>-u (kind 'undo',
-- ref = kind originale). I flussi di sistema (paycheck, job, faction, bill, house, vehicle, fine, refund, admin) arrivano da DB_BankLog con balance_after
-- fornito dal Pawn. channel: 0 ATM, 1 sportello, 2 sistema. flag = 1: bonifico fra personaggi con lo stesso IP.
CREATE TABLE IF NOT EXISTS `bank_ledger` (
  `id`                 BIGINT UNSIGNED  NOT NULL AUTO_INCREMENT,
  `op_id`              CHAR(40)         NOT NULL,
  `ts`                 TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `character_id`       INT UNSIGNED     NOT NULL,
  `other_character_id` INT UNSIGNED     NULL DEFAULT NULL,
  `kind`               VARCHAR(12)      NOT NULL,
  `delta`              INT              NOT NULL,
  `balance_after`      INT              NOT NULL,
  `channel`            TINYINT          NOT NULL DEFAULT 0,
  `flag`               TINYINT          NOT NULL DEFAULT 0,
  `ref`                VARCHAR(32)      NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_bankledger_op` (`op_id`),
  KEY `ix_bankledger_char_ts` (`character_id`, `ts`),
  KEY `ix_bankledger_char_kind_ts` (`character_id`, `kind`, `ts`),
  CONSTRAINT `ck_bankledger_delta` CHECK (`delta` <> 0),
  CONSTRAINT `ck_bankledger_channel` CHECK (`channel` BETWEEN 0 AND 2),
  CONSTRAINT `fk_bankledger_char` FOREIGN KEY (`character_id`)
    REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Esito delle operazioni transazionali (deposit, withdraw, xfer) INCLUSE quelle rifiutate (che non scrivono nel ledger): il Pawn legge da qui il risultato
-- committato. Registro di audit append-only, senza FK (come inventory_log). amount/fee in unita' di gioco; balance_after = saldo banca del personaggio
-- dopo l'operazione (o attuale se rifiutata); other_balance_after = saldo del destinatario (solo bonifici).
CREATE TABLE IF NOT EXISTS `bank_ops` (
  `id`                  BIGINT UNSIGNED  NOT NULL AUTO_INCREMENT,
  `op_id`               CHAR(40)         NOT NULL,
  `ts`                  TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `kind`                VARCHAR(12)      NOT NULL,
  `character_id`        INT UNSIGNED     NOT NULL,
  `other_character_id`  INT UNSIGNED     NULL DEFAULT NULL,
  `amount`              INT              NOT NULL DEFAULT 0,
  `fee`                 INT              NOT NULL DEFAULT 0,
  `channel`             TINYINT          NOT NULL DEFAULT 0,
  `result`              TINYINT          NOT NULL,
  `balance_after`       INT              NOT NULL DEFAULT 0,
  `other_balance_after` INT              NOT NULL DEFAULT 0,
  `undone`              TINYINT(1)       NOT NULL DEFAULT 0,
  `undo_result`         TINYINT          NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_bankops_op` (`op_id`),
  KEY `ix_bankops_char` (`character_id`, `id`),
  KEY `ix_bankops_ts` (`ts`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- FONTE UNICA delle routine dell'inventario (inv_room, inv_have, inv_put, inv_take, inv_log, inv_add, inv_remove, inv_transfer, inv_set_bag,
-- inv_fridge, inv_undo, inv_migrate_*). NON includere a mano: 'scripts/db.sh build-sql' lo concatena in sql/schema.sql e in
-- sql/migrations/008_inventory.sql (che restano file singoli, importabili con il client mysql/mariadb standard). Non modificare le copie.
-- Compatibile MariaDB e MySQL 8: DROP ... IF EXISTS + CREATE (CREATE OR REPLACE non esiste in MySQL). Richiede il privilegio CREATE ROUTINE
-- (+ ALTER ROUTINE per il DROP di routine gia' presenti, incluso in GRANT ALL ON <db>.*). Idempotente: si puo' rieseguire.
DELIMITER //

-- Posto disponibile (completamento degli stack esistenti + slot liberi sotto la capienza p_cap).
DROP FUNCTION IF EXISTS inv_room//
CREATE FUNCTION inv_room(p_char INT UNSIGNED, p_item INT, p_stack INT, p_cap INT) RETURNS INT
READS SQL DATA
BEGIN
  DECLARE v_sroom INT DEFAULT 0;
  DECLARE v_used INT DEFAULT 0;
  SELECT COALESCE(SUM(GREATEST(p_stack - `quantity`, 0)), 0) INTO v_sroom
    FROM `inventory_items` WHERE `character_id` = p_char AND `item_id` = p_item AND `slot` < p_cap;
  SELECT COUNT(*) INTO v_used FROM `inventory_items` WHERE `character_id` = p_char AND `slot` < p_cap;
  RETURN v_sroom + GREATEST(p_cap - v_used, 0) * p_stack;
END//

-- Quantita' posseduta di un oggetto.
DROP FUNCTION IF EXISTS inv_have//
CREATE FUNCTION inv_have(p_char INT UNSIGNED, p_item INT) RETURNS INT
READS SQL DATA
BEGIN
  DECLARE v_n INT DEFAULT 0;
  SELECT COALESCE(SUM(`quantity`), 0) INTO v_n FROM `inventory_items` WHERE `character_id` = p_char AND `item_id` = p_item;
  RETURN v_n;
END//

-- Inserisce p_qty unita': prima completa gli stack esistenti (slot crescente), poi usa il primo slot libero < p_cap.
-- Interna: il chiamante ha GIA' bloccato il personaggio e verificato inv_room >= p_qty (altrimenti errore e rollback del chiamante).
DROP PROCEDURE IF EXISTS inv_put//
CREATE PROCEDURE inv_put(IN p_char INT UNSIGNED, IN p_item INT, IN p_qty INT, IN p_stack INT, IN p_cap INT)
BEGIN
  DECLARE v_left INT DEFAULT p_qty;
  DECLARE v_slot INT;
  DECLARE v_q INT;
  fill: WHILE v_left > 0 DO
    SET v_slot = NULL;
    SELECT MIN(`slot`) INTO v_slot FROM `inventory_items`
      WHERE `character_id` = p_char AND `item_id` = p_item AND `quantity` < p_stack AND `slot` < p_cap;
    IF v_slot IS NULL THEN LEAVE fill; END IF;
    SELECT LEAST(p_stack - `quantity`, v_left) INTO v_q FROM `inventory_items` WHERE `character_id` = p_char AND `slot` = v_slot;
    UPDATE `inventory_items` SET `quantity` = `quantity` + v_q WHERE `character_id` = p_char AND `slot` = v_slot;
    SET v_left = v_left - v_q;
  END WHILE fill;
  SET v_slot = 0;
  fresh: WHILE v_left > 0 AND v_slot < p_cap DO
    IF NOT EXISTS (SELECT 1 FROM `inventory_items` WHERE `character_id` = p_char AND `slot` = v_slot) THEN
      SET v_q = LEAST(v_left, p_stack);
      INSERT INTO `inventory_items` (`character_id`, `slot`, `item_id`, `quantity`) VALUES (p_char, v_slot, p_item, v_q);
      SET v_left = v_left - v_q;
    END IF;
    SET v_slot = v_slot + 1;
  END WHILE fresh;
  IF v_left > 0 THEN SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'inv_put: capienza insufficiente'; END IF;
END//

-- Toglie p_qty unita' (dagli slot piu' alti). Interna: il chiamante ha bloccato il personaggio e verificato inv_have >= p_qty.
DROP PROCEDURE IF EXISTS inv_take//
CREATE PROCEDURE inv_take(IN p_char INT UNSIGNED, IN p_item INT, IN p_qty INT)
BEGIN
  DECLARE v_left INT DEFAULT p_qty;
  DECLARE v_slot INT;
  DECLARE v_q INT;
  WHILE v_left > 0 DO
    SET v_slot = NULL;
    SELECT MAX(`slot`) INTO v_slot FROM `inventory_items` WHERE `character_id` = p_char AND `item_id` = p_item;
    IF v_slot IS NULL THEN SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'inv_take: scorta insufficiente'; END IF;
    SELECT `quantity` INTO v_q FROM `inventory_items` WHERE `character_id` = p_char AND `slot` = v_slot;
    IF v_q <= v_left THEN
      DELETE FROM `inventory_items` WHERE `character_id` = p_char AND `slot` = v_slot;
      SET v_left = v_left - v_q;
    ELSE
      UPDATE `inventory_items` SET `quantity` = `quantity` - v_left WHERE `character_id` = p_char AND `slot` = v_slot;
      SET v_left = 0;
    END IF;
  END WHILE;
END//

-- Scrive la riga di log (e quindi l'esito) di un'operazione. Interna.
DROP PROCEDURE IF EXISTS inv_log//
CREATE PROCEDURE inv_log(IN p_op CHAR(32), IN p_kind VARCHAR(12), IN p_char INT UNSIGNED, IN p_other INT UNSIGNED, IN p_ref INT UNSIGNED,
  IN p_item INT, IN p_qty INT, IN p_res INT, IN p_stack INT, IN p_base INT, IN p_bag INT)
BEGIN
  INSERT INTO `inventory_log` (`op_id`, `kind`, `character_id`, `other_character_id`, `ref_id`, `item_id`, `quantity`, `result`, `stack_max`, `slots_base`, `slots_bag`)
  VALUES (p_op, p_kind, p_char, p_other, p_ref, GREATEST(p_item, 0), GREATEST(p_qty, 0), p_res,
          LEAST(GREATEST(p_stack, 0), 65535), LEAST(GREATEST(p_base, 0), 255), LEAST(GREATEST(p_bag, 0), 255));
END//

-- Aggiunge p_qty di p_item (kind: add, pickup, buy). Tutto o niente. p_total_max > 0: tetto di possesso totale dell'oggetto (es. 12 porzioni).
DROP PROCEDURE IF EXISTS inv_add//
CREATE PROCEDURE inv_add(IN p_op CHAR(32), IN p_char INT UNSIGNED, IN p_item INT, IN p_qty INT, IN p_stack INT, IN p_base INT, IN p_bag INT,
  IN p_total_max INT, IN p_kind VARCHAR(12))
main: BEGIN
  DECLARE v_bag INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  START TRANSACTION;
  SELECT MAX(`inv_bag`) INTO v_bag FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `inventory_log` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  IF v_bag IS NULL OR p_qty < 1 OR p_qty > 1000 OR p_item < 1 OR p_stack < 1 OR p_stack > 1000 OR p_base < 1 OR p_base > 64 OR p_bag < 0 OR p_bag > 64 THEN
    SET v_res = 0;
  ELSEIF p_total_max > 0 AND inv_have(p_char, p_item) + p_qty > p_total_max THEN
    SET v_res = 2;
  ELSEIF inv_room(p_char, p_item, p_stack, p_base + IF(v_bag = 1, p_bag, 0)) < p_qty THEN
    SET v_res = 2;
  ELSE
    CALL inv_put(p_char, p_item, p_qty, p_stack, p_base + IF(v_bag = 1, p_bag, 0));
    SET v_res = 1;
  END IF;
  CALL inv_log(p_op, p_kind, p_char, NULL, NULL, p_item, p_qty, v_res, p_stack, p_base, p_bag);
  COMMIT;
END//

-- Rimuove p_qty di p_item (kind: remove, use, drop). stack/base/bag servono solo a inv_undo (rimessa in inventario).
DROP PROCEDURE IF EXISTS inv_remove//
CREATE PROCEDURE inv_remove(IN p_op CHAR(32), IN p_char INT UNSIGNED, IN p_item INT, IN p_qty INT, IN p_stack INT, IN p_base INT, IN p_bag INT,
  IN p_kind VARCHAR(12))
main: BEGIN
  DECLARE v_bag INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  START TRANSACTION;
  SELECT MAX(`inv_bag`) INTO v_bag FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `inventory_log` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  IF v_bag IS NULL OR p_qty < 1 OR p_qty > 1000 OR p_item < 1 THEN
    SET v_res = 0;
  ELSEIF inv_have(p_char, p_item) < p_qty THEN
    SET v_res = 3;
  ELSE
    CALL inv_take(p_char, p_item, p_qty);
    SET v_res = 1;
  END IF;
  CALL inv_log(p_op, p_kind, p_char, NULL, NULL, p_item, p_qty, v_res, p_stack, p_base, p_bag);
  COMMIT;
END//

-- Trasferimento atomico giver -> receiver. Lock dei due personaggi in ordine di id crescente. Esito 2 = il destinatario non ha posto, 3 = il mittente non ha la scorta.
DROP PROCEDURE IF EXISTS inv_transfer//
CREATE PROCEDURE inv_transfer(IN p_op CHAR(32), IN p_giver INT UNSIGNED, IN p_recv INT UNSIGNED, IN p_item INT, IN p_qty INT,
  IN p_stack INT, IN p_base INT, IN p_bag INT)
main: BEGIN
  DECLARE v_gb INT;
  DECLARE v_rb INT;
  DECLARE v_lo INT UNSIGNED;
  DECLARE v_hi INT UNSIGNED;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  SET v_lo = LEAST(p_giver, p_recv);
  SET v_hi = GREATEST(p_giver, p_recv);
  START TRANSACTION;
  SELECT MAX(`inv_bag`) INTO v_gb FROM `characters` WHERE `id` = v_lo FOR UPDATE;
  IF v_hi <> v_lo THEN SELECT MAX(`inv_bag`) INTO v_rb FROM `characters` WHERE `id` = v_hi FOR UPDATE; END IF;
  SELECT MAX(`inv_bag`) INTO v_gb FROM `characters` WHERE `id` = p_giver;
  SELECT MAX(`inv_bag`) INTO v_rb FROM `characters` WHERE `id` = p_recv;
  SELECT MAX(`result`) INTO v_prev FROM `inventory_log` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  IF v_gb IS NULL OR v_rb IS NULL OR p_giver = p_recv OR p_qty < 1 OR p_qty > 1000 OR p_item < 1 OR p_stack < 1 OR p_stack > 1000
     OR p_base < 1 OR p_base > 64 OR p_bag < 0 OR p_bag > 64 THEN
    SET v_res = 0;
  ELSEIF inv_have(p_giver, p_item) < p_qty THEN
    SET v_res = 3;
  ELSEIF inv_room(p_recv, p_item, p_stack, p_base + IF(v_rb = 1, p_bag, 0)) < p_qty THEN
    SET v_res = 2;
  ELSE
    CALL inv_take(p_giver, p_item, p_qty);
    CALL inv_put(p_recv, p_item, p_qty, p_stack, p_base + IF(v_rb = 1, p_bag, 0));
    SET v_res = 1;
  END IF;
  CALL inv_log(p_op, 'give', p_giver, p_recv, NULL, p_item, p_qty, v_res, p_stack, p_base, p_bag);
  COMMIT;
END//

-- Acquisto dello zaino (flag). Esito 5 = gia' acquistato.
DROP PROCEDURE IF EXISTS inv_set_bag//
CREATE PROCEDURE inv_set_bag(IN p_op CHAR(32), IN p_char INT UNSIGNED, IN p_base INT, IN p_bag INT)
main: BEGIN
  DECLARE v_bag INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  START TRANSACTION;
  SELECT MAX(`inv_bag`) INTO v_bag FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `inventory_log` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  IF v_bag IS NULL THEN
    SET v_res = 0;
  ELSEIF v_bag = 1 THEN
    SET v_res = 5;
  ELSE
    UPDATE `characters` SET `inv_bag` = 1 WHERE `id` = p_char;
    SET v_res = 1;
  END IF;
  CALL inv_log(p_op, 'bag', p_char, NULL, NULL, 11, 1, v_res, 0, p_base, p_bag);
  COMMIT;
END//

-- Spostamento atomico inventario <-> frigo di casa (house_utilities.fridge_units). p_dir 1 = inventario -> frigo (solo proprietario),
-- 2 = frigo -> inventario (proprietario o ospite con chiave). p_fridge_cap = capienza del frigo; p_carry_max > 0 = tetto di possesso dell'oggetto.
-- Esiti: 3 scorta insufficiente (inventario o frigo), 2 inventario pieno, 6 frigo pieno, 4 negato (casa inesistente / non proprietario / senza chiave).
DROP PROCEDURE IF EXISTS inv_fridge//
CREATE PROCEDURE inv_fridge(IN p_op CHAR(32), IN p_char INT UNSIGNED, IN p_house INT UNSIGNED, IN p_item INT, IN p_qty INT, IN p_dir INT,
  IN p_stack INT, IN p_base INT, IN p_bag INT, IN p_fridge_cap INT, IN p_carry_max INT)
main: BEGIN
  DECLARE v_bag INT;
  DECLARE v_prev INT;
  DECLARE v_units INT;
  DECLARE v_owner INT UNSIGNED;
  DECLARE v_cap INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  START TRANSACTION;
  SELECT MAX(`inv_bag`) INTO v_bag FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(u.`fridge_units`), MAX(h.`owner_id`) INTO v_units, v_owner
    FROM `house_utilities` u JOIN `houses` h ON h.`id` = u.`house_id` WHERE u.`house_id` = p_house FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `inventory_log` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_cap = p_base + IF(v_bag = 1, p_bag, 0);
  IF v_bag IS NULL OR p_qty < 1 OR p_qty > 1000 OR p_item < 1 OR p_stack < 1 OR p_stack > 1000 OR p_base < 1 OR p_base > 64 OR p_bag < 0 OR p_bag > 64
     OR p_dir NOT IN (1, 2) OR p_fridge_cap < 1 OR p_fridge_cap > 65535 THEN
    SET v_res = 0;
  ELSEIF v_units IS NULL THEN
    SET v_res = 4;
  ELSEIF p_dir = 1 THEN
    IF v_owner IS NULL OR v_owner <> p_char THEN SET v_res = 4;
    ELSEIF inv_have(p_char, p_item) < p_qty THEN SET v_res = 3;
    ELSEIF v_units + p_qty > p_fridge_cap THEN SET v_res = 6;
    ELSE
      CALL inv_take(p_char, p_item, p_qty);
      UPDATE `house_utilities` SET `fridge_units` = `fridge_units` + p_qty WHERE `house_id` = p_house;
      SET v_res = 1;
    END IF;
  ELSE
    IF (v_owner IS NULL OR v_owner <> p_char) AND NOT EXISTS (SELECT 1 FROM `house_keys` WHERE `house_id` = p_house AND `char_id` = p_char) THEN SET v_res = 4;
    ELSEIF v_units < p_qty THEN SET v_res = 3;
    ELSEIF p_carry_max > 0 AND inv_have(p_char, p_item) + p_qty > p_carry_max THEN SET v_res = 2;
    ELSEIF inv_room(p_char, p_item, p_stack, v_cap) < p_qty THEN SET v_res = 2;
    ELSE
      CALL inv_put(p_char, p_item, p_qty, p_stack, v_cap);
      UPDATE `house_utilities` SET `fridge_units` = `fridge_units` - p_qty WHERE `house_id` = p_house;
      SET v_res = 1;
    END IF;
  END IF;
  CALL inv_log(p_op, IF(p_dir = 2, 'fridge_out', 'fridge_in'), p_char, NULL, p_house, p_item, p_qty, v_res, p_stack, p_base, p_bag);
  COMMIT;
END//

-- Annulla un'operazione gia' riuscita (add/pickup/buy: toglie; remove/use/drop: rimette; bag: toglie lo zaino se nessuno slot oltre la base e' occupato).
-- Idempotente (undone = 1 => nessun effetto). Esito in inventory_log.undo_result: 1 ok, 2 non c'e' posto per rimettere, 3 gli oggetti non ci sono piu',
-- 4 zaino non annullabile, 9 non annullabile (op fallita, give, fridge). Un esito diverso da 1 lascia undone = 0: si puo' ritentare. op_id sconosciuta = nessuna riga.
DROP PROCEDURE IF EXISTS inv_undo//
CREATE PROCEDURE inv_undo(IN p_op CHAR(32))
main: BEGIN
  DECLARE v_char INT UNSIGNED;
  DECLARE v_bag INT;
  DECLARE v_kind VARCHAR(12);
  DECLARE v_item INT;
  DECLARE v_qty INT;
  DECLARE v_result INT;
  DECLARE v_stack INT;
  DECLARE v_base INT;
  DECLARE v_bagn INT;
  DECLARE v_undone INT;
  DECLARE v_ur INT DEFAULT 9;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  SELECT MAX(`character_id`) INTO v_char FROM `inventory_log` WHERE `op_id` = p_op;
  IF v_char IS NULL THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`inv_bag`) INTO v_bag FROM `characters` WHERE `id` = v_char FOR UPDATE;
  SELECT MAX(`kind`), MAX(`item_id`), MAX(`quantity`), MAX(`result`), MAX(`stack_max`), MAX(`slots_base`), MAX(`slots_bag`), MAX(`undone`)
    INTO v_kind, v_item, v_qty, v_result, v_stack, v_base, v_bagn, v_undone
    FROM `inventory_log` WHERE `op_id` = p_op FOR UPDATE;
  IF v_undone = 1 THEN COMMIT; LEAVE main; END IF;
  IF v_bag IS NULL OR v_result <> 1 THEN
    SET v_ur = 9;
  ELSEIF v_kind IN ('add', 'pickup', 'buy') THEN
    IF inv_have(v_char, v_item) >= v_qty THEN CALL inv_take(v_char, v_item, v_qty); SET v_ur = 1; ELSE SET v_ur = 3; END IF;
  ELSEIF v_kind IN ('remove', 'use', 'drop') THEN
    IF inv_room(v_char, v_item, v_stack, v_base + IF(v_bag = 1, v_bagn, 0)) >= v_qty THEN
      CALL inv_put(v_char, v_item, v_qty, v_stack, v_base + IF(v_bag = 1, v_bagn, 0));
      SET v_ur = 1;
    ELSE
      SET v_ur = 2;
    END IF;
  ELSEIF v_kind = 'bag' THEN
    IF v_bag = 0 THEN SET v_ur = 1;
    ELSEIF EXISTS (SELECT 1 FROM `inventory_items` WHERE `character_id` = v_char AND `slot` >= v_base) THEN SET v_ur = 4;
    ELSE UPDATE `characters` SET `inv_bag` = 0 WHERE `id` = v_char; SET v_ur = 1;
    END IF;
  ELSE
    SET v_ur = 9;
  END IF;
  UPDATE `inventory_log` SET `undo_result` = v_ur, `undone` = IF(v_ur = 1, 1, 0) WHERE `op_id` = p_op;
  COMMIT;
END//

-- Migrazione dati: sposta characters.groceries -> inventory_items (item 1, stack 12, capienza base 12 + zaino 8). Idempotente per costruzione:
-- DECREMENTA groceries di quanto inserito, quindi una seconda esecuzione non trova nulla da spostare. Cio' che non entra (inventario pieno) RESTA in
-- groceries (nessuna perdita): si puo' richiamare CALL inv_migrate_groceries() in seguito (es. dopo l'acquisto dello zaino).
DROP PROCEDURE IF EXISTS inv_migrate_groceries//
CREATE PROCEDURE inv_migrate_groceries()
BEGIN
  DECLARE v_last INT UNSIGNED DEFAULT 0;
  DECLARE v_id INT UNSIGNED;
  DECLARE v_g INT;
  DECLARE v_bag INT;
  DECLARE v_cap INT;
  DECLARE v_move INT;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  nxt: LOOP
    SET v_id = NULL;
    SELECT MIN(`id`) INTO v_id FROM `characters` WHERE `id` > v_last AND `groceries` > 0;
    IF v_id IS NULL THEN LEAVE nxt; END IF;
    SET v_last = v_id;
    START TRANSACTION;
    SELECT MAX(`inv_bag`), MAX(`groceries`) INTO v_bag, v_g FROM `characters` WHERE `id` = v_id FOR UPDATE;
    IF v_g > 0 THEN
      SET v_cap = 12 + IF(v_bag = 1, 8, 0);
      SET v_move = LEAST(v_g, inv_room(v_id, 1, 12, v_cap));
      IF v_move > 0 THEN
        CALL inv_put(v_id, 1, v_move, 12, v_cap);
        UPDATE `characters` SET `groceries` = `groceries` - v_move WHERE `id` = v_id;
        CALL inv_log(CONCAT('mg', v_id, '-', UUID_SHORT()), 'migrate', v_id, NULL, NULL, 1, v_move, 1, 12, 12, 8);
      END IF;
    END IF;
    COMMIT;
  END LOOP nxt;
END//

-- Migrazione dati: documento d'identita' (item 10) a ogni personaggio che non lo ha (docs/design/inventory.md §7). Idempotente (salta chi ce l'ha).
DROP PROCEDURE IF EXISTS inv_migrate_idcards//
CREATE PROCEDURE inv_migrate_idcards()
BEGIN
  DECLARE v_last INT UNSIGNED DEFAULT 0;
  DECLARE v_id INT UNSIGNED;
  DECLARE v_bag INT;
  DECLARE v_cap INT;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  nxt: LOOP
    SET v_id = NULL;
    SELECT MIN(c.`id`) INTO v_id FROM `characters` c
      WHERE c.`id` > v_last AND NOT EXISTS (SELECT 1 FROM `inventory_items` i WHERE i.`character_id` = c.`id` AND i.`item_id` = 10);
    IF v_id IS NULL THEN LEAVE nxt; END IF;
    SET v_last = v_id;
    START TRANSACTION;
    SELECT MAX(`inv_bag`) INTO v_bag FROM `characters` WHERE `id` = v_id FOR UPDATE;
    SET v_cap = 12 + IF(v_bag = 1, 8, 0);
    IF NOT EXISTS (SELECT 1 FROM `inventory_items` WHERE `character_id` = v_id AND `item_id` = 10) AND inv_room(v_id, 10, 1, v_cap) >= 1 THEN
      CALL inv_put(v_id, 10, 1, 1, v_cap);
      CALL inv_log(CONCAT('mi', v_id, '-', UUID_SHORT()), 'migrate', v_id, NULL, NULL, 10, 1, 1, 1, 12, 8);
    END IF;
    COMMIT;
  END LOOP nxt;
END//


DELIMITER ;

-- FONTE UNICA delle routine della banca (bank_day_withdrawn, bank_day_xfer_out, bank_day_xfer_in, bank_entry, bank_oplog, bank_deposit, bank_withdraw,
-- bank_transfer, bank_undo). NON includere a mano: 'scripts/db.sh build-sql' lo concatena in sql/schema.sql e in sql/migrations/009_bank.sql.
-- Compatibile MariaDB e MySQL 8 (DROP ... IF EXISTS + CREATE). Idempotente. Semantica degli esiti: vedi sql/src/009_head.sql.
-- Tutte le routine scrivono characters.bank DIRETTAMENTE (il Pawn aggiorna pBank/pBankDb di conseguenza, vedi db_bank.inc): sono le uniche, insieme al
-- salvataggio a delta di DB_SaveCharacter (commutativo), a modificare quella colonna.
DELIMITER //

-- Somme sulla finestra mobile di p_hours ore (ledger). Interne ma richiamabili (es. DB_BankLimits). Un prelievo stornato (riga <op>-u) non conta.
DROP FUNCTION IF EXISTS bank_day_withdrawn//
CREATE FUNCTION bank_day_withdrawn(p_char INT UNSIGNED, p_hours INT) RETURNS BIGINT
READS SQL DATA
BEGIN
  DECLARE v BIGINT DEFAULT 0;
  SELECT COALESCE(SUM(-l.`delta`), 0) INTO v FROM `bank_ledger` l
    WHERE l.`character_id` = p_char AND l.`kind` = 'withdraw' AND l.`ts` > DATE_SUB(NOW(), INTERVAL p_hours HOUR)
      AND NOT EXISTS (SELECT 1 FROM `bank_ledger` u WHERE u.`op_id` = CONCAT(RTRIM(l.`op_id`), '-u'));
  RETURN v;
END//

DROP FUNCTION IF EXISTS bank_day_xfer_out//
CREATE FUNCTION bank_day_xfer_out(p_char INT UNSIGNED, p_hours INT) RETURNS BIGINT
READS SQL DATA
BEGIN
  DECLARE v BIGINT DEFAULT 0;
  SELECT COALESCE(SUM(-`delta`), 0) INTO v FROM `bank_ledger`
    WHERE `character_id` = p_char AND `kind` = 'xfer_out' AND `ts` > DATE_SUB(NOW(), INTERVAL p_hours HOUR);
  RETURN v;
END//

DROP FUNCTION IF EXISTS bank_day_xfer_in//
CREATE FUNCTION bank_day_xfer_in(p_char INT UNSIGNED, p_hours INT) RETURNS BIGINT
READS SQL DATA
BEGIN
  DECLARE v BIGINT DEFAULT 0;
  SELECT COALESCE(SUM(`delta`), 0) INTO v FROM `bank_ledger`
    WHERE `character_id` = p_char AND `kind` = 'xfer_in' AND `ts` > DATE_SUB(NOW(), INTERVAL p_hours HOUR);
  RETURN v;
END//

-- Riga di ledger. Interna: il chiamante ha gia' bloccato il personaggio.
DROP PROCEDURE IF EXISTS bank_entry//
CREATE PROCEDURE bank_entry(IN p_op VARCHAR(40), IN p_char INT UNSIGNED, IN p_other INT UNSIGNED, IN p_kind VARCHAR(12), IN p_delta INT,
  IN p_bal INT, IN p_channel TINYINT, IN p_flag TINYINT, IN p_ref VARCHAR(32))
BEGIN
  INSERT INTO `bank_ledger` (`op_id`, `character_id`, `other_character_id`, `kind`, `delta`, `balance_after`, `channel`, `flag`, `ref`)
  VALUES (p_op, p_char, p_other, p_kind, p_delta, p_bal, p_channel, p_flag, p_ref);
END//

-- Riga di esito (anche per le operazioni rifiutate). Interna.
DROP PROCEDURE IF EXISTS bank_oplog//
CREATE PROCEDURE bank_oplog(IN p_op VARCHAR(40), IN p_kind VARCHAR(12), IN p_char INT UNSIGNED, IN p_other INT UNSIGNED, IN p_amount INT, IN p_fee INT,
  IN p_channel TINYINT, IN p_res INT, IN p_bal INT, IN p_obal INT)
BEGIN
  INSERT INTO `bank_ops` (`op_id`, `kind`, `character_id`, `other_character_id`, `amount`, `fee`, `channel`, `result`, `balance_after`, `other_balance_after`)
  VALUES (p_op, p_kind, p_char, p_other, GREATEST(p_amount, 0), GREATEST(p_fee, 0), LEAST(GREATEST(p_channel, 0), 2), p_res, COALESCE(p_bal, 0), COALESCE(p_obal, 0));
END//

-- Deposito: bank += p_amount. p_max_op > 0: tetto per operazione (es. ATM 20000); p_cap = saldo massimo (GP_MONEY_MAX). Il contante lo toglie il Pawn.
DROP PROCEDURE IF EXISTS bank_deposit//
CREATE PROCEDURE bank_deposit(IN p_op CHAR(36), IN p_char INT UNSIGNED, IN p_amount INT, IN p_channel TINYINT, IN p_max_op INT, IN p_cap INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_bal INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `bank_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  IF v_bank IS NULL OR p_amount < 1 OR p_cap < 1 OR p_channel NOT IN (0, 1) OR p_max_op < 0 THEN
    SET v_res = 0;
  ELSEIF p_max_op > 0 AND p_amount > p_max_op THEN
    SET v_res = 4;
  ELSEIF CAST(v_bank AS SIGNED) + p_amount > p_cap THEN
    SET v_res = 2;
  ELSE
    SET v_bal = v_bank + p_amount;
    UPDATE `characters` SET `bank` = v_bal WHERE `id` = p_char;
    CALL bank_entry(p_op, p_char, NULL, 'deposit', p_amount, v_bal, p_channel, 0, NULL);
    SET v_res = 1;
  END IF;
  CALL bank_oplog(p_op, 'deposit', p_char, NULL, p_amount, 0, p_channel, v_res, v_bal, 0);
  COMMIT;
END//

-- Prelievo: bank -= p_amount. p_max_op > 0: tetto per operazione; p_max_day > 0: tetto delle uscite per prelievo nelle ultime p_hours ore.
-- Ordine dei controlli: argomenti (0), limite per operazione (4), fondi (3), limite giornaliero (5). Il contante lo accredita il Pawn.
DROP PROCEDURE IF EXISTS bank_withdraw//
CREATE PROCEDURE bank_withdraw(IN p_op CHAR(36), IN p_char INT UNSIGNED, IN p_amount INT, IN p_channel TINYINT, IN p_max_op INT, IN p_max_day INT, IN p_hours INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_bal INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `bank_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  IF v_bank IS NULL OR p_amount < 1 OR p_channel NOT IN (0, 1) OR p_max_op < 0 OR p_max_day < 0 OR p_hours < 1 THEN
    SET v_res = 0;
  ELSEIF p_max_op > 0 AND p_amount > p_max_op THEN
    SET v_res = 4;
  ELSEIF v_bank < p_amount THEN
    SET v_res = 3;
  ELSEIF p_max_day > 0 AND bank_day_withdrawn(p_char, p_hours) + p_amount > p_max_day THEN
    SET v_res = 5;
  ELSE
    SET v_bal = v_bank - p_amount;
    UPDATE `characters` SET `bank` = v_bal WHERE `id` = p_char;
    CALL bank_entry(p_op, p_char, NULL, 'withdraw', -p_amount, v_bal, p_channel, 0, NULL);
    SET v_res = 1;
  END IF;
  CALL bank_oplog(p_op, 'withdraw', p_char, NULL, p_amount, 0, p_channel, v_res, v_bal, 0);
  COMMIT;
END//

-- Bonifico atomico p_from -> p_to. Lock dei due personaggi in ordine di id crescente (niente deadlock fra A->B e B->A).
-- Commissione a carico del mittente, SOPRA l'importo: fee = GREATEST(CEIL(importo * p_fee_pct / 100), p_fee_min); il denaro commissionato sparisce (pozzo).
-- Mittente: bank -= importo + fee; destinatario: bank += importo. p_min/p_max: importo per operazione (p_max 0 = nessun tetto); p_out_day / p_in_day > 0:
-- tetti su importo uscito/entrato nelle ultime p_hours ore (ledger); p_cap = saldo massimo del destinatario; p_flag = 1 marca le righe (stesso IP).
-- Ordine dei controlli: argomenti (0), min/max (4), fondi per importo+fee (3), uscita giornaliera (5), entrata giornaliera (6), tetto del destinatario (2).
DROP PROCEDURE IF EXISTS bank_transfer//
CREATE PROCEDURE bank_transfer(IN p_op CHAR(36), IN p_from INT UNSIGNED, IN p_to INT UNSIGNED, IN p_amount INT, IN p_fee_pct INT, IN p_fee_min INT,
  IN p_min INT, IN p_max INT, IN p_out_day INT, IN p_in_day INT, IN p_cap INT, IN p_hours INT, IN p_flag TINYINT)
main: BEGIN
  DECLARE v_lo INT UNSIGNED;
  DECLARE v_hi INT UNSIGNED;
  DECLARE v_blo INT;
  DECLARE v_bhi INT;
  DECLARE v_fb INT;
  DECLARE v_tb INT;
  DECLARE v_fee BIGINT DEFAULT 0;
  DECLARE v_prev INT;
  DECLARE v_fbal INT;
  DECLARE v_tbal INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  SET v_lo = LEAST(p_from, p_to);
  SET v_hi = GREATEST(p_from, p_to);
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_blo FROM `characters` WHERE `id` = v_lo FOR UPDATE;
  IF v_hi <> v_lo THEN SELECT MAX(`bank`) INTO v_bhi FROM `characters` WHERE `id` = v_hi FOR UPDATE; END IF;
  SELECT MAX(`result`) INTO v_prev FROM `bank_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  IF p_from < p_to THEN SET v_fb = v_blo; SET v_tb = v_bhi; ELSE SET v_fb = v_bhi; SET v_tb = v_blo; END IF;
  SET v_fbal = v_fb;
  SET v_tbal = v_tb;
  IF p_amount >= 1 AND p_fee_pct BETWEEN 0 AND 100 AND p_fee_min >= 0 THEN
    SET v_fee = GREATEST((CAST(p_amount AS SIGNED) * p_fee_pct + 99) DIV 100, p_fee_min);
  END IF;
  IF v_fb IS NULL OR v_tb IS NULL OR p_from = p_to OR p_amount < 1 OR p_fee_pct < 0 OR p_fee_pct > 100 OR p_fee_min < 0 OR p_min < 0 OR p_max < 0
     OR p_out_day < 0 OR p_in_day < 0 OR p_cap < 1 OR p_hours < 1 THEN
    SET v_res = 0;
  ELSEIF p_amount < p_min OR (p_max > 0 AND p_amount > p_max) THEN
    SET v_res = 4;
  ELSEIF CAST(v_fb AS SIGNED) < p_amount + v_fee THEN
    SET v_res = 3;
  ELSEIF p_out_day > 0 AND bank_day_xfer_out(p_from, p_hours) + p_amount > p_out_day THEN
    SET v_res = 5;
  ELSEIF p_in_day > 0 AND bank_day_xfer_in(p_to, p_hours) + p_amount > p_in_day THEN
    SET v_res = 6;
  ELSEIF CAST(v_tb AS SIGNED) + p_amount > p_cap THEN
    SET v_res = 2;
  ELSE
    SET v_fbal = v_fb - p_amount - v_fee;
    SET v_tbal = v_tb + p_amount;
    UPDATE `characters` SET `bank` = v_fbal WHERE `id` = p_from;
    UPDATE `characters` SET `bank` = v_tbal WHERE `id` = p_to;
    CALL bank_entry(CONCAT(RTRIM(p_op), '-o'), p_from, p_to, 'xfer_out', -p_amount, v_fb - p_amount, 2, p_flag, NULL);
    IF v_fee > 0 THEN CALL bank_entry(CONCAT(RTRIM(p_op), '-f'), p_from, p_to, 'fee', -v_fee, v_fbal, 2, p_flag, NULL); END IF;
    CALL bank_entry(CONCAT(RTRIM(p_op), '-i'), p_to, p_from, 'xfer_in', p_amount, v_tbal, 2, p_flag, NULL);
    SET v_res = 1;
  END IF;
  CALL bank_oplog(p_op, 'xfer', p_from, p_to, p_amount, IF(v_res = 1, v_fee, 0), 2, v_res, v_fbal, v_tbal);
  COMMIT;
END//

-- Annulla un deposito o un prelievo gia' riuscito (compensazione se il Pawn non puo' completare il lato contanti o la sessione e' scaduta).
-- Deposito: toglie l'importo (esito 3 se i fondi non ci sono piu'); prelievo: lo rimette (esito 2 se supererebbe p_cap). Scrive la riga <op>-u (kind 'undo').
-- Idempotente (undone = 1 => nessun effetto). Un esito diverso da 1 lascia undone = 0: si puo' ritentare. 9 = non annullabile (op fallita, bonifico).
-- op_id sconosciuta = nessun effetto (nessuna riga).
DROP PROCEDURE IF EXISTS bank_undo//
CREATE PROCEDURE bank_undo(IN p_op CHAR(36), IN p_cap INT)
main: BEGIN
  DECLARE v_char INT UNSIGNED;
  DECLARE v_bank INT;
  DECLARE v_kind VARCHAR(12);
  DECLARE v_amount INT;
  DECLARE v_channel INT;
  DECLARE v_result INT;
  DECLARE v_undone INT;
  DECLARE v_ur INT DEFAULT 9;
  DECLARE v_bal INT;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  SELECT MAX(`character_id`) INTO v_char FROM `bank_ops` WHERE `op_id` = p_op;
  IF v_char IS NULL THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = v_char FOR UPDATE;
  SELECT MAX(`kind`), MAX(`amount`), MAX(`channel`), MAX(`result`), MAX(`undone`) INTO v_kind, v_amount, v_channel, v_result, v_undone
    FROM `bank_ops` WHERE `op_id` = p_op FOR UPDATE;
  IF v_undone = 1 THEN COMMIT; LEAVE main; END IF;
  IF v_bank IS NULL OR v_result <> 1 THEN
    SET v_ur = 9;
  ELSEIF v_kind = 'deposit' THEN
    IF v_bank >= v_amount THEN
      SET v_bal = v_bank - v_amount;
      UPDATE `characters` SET `bank` = v_bal WHERE `id` = v_char;
      CALL bank_entry(CONCAT(RTRIM(p_op), '-u'), v_char, NULL, 'undo', -v_amount, v_bal, v_channel, 0, 'deposit');
      SET v_ur = 1;
    ELSE
      SET v_ur = 3;
    END IF;
  ELSEIF v_kind = 'withdraw' THEN
    IF CAST(v_bank AS SIGNED) + v_amount <= p_cap THEN
      SET v_bal = v_bank + v_amount;
      UPDATE `characters` SET `bank` = v_bal WHERE `id` = v_char;
      CALL bank_entry(CONCAT(RTRIM(p_op), '-u'), v_char, NULL, 'undo', v_amount, v_bal, v_channel, 0, 'withdraw');
      SET v_ur = 1;
    ELSE
      SET v_ur = 2;
    END IF;
  ELSE
    SET v_ur = 9;
  END IF;
  UPDATE `bank_ops` SET `undo_result` = v_ur, `undone` = IF(v_ur = 1, 1, 0) WHERE `op_id` = p_op;
  COMMIT;
END//

DELIMITER ;

-- FONTE UNICA delle tabelle della storia del personaggio (migrazione 010, docs/design/intro.md). NON includere a mano: 'scripts/db.sh build-sql' lo
-- concatena in sql/schema.sql e in sql/migrations/010_intro_story.sql. Additivo e idempotente (IF NOT EXISTS). accounts.intro_seen sta in schema_tables.sql / 010_head.sql.

-- Provenienza e motivo (solo narrativi). Nessuna riga = non specificato. Una riga 0/0 vale "non specificato" e puo' essere impostata una volta (story_save).
CREATE TABLE IF NOT EXISTS `character_story` (
  `character_id` INT UNSIGNED     NOT NULL,
  `origin`       TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `motive`       TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `created_at`   TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`character_id`),
  CONSTRAINT `ck_story_origin` CHECK (`origin` BETWEEN 0 AND 5),
  CONSTRAINT `ck_story_motive` CHECK (`motive` BETWEEN 0 AND 4),
  CONSTRAINT `fk_story_char` FOREIGN KEY (`character_id`)
    REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Esito delle scritture della storia, INCLUSE le rifiutate: il Pawn legge da qui il risultato committato (come bank_ops). Registro di audit append-only, senza FK.
-- mode 0 = creazione (puo' ripartire i soldi iniziali), 1 = /storia imposta (mai soldi). money_delta = variazione dei contanti applicata dal DB
-- (la banca varia di -money_delta: zero-sum); money_after/bank_after = saldi del personaggio dopo l'operazione.
CREATE TABLE IF NOT EXISTS `story_ops` (
  `id`           BIGINT UNSIGNED  NOT NULL AUTO_INCREMENT,
  `op_id`        CHAR(36)         NOT NULL,
  `ts`           TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `character_id` INT UNSIGNED     NOT NULL,
  `mode`         TINYINT          NOT NULL DEFAULT 0,
  `origin`       TINYINT          NOT NULL DEFAULT 0,
  `motive`       TINYINT          NOT NULL DEFAULT 0,
  `result`       TINYINT          NOT NULL,
  `money_delta`  INT              NOT NULL DEFAULT 0,
  `money_after`  INT              NOT NULL DEFAULT 0,
  `bank_after`   INT              NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_storyops_op` (`op_id`),
  KEY `ix_storyops_char` (`character_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- FONTE UNICA delle routine della storia (story_start_total, story_motive_cash, story_save). NON includere a mano: 'scripts/db.sh build-sql' lo concatena in
-- sql/schema.sql e in sql/migrations/010_intro_story.sql. Compatibile MariaDB e MySQL 8 (DROP ... IF EXISTS + CREATE). Idempotente.
-- Semantica degli esiti: vedi sql/src/010_head.sql. Tabella contanti per motivo (docs/design/intro.md §4), totale sempre 7.500:
--   motivo 0/1 = 2.500 contanti / 5.000 banca, 2 = 2.000/5.500, 3 = 3.500/4.000, 4 = 3.000/4.500.
DELIMITER //

DROP FUNCTION IF EXISTS story_start_total//
CREATE FUNCTION story_start_total() RETURNS INT DETERMINISTIC
BEGIN
  RETURN 7500;
END//

DROP FUNCTION IF EXISTS story_motive_cash//
CREATE FUNCTION story_motive_cash(p_motive INT) RETURNS INT DETERMINISTIC
BEGIN
  RETURN CASE p_motive WHEN 2 THEN 2000 WHEN 3 THEN 3500 WHEN 4 THEN 3000 ELSE 2500 END;
END//

-- Salva la storia di un personaggio. p_mode 0 = creazione (se il personaggio e' appena nato, ha totale esatto 7.500 e non e' gia' nella ripartizione del
-- motivo, il DB sposta contanti<->banca in modo zero-sum), 1 = /storia imposta (mai soldi; richiede origin o motive != 0).
-- "Appena nato" = creato da non piu' di 30 minuti. Una storia gia' impostata (riga con origin o motive != 0) non si cambia mai: esito 2.
DROP PROCEDURE IF EXISTS story_save//
CREATE PROCEDURE story_save(IN p_op CHAR(36), IN p_char INT UNSIGNED, IN p_origin INT, IN p_motive INT, IN p_mode INT)
main: BEGIN
  DECLARE v_money INT;
  DECLARE v_bank INT;
  DECLARE v_age INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 0;
  DECLARE v_have INT DEFAULT 0;
  DECLARE v_o INT DEFAULT 0;
  DECLARE v_m INT DEFAULT 0;
  DECLARE v_cash INT;
  DECLARE v_delta INT DEFAULT 0;
  DECLARE v_mon_after INT;
  DECLARE v_bank_after INT;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`money`), MAX(`bank`), MAX(TIMESTAMPDIFF(MINUTE, `created_at`, NOW())) INTO v_money, v_bank, v_age FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `story_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_mon_after = COALESCE(v_money, 0);
  SET v_bank_after = COALESCE(v_bank, 0);
  IF v_money IS NULL OR p_origin IS NULL OR p_motive IS NULL OR p_mode IS NULL OR p_origin NOT BETWEEN 0 AND 5 OR p_motive NOT BETWEEN 0 AND 4
     OR p_mode NOT IN (0, 1) OR (p_mode = 1 AND p_origin = 0 AND p_motive = 0) THEN
    SET v_res = 0;
  ELSE
    SELECT COUNT(*), COALESCE(MAX(`origin`), 0), COALESCE(MAX(`motive`), 0) INTO v_have, v_o, v_m FROM `character_story` WHERE `character_id` = p_char FOR UPDATE;
    IF v_have = 1 AND (v_o <> 0 OR v_m <> 0) THEN
      SET v_res = 2;
    ELSE
      IF v_have = 0 THEN
        INSERT INTO `character_story` (`character_id`, `origin`, `motive`) VALUES (p_char, p_origin, p_motive);
      ELSE
        UPDATE `character_story` SET `origin` = p_origin, `motive` = p_motive, `created_at` = NOW() WHERE `character_id` = p_char;
      END IF;
      SET v_res = 1;
      IF p_mode = 0 THEN
        SET v_cash = story_motive_cash(p_motive);
        IF CAST(v_money AS SIGNED) + v_bank <> story_start_total() OR v_age > 30 THEN
          SET v_res = 4;
        ELSEIF v_money <> v_cash THEN
          SET v_delta = v_cash - v_money;
          SET v_mon_after = v_cash;
          SET v_bank_after = v_bank - v_delta;
          UPDATE `characters` SET `money` = v_mon_after, `bank` = v_bank_after WHERE `id` = p_char;
          SET v_res = 3;
        END IF;
      END IF;
    END IF;
  END IF;
  INSERT INTO `story_ops` (`op_id`, `character_id`, `mode`, `origin`, `motive`, `result`, `money_delta`, `money_after`, `bank_after`)
  VALUES (p_op, p_char, LEAST(GREATEST(COALESCE(p_mode, 0), 0), 1), LEAST(GREATEST(COALESCE(p_origin, 0), 0), 5), LEAST(GREATEST(COALESCE(p_motive, 0), 0), 4),
    v_res, v_delta, v_mon_after, v_bank_after);
  COMMIT;
END//

DELIMITER ;

-- FONTE UNICA delle tabelle di Living Los Santos (migrazione 011, docs/design/living_los_santos.md §8). NON includere a mano: 'scripts/db.sh build-sql'
-- lo concatena in sql/schema.sql e in sql/migrations/011_living_los_santos.sql. Additivo e idempotente (IF NOT EXISTS, seed INSERT IGNORE).
-- Nessuna colonna nuova su `characters`. Richiede 009 (bank_ledger + bank_entry): il denaro si muove SOLO su characters.bank con riga in bank_ledger.

-- Parametri di gioco usati dalle routine (valori del design §9). Le routine leggono SOLO da qui (ld_cfg): niente numeri magici nel SQL.
-- Il seed non sovrascrive mai un valore gia' presente; il Pawn puo' riallinearli a gameplay_config.inc con DB_LivingPushConfig (REPLACE).
CREATE TABLE IF NOT EXISTS `ld_config` (
  `k`          VARCHAR(40)      NOT NULL,
  `v`          INT              NOT NULL,
  `updated_at` TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`k`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Quartieri: stato persistente condiviso. base_* = valore verso cui il tick riporta attivita' e pressione. fest_until = festa di quartiere (attivita' minima).
CREATE TABLE IF NOT EXISTS `ld_districts` (
  `id`            TINYINT UNSIGNED  NOT NULL,
  `code`          VARCHAR(16)       NOT NULL,
  `name`          VARCHAR(32)       NOT NULL,
  `activity`      SMALLINT          NOT NULL,
  `security`      TINYINT           NOT NULL,
  `crime`         TINYINT           NOT NULL,
  `base_activity` SMALLINT          NOT NULL,
  `base_crime`    TINYINT           NOT NULL,
  `last_tick_at`  TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ticks`         INT UNSIGNED      NOT NULL DEFAULT 0,
  `fest_until`    TIMESTAMP         NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_lddistricts_code` (`code`),
  CONSTRAINT `ck_lddistricts_activity` CHECK (`activity` BETWEEN 0 AND 1000),
  CONSTRAINT `ck_lddistricts_security` CHECK (`security` BETWEEN 0 AND 100),
  CONSTRAINT `ck_lddistricts_crime` CHECK (`crime` BETWEEN 0 AND 100)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Reputazione locale per personaggio e quartiere (-50..+100). day_key = TO_DAYS del giorno di day_gain (tetto giornaliero dei guadagni).
CREATE TABLE IF NOT EXISTS `ld_district_rep` (
  `character_id` INT UNSIGNED      NOT NULL,
  `district_id`  TINYINT UNSIGNED  NOT NULL,
  `rep`          SMALLINT          NOT NULL DEFAULT 0,
  `day_gain`     SMALLINT          NOT NULL DEFAULT 0,
  `day_key`      INT               NOT NULL DEFAULT 0,
  PRIMARY KEY (`character_id`, `district_id`),
  KEY `ix_lddrep_district` (`district_id`),
  CONSTRAINT `fk_lddrep_char` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `fk_lddrep_district` FOREIGN KEY (`district_id`) REFERENCES `ld_districts` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Cronologia eventi di dominio (append-only). op_id UNIQUE = un evento per operazione. district_id 0 = evento cittadino (noleggi).
-- type: 1 CONSEGNA_COMPLETATA, 2 CONTRATTO_FALLITO, 3 CORSA_COMPLETATA, 4 NOLEGGIO_APERTO, 5 NOLEGGIO_CHIUSO, 6 CRIMINE_SEGNALATO, 7 PATTUGLIA, 8 COMUNITA', 9 ADMIN.
-- act_delta = variazione di attivita' APPLICATA (dopo i tetti): la somma dei positivi con actor_char nell'ultima ora e' il tetto anti-farming.
-- counted = 1 se l'evento ha prodotto effetti di quartiere (consegna "contabilizzata": max GP_LD_DELIV_HOURLY_PER_CHAR/h per personaggio e quartiere).
CREATE TABLE IF NOT EXISTS `ld_events` (
  `id`          BIGINT UNSIGNED   NOT NULL AUTO_INCREMENT,
  `op_id`       CHAR(40)          NOT NULL,
  `district_id` TINYINT UNSIGNED  NOT NULL,
  `type`        TINYINT UNSIGNED  NOT NULL,
  `actor_char`  INT UNSIGNED      NULL DEFAULT NULL,
  `value`       INT               NOT NULL DEFAULT 0,
  `act_delta`   SMALLINT          NOT NULL DEFAULT 0,
  `counted`     TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `ref`         BIGINT UNSIGNED   NULL DEFAULT NULL,
  `created_at`  TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_ldevents_op` (`op_id`),
  KEY `ix_ldevents_district` (`district_id`, `id`),
  KEY `ix_ldevents_district_ts` (`district_id`, `created_at`),
  KEY `ix_ldevents_actor` (`actor_char`, `district_id`, `type`, `created_at`),
  CONSTRAINT `ck_ldevents_type` CHECK (`type` BETWEEN 1 AND 9)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Reputazione professionale. track: 1 trasporti, 2 taxi. rating_* = totali storici (la media mobile sugli ultimi N voti si legge da taxi_rides).
CREATE TABLE IF NOT EXISTS `pro_rep` (
  `character_id` INT UNSIGNED      NOT NULL,
  `track`        TINYINT UNSIGNED  NOT NULL,
  `points`       INT               NOT NULL DEFAULT 0,
  `completed`    INT UNSIGNED      NOT NULL DEFAULT 0,
  `failed`       INT UNSIGNED      NOT NULL DEFAULT 0,
  `rating_sum`   INT UNSIGNED      NOT NULL DEFAULT 0,
  `rating_cnt`   INT UNSIGNED      NOT NULL DEFAULT 0,
  `day_key`      INT               NOT NULL DEFAULT 0,
  `day_gain`     SMALLINT          NOT NULL DEFAULT 0,
  PRIMARY KEY (`character_id`, `track`),
  CONSTRAINT `ck_prorep_track` CHECK (`track` IN (1, 2)),
  CONSTRAINT `ck_prorep_points` CHECK (`points` >= 0),
  CONSTRAINT `fk_prorep_char` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Contratti del Director. slot_key "b<bucket>s<i>" UNIQUE: rigenerare non duplica. kind 1 FREIGHT (2 riservato).
-- state: 0 APERTO, 1 ACCETTATO, 2 IN_CORSO, 3 COMPLETATO, 4 FALLITO, 5 SCADUTO, 6 ANNULLATO, 7 RILASCIATO (riapribile: come APERTO).
-- reward = valore di bacheca alla creazione (senza bonus personali); il compenso effettivo lo ricalcola sp_ct_complete. limit_s = tempo per la puntualita'.
CREATE TABLE IF NOT EXISTS `ct_contracts` (
  `id`               BIGINT UNSIGNED   NOT NULL AUTO_INCREMENT,
  `slot_key`         CHAR(24)          NOT NULL,
  `bucket`           INT UNSIGNED      NOT NULL,
  `kind`             TINYINT UNSIGNED  NOT NULL DEFAULT 1,
  `state`            TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `from_pt`          TINYINT UNSIGNED  NOT NULL,
  `to_pt`            TINYINT UNSIGNED  NOT NULL,
  `from_district`    TINYINT UNSIGNED  NOT NULL,
  `to_district`      TINYINT UNSIGNED  NOT NULL,
  `cargo`            TINYINT UNSIGNED  NOT NULL,
  `distance_m`       INT UNSIGNED      NOT NULL,
  `limit_s`          INT UNSIGNED      NOT NULL,
  `min_level`        TINYINT UNSIGNED  NOT NULL,
  `min_rep`          INT UNSIGNED      NOT NULL DEFAULT 0,
  `min_local_rep`    SMALLINT          NOT NULL DEFAULT 0,
  `premium`          TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `max_participants` TINYINT UNSIGNED  NOT NULL DEFAULT 1,
  `participants`     TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `reward`           INT               NOT NULL,
  `created_at`       TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `expires_at`       TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `accepted_at`      TIMESTAMP         NULL DEFAULT NULL,
  `picked_at`        TIMESTAMP         NULL DEFAULT NULL,
  `done_at`          TIMESTAMP         NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_ctcontracts_slot` (`slot_key`),
  KEY `ix_ctcontracts_state` (`state`, `expires_at`),
  CONSTRAINT `ck_ctcontracts_state` CHECK (`state` BETWEEN 0 AND 7),
  CONSTRAINT `ck_ctcontracts_part` CHECK (`participants` <= `max_participants`),
  CONSTRAINT `fk_ctcontracts_from` FOREIGN KEY (`from_district`) REFERENCES `ld_districts` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT,
  CONSTRAINT `fk_ctcontracts_to` FOREIGN KEY (`to_district`) REFERENCES `ld_districts` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Partecipanti. state: 1 attivo, 2 rilasciato, 3 completato, 4 fallito. active_char (generata) UNIQUE = UN contratto attivo per personaggio anche sotto concorrenza.
CREATE TABLE IF NOT EXISTS `ct_participants` (
  `contract_id`  BIGINT UNSIGNED   NOT NULL,
  `character_id` INT UNSIGNED      NOT NULL,
  `state`        TINYINT UNSIGNED  NOT NULL DEFAULT 1,
  `attempts`     TINYINT UNSIGNED  NOT NULL DEFAULT 1,
  `joined_at`    TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `released_at`  TIMESTAMP         NULL DEFAULT NULL,
  `active_char`  INT UNSIGNED      AS (IF(`state` = 1, `character_id`, NULL)) VIRTUAL,
  PRIMARY KEY (`contract_id`, `character_id`),
  UNIQUE KEY `uq_ctpart_active` (`active_char`),
  KEY `ix_ctpart_char` (`character_id`, `joined_at`),
  CONSTRAINT `ck_ctpart_state` CHECK (`state` BETWEEN 1 AND 4),
  CONSTRAINT `fk_ctpart_contract` FOREIGN KEY (`contract_id`) REFERENCES `ct_contracts` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `fk_ctpart_char` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Pagamenti: PK (contract_id, character_id) + op_id UNIQUE = ricompensa doppia impossibile. amount = pagato (dopo il tetto orario), reward = calcolato.
-- Registro di audit senza FK (come bank_ops). amount entra nel tetto orario GP_LD_WORK_HOURLY_CAP (somma su 60 min mobili, con taxi_rides NPC).
CREATE TABLE IF NOT EXISTS `ct_payouts` (
  `contract_id`  BIGINT UNSIGNED   NOT NULL,
  `character_id` INT UNSIGNED      NOT NULL,
  `op_id`        CHAR(40)          NOT NULL,
  `amount`       INT               NOT NULL,
  `reward`       INT               NOT NULL,
  `capped`       TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `integrity`    TINYINT UNSIGNED  NOT NULL DEFAULT 100,
  `seconds`      INT UNSIGNED      NOT NULL DEFAULT 0,
  `paid_at`      TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`contract_id`, `character_id`),
  UNIQUE KEY `uq_ctpayouts_op` (`op_id`),
  KEY `ix_ctpayouts_char_ts` (`character_id`, `paid_at`),
  CONSTRAINT `ck_ctpayouts_amount` CHECK (`amount` >= 0 AND `amount` <= `reward`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Noleggi. class 1 personale, 2 professionale (taxi). vkind 1 Faggio, 2 Previon, 3 Manana, 4 Taxi, 5 auto di strada (013). state: 1 aperto, 2 chiuso (riconsegna),
-- 3 interrotto (disconnessione/morte/riavvio: cauzione 100%), 4 scaduto (cauzione persa), 5 annullato (rimborso totale: compensazione).
-- fee/deposit = addebitati; cash_paid = parte pagata in contanti (il resto in banca). open_char (generata) UNIQUE = un solo noleggio aperto per personaggio.
CREATE TABLE IF NOT EXISTS `rentals` (
  `id`           BIGINT UNSIGNED   NOT NULL AUTO_INCREMENT,
  `character_id` INT UNSIGNED      NOT NULL,
  `class`        TINYINT UNSIGNED  NOT NULL,
  `vkind`        TINYINT UNSIGNED  NOT NULL,
  `model`        SMALLINT UNSIGNED NOT NULL DEFAULT 0,
  `blocks`       TINYINT UNSIGNED  NOT NULL,
  `fee`          INT               NOT NULL,
  `deposit`      INT               NOT NULL,
  `cash_paid`    INT               NOT NULL DEFAULT 0,
  `state`        TINYINT UNSIGNED  NOT NULL DEFAULT 1,
  `opened_at`    TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `due_at`       TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `closed_at`    TIMESTAMP         NULL DEFAULT NULL,
  `refund`       INT               NOT NULL DEFAULT 0,
  `open_op`      CHAR(40)          NOT NULL,
  `close_op`     CHAR(40)          NULL DEFAULT NULL,
  `open_char`    INT UNSIGNED      AS (IF(`state` = 1, `character_id`, NULL)) VIRTUAL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_rentals_open_op` (`open_op`),
  UNIQUE KEY `uq_rentals_close_op` (`close_op`),
  UNIQUE KEY `uq_rentals_open_char` (`open_char`),
  KEY `ix_rentals_state` (`state`, `due_at`),
  KEY `ix_rentals_char` (`character_id`, `id`),
  CONSTRAINT `ck_rentals_state` CHECK (`state` BETWEEN 1 AND 5),
  CONSTRAINT `ck_rentals_money` CHECK (`fee` >= 0 AND `deposit` >= 0 AND `cash_paid` >= 0 AND `refund` >= 0)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Corse taxi. kind 1 NPC (creazione di denaro, nel tetto orario), 2 fra giocatori (trasferimento: passeggero -> 90% autista, 10% pozzo).
-- state: 1 aperta, 2 pagata, 3 disputa, 4 annullata. fare = pagato dal passeggero (NPC: tariffa+mancia dopo il tetto), tax = pozzo, driver_net = all'autista.
-- open_driver (generata) UNIQUE = una corsa aperta per autista.
CREATE TABLE IF NOT EXISTS `taxi_rides` (
  `id`             BIGINT UNSIGNED   NOT NULL AUTO_INCREMENT,
  `kind`           TINYINT UNSIGNED  NOT NULL,
  `driver_char`    INT UNSIGNED      NOT NULL,
  `passenger_char` INT UNSIGNED      NULL DEFAULT NULL,
  `from_district`  TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `to_district`    TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `distance_m`     INT UNSIGNED      NOT NULL DEFAULT 0,
  `fare`           INT               NOT NULL DEFAULT 0,
  `tax`            INT               NOT NULL DEFAULT 0,
  `driver_net`     INT               NOT NULL DEFAULT 0,
  `state`          TINYINT UNSIGNED  NOT NULL DEFAULT 1,
  `rating`         TINYINT UNSIGNED  NULL DEFAULT NULL,
  `op_id`          CHAR(40)          NOT NULL,
  `pay_op`         CHAR(40)          NULL DEFAULT NULL,
  `created_at`     TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `paid_at`        TIMESTAMP         NULL DEFAULT NULL,
  `open_driver`    INT UNSIGNED      AS (IF(`state` = 1, `driver_char`, NULL)) VIRTUAL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_taxirides_op` (`op_id`),
  UNIQUE KEY `uq_taxirides_pay_op` (`pay_op`),
  UNIQUE KEY `uq_taxirides_open_driver` (`open_driver`),
  KEY `ix_taxirides_driver_ts` (`driver_char`, `kind`, `state`, `paid_at`),
  KEY `ix_taxirides_pass_ts` (`passenger_char`, `created_at`),
  CONSTRAINT `ck_taxirides_kind` CHECK (`kind` IN (1, 2)),
  CONSTRAINT `ck_taxirides_state` CHECK (`state` BETWEEN 1 AND 4),
  CONSTRAINT `ck_taxirides_rating` CHECK (`rating` IS NULL OR `rating` BETWEEN 1 AND 5)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Catalogo dei progetti comunitari per quartiere (testi originali): il progetto successivo prende la voce seguente (ciclica).
CREATE TABLE IF NOT EXISTS `ld_project_catalog` (
  `district_id` TINYINT UNSIGNED  NOT NULL,
  `seq`         TINYINT UNSIGNED  NOT NULL,
  `name`        VARCHAR(64)       NOT NULL,
  PRIMARY KEY (`district_id`, `seq`),
  CONSTRAINT `fk_ldpcat_district` FOREIGN KEY (`district_id`) REFERENCES `ld_districts` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Progetti comunitari dell'hub. state: 1 attivo, 2 completato, 3 scaduto. fund in decimi di dollaro (DECIMAL: +1,5$ per corsa).
-- active_district (generata) UNIQUE = un solo progetto attivo per quartiere.
CREATE TABLE IF NOT EXISTS `ld_projects` (
  `id`              INT UNSIGNED      NOT NULL AUTO_INCREMENT,
  `district_id`     TINYINT UNSIGNED  NOT NULL,
  `seq`             TINYINT UNSIGNED  NOT NULL DEFAULT 1,
  `name`            VARCHAR(64)       NOT NULL,
  `goal`            INT               NOT NULL,
  `fund`            DECIMAL(12,1)     NOT NULL DEFAULT 0.0,
  `state`           TINYINT UNSIGNED  NOT NULL DEFAULT 1,
  `created_at`      TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `ends_at`         TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `done_at`         TIMESTAMP         NULL DEFAULT NULL,
  `active_district` TINYINT UNSIGNED  AS (IF(`state` = 1, `district_id`, NULL)) VIRTUAL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_ldprojects_active` (`active_district`),
  KEY `ix_ldprojects_district` (`district_id`, `id`),
  CONSTRAINT `ck_ldprojects_state` CHECK (`state` BETWEEN 1 AND 3),
  CONSTRAINT `fk_ldprojects_district` FOREIGN KEY (`district_id`) REFERENCES `ld_districts` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Contributi ai progetti. points = punti comunita' (1 ogni 100$ donati + 1 per contratto/corsa da/per il quartiere), donated = $ donati.
-- day_key/day_points/day_donated = tetti giornalieri (sommati su tutti i progetti del personaggio nello stesso giorno).
CREATE TABLE IF NOT EXISTS `ld_project_contrib` (
  `project_id`   INT UNSIGNED      NOT NULL,
  `character_id` INT UNSIGNED      NOT NULL,
  `points`       INT               NOT NULL DEFAULT 0,
  `donated`      INT               NOT NULL DEFAULT 0,
  `day_key`      INT               NOT NULL DEFAULT 0,
  `day_points`   INT               NOT NULL DEFAULT 0,
  `day_donated`  INT               NOT NULL DEFAULT 0,
  `updated_at`   TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`project_id`, `character_id`),
  KEY `ix_ldpcontrib_char` (`character_id`, `day_key`),
  CONSTRAINT `fk_ldpcontrib_project` FOREIGN KEY (`project_id`) REFERENCES `ld_projects` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT,
  CONSTRAINT `fk_ldpcontrib_char` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Onboarding del primo giorno. step = ultimo passo completato (0..5); done_at = catena finita (o saltata); hints_off = suggerimenti spenti;
-- rewarded = bit dei premi una tantum consegnati (1 = consegna di benvenuto pagata, 2 = pacco accoglienza finale).
CREATE TABLE IF NOT EXISTS `ld_onboarding` (
  `character_id` INT UNSIGNED      NOT NULL,
  `step`         TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `done_at`      TIMESTAMP         NULL DEFAULT NULL,
  `hints_off`    TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `rewarded`     TINYINT UNSIGNED  NOT NULL DEFAULT 0,
  `updated_at`   TIMESTAMP         NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`character_id`),
  CONSTRAINT `ck_ldonb_step` CHECK (`step` BETWEEN 0 AND 5),
  CONSTRAINT `fk_ldonb_char` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Esito di OGNI operazione con op_id (anche rifiutata): il Pawn lo legge da qui dopo la CALL (come bank_ops). Append-only, senza FK.
-- kind: ct_accept ct_pickup ct_release ct_complete rent_open rent_close taxi_start taxi_pay taxi_rate prj_contrib onboard ld_emit ld_admin.
-- ref = id dell'oggetto (contratto/noleggio/corsa/progetto). amount/balance_after/other_* e v1..v3: significato per kind in db_living.inc.
CREATE TABLE IF NOT EXISTS `ld_ops` (
  `id`                  BIGINT UNSIGNED  NOT NULL AUTO_INCREMENT,
  `op_id`               CHAR(40)         NOT NULL,
  `ts`                  TIMESTAMP        NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `kind`                VARCHAR(12)      NOT NULL,
  `character_id`        INT UNSIGNED     NOT NULL DEFAULT 0,
  `other_character_id`  INT UNSIGNED     NOT NULL DEFAULT 0,
  `ref`                 BIGINT UNSIGNED  NOT NULL DEFAULT 0,
  `result`              TINYINT          NOT NULL,
  `amount`              INT              NOT NULL DEFAULT 0,
  `balance_after`       INT              NOT NULL DEFAULT 0,
  `other_amount`        INT              NOT NULL DEFAULT 0,
  `other_balance_after` INT              NOT NULL DEFAULT 0,
  `v1`                  INT              NOT NULL DEFAULT 0,
  `v2`                  INT              NOT NULL DEFAULT 0,
  `v3`                  INT              NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_ldops_op` (`op_id`),
  KEY `ix_ldops_char` (`character_id`, `id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- FONTE UNICA delle routine di Living Los Santos (migrazione 011, docs/design/living_los_santos.md §8). NON includere a mano: 'scripts/db.sh build-sql'
-- lo concatena in sql/schema.sql e in sql/migrations/011_living_los_santos.sql. Compatibile MariaDB 10.11 / MySQL 8 (DROP ... IF EXISTS + CREATE). Idempotente.
-- Semantica degli esiti: sql/src/011_head.sql. Parametri di gioco: SOLO da ld_config (ld_cfg), mai numeri magici.
-- Denaro: SOLO characters.bank + riga bank_ledger (bank_entry di 009), kind 'contract' | 'rental' | 'taxi' | 'project' | 'onboarding', channel 2 (sistema).
-- Ordine dei lock (niente deadlock): characters (id crescente) -> ct_contracts / rentals / taxi_rides -> ct_participants -> ld_districts (id crescente) -> ld_projects.
DELIMITER //

-- --- funzioni ---------------------------------------------------------------------------------------------------------

-- Valore di un parametro; chiave mancante = errore (meglio fallire che usare 0).
DROP FUNCTION IF EXISTS ld_cfg//
CREATE FUNCTION ld_cfg(p_k VARCHAR(40)) RETURNS INT
READS SQL DATA
BEGIN
  DECLARE r_val INT;
  DECLARE r_msg VARCHAR(80);
  SELECT MAX(c.`v`) INTO r_val FROM `ld_config` c WHERE c.`k` = p_k;
  IF r_val IS NULL THEN
    SET r_msg = CONCAT('ld_config: chiave mancante ', p_k);
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = r_msg;
  END IF;
  RETURN r_val;
END//

-- Bonus % di grado professionale (trasporti: reward; taxi: mancia NPC).
DROP FUNCTION IF EXISTS ld_tier_pct//
CREATE FUNCTION ld_tier_pct(p_points INT) RETURNS INT
READS SQL DATA
BEGIN
  IF p_points >= ld_cfg('PRO_TIER3') THEN RETURN ld_cfg('PRO_BONUS3_PCT'); END IF;
  IF p_points >= ld_cfg('PRO_TIER2') THEN RETURN ld_cfg('PRO_BONUS2_PCT'); END IF;
  IF p_points >= ld_cfg('PRO_TIER1') THEN RETURN ld_cfg('PRO_BONUS1_PCT'); END IF;
  RETURN 0;
END//

DROP FUNCTION IF EXISTS ld_pro_points//
CREATE FUNCTION ld_pro_points(p_char INT UNSIGNED, p_track INT) RETURNS INT
READS SQL DATA
BEGIN
  DECLARE v INT;
  SELECT MAX(`points`) INTO v FROM `pro_rep` WHERE `character_id` = p_char AND `track` = p_track;
  RETURN COALESCE(v, 0);
END//

DROP FUNCTION IF EXISTS ld_local_rep//
CREATE FUNCTION ld_local_rep(p_char INT UNSIGNED, p_district INT) RETURNS INT
READS SQL DATA
BEGIN
  DECLARE v INT;
  SELECT MAX(`rep`) INTO v FROM `ld_district_rep` WHERE `character_id` = p_char AND `district_id` = p_district;
  RETURN COALESCE(v, 0);
END//

-- Sconto % sui POZZI (canone noleggio personale, tariffa taxi pagata) dalla migliore reputazione locale del personaggio.
DROP FUNCTION IF EXISTS ld_disc_pct//
CREATE FUNCTION ld_disc_pct(p_char INT UNSIGNED) RETURNS INT
READS SQL DATA
BEGIN
  DECLARE v INT;
  SELECT COALESCE(MAX(`rep`), 0) INTO v FROM `ld_district_rep` WHERE `character_id` = p_char;
  IF v >= ld_cfg('LD_REP_HOME') THEN RETURN ld_cfg('LD_DISC_HOME_PCT'); END IF;
  IF v >= ld_cfg('LD_REP_KNOWN') THEN RETURN ld_cfg('LD_DISC_KNOWN_PCT'); END IF;
  RETURN 0;
END//

-- Guadagno "lavoro dinamico" (creazione di denaro) del personaggio negli ultimi 60 min: contratti pagati + corse taxi NPC pagate. Resiste ai riavvii.
DROP FUNCTION IF EXISTS ld_work_earned//
CREATE FUNCTION ld_work_earned(p_char INT UNSIGNED) RETURNS INT
READS SQL DATA
BEGIN
  DECLARE a BIGINT DEFAULT 0;
  DECLARE b BIGINT DEFAULT 0;
  SELECT COALESCE(SUM(`amount`), 0) INTO a FROM `ct_payouts` WHERE `character_id` = p_char AND `paid_at` > DATE_SUB(NOW(), INTERVAL 1 HOUR);
  SELECT COALESCE(SUM(`driver_net`), 0) INTO b FROM `taxi_rides`
    WHERE `driver_char` = p_char AND `kind` = 1 AND `state` = 2 AND `paid_at` > DATE_SUB(NOW(), INTERVAL 1 HOUR);
  RETURN LEAST(a + b, 2147483647);
END//

-- Reward di un contratto FREIGHT (design §1): (BASE + PER_M_X100/100 x m) x (1 + rep) x mult_attivita'(destinazione) x (1 + pericolo) x (1 + premium + ricostruzione)
-- x integrita'. Aritmetica intera, per difetto. Integrita' < FRT_INTEG_FAIL = 0 (fallito).
DROP FUNCTION IF EXISTS ct_reward//
CREATE FUNCTION ct_reward(p_dist INT, p_rep_points INT, p_act_to INT, p_crime_from INT, p_crime_to INT, p_premium INT, p_integrity INT) RETURNS INT
READS SQL DATA
BEGIN
  DECLARE v_base BIGINT;
  DECLARE v_rep INT;
  DECLARE v_act INT;
  DECLARE v_haz INT DEFAULT 0;
  DECLARE v_extra INT DEFAULT 0;
  DECLARE v_int INT;
  SET v_base = ld_cfg('FRT_BASE') + (CAST(GREATEST(p_dist, 0) AS SIGNED) * ld_cfg('FRT_PER_M_X100')) DIV 100;
  SET v_rep = ld_tier_pct(p_rep_points);
  SET v_act = ld_cfg('LD_ACT_MULT_MIN_PCT')
    + ((ld_cfg('LD_ACT_MULT_MAX_PCT') - ld_cfg('LD_ACT_MULT_MIN_PCT')) * LEAST(GREATEST(p_act_to, 0), ld_cfg('LD_ACT_MAX'))) DIV ld_cfg('LD_ACT_MAX');
  IF GREATEST(p_crime_from, p_crime_to) >= ld_cfg('LD_CRIME_HAZARD_AT') THEN SET v_haz = ld_cfg('LD_CRIME_HAZARD_PCT'); END IF;
  IF p_premium <> 0 THEN SET v_extra = ld_cfg('LD_PREMIUM_PCT'); END IF;
  IF p_act_to < ld_cfg('LD_PROSP_LOW') THEN SET v_extra = v_extra + ld_cfg('LD_PROSP_LOW_BONUS_PCT'); END IF;
  IF p_integrity >= ld_cfg('FRT_INTEG_OK') THEN SET v_int = 100;
  ELSEIF p_integrity >= ld_cfg('FRT_INTEG_FAIL') THEN SET v_int = 100 - ld_cfg('FRT_INTEG_PENALTY_PCT');
  ELSE SET v_int = 0;
  END IF;
  RETURN (v_base * (100 + v_rep) * v_act * (100 + v_haz) * (100 + v_extra) * v_int) DIV 10000000000;
END//

-- Contratti per bucket in base alla prosperita': PER_BUCKET + DELTA per quartiere Fiorente - DELTA per quartiere In difficolta', minimo PROSP_BOARD_MIN.
DROP FUNCTION IF EXISTS ct_bucket_slots//
CREATE FUNCTION ct_bucket_slots() RETURNS INT
READS SQL DATA
BEGIN
  DECLARE v_hi INT DEFAULT 0;
  DECLARE v_lo INT DEFAULT 0;
  SELECT COALESCE(SUM(`activity` >= ld_cfg('LD_PROSP_HIGH')), 0), COALESCE(SUM(`activity` < ld_cfg('LD_PROSP_LOW')), 0) INTO v_hi, v_lo FROM `ld_districts`;
  RETURN GREATEST(ld_cfg('LD_PROSP_BOARD_MIN'), ld_cfg('CT_PER_BUCKET') + ld_cfg('LD_PROSP_BOARD_DELTA') * (v_hi - v_lo));
END//

-- --- procedure interne (il chiamante ha gia' aperto la transazione e bloccato le righe indicate) --------------------------

DROP PROCEDURE IF EXISTS ld_oplog//
CREATE PROCEDURE ld_oplog(IN p_op VARCHAR(40), IN p_kind VARCHAR(12), IN p_char INT UNSIGNED, IN p_other INT UNSIGNED, IN p_ref BIGINT UNSIGNED, IN p_res INT,
  IN p_amount INT, IN p_bal INT, IN p_oamount INT, IN p_obal INT, IN p_v1 INT, IN p_v2 INT, IN p_v3 INT)
BEGIN
  INSERT INTO `ld_ops` (`op_id`, `kind`, `character_id`, `other_character_id`, `ref`, `result`, `amount`, `balance_after`, `other_amount`, `other_balance_after`, `v1`, `v2`, `v3`)
  VALUES (p_op, p_kind, COALESCE(p_char, 0), COALESCE(p_other, 0), COALESCE(p_ref, 0), p_res, COALESCE(p_amount, 0), COALESCE(p_bal, 0),
    COALESCE(p_oamount, 0), COALESCE(p_obal, 0), COALESCE(p_v1, 0), COALESCE(p_v2, 0), COALESCE(p_v3, 0));
END//

-- Movimento di banca con riga di ledger (personaggio gia' bloccato). Accredito limitato a MONEY_MAX (p_applied = quanto applicato). delta 0 = nessuna riga.
DROP PROCEDURE IF EXISTS ld_bank_move//
CREATE PROCEDURE ld_bank_move(IN p_op VARCHAR(40), IN p_char INT UNSIGNED, IN p_kind VARCHAR(12), IN p_delta INT, IN p_ref VARCHAR(32), OUT p_applied INT, OUT p_bal INT)
BEGIN
  DECLARE v_bank BIGINT;
  SELECT `bank` INTO v_bank FROM `characters` WHERE `id` = p_char;
  SET p_applied = p_delta;
  IF p_delta > 0 THEN SET p_applied = LEAST(p_delta, GREATEST(ld_cfg('MONEY_MAX') - v_bank, 0)); END IF;
  SET p_bal = v_bank + p_applied;
  IF p_applied <> 0 THEN
    UPDATE `characters` SET `bank` = p_bal WHERE `id` = p_char;
    CALL bank_entry(p_op, p_char, NULL, p_kind, p_applied, p_bal, 2, 0, p_ref);
  END IF;
END//

-- Reputazione professionale: i guadagni rispettano PRO_REP_DAY_CAP per track e giorno; le perdite no (minimo 0).
DROP PROCEDURE IF EXISTS ld_rep_add//
CREATE PROCEDURE ld_rep_add(IN p_char INT UNSIGNED, IN p_track INT, IN p_delta INT, IN p_done INT, IN p_fail INT, OUT p_points INT)
BEGIN
  DECLARE v_day INT DEFAULT TO_DAYS(NOW());
  DECLARE v_pts INT;
  DECLARE v_gain INT;
  DECLARE v_dk INT;
  DECLARE v_add INT;
  INSERT IGNORE INTO `pro_rep` (`character_id`, `track`, `day_key`) VALUES (p_char, p_track, v_day);
  SELECT `points`, `day_gain`, `day_key` INTO v_pts, v_gain, v_dk FROM `pro_rep` WHERE `character_id` = p_char AND `track` = p_track FOR UPDATE;
  IF v_dk <> v_day THEN SET v_gain = 0; END IF;
  SET v_add = p_delta;
  IF p_delta > 0 THEN SET v_add = LEAST(p_delta, GREATEST(ld_cfg('PRO_REP_DAY_CAP') - v_gain, 0)); END IF;
  SET v_pts = GREATEST(v_pts + v_add, 0);
  UPDATE `pro_rep` SET `points` = v_pts, `day_key` = v_day, `day_gain` = v_gain + GREATEST(v_add, 0),
    `completed` = `completed` + p_done, `failed` = `failed` + p_fail
    WHERE `character_id` = p_char AND `track` = p_track;
  SET p_points = v_pts;
END//

-- Reputazione locale: p_delta > 0 rispetta LD_LOCALREP_DAY_CAP (se p_capped) e vale doppio (RECOVER/PER_EVENT) se la reputazione e' negativa ("seconda occasione").
DROP PROCEDURE IF EXISTS ld_local_add//
CREATE PROCEDURE ld_local_add(IN p_char INT UNSIGNED, IN p_district INT, IN p_delta INT, IN p_capped INT, OUT p_rep INT)
BEGIN
  DECLARE v_day INT DEFAULT TO_DAYS(NOW());
  DECLARE v_rep INT;
  DECLARE v_gain INT;
  DECLARE v_dk INT;
  DECLARE v_add INT;
  INSERT IGNORE INTO `ld_district_rep` (`character_id`, `district_id`, `day_key`) VALUES (p_char, p_district, v_day);
  SELECT `rep`, `day_gain`, `day_key` INTO v_rep, v_gain, v_dk FROM `ld_district_rep` WHERE `character_id` = p_char AND `district_id` = p_district FOR UPDATE;
  IF v_dk <> v_day THEN SET v_gain = 0; END IF;
  SET v_add = p_delta;
  IF p_delta > 0 THEN
    IF v_rep < 0 THEN SET v_add = (p_delta * ld_cfg('LD_REP_RECOVER_PER_EVENT')) DIV ld_cfg('LD_LOCALREP_PER_EVENT'); END IF;
    IF p_capped <> 0 THEN SET v_add = LEAST(v_add, GREATEST(ld_cfg('LD_LOCALREP_DAY_CAP') - v_gain, 0)); END IF;
  END IF;
  SET v_rep = LEAST(GREATEST(v_rep + v_add, ld_cfg('LD_LOCALREP_MIN')), ld_cfg('LD_LOCALREP_MAX'));
  UPDATE `ld_district_rep` SET `rep` = v_rep, `day_key` = v_day, `day_gain` = v_gain + IF(p_capped <> 0, GREATEST(v_add, 0), 0)
    WHERE `character_id` = p_char AND `district_id` = p_district;
  SET p_rep = v_rep;
END//

-- Attivita' del quartiere (riga gia' bloccata). Guadagni da giocatore (p_player = 1) limitati a LD_ACT_HOURLY_GAIN_CAP/h (somma ld_events). Clamp 0..LD_ACT_MAX.
DROP PROCEDURE IF EXISTS ld_act_add//
CREATE PROCEDURE ld_act_add(IN p_district INT, IN p_delta INT, IN p_player INT, OUT p_applied INT, OUT p_act INT)
BEGIN
  DECLARE v_act INT;
  DECLARE v_used INT DEFAULT 0;
  DECLARE v_add INT;
  SELECT `activity` INTO v_act FROM `ld_districts` WHERE `id` = p_district;
  SET v_add = p_delta;
  IF p_delta > 0 AND p_player <> 0 THEN
    SELECT COALESCE(SUM(`act_delta`), 0) INTO v_used FROM `ld_events`
      WHERE `district_id` = p_district AND `actor_char` IS NOT NULL AND `act_delta` > 0 AND `created_at` > DATE_SUB(NOW(), INTERVAL 1 HOUR);
    SET v_add = LEAST(p_delta, GREATEST(ld_cfg('LD_ACT_HOURLY_GAIN_CAP') - v_used, 0));
  END IF;
  SET p_act = LEAST(GREATEST(v_act + v_add, 0), ld_cfg('LD_ACT_MAX'));
  SET p_applied = p_act - v_act;
  IF p_applied <> 0 THEN UPDATE `ld_districts` SET `activity` = p_act WHERE `id` = p_district; END IF;
END//

DROP PROCEDURE IF EXISTS ld_event//
CREATE PROCEDURE ld_event(IN p_op VARCHAR(40), IN p_district INT, IN p_type INT, IN p_actor INT UNSIGNED, IN p_value INT, IN p_act INT, IN p_counted INT, IN p_ref BIGINT UNSIGNED)
BEGIN
  INSERT INTO `ld_events` (`op_id`, `district_id`, `type`, `actor_char`, `value`, `act_delta`, `counted`, `ref`)
  VALUES (p_op, p_district, p_type, NULLIF(p_actor, 0), p_value, p_act, p_counted, p_ref);
END//

-- Nuovo progetto attivo del quartiere (voce successiva del catalogo), con un fondo iniziale (riporto). Nessun effetto se ce n'e' gia' uno attivo.
DROP PROCEDURE IF EXISTS ld_project_new//
CREATE PROCEDURE ld_project_new(IN p_district INT, IN p_fund DECIMAL(12,1))
BEGIN
  DECLARE v_last INT;
  DECLARE v_max INT;
  DECLARE v_seq INT;
  DECLARE v_name VARCHAR(64);
  SELECT MAX(`seq`) INTO v_last FROM `ld_projects` WHERE `district_id` = p_district;
  SELECT MAX(`seq`) INTO v_max FROM `ld_project_catalog` WHERE `district_id` = p_district;
  SET v_seq = IF(v_last IS NULL OR v_max IS NULL OR v_last >= v_max, 1, v_last + 1);
  SELECT MAX(`name`) INTO v_name FROM `ld_project_catalog` WHERE `district_id` = p_district AND `seq` = v_seq;
  INSERT IGNORE INTO `ld_projects` (`district_id`, `seq`, `name`, `goal`, `fund`, `ends_at`)
  VALUES (p_district, v_seq, COALESCE(v_name, 'Progetto di quartiere'), ld_cfg('PRJ_GOAL'), GREATEST(COALESCE(p_fund, 0), 0),
    DATE_ADD(NOW(), INTERVAL ld_cfg('PRJ_DAYS') DAY));
END//

-- Completamento (quartiere e progetto gia' bloccati): evento 8 COMUNITA', attivita' +PRJ_ACT_BONUS, festa PRJ_FEST_H, +PRJ_REP_DONE ai contributori con
-- almeno PRJ_REP_MIN_POINTS punti, progetto successivo con il fondo eccedente.
DROP PROCEDURE IF EXISTS ld_project_finish//
CREATE PROCEDURE ld_project_finish(IN p_project INT UNSIGNED)
BEGIN
  DECLARE v_d INT;
  DECLARE v_fund DECIMAL(12,1);
  DECLARE v_goal INT;
  DECLARE v_app INT;
  DECLARE v_act INT;
  DECLARE v_c INT UNSIGNED;
  DECLARE v_r INT;
  DECLARE v_done INT DEFAULT 0;
  DECLARE cur CURSOR FOR SELECT `character_id` FROM `ld_project_contrib` WHERE `project_id` = p_project AND `points` >= ld_cfg('PRJ_REP_MIN_POINTS') ORDER BY `character_id`;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET v_done = 1;
  SELECT `district_id`, `fund`, `goal` INTO v_d, v_fund, v_goal FROM `ld_projects` WHERE `id` = p_project;
  UPDATE `ld_projects` SET `state` = 2, `done_at` = NOW() WHERE `id` = p_project;
  CALL ld_act_add(v_d, ld_cfg('PRJ_ACT_BONUS'), 0, v_app, v_act);
  UPDATE `ld_districts` SET `fest_until` = DATE_ADD(NOW(), INTERVAL ld_cfg('PRJ_FEST_H') HOUR) WHERE `id` = v_d;
  CALL ld_event(CONCAT('pj', p_project, '-done'), v_d, 8, 0, v_goal, v_app, 1, p_project);
  OPEN cur;
  rl: LOOP
    FETCH cur INTO v_c;
    IF v_done = 1 THEN LEAVE rl; END IF;
    CALL ld_local_add(v_c, v_d, ld_cfg('PRJ_REP_DONE'), 0, v_r);
  END LOOP;
  CLOSE cur;
  CALL ld_project_new(v_d, v_fund - v_goal);
END//

-- Fondo del progetto attivo del quartiere (riga del quartiere gia' bloccata): +p_amount e, se p_char, +p_points punti comunita' (tetto PRJ_DAY_CAP/giorno).
-- p_donated = $ donati (gia' addebitati). OUT p_project/p_pts (punti effettivi)/p_state.
DROP PROCEDURE IF EXISTS ld_project_fund//
CREATE PROCEDURE ld_project_fund(IN p_district INT, IN p_amount DECIMAL(12,1), IN p_char INT UNSIGNED, IN p_points INT, IN p_donated INT,
  OUT p_project INT UNSIGNED, OUT p_pts INT, OUT p_state INT)
BEGIN
  DECLARE v_day INT DEFAULT TO_DAYS(NOW());
  DECLARE v_fund DECIMAL(12,1);
  DECLARE v_goal INT;
  DECLARE v_today INT DEFAULT 0;
  SET p_pts = 0;
  SET p_state = 0;
  SELECT MAX(`id`) INTO p_project FROM `ld_projects` WHERE `district_id` = p_district AND `state` = 1 AND `ends_at` > NOW();
  IF p_project IS NOT NULL THEN
    SELECT `fund`, `goal` INTO v_fund, v_goal FROM `ld_projects` WHERE `id` = p_project FOR UPDATE;
    SET v_fund = v_fund + GREATEST(p_amount, 0);
    UPDATE `ld_projects` SET `fund` = v_fund WHERE `id` = p_project;
    IF p_char IS NOT NULL AND p_char > 0 THEN
      SELECT COALESCE(SUM(`day_points`), 0) INTO v_today FROM `ld_project_contrib` WHERE `character_id` = p_char AND `day_key` = v_day;
      SET p_pts = LEAST(GREATEST(p_points, 0), GREATEST(ld_cfg('PRJ_DAY_CAP') - v_today, 0));
      INSERT INTO `ld_project_contrib` (`project_id`, `character_id`, `points`, `donated`, `day_key`, `day_points`, `day_donated`)
      VALUES (p_project, p_char, p_pts, p_donated, v_day, p_pts, p_donated)
      ON DUPLICATE KEY UPDATE `day_points` = IF(`day_key` = v_day, `day_points`, 0) + VALUES(`day_points`),
        `day_donated` = IF(`day_key` = v_day, `day_donated`, 0) + VALUES(`day_donated`),
        `points` = `points` + VALUES(`points`), `donated` = `donated` + VALUES(`donated`), `day_key` = v_day;
    END IF;
    SET p_state = 1;
    IF v_fund >= v_goal THEN
      CALL ld_project_finish(p_project);
      SET p_state = 2;
    END IF;
  END IF;
END//

-- Chiusura di un noleggio APERTO (personaggio e noleggio gia' bloccati). p_state: 2 riconsegnato (rimborso da salute), 3 interrotto (cauzione 100%),
-- 4 scaduto (cauzione persa), 5 annullato (rimborso totale: canone + cauzione, anche la parte pagata in contanti, in banca). Rimborso sempre in banca.
DROP PROCEDURE IF EXISTS ld_rental_finish//
CREATE PROCEDURE ld_rental_finish(IN p_rental BIGINT UNSIGNED, IN p_state INT, IN p_health INT, IN p_op VARCHAR(40), OUT p_refund INT, OUT p_bal INT)
BEGIN
  DECLARE v_char INT UNSIGNED;
  DECLARE v_dep INT;
  DECLARE v_fee INT;
  DECLARE v_h INT;
  DECLARE v_z INT;
  DECLARE v_f INT;
  DECLARE v_app INT;
  SELECT `character_id`, `deposit`, `fee` INTO v_char, v_dep, v_fee FROM `rentals` WHERE `id` = p_rental;
  SET v_z = ld_cfg('RENT_HEALTH_ZERO');
  SET v_f = ld_cfg('RENT_HEALTH_FULL');
  CASE p_state
    WHEN 2 THEN
      SET v_h = LEAST(GREATEST(COALESCE(p_health, 0), v_z), v_f);
      SET p_refund = LEAST(GREATEST((v_dep * (v_h - v_z) + (v_f - v_z) DIV 2) DIV (v_f - v_z), 0), v_dep);
    WHEN 3 THEN SET p_refund = v_dep;
    WHEN 5 THEN SET p_refund = v_dep + v_fee;
    ELSE SET p_refund = 0;
  END CASE;
  CALL ld_bank_move(p_op, v_char, 'rental', p_refund, CONCAT('rt', p_rental), v_app, p_bal);
  SET p_refund = v_app;
  UPDATE `rentals` SET `state` = p_state, `closed_at` = NOW(), `refund` = p_refund, `close_op` = p_op WHERE `id` = p_rental;
  CALL ld_event(CONCAT(p_op, '-e'), 0, 5, v_char, p_refund, 0, 0, p_rental);
END//

-- Rilascia (senza penalita') la partecipazione attiva del personaggio p_char al contratto p_contract (righe NON bloccate: le blocca qui, nell'ordine).
DROP PROCEDURE IF EXISTS ld_ct_release_row//
CREATE PROCEDURE ld_ct_release_row(IN p_contract BIGINT UNSIGNED, IN p_char INT UNSIGNED, OUT p_state INT)
BEGIN
  DECLARE v_exp TIMESTAMP;
  DECLARE v_part INT;
  SELECT `expires_at`, `participants` INTO v_exp, v_part FROM `ct_contracts` WHERE `id` = p_contract FOR UPDATE;
  UPDATE `ct_participants` SET `state` = 2, `released_at` = NOW() WHERE `contract_id` = p_contract AND `character_id` = p_char AND `state` = 1;
  IF ROW_COUNT() > 0 THEN SET v_part = GREATEST(v_part - 1, 0); END IF;
  SET p_state = IF(v_exp <= NOW(), 5, IF(v_part = 0, 7, 1));
  UPDATE `ct_contracts` SET `participants` = v_part, `state` = p_state,
    `accepted_at` = IF(v_part = 0, NULL, `accepted_at`), `picked_at` = IF(v_part = 0, NULL, `picked_at`)
    WHERE `id` = p_contract;
END//

-- --- inizializzazione, tick, manutenzione ------------------------------------------------------------------------------

-- Seed idempotente: un progetto attivo per ogni quartiere che non ne ha (i quartieri e il catalogo sono in living_seed.sql).
DROP PROCEDURE IF EXISTS sp_ld_seed//
CREATE PROCEDURE sp_ld_seed()
BEGIN
  DECLARE v_d INT;
  DECLARE v_done INT DEFAULT 0;
  DECLARE cur CURSOR FOR SELECT `id` FROM `ld_districts` d WHERE NOT EXISTS (SELECT 1 FROM `ld_projects` p WHERE p.`district_id` = d.`id` AND p.`state` = 1) ORDER BY `id`;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET v_done = 1;
  OPEN cur;
  sl: LOOP
    FETCH cur INTO v_d;
    IF v_done = 1 THEN LEAVE sl; END IF;
    CALL ld_project_new(v_d, 0);
  END LOOP;
  CLOSE cur;
END//

-- Tick del quartiere con catch-up: applica i tick interi trascorsi da last_tick_at (max LD_TICK_CATCHUP_MAX; quelli oltre si saltano) e sposta last_tick_at
-- di n x periodo. Idempotente: ripetuto nello stesso periodo non applica nulla. Ogni tick: attivita' e pressione verso la base di max(MIN_STEP, distanza/DIV),
-- sicurezza +1 se pressione < LD_SEC_TICK_CRIME_AT altrimenti -1; durante la festa attivita' >= PRJ_FEST_ACT_MIN. Chiude anche il progetto scaduto (fondo riportato).
DROP PROCEDURE IF EXISTS sp_ld_tick//
CREATE PROCEDURE sp_ld_tick(IN p_district INT)
main: BEGIN
  DECLARE v_act INT;
  DECLARE v_sec INT;
  DECLARE v_cr INT;
  DECLARE v_ba INT;
  DECLARE v_bc INT;
  DECLARE v_last TIMESTAMP;
  DECLARE v_fest TIMESTAMP;
  DECLARE v_per INT;
  DECLARE v_n INT;
  DECLARE v_apply INT;
  DECLARE v_i INT DEFAULT 0;
  DECLARE v_d INT;
  DECLARE v_t DATETIME;
  DECLARE v_div INT;
  DECLARE v_min INT;
  DECLARE v_pid INT UNSIGNED;
  DECLARE v_pfund DECIMAL(12,1);
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  SET v_per = GREATEST(ld_cfg('LD_TICK_MS') DIV 1000, 1);
  SET v_div = GREATEST(ld_cfg('LD_TICK_DIV'), 1);
  SET v_min = ld_cfg('LD_TICK_MIN_STEP');
  START TRANSACTION;
  SELECT `activity`, `security`, `crime`, `base_activity`, `base_crime`, `last_tick_at`, `fest_until` INTO v_act, v_sec, v_cr, v_ba, v_bc, v_last, v_fest
    FROM `ld_districts` WHERE `id` = p_district FOR UPDATE;
  IF v_act IS NULL THEN COMMIT; LEAVE main; END IF;
  SET v_n = GREATEST(TIMESTAMPDIFF(SECOND, v_last, NOW()), 0) DIV v_per;
  IF v_n > 0 THEN
    SET v_apply = LEAST(v_n, ld_cfg('LD_TICK_CATCHUP_MAX'));
    WHILE v_i < v_apply DO
      SET v_t = DATE_ADD(v_last, INTERVAL (v_n - v_apply + v_i + 1) * v_per SECOND);
      SET v_d = v_ba - v_act;
      IF v_d <> 0 THEN SET v_act = v_act + SIGN(v_d) * LEAST(GREATEST(v_min, ABS(v_d) DIV v_div), ABS(v_d)); END IF;
      SET v_d = v_bc - v_cr;
      IF v_d <> 0 THEN SET v_cr = v_cr + SIGN(v_d) * LEAST(GREATEST(v_min, ABS(v_d) DIV v_div), ABS(v_d)); END IF;
      SET v_sec = LEAST(GREATEST(v_sec + IF(v_cr < ld_cfg('LD_SEC_TICK_CRIME_AT'), 1, -1), 0), ld_cfg('LD_SEC_MAX'));
      IF v_fest IS NOT NULL AND v_t <= v_fest AND v_act < ld_cfg('PRJ_FEST_ACT_MIN') THEN SET v_act = ld_cfg('PRJ_FEST_ACT_MIN'); END IF;
      SET v_act = LEAST(GREATEST(v_act, 0), ld_cfg('LD_ACT_MAX'));
      SET v_i = v_i + 1;
    END WHILE;
    UPDATE `ld_districts` SET `activity` = v_act, `security` = v_sec, `crime` = v_cr,
      `last_tick_at` = DATE_ADD(v_last, INTERVAL v_n * v_per SECOND), `ticks` = `ticks` + v_apply WHERE `id` = p_district;
  END IF;
  -- progetto scaduto senza obiettivo: nessuna penalita', fondo al progetto successivo
  SELECT MAX(`id`) INTO v_pid FROM `ld_projects` WHERE `district_id` = p_district AND `state` = 1 AND `ends_at` <= NOW();
  IF v_pid IS NOT NULL THEN
    SELECT `fund` INTO v_pfund FROM `ld_projects` WHERE `id` = v_pid FOR UPDATE;
    UPDATE `ld_projects` SET `state` = 3, `done_at` = NOW() WHERE `id` = v_pid;
    CALL ld_project_new(p_district, v_pfund);
  END IF;
  IF NOT EXISTS (SELECT 1 FROM `ld_projects` WHERE `district_id` = p_district AND `state` = 1) THEN CALL ld_project_new(p_district, 0); END IF;
  COMMIT;
END//

DROP PROCEDURE IF EXISTS sp_ld_tick_all//
CREATE PROCEDURE sp_ld_tick_all()
BEGIN
  DECLARE v_d INT;
  DECLARE v_done INT DEFAULT 0;
  DECLARE cur CURSOR FOR SELECT `id` FROM `ld_districts` ORDER BY `id`;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET v_done = 1;
  OPEN cur;
  tl: LOOP
    FETCH cur INTO v_d;
    IF v_done = 1 THEN LEAVE tl; END IF;
    CALL sp_ld_tick(v_d);
  END LOOP;
  CLOSE cur;
END//

-- Scadenze: contratti aperti/rilasciati scaduti -> SCADUTO; contratti attivi scaduti -> SCADUTO con partecipanti rilasciati (nessun pagamento).
DROP PROCEDURE IF EXISTS sp_ct_sweep//
CREATE PROCEDURE sp_ct_sweep()
BEGIN
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; RESIGNAL; END;
  START TRANSACTION;
  UPDATE `ct_contracts` c JOIN `ct_participants` p ON p.`contract_id` = c.`id`
    SET p.`state` = 2, p.`released_at` = NOW() WHERE c.`state` IN (1, 2) AND c.`expires_at` <= NOW() AND p.`state` = 1;
  UPDATE `ct_contracts` SET `state` = 5, `participants` = 0 WHERE `state` IN (0, 1, 2, 7) AND `expires_at` <= NOW();
  COMMIT;
END//

-- Pulizia di un personaggio (al login, e come compensazione di una sessione scaduta): contratti attivi RILASCIATI senza penalita', noleggio aperto INTERROTTO
-- (cauzione 100% in banca), corse aperte ANNULLATE. Idempotente (agisce solo sugli stati aperti). p_op (opzionale, NULL o '' = nessun esito): con op_id l'esito va
-- in ld_ops (kind ld_cleanup, result 1, amount = rimborso accreditato, balance_after, v1 contratti rilasciati, v2 noleggi chiusi, v3 corse annullate).
DROP PROCEDURE IF EXISTS sp_ld_char_cleanup//
CREATE PROCEDURE sp_ld_char_cleanup(IN p_char INT UNSIGNED, IN p_op VARCHAR(40))
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_ct BIGINT UNSIGNED;
  DECLARE v_rt BIGINT UNSIGNED;
  DECLARE v_st INT;
  DECLARE v_ref INT DEFAULT 0;
  DECLARE v_bal INT;
  DECLARE v_n1 INT DEFAULT 0;
  DECLARE v_n2 INT DEFAULT 0;
  DECLARE v_n3 INT DEFAULT 0;
  DECLARE v_op INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF v_op = 0 OR NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NOT NULL AND CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  IF p_op IS NOT NULL AND CHAR_LENGTH(p_op) > 0 THEN SET v_op = 1; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  IF v_op = 1 THEN
    SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
    IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  END IF;
  SET v_bal = v_bank;
  IF v_bank IS NOT NULL THEN
    SELECT MAX(`contract_id`) INTO v_ct FROM `ct_participants` WHERE `active_char` = p_char;
    IF v_ct IS NOT NULL THEN CALL ld_ct_release_row(v_ct, p_char, v_st); SET v_n1 = 1; END IF;
    SELECT MAX(`id`) INTO v_rt FROM `rentals` WHERE `open_char` = p_char;
    IF v_rt IS NOT NULL THEN
      SELECT `id` INTO v_rt FROM `rentals` WHERE `id` = v_rt FOR UPDATE;
      CALL ld_rental_finish(v_rt, 3, 0, CONCAT('rt', v_rt, '-int'), v_ref, v_bal);
      SET v_n2 = 1;
    END IF;
    UPDATE `taxi_rides` SET `state` = 4 WHERE `open_driver` = p_char;
    SET v_n3 = ROW_COUNT();
    UPDATE `taxi_rides` SET `state` = 4 WHERE `passenger_char` = p_char AND `state` = 1;
    SET v_n3 = v_n3 + ROW_COUNT();
  END IF;
  IF v_op = 1 THEN CALL ld_oplog(p_op, 'ld_cleanup', p_char, 0, 0, IF(v_bank IS NULL, 9, 1), v_ref, v_bal, 0, 0, v_n1, v_n2, v_n3); END IF;
  COMMIT;
END//

-- Compensazione di un'apertura di noleggio con esito incerto (errore DB) o sessione scaduta. Se il noleggio aperto da p_op esiste ed e' ancora aperto: ANNULLATO
-- (stato 5, canone + cauzione rimborsati in banca, anche la parte in contanti) e i contanti riservati ma non usati (p_cash - cash_paid) tornano in banca.
-- Se non esiste (CALL non eseguita o rifiutata) e p_cash > 0: tutti i contanti riservati dal Pawn tornano in BANCA. Idempotente. Esito in ld_ops con op_id '<p_op>-x' (kind rent_undo): amount = accreditato, balance_after, v1 = 1 annullato / 2 contanti.
DROP PROCEDURE IF EXISTS sp_rental_undo//
CREATE PROCEDURE sp_rental_undo(IN p_op VARCHAR(40), IN p_char INT UNSIGNED, IN p_cash INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_op VARCHAR(40);
  DECLARE v_rt BIGINT UNSIGNED;
  DECLARE v_st INT;
  DECLARE v_ref INT DEFAULT 0;
  DECLARE v_bal INT;
  DECLARE v_how INT DEFAULT 0;
  DECLARE v_app INT;
  DECLARE v_cpaid INT DEFAULT 0;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = v_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  SET v_op = CONCAT(p_op, '-x');
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = v_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  SELECT MAX(`id`), MAX(`state`) INTO v_rt, v_st FROM `rentals` WHERE `open_op` = p_op AND `character_id` = p_char;
  IF v_bank IS NOT NULL AND v_rt IS NOT NULL THEN
    SELECT `state`, `cash_paid` INTO v_st, v_cpaid FROM `rentals` WHERE `id` = v_rt FOR UPDATE;
    IF v_st = 1 THEN
      CALL ld_rental_finish(v_rt, 5, 0, v_op, v_ref, v_bal);
      SET v_how = 1;
    END IF;
    IF COALESCE(p_cash, 0) > v_cpaid THEN
      CALL ld_bank_move(CONCAT(v_op, '-c'), p_char, 'rental', p_cash - v_cpaid, 'cashback', v_app, v_bal);
      SET v_ref = v_ref + v_app;
    END IF;
  ELSEIF v_bank IS NOT NULL AND COALESCE(p_cash, 0) > 0 THEN
    CALL ld_bank_move(v_op, p_char, 'rental', p_cash, 'cashback', v_app, v_bal);
    SET v_ref = v_app;
    SET v_how = 2;
  END IF;
  CALL ld_oplog(v_op, 'rent_undo', p_char, 0, COALESCE(v_rt, 0), IF(v_bank IS NULL, 9, 1), v_ref, v_bal, 0, 0, v_how, 0, 0);
  COMMIT;
END//

-- Avvio del server (prima di accettare giocatori): pulizia di TUTTI i personaggi con oggetti aperti, catch-up dei tick, scadenze.
DROP PROCEDURE IF EXISTS sp_ld_boot//
CREATE PROCEDURE sp_ld_boot()
BEGIN
  DECLARE v_c INT UNSIGNED;
  DECLARE v_done INT DEFAULT 0;
  DECLARE cur CURSOR FOR
    SELECT `active_char` FROM `ct_participants` WHERE `active_char` IS NOT NULL
    UNION SELECT `open_char` FROM `rentals` WHERE `open_char` IS NOT NULL
    UNION SELECT `driver_char` FROM `taxi_rides` WHERE `state` = 1
    ORDER BY 1;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET v_done = 1;
  OPEN cur;
  bl: LOOP
    FETCH cur INTO v_c;
    IF v_done = 1 THEN LEAVE bl; END IF;
    CALL sp_ld_char_cleanup(v_c, NULL);
  END LOOP;
  CLOSE cur;
  UPDATE `taxi_rides` SET `state` = 4 WHERE `state` = 1;
  CALL sp_ld_tick_all();
  CALL sp_ct_sweep();
END//

-- --- contratti ------------------------------------------------------------------------------------------------------------

-- Genera lo slot p_slot del bucket p_bucket (route scelta dal Director in Pawn, deterministica). INSERT IGNORE su slot_key "b<bucket>s<slot>": ripetuta non duplica.
-- Ignorata (nessuna riga) se: slot >= ct_bucket_slots(), quartieri inesistenti o uguali, distanza < FRT_MIN_DIST, merce fuori range, bucket gia' scaduto.
-- Reward di bacheca = ct_reward senza bonus personali con i valori correnti dei quartieri; scadenza = fine bucket + CT_EXPIRE_EXTRA_S; premium: min_local_rep = LD_PREMIUM_LOCALREP.
DROP PROCEDURE IF EXISTS sp_ct_generate//
CREATE PROCEDURE sp_ct_generate(IN p_bucket INT UNSIGNED, IN p_slot INT, IN p_kind INT, IN p_from_pt INT, IN p_to_pt INT, IN p_from_d INT, IN p_to_d INT,
  IN p_cargo INT, IN p_dist INT, IN p_min_level INT, IN p_min_rep INT, IN p_min_local INT, IN p_premium INT)
main: BEGIN
  DECLARE v_af INT;
  DECLARE v_at INT;
  DECLARE v_cf INT;
  DECLARE v_ctt INT;
  DECLARE v_exp BIGINT;
  DECLARE v_prem INT;
  IF p_bucket IS NULL OR p_slot IS NULL OR p_slot < 0 OR p_slot >= ct_bucket_slots() OR p_kind <> 1 OR p_from_d = p_to_d
     OR p_dist < ld_cfg('FRT_MIN_DIST') OR p_cargo NOT BETWEEN 1 AND ld_cfg('FRT_CARGO_TYPES') OR p_from_pt NOT BETWEEN 0 AND 255 OR p_to_pt NOT BETWEEN 0 AND 255
     OR p_min_level NOT BETWEEN 1 AND 255 OR p_min_rep < 0 THEN
    LEAVE main;
  END IF;
  SELECT MAX(`activity`), MAX(`crime`) INTO v_af, v_cf FROM `ld_districts` WHERE `id` = p_from_d;
  SELECT MAX(`activity`), MAX(`crime`) INTO v_at, v_ctt FROM `ld_districts` WHERE `id` = p_to_d;
  IF v_af IS NULL OR v_at IS NULL THEN LEAVE main; END IF;
  SET v_exp = (CAST(p_bucket AS SIGNED) + 1) * ld_cfg('CT_BUCKET_S') + ld_cfg('CT_EXPIRE_EXTRA_S');
  IF v_exp <= UNIX_TIMESTAMP() THEN LEAVE main; END IF;
  SET v_prem = IF(p_premium <> 0, 1, 0);
  INSERT IGNORE INTO `ct_contracts` (`slot_key`, `bucket`, `kind`, `state`, `from_pt`, `to_pt`, `from_district`, `to_district`, `cargo`, `distance_m`, `limit_s`,
    `min_level`, `min_rep`, `min_local_rep`, `premium`, `max_participants`, `reward`, `expires_at`)
  VALUES (CONCAT('b', p_bucket, 's', p_slot), p_bucket, p_kind, 0, p_from_pt, p_to_pt, p_from_d, p_to_d, p_cargo, p_dist,
    (p_dist * 10) DIV GREATEST(ld_cfg('FRT_TIME_MPS_X10'), 1) + ld_cfg('FRT_TIME_BASE_S'),
    p_min_level, p_min_rep, IF(v_prem = 1, GREATEST(p_min_local, ld_cfg('LD_PREMIUM_LOCALREP')), p_min_local), v_prem, 1,
    ct_reward(p_dist, 0, v_at, v_cf, v_ctt, v_prem, 100), FROM_UNIXTIME(v_exp));
END//

-- Accettazione. Controlli in ordine: argomenti (9), scaduto (5), stato/posti (4), gia' completato da lui (2), requisiti (7: un contratto attivo, noleggio aperto,
-- livello, rep trasporti, rep locale/premium, blocco riaccettazione, cooldown accettazione/completamento), tetto orario (6: ricompense/h o $/h esauriti).
-- ld_ops: amount = reward di bacheca, v1 = stato del contratto dopo, v2 = expires_at (UNIX), v3 = limit_s.
DROP PROCEDURE IF EXISTS sp_ct_accept//
CREATE PROCEDURE sp_ct_accept(IN p_contract BIGINT UNSIGNED, IN p_char INT UNSIGNED, IN p_op VARCHAR(40), IN p_level INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_state INT;
  DECLARE v_exp TIMESTAMP;
  DECLARE v_part INT;
  DECLARE v_max INT;
  DECLARE v_lvl INT;
  DECLARE v_mrep INT;
  DECLARE v_mloc INT;
  DECLARE v_prem INT;
  DECLARE v_fd INT;
  DECLARE v_td INT;
  DECLARE v_reward INT DEFAULT 0;
  DECLARE v_lim INT DEFAULT 0;
  DECLARE v_pst INT;
  DECLARE v_prel TIMESTAMP;
  DECLARE v_rep INT;
  DECLARE v_loc INT;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SELECT `state`, `expires_at`, `participants`, `max_participants`, `min_level`, `min_rep`, `min_local_rep`, `premium`, `from_district`, `to_district`, `reward`, `limit_s`
    INTO v_state, v_exp, v_part, v_max, v_lvl, v_mrep, v_mloc, v_prem, v_fd, v_td, v_reward, v_lim
    FROM `ct_contracts` WHERE `id` = p_contract FOR UPDATE;
  SELECT MAX(`state`), MAX(`released_at`) INTO v_pst, v_prel FROM `ct_participants` WHERE `contract_id` = p_contract AND `character_id` = p_char;
  SET v_rep = ld_pro_points(p_char, 1);
  SET v_loc = GREATEST(ld_local_rep(p_char, COALESCE(v_fd, 0)), ld_local_rep(p_char, COALESCE(v_td, 0)));
  IF v_bank IS NULL OR v_state IS NULL OR p_level IS NULL OR p_level < 1 THEN
    SET v_res = 9;
  ELSEIF v_pst IN (3, 4) THEN
    SET v_res = 2;
  ELSEIF v_state IN (0, 7) AND v_exp <= NOW() THEN
    UPDATE `ct_contracts` SET `state` = 5 WHERE `id` = p_contract;
    SET v_state = 5;
    SET v_res = 5;
  ELSEIF v_state NOT IN (0, 7) OR v_part >= v_max THEN
    SET v_res = IF(v_state = 5, 5, 4);
  ELSEIF EXISTS (SELECT 1 FROM `ct_participants` WHERE `active_char` = p_char) OR EXISTS (SELECT 1 FROM `rentals` WHERE `open_char` = p_char)
     OR p_level < v_lvl OR v_rep < v_mrep OR (v_prem = 1 AND v_loc < ld_cfg('LD_PREMIUM_LOCALREP') AND v_rep < ld_cfg('CT_PREMIUM_PROREP'))
     OR (v_prem = 0 AND v_loc < v_mloc)
     OR (v_pst = 2 AND v_prel > DATE_SUB(NOW(), INTERVAL ld_cfg('CT_REACCEPT_LOCK_S') SECOND))
     OR EXISTS (SELECT 1 FROM `ct_participants` WHERE `character_id` = p_char AND `joined_at` > DATE_SUB(NOW(), INTERVAL ld_cfg('CT_ACCEPT_CD_MS') DIV 1000 SECOND))
     OR EXISTS (SELECT 1 FROM `ct_payouts` WHERE `character_id` = p_char AND `paid_at` > DATE_SUB(NOW(), INTERVAL ld_cfg('CT_DONE_CD_MS') DIV 1000 SECOND)) THEN
    SET v_res = 7;
  ELSEIF (SELECT COUNT(*) FROM `ct_payouts` WHERE `character_id` = p_char AND `paid_at` > DATE_SUB(NOW(), INTERVAL 1 HOUR)) >= ld_cfg('CT_HOURLY_MAX')
     OR ld_work_earned(p_char) >= ld_cfg('LD_WORK_HOURLY_CAP') THEN
    SET v_res = 6;
  ELSE
    INSERT INTO `ct_participants` (`contract_id`, `character_id`, `state`, `attempts`, `joined_at`) VALUES (p_contract, p_char, 1, 1, NOW())
      ON DUPLICATE KEY UPDATE `state` = 1, `attempts` = LEAST(`attempts` + 1, 255), `joined_at` = NOW(), `released_at` = NULL;
    UPDATE `ct_contracts` SET `state` = 1, `participants` = `participants` + 1, `accepted_at` = NOW() WHERE `id` = p_contract;
    SET v_state = 1;
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'ct_accept', p_char, 0, p_contract, v_res, v_reward, v_bank, 0, 0, v_state, COALESCE(UNIX_TIMESTAMP(v_exp), 0), v_lim);
  COMMIT;
END//

-- Ritiro della merce (ACCETTATO -> IN_CORSO). 2 = gia' ritirato da lui, 4 = non e' il suo contratto attivo, 5 = scaduto (partecipazione rilasciata).
DROP PROCEDURE IF EXISTS sp_ct_pickup//
CREATE PROCEDURE sp_ct_pickup(IN p_contract BIGINT UNSIGNED, IN p_char INT UNSIGNED, IN p_op VARCHAR(40))
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_state INT;
  DECLARE v_exp TIMESTAMP;
  DECLARE v_pst INT;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SELECT `state`, `expires_at` INTO v_state, v_exp FROM `ct_contracts` WHERE `id` = p_contract FOR UPDATE;
  SELECT MAX(`state`) INTO v_pst FROM `ct_participants` WHERE `contract_id` = p_contract AND `character_id` = p_char FOR UPDATE;
  IF v_bank IS NULL OR v_state IS NULL THEN
    SET v_res = 9;
  ELSEIF v_pst IS NULL OR v_pst <> 1 THEN
    SET v_res = 4;
  ELSEIF v_exp <= NOW() THEN
    CALL ld_ct_release_row(p_contract, p_char, v_state);
    SET v_res = 5;
  ELSEIF v_state = 2 THEN
    SET v_res = 2;
  ELSEIF v_state <> 1 THEN
    SET v_res = 4;
  ELSE
    UPDATE `ct_contracts` SET `state` = 2, `picked_at` = NOW() WHERE `id` = p_contract;
    SET v_state = 2;
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'ct_pickup', p_char, 0, p_contract, v_res, 0, v_bank, 0, 0, v_state, 0, 0);
  COMMIT;
END//

-- Rilascio del proprio contratto attivo. p_reason 0 = interruzione (disconnessione/riavvio: nessuna penalita'), 1 = abbandono (FRT_ABANDON_REP sulla rep trasporti).
-- Il contratto torna RILASCIATO (7, riaccettabile da altri; lui dopo CT_REACCEPT_LOCK_S) o SCADUTO. ld_ops: v1 = stato del contratto, v2 = punti trasporti.
DROP PROCEDURE IF EXISTS sp_ct_release//
CREATE PROCEDURE sp_ct_release(IN p_contract BIGINT UNSIGNED, IN p_char INT UNSIGNED, IN p_op VARCHAR(40), IN p_reason INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_state INT DEFAULT 0;
  DECLARE v_pst INT;
  DECLARE v_pts INT DEFAULT 0;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SELECT MAX(`state`) INTO v_pst FROM `ct_participants` WHERE `contract_id` = p_contract AND `character_id` = p_char;
  SET v_pts = ld_pro_points(p_char, 1);
  IF v_bank IS NULL OR p_reason NOT IN (0, 1) THEN
    SET v_res = 9;
  ELSEIF v_pst IS NULL OR v_pst <> 1 THEN
    SET v_res = IF(v_pst = 2, 2, 4);
  ELSE
    CALL ld_ct_release_row(p_contract, p_char, v_state);
    IF p_reason = 1 THEN CALL ld_rep_add(p_char, 1, ld_cfg('FRT_ABANDON_REP'), 0, 0, v_pts); END IF;
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'ct_release', p_char, 0, p_contract, v_res, 0, v_bank, 0, 0, v_state, v_pts, p_reason);
  COMMIT;
END//

-- Completamento (TUTTO in una transazione). Il Pawn invia solo integrita' (% salute del veicolo) e secondi; tempo minimo e puntualita' li misura il DB da picked_at.
-- Esiti: 9 argomenti, 2 gia' pagato a lui (PK ct_payouts), 4 stato non valido / consegna troppo rapida (teletrasporto), 5 scaduto, 8 FALLITO (integrita' < FRT_INTEG_FAIL:
-- nessun pagamento, rep -PRO_REP_FAIL, attivita' destinazione LD_ACT_FAIL), 1 pagato, 6 pagato il residuo del tetto orario (anche 0).
-- Pagamento: reward ricalcolato con i valori CORRENTI (rep trasporti, attivita' destinazione, pressione), tetto LD_WORK_HOURLY_CAP su 60 min, bank += paid,
-- bank_ledger kind 'contract' (op_id = p_op, ref = 'ct<id>'), ct_payouts, pro_rep (+DELIVERY, +ONTIME), e se la consegna e' contabilizzata (anti-farming):
-- attivita' +DEST/+ORIG (tetto LD_ACT_HOURLY_GAIN_CAP/h), rep locale +PER_EVENT nei due quartieri, fondo progetti +PRJ_FUND_PCT% del reward e 1 punto; eventi 1.
-- ld_ops: amount = pagato, balance_after, v1 = punti trasporti, v2 = attivita' destinazione, v3 = reward calcolato, other_amount = puntuale (0/1),
-- other_balance_after = attivita' origine.
DROP PROCEDURE IF EXISTS sp_ct_complete//
CREATE PROCEDURE sp_ct_complete(IN p_contract BIGINT UNSIGNED, IN p_char INT UNSIGNED, IN p_op VARCHAR(40), IN p_integrity INT, IN p_seconds INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_state INT;
  DECLARE v_exp TIMESTAMP;
  DECLARE v_picked TIMESTAMP;
  DECLARE v_dist INT;
  DECLARE v_lim INT;
  DECLARE v_prem INT;
  DECLARE v_fd INT;
  DECLARE v_td INT;
  DECLARE v_fpt INT;
  DECLARE v_tpt INT;
  DECLARE v_pst INT;
  DECLARE v_paid_prev INT;
  DECLARE v_el INT DEFAULT 0;
  DECLARE v_af INT DEFAULT 0;
  DECLARE v_at INT DEFAULT 0;
  DECLARE v_cf INT;
  DECLARE v_ct INT;
  DECLARE v_pts INT DEFAULT 0;
  DECLARE v_reward INT DEFAULT 0;
  DECLARE v_paid INT DEFAULT 0;
  DECLARE v_app INT;
  DECLARE v_bal INT;
  DECLARE v_ontime INT DEFAULT 0;
  DECLARE v_counted INT DEFAULT 0;
  DECLARE v_aapp INT DEFAULT 0;
  DECLARE v_oapp INT DEFAULT 0;
  DECLARE v_r INT;
  DECLARE v_pj INT UNSIGNED;
  DECLARE v_pp INT;
  DECLARE v_ps INT;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  SELECT `state`, `expires_at`, `picked_at`, `distance_m`, `limit_s`, `premium`, `from_district`, `to_district`, `from_pt`, `to_pt`
    INTO v_state, v_exp, v_picked, v_dist, v_lim, v_prem, v_fd, v_td, v_fpt, v_tpt
    FROM `ct_contracts` WHERE `id` = p_contract FOR UPDATE;
  SELECT MAX(`state`) INTO v_pst FROM `ct_participants` WHERE `contract_id` = p_contract AND `character_id` = p_char FOR UPDATE;
  SELECT MAX(`amount`) INTO v_paid_prev FROM `ct_payouts` WHERE `contract_id` = p_contract AND `character_id` = p_char;
  IF v_picked IS NOT NULL THEN SET v_el = GREATEST(TIMESTAMPDIFF(SECOND, v_picked, NOW()), 0); END IF;
  SET v_pts = ld_pro_points(p_char, 1);
  IF v_bank IS NULL OR v_state IS NULL OR p_integrity IS NULL OR p_integrity NOT BETWEEN 0 AND 100 OR p_seconds IS NULL OR p_seconds < 0 THEN
    SET v_res = 9;
  ELSEIF v_paid_prev IS NOT NULL OR v_pst IN (3, 4) THEN
    SET v_res = 2;
  ELSEIF v_pst IS NULL OR v_pst <> 1 OR v_state <> 2 THEN
    SET v_res = 4;
  ELSEIF v_exp <= NOW() THEN
    CALL ld_ct_release_row(p_contract, p_char, v_state);
    SET v_res = 5;
  ELSEIF v_el < GREATEST(ld_cfg('FRT_MIN_SECONDS'), v_dist DIV GREATEST(ld_cfg('FRT_MIN_MPS'), 1)) THEN
    SET v_res = 4;
  ELSE
    SELECT MAX(IF(`id` = v_fd, `activity`, NULL)), MAX(IF(`id` = v_td, `activity`, NULL)), MAX(IF(`id` = v_fd, `crime`, NULL)), MAX(IF(`id` = v_td, `crime`, NULL))
      INTO v_af, v_at, v_cf, v_ct FROM (SELECT `id`, `activity`, `crime` FROM `ld_districts` WHERE `id` IN (v_fd, v_td) ORDER BY `id` FOR UPDATE) d;
    IF p_integrity < ld_cfg('FRT_INTEG_FAIL') THEN
      UPDATE `ct_contracts` SET `state` = 4, `participants` = 0, `done_at` = NOW() WHERE `id` = p_contract;
      UPDATE `ct_participants` SET `state` = 4, `released_at` = NOW() WHERE `contract_id` = p_contract AND `character_id` = p_char;
      CALL ld_rep_add(p_char, 1, ld_cfg('PRO_REP_FAIL'), 0, 1, v_pts);
      CALL ld_act_add(v_td, ld_cfg('LD_ACT_FAIL'), 1, v_aapp, v_at);
      CALL ld_local_add(p_char, v_td, -ld_cfg('LD_LOCALREP_PER_EVENT'), 0, v_r);
      CALL ld_event(p_op, v_td, 2, p_char, 0, v_aapp, 0, p_contract);
      SET v_state = 4;
      SET v_res = 8;
    ELSE
      SET v_reward = ct_reward(v_dist, v_pts, v_at, v_cf, v_ct, v_prem, p_integrity);
      SET v_paid = LEAST(v_reward, GREATEST(ld_cfg('LD_WORK_HOURLY_CAP') - ld_work_earned(p_char), 0));
      CALL ld_bank_move(p_op, p_char, 'contract', v_paid, CONCAT('ct', p_contract), v_app, v_bal);
      SET v_paid = v_app;
      INSERT INTO `ct_payouts` (`contract_id`, `character_id`, `op_id`, `amount`, `reward`, `capped`, `integrity`, `seconds`)
      VALUES (p_contract, p_char, p_op, v_paid, v_reward, IF(v_paid < v_reward, 1, 0), p_integrity, LEAST(p_seconds, 4294967295));
      UPDATE `ct_contracts` SET `state` = 3, `participants` = 0, `done_at` = NOW() WHERE `id` = p_contract;
      UPDATE `ct_participants` SET `state` = 3 WHERE `contract_id` = p_contract AND `character_id` = p_char;
      SET v_state = 3;
      SET v_ontime = IF(v_el <= v_lim, 1, 0);
      CALL ld_rep_add(p_char, 1, ld_cfg('PRO_REP_DELIVERY') + IF(v_ontime = 1, ld_cfg('PRO_REP_ONTIME'), 0), 1, 0, v_pts);
      -- anti-farming: max LD_DELIV_HOURLY_PER_CHAR consegne contabilizzate/h per personaggio e quartiere; stessa coppia di punti entro LD_PAIR_LOCK_S non conta
      IF (SELECT COUNT(*) FROM `ld_events` WHERE `actor_char` = p_char AND `district_id` = v_td AND `type` = 1 AND `counted` = 1
            AND `created_at` > DATE_SUB(NOW(), INTERVAL 1 HOUR)) < ld_cfg('LD_DELIV_HOURLY_PER_CHAR')
         AND NOT EXISTS (SELECT 1 FROM `ct_payouts` y JOIN `ct_contracts` k ON k.`id` = y.`contract_id`
            WHERE y.`character_id` = p_char AND y.`contract_id` <> p_contract AND k.`from_pt` = v_fpt AND k.`to_pt` = v_tpt
              AND k.`from_district` = v_fd AND k.`to_district` = v_td AND y.`paid_at` > DATE_SUB(NOW(), INTERVAL ld_cfg('LD_PAIR_LOCK_S') SECOND)) THEN
        SET v_counted = 1;
        CALL ld_act_add(v_td, ld_cfg('LD_ACT_DELIVERY_DEST'), 1, v_aapp, v_at);
        CALL ld_act_add(v_fd, ld_cfg('LD_ACT_DELIVERY_ORIG'), 1, v_oapp, v_af);
        CALL ld_local_add(p_char, v_td, ld_cfg('LD_LOCALREP_PER_EVENT'), 1, v_r);
        CALL ld_local_add(p_char, v_fd, ld_cfg('LD_LOCALREP_PER_EVENT'), 1, v_r);
        CALL ld_event(CONCAT(p_op, '-d'), v_td, 1, p_char, v_paid, v_aapp, 1, p_contract);
        CALL ld_event(CONCAT(p_op, '-o'), v_fd, 1, p_char, v_paid, v_oapp, 1, p_contract);
        CALL ld_project_fund(v_td, (v_reward * ld_cfg('PRJ_FUND_PCT')) / 100, p_char, 1, 0, v_pj, v_pp, v_ps);
        CALL ld_project_fund(v_fd, (v_reward * ld_cfg('PRJ_FUND_PCT')) / 100, p_char, 1, 0, v_pj, v_pp, v_ps);
        SELECT `activity` INTO v_at FROM `ld_districts` WHERE `id` = v_td;
        SELECT `activity` INTO v_af FROM `ld_districts` WHERE `id` = v_fd;
      ELSE
        CALL ld_event(CONCAT(p_op, '-d'), v_td, 1, p_char, v_paid, 0, 0, p_contract);
      END IF;
      SET v_res = IF(v_paid < v_reward, 6, 1);
    END IF;
  END IF;
  CALL ld_oplog(p_op, 'ct_complete', p_char, 0, p_contract, v_res, v_paid, v_bal, v_ontime, v_af, v_pts, v_at, v_reward);
  COMMIT;
END//

-- --- noleggi -------------------------------------------------------------------------------------------------------------

-- Apertura. p_vkind 1 Faggio, 2 Previon, 3 Manana (classe 1, blocchi da RENT_BLOCK_MIN, sconto rep locale sul canone), 4 Taxi (classe 2, blocchi da 60 min,
-- livello TAXI_MIN_LEVEL, nessuno sconto). Canone + cauzione addebitati INSIEME, prima dalla BANCA e per il resto dai contanti: p_cash = contanti che il Pawn ha
-- GIA' riservato (tolti da pMoney); ne usa al massimo totale - banca e il Pawn restituisce la differenza (other_amount = contanti usati).
-- Esiti: 9 argomenti, 7 requisiti (noleggio gia' aperto, contratto attivo, livello), 3 fondi insufficienti (banca + contanti riservati < totale), 1 ok.
-- ld_ops: ref = id noleggio, amount = totale addebitato, balance_after, other_amount = contanti usati, v1 = due_at (UNIX), v2 = canone, v3 = cauzione.
DROP PROCEDURE IF EXISTS sp_rental_open//
CREATE PROCEDURE sp_rental_open(IN p_op VARCHAR(40), IN p_char INT UNSIGNED, IN p_vkind INT, IN p_model INT, IN p_blocks INT, IN p_cash INT, IN p_level INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_class INT;
  DECLARE v_feeb INT DEFAULT 0;
  DECLARE v_dep INT DEFAULT 0;
  DECLARE v_min INT DEFAULT 0;
  DECLARE v_lvl INT DEFAULT 0;
  DECLARE v_disc INT DEFAULT 0;
  DECLARE v_fee INT DEFAULT 0;
  DECLARE v_tot INT DEFAULT 0;
  DECLARE v_bpart INT DEFAULT 0;
  DECLARE v_cash INT DEFAULT 0;
  DECLARE v_id BIGINT UNSIGNED DEFAULT 0;
  DECLARE v_app INT;
  DECLARE v_bal INT;
  DECLARE v_due BIGINT DEFAULT 0;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  CASE p_vkind
    WHEN 1 THEN SET v_feeb = ld_cfg('RENT_FEE_FAGGIO');  SET v_dep = ld_cfg('RENT_DEP_FAGGIO');
    WHEN 2 THEN SET v_feeb = ld_cfg('RENT_FEE_PREVION'); SET v_dep = ld_cfg('RENT_DEP_PREVION');
    WHEN 3 THEN SET v_feeb = ld_cfg('RENT_FEE_MANANA');  SET v_dep = ld_cfg('RENT_DEP_MANANA');
    WHEN 4 THEN SET v_feeb = ld_cfg('TAXI_RENT_HOUR');   SET v_dep = ld_cfg('TAXI_DEP');
    WHEN 5 THEN SET v_feeb = ld_cfg('RENT_FEE_STREET');  SET v_dep = ld_cfg('RENT_DEP_STREET');
    ELSE SET v_feeb = 0;
  END CASE;
  SET v_class = IF(p_vkind = 4, 2, 1);
  SET v_min = IF(v_class = 2, 60, IF(p_vkind = 5, ld_cfg('RENT_STREET_MIN'), ld_cfg('RENT_BLOCK_MIN')));
  SET v_lvl = IF(v_class = 2, ld_cfg('TAXI_MIN_LEVEL'), ld_cfg('RENT_MIN_LEVEL'));
  IF v_class = 1 AND p_vkind <> 5 AND v_bank IS NOT NULL THEN SET v_disc = ld_disc_pct(p_char); END IF;
  SET v_fee = (v_feeb * COALESCE(p_blocks, 0) * (100 - v_disc) + 99) DIV 100;
  SET v_tot = v_fee + v_dep;
  IF v_bank IS NULL OR v_feeb = 0 OR p_blocks IS NULL OR p_blocks NOT BETWEEN 1 AND IF(p_vkind = 5, 1, ld_cfg('RENT_BLOCKS_MAX')) OR p_cash IS NULL OR p_cash < 0
     OR p_level IS NULL OR p_model IS NULL OR p_model NOT BETWEEN 0 AND 65535 THEN
    SET v_res = 9;
  ELSEIF EXISTS (SELECT 1 FROM `rentals` WHERE `open_char` = p_char) OR EXISTS (SELECT 1 FROM `ct_participants` WHERE `active_char` = p_char) OR p_level < v_lvl THEN
    SET v_res = 7;
  ELSEIF CAST(v_bank AS SIGNED) + p_cash < v_tot THEN
    SET v_res = 3;
  ELSE
    SET v_bpart = LEAST(v_bank, v_tot);
    SET v_cash = v_tot - v_bpart;
    INSERT INTO `rentals` (`character_id`, `class`, `vkind`, `model`, `blocks`, `fee`, `deposit`, `cash_paid`, `state`, `due_at`, `open_op`)
    VALUES (p_char, v_class, p_vkind, p_model, p_blocks, v_fee, v_dep, v_cash, 1, DATE_ADD(NOW(), INTERVAL p_blocks * v_min MINUTE), p_op);
    SET v_id = LAST_INSERT_ID();
    CALL ld_bank_move(p_op, p_char, 'rental', -v_bpart, CONCAT('rt', v_id), v_app, v_bal);
    SELECT UNIX_TIMESTAMP(`due_at`) INTO v_due FROM `rentals` WHERE `id` = v_id;
    CALL ld_event(CONCAT(p_op, '-e'), 0, 4, p_char, v_tot, 0, 0, v_id);
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'rent_open', p_char, 0, v_id, v_res, IF(v_res = 1, v_tot, 0), v_bal, v_cash, 0, v_due, v_fee, v_dep);
  COMMIT;
END//

-- Chiusura. p_mode: 1 riconsegna (rimborso = deposit x (salute-ZERO)/(FULL-ZERO); oltre due_at + RENT_GRACE_MS = scaduto, esito 5), 2 scaduto (cauzione persa),
-- 3 interrotto (disconnessione/morte: cauzione 100%), 5 annullato (compensazione: rimborso totale). Esiti: 9, 4 non e' suo, 2 gia' chiuso, 5 scaduto, 1 ok.
-- ld_ops: ref = id, amount = rimborso (in banca), balance_after, v1 = nuovo stato.
DROP PROCEDURE IF EXISTS sp_rental_close//
CREATE PROCEDURE sp_rental_close(IN p_op VARCHAR(40), IN p_rental BIGINT UNSIGNED, IN p_char INT UNSIGNED, IN p_mode INT, IN p_health INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_owner INT UNSIGNED;
  DECLARE v_state INT;
  DECLARE v_due TIMESTAMP;
  DECLARE v_new INT DEFAULT 0;
  DECLARE v_ref INT DEFAULT 0;
  DECLARE v_bal INT;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  SELECT `character_id`, `state`, `due_at` INTO v_owner, v_state, v_due FROM `rentals` WHERE `id` = p_rental FOR UPDATE;
  IF v_bank IS NULL OR v_owner IS NULL OR p_mode NOT IN (1, 2, 3, 5) OR p_health IS NULL THEN
    SET v_res = 9;
  ELSEIF v_owner <> p_char THEN
    SET v_res = 4;
  ELSEIF v_state <> 1 THEN
    SET v_res = 2;
    SET v_new = v_state;
  ELSE
    SET v_new = CASE p_mode WHEN 1 THEN 2 WHEN 2 THEN 4 WHEN 3 THEN 3 ELSE 5 END;
    IF p_mode = 1 AND NOW() > DATE_ADD(v_due, INTERVAL ld_cfg('RENT_GRACE_MS') DIV 1000 SECOND) THEN SET v_new = 4; END IF;
    CALL ld_rental_finish(p_rental, v_new, p_health, p_op, v_ref, v_bal);
    SET v_res = IF(p_mode = 1 AND v_new = 4, 5, 1);
  END IF;
  CALL ld_oplog(p_op, 'rent_close', p_char, 0, p_rental, v_res, v_ref, v_bal, 0, 0, v_new, 0, 0);
  COMMIT;
END//

-- Compensazione dei contanti riservati dal Pawn per un'operazione NON riuscita la cui sessione e' scaduta: accredito in BANCA (commutativo con il salvataggio
-- a delta), kind p_kind, ref 'cashback'. Idempotente per p_op (riga di ledger <p_op>-c). Esiti: 1 ok, 2 gia' fatto, 9 argomenti.
DROP PROCEDURE IF EXISTS sp_ld_cash_back//
CREATE PROCEDURE sp_ld_cash_back(IN p_op VARCHAR(40), IN p_char INT UNSIGNED, IN p_kind VARCHAR(12), IN p_amount INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_app INT;
  DECLARE v_bal INT;
  DECLARE v_op VARCHAR(40);
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = v_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  SET v_op = CONCAT(p_op, '-c');
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = v_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  IF v_bank IS NULL OR p_amount IS NULL OR p_amount < 1 OR p_kind NOT IN ('rental', 'taxi', 'project') THEN
    SET v_res = 9;
  ELSE
    CALL ld_bank_move(v_op, p_char, p_kind, p_amount, 'cashback', v_app, v_bal);
    SET v_res = 1;
  END IF;
  CALL ld_oplog(v_op, 'cash_back', p_char, 0, 0, v_res, IF(v_res = 1, v_app, 0), v_bal, 0, 0, 0, 0, 0);
  COMMIT;
END//

-- --- taxi ----------------------------------------------------------------------------------------------------------------

-- Apertura di una corsa. p_kind 1 NPC (p_passenger 0), 2 giocatore. Requisiti (7): livello TAXI_MIN_LEVEL, taxi a noleggio aperto (vkind 4), passeggero valido
-- e diverso; stesso account o stesso IP: max TAXI_PAIR_HOURLY corse/h. 4 = corsa gia' aperta per l'autista. 6 = NPC: TAXI_NPC_HOURLY corse pagate nell'ultima ora.
-- ld_ops: ref = id corsa.
DROP PROCEDURE IF EXISTS sp_taxi_start//
CREATE PROCEDURE sp_taxi_start(IN p_op VARCHAR(40), IN p_kind INT, IN p_driver INT UNSIGNED, IN p_passenger INT UNSIGNED, IN p_from_d INT, IN p_level INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_pb INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_id BIGINT UNSIGNED DEFAULT 0;
  DECLARE v_same INT DEFAULT 0;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  IF p_kind = 2 AND p_passenger > 0 AND p_passenger < p_driver THEN SELECT MAX(`bank`) INTO v_pb FROM `characters` WHERE `id` = p_passenger FOR UPDATE; END IF;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_driver FOR UPDATE;
  IF p_kind = 2 AND p_passenger > p_driver THEN SELECT MAX(`bank`) INTO v_pb FROM `characters` WHERE `id` = p_passenger FOR UPDATE; END IF;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  IF p_kind = 2 AND v_pb IS NOT NULL THEN
    SELECT COUNT(*) INTO v_same FROM `characters` a JOIN `accounts` aa ON aa.`id` = a.`account_id`, `characters` b JOIN `accounts` ba ON ba.`id` = b.`account_id`
      WHERE a.`id` = p_driver AND b.`id` = p_passenger AND (a.`account_id` = b.`account_id` OR (aa.`last_ip` IS NOT NULL AND aa.`last_ip` = ba.`last_ip`));
  END IF;
  IF v_bank IS NULL OR p_kind NOT IN (1, 2) OR p_level IS NULL OR p_from_d IS NULL OR p_from_d NOT BETWEEN 0 AND 255
     OR (p_kind = 1 AND COALESCE(p_passenger, 0) <> 0) OR (p_kind = 2 AND (v_pb IS NULL OR p_passenger = p_driver)) THEN
    SET v_res = 9;
  ELSEIF p_level < ld_cfg('TAXI_MIN_LEVEL') OR NOT EXISTS (SELECT 1 FROM `rentals` WHERE `open_char` = p_driver AND `vkind` = 4)
     OR (v_same > 0 AND (SELECT COUNT(*) FROM `taxi_rides` WHERE `driver_char` IN (p_driver, p_passenger) AND `passenger_char` IN (p_driver, p_passenger)
          AND `created_at` > DATE_SUB(NOW(), INTERVAL 1 HOUR)) >= ld_cfg('TAXI_PAIR_HOURLY')) THEN
    SET v_res = 7;
  ELSEIF EXISTS (SELECT 1 FROM `taxi_rides` WHERE `open_driver` = p_driver) THEN
    SET v_res = 4;
  ELSEIF p_kind = 1 AND (SELECT COUNT(*) FROM `taxi_rides` WHERE `driver_char` = p_driver AND `kind` = 1 AND `state` = 2
          AND `paid_at` > DATE_SUB(NOW(), INTERVAL 1 HOUR)) >= ld_cfg('TAXI_NPC_HOURLY') THEN
    SET v_res = 6;
  ELSE
    INSERT INTO `taxi_rides` (`kind`, `driver_char`, `passenger_char`, `from_district`, `state`, `op_id`)
    VALUES (p_kind, p_driver, IF(p_kind = 2, p_passenger, NULL), p_from_d, 1, p_op);
    SET v_id = LAST_INSERT_ID();
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'taxi_start', p_driver, IF(p_kind = 2, p_passenger, 0), v_id, v_res, 0, v_bank, 0, 0, p_kind, 0, 0);
  COMMIT;
END//

-- Pagamento all'arrivo. Tariffa = TAXI_BASE + TAXI_PER_M_X100/100 x metri percorsi. NPC: + mancia di grado (ld_tier_pct sui punti taxi), creazione di denaro nel
-- tetto LD_WORK_HOURLY_CAP (esito 6 = residuo), p_rating 1..5 = voto NPC (0 nessuno). Giocatore: p_accept 1 = il passeggero paga dalla banca la tariffa scontata
-- (ld_disc_pct, pozzo) e l'autista riceve tutto meno TAXI_TAX_PCT (pozzo); p_accept 0 = DISPUTA (esito 8, nessun pagamento). Fondi del passeggero insufficienti = 3
-- (la corsa resta aperta). Effetti (se la coppia non supera TAXI_PAIR_HOURLY/h): rep taxi +PRO_REP_RIDE, attivita' +LD_ACT_RIDE a prelievo e arrivo, rep locale,
-- fondo progetti +PRJ_FUND_RIDE_X10/10 $, eventi 3. Ledger: NPC <op> (autista); giocatore <op>-o (passeggero, -), <op>-i (autista, +), kind 'taxi', ref 'tx<id>'.
-- ld_ops: amount = netto autista, balance_after = saldo autista, other_amount = pagato dal passeggero, other_balance_after = saldo passeggero, v1 = punti taxi,
-- v2 = stato corsa, v3 = tassa.
DROP PROCEDURE IF EXISTS sp_taxi_pay//
CREATE PROCEDURE sp_taxi_pay(IN p_op VARCHAR(40), IN p_ride BIGINT UNSIGNED, IN p_driver INT UNSIGNED, IN p_dist INT, IN p_to_d INT, IN p_accept INT, IN p_rating INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_pb INT;
  DECLARE v_pass INT UNSIGNED;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_kind INT;
  DECLARE v_drv INT UNSIGNED;
  DECLARE v_state INT DEFAULT 0;
  DECLARE v_fd INT DEFAULT 0;
  DECLARE v_base INT DEFAULT 0;
  DECLARE v_gross INT DEFAULT 0;
  DECLARE v_net INT DEFAULT 0;
  DECLARE v_paid INT DEFAULT 0;
  DECLARE v_tax INT DEFAULT 0;
  DECLARE v_app INT;
  DECLARE v_bal INT;
  DECLARE v_pbal INT DEFAULT 0;
  DECLARE v_pts INT DEFAULT 0;
  DECLARE v_count INT DEFAULT 1;
  DECLARE v_a INT;
  DECLARE v_x INT;
  DECLARE v_r INT;
  DECLARE v_pj INT UNSIGNED;
  DECLARE v_pp INT;
  DECLARE v_ps INT;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  SELECT MAX(`passenger_char`) INTO v_pass FROM `taxi_rides` WHERE `id` = p_ride;
  START TRANSACTION;
  IF v_pass IS NOT NULL AND v_pass < p_driver THEN SELECT MAX(`bank`) INTO v_pb FROM `characters` WHERE `id` = v_pass FOR UPDATE; END IF;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_driver FOR UPDATE;
  IF v_pass IS NOT NULL AND v_pass > p_driver THEN SELECT MAX(`bank`) INTO v_pb FROM `characters` WHERE `id` = v_pass FOR UPDATE; END IF;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  SET v_pbal = COALESCE(v_pb, 0);
  SELECT `kind`, `driver_char`, `state`, `from_district` INTO v_kind, v_drv, v_state, v_fd FROM `taxi_rides` WHERE `id` = p_ride FOR UPDATE;
  SET v_pts = ld_pro_points(p_driver, 2);
  IF v_bank IS NULL OR v_kind IS NULL OR p_dist IS NULL OR p_dist < 0 OR p_to_d IS NULL OR p_to_d NOT BETWEEN 0 AND 255 OR p_accept NOT IN (0, 1)
     OR p_rating IS NULL OR p_rating NOT BETWEEN 0 AND 5 OR v_drv <> p_driver OR (v_kind = 2 AND v_pb IS NULL) THEN
    SET v_res = 9;
  ELSEIF v_state <> 1 THEN
    SET v_res = IF(v_state IN (2, 3), 2, 4);
  ELSEIF v_kind = 2 AND p_accept = 0 THEN
    UPDATE `taxi_rides` SET `state` = 3, `distance_m` = p_dist, `to_district` = p_to_d, `pay_op` = p_op WHERE `id` = p_ride;
    SET v_state = 3;
    SET v_res = 8;
  ELSE
    SET v_base = ld_cfg('TAXI_BASE') + (CAST(p_dist AS SIGNED) * ld_cfg('TAXI_PER_M_X100')) DIV 100;
    IF v_kind = 1 THEN
      SET v_gross = v_base + (v_base * ld_tier_pct(v_pts)) DIV 100;
      SET v_net = LEAST(v_gross, GREATEST(ld_cfg('LD_WORK_HOURLY_CAP') - ld_work_earned(p_driver), 0));
      CALL ld_bank_move(p_op, p_driver, 'taxi', v_net, CONCAT('tx', p_ride), v_app, v_bal);
      SET v_net = v_app;
      SET v_paid = v_net;
      SET v_res = IF(v_net < v_gross, 6, 1);
    ELSE
      SET v_paid = (v_base * (100 - ld_disc_pct(v_pass)) + 99) DIV 100;
      IF v_pb < v_paid THEN
        SET v_res = 3;
      ELSE
        SET v_tax = (v_paid * ld_cfg('TAXI_TAX_PCT') + 99) DIV 100;
        SET v_net = v_paid - v_tax;
        CALL ld_bank_move(CONCAT(p_op, '-o'), v_pass, 'taxi', -v_paid, CONCAT('tx', p_ride), v_app, v_pbal);
        CALL ld_bank_move(CONCAT(p_op, '-i'), p_driver, 'taxi', v_net, CONCAT('tx', p_ride), v_app, v_bal);
        SET v_res = 1;
      END IF;
    END IF;
    IF v_res IN (1, 6) THEN
      UPDATE `taxi_rides` SET `state` = 2, `distance_m` = p_dist, `to_district` = p_to_d, `fare` = v_paid, `tax` = v_tax, `driver_net` = v_net,
        `rating` = IF(v_kind = 1 AND p_rating > 0, p_rating, NULL), `pay_op` = p_op, `paid_at` = NOW() WHERE `id` = p_ride;
      SET v_state = 2;
      IF v_kind = 2 AND (SELECT COUNT(*) FROM `taxi_rides` WHERE `driver_char` = p_driver AND `passenger_char` = v_pass AND `state` = 2 AND `id` <> p_ride
            AND `paid_at` > DATE_SUB(NOW(), INTERVAL 1 HOUR)) >= ld_cfg('TAXI_PAIR_HOURLY') THEN
        SET v_count = 0;
      END IF;
      IF v_count = 1 THEN
        CALL ld_rep_add(p_driver, 2, ld_cfg('PRO_REP_RIDE'), 1, 0, v_pts);
        IF v_kind = 1 AND p_rating > 0 THEN UPDATE `pro_rep` SET `rating_sum` = `rating_sum` + p_rating, `rating_cnt` = `rating_cnt` + 1 WHERE `character_id` = p_driver AND `track` = 2; END IF;
        -- quartieri in ordine di id (lock), un effetto per quartiere distinto
        IF v_fd > 0 OR p_to_d > 0 THEN
          SELECT COUNT(*) INTO v_x FROM (SELECT `id` FROM `ld_districts` WHERE `id` IN (v_fd, p_to_d) ORDER BY `id` FOR UPDATE) d;
        END IF;
        IF v_fd > 0 AND EXISTS (SELECT 1 FROM `ld_districts` WHERE `id` = v_fd) THEN
          CALL ld_act_add(v_fd, ld_cfg('LD_ACT_RIDE'), 1, v_a, v_x);
          CALL ld_local_add(p_driver, v_fd, ld_cfg('LD_LOCALREP_PER_EVENT'), 1, v_r);
          CALL ld_event(CONCAT(p_op, '-p'), v_fd, 3, p_driver, v_paid, v_a, 1, p_ride);
          CALL ld_project_fund(v_fd, ld_cfg('PRJ_FUND_RIDE_X10') / 10, p_driver, 1, 0, v_pj, v_pp, v_ps);
        END IF;
        IF p_to_d > 0 AND p_to_d <> v_fd AND EXISTS (SELECT 1 FROM `ld_districts` WHERE `id` = p_to_d) THEN
          CALL ld_act_add(p_to_d, ld_cfg('LD_ACT_RIDE'), 1, v_a, v_x);
          CALL ld_local_add(p_driver, p_to_d, ld_cfg('LD_LOCALREP_PER_EVENT'), 1, v_r);
          CALL ld_event(CONCAT(p_op, '-a'), p_to_d, 3, p_driver, v_paid, v_a, 1, p_ride);
          CALL ld_project_fund(p_to_d, ld_cfg('PRJ_FUND_RIDE_X10') / 10, p_driver, 1, 0, v_pj, v_pp, v_ps);
        END IF;
      END IF;
    END IF;
  END IF;
  CALL ld_oplog(p_op, 'taxi_pay', p_driver, COALESCE(v_pass, 0), p_ride, v_res, IF(v_res IN (1, 6), v_net, 0), v_bal,
    IF(v_res IN (1, 6), v_paid, 0), v_pbal, v_pts, v_state, IF(v_res IN (1, 6), v_tax, 0));
  COMMIT;
END//

-- Annullo di una corsa aperta (veicolo distrutto, fuori dal taxi, passeggero sceso). 1 ok, 2 gia' chiusa, 4 non sua, 9 argomenti.
DROP PROCEDURE IF EXISTS sp_taxi_cancel//
CREATE PROCEDURE sp_taxi_cancel(IN p_op VARCHAR(40), IN p_ride BIGINT UNSIGNED, IN p_driver INT UNSIGNED)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_drv INT UNSIGNED;
  DECLARE v_state INT DEFAULT 0;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_driver FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SELECT `driver_char`, `state` INTO v_drv, v_state FROM `taxi_rides` WHERE `id` = p_ride FOR UPDATE;
  IF v_bank IS NULL OR v_drv IS NULL THEN SET v_res = 9;
  ELSEIF v_drv <> p_driver THEN SET v_res = 4;
  ELSEIF v_state <> 1 THEN SET v_res = 2;
  ELSE
    UPDATE `taxi_rides` SET `state` = 4 WHERE `id` = p_ride;
    SET v_state = 4;
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'taxi_cancel', p_driver, 0, p_ride, v_res, 0, COALESCE(v_bank, 0), 0, 0, 0, v_state, 0);
  COMMIT;
END//

-- Voto 1..5 del passeggero GIOCATORE a una corsa pagata (una volta). 1 ok, 2 gia' votata, 4 non e' il passeggero / non pagata, 9 argomenti.
-- ld_ops: v1 = voto, v2 = media mobile x100 sugli ultimi TAXI_RATING_WINDOW voti dell'autista.
DROP PROCEDURE IF EXISTS sp_taxi_rate//
CREATE PROCEDURE sp_taxi_rate(IN p_op VARCHAR(40), IN p_ride BIGINT UNSIGNED, IN p_passenger INT UNSIGNED, IN p_stars INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_drv INT UNSIGNED;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_kind INT;
  DECLARE v_pass INT UNSIGNED;
  DECLARE v_state INT;
  DECLARE v_rating INT;
  DECLARE v_avg INT DEFAULT 0;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  SELECT MAX(`driver_char`) INTO v_drv FROM `taxi_rides` WHERE `id` = p_ride;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = v_drv FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SELECT `kind`, `passenger_char`, `state`, `rating` INTO v_kind, v_pass, v_state, v_rating FROM `taxi_rides` WHERE `id` = p_ride FOR UPDATE;
  IF v_bank IS NULL OR v_kind IS NULL OR p_stars IS NULL OR p_stars NOT BETWEEN 1 AND 5 THEN SET v_res = 9;
  ELSEIF v_kind <> 2 OR v_pass IS NULL OR v_pass <> p_passenger OR v_state <> 2 THEN SET v_res = 4;
  ELSEIF v_rating IS NOT NULL THEN SET v_res = 2;
  ELSE
    UPDATE `taxi_rides` SET `rating` = p_stars WHERE `id` = p_ride;
    INSERT IGNORE INTO `pro_rep` (`character_id`, `track`) VALUES (v_drv, 2);
    UPDATE `pro_rep` SET `rating_sum` = `rating_sum` + p_stars, `rating_cnt` = `rating_cnt` + 1 WHERE `character_id` = v_drv AND `track` = 2;
    SET v_res = 1;
  END IF;
  SELECT COALESCE(ROUND(AVG(`rating`) * 100), 0) INTO v_avg FROM (SELECT `rating` FROM `taxi_rides` WHERE `driver_char` = v_drv AND `rating` IS NOT NULL
    ORDER BY `id` DESC LIMIT 20) z;
  CALL ld_oplog(p_op, 'taxi_rate', p_passenger, COALESCE(v_drv, 0), p_ride, v_res, 0, 0, 0, 0, p_stars, v_avg, 0);
  COMMIT;
END//

-- --- progetti comunitari ------------------------------------------------------------------------------------------------

-- Donazione volontaria (pozzo) dalla banca: PRJ_DON_MIN..PRJ_DON_MAX (9), tempo di gioco >= PRJ_DON_MIN_PLAYED_MIN (7, p_played_min dal Pawn),
-- max PRJ_DON_DAY $/giorno (7), progetto attivo (4; 5 se scaduto), fondi (3). Punti comunita' = importo / PRJ_DON_REP_PER (tetto PRJ_DAY_CAP/giorno) e rep locale
-- dello stesso valore; con rep locale negativa annulla anche una penalita' (+LD_LOCALREP_PER_EVENT, fino a 0). Ledger kind 'project' ref 'pj<id>'.
-- ld_ops: ref = progetto, amount = donato, balance_after, v1 = punti ottenuti, v2 = fondo (intero), v3 = stato progetto dopo (1 attivo, 2 completato).
DROP PROCEDURE IF EXISTS sp_project_contrib//
CREATE PROCEDURE sp_project_contrib(IN p_op VARCHAR(40), IN p_char INT UNSIGNED, IN p_project INT UNSIGNED, IN p_amount INT, IN p_played_min INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_d INT;
  DECLARE v_state INT;
  DECLARE v_ends TIMESTAMP;
  DECLARE v_today INT DEFAULT 0;
  DECLARE v_app INT;
  DECLARE v_bal INT;
  DECLARE v_pj INT UNSIGNED;
  DECLARE v_pts INT DEFAULT 0;
  DECLARE v_ps INT DEFAULT 0;
  DECLARE v_r INT;
  DECLARE v_fund INT DEFAULT 0;
  DECLARE v_x INT;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  SELECT MAX(`district_id`) INTO v_d FROM `ld_projects` WHERE `id` = p_project;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  IF v_d IS NOT NULL THEN SELECT `id` INTO v_x FROM `ld_districts` WHERE `id` = v_d FOR UPDATE; END IF;
  SELECT MAX(`state`), MAX(`ends_at`) INTO v_state, v_ends FROM `ld_projects` WHERE `id` = p_project;
  SELECT COALESCE(SUM(`day_donated`), 0) INTO v_today FROM `ld_project_contrib` WHERE `character_id` = p_char AND `day_key` = TO_DAYS(NOW());
  IF v_bank IS NULL OR v_state IS NULL OR p_amount IS NULL OR p_amount NOT BETWEEN ld_cfg('PRJ_DON_MIN') AND ld_cfg('PRJ_DON_MAX') OR p_played_min IS NULL THEN
    SET v_res = 9;
  ELSEIF v_state <> 1 THEN
    SET v_res = 4;
  ELSEIF v_ends <= NOW() THEN
    SET v_res = 5;
  ELSEIF p_played_min < ld_cfg('PRJ_DON_MIN_PLAYED_MIN') OR v_today + p_amount > ld_cfg('PRJ_DON_DAY') THEN
    SET v_res = 7;
  ELSEIF v_bank < p_amount THEN
    SET v_res = 3;
  ELSE
    CALL ld_bank_move(p_op, p_char, 'project', -p_amount, CONCAT('pj', p_project), v_app, v_bal);
    UPDATE `ld_district_rep` SET `rep` = LEAST(`rep` + ld_cfg('LD_LOCALREP_PER_EVENT'), 0) WHERE `character_id` = p_char AND `district_id` = v_d AND `rep` < 0;
    CALL ld_project_fund(v_d, p_amount, p_char, p_amount DIV GREATEST(ld_cfg('PRJ_DON_REP_PER'), 1), p_amount, v_pj, v_pts, v_ps);
    IF v_pts > 0 THEN CALL ld_local_add(p_char, v_d, v_pts, 0, v_r); END IF;
    SELECT FLOOR(`fund`) INTO v_fund FROM `ld_projects` WHERE `id` = p_project;
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'prj_contrib', p_char, 0, p_project, v_res, IF(v_res = 1, p_amount, 0), v_bal, 0, 0, v_pts, v_fund, IF(v_res = 1, v_ps, COALESCE(v_state, 0)));
  COMMIT;
END//

-- --- onboarding -----------------------------------------------------------------------------------------------------------

-- Passo p_step (1..ONB_STEPS, solo il successivo all'ultimo completato) o 0 = salta (suggerimenti spenti, catena chiusa SENZA premio). Premi una tantum:
-- passo 3 (consegna di benvenuto) = ONB_REWARD $ in banca (kind 'onboarding', fuori dai tetti e dall'attivita'); passo finale = pacco accoglienza + ONB_EXP exp
-- (consegnati dal Pawn: inventario ed exp sono suoi). Esiti: 1 ok, 2 gia' fatto / catena chiusa, 4 passo fuori sequenza, 9 argomenti.
-- ld_ops: amount = $ pagati, balance_after, v1 = passo raggiunto, v2 = 1 se il Pawn deve consegnare il pacco finale, v3 = exp da dare.
DROP PROCEDURE IF EXISTS sp_onboard_step//
CREATE PROCEDURE sp_onboard_step(IN p_op VARCHAR(40), IN p_char INT UNSIGNED, IN p_step INT)
main: BEGIN
  DECLARE v_bank INT;
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_step INT DEFAULT 0;
  DECLARE v_done TIMESTAMP;
  DECLARE v_rew INT DEFAULT 0;
  DECLARE v_pay INT DEFAULT 0;
  DECLARE v_kit INT DEFAULT 0;
  DECLARE v_exp INT DEFAULT 0;
  DECLARE v_app INT;
  DECLARE v_bal INT;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`bank`) INTO v_bank FROM `characters` WHERE `id` = p_char FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  SET v_bal = v_bank;
  IF v_bank IS NOT NULL THEN
    INSERT IGNORE INTO `ld_onboarding` (`character_id`) VALUES (p_char);
    SELECT `step`, `done_at`, `rewarded` INTO v_step, v_done, v_rew FROM `ld_onboarding` WHERE `character_id` = p_char FOR UPDATE;
  END IF;
  IF v_bank IS NULL OR p_step IS NULL OR p_step NOT BETWEEN 0 AND ld_cfg('ONB_STEPS') THEN
    SET v_res = 9;
  ELSEIF v_done IS NOT NULL OR (p_step > 0 AND p_step <= v_step) THEN
    SET v_res = 2;
  ELSEIF p_step = 0 THEN
    UPDATE `ld_onboarding` SET `done_at` = NOW(), `hints_off` = 1 WHERE `character_id` = p_char;
    SET v_res = 1;
  ELSEIF p_step <> v_step + 1 THEN
    SET v_res = 4;
  ELSE
    SET v_step = p_step;
    IF p_step = 3 AND (v_rew & 1) = 0 THEN
      CALL ld_bank_move(p_op, p_char, 'onboarding', ld_cfg('ONB_REWARD'), 'welcome', v_app, v_bal);
      SET v_pay = v_app;
      SET v_rew = v_rew | 1;
    END IF;
    IF p_step = ld_cfg('ONB_STEPS') AND (v_rew & 2) = 0 THEN
      SET v_kit = 1;
      SET v_exp = ld_cfg('ONB_EXP');
      SET v_rew = v_rew | 2;
    END IF;
    UPDATE `ld_onboarding` SET `step` = v_step, `rewarded` = v_rew, `done_at` = IF(p_step = ld_cfg('ONB_STEPS'), NOW(), NULL) WHERE `character_id` = p_char;
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'onboard', p_char, 0, 0, v_res, v_pay, v_bal, 0, 0, v_step, v_kit, v_exp);
  COMMIT;
END//

-- Suggerimenti on/off (senza chiudere la catena). Nessun denaro, nessun op_id.
DROP PROCEDURE IF EXISTS sp_onboard_hints//
CREATE PROCEDURE sp_onboard_hints(IN p_char INT UNSIGNED, IN p_off INT)
BEGIN
  INSERT INTO `ld_onboarding` (`character_id`, `hints_off`) SELECT `id`, IF(p_off <> 0, 1, 0) FROM `characters` WHERE `id` = p_char
    ON DUPLICATE KEY UPDATE `hints_off` = VALUES(`hints_off`);
END//

-- --- hook ed eventi di staff ----------------------------------------------------------------------------------------------

-- Evento esterno: 6 CRIMINE_SEGNALATO (pressione +LD_CRIME_EVENT, sicurezza +LD_SEC_CRIME_EVENT), 7 PATTUGLIA (solo cronologia in slice).
-- 1 ok, 9 argomenti (quartiere o tipo non validi). ld_ops: v1 = attivita', v2 = sicurezza, v3 = pressione dopo.
DROP PROCEDURE IF EXISTS sp_ld_emit//
CREATE PROCEDURE sp_ld_emit(IN p_op VARCHAR(40), IN p_district INT, IN p_type INT, IN p_actor INT UNSIGNED, IN p_value INT)
main: BEGIN
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_act INT DEFAULT 0;
  DECLARE v_sec INT DEFAULT 0;
  DECLARE v_cr INT DEFAULT 0;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`activity`), MAX(`security`), MAX(`crime`) INTO v_act, v_sec, v_cr FROM `ld_districts` WHERE `id` = p_district FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  IF v_act IS NULL OR p_type NOT IN (6, 7) OR p_value IS NULL THEN
    SET v_res = 9;
  ELSE
    IF p_type = 6 THEN
      SET v_cr = LEAST(GREATEST(v_cr + ld_cfg('LD_CRIME_EVENT'), 0), ld_cfg('LD_CRIME_MAX'));
      SET v_sec = LEAST(GREATEST(v_sec + ld_cfg('LD_SEC_CRIME_EVENT'), 0), ld_cfg('LD_SEC_MAX'));
      UPDATE `ld_districts` SET `crime` = v_cr, `security` = v_sec WHERE `id` = p_district;
    END IF;
    CALL ld_event(p_op, p_district, p_type, COALESCE(p_actor, 0), p_value, 0, 1, NULL);
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'ld_emit', COALESCE(p_actor, 0), 0, COALESCE(p_district, 0), v_res, 0, 0, 0, 0, v_act, v_sec, v_cr);
  COMMIT;
END//

-- /ldset staff: imposta i valori del quartiere (-1 = invariato), clamp nei range; evento 9 ADMIN. Il Pawn scrive admin_log (DB_AdminLog).
DROP PROCEDURE IF EXISTS sp_ld_admin_set//
CREATE PROCEDURE sp_ld_admin_set(IN p_op VARCHAR(40), IN p_district INT, IN p_activity INT, IN p_security INT, IN p_crime INT, IN p_staff_char INT UNSIGNED)
main: BEGIN
  DECLARE v_prev INT;
  DECLARE v_res INT DEFAULT 9;
  DECLARE v_act INT DEFAULT 0;
  DECLARE v_sec INT DEFAULT 0;
  DECLARE v_cr INT DEFAULT 0;
  -- stessa op_id committata nel frattempo da un'altra connessione (UNIQUE): replay, nessun effetto
  DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN ROLLBACK; IF NOT EXISTS (SELECT 1 FROM `ld_ops` WHERE `op_id` = p_op) THEN RESIGNAL; END IF; END;
  IF p_op IS NULL OR CHAR_LENGTH(p_op) < 1 OR CHAR_LENGTH(p_op) > 36 THEN LEAVE main; END IF;
  START TRANSACTION;
  SELECT MAX(`activity`), MAX(`security`), MAX(`crime`) INTO v_act, v_sec, v_cr FROM `ld_districts` WHERE `id` = p_district FOR UPDATE;
  SELECT MAX(`result`) INTO v_prev FROM `ld_ops` WHERE `op_id` = p_op;
  IF v_prev IS NOT NULL THEN COMMIT; LEAVE main; END IF;
  IF v_act IS NULL OR p_activity IS NULL OR p_security IS NULL OR p_crime IS NULL THEN
    SET v_res = 9;
  ELSE
    IF p_activity >= 0 THEN SET v_act = LEAST(p_activity, ld_cfg('LD_ACT_MAX')); END IF;
    IF p_security >= 0 THEN SET v_sec = LEAST(p_security, ld_cfg('LD_SEC_MAX')); END IF;
    IF p_crime >= 0 THEN SET v_cr = LEAST(p_crime, ld_cfg('LD_CRIME_MAX')); END IF;
    UPDATE `ld_districts` SET `activity` = v_act, `security` = v_sec, `crime` = v_cr WHERE `id` = p_district;
    CALL ld_event(p_op, p_district, 9, COALESCE(p_staff_char, 0), v_act, 0, 0, NULL);
    SET v_res = 1;
  END IF;
  CALL ld_oplog(p_op, 'ld_admin', COALESCE(p_staff_char, 0), 0, COALESCE(p_district, 0), v_res, 0, 0, 0, 0, v_act, v_sec, v_cr);
  COMMIT;
END//

DELIMITER ;

-- FONTE UNICA dei dati iniziali di Living Los Santos (migrazione 011). NON includere a mano: 'scripts/db.sh build-sql' lo concatena DOPO
-- sql/procedures_living.sql. Idempotente: INSERT IGNORE (un valore gia' presente, anche riallineato dal Pawn, non viene mai sovrascritto).
-- Chiave = nome della costante di gameplay_config.inc SENZA il prefisso "GP_" (valori [DB] del §9): tenerle allineate (DB_LivingPushConfig lo fa al boot).
INSERT IGNORE INTO `ld_config` (`k`, `v`) VALUES
  ('MONEY_MAX', 1000000000),
  ('LD_WORK_HOURLY_CAP', 2800), ('LD_MIN_LEVEL', 2),
  ('LD_TICK_MS', 600000), ('LD_TICK_CATCHUP_MAX', 144), ('LD_TICK_DIV', 20), ('LD_TICK_MIN_STEP', 1),
  ('LD_ACT_MAX', 1000), ('LD_SEC_MAX', 100), ('LD_CRIME_MAX', 100),
  ('LD_D1_ACT_BASE', 500), ('LD_D1_SEC_BASE', 45), ('LD_D1_CRIME_BASE', 50),
  ('LD_D2_ACT_BASE', 450), ('LD_D2_SEC_BASE', 35), ('LD_D2_CRIME_BASE', 60),
  ('LD_D3_ACT_BASE', 500), ('LD_D3_SEC_BASE', 40), ('LD_D3_CRIME_BASE', 55),
  ('LD_ACT_DELIVERY_DEST', 6), ('LD_ACT_DELIVERY_ORIG', 3), ('LD_ACT_RIDE', 2), ('LD_ACT_FAIL', -3), ('LD_ACT_HOURLY_GAIN_CAP', 60),
  ('LD_CRIME_EVENT', 5), ('LD_SEC_CRIME_EVENT', -2), ('LD_SEC_TICK_CRIME_AT', 40),
  ('LD_ACT_MULT_MIN_PCT', 90), ('LD_ACT_MULT_MAX_PCT', 110), ('LD_CRIME_HAZARD_AT', 70), ('LD_CRIME_HAZARD_PCT', 10),
  ('LD_DELIV_HOURLY_PER_CHAR', 3), ('LD_PAIR_LOCK_S', 1800),
  ('LD_LOCALREP_MIN', -50), ('LD_LOCALREP_MAX', 100), ('LD_LOCALREP_PER_EVENT', 2), ('LD_REP_RECOVER_PER_EVENT', 4), ('LD_LOCALREP_DAY_CAP', 10),
  ('LD_PREMIUM_LOCALREP', 20), ('LD_PREMIUM_PCT', 20),
  ('LD_REP_KNOWN', 10), ('LD_REP_HOME', 40), ('LD_DISC_KNOWN_PCT', 5), ('LD_DISC_HOME_PCT', 10),
  ('LD_PROSP_LOW', 350), ('LD_PROSP_HIGH', 650), ('LD_PROSP_BOARD_DELTA', 1), ('LD_PROSP_BOARD_MIN', 3), ('LD_PROSP_LOW_BONUS_PCT', 10),
  ('CT_BUCKET_S', 1800), ('CT_PER_BUCKET', 4), ('CT_EXPIRE_EXTRA_S', 3600), ('CT_ACCEPT_CD_MS', 20000), ('CT_DONE_CD_MS', 90000),
  ('CT_HOURLY_MAX', 8), ('CT_REACCEPT_LOCK_S', 600), ('CT_PREMIUM_PROREP', 100),
  ('FRT_BASE', 180), ('FRT_PER_M_X100', 14), ('FRT_MIN_DIST', 300), ('FRT_CARGO_TYPES', 3),
  ('FRT_MIN_SECONDS', 25), ('FRT_MIN_MPS', 40), ('FRT_TIME_BASE_S', 120), ('FRT_TIME_MPS_X10', 60),
  ('FRT_INTEG_OK', 70), ('FRT_INTEG_PENALTY_PCT', 25), ('FRT_INTEG_FAIL', 30), ('FRT_ABANDON_REP', -5),
  ('PRO_REP_DELIVERY', 12), ('PRO_REP_ONTIME', 4), ('PRO_REP_FAIL', -15), ('PRO_REP_RIDE', 8), ('PRO_REP_DAY_CAP', 60),
  ('PRO_TIER1', 100), ('PRO_TIER2', 300), ('PRO_TIER3', 600), ('PRO_BONUS1_PCT', 5), ('PRO_BONUS2_PCT', 10), ('PRO_BONUS3_PCT', 15),
  ('RENT_FEE_STREET', 179), ('RENT_DEP_STREET', 0), ('RENT_STREET_MIN', 60), ('RENT_MIN_LEVEL', 1), ('RENT_BLOCK_MIN', 20), ('RENT_BLOCKS_MAX', 3), ('RENT_GRACE_MS', 300000), ('RENT_HEALTH_ZERO', 300), ('RENT_HEALTH_FULL', 1000),
  ('RENT_FEE_FAGGIO', 30), ('RENT_FEE_PREVION', 60), ('RENT_FEE_MANANA', 70), ('RENT_DEP_FAGGIO', 200), ('RENT_DEP_PREVION', 400), ('RENT_DEP_MANANA', 450),
  ('TAXI_MIN_LEVEL', 2), ('TAXI_DEP', 300), ('TAXI_RENT_HOUR', 60), ('TAXI_BASE', 15), ('TAXI_PER_M_X100', 12), ('TAXI_TAX_PCT', 10),
  ('TAXI_NPC_HOURLY', 10), ('TAXI_PAIR_HOURLY', 3), ('TAXI_RATING_WINDOW', 20),
  ('PRJ_GOAL', 6000), ('PRJ_DAYS', 7), ('PRJ_FUND_PCT', 8), ('PRJ_FUND_RIDE_X10', 15), ('PRJ_DON_MIN', 50), ('PRJ_DON_MAX', 500), ('PRJ_DON_DAY', 1000),
  ('PRJ_DON_REP_PER', 100), ('PRJ_DON_MIN_PLAYED_MIN', 60), ('PRJ_DAY_CAP', 15), ('PRJ_REP_DONE', 5), ('PRJ_REP_MIN_POINTS', 3),
  ('PRJ_ACT_BONUS', 100), ('PRJ_FEST_H', 72), ('PRJ_FEST_ACT_MIN', 600),
  ('ONB_STEPS', 5), ('ONB_REWARD', 120), ('ONB_EXP', 1);

-- Quartieri: valori iniziali = base di ritorno (GP_LD_D<d>_*).
INSERT IGNORE INTO `ld_districts` (`id`, `code`, `name`, `activity`, `security`, `crime`, `base_activity`, `base_crime`) VALUES
  (1, 'idlewood',  'Idlewood',  ld_cfg('LD_D1_ACT_BASE'), ld_cfg('LD_D1_SEC_BASE'), ld_cfg('LD_D1_CRIME_BASE'), ld_cfg('LD_D1_ACT_BASE'), ld_cfg('LD_D1_CRIME_BASE')),
  (2, 'ganton',    'Ganton',    ld_cfg('LD_D2_ACT_BASE'), ld_cfg('LD_D2_SEC_BASE'), ld_cfg('LD_D2_CRIME_BASE'), ld_cfg('LD_D2_ACT_BASE'), ld_cfg('LD_D2_CRIME_BASE')),
  (3, 'jefferson', 'Jefferson', ld_cfg('LD_D3_ACT_BASE'), ld_cfg('LD_D3_SEC_BASE'), ld_cfg('LD_D3_CRIME_BASE'), ld_cfg('LD_D3_ACT_BASE'), ld_cfg('LD_D3_CRIME_BASE'));
-- Catalogo progetti comunitari (testi originali, tono "comunita' e seconda occasione").
INSERT IGNORE INTO `ld_project_catalog` (`district_id`, `seq`, `name`) VALUES
  (1, 1, 'Riparazione del campetto di Idlewood'), (1, 2, 'Murale della Casa del Quartiere'), (1, 3, 'Lampioni nuovi su Idlewood Way'),
  (2, 1, 'Palestra popolare di Ganton'), (2, 2, 'Orto comunitario di Ganton'), (2, 3, 'Doposcuola nel garage di quartiere'),
  (3, 1, 'Biblioteca di strada di Jefferson'), (3, 2, 'Pulizia del canale di Jefferson'), (3, 3, 'Panchine e ombra in piazza');
-- Un progetto attivo per quartiere (nessun effetto se c'e' gia').
CALL sp_ld_seed();

INSERT IGNORE INTO schema_migrations (name) VALUES ('schema_full_v8'), ('012_account_character_identity');
