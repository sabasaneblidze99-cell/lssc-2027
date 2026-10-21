# Georgia: economic reference data for game-economy design (research date 21 Oct 2026)

Conventions
- GEL = Georgian lari. USD conversions use the NBG official rate of 20 Oct 2026 (1 USD = 2.5979 GEL) unless stated.
- Reliability tags: **OFFICIAL** (Geostat, NBG, Treasury, regulator, matsne, state company), **CROWDSOURCED** (Numbeo, Expatistan), **MARKET LISTINGS** (fuel boards, car listings, dealer or portal price lists), **ESTIMATE** (journalists, blogs, consultancies, or my own unit conversions).
- Numbeo pages render market items **per 1 lb** (453.6 g) and property **per sq ft**. Where I give per-kg / per-500 g / per-m² values I converted them myself (×2.2046 for kg, ×1.1023 for 500 g, ×10.764 for m²). These are marked "conv."
- Nothing in this file is invented. Where no 2026 value was found, the latest value is given and dated.

---

## 0. User-supplied baselines: verification

| Baseline | Claimed | Found | Verdict | Source |
|---|---|---|---|---|
| Avg gross monthly salary Q2 2026 | ~2,389 GEL | **2,389.3 GEL** (Geostat, released 15 Sep 2026) | **CONFIRMED** | https://www.geostat.ge/en/modules/categories/39/wages ; https://georgiatoday.ge/average-monthly-salary-in-georgia-rises-8-to-gel-2389-3-geostat/ |
| Gulf regular petrol 8 Oct 2026 | 4.03 GEL/L | **4.03 GEL/L** (G-Force Euro Regular, 2026-10-08, gulf.ge history; a second, 10-tetri-lower price column of 3.93 is also shown and not labelled) | **CONFIRMED** (now 4.09 on 19 Oct) | https://gulf.ge/en/fuel_prices |
| Bread 500 g Tbilisi (Numbeo) | ~1.88 GEL | Numbeo Tbilisi shows **1.80 GEL per 1 lb loaf** (20 Oct 2026), which converts to **≈1.98 GEL per 500 g** | **DIFFERENT** (slightly higher than 1.88; 1.80 is the per-lb figure) | https://www.numbeo.com/cost-of-living/in/Tbilisi |
| Inexpensive restaurant meal Tbilisi (Numbeo) | ~30 GEL | **30.00 GEL** (range 20–50), 20 Oct 2026 | **CONFIRMED** | https://www.numbeo.com/cost-of-living/in/Tbilisi |

---

## 1. Wages and minimum wage

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Average monthly nominal gross earnings, all employees | 2,389.3 | GEL/month | Q2 2026 (rel. 15 Sep 2026) | National | https://www.geostat.ge/en/modules/categories/39/wages | OFFICIAL | +8.0% y/y (+177.3 GEL). Q1 2026: 2,363.8 |
| Men / women | 2,856.4 / 1,927.0 | GEL/month | Q2 2026 | National | https://georgiatoday.ge/average-monthly-salary-in-georgia-rises-8-to-gel-2389-3-geostat/ | OFFICIAL (Geostat via press) | Gap 929.4 GEL |
| Business sector average | 2,471.5 | GEL/month | Q2 2026 | National | https://www.trend.az/scaucasus/georgia/4221851.html | OFFICIAL (Geostat via press) | Large firms 2,622.8; medium 2,918.9; small 1,972.3 |
| Non-business (incl. public) sector average | 2,239.0 | GEL/month | Q2 2026 | National | https://georgiatoday.ge/average-monthly-salary-in-georgia-rises-8-to-gel-2389-3-geostat/ | OFFICIAL (via press) | +10.6% y/y |
| Information & communication | 4,400.4 | GEL/month | Q2 2026 | National | https://www.alia.ge/ramdeni-lari-sheadgina-sashualo-thviurma-khelphasma-saqstati-ganakhlebul-monatsemebs-aqveqhnebs/ | OFFICIAL (via press) | Highest sector |
| Financial & insurance | 4,074.1 | GEL/month | Q2 2026 | National | same | OFFICIAL (via press) | Q1 2026 was 5,952.5 (bonus quarter) |
| Construction | 3,496.6 | GEL/month | Q2 2026 | National | same | OFFICIAL (via press) | +0.8% y/y |
| Professional, scientific, technical | 3,304.8 | GEL/month | Q2 2026 | National | same | OFFICIAL (via press) | |
| Mining & quarrying | 3,230.4 | GEL/month | **Q1 2026** | National | https://geostat.ge/media/80106/Average-Monthly-Nominal-Earnings-of-Employees---%28I-Quarter%2C-2026%29.pdf | OFFICIAL | Q2 2026 value not published in text |
| Lowest sector (Q2 2026) | ~1,379 | GEL/month | Q2 2026 | National | https://trend.az/scaucasus/georgia/4226384.html | ESTIMATE (Trend calc. from Geostat) | "Other service activities"; top sector earns 3.2× lowest |
| Sector table, business sector (latest full table found) | Agriculture 1,478.3; Industry 2,020.5; Construction 2,912.6; Trade 1,832.1; **Transport & storage 2,322.3**; **Accommodation & food 1,574.6**; ICT 4,255.4; Real estate 1,956.8; Prof./sci. 3,402.5; Admin/support 1,598.6; **Education 1,732.6**; **Health & social 1,775.6**; Arts/recreation 3,027.6; Other services 795.3 | GEL/month | **Q1 2025** | National | https://www.geostat.ge/media/72910/QuarterlyBull_2025_II_Eng.pdf | OFFICIAL | Business sector only (excludes public admin). 2026 full sector table exists only as Geostat XLSX (https://geostat.ge/media/82739/04-earnings-quarterly.xlsx), which could not be fetched. Scale ~+8-10% for 2026 = ESTIMATE |
| Regional (business sector) | Tbilisi 2,402.1; Adjara 1,777.3; Imereti 1,315.7; Kakheti 1,475.6; Kvemo Kartli 2,028.3; Shida Kartli 1,231.5; Racha 1,020.8 | GEL/month | Q1 2025 | Regions | https://www.geostat.ge/media/72910/QuarterlyBull_2025_II_Eng.pdf | OFFICIAL | Kutaisi is in Imereti; Batumi in Adjara |
| Numbeo average net salary | 1,720.92 (Tbilisi); 1,529.79 (Batumi); 1,233.11 (Kutaisi); 1,509.07 (national) | GEL/month net | 20 Oct 2026 (Kutaisi 2 Oct) | Cities | https://www.numbeo.com/cost-of-living/in/Tbilisi ; https://www.numbeo.com/cost-of-living/in/Batumi ; https://www.numbeo.com/cost-of-living/in/Kutaisi-Georgia | CROWDSOURCED | Consistent with 2,389 gross × 0.78 (20% PIT + 2% pension) ≈ 1,864 net |
| Minimum wage, private sector | 20 | GEL/month | since 4 Jun 1999, unchanged in 2026 | National | https://wageindicator.org/salary/minimum-wage/georgia | ESTIMATE (secondary compiler) | Symbolic; no effective private minimum wage |
| Public sector basic (minimum) salary | 1,600 (up from 1,460) | GEL/month | 2026 budget | National | https://transparency.ge/en/blog/brief-analysis-draft-state-budget-2026 | ESTIMATE (NGO analysis of budget) | Also cited by WageIndicator as public-sector minimum from 1 Jan 2026. Public pay +≥10% in 2026 |
| State old-age pension | 370 (<70 yrs); 495 (≥70 yrs) | GEL/month | 2026 | National | https://transparency.ge/en/blog/brief-analysis-draft-state-budget-2026 | ESTIMATE (NGO analysis of budget) | |

## 2. Food / grocery prices

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| White bread | 1.80 (range 1.27–3.63) | GEL / 1 lb loaf | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | ≈1.98 GEL / 500 g (conv.). Batumi 1.68/lb, Kutaisi 1.16/lb, national 1.55/lb |
| Milk | 5.67 (4.00–7.00) | GEL / 1 L | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 5.50; Kutaisi 4.62; national 5.31 |
| Eggs | 7.16 (5.40–8.40) | GEL / 12 | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 6.96; Kutaisi 6.05 |
| Chicken fillets | 7.73/lb → **≈17.0/kg** | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED (conv.) | Batumi 7.06/lb (≈15.6/kg); Kutaisi 5.70/lb (≈12.6/kg) |
| Beef round | 15.36/lb → ≈33.9/kg | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED (conv.) | |
| White rice | 2.08/lb → ≈4.59/kg | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED (conv.) | |
| Potatoes | 1.00/lb → ≈2.20/kg | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED (conv.) | Kutaisi 0.87/lb |
| Apples | 1.94/lb → ≈4.28/kg | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED (conv.) | |
| Tomatoes / onions / bananas | 2.89 / 0.83 / 2.13 per lb | GEL/lb | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | |
| Local cheese | 10.86/lb → ≈23.9/kg | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED (conv.) | |
| Bottled water 1.5 L (50 oz) | 1.93 | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | |
| Domestic beer 0.5 L (shop) | 4.34 | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 3.82; Kutaisi 4.17 |
| Wine, mid-range bottle | 20.00 | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 15; Kutaisi 10 |
| Cigarettes (Marlboro, 20) | 9.00 | GEL/pack | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 8.10; Kutaisi 8.00; national 8.50 |
| Imeruli khachapuri (bakery) | 5–8 | GEL | Apr–May 2026 | Tbilisi | https://sakhva-travel.com/en/blog/tbilisi-prices-2026/ | ESTIMATE (travel blog) | |
| Adjaruli khachapuri (restaurant) | 10–14 | GEL | Apr–May 2026 | Tbilisi | same | ESTIMATE | |
| Khinkali | 1–1.5 per piece (2–2.5 tourist places) | GEL | Apr–May 2026 | Tbilisi | same | ESTIMATE | |

## 3. Restaurants

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Meal, inexpensive restaurant | 30.00 (20–50) | GEL | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | Batumi 30.00; Kutaisi 26.00; national 28.00 |
| Meal for 2, mid-range, 3 courses | 100.00 (70–150) | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 84; Kutaisi 80 |
| McDonald's combo | 25.00 (20–25.73) | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 27; Kutaisi 27.50 |
| Cappuccino | 9.08 (5–11.95) | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 9.18; Kutaisi 5.29 |
| Draft beer 0.5 L (pint) in bar | 7.00 | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Batumi 4.50; Kutaisi 3.25 |
| Soft drink 0.33 L | 2.73 | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | |
| Glass of wine, restaurant | 6–12 | GEL | Apr–May 2026 | Tbilisi | https://sakhva-travel.com/en/blog/tbilisi-prices-2026/ | ESTIMATE | |

## 4. Transport and fuel

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Metro / bus / minibus single ride (90 min transfer) | 1.00 | GEL | system since Feb 2022; still 1.00 in 2026 | Tbilisi | https://oldwp.civil.ge/archives/463605 ; https://www.numbeo.com/cost-of-living/in/Tbilisi | OFFICIAL (city announcement via press) + CROWDSOURCED | Metrocard deposit 5 GEL (sakhva-travel 2026) |
| Day / week pass | 3 / 20 | GEL | from Feb 2022 | Tbilisi | https://oldwp.civil.ge/archives/463605 | OFFICIAL (via press) | 2026 level not re-verified on ttc.ge |
| Monthly / 3-month / 6-month / annual pass | 40 / 100 / 150 / 250 | GEL | from Feb 2022 | Tbilisi | same | OFFICIAL (via press) | Numbeo Oct 2026 also shows monthly 40.00 |
| Local transport single ticket | 0.70 (Batumi); 0.90 (Kutaisi) | GEL | Oct 2026 | Batumi / Kutaisi | https://www.numbeo.com/cost-of-living/in/Batumi ; https://www.numbeo.com/cost-of-living/in/Kutaisi-Georgia | CROWDSOURCED | Monthly pass Batumi 30, Kutaisi 20 |
| Taxi start (standard tariff) | 3.00 (2.20–4.00) | GEL | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | Kutaisi 2.00 |
| Taxi per km | 3.22/mile → **≈2.00/km** | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED (conv.) | Batumi 2.74/mile ≈1.70/km |
| Taxi 1 h waiting | 30.30 | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | |
| Bolt typical trips | 2–4 km: 4–7; 6–10 km: 8–15; airport→centre (17 km): 15–25 | GEL | Jul–Aug 2026 (before +12%) | Tbilisi | https://sakhva-travel.com/en/blog/tbilisi-taxi-guide/ | ESTIMATE (travel blog) | Surge 1.5–2× in rain/rush hour |
| Bolt fare increase | +12% all categories (not minimum fares) | % | 18 Sep 2026 | National | https://georgiatoday.ge/?p=112380 | ESTIMATE (press) | Base/per-km tariff not published in sources found |
| **Gulf G-Force Euro Regular** | **4.09** (8 Oct: 4.03; 2 Oct: 3.97) | GEL/L | 19 Oct 2026 | National | https://gulf.ge/en/fuel_prices | MARKET LISTINGS (retailer) | Second column on page 0.10 lower (3.99), unlabelled (likely loyalty/card price) |
| Gulf G-Force Premium | 4.39 | GEL/L | 19 Oct 2026 | National | same | MARKET LISTINGS | 8 Oct: 4.33 |
| Gulf G-Force Super | 4.69 | GEL/L | 19 Oct 2026 | National | same | MARKET LISTINGS | 8 Oct: 4.65 |
| Gulf Euro Diesel / G-Force Euro Diesel | 4.99 / 5.05 | GEL/L | 19 Oct 2026 | National | same | MARKET LISTINGS | |
| Wissol regular / premium / super / diesel | 4.08 / 4.32 / 4.64 / 4.98 | GEL/L | 20 Oct 2026 | National | https://amindi.org/en/fuel-prices | MARKET LISTINGS (aggregator) | |
| Rompetrol regular / premium / super / diesel | 4.02 / 4.32 / 4.65 / 4.92 | GEL/L | 20 Oct 2026 | National | same | MARKET LISTINGS | |
| SOCAR regular / premium / super / diesel | 4.02 / 4.31 / 4.62 / 4.90 | GEL/L | 20 Oct 2026 | National | same | MARKET LISTINGS | |
| Lukoil regular / premium / super / diesel | 3.95 / 4.25 / 4.60 / 4.97 | GEL/L | 20 Oct 2026 | National | same | MARKET LISTINGS | |
| Portal regular / premium / super / diesel | 3.94 / 4.19 / 4.39 / 4.94 | GEL/L | 20 Oct 2026 | National | same | MARKET LISTINGS | Cheapest chain |
| National average petrol / diesel | 4.285 / 4.960 | GEL/L | 19 Oct 2026 | National | https://dailyfuels.com/georgia/ | ESTIMATE (aggregator) | |
| LPG | 0.753 | GEL/L | 19 Oct 2026 | National | https://dailyfuels.com/georgia/ | ESTIMATE (low confidence) | Looks implausibly low; verify before use |
| CNG | not found | – | – | – | – | – | No 2026 CNG price found in any source |
| Fuel price trend 2026 | Gulf regular 3.27 (24 Mar) → 3.65 (12 Aug) → 4.03 (8 Oct) → 4.09 (19 Oct) | GEL/L | 2026 | National | https://georgiatoday.ge/fuel-prices-rise-across-georgia-amid-global-oil-market-pressure/ ; https://georgiatoday.ge/global-fuel-prices-rise-what-a-liter-of-petrol-costs-in-georgia-today/ | MARKET LISTINGS (via press) | +25% since March |

## 5. Rent (monthly)

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| 1-bedroom, city centre | 1,754.56 (1,297–2,334) | GEL/month | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | ≈675 USD |
| 1-bedroom, outside centre | 1,171.39 (908–1,556) | GEL/month | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | |
| 3-bedroom, centre / outside | 3,365.45 / 2,272.64 | GEL/month | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | |
| 1-bedroom centre / outside | 1,310.28 / 956.34 | GEL/month | 20 Oct 2026 | Batumi | https://www.numbeo.com/cost-of-living/in/Batumi | CROWDSOURCED | 111 contributors |
| 3-bedroom centre / outside | 2,540.72 / 1,786.10 | GEL/month | 20 Oct 2026 | Batumi | same | CROWDSOURCED | |
| 1-bedroom centre / outside | 921.00 / 682.59 | GEL/month | 2 Oct 2026 | Kutaisi | https://www.numbeo.com/cost-of-living/in/Kutaisi-Georgia | CROWDSOURCED | Only 14 contributors |
| 3-bedroom centre / outside | 1,344.90 / 876.00 | GEL/month | 2 Oct 2026 | Kutaisi | same | CROWDSOURCED | |
| Kutaisi rent by rooms (median) | 1-room 229; 2-room 334; 3-room 440 | USD/month | 31 Aug 2026 | Kutaisi | https://georgia-aparts.com/blog/cheapest-areas-kutaisi-2026-08-31 | MARKET LISTINGS (ss.ge/myhome.ge sample) | Averages 311 / 377 / 455 USD |
| Tbilisi asking rent per m² | Old Tbilisi 11.1; Vake–Saburtalo 10.5; Didube–Chughureti 9.5; Gldani–Nadzaladevi 8.8; Isani–Samgori 8.8 | USD/m²/month | Q3 2026 (1 Sep) | Tbilisi | https://tbilisihome.ge/market-report | MARKET LISTINGS (219k listings) | 50 m² flat in Vake ≈ 525 USD |
| Batumi rent per m² | Old Batumi 10.9; New Boulevard 10.3; inner 9.3; periphery 9.5 | USD/m²/month | 2025 | Batumi | https://ramad.bog.ge/galt/yepiN99l-Regional-Residential-Real-Estate_ENG_2025.pdf | ESTIMATE (Galt & Taggart research) | Short-term avg 35.6 USD/day; yield 7.4% |
| National Numbeo 1-bed centre / outside | 1,353.93 / 940.64 | GEL/month | 20 Oct 2026 | National | https://www.numbeo.com/cost-of-living/country_result.jsp?country=Georgia | CROWDSOURCED | |

## 6. Property purchase prices

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Tbilisi RPPI | +0.9% q/q, +4.9% y/y (apartments +4.8%, houses +5.5%) | index | Q2 2026 (rel. 23 Jul 2026) | Tbilisi | https://www.businessinsider.ge/Economic/tbilisi-residential-property-prices-rise-4-9-year-on-tear-in-q2-2026-geostat-39357?lng=eng | OFFICIAL (Geostat via press) | New-build market only |
| New-build apartments by district | Mtatsminda 6,730; Vake 5,914; Krtsanisi 4,630; Saburtalo 4,307; Didube 4,184; Isani 4,088; Chughureti 4,064; Nadzaladevi 4,038; Gldani 3,867; Samgori 3,680 | GEL/m² | Q2 2026 | Tbilisi | same | OFFICIAL (Geostat via press) | ≈1,417–2,590 USD/m² |
| Private houses by district | Vake 4,656; Mtatsminda 4,409; Chughureti 3,524; Saburtalo 3,510; Didube 3,477; Isani 3,268; Samgori 2,992; Krtsanisi 2,926; Gldani 2,811; Nadzaladevi 2,778 | GEL/m² | Q2 2026 | Tbilisi | same | OFFICIAL (Geostat via press) | 150 m² house in Gldani ≈ 420k GEL (ESTIMATE, my calc) |
| Asking price, new renovation / older renovation / white-black frame | Old Tbilisi 1,990 / 1,780 / 1,550; Vake–Saburtalo 1,890 / 1,600 / 1,490; Didube–Chughureti 1,730 / 1,420 / 1,400; Gldani–Nadzaladevi 1,560 / 1,200 / 1,230; Isani–Samgori 1,480 / 1,210 / 1,080 | USD/m² | Q3 2026 (1 Sep) | Tbilisi | https://tbilisihome.ge/market-report | MARKET LISTINGS | Asking, not transaction prices. "Older renovation" = old-build proxy |
| City average (Galt & Taggart) | ~1,373 | USD/m² | 2025 | Tbilisi | https://bm.ge/en/news/in-which-districts-of-tbilisi-was-the-square-meter-most-expensive | ESTIMATE (consultancy) | Secondary market +7.9% y/y |
| Numbeo buy price, centre / outside | 579.43 / 337.55 per sq ft → **≈6,237 / 3,633 GEL/m²** | GEL | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED (conv.) | |
| Batumi primary (turnkey) average | 1,865 (2024: 1,699) | USD/m² | 2025 | Batumi | https://www.investor.ge/2026/04/20/sector-snapshot-batumi-residential-real-estate/ | ESTIMATE (Galt & Taggart) | Old Batumi 3,028; New Boulevard 1,761; periphery 1,487. Forecast +4–6% in 2026 |
| Numbeo buy price, centre / outside | 417.08 / 303.81 per sq ft → **≈4,489 / 3,270 GEL/m²** | GEL | 20 Oct 2026 | Batumi | https://www.numbeo.com/cost-of-living/in/Batumi | CROWDSOURCED (conv.) | |
| Kutaisi centre | 550–950 | USD/m² | Q3 2026 (15 Aug) | Kutaisi | https://www.livo-property.com/en/market-data | ESTIMATE (agency, mixed sources) | Entry price from ~35k USD |
| Numbeo buy price, centre / outside | 255.26 / 164.33 per sq ft → **≈2,748 / 1,769 GEL/m²** | GEL | 2 Oct 2026 | Kutaisi | https://www.numbeo.com/cost-of-living/in/Kutaisi-Georgia | CROWDSOURCED (conv.) | Small sample |
| Mortgage rate (20-yr) | 12.62% (Tbilisi); 15.67% (Kutaisi) | %/yr | Oct 2026 | Cities | Numbeo pages above | CROWDSOURCED | |

## 7. Cars and two-wheelers

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Used-car market median asking price | 13,800 | USD | Jul 2026 | National | https://data.autobridge.ge/en/reports/july-listing-ledger | MARKET LISTINGS | Median model year 2020 |
| Toyota Camry (used) median | 13,500 | USD | Jul 2026 | National | same | MARKET LISTINGS | Most-listed model (330) |
| Hyundai Elantra median / BMW X5 median | 11,500 / 26,800 | USD | Jul 2026 | National | same | MARKET LISTINGS | |
| Toyota Prius 2013, 210k km, cleared | 14,504 GEL (~5,500 USD) | GEL | ~Aug 2026 | Tbilisi | https://autobridge.ge/en/listings/toyota-prius-2013-c4d4771c | MARKET LISTINGS | Comparable sold avg 6,190 USD |
| Toyota Prius 2013, 232k km, cleared | 18,892 GEL (~7,200 USD) | GEL | ~Sep 2026 | Tbilisi | https://autobridge.ge/en/listings/toyota-prius-2013-89592115 | MARKET LISTINGS | Flagged "above market" |
| Toyota Prius 2013, 311k km, not cleared | 2,400 USD + 7,024 GEL customs ≈ 5,001 USD total | USD | ~2026 | Tbilisi | https://autobridge.ge/en/listings/toyota-prius-2013-34b428ea | MARKET LISTINGS | Shows new excise weight: customs ≈ car price |
| Mercedes-Benz E 400 2013, 166k km | 27,785 GEL (10,500 USD) | GEL | ~Jul 2026 | Batumi | https://autobridge.ge/en/listings/mercedes-benz-e-400-2013-22e96808 | MARKET LISTINGS | Comparable 2014 E 400s avg 13,500 USD |
| Cheap old car: Opel Astra 2002, 202k km | 3,400 USD (~8,830 GEL) | USD | ~Jul 2026 | Tbilisi | https://autobridge.ge/en/listings/opel-astra-2002-81400fa1 | MARKET LISTINGS | Comparable avg 3,833 USD |
| Cheap old car: Opel Astra 2006, 295k km | 7,792 GEL (~2,900 USD) | GEL | ~Feb 2026 | Tbilisi | https://autobridge.ge/en/listings/opel-astra-2006-f5271bf6 | MARKET LISTINGS | Comparables 1,100–3,800 USD |
| Lada | not found | – | – | – | – | – | No Lada listing data retrieved; myauto.ge not fetchable |
| New Toyota Camry (dealer) | 83,875 (hybrid 90,475) | GEL | undated, live site Oct 2026 | National | https://toyota-tegeta.ge/en | MARKET LISTINGS (dealer price list) | |
| New Toyota RAV4 hybrid (dealer) | 97,680 | GEL | same | National | same | MARKET LISTINGS | |
| New Toyota Corolla (dealer) | 48,950 (hybrid 59,675) | GEL | same | National | same | MARKET LISTINGS | Land Cruiser 300: 143,100; Prado 136,675 |
| New Camry 2026 hybrid, 1,750 km (private resale) | 21,800 USD + 1,775 GEL customs ≈ 60,635 GEL | USD/GEL | ~Aug 2026 | Tbilisi | https://autobridge.ge/en/listings/toyota-camry-2026-2737324a | MARKET LISTINGS | US-import route is far cheaper than dealer |
| Numbeo new VW Golf / Toyota Corolla | 64,064 / 58,193 | GEL | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | |
| Scooter, new (Vespa 150 / Aprilia SR 175) | 6,490–8,490 / 6,990 | GEL | live shop listing (2024–2026 models) | Tbilisi | https://scootscoot.ge/en/blogs/why-you-should-consider-buying-a-scooter-moped-in-tbilisi/ | MARKET LISTINGS | Motorcycle Yamaha XSR 155: 7,290 GEL |

## 8. Services and household costs

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Basic utilities, 85 m² (915 sq ft) flat | 227.87 (133.57–350) | GEL/month | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | Batumi 194.15; Kutaisi 201.09; national 203.85. Heavily seasonal (gas heating in winter) |
| Typical seasonal bills (couple, 60–80 m²) | Summer 61–102; winter gas-heated 123–234; winter electric-heated 161–322 | GEL/month | Feb 2026 | Tbilisi | https://georgiaexpats.com/guides/utilities/ | ESTIMATE (expat guide) | Garbage fee 3–4 GEL/month |
| Mobile plan (calls + 10 GB+) | 34.00 (18–60) | GEL/month | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | Batumi 23.71; Kutaisi 22.50 |
| Home internet | Silknet 45 Mbps 35, 60 Mbps 40, 100 Mbps 75; Magti 50 Mbps 37, 100 Mbps 80 | GEL/month | Sep 2026 | National | https://madloba.info/en/blog/life-in-georgia/home-internet-in-georgia/ | ESTIMATE (guide quoting tariffs) | Numbeo Tbilisi 60 Mbps+: 50.39 |
| Gym membership | 166.82 (85–250) | GEL/month | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | Kutaisi 107.50; national 144.31 |
| Cinema ticket | 18.00 | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | Kutaisi 12.50 |
| Men's haircut | 34 | GEL | 25 Sep 2026 | Tbilisi (expat area) | https://expatistan.com/price/doctor/tbilisi | CROWDSOURCED (small sample) | Local barbers likely cheaper |
| Doctor visit (private, 15 min) | 92 | GEL | 25 Sep 2026 | Tbilisi | https://expatistan.com/price/doctor/tbilisi | CROWDSOURCED (8 data points) | Outpatient services +9.1% m/m in Sep 2026 CPI |
| Private kindergarten | 962.92 | GEL/month | 20 Oct 2026 | Tbilisi | https://www.numbeo.com/cost-of-living/in/Tbilisi | CROWDSOURCED | Kutaisi 337.50 |
| Jeans (Levi's 501) / Nike shoes | 267.50 / 300.77 | GEL | 20 Oct 2026 | Tbilisi | same | CROWDSOURCED | |

## 9. Energy and water tariffs

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Household electricity, 0–101 kWh/month | 20.041 | tetri/kWh | from 1 Apr 2026 | Tbilisi + regions (EP Georgia) | https://georgiatoday.ge/electricity-tariffs-to-increase-in-tbilisi-and-regions-starting-april-1/ | OFFICIAL (GNERC decision via press) | Was 15.041 |
| Household electricity, 101–301 kWh | 24.053 | tetri/kWh | from 1 Apr 2026 | same | same | OFFICIAL (via press) | Was 19.053 |
| Household electricity, >301 kWh | 28.537 | tetri/kWh | from 1 Apr 2026 | same | same | OFFICIAL (via press) | Was 23.537 |
| Natural gas, household, Tbilisi (Tbilisi Energy) | 53.972 | tetri/m³ incl. VAT | set 1 Jun 2021 (last confirmed value) | Tbilisi | https://1tv.ge/lang/en/news/gas-tariff-rises-for-tbilisi-energy-subscribers | OFFICIAL (via press) | No 2026 change found; expat guide (Feb 2026) quotes ~0.555 GEL/m³ (ESTIMATE). te.ge tariff page could not be read |
| Natural gas tariffs, regional distributors (SOCAR Georgia Gas; Sakorggaz) | 13.683 / 32.735 / **48.244** (SOCAR); 13.468 / 33.002 / **48.296** (Sakorggaz) | tetri/m³ | 1 Jan 2026 – 1 Jan 2028 | Regions | https://matsne.gov.ge/ka/document/view/6736115 ; https://matsne.gov.ge/ka/document/view/6736215 | OFFICIAL (GNERC decisions 65 and 66, 29 Dec 2025) | Column headers not extracted; the third figure is probably the end-user tariff (~0.48 GEL/m³). Treat as likely, not certain |
| Water, household metered | 0.500 (supply 0.391 + wastewater 0.110) | GEL/m³ | current (tariff period 2024–2026) | Tbilisi, Mtskheta | https://www.gwp.ge/en/prices | OFFICIAL (utility) | |
| Water, household non-metered | 4.518 | GEL per person/month | current | Tbilisi, Mtskheta | same | OFFICIAL (utility) | |
| Water, non-household | 6.503 (GWP page); 8.807 (1TV, 2024–2026 period) | GEL/m³ | 2024–2026 | Tbilisi | https://www.gwp.ge/en/prices ; https://1tv.ge/lang/en/news/household-water-tariffs-in-tbilisi-mtskheta-rustavi-remain-unchanged-non-household-see-increase/ | OFFICIAL (sources conflict) | 8.807 is the newer regulator figure |

## 10. Taxes

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Personal income tax | 20 (flat) | % | reviewed 2 Sep 2026 | National | https://taxsummaries.pwc.com/Georgia/Individual/Taxes-on-personal-income | OFFICIAL-equivalent (PwC summary) | |
| Small business status (individual entrepreneur) | 1% of turnover ≤ 500,000 GEL/yr; 3% above | % turnover | 2026 | National | same | PwC summary | |
| VAT | 18 | % | 2026 | National | https://taxsummaries.pwc.com/georgia/individual/other-taxes | PwC summary | |
| Pension contributions | Employee 2% + employer 2% + state 2% (state 1% for income 24k–60k GEL/yr; 0% above 60k); self-employed 4% | % of gross wage | 2026 | National | same | PwC summary | No other social-security contributions |
| Property tax, individuals (non-land, incl. cars HS 8703) | Exempt if family income ≤ 40,000 GEL; 0.05–0.2% of market value if 40k–100k; 0.8–1% if ≥ 100k | % per year | 2026 | National | https://ibccs.tax/blog/property-tax-georgia-rates-thresholds-filing/ ; https://taxsummaries.pwc.com/georgia/individual/other-taxes | ESTIMATE (tax firm) + PwC | Tbilisi rates: 0.2% / 0.8%. Cars are taxed under this property tax; no separate annual road tax found |
| Land tax, non-agricultural | 0.24 GEL/m²/yr × municipal coefficient (≤1.5) | GEL/m²/yr | 2026 | National | https://ibccs.tax/blog/property-tax-georgia-rates-thresholds-filing/ | ESTIMATE (tax firm) | Due 15 Nov |
| Car import excise, age 0–6 yrs | 1.5 | GEL per cm³ | from 2 Apr 2026 | National | https://jam-news.net/car-import-clearance-costs-to-rise-in-georgia/ | OFFICIAL (law N1477 via press) | 60% reduction for hybrids < 6 yrs was proposed (Georgia Today 17 Mar 2026); not confirmed in the final law |
| Car import excise, age > 6 yrs | 4.5 (was ~0.8) | GEL per cm³ | from 2 Apr 2026 | National | same; https://www.wcshipping.com/blog/georgias-6-year-car-import-rule-what-changed-in-poti-2026-update | OFFICIAL (via press) | 1.5 L = 6,750; 2.0 L = 9,000; 3.0 L = 13,500 GEL. RHD ×3; EV exempt. 18% VAT on import. A 5% customs duty is claimed by wcshipping only (unverified) |

## 11. Agricultural / farm-gate prices

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| Grapes, Saperavi, state purchase | 1.50 (standard, high sugar); 1.00 (16–20°) ; earlier tier 0.90 (17–21%) | GEL/kg | Rtveli 2026 (18–22 Sep 2026) | Kakheti | https://georgiatoday.ge/national-wine-agency-over-12400-tonnes-of-grapes-processed-across-kakheti/ ; https://www.eng.kavkaz-uzel.eu/articles/78778 | OFFICIAL (state Harvest Management Co., via press) | Prices raised 30–50 tetri on 22 Sep after farmer protests |
| Grapes, Rkatsiteli and other varieties | 1.30 (standard); 0.90 (16–19°); earlier tier 0.80 | GEL/kg | Rtveli 2026 | Kakheti | same | OFFICIAL (via press) | |
| Grapes, below 16° / hybrids | 0.50 (other) / 0.30 (hybrid) | GEL/kg | Rtveli 2026 | Kakheti | https://www.eng.kavkaz-uzel.eu/articles/78778 | ESTIMATE (press) | Harvest forecast ~320,000 t |
| Wheat, local purchase | ~0.60 (≈600 GEL/t) | GEL/kg | Jul 2026 (pre-harvest) | National | https://ukragroconsult.com/en/news/georgia-expects-stable-wheat-harvest-but-food-grade-share-may-decline/ | ESTIMATE (trade press) | Import price ~257 USD/t (Jan–May 2026); harvest ~200,000 t |
| Raw milk, farm-gate | 1.2 | GEL/L | Mar 2025 (no 2026 value found) | National | https://jnews.ge/139000/ | ESTIMATE (press, dated) | Milk supply falling (cows 396k in 2025) |

## 12. Exchange rates and inflation

| Item | Value | Unit | Date | Region | Source URL | Reliability | Notes |
|---|---|---|---|---|---|---|---|
| USD/GEL official | 2.5979 (21 Oct: 2.5978) | GEL per USD | 20 Oct 2026 | National | https://myvaluta.ge/en/national-bank-exchange-rates ; https://treasury.ge/en/ExchangeRate | OFFICIAL (NBG rate via Treasury) | |
| EUR/GEL official | 2.9128 (21 Oct: 2.9127) | GEL per EUR | 20 Oct 2026 | National | same | OFFICIAL | |
| CPI annual inflation | 5.6 | % y/y | Sep 2026 (rel. 2 Oct 2026) | National | https://1tv.ge/lang/en/news/geostat-georgias-annual-inflation-stood-at-5-6-in-september/ | OFFICIAL (Geostat via press) | Above 5% for 5+ months; Aug 5.6%, Jul 5.5% |
| CPI monthly | +0.4 | % m/m | Sep 2026 | National | same | OFFICIAL | Transport +2.9% m/m, health +2.8%, education −12.2% |
| Core inflation | 3.7 (excl. tobacco 3.5) | % y/y | Sep 2026 | National | same | OFFICIAL | |

---

## Sources

Official
- Geostat wages page (Q2 2026 total): https://www.geostat.ge/en/modules/categories/39/wages
- Geostat Q1 2026 earnings release: https://geostat.ge/media/80106/Average-Monthly-Nominal-Earnings-of-Employees---%28I-Quarter%2C-2026%29.pdf
- Geostat Q2 2025 earnings release: https://geostat.ge/media/73285/Average-Monthly-Nominal-Earnings-of-Employees---%28II-Quarter%2C-2025%29.pdf
- Geostat Quarterly Bulletin 2025 II: https://www.geostat.ge/media/72910/QuarterlyBull_2025_II_Eng.pdf
- Geostat earnings XLSX (not fetchable here): https://geostat.ge/media/82739/04-earnings-quarterly.xlsx
- Treasury of Georgia exchange rates: https://treasury.ge/en/ExchangeRate
- GWP water tariffs: https://www.gwp.ge/en/prices
- GNERC decisions on gas tariffs (matsne): https://matsne.gov.ge/ka/document/view/6736115 ; https://matsne.gov.ge/ka/document/view/6736215
- Gulf Georgia fuel prices: https://gulf.ge/en/fuel_prices
- Toyota Tegeta price list: https://toyota-tegeta.ge/en

Press reporting official data
- https://georgiatoday.ge/average-monthly-salary-in-georgia-rises-8-to-gel-2389-3-geostat/
- https://trend.az/scaucasus/georgia/4226384.html
- https://www.trend.az/scaucasus/georgia/4221851.html
- https://www.alia.ge/ramdeni-lari-sheadgina-sashualo-thviurma-khelphasma-saqstati-ganakhlebul-monatsemebs-aqveqhnebs/
- https://forbes.ge/saqstatis-gantskhadebith-saqarthveloshi-sashualo-thviuri-khelphasi-2-389-laramde-gaizarda/
- https://1tv.ge/lang/en/news/geostat-georgias-annual-inflation-stood-at-5-6-in-september/
- https://www.businessinsider.ge/Economic/tbilisi-residential-property-prices-rise-4-9-year-on-tear-in-q2-2026-geostat-39357?lng=eng
- https://georgiatoday.ge/electricity-tariffs-to-increase-in-tbilisi-and-regions-starting-april-1/
- https://1tv.ge/lang/en/news/gas-tariff-rises-for-tbilisi-energy-subscribers
- https://1tv.ge/lang/en/news/household-water-tariffs-in-tbilisi-mtskheta-rustavi-remain-unchanged-non-household-see-increase/
- https://jam-news.net/car-import-clearance-costs-to-rise-in-georgia/
- https://georgiatoday.ge/georgian-mps-propose-higher-excise-duties-on-older-car-imports/
- https://georgiatoday.ge/national-wine-agency-over-12400-tonnes-of-grapes-processed-across-kakheti/
- https://www.eng.kavkaz-uzel.eu/articles/78778
- https://sovanews.tv/en/2026/09/22/protesting-kakheti-farmers-begin-delivering-grapes-following-price-hike/
- https://oldwp.civil.ge/archives/463605
- https://georgiatoday.ge/?p=112380
- https://georgiatoday.ge/fuel-prices-rise-across-georgia-amid-global-oil-market-pressure/
- https://georgiatoday.ge/global-fuel-prices-rise-what-a-liter-of-petrol-costs-in-georgia-today/
- https://georgiatoday.ge/fuel-prices-rise-sharply-in-georgia-nearing-4-gel-per-liter/
- https://transparency.ge/en/blog/brief-analysis-draft-state-budget-2026

Tax summaries
- https://taxsummaries.pwc.com/Georgia/Individual/Taxes-on-personal-income
- https://taxsummaries.pwc.com/georgia/individual/other-taxes
- https://ibccs.tax/blog/property-tax-georgia-rates-thresholds-filing/
- https://www.wcshipping.com/blog/georgias-6-year-car-import-rule-what-changed-in-poti-2026-update

Crowdsourced
- https://www.numbeo.com/cost-of-living/in/Tbilisi
- https://www.numbeo.com/cost-of-living/in/Batumi
- https://www.numbeo.com/cost-of-living/in/Kutaisi-Georgia
- https://www.numbeo.com/cost-of-living/country_result.jsp?country=Georgia
- https://expatistan.com/price/doctor/tbilisi
- https://wageindicator.org/salary/minimum-wage/georgia

Market listings / aggregators
- https://amindi.org/en/fuel-prices
- https://dailyfuels.com/georgia/
- https://data.autobridge.ge/en/reports/july-listing-ledger
- https://data.autobridge.ge/en/reports/ten-k-budget
- https://autobridge.ge/en/listings/toyota-prius-2013-c4d4771c
- https://autobridge.ge/en/listings/toyota-prius-2013-89592115
- https://autobridge.ge/en/listings/toyota-prius-2013-34b428ea
- https://autobridge.ge/en/listings/mercedes-benz-e-400-2013-22e96808
- https://autobridge.ge/en/listings/opel-astra-2002-81400fa1
- https://autobridge.ge/en/listings/opel-astra-2006-f5271bf6
- https://autobridge.ge/en/listings/toyota-camry-2026-2737324a
- https://scootscoot.ge/en/blogs/why-you-should-consider-buying-a-scooter-moped-in-tbilisi/
- https://tbilisihome.ge/market-report
- https://georgia-aparts.com/blog/cheapest-areas-kutaisi-2026-08-31
- https://www.livo-property.com/en/market-data

Estimates / research / guides
- https://www.investor.ge/2026/04/20/sector-snapshot-batumi-residential-real-estate/
- https://ramad.bog.ge/galt/yepiN99l-Regional-Residential-Real-Estate_ENG_2025.pdf
- https://bm.ge/en/news/in-which-districts-of-tbilisi-was-the-square-meter-most-expensive
- https://www.globalpropertyguide.com/asia/georgia/price-history
- https://ukragroconsult.com/en/news/georgia-expects-stable-wheat-harvest-but-food-grade-share-may-decline/
- https://jnews.ge/139000/
- https://sakhva-travel.com/en/blog/tbilisi-prices-2026/
- https://sakhva-travel.com/en/blog/tbilisi-taxi-guide/
- https://madloba.info/en/blog/life-in-georgia/home-internet-in-georgia/
- https://georgiaexpats.com/guides/utilities/

Gaps (not found or not fetchable)
- Full Q2 2026 sector wage table (only top 4 sectors are public in text; the XLSX was blocked). Latest full table used: Q1 2025, business sector.
- CNG price; a reliable LPG price.
- 2026 Tbilisi Energy household gas tariff (te.ge page unreadable); last confirmed value is from 2021.
- myauto.ge (not reachable); car prices taken from autobridge.ge listings instead. No Lada data.
- Bolt/Yandex base and per-km tariff (not published in any source found).
- 2026 farm-gate milk price.
