// Test di U.money / U.parseMoney di cef/assets/bridge.js (node tests/economy/money_js_test.js)
const fs = require('fs'), vm = require('vm');
const src = fs.readFileSync(__dirname + '/../../cef/assets/bridge.js', 'utf8');
const win = { addEventListener() {}, document: { addEventListener() {}, getElementById() { return null; } } };
const ctx = { window: win, document: win.document, console, setTimeout() {}, clearTimeout() {}, navigator: {} };
ctx.self = ctx.window; vm.createContext(ctx);
try { vm.runInContext(src, ctx); } catch (e) { console.log('nota: ' + e.message); }
const L = ctx.window.LSSC || ctx.LSSC; const U = L && L.util;
if (!U) { console.log('U non trovato'); process.exit(1); }
let fail = 0;
const eq = (a, b, t) => { if (a !== b) { fail++; console.log('FAIL', t, JSON.stringify(a), '!=', JSON.stringify(b)); } };
eq(U.money(125050), '₾1.250,50', 'money 1250,50'); eq(U.money(8500000), '₾85.000', 'money 85000');
eq(U.money(-4550), '-₾45,50', 'money neg'); eq(U.money(5), '₾0,05', 'money 5 tetri'); eq(U.money(0), '₾0', 'zero');
eq(U.parseMoney('25'), 2500, 'p25'); eq(U.parseMoney('12,5'), 1250, 'p12,5'); eq(U.parseMoney('12.50'), 1250, 'p12.50');
eq(Number.isNaN(U.parseMoney('1.250,50')), true, 'p migliaia'); eq(Number.isNaN(U.parseMoney('-3')), true, 'p neg'); eq(Number.isNaN(U.parseMoney('')), true, 'p vuoto');
console.log(fail ? fail + ' falliti' : 'money js: tutti ok');
process.exit(fail ? 1 : 0);
