/* notifications.js - motore notifiche: massimo 4 visibili, le altre in coda; niente duplicati ravvicinati. */
(function () {
  "use strict";
  var HUD = window.HUD, T = HUD.S, I = HUD.icon;

  var ICON = { success: "check", info: "info", warn: "warn", error: "x", "money-in": "moneyIn", "money-out": "moneyOut",
    mission: "star", job: "briefcase", rp: "people" };
  var MAX_VISIBLE = 4, DEF_MS = 5000, MIN_MS = 1500, MAX_MS = 15000, QUEUE_MAX = 20, DEDUPE_MS = 2500;

  HUD.modules.notifications = function (host) {
    var wrap = document.createElement("section");
    wrap.className = "nt-wrap";
    wrap.hidden = true;
    wrap.innerHTML = '<div class="nt-head"><span>' + T.notifications + '</span><b id="nt-count"></b></div><div class="nt-list" id="nt-list"></div>';
    host.appendChild(wrap);
    var list = wrap.querySelector("#nt-list"), count = wrap.querySelector("#nt-count");
    var queue = [], visible = 0, last = {};

    function refresh() {
      wrap.hidden = visible === 0;
      count.textContent = queue.length ? T.newCount(queue.length) : "";
    }
    // spazio libero sotto la colonna: mai sopra il tachimetro (o oltre il bordo dello schermo)
    function hasRoom() {
      if (visible === 0) return true;
      var veh = document.querySelector("#hud .vh");
      var limit = limitY();
      var last = list.lastElementChild;
      if (!last) return true;
      var h = last.getBoundingClientRect().height + 8;
      return list.getBoundingClientRect().bottom + h <= limit;
    }
    function limitY() {
      var veh = document.querySelector("#hud .vh");
      return veh && !veh.hidden && veh.offsetParent !== null ? veh.getBoundingClientRect().top - 12 : window.innerHeight - 12;
    }
    function pump() { fit(); while (queue.length && visible < MAX_VISIBLE && hasRoom()) show(queue.shift()); refresh(); }
    // il layout e' cambiato (tachimetro o contratto comparsi): le notifiche che non entrano tornano in coda
    function fit() {
      var lim = limitY();
      while (list.lastElementChild && list.getBoundingClientRect().bottom > lim) {
        var it = list.lastElementChild;
        clearTimeout(it._t);
        list.removeChild(it);
        visible--;
        if (it._n) queue.unshift(it._n);
      }
      refresh();
    }
    HUD.state.sub("vehShow", function () { setTimeout(pump, 30); });
    HUD.state.sub("contract", function () { setTimeout(pump, 30); });
    function show(n) {
      visible++;
      var item = document.createElement("div");
      item.className = "nt anim-in-right k-" + n.kind;
      item.setAttribute("role", "status");
      item.innerHTML = '<div class="nt-ico">' + I(ICON[n.kind] || "info") + '</div><div><div class="nt-t"></div><div class="nt-d"></div></div><div class="nt-when"></div>';
      item.querySelector(".nt-t").textContent = n.title || T.ntTitle[n.kind] || "";
      var d = item.querySelector(".nt-d");
      if (n.text) d.textContent = n.text; else d.remove();
      item.querySelector(".nt-when").textContent = T.ago(0);
      list.insertBefore(item, list.firstChild);
      item._n = n;
      refresh();
      item._t = setTimeout(function () {
        item.classList.remove("anim-in-right");
        item.classList.add("anim-out-right");
        setTimeout(function () {
          if (item.parentNode) item.parentNode.removeChild(item);
          visible--;
          pump();
        }, 230);
      }, n.ms);
    }

    // API usata da events.js (lssc:notify, lssc:toast, lssc:reject)
    HUD.notify = function (kind, title, text, ms) {
      var key = kind + "|" + title + "|" + text, now = Date.now();
      if (last[key] && now - last[key] < DEDUPE_MS) return;   // stesso messaggio ripetuto: ignorato
      last[key] = now;
      ms = ms > 0 ? Math.max(MIN_MS, Math.min(MAX_MS, ms)) : DEF_MS;
      var n = { kind: kind, title: title, text: text, ms: ms };
      if (visible < MAX_VISIBLE && !queue.length && hasRoom()) show(n);
      else { if (queue.length >= QUEUE_MAX) queue.shift(); queue.push(n); refresh(); }
    };
    // compatibilita' con il vecchio LSSC.toast(text, kind)
    window.LSSC.toast = function (text, kind) { HUD.notify(["info", "success", "warn", "error"][kind | 0] || "info", "", String(text), 0); };
  };
})();
