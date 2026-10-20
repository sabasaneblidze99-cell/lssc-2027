/* hud/js/app.js - avvio dell'HUD: crea le zone dello schermo e monta i moduli.
   Ordine: rendering (modules/*) <- stato (state.js) <- eventi (events.js) <- bridge (bridge.js). */
(function () {
  "use strict";
  var HUD = window.HUD, M = HUD.modules, S = HUD.state;
  var root = document.getElementById("hud");
  root.innerHTML =
    '<div class="hud-brand"><div class="brand"><div class="k">LOS SANTOS</div><div class="t"><span>STREET CRIME\'S</span></div></div></div>' +
    '<div class="hud-chat" id="hz-chat"></div>' +
    '<div class="hud-right" id="hz-right"></div>' +
    '<div class="hud-zone" id="hz-zone"></div>' +
    '<div class="hud-vehicle" id="hz-veh"></div>' +
    '<div class="hud-interact" id="hz-ia"></div>' +
    '<div class="hud-levelup" id="hz-lv"></div>' +
    '<div id="hz-ov"></div>';
  function z(id) { return document.getElementById(id); }

  M.status(z("hz-right"));
  M.contract(z("hz-right"));
  M.notifications(z("hz-right"));
  M.vehicle(z("hz-veh"));
  M.zone(z("hz-zone"));
  M.chat(z("hz-chat"));
  M.interact(z("hz-ia"));
  M.levelup(z("hz-lv"));
  M.quick(z("hz-ov"));

  // HUD nascosto finche' il server non lo mostra (login, intro, cutscene: lssc:hud_visible)
  root.hidden = true;
  S.sub("visible", function (v) { root.hidden = !v; if (!v && HUD.chat) HUD.chat.close(); });
})();
