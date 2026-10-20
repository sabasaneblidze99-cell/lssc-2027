/* chars.js - vista "chars": tre personaggi per account (M15).
   server -> pagina  lssc:chars_begin slots
                     lssc:chars_add slot used name level cash skin lastText   (una per slot)
                     lssc:chars_new slot                                    (mostra il modulo di creazione)
                     lssc:chars_err code   1 nome non valido, 2 nome occupato, 3 data non valida, 4 riprova
                     lssc:open chars / lssc:close chars
   pagina -> server  lssc:char_pick slot | lssc:char_create slot Nome Cognome giorno mese anno | lssc:char_back
   Nomi: solo lettere latine (gli argomenti samp-cef sono separati da spazi). Il server rivalida tutto. */
(function () {
  "use strict";
  var LSSC = window.LSSC, U = LSSC.util, HUD = window.HUD;
  var root = document.createElement("div");
  root.id = "view-chars"; root.hidden = true;
  document.body.appendChild(root);

  var slots = [], mode = "list", newSlot = -1, busy = false, err = "";
  var ERR = { 1: "სახელი და გვარი: მხოლოდ ლათინური ასოები, 2-დან 20 სიმბოლომდე, დიდი ასოთი.", 2: "ეს სახელი უკვე დაკავებულია. აირჩიე სხვა.",
              3: "დაბადების თარიღი არასწორია (ასაკი 18-დან 80 წლამდე).", 4: "ვერ მოხერხდა, სცადე თავიდან." };
  function icon(n) { return HUD && HUD.icon ? HUD.icon(n) : ""; }
  function el(tag, cls, text) { var e = document.createElement(tag); if (cls) e.className = cls; if (text !== undefined) e.textContent = text; return e; }

  LSSC.on("lssc:chars_begin", function (n) { slots = []; for (var i = 0; i < Math.max(1, U.int(n) || 3); i++) slots.push({ used: 0 }); mode = "list"; busy = false; err = ""; });
  LSSC.on("lssc:chars_add", function (s, used, name, level, cash, skin, last) {
    s = U.int(s); if (s < 0 || s >= slots.length) return;
    slots[s] = { used: U.int(used), name: U.str(name), level: U.int(level), cash: U.int(cash), skin: U.int(skin), last: U.str(last) };
    if (!root.hidden) render();
  });
  LSSC.on("lssc:chars_new", function (s) { newSlot = U.int(s); mode = "new"; busy = false; err = ""; if (!root.hidden) render(); });
  LSSC.on("lssc:chars_err", function (c) { busy = false; err = ERR[U.int(c)] || ERR[4]; if (!root.hidden) render(); });

  function renderList(wrap) {
    wrap.appendChild(el("div", "cs-title", "შენი პერსონაჟები"));
    wrap.appendChild(el("div", "cs-lead", "აირჩიე, ვისით ითამაშებ, ან შექმენი ახალი."));
    var grid = el("div", "cs-slots");
    slots.forEach(function (c, i) {
      var b = el("button", "cs-slot" + (c.used ? "" : " empty")); b.type = "button";
      b.appendChild(el("span", "cs-num", String(i + 1)));
      if (c.used) {
        var bd = el("span", "cs-badge"); bd.innerHTML = icon("people"); b.appendChild(bd);
        b.appendChild(el("span", "cs-name", c.name.replace(/_/g, " ")));
        var meta = el("span", "cs-meta");
        var lv = el("span", "", String(c.level)); lv.insertBefore(el("span", "", "დონე"), lv.firstChild);
        var ca = el("span", "", U.money(c.cash)); ca.insertBefore(el("span", "", "ფული"), ca.firstChild);
        meta.appendChild(lv); meta.appendChild(ca); b.appendChild(meta);
        if (c.last) b.appendChild(el("span", "cs-last", "ბოლოს: " + c.last));
        b.addEventListener("click", function () { if (busy) return; busy = true; LSSC.emit("lssc:char_pick", i); render(); });
      } else {
        b.appendChild(el("span", "cs-plus", "+"));
        b.appendChild(el("span", "", "ახალი პერსონაჟი"));
        b.addEventListener("click", function () { if (busy) return; newSlot = i; mode = "new"; err = ""; render(); });
      }
      if (busy) b.disabled = true;
      grid.appendChild(b);
    });
    wrap.appendChild(grid);
    wrap.appendChild(el("div", "cs-hint", "თითოეულ პერსონაჟს აქვს საკუთარი ფული, სახლი, მანქანა და ისტორია."));
  }

  function cap(v) { v = v.replace(/[^A-Za-z]/g, "").slice(0, 20); return v ? v.charAt(0).toUpperCase() + v.slice(1).toLowerCase() : ""; }
  function renderNew(wrap) {
    var hasChars = slots.some(function (c) { return c.used; });
    wrap.appendChild(el("div", "cs-title", "ახალი პერსონაჟი"));
    wrap.appendChild(el("div", "cs-lead", "სლოტი " + (newSlot + 1) + ". სახელი ჩანს ქალაქში ყველასთვის, შემდეგ ვეღარ შეცვლი."));
    var f = el("form", "cs-form"); f.autocomplete = "off";
    function field(id, label, ph, extra) {
      var d = el("div", "cs-field"); var l = el("label", "", label); l.htmlFor = id;
      var i = document.createElement("input"); i.id = id; i.placeholder = ph; i.spellcheck = false;
      if (extra) for (var k in extra) i.setAttribute(k, extra[k]);
      d.appendChild(l); d.appendChild(i); return d;
    }
    var r1 = el("div", "cs-row");
    r1.appendChild(field("cs-first", "სახელი (ლათინურად)", "Giorgi", { maxlength: 20 }));
    r1.appendChild(field("cs-last", "გვარი (ლათინურად)", "Beridze", { maxlength: 20 }));
    var r2 = el("div", "cs-row date");
    r2.appendChild(field("cs-d", "დღე", "14", { inputmode: "numeric", maxlength: 2 }));
    r2.appendChild(field("cs-m", "თვე", "05", { inputmode: "numeric", maxlength: 2 }));
    r2.appendChild(field("cs-y", "წელი", "1998", { inputmode: "numeric", maxlength: 4 }));
    f.appendChild(r1); f.appendChild(r2);
    var pv = el("div", "cs-preview"); f.appendChild(pv);
    f.appendChild(el("div", "cs-err", err));
    var act = el("div", "cs-actions");
    if (hasChars) {
      var back = el("button", "cs-btn", "უკან"); back.type = "button";
      back.addEventListener("click", function () { if (busy) return; mode = "list"; err = ""; LSSC.emit("lssc:char_back"); render(); });
      act.appendChild(back);
    }
    var go = el("button", "cs-btn go", "შემდეგი"); go.type = "submit"; act.appendChild(go);
    f.appendChild(act);
    wrap.appendChild(f);
    var fi = f.querySelector("#cs-first"), la = f.querySelector("#cs-last");
    function upd() {
      fi.value = cap(fi.value); la.value = cap(la.value);
      ["cs-d", "cs-m", "cs-y"].forEach(function (id) { var x = f.querySelector("#" + id); x.value = x.value.replace(/\D/g, ""); });
      pv.textContent = fi.value && la.value ? fi.value + "_" + la.value : "";
      go.disabled = busy || fi.value.length < 2 || la.value.length < 2 || !f.querySelector("#cs-y").value;
    }
    f.addEventListener("input", upd);
    f.addEventListener("submit", function (ev) {
      ev.preventDefault(); upd(); if (go.disabled) return;
      var d = U.int(f.querySelector("#cs-d").value), m = U.int(f.querySelector("#cs-m").value), y = U.int(f.querySelector("#cs-y").value);
      if (fi.value.length + la.value.length + 1 > 20) { err = ERR[1]; render(); return; }
      if (!d || !m || y < 1000) { err = ERR[3]; render(); return; }
      busy = true; err = "";
      LSSC.emit("lssc:char_create", newSlot, fi.value, la.value, d, m, y);
      go.disabled = true; f.querySelector(".cs-err").textContent = "";
    });
    upd();
    setTimeout(function () { fi.focus(); }, 0);
  }

  function render() {
    var keep = {};
    ["cs-first", "cs-last", "cs-d", "cs-m", "cs-y"].forEach(function (id) { var x = document.getElementById(id); if (x) keep[id] = x.value; });
    root.textContent = "";
    var wrap = el("div", "cs-wrap");
    if (mode === "new") renderNew(wrap); else renderList(wrap);
    root.appendChild(wrap);
    for (var id in keep) { var x = document.getElementById(id); if (x) { x.value = keep[id]; x.dispatchEvent(new Event("input", { bubbles: true })); } }
  }

  LSSC.registerView("chars", {
    show: function () { busy = false; render(); root.hidden = false; LSSC.emit("lssc:view_ok", "chars"); },
    hide: function () { root.hidden = true; },
    onEscape: function () { if (mode === "new" && slots.some(function (c) { return c.used; }) && !busy) { mode = "list"; err = ""; LSSC.emit("lssc:char_back"); render(); } return true; }
  });
})();
