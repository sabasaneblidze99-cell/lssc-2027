-- 014_spawn_select.sql - scelta dello spawn: obbligo di ultima posizione dopo un'uscita in combattimento.
-- Idempotente (MySQL 8 / MariaDB 10.3+). Senza questa colonna il gamemode funziona lo stesso, ma senza l'obbligo.
SET @have := (SELECT COUNT(*) FROM information_schema.COLUMNS
              WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'characters' AND COLUMN_NAME = 'combat_logout');
SET @sql := IF(@have = 0,
  'ALTER TABLE characters ADD COLUMN combat_logout TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER last_save',
  'SELECT ''combat_logout gia presente''');
PREPARE st FROM @sql; EXECUTE st; DEALLOCATE PREPARE st;
