/* auth.js - vista "auth": accesso, registrazione, genere, aspetto, storia. Musica di sottofondo ed effetti sonori.
   La pagina non decide nulla: invia la scelta e aspetta il passo successivo dal server (lssc:auth).
   Protocollo:
     server -> pagina  lssc:auth step name err n minLen maxLen   step = login | register | gender | skin | story
                       lssc:auth_skin idx skinId                 (prima di step "skin")
                       lssc:auth_story kind idx short long       kind 1 provenienza, 2 motivo (prima di step "story")
                       lssc:close auth                           fine percorso (intro o spawn): dissolvenza e musica sfumata
     pagina -> server  lssc:login hex · lssc:register hex · lssc:gender 0|1 · lssc:skin idx · lssc:story origin motive · lssc:quit
   Password in esadecimale, un byte per carattere (Latin-1, come i dialog SA-MP): gli spazi non spezzano gli argomenti.
   Musica: cef/assets/audio/login.ogg (oppure .webm / .mp3) se presente, altrimenti un tappeto sonoro generato.
   Sfondo: cef/assets/img/login.jpg se presente (una propria ripresa di Idlewood), altrimenti il tramonto in CSS. */
(function () {
  "use strict";
  var LSSC = window.LSSC, U = LSSC.util, T = LSSC.T;
  var root = U.el("view-auth");

  // ================================================================ AUDIO
  var A = (function () {
    var ctx = null, master = null, sfxBus = null, musicGain = null, musicEl = null, padTimer = 0, padOn = false;
    var muted = false;
    try { muted = window.localStorage.getItem("lssc_mute") === "1"; } catch (e) { muted = false; }

    function ac() {
      if (ctx) return ctx;
      var C = window.AudioContext || window.webkitAudioContext;
      if (!C) return null;
      ctx = new C();
      master = ctx.createGain(); master.gain.value = muted ? 0 : 1; master.connect(ctx.destination);
      sfxBus = ctx.createGain(); sfxBus.gain.value = 0.5; sfxBus.connect(master);
      musicGain = ctx.createGain(); musicGain.gain.value = 0; musicGain.connect(master);
      return ctx;
    }

    function tone(type, f1, f2, dur, vol, when) {
      var c = ac(); if (!c) return;
      var t = c.currentTime + (when || 0), o = c.createOscillator(), g = c.createGain();
      o.type = type; o.frequency.setValueAtTime(f1, t);
      if (f2) o.frequency.exponentialRampToValueAtTime(f2, t + dur);
      g.gain.setValueAtTime(0.0001, t); g.gain.exponentialRampToValueAtTime(vol, t + 0.012);
      g.gain.exponentialRampToValueAtTime(0.0001, t + dur);
      o.connect(g); g.connect(sfxBus); o.start(t); o.stop(t + dur + 0.05);
    }
    function noise(dur, fromHz, toHz, vol) {
      var c = ac(); if (!c) return;
      var len = Math.floor(c.sampleRate * dur), buf = c.createBuffer(1, len, c.sampleRate), d = buf.getChannelData(0);
      for (var i = 0; i < len; i++) d[i] = Math.random() * 2 - 1;
      var src = c.createBufferSource(), bp = c.createBiquadFilter(), g = c.createGain(), t = c.currentTime;
      src.buffer = buf; bp.type = "bandpass"; bp.Q.value = 1.2;
      bp.frequency.setValueAtTime(fromHz, t); bp.frequency.exponentialRampToValueAtTime(toHz, t + dur);
      g.gain.setValueAtTime(0.0001, t); g.gain.exponentialRampToValueAtTime(vol, t + dur * 0.4); g.gain.exponentialRampToValueAtTime(0.0001, t + dur);
      src.connect(bp); bp.connect(g); g.connect(sfxBus); src.start(t);
    }

    var sfx = {
      hover: function () { tone("sine", 1900, 0, 0.035, 0.035); },
      click: function () { tone("triangle", 720, 420, 0.09, 0.22); tone("sine", 2400, 0, 0.02, 0.05); },
      ok: function () { tone("sine", 660, 0, 0.14, 0.18); tone("sine", 990, 0, 0.22, 0.16, 0.09); },
      error: function () { tone("square", 150, 110, 0.22, 0.08); tone("sine", 90, 0, 0.25, 0.12); },
      whoosh: function () { noise(0.45, 260, 2400, 0.12); },
      type: function () { tone("sine", 1300 + Math.random() * 300, 0, 0.02, 0.02); }
    };

    // Tappeto sonoro di riserva: accordi lenti "al tramonto" (La-7, Fa maj7, Do maj7, Sol6), filtrati e con eco
    var CHORDS = [[57, 60, 64, 67], [53, 57, 60, 64], [48, 55, 59, 64], [55, 59, 62, 64]];
    function midi(n) { return 440 * Math.pow(2, (n - 69) / 12); }
    function padChord(i) {
      var c = ac(); if (!c || !padOn) return;
      var t = c.currentTime, lp = c.createBiquadFilter(), g = c.createGain(), dl = c.createDelay(), fb = c.createGain();
      lp.type = "lowpass"; lp.frequency.value = 900; lp.Q.value = 0.4;
      dl.delayTime.value = 0.42; fb.gain.value = 0.32;
      g.gain.setValueAtTime(0.0001, t); g.gain.exponentialRampToValueAtTime(0.09, t + 2.5); g.gain.exponentialRampToValueAtTime(0.0001, t + 9.5);
      lp.connect(g); g.connect(musicGain); g.connect(dl); dl.connect(fb); fb.connect(dl); dl.connect(musicGain);
      CHORDS[i % CHORDS.length].forEach(function (n) {
        [-6, 6].forEach(function (cents) {
          var o = c.createOscillator(); o.type = "sawtooth"; o.frequency.value = midi(n); o.detune.value = cents;
          o.connect(lp); o.start(t); o.stop(t + 10);
        });
      });
      var sub = c.createOscillator(), sg = c.createGain(); sub.type = "sine"; sub.frequency.value = midi(CHORDS[i % CHORDS.length][0] - 12);
      sg.gain.setValueAtTime(0.0001, t); sg.gain.exponentialRampToValueAtTime(0.12, t + 2); sg.gain.exponentialRampToValueAtTime(0.0001, t + 9.5);
      sub.connect(sg); sg.connect(musicGain); sub.start(t); sub.stop(t + 10);
      padTimer = setTimeout(function () { padChord(i + 1); }, 8000);
    }

    function startPad() { if (padOn) return; padOn = true; padChord(0); }

    function tryFiles(list) {
      if (!list.length) { startPad(); return; }
      var el = new Audio(); el.loop = true; el.preload = "auto";
      el.addEventListener("error", function () { tryFiles(list.slice(1)); }, { once: true });
      el.addEventListener("canplay", function () {
        musicEl = el;
        try { var c = ac(); c.createMediaElementSource(el).connect(musicGain); } catch (e) { /* senza WebAudio: volume dell'elemento */ el.volume = muted ? 0 : 0.6; }
        el.play().catch(function () { startPad(); });
      }, { once: true });
      el.src = list[0];
    }

    function musicStart() {
      var c = ac(); if (!c) return;
      if (c.state === "suspended") c.resume();
      musicGain.gain.cancelScheduledValues(c.currentTime);
      musicGain.gain.setValueAtTime(musicGain.gain.value, c.currentTime);
      musicGain.gain.linearRampToValueAtTime(0.6, c.currentTime + 3);
      if (musicEl) { musicEl.play().catch(function () {}); return; }
      if (padOn) return;
      tryFiles(["audio/login.ogg", "audio/login.webm", "audio/login.mp3"]);
    }

    function musicStop() {
      var c = ctx; if (!c) return;
      musicGain.gain.cancelScheduledValues(c.currentTime);
      musicGain.gain.setValueAtTime(musicGain.gain.value, c.currentTime);
      musicGain.gain.linearRampToValueAtTime(0, c.currentTime + 2.5);
      setTimeout(function () {
        padOn = false; if (padTimer) { clearTimeout(padTimer); padTimer = 0; }
        if (musicEl) musicEl.pause();
      }, 2700);
    }

    function setMuted(m) {
      muted = !!m;
      try { window.localStorage.setItem("lssc_mute", muted ? "1" : "0"); } catch (e) { /* storage non disponibile */ }
      if (master) master.gain.value = muted ? 0 : 1;
      if (musicEl && !ctx) musicEl.volume = muted ? 0 : 0.6;
    }

    return { sfx: sfx, musicStart: musicStart, musicStop: musicStop, setMuted: setMuted, isMuted: function () { return muted; } };
  })();
  LSSC.audio = A;

  // ================================================================ SCENA
  var IC = {
    male: '<svg viewBox="0 0 48 48"><circle cx="20" cy="28" r="11"/><path d="M28 20l12-12M31 8h9v9"/></svg>',
    female: '<svg viewBox="0 0 48 48"><circle cx="24" cy="18" r="11"/><path d="M24 29v14M17 37h14"/></svg>',
    sound: '<svg viewBox="0 0 24 24"><path d="M4 9v6h4l5 4V5L8 9H4z"/><path d="M16 9a4 4 0 010 6M18.5 6.5a8 8 0 010 11"/></svg>',
    mute: '<svg viewBox="0 0 24 24"><path d="M4 9v6h4l5 4V5L8 9H4z"/><path d="M17 9l5 6M22 9l-5 6"/></svg>'
  };

  function windowsHtml() {
    var out = "";
    for (var i = 0; i < 46; i++) {
      out += '<i style="left:' + (Math.random() * 100).toFixed(1) + "%;bottom:" + (Math.random() * 70 + 5).toFixed(1) +
        "%;animation-delay:-" + (Math.random() * 6).toFixed(1) + "s;animation-duration:" + (4 + Math.random() * 5).toFixed(1) + 's"></i>';
    }
    return out;
  }

  root.innerHTML =
    '<div class="au-sky"></div><div class="au-photo" id="au-photo"></div><div class="au-sun"></div><div class="au-clouds"></div>' +
    '<svg class="au-palms" viewBox="0 0 1280 720" preserveAspectRatio="xMidYMax slice" aria-hidden="true"><g fill="#07080d">' +
      '<g class="au-palm"><rect x="1088" y="360" width="9" height="360"/><path d="M1092 362 q-70 -10 -120 30 q60 -20 118 -18 q-60 10 -95 60 q55 -40 99 -40 q-20 30 -18 80 q20 -50 30 -80 q30 20 50 70 q-6 -50 -36 -82 q60 0 110 30 q-40 -40 -110 -40 q70 -10 120 10 q-50 -30 -128 -20z"/></g>' +
      '<g class="au-palm p2"><rect x="1180" y="420" width="8" height="300"/><path d="M1184 422 q-60 -8 -100 26 q50 -16 98 -14 q-46 10 -78 50 q46 -34 82 -34 q20 24 36 60 q-4 -40 -30 -64 q50 0 92 24 q-34 -34 -92 -34 q58 -8 100 8 q-42 -24 -108 -16z"/></g>' +
      '<g class="au-palm p3"><rect x="150" y="440" width="7" height="280"/><path d="M153 442 q-55 -8 -92 24 q46 -15 90 -13 q-42 9 -72 46 q42 -30 76 -31 q18 22 33 55 q-4 -37 -28 -59 q46 0 85 22 q-31 -31 -85 -31 q53 -7 92 7 q-39 -22 -99 -15z"/></g>' +
    "</g></svg>" +
    '<div class="au-city"></div><div class="au-windows">' + windowsHtml() + "</div>" +
    '<div class="au-road"><b></b><b class="r"></b><b style="animation-delay:-3.5s"></b></div><div class="au-vignette"></div>' +
    '<div class="au-brand"><div class="k">' + T.brandKicker + '</div><div class="t">STREET<br>CRIME\'S<span>' + T.brandSub + '</span></div></div>' +
    '<div class="au-progress" id="au-progress"></div>' +
    '<div class="au-stage" id="au-stage">' +
      '<div class="au-slab s1"></div><div class="au-slab s2"></div><div class="au-slab s3"></div><div class="au-slab s4"></div>' +
      '<div class="au-pane" id="au-pane"></div><div class="au-side" id="au-side"></div>' +
    "</div>" +
    '<div class="au-audio" id="au-audio"><div class="au-eq"><i></i><i></i><i></i></div><span id="au-audio-t">' + T.music + '</span><button id="au-mute" aria-label="ხმა"></button></div>' +
    '<div class="au-foot">' + T.slogan + " · " + T.enterHint + '</div>';

  var pane = U.el("au-pane"), side = U.el("au-side"), stage = U.el("au-stage"), prog = U.el("au-progress");

  // Sfondo personalizzato (img/login.jpg) se esiste
  (function () {
    var img = new Image();
    img.onload = function () { var p = U.el("au-photo"); p.style.backgroundImage = "url(img/login.jpg)"; p.classList.add("on"); };
    img.src = "img/login.jpg";
  })();

  function paintMute() {
    var m = A.isMuted();
    U.el("au-mute").innerHTML = m ? IC.mute : IC.sound;
    U.el("au-audio-t").textContent = m ? T.musicOff : T.music;
    root.classList.toggle("au-muted", m);
  }
  U.el("au-mute").addEventListener("click", function () { A.setMuted(!A.isMuted()); paintMute(); if (!A.isMuted()) A.sfx.click(); });
  paintMute();

  // Suoni su tutti i pulsanti e le carte della vista
  root.addEventListener("mouseover", function (ev) {
    var t = ev.target.closest && ev.target.closest("button, .au-card");
    if (t && !t.contains(ev.relatedTarget)) A.sfx.hover();
  });
  root.addEventListener("click", function (ev) {
    var t = ev.target.closest && ev.target.closest("button, .au-card, .au-check");
    if (t && t.id !== "au-mute" && !t.hasAttribute("data-silent")) A.sfx.click();
  });

  // ================================================================ STATO
  var S = {
    step: "", name: "", err: "", n: 0, minLen: 6, maxLen: 32, busy: false,
    skins: [], origins: {}, motives: {}, sub: "origin", origin: -1, motive: -1, gender: -1, skinIdx: -1, rules: false
  };

  function esc(s) { return U.esc(s); }
  function displayName() { return S.name.replace(/_/g, " "); }

  // Latin-1 -> esadecimale; null se ci sono caratteri fuori dal set dei dialog SA-MP
  function toHex(s) {
    var out = "";
    for (var i = 0; i < s.length; i++) {
      var c = s.charCodeAt(i);
      if (c < 32 || c > 255 || c === 127) return null;
      out += (c < 16 ? "0" : "") + c.toString(16);
    }
    return out;
  }

  function strength(pw) {
    var n = 0;
    if (pw.length >= S.minLen) n++;
    if (pw.length >= 10) n++;
    if (/[0-9]/.test(pw) && /[a-zA-Z]/.test(pw)) n++;
    if (/[^a-zA-Z0-9]/.test(pw) || /[A-Z]/.test(pw) && /[a-z]/.test(pw)) n++;
    return n;
  }

  function setWide(on) { root.classList.toggle("au-wide", on); }

  function progress(active) {
    var steps = T.steps;
    var html = steps.map(function (s, i) { return '<span class="' + (i === active ? "on" : "") + '">' + s + "</span>"; }).join("");
    prog.innerHTML = html;
    return html;
  }

  function fail(msg) {
    var e = U.el("au-err");
    if (e) { e.textContent = msg; e.classList.remove("shake"); void e.offsetWidth; e.classList.add("shake"); }
    A.sfx.error();
  }

  function busy(on, label) {
    S.busy = on;
    var b = U.el("au-go");
    if (b) { b.disabled = on; if (label) b.textContent = label; }
  }

  // ================================================================ PASSI
  var R = {};

  R.login = function () {
    setWide(false);
    pane.innerHTML =
      '<div class="au-h1">' + T.loginTitle + '</div>' +
      '<div class="au-sub">' + T.loginSub + '</div>' +
      '<div><div class="au-lbl">' + T.character + '</div><div class="au-field"><input class="au-input" readonly value="' + esc(S.name) + '"><span class="au-tag">' + T.fromLauncher + '</span></div></div>' +
      '<div><div class="au-lbl">' + T.password + '</div><div class="au-field"><input class="au-input" id="au-pw" type="password" maxlength="' + S.maxLen + '" autocomplete="off">' +
        '<button class="au-tag" id="au-eye" data-silent>' + T.show + '</button></div></div>' +
      '<div class="au-err" id="au-err">' + esc(S.err) + "</div>" +
      '<button class="au-btn" id="au-go">' + T.enterCity + '</button>' +
      '<div class="au-links"><span>' + T.attemptsLeft + S.n + '</span><button id="au-forgot" data-silent>' + T.forgot + '</button></div>';
    side.innerHTML = '<div class="q">' + T.notYourName + '</div><div class="a">' + T.notYourNameSub + '</div>' +
      '<button class="b" id="au-quit">' + T.quit + '</button><div class="au-steps">' + progress(0) + "</div>";
    var pw = U.el("au-pw");
    U.el("au-eye").onclick = function () { pw.type = pw.type === "password" ? "text" : "password"; this.textContent = pw.type === "password" ? T.show : T.hide; pw.focus(); };
    U.el("au-forgot").onclick = function () { var e = U.el("au-err"); e.style.color = "var(--muted)"; e.textContent = T.forgotHelp; };
    U.el("au-quit").onclick = function () { LSSC.emit("lssc:quit"); };
    U.el("au-go").onclick = function () {
      if (S.busy) return;
      var v = pw.value;
      if (!v.length) { fail(T.typePassword); return; }
      var hex = toHex(v);
      if (!hex) { fail(T.latinOnly); return; }
      busy(true, T.checking);
      LSSC.emit("lssc:login", hex);
      pw.value = "";
    };
    pw.addEventListener("keydown", function (ev) { if (ev.key === "Enter") U.el("au-go").click(); else if (ev.key.length === 1) A.sfx.type(); });
    setTimeout(function () { pw.focus(); }, 350);
  };

  R.register = function () {
    setWide(false);
    pane.innerHTML =
      '<div class="au-h1">' + T.registerTitle + '</div>' +
      '<div class="au-sub">' + T.registerSub + '</div>' +
      '<div><div class="au-lbl">' + T.character + '</div><div class="au-field"><input class="au-input" readonly value="' + esc(S.name) + '"><span class="au-tag">' + T.validName + '</span></div></div>' +
      '<div><div class="au-lbl">' + T.choosePassword(S.minLen, S.maxLen) + '</div><div class="au-field"><input class="au-input" id="au-pw" type="password" maxlength="' + S.maxLen + '" autocomplete="off">' +
        '<button class="au-tag" id="au-eye" data-silent>' + T.show + '</button></div>' +
        '<div class="au-meter" id="au-meter"><b></b><b></b><b></b><b></b></div></div>' +
      '<div><div class="au-lbl">' + T.repeatPassword + '</div><div class="au-field"><input class="au-input" id="au-pw2" type="password" maxlength="' + S.maxLen + '" autocomplete="off"></div></div>' +
      '<div><div class="au-lbl">' + T.emailOptional + '</div><div class="au-field"><input class="au-input" id="au-mail" type="email" maxlength="254" autocomplete="off" placeholder="name@mail.ge"><span class="au-tag">' + T.emailWhy + '</span></div></div>' +
      '<div class="au-check' + (S.rules ? " on" : "") + '" id="au-rules"><i>' + (S.rules ? "✓" : "") + "</i><span>" + T.rules + "</span></div>" +
      '<div class="au-err" id="au-err">' + esc(S.err) + "</div>" +
      '<button class="au-btn" id="au-go">' + T.next + '</button>';
    side.innerHTML = '<div class="q">' + T.firstTime + '</div><div class="a">' + T.firstTimeSub + '</div>' +
      '<button class="b" id="au-quit">' + T.quit + '</button><div class="au-steps">' + progress(0) + "</div>";
    var pw = U.el("au-pw"), pw2 = U.el("au-pw2"), meter = U.el("au-meter").children;
    U.el("au-eye").onclick = function () { var t = pw.type === "password" ? "text" : "password"; pw.type = t; pw2.type = t; this.textContent = t === "password" ? T.show : T.hide; };
    pw.addEventListener("input", function () { var n = strength(pw.value); for (var i = 0; i < 4; i++) meter[i].classList.toggle("on", i < n); });
    U.el("au-rules").onclick = function () { S.rules = !S.rules; this.classList.toggle("on", S.rules); this.querySelector("i").textContent = S.rules ? "✓" : ""; };
    U.el("au-quit").onclick = function () { LSSC.emit("lssc:quit"); };
    U.el("au-go").onclick = function () {
      if (S.busy) return;
      var v = pw.value;
      if (v.length < S.minLen || v.length > S.maxLen) { fail(T.pwLength(S.minLen, S.maxLen)); return; }
      if (v !== pw2.value) { fail(T.pwMismatch); return; }
      if (!S.rules) { fail(T.acceptRules); return; }
      var mail = (U.el("au-mail").value || "").trim();
      if (mail && !/^[A-Za-z0-9._%+-]{1,64}@[A-Za-z0-9-]+(\.[A-Za-z0-9-]+)*\.[A-Za-z]{2,}$/.test(mail)) { fail(T.emailBad); return; }
      var hex = toHex(v);
      if (!hex) { fail(T.latinOnly); return; }
      busy(true, T.creating);
      LSSC.emit("lssc:register", hex, mail || "-");
      pw.value = ""; pw2.value = "";
    };
    [pw, pw2, U.el("au-mail")].forEach(function (el) { el.addEventListener("keydown", function (ev) { if (ev.key === "Enter") U.el("au-go").click(); else if (ev.key.length === 1) A.sfx.type(); }); });
    setTimeout(function () { pw.focus(); }, 350);
  };

  R.gender = function () {
    setWide(true); progress(1);
    pane.innerHTML =
      '<div class="au-h1">' + T.yourCharacter + '</div>' +
      '<div class="au-sub">' + esc(T.arriving(displayName())) + '</div>' +
      '<div class="au-cards c2" id="au-g">' +
        '<button class="au-card big" data-g="0">' + IC.male + "<h3>" + T.man + "</h3></button>" +
        '<button class="au-card big" data-g="1">' + IC.female + "<h3>" + T.woman + "</h3></button>" +
      "</div>" +
      '<div class="au-err" id="au-err">' + esc(S.err) + "</div>" +
      '<div class="au-row" style="max-width:28rem"><button class="au-btn" id="au-go" disabled>' + T.forward + '</button></div>';
    var cards = U.el("au-g");
    cards.onclick = function (ev) {
      var c = ev.target.closest("[data-g]"); if (!c) return;
      S.gender = U.int(c.getAttribute("data-g"));
      Array.prototype.forEach.call(cards.children, function (x) { x.classList.toggle("sel", x === c); });
      U.el("au-go").disabled = false;
    };
    U.el("au-go").onclick = function () { if (S.busy || S.gender < 0) return; busy(true); LSSC.emit("lssc:gender", S.gender); };
  };

  R.skin = function () {
    setWide(true); progress(1);
    var html = S.skins.map(function (s, i) {
      return '<button class="au-card" data-i="' + i + '"><h3>' + T.look + " " + (i + 1) + "</h3><p>" + T.model + " " + s + "</p></button>";
    }).join("");
    pane.innerHTML =
      '<div class="au-h1">' + T.lookTitle + '</div>' +
      '<div class="au-sub">' + T.lookSub + '</div>' +
      '<div class="au-cards c4" id="au-s">' + html + "</div>" +
      '<div class="au-err" id="au-err">' + esc(S.err) + "</div>" +
      '<div class="au-row" style="max-width:28rem"><button class="au-btn" id="au-go" disabled>' + T.confirm + '</button></div>';
    var cards = U.el("au-s");
    cards.onclick = function (ev) {
      var c = ev.target.closest("[data-i]"); if (!c) return;
      S.skinIdx = U.int(c.getAttribute("data-i"));
      Array.prototype.forEach.call(cards.children, function (x) { x.classList.toggle("sel", x === c); });
      U.el("au-go").disabled = false;
    };
    U.el("au-go").onclick = function () { if (S.busy || S.skinIdx < 0) return; busy(true); LSSC.emit("lssc:skin", S.skinIdx); };
  };

  function storyCards(list, cur) {
    var keys = Object.keys(list).map(Number).sort(function (a, b) { return a - b; });
    var html = keys.map(function (k) {
      return '<button class="au-card' + (k === cur ? " sel" : "") + '" data-k="' + k + '"><h3>' + esc(list[k].short) + "</h3><p>" + esc(list[k].long) + "</p></button>";
    }).join("");
    return html + '<button class="au-card muted' + (cur === 0 ? " sel" : "") + '" data-k="0"><h3>' + T.preferNot + '</h3><p>' + T.preferNotSub + '</p></button>';
  }

  function compose() {
    var nm = displayName(), o = S.origins[S.origin], m = S.motives[S.motive];
    if (S.origin <= 0 && S.motive <= 0) return T.pastUnknown;
    if (S.origin <= 0) return nm + ". " + T.originUnknown + " " + m.long;
    if (S.motive <= 0) return nm + ". " + o.long;
    return nm + ". " + o.long + " " + m.long;
  }

  R.story = function () {
    setWide(true); progress(2);
    var html, title, sub;
    if (S.sub === "origin") {
      title = T.originTitle; sub = T.originSub;
      html = '<div class="au-cards c3" id="au-c">' + storyCards(S.origins, S.origin) + "</div>" +
        '<div class="au-err" id="au-err"></div><div class="au-row" style="max-width:28rem"><button class="au-btn" id="au-go"' + (S.origin < 0 ? " disabled" : "") + ">" + T.forward + "</button></div>";
    } else if (S.sub === "motive") {
      title = T.motiveTitle; sub = T.motiveSub;
      html = '<div class="au-cards c3" id="au-c">' + storyCards(S.motives, S.motive) + "</div>" +
        '<div class="au-err" id="au-err"></div><div class="au-row" style="max-width:40rem"><button class="au-btn ghost" id="au-back">' + T.back + '</button><button class="au-btn" id="au-go"' + (S.motive < 0 ? " disabled" : "") + ">" + T.forward + "</button></div>";
    } else {
      title = T.storyTitle; sub = T.storySub;
      html = '<div class="au-quote">' + esc(compose()) + "</div>" +
        '<div class="au-err" id="au-err"></div><div class="au-row" style="max-width:40rem"><button class="au-btn ghost" id="au-back">' + T.back + '</button><button class="au-btn" id="au-go">' + T.boardBus + '</button></div>';
    }
    pane.innerHTML = '<div class="au-h1">' + title + '</div><div class="au-sub">' + sub + "</div>" + html;
    var cards = U.el("au-c");
    if (cards) cards.onclick = function (ev) {
      var c = ev.target.closest("[data-k]"); if (!c) return;
      var k = U.int(c.getAttribute("data-k"));
      if (S.sub === "origin") S.origin = k; else S.motive = k;
      Array.prototype.forEach.call(cards.children, function (x) { x.classList.toggle("sel", x === c); });
      U.el("au-go").disabled = false;
    };
    var back = U.el("au-back");
    if (back) back.onclick = function () { S.sub = (S.sub === "confirm" && S.origin > 0) ? "motive" : "origin"; A.sfx.whoosh(); R.story(); };
    U.el("au-go").onclick = function () {
      if (S.busy) return;
      if (S.sub === "origin") { if (S.origin < 0) return; if (S.origin === 0) S.motive = 0; S.sub = S.origin === 0 ? "confirm" : "motive"; A.sfx.whoosh(); R.story(); return; }
      if (S.sub === "motive") { if (S.motive < 0) return; S.sub = "confirm"; A.sfx.whoosh(); R.story(); return; }
      busy(true, T.leaving); A.sfx.ok();
      LSSC.emit("lssc:story", Math.max(0, S.origin), Math.max(0, S.motive));
    };
  };

  function render() {
    S.busy = false;
    side.style.display = (S.step === "login" || S.step === "register") ? "" : "none";
    (R[S.step] || R.login)();
  }

  // ================================================================ EVENTI DAL SERVER
  LSSC.on("lssc:auth_skin", function (idx, skin) { idx = U.int(idx); if (idx === 0) S.skins = []; S.skins[idx] = U.int(skin); });
  LSSC.on("lssc:auth_story", function (kind, idx, shortText, longText) {
    var o = { short: U.str(shortText), long: U.str(longText) };
    if (U.int(kind) === 1) S.origins[U.int(idx)] = o; else S.motives[U.int(idx)] = o;
  });
  LSSC.on("lssc:auth", function (step, name, err, n, minLen, maxLen) {
    step = U.str(step);
    var same = step === S.step;
    S.name = U.str(name); S.err = U.str(err); S.n = U.int(n);
    S.minLen = U.int(minLen, 6) || 6; S.maxLen = U.int(maxLen, 32) || 32;
    if (step === "story" && !same) { S.sub = "origin"; S.origin = -1; S.motive = -1; }
    if (step === "skin" && !same) S.skinIdx = -1;
    if (step === "gender" && !same) S.gender = -1;
    S.step = step;
    render();
    if (S.err) fail(S.err);
    else if (!same) A.sfx.whoosh();
    if (!same && step !== "login" && step !== "register") A.sfx.ok();
  });

  // ================================================================ VISTA
  LSSC.registerView("auth", {
    show: function () {
      root.classList.remove("leaving");
      root.hidden = false;
      A.musicStart();
    },
    hide: function () {
      A.musicStop();
      root.classList.add("leaving");
      setTimeout(function () { if (root.classList.contains("leaving")) { root.hidden = true; root.classList.remove("leaving"); } }, 900);
      S.step = "";
    },
    onEscape: function () { return true; }     // l'accesso non si chiude con ESC (si usa "Esci")
  });
})();
