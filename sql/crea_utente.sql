-- LSSC 2027 - utente MySQL del server di gioco (da eseguire UNA volta, come root, DOPO lsrp_completo.sql).
-- 1) Sostituisci CAMBIA_QUESTA_PASSWORD con una password tua (lunga, senza apostrofi). Non committarla mai in git.
-- 2) Metti la stessa password in mysql.ini (accanto a omp-server).
-- 3) Esegui SOLO il blocco del tuo database e lascia l'altro commentato.

-- ===================== MySQL 8.0 / 8.4 =====================
-- Il plugin MySQL R41-4 si collega con mysql_native_password.
-- MySQL 8.4: aggiungi prima in my.ini, sezione [mysqld], la riga   mysql_native_password=ON   e riavvia MySQL.
CREATE USER IF NOT EXISTS 'lsrp'@'localhost' IDENTIFIED WITH mysql_native_password BY 'CAMBIA_QUESTA_PASSWORD';
CREATE USER IF NOT EXISTS 'lsrp'@'127.0.0.1' IDENTIFIED WITH mysql_native_password BY 'CAMBIA_QUESTA_PASSWORD';

-- ===================== MariaDB 10.4+ (XAMPP) =====================
-- (togli i due trattini davanti alle due righe qui sotto e metti i due trattini davanti alle due righe MySQL qui sopra)
-- CREATE USER IF NOT EXISTS 'lsrp'@'localhost' IDENTIFIED BY 'CAMBIA_QUESTA_PASSWORD';
-- CREATE USER IF NOT EXISTS 'lsrp'@'127.0.0.1' IDENTIFIED BY 'CAMBIA_QUESTA_PASSWORD';

-- Permessi solo sul database del gioco (niente privilegi globali).
GRANT SELECT, INSERT, UPDATE, DELETE, EXECUTE, CREATE TEMPORARY TABLES, LOCK TABLES ON lsrp.* TO 'lsrp'@'localhost';
GRANT SELECT, INSERT, UPDATE, DELETE, EXECUTE, CREATE TEMPORARY TABLES, LOCK TABLES ON lsrp.* TO 'lsrp'@'127.0.0.1';
FLUSH PRIVILEGES;
