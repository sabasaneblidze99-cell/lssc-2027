/* bridge.js - unico punto di contatto con samp-cef (window.cef: on/off/emit/set_focus/hide).
   La pagina NON decide nulla di gioco: mostra i dati ricevuti e invia richieste che il server rivalida.
   Senza window.cef (browser normale / preview.html) usa postMessage verso la finestra madre. */
(function () {
  "use strict";

  var LSSC = window.LSSC = window.LSSC || {};
  var handlers = {};
  var hasCef = typeof window.cef === "object" && window.cef !== null && typeof window.cef.on === "function";

  // Dispatch locale: ogni evento arriva come lista di argomenti (string/int/float, come cef_emit_event)
  function dispatch(name, args) {
    var list = handlers[name];
    if (!list) return;
    for (var i = 0; i < list.length; i++) {
      try { list[i].apply(null, args); }
      catch (e) { console.error("[lssc] errore in " + name + ":", e); }
    }
  }

  // Registra un evento server->pagina. Con samp-cef si sottoscrive una sola volta per nome.
  LSSC.on = function (name, fn) {
    if (!handlers[name]) {
      handlers[name] = [];
      if (hasCef) window.cef.on(name, function () { dispatch(name, Array.prototype.slice.call(arguments)); });
    }
    handlers[name].push(fn);
  };

  // Pagina->server. ATTENZIONE: sul server gli argomenti arrivano come UNA stringa separata da spazi,
  // quindi qui si passano solo numeri o parole senza spazi.
  LSSC.emit = function (name) {
    var args = Array.prototype.slice.call(arguments, 1).map(function (a) {
      if (typeof a === "number") return Math.trunc(a);
      return String(a).replace(/\s+/g, "_");
    });
    if (hasCef) window.cef.emit.apply(window.cef, [name].concat(args));
    else if (window.parent !== window) window.parent.postMessage({ lssc: "emit", name: name, args: args }, "*");
    else console.debug("[lssc] emit", name, args);
  };

  // Focus = mouse/tastiera alla pagina. Richiesto solo dalle viste interattive (vedi PROTOCOLLO.md).
  LSSC.focus = function (on) {
    if (hasCef) window.cef.set_focus(!!on);
    else if (window.parent !== window) window.parent.postMessage({ lssc: "focus", on: !!on }, "*");
  };

  LSSC.hasCef = hasCef;

  // Fallback anteprima: eventi simulati dalla finestra madre
  if (!hasCef) {
    window.addEventListener("message", function (ev) {
      var d = ev.data;
      if (d && d.lssc === "event" && typeof d.name === "string") dispatch(d.name, Array.isArray(d.args) ? d.args : []);
    });
  }

  // ---------------------------------------------------------------- utilita' di visualizzazione condivise
  var U = LSSC.util = {};

  U.int = function (v, def) { var n = Number(v); return isFinite(n) ? Math.trunc(n) : (def || 0); };
  U.str = function (v) { return v === undefined || v === null ? "" : String(v); };

  // 12450 -> "12.450" (come UI_LvFmt)
  U.fmt = function (v) {
    var n = U.int(v), neg = n < 0, s = String(Math.abs(n)), out = "";
    for (var i = 0; i < s.length; i++) {
      if (i > 0 && (s.length - i) % 3 === 0) out += ".";
      out += s[i];
    }
    return (neg ? "-" : "") + out;
  };
  // denaro: il server manda sempre TETRI interi (1 ლარი = 100 თეთრი). 125050 -> "₾1.250,50", 8500000 -> "₾85.000".
  // Solo visualizzazione: il saldo vero e' quello del server.
  U.money = function (v) {
    var n = U.int(v), neg = n < 0, a = Math.abs(n), lari = Math.floor(a / 100), c = a % 100;
    return (neg ? "-" : "") + "\u20BE" + U.fmt(lari) + (c ? "," + (c < 10 ? "0" : "") + c : "");
  };
  // lari digitati -> tetri interi ("12", "12,5", "12.50"); NaN se non valido. Il server rivalida comunque.
  U.parseMoney = function (str) {
    var m = /^\s*(\d{1,8})(?:[.,](\d{1,2}))?\s*$/.exec(U.str(str));
    if (!m) return NaN;
    return Number(m[1]) * 100 + (m[2] ? Number(m[2].length === 1 ? m[2] + "0" : m[2]) : 0);
  };

  // secondi -> "mm:ss"; negativo -> "--:--" (come UI_LvFmtTime)
  U.time = function (sec) {
    sec = U.int(sec, -1);
    if (sec < 0) return "--:--";
    var m = Math.floor(sec / 60), s = sec % 60;
    return (m < 10 ? "0" : "") + m + ":" + (s < 10 ? "0" : "") + s;
  };

  // metri -> "420 m" / "1,5 km" (come UI_LvFmtDist)
  U.dist = function (m) {
    m = Math.max(0, U.int(m));
    if (m < 1000) return m + " მ";
    return Math.floor(m / 1000) + "," + Math.floor((m % 1000) / 100) + " კმ";
  };

  // secondi fa (come UI_LvFmtAgo)
  U.ago = function (s) {
    s = Math.max(0, U.int(s));
    if (s < 60) return "ახლა";
    if (s < 3600) return Math.floor(s / 60) + " წთ წინ";
    if (s < 86400) return Math.floor(s / 3600) + " სთ წინ";
    return Math.floor(s / 86400) + " დღის წინ";
  };

  U.pct = function (v, max) {
    max = U.int(max) || 100;
    return Math.max(0, Math.min(100, U.int(v) * 100 / max));
  };

  U.el = function (id) { return document.getElementById(id); };
  U.esc = function (s) {
    return U.str(s).replace(/[&<>"']/g, function (c) {
      return { "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c];
    });
  };

  // Tabelle di sola visualizzazione: stessi testi di modules/ui/living_ui.inc
  LSSC.names = {
    district: ["ყველა", "Idlewood", "Ganton", "Jefferson"],
    cargo: ["სხვადასხვა ტვირთი", "ზოგადი ტვირთი", "სურსათი", "სამშენებლო მასალები"],
    kind: ["ყველა", "ტვირთის გადაზიდვა"],
    track: ["", "ტრანსპორტირება", "ტაქსი"],
    tier: ["დამწყები", "სანდო", "გამოცდილი", "ელიტა"],
    block: [
      "",
      "არასაკმარისი დონე",
      "ტრანსპორტირებაში მეტი რეპუტაცია გჭირდება",
      "უბნის მეტი ნდობა გჭირდება",
      "უკვე გაქვს აქტიური კონტრაქტი",
      "უკვე სხვა სამსახურით ხარ დაკავებული",
      "შეისვენე და ცოტა ხანში დაბრუნდი",
      "ამ საათში უკვე ბევრი იმუშავე",
      "ახლახან თქვი უარი: მოგვიანებით სცადე",
      "სხვა მძღოლმა უკვე აიღო",
      "ამჟამად მიუწვდომელია"
    ],
    prjBlock: [
      "",
      "შეწირვას ქალაქში ცოტა ხნის გატარების შემდეგ შეძლებ.",
      "დღეს უკვე მაქსიმუმი შეწირე: დიდი მადლობა.",
      "ეს პროექტი თანხას აღარ აგროვებს.",
      "შემოწირულობა მიიღება უბნის სახლში ან დაფასთან."
    ],
    event: [
      "მოვლენა", "მიწოდება დასრულდა", "მიწოდება ჩაიშალა", "ტაქსით მგზავრობა", "ქირაობა დაიწყო",
      "ნაქირავები დაბრუნდა", "დანაშაულის შეტყობინება", "პატრულმა ჩაიარა", "საზოგადოებრივი პროექტი", "ადმინისტრაციის ჩარევა"
    ]
  };
  U.name = function (table, idx) {
    var t = LSSC.names[table], i = U.int(idx);
    return (t && i >= 0 && i < t.length) ? t[i] : t[0];
  };

  // ---------------------------------------------------------------- gestione viste e focus
  // Viste interattive: "board", "phone". L'HUD non e' una vista e non chiede mai il focus.
  var views = {};
  var current = null;

  LSSC.registerView = function (name, api) { views[name] = api; };
  LSSC.currentView = function () { return current; };

  LSSC.openView = function (name, arg) {
    if (!views[name]) { console.warn("[lssc] vista sconosciuta:", name); return; }
    if (current && current !== name) views[current].hide();
    current = name;
    views[name].show(arg);
    document.body.classList.add("view-open");
    LSSC.focus(true);
  };

  // fromPage = chiusura chiesta dal giocatore (ESC/X): si avvisa il server
  LSSC.closeView = function (fromPage) {
    if (!current) return;
    var name = current;
    views[name].hide();
    current = null;
    document.body.classList.remove("view-open");
    LSSC.focus(false);
    if (fromPage) LSSC.emit("lssc:close", name);
  };

  LSSC.on("lssc:open", function (view, arg) { LSSC.openView(U.str(view), U.str(arg)); });
  LSSC.on("lssc:close", function (view) {
    view = U.str(view);
    if (!view || view === current) LSSC.closeView(false);
  });

  // ESC: prima chiude la conferma aperta, poi la vista. Funziona solo con il focus (vista interattiva aperta).
  document.addEventListener("keydown", function (ev) {
    if (!current) return;
    if (ev.key === "Escape") {
      ev.preventDefault();
      var v = views[current];
      if (v.onEscape && v.onEscape()) return;
      LSSC.closeView(true);
    } else if (ev.key === "Backspace" && !/INPUT|TEXTAREA/.test(document.activeElement.tagName)) {
      var b = views[current];
      if (b.onBack) { ev.preventDefault(); b.onBack(); }
    }
  });

  // Conferma generica (riusata da bacheca e telefono). Ritorna l'API per chiuderla.
  LSSC.confirm = function (host, opts) {
    var wrap = document.createElement("div");
    wrap.className = "confirm";
    wrap.innerHTML =
      '<div class="confirm-box glass" role="dialog" aria-modal="true">' +
        '<div class="confirm-title"></div><div class="confirm-text"></div>' +
        '<div class="confirm-actions"><button class="btn" data-a="no"></button><button class="btn btn-primary" data-a="yes"></button></div>' +
      "</div>";
    wrap.querySelector(".confirm-title").textContent = opts.title;
    wrap.querySelector(".confirm-text").innerHTML = opts.html;
    wrap.querySelector('[data-a="no"]').textContent = opts.no || "გაუქმება";
    var yes = wrap.querySelector('[data-a="yes"]');
    yes.textContent = opts.yes || "დადასტურება";
    if (opts.danger) yes.className = "btn btn-danger";
    var api = {
      close: function () { if (wrap.parentNode) wrap.parentNode.removeChild(wrap); api.open = false; },
      open: true
    };
    wrap.addEventListener("click", function (ev) {
      var a = ev.target.getAttribute && ev.target.getAttribute("data-a");
      if (a === "no") { api.close(); if (opts.onNo) opts.onNo(); }
      else if (a === "yes") { api.close(); opts.onYes(); }
    });
    host.appendChild(wrap);
    yes.focus();
    return api;
  };
})();
