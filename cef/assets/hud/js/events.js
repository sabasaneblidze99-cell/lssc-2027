/* hud/js/events.js - mappatura eventi server -> stato HUD. Unico posto che conosce i nomi degli eventi.
   Tutti i valori sono decisi dal server (Pawn); la pagina li mostra soltanto. Protocollo: PROTOCOLLO.md. */
(function () {
  "use strict";
  var HUD = window.HUD, S = HUD.state, U = HUD.util;

  var CHAT_CAT = ["system", "ic", "ooc", "faction", "job", "ads", "emergency", "admin"];
  var NT_KIND = ["success", "info", "warn", "error", "money-in", "money-out", "mission", "job", "rp"];
  var TOAST_KIND = ["info", "success", "warn", "error"];   // lssc:toast (UI_LV_TOAST_*)

  HUD.CHAT_CAT = CHAT_CAT;

  // lssc:hud money bank level expIn expStep hour minute (esistente)
  HUD.on("lssc:hud", function (money, bank, level, expIn, expStep, hour, minute) {
    S.set("money", { cash: U.int(money), bank: U.int(bank) });
    S.set("level", { level: Math.max(1, U.int(level, 1)), expIn: U.int(expIn), expStep: U.int(expStep) });
    var h = U.int(hour, -1);
    if (h >= 0) {
      var c = S.get("clock") || {};
      S.set("clock", { h: h, m: U.int(minute), d: c.d || 0, mo: c.mo || 0, y: c.y || 0 });
    }
  });
  // lssc:clock hour minute day month year  (ora del server)
  HUD.on("lssc:clock", function (h, m, d, mo, y) {
    S.set("clock", { h: U.int(h), m: U.int(m), d: U.int(d), mo: U.int(mo), y: U.int(y) });
  });
  HUD.on("lssc:hud_visible", function (show) { S.set("visible", !!U.int(show)); });
  // lssc:identity name playerid
  HUD.on("lssc:identity", function (name, id) { S.set("identity", { name: U.str(name).replace(/_/g, " "), id: U.int(id) }); });
  // lssc:status health armor hunger thirst   (0-100; thirst -1 = sistema non presente nel server)
  HUD.on("lssc:status", function (hp, armor, hunger, thirst) {
    S.set("status", { hp: U.int(hp), armor: U.int(armor), hunger: U.int(hunger, -1), thirst: U.int(thirst, -1) });
  });
  // lssc:contract (esistente, stessi argomenti di prima)
  HUD.on("lssc:contract", function (id, state, objective, destination, distanceM, secondsLeft, integrityPct) {
    S.set("contract", {
      id: U.int(id), state: U.int(state), goal: U.str(objective), dest: U.str(destination),
      dist: U.int(distanceM, -1), left: U.int(secondsLeft, -1), integ: U.int(integrityPct, -1), at: Date.now()
    });
  });
  // lssc:veh_show show modelName
  HUD.on("lssc:veh_show", function (show, name) { S.set("vehShow", { show: !!U.int(show), name: U.str(name) }); });
  // lssc:veh speedKmh fuelPct engine lights healthPct locked   (fuelPct -1 = veicolo senza carburante gestito)
  HUD.on("lssc:veh", function (speed, fuel, engine, lights, health, locked) {
    S.set("veh", { speed: Math.max(0, U.int(speed)), fuel: U.int(fuel, -1), engine: !!U.int(engine),
      lights: !!U.int(lights), health: U.int(health, 100), locked: !!U.int(locked) });
  });
  // lssc:zone districtId danger indoor   (danger -1 sconosciuto, 0 sicura, 1 normale, 2 pericolosa)
  HUD.on("lssc:zone", function (d, danger, indoor) { S.set("zone", { d: U.int(d), danger: U.int(danger, -1), indoor: !!U.int(indoor) }); });
  // lssc:voice state key   (0 plugin assente, 1 nessun microfono, 2 pronto)
  HUD.on("lssc:voice", function (st, key) { S.set("voice", { st: U.int(st), key: U.str(key) }); });
  // lssc:interact show key label progress   (progress -1 = nessuna barra)
  HUD.on("lssc:interact", function (show, key, label, progress) {
    S.set("interact", { show: !!U.int(show), key: U.str(key), label: U.str(label), progress: U.int(progress, -1) });
  });

  // eventi "flusso" (non stato)
  HUD.on("lssc:chat", function (cat, text) { HUD.chat && HUD.chat.add(CHAT_CAT[U.int(cat)] || "system", U.str(text)); });
  HUD.on("lssc:chat_clear", function () { HUD.chat && HUD.chat.clear(); });
  HUD.on("lssc:chat_mode", function (on) { S.set("chatMode", !!U.int(on)); });
  HUD.on("lssc:notify", function (kind, title, text, ms) {
    HUD.notify && HUD.notify(NT_KIND[U.int(kind)] || "info", U.str(title), U.str(text), U.int(ms));
  });
  HUD.on("lssc:toast", function (text, kind) { HUD.notify && HUD.notify(TOAST_KIND[U.int(kind)] || "info", "", U.str(text), 0); });
  HUD.on("lssc:reject", function (text) { HUD.notify && HUD.notify("error", "", U.str(text), 0); });
  HUD.on("lssc:money_delta", function (delta, bank) { HUD.moneyDelta && HUD.moneyDelta(U.int(delta), !!U.int(bank)); });
  HUD.on("lssc:levelup", function (level) { HUD.levelUp && HUD.levelUp(U.int(level)); });
  // lssc:quick_open mask  (bit 0 inventario, 1 telefono, 2 mappa, 3 animazioni, 4 veicoli; impostazioni sempre)
  HUD.on("lssc:quick_open", function (mask) { HUD.quick && HUD.quick.open(U.int(mask)); });
  HUD.on("lssc:quick_close", function () { HUD.quick && HUD.quick.close(false); });
})();
