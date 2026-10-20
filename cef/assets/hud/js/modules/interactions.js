/* interactions.js - suggerimento contestuale ([Y] ...) quando il server rileva un punto interattivo vicino,
   con barra di avanzamento opzionale. L'azione vera la esegue il server (tasto Y -> comando esistente). */
(function () {
  "use strict";
  var HUD = window.HUD, S = HUD.state, T = HUD.S;

  HUD.modules.interact = function (host) {
    var el = document.createElement("div");
    el.className = "ia anim-pop";
    el.hidden = true;
    el.innerHTML = '<span class="ia-key" id="ia-key">Y</span><span class="ia-label" id="ia-label"></span><div class="ia-prog" id="ia-prog" hidden><div class="hb"><i id="ia-bar"></i></div></div>';
    host.appendChild(el);
    function $(id) { return document.getElementById(id); }
    S.sub("interact", function (v) {
      el.hidden = !v.show;
      if (!v.show) return;
      $("ia-key").textContent = v.key || "Y";
      $("ia-label").textContent = v.label || T.interact;
      $("ia-prog").hidden = v.progress < 0;
      if (v.progress >= 0) $("ia-bar").style.width = Math.min(100, v.progress) + "%";
    });
  };

  /* livello raggiunto: animazione leggera (2,6 s), solo su lssc:levelup dal server */
  HUD.modules.levelup = function (host) {
    var el = document.createElement("div");
    el.className = "g-panel lvup";
    el.hidden = true;
    el.innerHTML = '<div class="k">' + T.levelUp + '</div><div class="n" id="lv-n"></div>';
    host.appendChild(el);
    var t = 0;
    HUD.levelUp = function (lv) {
      el.querySelector("#lv-n").textContent = String(lv);
      el.hidden = false;
      el.classList.remove("show"); void el.offsetWidth; el.classList.add("show");
      if (t) clearTimeout(t);
      t = setTimeout(function () { el.hidden = true; el.classList.remove("show"); }, 2700);
    };
  };
})();
