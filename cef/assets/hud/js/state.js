/* hud/js/state.js - stato dell'HUD. Unica fonte per i componenti: i valori arrivano solo dal server
   (events.js). set() notifica soltanto se il valore cambia davvero, cosi' il DOM si tocca il meno possibile. */
(function () {
  "use strict";
  var HUD = window.HUD = window.HUD || {};
  var data = {};
  var subs = {};

  function same(a, b) {
    if (a === b) return true;
    if (typeof a !== "object" || typeof b !== "object" || !a || !b) return false;
    var ka = Object.keys(a), kb = Object.keys(b);
    if (ka.length !== kb.length) return false;
    for (var i = 0; i < ka.length; i++) if (a[ka[i]] !== b[ka[i]]) return false;
    return true;
  }

  HUD.state = {
    get: function (key) { return data[key]; },
    set: function (key, value) {
      var prev = data[key];
      if (same(prev, value)) return false;
      data[key] = value;
      var list = subs[key];
      if (list) for (var i = 0; i < list.length; i++) {
        try { list[i](value, prev); } catch (e) { console.error("[hud] " + key, e); }
      }
      return true;
    },
    sub: function (key, fn) {
      (subs[key] = subs[key] || []).push(fn);
      if (data[key] !== undefined) fn(data[key], undefined);
      return function () { subs[key] = subs[key].filter(function (f) { return f !== fn; }); };
    }
  };

  // Impostazioni del giocatore (solo comodita' locali: localStorage puo' fallire in samp-cef / anteprima)
  var DEF = { chatLines: 10, chatOpacity: 92, timestamps: false, chatFade: true, hudScale: 100 };
  var settings = {};
  try { settings = JSON.parse(localStorage.getItem("lssc.hud.settings") || "{}") || {}; } catch (e) { settings = {}; }
  HUD.settings = {
    get: function (k) { return settings[k] !== undefined ? settings[k] : DEF[k]; },
    all: function () { var o = {}; for (var k in DEF) o[k] = HUD.settings.get(k); return o; },
    save: function (o) {
      for (var k in o) if (k in DEF) settings[k] = o[k];
      try { localStorage.setItem("lssc.hud.settings", JSON.stringify(settings)); } catch (e) { /* non persistente */ }
      HUD.state.set("settings", HUD.settings.all());
    },
    reset: function () { settings = {}; HUD.settings.save({}); },
    defaults: DEF
  };
  HUD.state.set("settings", HUD.settings.all());
})();
