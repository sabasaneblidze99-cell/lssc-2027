#!/usr/bin/env python3
"""Simulazione della progressione economica di LS Crime's con i valori REALI di econ_config.inc.
Uso: python3 tests/economy/simulate.py [--md]   (--md: tabelle markdown per il piano)
Le ipotesi (ore al giorno, quota di tempo per attivita', consumi) sono dichiarate qui sotto: e' una stima, non un benchmark."""
import os, re, sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
src = ''
for f in ('currency.inc', 'econ_config.inc'):
    src += open(os.path.join(ROOT, 'gamemodes', 'modules', 'economy', f), encoding='utf-8').read()
D = {m.group(1): m.group(2).strip() for m in re.finditer(r'^#define\s+(ECON_\w+)\s+\(([^)]*)\)', src, re.M)}
def v(n):
    x = D[n]
    return int(x) if re.fullmatch(r'-?\d+', x) else v(x)
G = lambda n: v(n) / 100.0   # in lari

# ---------------------------------------------------------------- redditi orari (lari/h di gioco attivo)
def paycheck(level):
    return min(G('ECON_PAYCHECK_BASE') + level * G('ECON_PAYCHECK_PER_LEVEL'), G('ECON_PAYCHECK_MAX')) * 4
JOB_MIN = {'bus': 7, 'courier': 9, 'cleaner': 8}                     # minuti medi a giro (job_ui)
def job_hourly(job):
    step, bonus, n = {'bus': ('BUS', 6), 'courier': ('COURIER', 5), 'cleaner': ('CLEANER', 8)}[job][0], None, None
    k = {'bus': ('BUS', 6), 'courier': ('COURIER', 5), 'cleaner': ('CLEANER', 8)}[job]
    run = G('ECON_JOB_%s_STEP' % k[0]) * k[1] + G('ECON_JOB_%s_BONUS' % k[0])
    return min(run * 60 / JOB_MIN[job], G('ECON_JOB_HOURLY_CAP'))
def freight_hourly(per_hour=6, dist=1500, mult=1.05):
    r = (G('ECON_LD_FRT_BASE') + dist * v('ECON_LD_FRT_PER_M_X100') / 100 / 100) * mult
    return min(r * per_hour, G('ECON_LD_LD_WORK_HOURLY_CAP'))
def taxi_hourly(npc=10, players=2, dist=1500):
    fare = G('ECON_LD_TAXI_BASE') + dist * v('ECON_LD_TAXI_PER_M_X100') / 100 / 100
    return (npc * fare * 1.05 + players * fare * 0.9) - G('ECON_LD_TAXI_RENT_HOUR')   # NPC: +mancia; giocatori: -10% tassa
def faction_hourly(rank):
    return G('ECON_FAC_PAY_R%d' % rank) * 4

# ---------------------------------------------------------------- costi orari
FUEL_L_PER_H = 21.6 * 0.5      # vehicle.inc: 6 cl/10 s in movimento = 21,6 L/h; si guida meta' del tempo
def living_cost(has_car, house_tier=None):
    food = G('ECON_ITEM_FOOD') * 1.5 + G('ECON_ITEM_WATER') * 1.5   # ~3 consumazioni l'ora
    fuel = FUEL_L_PER_H * G('ECON_FUEL_PER_LITER') if has_car else 0
    bills = 0
    if house_tier is not None:   # un ciclo di bollette ogni ~3 ore di consumo
        bills = (G('ECON_BILL_LUCE_T%d' % house_tier) + G('ECON_BILL_GAS_T%d' % house_tier)) / 3
    return food + fuel + bills

PROFILES = [
    # nome, ore/giorno, reddito orario (funzione del giorno), auto?, casa?
    ('Nuovo giocatore', 1.0, lambda d: 0.7 * job_hourly('cleaner') + paycheck(2), False, None),
    ('Occasionale', 1.5, lambda d: 0.7 * job_hourly('courier') + paycheck(4), False, None),
    ('Attivo', 3.0, lambda d: 0.75 * freight_hourly() + paycheck(6), True, None),
    ('Lavoratore specializzato', 3.0, lambda d: 0.8 * freight_hourly(per_hour=7, mult=1.15) + paycheck(10), True, 1),
    ('Fazione (LSPD/EMS)', 2.0, lambda d: 0.85 * faction_hourly(min(1 + d // 20, 6)) + paycheck(5), True, 0),
    ('Giocatore ricco', 4.0, lambda d: 0.8 * max(freight_hourly(per_hour=8, mult=1.2), faction_hourly(6)) + paycheck(15), True, 2),
]
GOALS = [('Scooter (Faggio)', G('ECON_VEH_FAGGIO')), ('Prima auto (Manana)', G('ECON_VEH_MANANA')),
         ('Berlina (Primo)', G('ECON_VEH_PRIMO')), ('Casa economica (₾58.000)', 58000.0),
         ('Casa media (₾98.000)', 98000.0), ('Business (stima ₾150.000)', 150000.0), ('Berlina premium (Sentinel)', G('ECON_VEH_SENTINEL'))]

def simulate(prof, days):
    name, hpd, inc, car, tier = prof
    bal = G('ECON_START_TOTAL'); hours = 0.0; reached = {}
    for d in range(days):
        h_inc = inc(d)
        h_cost = living_cost(car and bal > G('ECON_VEH_MANANA') * 0, tier)
        bal += (h_inc - h_cost) * hpd
        hours += hpd
        for g, price in GOALS:
            if g not in reached and bal >= price:
                reached[g] = (d + 1, hours)
    return bal, reached

def fmt(x):
    return '₾{:,.0f}'.format(x).replace(',', '.')

md = '--md' in sys.argv
print('## Redditi orari di riferimento (lari/h di gioco)')
for lab, val in [('Reddito di base (liv. 1 / 10)', '%s / %s' % (fmt(paycheck(1)), fmt(paycheck(10)))),
                 ('Netturbino / autista bus / corriere', '%s / %s / %s' % (fmt(job_hourly('cleaner')), fmt(job_hourly('bus')), fmt(job_hourly('courier')))),
                 ('Consegne camion (6/h, 1,5 km)', fmt(freight_hourly())), ('Taxi (10 NPC + 2 giocatori/h)', fmt(taxi_hourly())),
                 ('Fazione grado 1 / 6', '%s / %s' % (fmt(faction_hourly(1)), fmt(faction_hourly(6)))),
                 ('Costo vita/h senza auto / con auto', '%s / %s' % (fmt(living_cost(False)), fmt(living_cost(True))))]:
    print('- %s: %s' % (lab, val))
print()
print('## Simulazione 30 / 90 / 180 giorni')
print('| Profilo | Ore/giorno | Netto/h (g. 1) | Netto settimana | Saldo 30 g | Saldo 90 g | Saldo 180 g |')
print('|---|---|---|---|---|---|---|')
for p in PROFILES:
    net_h = p[2](0) - living_cost(p[3], p[4])
    out = [simulate(p, d)[0] for d in (30, 90, 180)]
    print('| %s | %.1f | %s | %s | %s | %s | %s |' % (p[0], p[1], fmt(net_h), fmt(net_h * p[1] * 7), *[fmt(x) for x in out]))
print()
print('## Quando si raggiunge un obiettivo (giorno / ore di gioco, risparmiando tutto)')
print('| Profilo | ' + ' | '.join(g for g, _ in GOALS) + ' |')
print('|---|' + '---|' * len(GOALS))
for p in PROFILES:
    _, r = simulate(p, 365)
    print('| %s | ' % p[0] + ' | '.join(('g. %d (%.0f h)' % r[g]) if g in r else 'oltre 1 anno' for g, _ in GOALS) + ' |')
