/* chat.js - chat roleplay CEF (sinistra). Adapter graduale: e' attiva solo se il server la abilita (lssc:chat_mode 1);
   in quel caso il server manda qui i messaggi INVECE che alla chat di SA-MP (nessun doppione).
   Testo dal server: categoria + testo UTF-8 (anche georgiano), con eventuali codici colore SA-MP {RRGGBB}.
   Mai innerHTML con testo del server/giocatore: solo textContent (niente HTML iniettato).
   Apertura input: F6 (serve cef_always_listen_keys lato server) oppure evento lssc:chat_open. */
(function () {
  "use strict";
  var HUD = window.HUD, S = HUD.state, T = HUD.S, I = HUD.icon;
  var MAX_LINES = 120, MAX_INPUT = 120, FADE_MS = 30000;

  HUD.modules.chat = function (host) {
    var el = document.createElement("section");
    el.className = "ch";
    el.hidden = true;
    // le schede compaiono quando arriva il primo messaggio di quella categoria (solo sistemi reali del server)
    var ALWAYS = { all: 1, ic: 1, ooc: 1, system: 1 };
    var tabs = T.chatTabs.map(function (t) {
      return '<span class="ch-tab' + (t[0] === "all" ? " on" : "") + '" data-f="' + t[0] + '"' + (ALWAYS[t[0]] ? "" : " hidden") + ">" + t[1] + "</span>";
    }).join("");
    el.innerHTML =
      '<div class="ch-box"><div class="ch-tabs">' + tabs + '</div><div class="ch-lines" id="ch-lines" aria-live="polite"></div></div>' +
      '<div class="ch-input"><span class="g-ico">' + I("send") + '</span><input id="ch-in" maxlength="' + MAX_INPUT + '" autocomplete="off" spellcheck="false"><span class="cnt" id="ch-cnt"></span></div>' +
      '<div class="ch-hint">' + T.chatHint + "</div>";
    host.appendChild(el);
    var box = el.querySelector("#ch-lines"), input = el.querySelector("#ch-in"), cnt = el.querySelector("#ch-cnt");
    input.placeholder = T.chatPlaceholder;
    var filter = "all", open = false, fadeTimer = 0, history = [], hpos = -1;

    S.sub("chatMode", function (on) { el.hidden = !on; if (!on) close(); });
    S.sub("settings", function (s) {
      document.documentElement.style.setProperty("--chat-lines", s.chatLines);
      document.documentElement.style.setProperty("--chat-opacity", (s.chatOpacity / 100).toFixed(2));
      el.classList.toggle("ts-on", !!s.timestamps);
      for (var i = 0; i < box.children.length; i++) box.children[i].querySelector(".ts").hidden = !s.timestamps;
      scheduleFade();
    });

    function pad(n) { return (n < 10 ? "0" : "") + n; }
    // "{FF0000}ciao {FFFFFF}mondo" -> span colorati (solo colori, nessun HTML)
    function appendColored(node, text) {
      var re = /\{([0-9A-Fa-f]{6})\}/g, last = 0, color = null, m;
      while ((m = re.exec(text)) !== null) {
        if (m.index > last) seg(text.slice(last, m.index));
        color = "#" + m[1];
        last = re.lastIndex;
      }
      if (last < text.length) seg(text.slice(last));
      function seg(s) {
        var sp = document.createElement("span");
        if (color) sp.style.color = color;
        sp.textContent = s;
        node.appendChild(sp);
      }
    }

    HUD.chat = {
      add: function (cat, text) {
        var tab = el.querySelector('.ch-tab[data-f="' + cat + '"]');
        if (tab) tab.hidden = false;
        var line = document.createElement("div");
        line.className = "ch-line c-" + cat;
        line.dataset.cat = cat;
        var d = new Date();
        var ts = document.createElement("span");
        ts.className = "ts"; ts.textContent = "[" + pad(d.getHours()) + ":" + pad(d.getMinutes()) + "]";
        ts.hidden = !HUD.settings.get("timestamps");
        line.appendChild(ts);
        var tag = T.chatTag[cat];
        if (tag && text.charAt(0) !== "[") {
          var tg = document.createElement("span"); tg.className = "tag"; tg.textContent = tag; line.appendChild(tg);
        }
        appendColored(line, text);
        line.hidden = filter !== "all" && filter !== cat;
        var atBottom = box.scrollTop + box.clientHeight >= box.scrollHeight - 4;
        box.appendChild(line);
        while (box.children.length > MAX_LINES) box.removeChild(box.firstChild);
        if (!open || atBottom) box.scrollTop = box.scrollHeight;
        el.classList.remove("faded");
        scheduleFade();
      },
      clear: function () { box.textContent = ""; },
      open: openInput,
      close: function () { close(); }
    };

    function scheduleFade() {
      if (fadeTimer) clearTimeout(fadeTimer);
      fadeTimer = 0;
      if (HUD.settings.get("chatFade")) fadeTimer = setTimeout(function () { el.classList.add("faded"); }, FADE_MS);
      else el.classList.remove("faded");
    }

    el.querySelector(".ch-tabs").addEventListener("click", function (ev) {
      var f = ev.target.getAttribute("data-f");
      if (!f) return;
      filter = f;
      el.querySelectorAll(".ch-tab").forEach(function (t) { t.classList.toggle("on", t.getAttribute("data-f") === f); });
      for (var i = 0; i < box.children.length; i++) {
        var c = box.children[i].dataset.cat;
        box.children[i].hidden = f !== "all" && f !== c;
      }
      box.scrollTop = box.scrollHeight;
      input.focus();
    });

    function openInput(prefill) {
      if (el.hidden || open) return;
      if (!HUD.takeFocus("chat")) return;
      open = true;
      el.classList.add("open");
      el.classList.remove("faded");
      input.value = prefill || "";
      upd();
      setTimeout(function () { input.focus(); }, 0);
      box.scrollTop = box.scrollHeight;
    }
    function close() {
      if (!open) return;
      open = false;
      el.classList.remove("open");
      input.blur();
      HUD.releaseFocus("chat");
      scheduleFade();
    }
    function upd() { cnt.textContent = input.value.length ? input.value.length + "/" + MAX_INPUT : ""; }
    input.addEventListener("input", upd);
    input.addEventListener("keydown", function (ev) {
      if (ev.key === "Enter") {
        ev.preventDefault();
        var t = input.value.replace(/\s+/g, " ").trim();
        if (t) {
          HUD.emit("lssc:chat_send", HUD.hex(t));   // il server valida (lunghezza, mute, comandi)
          history.unshift(t); if (history.length > 20) history.pop();
        }
        hpos = -1;
        close();
      } else if (ev.key === "Escape") { ev.preventDefault(); close(); }
      else if (ev.key === "PageUp") { ev.preventDefault(); box.scrollTop -= box.clientHeight * 0.8; }
      else if (ev.key === "PageDown") { ev.preventDefault(); box.scrollTop += box.clientHeight * 0.8; }
      else if (ev.key === "ArrowUp" && history.length) { ev.preventDefault(); hpos = Math.min(history.length - 1, hpos + 1); input.value = history[hpos]; upd(); }
      else if (ev.key === "ArrowDown") { ev.preventDefault(); hpos = Math.max(-1, hpos - 1); input.value = hpos >= 0 ? history[hpos] : ""; upd(); }
      ev.stopPropagation();
    });

    HUD.onKey(function (ev) {
      if (open || HUD.focusOwner()) return false;
      if (ev.key === "F6") { openInput(""); return true; }
      return false;
    });
    HUD.on("lssc:chat_open", function (prefill) { openInput(HUD.util.str(prefill)); });
  };
})();
