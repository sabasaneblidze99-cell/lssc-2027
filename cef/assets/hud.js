/* hud.js - HUD sempre visibile (soldi, banca, livello, ora, contratto attivo, toast).
   Niente focus, niente blur, aggiornamenti solo del testo che cambia (rendering software OSR). */
(function () {
  "use strict";
  var LSSC = window.LSSC, U = LSSC.util;
  var root = U.el("hud");

  root.innerHTML =
    '<section class="hud-stats" id="hud-stats" hidden aria-label="Soldi e livello">' +
      '<div class="hud-clock num" id="hud-clock">--:--</div>' +
      '<div class="hud-cash num" id="hud-cash">$0</div>' +
      '<div class="hud-bank">Banca <b class="num" id="hud-bank">$0</b></div>' +
      '<div class="hud-level"><span class="hud-exp-text num" id="hud-exp-text"></span>' +
        '<div class="hud-exp"><i id="hud-exp-bar" style="width:0%"></i></div>' +
        '<span class="hud-level-tag num" id="hud-level">Liv. 1</span></div>' +
    "</section>" +
    '<section class="hud-contract" id="hud-contract" hidden aria-label="Contratto attivo">' +
      '<div class="hud-contract-head"><span class="hud-contract-title num" id="hc-title"></span><span class="hud-contract-state" id="hc-state"></span></div>' +
      '<div class="hud-contract-goal" id="hc-goal"></div>' +
      '<div class="hud-contract-dest" id="hc-dest"></div>' +
      '<div class="hud-contract-row">' +
        '<div class="hud-timer num" id="hc-timer-wrap"><span id="hc-timer">--:--</span><small>alla scadenza</small></div>' +
        '<div class="hud-integ" id="hc-integ-wrap"><div class="hud-integ-label"><span>Integrità merce</span><span class="num" id="hc-integ">100%</span></div>' +
          '<div class="bar" id="hc-integ-bar"><i style="width:100%"></i></div></div>' +
      "</div>" +
    "</section>";

  var el = {
    stats: U.el("hud-stats"), clock: U.el("hud-clock"), cash: U.el("hud-cash"), bank: U.el("hud-bank"),
    level: U.el("hud-level"), expText: U.el("hud-exp-text"), expBar: U.el("hud-exp-bar"),
    ct: U.el("hud-contract"), title: U.el("hc-title"), state: U.el("hc-state"), goal: U.el("hc-goal"), dest: U.el("hc-dest"),
    timerWrap: U.el("hc-timer-wrap"), timer: U.el("hc-timer"), integWrap: U.el("hc-integ-wrap"), integ: U.el("hc-integ"), integBar: U.el("hc-integ-bar")
  };

  function setText(node, text) { if (node.textContent !== text) node.textContent = text; }

  // lssc:hud money bank level expIn expStep hour minute   (expStep 0 = livello massimo; hour < 0 = ora non mostrata)
  LSSC.on("lssc:hud", function (money, bank, level, expIn, expStep, hour, minute) {
    el.stats.hidden = false;
    setText(el.cash, U.money(money));
    setText(el.bank, U.money(bank));
    setText(el.level, "Liv. " + Math.max(1, U.int(level, 1)));
    var step = U.int(expStep), inLv = U.int(expIn);
    if (step > 0) {
      setText(el.expText, inLv + "/" + step + " exp");
      el.expBar.style.width = U.pct(inLv, step) + "%";
    } else {
      setText(el.expText, "livello massimo");
      el.expBar.style.width = "100%";
    }
    var h = U.int(hour, -1), m = U.int(minute);
    setText(el.clock, h < 0 ? "" : (h < 10 ? "0" : "") + h + ":" + (m < 10 ? "0" : "") + m);
  });

  // lssc:hud_visible 0/1 (es. nascosto durante login, intro o cutscene)
  LSSC.on("lssc:hud_visible", function (show) { root.hidden = !U.int(show); });

  // ---------------------------------------------------------------- contratto attivo
  var expAt = 0, timer = 0;

  function renderTimer() {
    if (!expAt) { setText(el.timer, "--:--"); return; }
    var left = Math.max(0, Math.round((expAt - Date.now()) / 1000));
    setText(el.timer, U.time(left));
    el.timerWrap.classList.toggle("low", left <= 60);
    if (left <= 0) stopTimer();
  }
  function stopTimer() { if (timer) { clearInterval(timer); timer = 0; } }

  // lssc:contract contractId state objective destination distanceM secondsLeft integrityPct fromDistrict toDistrict cargo reward repPenalty
  // contractId 0 = nessun contratto (HUD nascosto). Stesso significato di UI_CityContractHud/UI_CityContractStatus.
  LSSC.on("lssc:contract", function (id, state, objective, destination, distanceM, secondsLeft, integrityPct) {
    id = U.int(id);
    stopTimer();
    if (id <= 0) { el.ct.hidden = true; expAt = 0; return; }
    var inProgress = U.int(state) === 2;
    setText(el.title, "Contratto #" + id);
    setText(el.state, inProgress ? "In consegna" : "Accettato");
    setText(el.goal, U.str(objective) || "-");
    var d = U.int(distanceM, -1), dest = U.str(destination) || "-";
    setText(el.dest, "Verso " + dest + (d >= 0 ? " · " + U.dist(d) : ""));
    var sec = U.int(secondsLeft, -1);
    expAt = sec >= 0 ? Date.now() + sec * 1000 : 0;
    renderTimer();
    if (expAt) timer = setInterval(renderTimer, 1000);
    var ig = U.int(integrityPct, -1);
    el.integWrap.hidden = ig < 0;
    if (ig >= 0) {
      ig = Math.min(100, ig);
      setText(el.integ, ig + "%");
      el.integBar.firstChild.style.width = ig + "%";
      el.integBar.className = "bar" + (ig < 40 ? " danger" : ig < 70 ? " warn" : "");
    }
    el.ct.hidden = false;
  });

  // ---------------------------------------------------------------- toast
  var box = U.el("toasts");
  var KIND = ["info", "ok", "warn", "err"];
  var TTL = 4000, MAX = 3;

  function toast(text, kind) {
    var t = document.createElement("div");
    t.className = "toast " + (KIND[U.int(kind)] || "info");
    t.setAttribute("role", "status");
    t.textContent = U.str(text);
    box.appendChild(t);
    while (box.children.length > MAX) box.removeChild(box.firstChild);
    setTimeout(function () { t.classList.add("out"); }, TTL);
    setTimeout(function () { if (t.parentNode) t.parentNode.removeChild(t); }, TTL + 220);
  }
  LSSC.toast = toast;

  // lssc:toast text kind   (kind 0 info, 1 ok, 2 attenzione, 3 errore: come UI_LV_TOAST_*)
  LSSC.on("lssc:toast", toast);
  // lssc:reject text: esito negativo del server (come UI_LivingReject). Bacheca e telefono chiudono le conferme.
  LSSC.on("lssc:reject", function (text) { toast(text, 3); });
})();
