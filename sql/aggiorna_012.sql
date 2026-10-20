-- LSSC 2027 v8: aggiornamento del database da v6/v7 (MySQL 8). Importare in phpMyAdmin DENTRO il database lsrp (selezionalo a sinistra, poi Importa).
-- Aggiunge email facoltativa, blocco dopo password errate, identita' del personaggio e numero di telefono. Non cancella nulla.
-- 012_account_character_identity: Chapter Zero M14 (docs/DATABASE_CHARACTER_PLAN.md). Additiva e idempotente (MariaDB: IF NOT EXISTS;
-- per MySQL 8 si usa la variante generata nel pacchetto Windows, senza IF NOT EXISTS). Nessun DROP, nessun dato cancellato.
--
-- ACCOUNT: email FACOLTATIVA e MAI considerata verificata finche' non esiste un vero invio del codice (email_verified resta 0);
--          blocco temporaneo dopo troppi accessi falliti (failed_logins conta i tentativi errati consecutivi fra sessioni,
--          locked_until = UNIX ts fino a cui il login e' rifiutato); cooldown del cambio personaggio; lingua.
-- PERSONAGGI: identita' (nome/cognome RP latini, char_name "Nome_Cognome" unico), data di nascita RP, genere, archetipo
--          (0 legacy, 1 outlaw, 2 citizen, 3 recruit), flag di background e di stato, numero di telefono in gioco (unico).
-- Backfill: i personaggi esistenti (slot 0) prendono nome e cognome dal nome dell'account se e' nel formato Nome_Cognome;
--          numero di telefono 5550000 + id per i primi 9999 personaggi.
ALTER TABLE `accounts` ADD COLUMN `email` VARCHAR(254) NULL DEFAULT NULL AFTER `intro_seen`;
ALTER TABLE `accounts` ADD COLUMN `email_verified` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `email`;
ALTER TABLE `accounts` ADD COLUMN `email_token_hash` CHAR(64) NULL DEFAULT NULL AFTER `email_verified`;
ALTER TABLE `accounts` ADD COLUMN `password_changed_at` TIMESTAMP NULL DEFAULT NULL AFTER `email_token_hash`;
ALTER TABLE `accounts` ADD COLUMN `failed_logins` SMALLINT UNSIGNED NOT NULL DEFAULT 0 AFTER `password_changed_at`;
ALTER TABLE `accounts` ADD COLUMN `locked_until` INT UNSIGNED NOT NULL DEFAULT 0 AFTER `failed_logins`;
ALTER TABLE `accounts` ADD COLUMN `last_char_switch` INT UNSIGNED NOT NULL DEFAULT 0 AFTER `locked_until`;
ALTER TABLE `accounts` ADD COLUMN `lang` CHAR(5) NOT NULL DEFAULT 'ka' AFTER `last_char_switch`;
ALTER TABLE `accounts` ADD UNIQUE KEY `uq_accounts_email` (`email`);

ALTER TABLE `characters` ADD COLUMN `char_name` VARCHAR(24) NULL DEFAULT NULL AFTER `inv_bag`;
ALTER TABLE `characters` ADD COLUMN `first_name` VARCHAR(20) NULL DEFAULT NULL AFTER `char_name`;
ALTER TABLE `characters` ADD COLUMN `last_name` VARCHAR(20) NULL DEFAULT NULL AFTER `first_name`;
ALTER TABLE `characters` ADD COLUMN `display_name_ka` VARCHAR(64) NULL DEFAULT NULL AFTER `last_name`;
ALTER TABLE `characters` ADD COLUMN `birth_date` DATE NULL DEFAULT NULL AFTER `display_name_ka`;
ALTER TABLE `characters` ADD COLUMN `gender` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `birth_date`;
ALTER TABLE `characters` ADD COLUMN `archetype` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `gender`;
ALTER TABLE `characters` ADD COLUMN `background_flags` INT UNSIGNED NOT NULL DEFAULT 0 AFTER `archetype`;
ALTER TABLE `characters` ADD COLUMN `phone_number` INT UNSIGNED NULL DEFAULT NULL AFTER `background_flags`;
ALTER TABLE `characters` ADD COLUMN `status_flags` INT UNSIGNED NOT NULL DEFAULT 0 AFTER `phone_number`;
ALTER TABLE `characters` ADD COLUMN `last_played_at` TIMESTAMP NULL DEFAULT NULL AFTER `status_flags`;
ALTER TABLE `characters` ADD UNIQUE KEY `uq_characters_name` (`char_name`);
ALTER TABLE `characters` ADD UNIQUE KEY `uq_characters_phone` (`phone_number`);

UPDATE `characters` c JOIN `accounts` a ON a.`id` = c.`account_id`
   SET c.`char_name` = a.`name`,
       c.`first_name` = SUBSTRING_INDEX(a.`name`, '_', 1),
       c.`last_name` = SUBSTRING_INDEX(a.`name`, '_', -1)
 WHERE c.`char_name` IS NULL AND c.`slot` = 0
   AND a.`name` REGEXP '^[A-Z][a-z]{1,18}_[A-Z][a-z]{1,18}$';
UPDATE `characters` SET `phone_number` = 5550000 + `id` WHERE `phone_number` IS NULL AND `id` < 10000;
