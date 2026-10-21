-- 018_rollback.sql - GENERATO da scripts/gen_econ_sql.py. Rimette saldi e prezzi pagati com'erano prima di 018.
START TRANSACTION;
UPDATE characters c JOIN econ_backup_018_characters b ON b.id = c.id SET c.money = b.money, c.bank = b.bank;
UPDATE houses h JOIN econ_backup_018_houses b ON b.id = h.id SET h.price = IF(h.owner_id IS NOT NULL, b.price, h.price), h.price_paid = b.price_paid;
UPDATE vehicles v JOIN econ_backup_018_vehicles b ON b.id = v.id SET v.price_paid = b.price_paid;
UPDATE economy_meta SET v = 'non convertiti' WHERE k = 'balances';
COMMIT;
