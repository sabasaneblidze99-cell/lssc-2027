/* hud/js/bridge.js - accesso dell'HUD al bridge centrale (../bridge.js, window.LSSC).
   Nessun modulo chiama window.cef direttamente: tutto passa da qui.
   Testo libero (chat) verso il server: UTF-8 in esadecimale, perche' samp-cef unisce gli
   argomenti con spazi e il bridge centrale sostituisce gli spazi (vedi PROTOCOLLO.md). */
(function () {
  "use strict";
  var HUD = window.HUD = window.HUD || {};
  var LSSC = window.LSSC;
  HUD.modules = HUD.modules || {};

  HUD.on = function (name, fn) { LSSC.on(name, fn); };
  HUD.emit = function () { LSSC.emit.apply(null, arguments); };
  HUD.focus = function (on) { LSSC.focus(!!on); };
  HUD.util = LSSC.util;

  HUD.hex = function (text) {
    var bytes = new TextEncoder().encode(String(text));
    var out = "";
    for (var i = 0; i < bytes.length; i++) out += (bytes[i] < 16 ? "0" : "") + bytes[i].toString(16);
    return out;
  };

  // Vista interattiva attiva nell'HUD (chat, azioni rapide, impostazioni): una sola alla volta.
  // Le viste di bridge.js (bacheca, telefono) hanno la precedenza: se una e' aperta l'HUD non prende il focus.
  var owner = null;
  HUD.takeFocus = function (who) {
    if (LSSC.currentView && LSSC.currentView()) return false;
    owner = who;
    document.body.classList.add("hud-focus");
    HUD.focus(true);
    return true;
  };
  HUD.releaseFocus = function (who) {
    if (owner !== who) return;
    owner = null;
    document.body.classList.remove("hud-focus");
    HUD.focus(false);
  };
  HUD.focusOwner = function () { return owner; };

  // Tasti con la pagina NON a fuoco: arrivano solo se il server ha chiamato cef_always_listen_keys
  // e la chat/dialog di SA-MP non e' attiva (comportamento del client samp-cef).
  var keyHandlers = [];
  HUD.onKey = function (fn) { keyHandlers.push(fn); };
  document.addEventListener("keydown", function (ev) {
    for (var i = 0; i < keyHandlers.length; i++) {
      if (keyHandlers[i](ev) === true) { ev.preventDefault(); return; }
    }
  });
})();
