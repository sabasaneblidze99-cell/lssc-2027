/* contract.js - contratto attivo (Living Los Santos). Stessi dati dell'HUD precedente (lssc:contract). */
(function () {
  "use strict";
  var HUD = window.HUD, S = HUD.state, T = HUD.S, U = HUD.util;

  HUD.modules.contract = function (host) {
    var el = document.createElement("section");
    el.className = "g-panel hct";
    el.hidden = true;
    el.innerHTML =
      '<div class="ct-head"><span class="ct-title" id="ct-title"></span><span class="ct-state" id="ct-state"></span></div>' +
      '<div class="ct-goal" id="ct-goal"></div><div class="ct-dest" id="ct-dest"></div>' +
      '<div class="ct-row"><div class="ct-timer" id="ct-tw"><span id="ct-timer">--:--</span><small>' + T.timeLeft + '</small></div>' +
      '<div class="ct-integ" id="ct-iw"><div class="lbl"><span>' + T.integrity + '</span><span class="g-num" id="ct-ig">100%</span></div><div class="hb"><i id="ct-ib"></i></div></div></div>';
    host.appendChild(el);
    function $(id) { return document.getElementById(id); }
    function txt(n, t) { if (n.textContent !== t) n.textContent = t; }

    var expAt = 0, timer = 0;
    function tick() {
      var left = expAt ? Math.max(0, Math.round((expAt - Date.now()) / 1000)) : -1;
      txt($("ct-timer"), U.time(left));
      $("ct-tw").classList.toggle("low", left >= 0 && left <= 60);
      if (left <= 0 && timer) { clearInterval(timer); timer = 0; }
    }
    S.sub("contract", function (c) {
      if (timer) { clearInterval(timer); timer = 0; }
      if (c.id <= 0) { el.hidden = true; expAt = 0; return; }
      txt($("ct-title"), T.contract(c.id));
      txt($("ct-state"), c.state === 2 ? T.delivering : T.accepted);
      txt($("ct-goal"), c.goal || "-");
      txt($("ct-dest"), "→ " + (c.dest || "-") + (c.dist >= 0 ? " · " + U.dist(c.dist) : ""));
      expAt = c.left >= 0 ? c.at + c.left * 1000 : 0;
      $("ct-tw").hidden = !expAt;
      tick();
      if (expAt) timer = setInterval(tick, 1000);   // unico timer locale: conto alla rovescia
      var ig = c.integ;
      $("ct-iw").hidden = ig < 0;
      if (ig >= 0) {
        ig = Math.min(100, ig);
        txt($("ct-ig"), ig + "%");
        $("ct-ib").style.width = ig + "%";
        $("ct-iw").className = "ct-integ" + (ig < 40 ? " bad" : ig < 70 ? " warn" : "");
      }
      el.hidden = false;
    });
  };
})();
