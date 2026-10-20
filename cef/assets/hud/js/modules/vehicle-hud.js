/* vehicle-hud.js - tachimetro semicircolare (basso a destra). Appare solo quando il server invia lssc:veh_show 1
   (il giocatore guida un veicolo compatibile) e sparisce con lssc:veh_show 0.
   Velocita' dal server (~4 aggiornamenti/s, solo se cambia): l'arco si muove con una transizione CSS di 250 ms.
   Nessuna marcia: il gamemode non simula un cambio. */
(function () {
  "use strict";
  var HUD = window.HUD, S = HUD.state, T = HUD.S, I = HUD.icon;

  var MAX = 240, CX = 100, CY = 100, R = 82, A0 = 210, A1 = -30;   // gradi: da 210 (basso sx) a -30 (basso dx)
  var SWEEP = (A0 - A1) * Math.PI / 180, LEN = R * SWEEP;

  function pt(deg, r) {
    var a = deg * Math.PI / 180;
    return [CX + r * Math.cos(a), CY - r * Math.sin(a)];
  }
  function arcPath(r) {
    var p0 = pt(A0, r), p1 = pt(A1, r);
    return "M" + p0[0].toFixed(2) + " " + p0[1].toFixed(2) + " A" + r + " " + r + " 0 1 1 " + p1[0].toFixed(2) + " " + p1[1].toFixed(2);
  }

  HUD.modules.vehicle = function (host) {
    var ticks = "", labels = "";
    for (var v = 0; v <= MAX; v += 20) {
      var deg = A0 - (A0 - A1) * v / MAX, major = v % 40 === 0;
      var a = pt(deg, R - 10), b = pt(deg, R - (major ? 17 : 14));
      ticks += '<line class="vh-tick' + (major ? " major" : "") + '" x1="' + a[0].toFixed(1) + '" y1="' + a[1].toFixed(1) + '" x2="' + b[0].toFixed(1) + '" y2="' + b[1].toFixed(1) + '"/>';
      if (major) { var l = pt(deg, R - 27); labels += '<text class="vh-label" x="' + l[0].toFixed(1) + '" y="' + l[1].toFixed(1) + '">' + v + "</text>"; }
    }
    var el = document.createElement("section");
    el.className = "g-panel vh";
    el.hidden = true;
    el.innerHTML =
      '<div class="vh-gauge"><svg viewBox="0 0 200 130">' +
        '<defs><linearGradient id="vh-grad" x1="0" y1="0" x2="1" y2="0"><stop offset="0" stop-color="#A855F7"/><stop offset="0.55" stop-color="#FF3674"/><stop offset="1" stop-color="#FF914D"/></linearGradient></defs>' +
        '<path class="vh-track" d="' + arcPath(R) + '"/>' +
        '<path class="vh-arc" id="vh-arc" d="' + arcPath(R) + '" stroke-dasharray="' + LEN.toFixed(1) + '" stroke-dashoffset="' + LEN.toFixed(1) + '"/>' +
        ticks + labels +
      '</svg><div class="vh-center"><div class="vh-speed" id="vh-speed">000</div><div class="vh-unit">' + T.speedUnit + '</div></div>' +
      '<div class="vh-name" id="vh-name"></div></div>' +
      '<div class="vh-info">' +
        '<div class="vh-fuel" id="vh-fuel" title="' + T.fuel + '"><span class="g-ico">' + I("fuel") + '</span><div class="hb"><i id="vh-fb"></i></div><b id="vh-fv">-</b></div>' +
        '<div class="vh-flags">' +
          flag("engine", "engine", T.engine) + flag("lights", "lights", T.lights) + flag("lock", "lock", T.locked) + flag("damage", "wrench", T.damage) +
        "</div>" +
      "</div>";
    host.appendChild(el);
    function flag(cls, icon, title) { return '<span class="vh-flag ' + cls + '" id="vh-f-' + cls + '" title="' + title + '">' + I(icon) + "</span>"; }
    function $(id) { return document.getElementById(id); }
    var arc = $("vh-arc"), speed = $("vh-speed");

    S.sub("vehShow", function (v) {
      el.hidden = !v.show;
      $("vh-name").textContent = v.name || "";
      if (!v.show) { arc.setAttribute("stroke-dashoffset", LEN.toFixed(1)); speed.textContent = "000"; }
    });
    S.sub("veh", function (v) {
      var s = Math.min(999, v.speed);
      var t = (s < 10 ? "00" : s < 100 ? "0" : "") + s;
      if (speed.textContent !== t) speed.textContent = t;
      arc.setAttribute("stroke-dashoffset", (LEN * (1 - Math.min(MAX, s) / MAX)).toFixed(1));
      var f = $("vh-fuel");
      f.hidden = v.fuel < 0;
      if (v.fuel >= 0) {
        $("vh-fb").style.width = Math.min(100, v.fuel) + "%";
        $("vh-fv").textContent = v.fuel + "%";
        f.classList.toggle("low", v.fuel <= 15);
      }
      $("vh-f-engine").classList.toggle("on", v.engine);
      $("vh-f-lights").classList.toggle("on", v.lights);
      $("vh-f-lock").classList.toggle("on", v.locked);
      var dmg = $("vh-f-damage");
      dmg.classList.toggle("on", v.health < 35);
      dmg.classList.toggle("warn", v.health >= 35 && v.health < 65);
      dmg.title = T.damage + ": " + (100 - v.health) + "%";
    });
  };
})();
