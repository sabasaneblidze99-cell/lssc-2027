/* player-status.js - pannello in alto a destra: identita', ora, salute/armatura/fame(/sete), soldi, livello. */
(function () {
  "use strict";
  var HUD = window.HUD, S = HUD.state, T = HUD.S, U = HUD.util, I = HUD.icon;

  HUD.modules.status = function (host) {
    var el = document.createElement("section");
    el.className = "g-panel st";
    el.hidden = true;
    el.innerHTML =
      '<div class="st-head">' +
        '<div class="st-avatar" id="st-av">?</div>' +
        '<div class="st-who"><div class="st-name" id="st-name"></div><div class="st-id">' + T.id + ': <b id="st-id">-</b></div></div>' +
        '<div class="st-clock"><div class="st-online">' + T.online + '</div><div class="st-time g-num" id="st-time">--:--</div><div class="st-date" id="st-date"></div></div>' +
      "</div>" +
      '<div class="st-body">' +
        '<div class="st-bars">' +
          row("hp", "heart", T.hp) + row("armor", "shield", T.armor) + row("hunger", "food", T.hunger) + row("thirst", "drop", T.thirst) +
        "</div>" +
        '<div class="st-money">' +
          '<div class="money-row cash" title="' + T.cash + '"><span class="g-ico">' + I("wallet") + '</span><b id="st-cash">$0</b></div>' +
          '<div class="money-row bank" title="' + T.bank + '"><span class="g-ico">' + I("bank") + '</span><b id="st-bank">$0</b></div>' +
        "</div>" +
      "</div>" +
      '<div class="st-level"><span class="lv" id="st-lv"></span><div class="hb"><i id="st-xpbar"></i></div><span class="xp" id="st-xp"></span></div>';
    host.appendChild(el);

    function row(k, icon, label) {
      return '<div class="bar-row ' + k + '" id="st-r-' + k + '" title="' + label + '"><span class="g-ico">' + I(icon) + '</span>' +
        '<div class="hb"><i id="st-b-' + k + '"></i></div><span class="v" id="st-v-' + k + '">0</span></div>';
    }
    function $(id) { return document.getElementById(id); }
    function txt(node, t) { if (node.textContent !== t) node.textContent = t; }
    var money = host.querySelector(".st-money");

    S.sub("identity", function (v) {
      el.hidden = false;
      txt($("st-name"), v.name || "");
      txt($("st-id"), String(v.id));
      var parts = (v.name || "?").split(" ");
      txt($("st-av"), (parts[0][0] || "?") + (parts[1] ? parts[1][0] : ""));
    });
    S.sub("clock", function (c) {
      txt($("st-time"), pad(c.h) + ":" + pad(c.m));
      txt($("st-date"), c.d ? pad(c.d) + "/" + pad(c.mo) + "/" + c.y : "");
    });
    function pad(n) { return (n < 10 ? "0" : "") + n; }

    S.sub("status", function (s) {
      el.hidden = false;
      setBar("hp", s.hp); setBar("armor", s.armor); setBar("hunger", s.hunger); setBar("thirst", s.thirst);
    });
    function setBar(k, v) {
      var r = $("st-r-" + k);
      r.hidden = v < 0;                     // indicatore non gestito dal server: non si mostra
      if (v < 0) return;
      v = Math.max(0, Math.min(100, v));
      $("st-b-" + k).style.width = v + "%";
      txt($("st-v-" + k), String(v));
      r.classList.toggle("low", v <= 20);
    }
    S.sub("money", function (m, prev) {
      el.hidden = false;
      txt($("st-cash"), U.money(m.cash));
      txt($("st-bank"), U.money(m.bank));
    });
    S.sub("level", function (l) {
      txt($("st-lv"), T.level + " " + l.level);
      if (l.expStep > 0) {
        $("st-xpbar").style.width = U.pct(l.expIn, l.expStep) + "%";
        $("st-xp").innerHTML = "<b>" + U.fmt(l.expIn) + "</b> / " + U.fmt(l.expStep);
      } else {
        $("st-xpbar").style.width = "100%";
        txt($("st-xp"), T.maxLevel);
      }
    });

    // +$250 / -$120: solo quando il server comunica un movimento reale (lssc:money_delta)
    HUD.moneyDelta = function (delta, bank) {
      if (!delta) return;
      var d = document.createElement("div");
      d.className = "money-delta " + (delta > 0 ? "plus" : "minus");
      d.style.top = bank ? "auto" : "";
      if (bank) d.style.bottom = "-1.25rem";
      d.textContent = (delta > 0 ? "+" : "-") + U.money(Math.abs(delta));
      money.appendChild(d);
      setTimeout(function () { if (d.parentNode) d.parentNode.removeChild(d); }, 1700);
    };
  };
})();
