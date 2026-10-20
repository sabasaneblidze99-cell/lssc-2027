/* zone.js - pannello accanto al radar nativo: quartiere (Living Los Santos), zona sicura/pericolosa, voce.
   Il radar di GTA resta la navigazione: qui niente mappa finta. */
(function () {
  "use strict";
  var HUD = window.HUD, S = HUD.state, T = HUD.S, I = HUD.icon;

  HUD.modules.zone = function (host) {
    var el = document.createElement("section");
    el.className = "g-panel zn";
    el.hidden = true;
    el.innerHTML =
      '<div class="zn-place"><span class="g-ico">' + I("pin") + '</span><div><div class="zn-name" id="zn-name"></div><div class="zn-city" id="zn-city"></div></div></div>' +
      '<div class="zn-chips"><span class="hchip" id="zn-safety" hidden></span><span class="hchip" id="zn-voice" hidden></span></div>';
    host.appendChild(el);
    function $(id) { return document.getElementById(id); }

    S.sub("zone", function (z) {
      el.hidden = false;
      var d = z.d > 0 && z.d < T.districts.length ? z.d : 0;
      $("zn-name").textContent = d ? T.districts[d] : T.city;
      $("zn-city").textContent = z.indoor ? T.indoor : (d ? T.city : "San Andreas");
      var c = $("zn-safety");
      c.hidden = z.danger < 0 || z.indoor;
      if (!c.hidden) {
        c.className = "hchip " + (z.danger === 0 ? "safe" : z.danger === 2 ? "danger" : "");
        c.innerHTML = I(z.danger === 2 ? "alert" : "shieldOk") + "<span></span>";
        c.lastChild.textContent = z.danger === 0 ? T.safe : z.danger === 2 ? T.danger : T.neutral;
      }
    });
    S.sub("voice", function (v) {
      var c = $("zn-voice");
      c.hidden = false;
      el.hidden = false;
      var ok = v.st === 2;
      c.className = "hchip " + (ok ? "mic-on" : "mic-off");
      c.innerHTML = I(ok ? "mic" : "micOff") + "<span></span>";
      c.lastChild.textContent = ok ? T.voiceReady(v.key || "B") : v.st === 1 ? T.voiceNoMic : T.voiceOff;
    });
  };
})();
