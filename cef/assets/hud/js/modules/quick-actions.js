/* quick-actions.js - menu azioni rapide (si apre solo su richiesta: tasto N lato server -> lssc:quick_open mask)
   e impostazioni dell'HUD. I pulsanti disponibili li decide il server con la maschera di bit. */
(function () {
  "use strict";
  var HUD = window.HUD, T = HUD.S, I = HUD.icon;

  // bit della maschera -> id inviato al server con lssc:quick
  var ACTIONS = [
    { id: "inventory", bit: 0, icon: "backpack" },
    { id: "phone", bit: 1, icon: "phone" },
    { id: "map", bit: 2, icon: "map" },
    { id: "anims", bit: 3, icon: "run" },
    { id: "vehicles", bit: 4, icon: "car" },
    { id: "settings", bit: -1, icon: "gear" }      // sempre disponibile: e' solo della pagina
  ];

  HUD.modules.quick = function (host) {
    var ov = document.createElement("div");
    ov.className = "hud-overlay";
    ov.hidden = true;
    host.appendChild(ov);

    var qa = document.createElement("section");
    qa.className = "g-panel qa anim-pop";
    qa.innerHTML = '<h2>' + T.quickTitle + '</h2><p class="sub">' + T.quickSub + '</p><div class="qa-grid">' +
      ACTIONS.map(function (a) {
        return '<button class="qa-btn" data-a="' + a.id + '" data-bit="' + a.bit + '">' + I(a.icon) + "<span>" + T.quick[a.id] + "</span></button>";
      }).join("") + '</div><div class="qa-foot"><span>' + T.quickFoot[0] + "</span><span>" + T.quickFoot[1] + "</span></div>";

    var st = document.createElement("section");
    st.className = "g-panel stg anim-pop";
    st.innerHTML = '<h2>' + T.settingsTitle + '</h2><p class="sub">' + T.settingsSub + "</p>" +
      range("chatLines", T.chatLines, 4, 20, 1, "") + range("chatOpacity", T.chatOpacity, 40, 100, 5, "%") +
      toggle("timestamps", T.timestamps) + toggle("chatFade", T.chatFade) + range("hudScale", T.hudScale, 85, 115, 5, "%") +
      '<div class="stg-actions"><button class="g-btn ghost" data-x="reset">' + T.reset + '</button><button class="g-btn ghost" data-x="close">' + T.close +
      '</button><button class="g-btn" data-x="save">' + T.save + "</button></div>";

    function range(k, label, min, max, step, unit) {
      return '<div class="stg-row"><label for="sg-' + k + '">' + label + '</label><input type="range" id="sg-' + k + '" data-k="' + k +
        '" min="' + min + '" max="' + max + '" step="' + step + '" data-u="' + unit + '"><span class="val" id="sv-' + k + '"></span></div>';
    }
    function toggle(k, label) { return '<div class="stg-row"><label>' + label + '</label><button class="tgl" data-t="' + k + '"></button></div>'; }

    var mode = null;   // "quick" | "settings" | null
    function show(which) {
      ov.textContent = "";
      ov.appendChild(which === "quick" ? qa : st);
      ov.hidden = false;
      mode = which;
      if (which === "settings") fill(HUD.settings.all());
      var first = (which === "quick" ? qa : st).querySelector("button:not([disabled])");
      if (first) setTimeout(function () { first.focus(); }, 0);
    }
    function hide(notify) {
      if (!mode) return;
      var was = mode;
      mode = null;
      ov.hidden = true;
      HUD.releaseFocus("quick");
      if (notify && was === "quick") HUD.emit("lssc:quick_close");
    }

    var draft = {};
    function fill(s) {
      draft = JSON.parse(JSON.stringify(s));
      st.querySelectorAll("input[type=range]").forEach(function (r) {
        r.value = draft[r.dataset.k];
        document.getElementById("sv-" + r.dataset.k).textContent = r.value + r.dataset.u;
      });
      st.querySelectorAll(".tgl").forEach(function (t) { t.classList.toggle("on", !!draft[t.dataset.t]); });
    }
    st.addEventListener("input", function (ev) {
      var r = ev.target;
      if (!r.dataset.k) return;
      draft[r.dataset.k] = Number(r.value);
      document.getElementById("sv-" + r.dataset.k).textContent = r.value + r.dataset.u;
      applyScale(draft.hudScale);   // anteprima immediata della scala
    });
    st.addEventListener("click", function (ev) {
      var b = ev.target.closest("button");
      if (!b) return;
      if (b.dataset.t) { draft[b.dataset.t] = !draft[b.dataset.t]; b.classList.toggle("on", draft[b.dataset.t]); return; }
      if (b.dataset.x === "save") { HUD.settings.save(draft); hide(false); }
      else if (b.dataset.x === "reset") { HUD.settings.reset(); fill(HUD.settings.all()); }
      else if (b.dataset.x === "close") { applyScale(HUD.settings.get("hudScale")); hide(false); }
    });
    function applyScale(v) { document.documentElement.style.setProperty("--hud-scale", (v / 100).toFixed(2)); }
    HUD.state.sub("settings", function (s) { applyScale(s.hudScale); });

    qa.addEventListener("click", function (ev) {
      var b = ev.target.closest("button[data-a]");
      if (!b || b.disabled) return;
      if (b.dataset.a === "settings") { show("settings"); return; }
      HUD.emit("lssc:quick", b.dataset.a);   // il server esegue lo stesso comando del gioco (o rifiuta)
      hide(false);
    });

    document.addEventListener("keydown", function (ev) {
      if (!mode) return;
      if (ev.key === "Escape") { ev.preventDefault(); if (mode === "settings") { applyScale(HUD.settings.get("hudScale")); hide(false); } else hide(true); }
    });

    HUD.quick = {
      open: function (mask) {
        if (mode) return;
        if (!HUD.takeFocus("quick")) return;
        qa.querySelectorAll("button[data-bit]").forEach(function (b) {
          var bit = Number(b.dataset.bit);
          b.disabled = bit >= 0 && !(mask & (1 << bit));
        });
        show("quick");
      },
      close: function (notify) { hide(notify); },
      settings: function () { if (!mode && HUD.takeFocus("quick")) show("settings"); }
    };
  };
})();
