/* spawn.js - vista "spawn": scelta del punto di spawn dopo il login.
   server -> pagina  lssc:spawn_begin forced seconds   (forced 1 = uscito in combattimento: solo l'ultima posizione)
                     lssc:spawn_add idx icon title sub (una per opzione, nell'ordine)
                     lssc:open spawn / lssc:close spawn
   pagina -> server  lssc:spawn_pick idx
   Testo solo con textContent. ESC non chiude: la scelta e' obbligatoria (dopo il tempo il server sceglie la prima). */
(function () {
  "use strict";
  var LSSC = window.LSSC, U = LSSC.util, HUD = window.HUD;
  var root = document.createElement("div");
  root.id = "view-spawn"; root.hidden = true;
  document.body.appendChild(root);

  var opts = [], forced = 0, seconds = 90, left = 0, tick = 0, sel = 0, sent = false;
  function icon(n) { return HUD && HUD.icon ? HUD.icon(n) : ""; }

  LSSC.on("lssc:spawn_begin", function (f, s) { opts = []; forced = U.int(f); seconds = Math.max(5, U.int(s) || 90); });
  LSSC.on("lssc:spawn_add", function (idx, ic, title, sub) {
    opts.push({ idx: U.int(idx), icon: U.str(ic), title: U.str(title), sub: U.str(sub) });
  });

  function render() {
    root.textContent = "";
    var wrap = document.createElement("div"); wrap.className = "sp-wrap";
    var head = document.createElement("div"); head.className = "sp-head";
    var t = document.createElement("div"); t.className = "sp-title";
    t.textContent = forced ? "დაბრუნება ქალაქში" : "სად გინდა გაჩნდე?";
    var sm = document.createElement("small");
    sm.textContent = forced ? "ბრძოლის დროს გახვედი" : "აირჩიე ადგილი, საიდანაც დღეს დაიწყებ";
    t.appendChild(sm);
    var tm = document.createElement("div"); tm.className = "sp-timer"; tm.id = "sp-timer";
    head.appendChild(t); head.appendChild(tm); wrap.appendChild(head);
    if (forced) {
      var fb = document.createElement("div"); fb.className = "sp-forced";
      fb.innerHTML = icon("alert");
      var fs = document.createElement("span");
      fs.textContent = "პოლიციასთან ბრძოლის დროს გახვედი თამაშიდან. 3 საათის განმავლობაში ბრუნდები იქ, სადაც იყავი.";
      fb.appendChild(fs); wrap.appendChild(fb);
    }
    var list = document.createElement("div"); list.className = "sp-list";
    opts.forEach(function (o, i) {
      var b = document.createElement("button"); b.type = "button"; b.className = "sp-card" + (i === sel ? " sel" : "");
      b.dataset.icon = o.icon;
      b.innerHTML = '<span class="sp-ico">' + icon(o.icon) + "</span>";
      var k = document.createElement("span"); k.className = "sp-key"; k.textContent = String(i + 1);
      var n = document.createElement("span"); n.className = "sp-name"; n.textContent = o.title;
      var s = document.createElement("span"); s.className = "sp-sub"; s.textContent = o.sub;
      b.appendChild(k); b.appendChild(n); b.appendChild(s);
      b.addEventListener("mouseenter", function () { select(i); });
      b.addEventListener("click", function () { pick(i); });
      list.appendChild(b);
    });
    wrap.appendChild(list);
    var h = document.createElement("div"); h.className = "sp-hint";
    h.textContent = opts.length > 1 ? "დააჭირე ბარათს, ციფრს 1-" + opts.length + " ან Enter" : "დააჭირე ბარათს ან Enter";
    wrap.appendChild(h);
    root.appendChild(wrap);
    upTimer();
  }
  function select(i) {
    sel = i;
    var cards = root.querySelectorAll(".sp-card");
    for (var c = 0; c < cards.length; c++) cards[c].classList.toggle("sel", c === i);
  }
  function upTimer() {
    var el = document.getElementById("sp-timer");
    if (el) el.textContent = left > 0 ? Math.floor(left / 60) + ":" + (left % 60 < 10 ? "0" : "") + (left % 60) : "";
  }
  function pick(i) {
    if (sent || !opts[i]) return;
    sent = true;
    root.querySelectorAll(".sp-card").forEach(function (b) { b.disabled = true; });
    LSSC.emit("lssc:spawn_pick", opts[i].idx);
  }
  document.addEventListener("keydown", function (ev) {
    if (root.hidden || sent) return;
    var n = parseInt(ev.key, 10);
    if (n >= 1 && n <= opts.length) { ev.preventDefault(); select(n - 1); pick(n - 1); }
    else if (ev.key === "ArrowRight" || ev.key === "ArrowDown") { ev.preventDefault(); select((sel + 1) % opts.length); }
    else if (ev.key === "ArrowLeft" || ev.key === "ArrowUp") { ev.preventDefault(); select((sel + opts.length - 1) % opts.length); }
    else if (ev.key === "Enter") { ev.preventDefault(); pick(sel); }
  });

  LSSC.registerView("spawn", {
    show: function () {
      sel = 0; sent = false; left = seconds;
      render();
      root.hidden = false;
      if (tick) clearInterval(tick);
      tick = setInterval(function () { if (left > 0) { left--; upTimer(); } }, 1000);
    },
    hide: function () { root.hidden = true; if (tick) { clearInterval(tick); tick = 0; } },
    onEscape: function () { return true; }    // la scelta non si annulla
  });
})();
