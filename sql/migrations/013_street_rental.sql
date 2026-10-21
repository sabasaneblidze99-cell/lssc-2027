-- 013_street_rental: auto a noleggio parcheggiate in citta' (vkind 5, "noleggio di strada").
-- Prezzo fisso $179 per 60 minuti, nessuna cauzione, nessuno sconto di quartiere. Additiva: nessun dato cancellato.
-- Importare in phpMyAdmin DENTRO il database lsrp (MySQL 8 o MariaDB).
INSERT IGNORE INTO `ld_config` (`k`, `v`) VALUES ('RENT_FEE_STREET', 179), ('RENT_DEP_STREET', 0), ('RENT_STREET_MIN', 60);

DELIMITER //
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
DELIMITER ;
INSERT IGNORE INTO schema_migrations (name) VALUES ('013_street_rental');
