/* telefono.js - Telefono CityOS (vista interattiva "phone").
   App: Contratti, Messaggi, Quartiere, Banca, Lavoro, Profilo (+ Progetto del quartiere).
   Ogni app chiede i dati al server all'apertura e mostra solo quello che riceve. Azioni = stesse callback dei dialog
   di living_ui.inc (abbandono, scelta quartiere, progetto, donazione) piu' la richiesta di checkpoint del Job Center. */
(function () {
  "use strict";
  var LSSC = window.LSSC, U = LSSC.util, N = LSSC.names;
  var root = U.el("view-phone");

  var SVG = function (p) { return '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">' + p + "</svg>"; };
  var IC = {
    contracts: SVG('<rect x="5" y="4" width="14" height="17" rx="2"/><path d="M9 4h6v3H9zM9 12h6M9 16h4"/>'),
    messages: SVG('<path d="M4 6a2 2 0 0 1 2-2h12a2 2 0 0 1 2 2v9a2 2 0 0 1-2 2H9l-5 4z"/>'),
    district: SVG('<path d="M3 11l9-7 9 7"/><path d="M5 10v10h14V10"/><path d="M10 20v-5h4v5"/>'),
    bank: SVG('<path d="M3 9l9-5 9 5"/><path d="M5 10v7M10 10v7M14 10v7M19 10v7M3 20h18"/>'),
    job: SVG('<rect x="3" y="7" width="18" height="13" rx="2"/><path d="M9 7V5a1 1 0 0 1 1-1h4a1 1 0 0 1 1 1v2M3 13h18"/>'),
    profile: SVG('<circle cx="12" cy="8" r="4"/><path d="M4 21c1.5-4 4.5-6 8-6s6.5 2 8 6"/>'),
    back: SVG('<path d="M15 5l-7 7 7 7"/>')
  };
  var APPS = [
    ["contracts", "Contratti"], ["messages", "Messaggi"], ["district", "Quartiere"],
    ["bank", "Banca"], ["job", "Lavoro"], ["profile", "Profilo"]
  ];

  root.innerHTML =
    '<section class="phone" role="dialog" aria-modal="true" aria-label="Telefono CityOS">' +
      '<div class="phone-screen" id="ph-screen">' +
        '<div class="phone-status"><span class="num" id="ph-clock">--:--</span><span class="os">CityOS</span></div>' +
        '<div class="phone-scroll" id="ph-body"></div>' +
        '<div class="phone-home-bar"><button id="ph-home" aria-label="Schermata principale"></button></div>' +
      "</div>" +
    "</section>";

  var body = U.el("ph-body"), screen = U.el("ph-screen");
  var app = "home", confirmBox = null, pendingAbandon = false;

  // ---------------------------------------------------------------- dati ricevuti dal server
  var D = {
    hud: { money: 0, bank: 0, level: 1, hour: -1, minute: 0 },
    ct: null,
    msgs: [],
    profile: { name: "", job: "" },
    district: null, districtTab: 0, districtWait: false,
    project: null, projectWait: false,
    rep: { tracks: {}, local: {}, ready: false },
    bankRows: [], bankReady: false
  };

  LSSC.on("lssc:hud", function (money, bank, level, expIn, expStep, hour, minute) {
    D.hud = { money: U.int(money), bank: U.int(bank), level: U.int(level, 1), hour: U.int(hour, -1), minute: U.int(minute) };
    U.el("ph-clock").textContent = clock();
    if (isOpen() && (app === "home" || app === "bank")) render();
  });
  LSSC.on("lssc:contract", function (id, state, objective, destination, distanceM, secondsLeft, integrityPct, from, to, cargo, reward, repPenalty) {
    id = U.int(id);
    pendingAbandon = false;
    var prev = D.ct;
    D.ct = id > 0 ? {
      id: id, state: U.int(state), objective: U.str(objective), dest: U.str(destination), dist: U.int(distanceM, -1),
      expAt: U.int(secondsLeft, -1) >= 0 ? Date.now() + U.int(secondsLeft) * 1000 : 0, integ: U.int(integrityPct, -1),
      from: U.int(from), to: U.int(to), cargo: U.int(cargo), reward: U.int(reward), penalty: Math.abs(U.int(repPenalty))
    } : null;
    // Gli aggiornamenti dell'HUD inviano da/a/merce/compenso a 0: si tengono quelli gia' noti dello stesso contratto
    if (D.ct && prev && prev.id === D.ct.id && !D.ct.from) {
      D.ct.from = prev.from; D.ct.to = prev.to; D.ct.cargo = prev.cargo; D.ct.reward = prev.reward; D.ct.penalty = prev.penalty;
    }
    if (isOpen() && (app === "home" || app === "contracts")) render();
  });
  // lssc:msg sender text secondsAgo unread
  LSSC.on("lssc:msg", function (sender, text, ago, unread) {
    D.msgs.unshift({ from: U.str(sender), text: U.str(text), at: Date.now() - U.int(ago) * 1000, unread: !!U.int(unread) });
    if (D.msgs.length > 20) D.msgs.length = 20;
    if (isOpen() && (app === "home" || app === "messages")) render();
  });
  LSSC.on("lssc:msg_clear", function () { D.msgs = []; if (isOpen()) render(); });
  // lssc:profile name jobName
  LSSC.on("lssc:profile", function (name, job) { D.profile = { name: U.str(name), job: U.str(job) }; if (isOpen()) render(); });

  // Quartiere: lssc:district_begin id activity security crime localRep festivalHours prosperity localTier discountPct hazard
  LSSC.on("lssc:district_begin", function (id, act, sec, crime, lrep, fest, prosp, tier, disc, hazard) {
    D.district = {
      id: U.int(id), act: U.int(act), sec: U.int(sec), crime: U.int(crime), lrep: U.int(lrep), fest: U.int(fest),
      prosp: U.int(prosp, 1), tier: U.int(tier), disc: U.int(disc), hazard: !!U.int(hazard), rumor: "", events: []
    };
  });
  LSSC.on("lssc:district_rumor", function (t) { if (D.district) D.district.rumor = U.str(t); });
  LSSC.on("lssc:district_event", function (type, ago, value) {
    if (D.district && D.district.events.length < 8) D.district.events.push({ type: U.int(type), ago: U.int(ago), value: U.int(value) });
  });
  LSSC.on("lssc:district_end", function () {
    D.districtWait = false;
    if (D.district) D.districtTab = D.district.id;
    if (isOpen() && app === "district") render();
  });
  // lssc:project projectId districtId name goal fund hoursLeft donMin donMax dayLeft blockCode
  LSSC.on("lssc:project", function (pid, did, name, goal, fund, hours, dmin, dmax, dayLeft, block) {
    D.projectWait = false;
    D.project = { id: U.int(pid), district: U.int(did), name: U.str(name), goal: U.int(goal), fund: U.int(fund), hours: U.int(hours),
      min: U.int(dmin), max: U.int(dmax), dayLeft: U.int(dayLeft), block: U.int(block) };
    if (isOpen() && app === "project") render();
  });
  // Reputazione: lssc:rep_track track points completed failed ratingSum ratingCnt dayGain tier nextAt bonusPct
  LSSC.on("lssc:rep_begin", function () { D.rep = { tracks: {}, local: {}, ready: false }; });
  LSSC.on("lssc:rep_track", function (track, pts, done, fail, rsum, rcnt, day, tier, nextAt, bonus) {
    D.rep.tracks[U.int(track)] = { pts: U.int(pts), done: U.int(done), fail: U.int(fail), rsum: U.int(rsum), rcnt: U.int(rcnt),
      day: U.int(day), tier: U.int(tier), next: U.int(nextAt), bonus: U.int(bonus) };
  });
  // lssc:rep_local districtId rep localTier discountPct premium
  LSSC.on("lssc:rep_local", function (did, rep, tier, disc, premium) {
    D.rep.local[U.int(did)] = { rep: U.int(rep), tier: U.int(tier), disc: U.int(disc), premium: !!U.int(premium) };
  });
  LSSC.on("lssc:rep_end", function () { D.rep.ready = true; if (isOpen() && app === "profile") render(); });
  // Banca: lssc:bank_row when kind other amount balanceAfter (estratto conto, sola lettura)
  LSSC.on("lssc:bank_begin", function () { D.bankRows = []; D.bankReady = false; });
  LSSC.on("lssc:bank_row", function (when, kind, other, amount, after) {
    if (D.bankRows.length < 10) D.bankRows.push({ when: U.str(when), kind: U.str(kind), other: U.str(other), amount: U.int(amount), after: U.int(after) });
  });
  LSSC.on("lssc:bank_end", function () { D.bankReady = true; if (isOpen() && app === "bank") render(); });
  LSSC.on("lssc:reject", function () {
    if (confirmBox) { confirmBox.close(); confirmBox = null; }
    pendingAbandon = false; D.projectWait = false; D.districtWait = false;
    if (isOpen()) render();
  });

  // ---------------------------------------------------------------- rendering
  function isOpen() { return LSSC.currentView() === "phone"; }
  function clock() {
    var h = D.hud.hour, m = D.hud.minute;
    return h < 0 ? "--:--" : (h < 10 ? "0" : "") + h + ":" + (m < 10 ? "0" : "") + m;
  }
  function head(title, backTo) {
    return '<div class="app-head"><button class="icon-btn" data-go="' + (backTo || "home") + '" aria-label="Indietro">' + IC.back +
      '</button><h2 class="app-title">' + U.esc(title) + "</h2></div>";
  }
  function ctLeft() { return D.ct && D.ct.expAt ? Math.max(0, Math.round((D.ct.expAt - Date.now()) / 1000)) : -1; }
  function unread() { return D.msgs.filter(function (m) { return m.unread; }).length; }
  function meter(cls, label, value, max, note) {
    return '<div class="meter ' + cls + '"><div class="meter-label"><span>' + label + '</span><span class="num">' + value + "/" + max + "</span></div>" +
      '<div class="bar"><i style="width:' + U.pct(value, max) + '%"></i></div>' + (note ? '<div class="meter-note">' + note + "</div>" : "") + "</div>";
  }
  function localLabel(tier, disc) {
    if (tier >= 2) return "Di casa (-" + disc + "% su noleggio e taxi)";
    if (tier === 1) return "Conosciuto (-" + disc + "% su noleggio e taxi)";
    if (tier < 0) return "Da riconquistare: si risale più in fretta";
    return "Sconosciuto";
  }

  function contractWidget(full) {
    if (!D.ct) {
      return '<div class="widget empty"><div class="widget-head"><span>Contratto</span></div>' +
        '<div class="widget-title">Nessun contratto in corso</div>' +
        '<p class="widget-sub">La bacheca della Casa del Quartiere di Idlewood ha sempre qualcosa da fare.</p></div>';
    }
    var c = D.ct, t = ctLeft();
    var html = '<div class="widget"><div class="widget-head"><span class="num">Contratto #' + c.id + "</span><b>" + (c.state === 2 ? "In consegna" : "Accettato") + "</b></div>" +
      '<div class="widget-title">' + U.esc(U.name("district", c.from)) + '<span class="arrow">›</span>' + U.esc(U.name("district", c.to)) + "</div>" +
      '<p class="widget-sub">' + U.esc(c.objective || "-") + "</p>" +
      '<div class="widget-row"><span class="muted">Verso ' + U.esc(c.dest || "-") + (c.dist >= 0 ? " · " + U.dist(c.dist) : "") + '</span><span class="big num" data-ct-timer>' + U.time(t) + "</span></div>";
    if (c.integ >= 0) html += '<div class="meter"><div class="meter-label"><span>Integrità merce</span><span class="num">' + c.integ + '%</span></div><div class="bar' + (c.integ < 40 ? " danger" : c.integ < 70 ? " warn" : "") + '"><i style="width:' + Math.min(100, c.integ) + '%"></i></div></div>';
    if (full) {
      html += '<dl class="facts" style="margin-top:0.9rem"><dt>Merce</dt><dd>' + U.esc(U.name("cargo", c.cargo)) + '</dd><dt>Compenso indicativo</dt><dd class="money num">' + U.money(c.reward) + "</dd></dl>";
    }
    return html + "</div>";
  }

  var R = {};
  R.home = function () {
    var h = D.hud.hour, greet = h < 0 ? "Ciao" : h < 6 ? "Buonanotte" : h < 13 ? "Buongiorno" : h < 18 ? "Buon pomeriggio" : "Buonasera";
    var first = D.profile.name.split(/[_ ]/)[0];
    var badge = unread();
    return '<div class="os-greet num">' + clock() + "</div>" +
      '<p class="os-date">' + greet + (first ? ", " + U.esc(first) : "") + ". Saldo in banca " + '<span class="money num">' + U.money(D.hud.bank) + "</span></p>" +
      contractWidget(false) +
      '<div class="apps">' + APPS.map(function (a) {
        return '<button class="app' + (a[0] === "contracts" ? " primary" : "") + '" data-go="' + a[0] + '"><span class="app-icon">' + IC[a[0]] +
          (a[0] === "messages" && badge ? '<span class="badge">' + badge + "</span>" : "") + "</span>" + a[1] + "</button>";
      }).join("") + "</div>";
  };
  R.contracts = function () {
    var html = head("Contratti") + contractWidget(true) + '<div class="stack" style="margin-top:1rem">';
    html += '<button class="btn btn-primary" data-act="board">Apri la bacheca</button>';
    if (D.ct) html += '<button class="btn btn-danger" data-act="abandon"' + (pendingAbandon ? " disabled" : "") + ">" + (pendingAbandon ? "In attesa…" : "Lascia il contratto") + "</button>" +
      '<p class="small muted">Se lasci il contratto non perdi denaro, ma perdi ' + D.ct.penalty + " punti di reputazione nei trasporti.</p>";
    else html += '<p class="small muted">Dal telefono consulti la bacheca ovunque; per accettare serve una bacheca: Casa del Quartiere di Idlewood, magazzino di Ganton o di Jefferson.</p>';
    return html + "</div>";
  };
  R.messages = function () {
    var html = head("Messaggi");
    if (!D.msgs.length) return html + '<p class="muted">Nessun messaggio. Qui arrivano gli avvisi della bacheca e del quartiere.</p>';
    html += '<div class="stack">' + D.msgs.map(function (m) {
      return '<div class="msg' + (m.unread ? " unread" : "") + '"><div class="msg-head"><b>' + U.esc(m.from) + "</b><span>" + U.ago((Date.now() - m.at) / 1000) + "</span></div><p>" + U.esc(m.text) + "</p></div>";
    }).join("") + "</div>";
    D.msgs.forEach(function (m) { m.unread = false; });
    return html;
  };
  R.district = function () {
    var tabs = '<div class="tabs">' + [1, 2, 3].map(function (i) {
      return '<button class="chip" data-district="' + i + '" aria-pressed="' + (D.districtTab === i) + '">' + N.district[i] + "</button>";
    }).join("") + "</div>";
    var html = head("Quartiere") + tabs;
    var d = D.district;
    if (D.districtWait || !d || d.id !== D.districtTab) return html + '<p class="muted">Sto chiedendo le notizie del quartiere…</p>';
    var prosp = d.prosp === 0 ? ["low", "In difficoltà", "Ogni consegna conta per la ricostruzione."]
      : d.prosp === 2 ? ["high", "Fiorente", "C'è più lavoro per tutti."] : ["", "Stabile", "Il quartiere tiene il passo."];
    html += '<div class="widget"><div class="widget-head"><span>Prosperità</span></div><div class="district-state ' + prosp[0] + '">' + prosp[1] + "</div>" +
      '<p class="widget-sub">' + prosp[2] + "</p>" +
      meter("act", "Attività economica", d.act, 1000) +
      meter("sec", "Sicurezza", d.sec, 100) +
      meter("pre", "Pressione criminale", d.crime, 100, d.hazard ? "Zona a rischio: i contratti pagano di più." : "") +
      (d.fest > 0 ? '<p class="festa">Festa di quartiere in corso: ancora ' + d.fest + " ore.</p>" : "") + "</div>";
    html += '<div class="widget" style="margin-top:0.7rem"><div class="widget-head"><span>La tua reputazione qui</span></div>' +
      '<div class="rep-line"><span>' + U.esc(localLabel(d.tier, d.disc)) + '</span><span class="rep-num num">' + d.lrep + "</span></div></div>";
    if (d.rumor) html += '<p class="rumor">Si dice in giro: “' + U.esc(d.rumor) + "”</p>";
    html += '<div class="section-label">Ultimi avvenimenti</div>';
    if (!d.events.length) html += '<p class="muted small">Una giornata tranquilla, per ora.</p>';
    else html += '<ul class="timeline">' + d.events.map(function (e) {
      var v = e.value > 0 ? '<span class="plus num">+' + e.value + "</span>" : e.value < 0 ? '<span class="minus num">' + e.value + "</span>" : "<span></span>";
      return '<li><span class="ago">' + U.ago(e.ago) + "</span><span>" + U.esc(U.name("event", e.type)) + "</span>" + v + "</li>";
    }).join("") + "</ul>";
    html += '<div class="stack" style="margin-top:1rem"><button class="btn" data-act="project">Progetto della comunità</button></div>';
    return html;
  };
  R.project = function () {
    var html = head("Progetto", "district"), p = D.project;
    if (D.projectWait || !p) return html + '<p class="muted">Sto aprendo il progetto del quartiere…</p>';
    if (!p.id) return html + '<p class="muted">In questo quartiere non c\'è un progetto attivo adesso.</p>';
    var cap = Math.min(p.max, p.dayLeft);
    html += '<div class="widget"><div class="widget-head"><span>' + U.esc(U.name("district", p.district)) + '</span><span class="num">' + p.hours + " h rimaste</span></div>" +
      '<div class="widget-title">' + U.esc(p.name) + "</div>" +
      '<div class="meter"><div class="meter-label"><span class="money num">' + U.money(p.fund) + '</span><span class="num">obiettivo ' + U.money(p.goal) + "</span></div>" +
      '<div class="bar"><i style="width:' + U.pct(p.fund, p.goal) + '%"></i></div></div></div>';
    html += '<p class="small muted" style="margin-top:0.8rem">Il fondo cresce anche con ogni contratto completato nel quartiere, senza toccare il tuo compenso.</p>';
    if (p.block) return html + '<p class="detail-status away" style="margin-top:0.8rem">' + U.esc(U.name("prjBlock", p.block)) + "</p>";
    return html + '<div class="section-label">Contribuisci (da ' + U.money(p.min) + " a " + U.money(cap) + ")</div>" +
      '<div class="amount-input"><input id="ph-amount" inputmode="numeric" maxlength="9" placeholder="' + p.min + '" aria-label="Importo"><button class="btn btn-primary" data-act="contribute">Dona</button></div>' +
      '<p class="small muted" id="ph-amount-err" style="margin-top:0.4rem"></p>';
  };
  R.bank = function () {
    var html = head("Banca") +
      '<div class="widget"><div class="widget-head"><span>Saldo</span></div><div class="widget-title money num">' + U.money(D.hud.bank) + "</div>" +
      '<div class="widget-row"><span class="muted">Contanti</span><span class="num">' + U.money(D.hud.money) + "</span></div></div>" +
      '<div class="section-label">Ultimi movimenti</div>';
    if (!D.bankReady) html += '<p class="muted small">Sto caricando l\'estratto conto…</p>';
    else if (!D.bankRows.length) html += '<p class="muted small">Nessun movimento recente.</p>';
    else html += D.bankRows.map(function (r) {
      return '<div class="list-row"><span><b>' + U.esc(r.kind) + "</b>" + (r.other ? " · " + U.esc(r.other) : "") + '<br><span class="small muted num">' + U.esc(r.when) + "</span></span>" +
        '<span class="num ' + (r.amount >= 0 ? "money" : "") + '">' + (r.amount >= 0 ? "+" : "") + U.money(r.amount) + "</span></div>";
    }).join("");
    return html + '<p class="small muted" style="margin-top:0.8rem">Depositi, prelievi e bonifici si fanno agli ATM o allo sportello.</p>';
  };
  R.job = function () {
    return head("Lavoro") +
      '<div class="widget"><div class="widget-head"><span>Job Center</span><span>Pershing Square</span></div>' +
      '<div class="widget-title">Lavori stabili</div><p class="widget-sub">Il Job Center di Pershing Square offre lavori stabili; se ti serve un mezzo, il noleggio è proprio lì accanto.</p></div>' +
      (D.profile.job ? '<div class="widget-row"><span class="muted">Il tuo lavoro</span><span>' + U.esc(D.profile.job) + "</span></div>" : "") +
      '<div class="stack" style="margin-top:1rem"><button class="btn btn-primary" data-act="waypoint">Segna il Job Center sulla mappa</button></div>';
  };
  R.profile = function () {
    var html = head("Profilo") +
      '<div class="widget"><div class="widget-title">' + U.esc((D.profile.name || "-").replace(/_/g, " ")) + "</div>" +
      '<div class="widget-row"><span class="muted">Livello</span><span class="num">' + D.hud.level + "</span></div>" +
      (D.profile.job ? '<div class="widget-row"><span class="muted">Lavoro</span><span>' + U.esc(D.profile.job) + "</span></div>" : "") + "</div>";
    if (!D.rep.ready) return html + '<p class="muted small" style="margin-top:1rem">Sto caricando la reputazione…</p>';
    html += '<div class="section-label">Il tuo mestiere</div>';
    [1, 2].forEach(function (t) {
      var r = D.rep.tracks[t];
      if (!r) { html += '<div class="list-row"><span>' + N.track[t] + '</span><span class="muted small">Non hai ancora iniziato</span></div>'; return; }
      var nextTxt = r.next > 0 ? (N.tier[r.tier + 1] || "") + " tra " + Math.max(0, r.next - r.pts) + " punti" : "il grado più alto";
      var avg = r.rcnt > 0 ? ", voto " + (r.rsum / r.rcnt).toFixed(1).replace(".", ",") + "/5" : "";
      html += '<div class="meter"><div class="meter-label"><span>' + N.track[t] + ": <b>" + U.esc(U.name("tier", r.tier)) + "</b> (+" + r.bonus + '%)</span><span class="num">' + r.pts + " punti</span></div>" +
        '<div class="bar"><i style="width:' + (r.next > 0 ? U.pct(r.pts, r.next) : 100) + '%;background:var(--accent)"></i></div>' +
        '<div class="small muted">' + nextTxt + " · completati " + r.done + ", non riusciti " + r.fail + avg + ", oggi +" + r.day + "</div></div>";
    });
    html += '<div class="section-label">Il tuo nome nei quartieri</div>';
    [1, 2, 3].forEach(function (d) {
      var l = D.rep.local[d] || { rep: 0, tier: 0, disc: 0, premium: false };
      html += '<div class="list-row"><span>' + N.district[d] + '<br><span class="small muted">' + U.esc(localLabel(l.tier, l.disc)) + (l.premium ? " · contratti premium" : "") +
        '</span></span><span class="rep-num num">' + l.rep + "</span></div>";
    });
    return html + '<p class="small muted" style="margin-top:0.6rem">La reputazione non si compra: cresce con il lavoro, ogni giorno un passo alla volta.</p>';
  };

  function render() {
    body.innerHTML = (R[app] || R.home)();
  }

  // Apertura di un'app: chiede al server i dati che servono (il server risponde con gli eventi sopra)
  function go(name) {
    if (!R[name]) name = "home";
    if (confirmBox) { confirmBox.close(); confirmBox = null; }
    app = name;
    if (name === "district") {
      D.districtWait = true;
      if (!D.districtTab) D.districtTab = D.district ? D.district.id : 0;
      LSSC.emit("lssc:district", D.districtTab);      // 0 = quartiere in cui si trova il giocatore (lo decide il server)
    } else if (name === "project") {
      D.projectWait = true;
      LSSC.emit("lssc:project_open", D.districtTab || 1);
    } else if (name === "bank") {
      D.bankReady = false; LSSC.emit("lssc:bank_req");
    } else if (name === "profile") {
      D.rep.ready = false; LSSC.emit("lssc:rep_req");
    }
    render();
    body.scrollTop = 0;
  }

  body.addEventListener("click", function (ev) {
    var t = ev.target.closest("[data-go],[data-act],[data-district]");
    if (!t) return;
    if (t.hasAttribute("data-go")) { go(t.getAttribute("data-go")); return; }
    if (t.hasAttribute("data-district")) {
      D.districtTab = U.int(t.getAttribute("data-district"));
      D.districtWait = true; render();
      LSSC.emit("lssc:district", D.districtTab);
      return;
    }
    var act = t.getAttribute("data-act");
    if (act === "board") LSSC.emit("lssc:board_refresh");             // il server ricarica e apre la bacheca (lssc:open board)
    else if (act === "abandon" && D.ct) askAbandon();
    else if (act === "project") go("project");
    else if (act === "waypoint") LSSC.emit("lssc:job_waypoint");
    else if (act === "contribute") askContribute();
  });
  U.el("ph-home").addEventListener("click", function () { go("home"); });

  function askAbandon() {
    var c = D.ct;
    confirmBox = LSSC.confirm(screen, {
      title: "Lasci il contratto #" + c.id + "?",
      html: "Non perdi denaro, ma perdi <b>" + c.penalty + " punti</b> di reputazione nei trasporti.",
      yes: "Lascia", no: "Continua", danger: true,
      onYes: function () { confirmBox = null; pendingAbandon = true; LSSC.emit("lssc:abandon", c.id); render(); },
      onNo: function () { confirmBox = null; }
    });
  }

  function askContribute() {
    var p = D.project, input = U.el("ph-amount"), err = U.el("ph-amount-err");
    var raw = (input.value || "").trim(), cap = Math.min(p.max, p.dayLeft);
    // controllo di comodo, come UI_LvParseAmount: il server rivalida importo, tetti e fondi
    if (!/^\d{1,9}$/.test(raw)) { err.textContent = "Scrivi un importo in dollari, solo cifre."; return; }
    var amount = U.int(raw);
    if (amount < p.min || amount > cap) { err.textContent = "Puoi donare da " + U.money(p.min) + " a " + U.money(cap) + " oggi."; return; }
    err.textContent = "";
    confirmBox = LSSC.confirm(screen, {
      title: "Doni " + U.money(amount) + "?",
      html: "Vanno al progetto <b>" + U.esc(p.name) + "</b>. I soldi escono dai tuoi contanti.",
      yes: "Dona", no: "Annulla",
      onYes: function () { confirmBox = null; LSSC.emit("lssc:contribute", p.id, amount); },
      onNo: function () { confirmBox = null; }
    });
  }

  // Timer del contratto nel widget (solo testo, 1 s, solo a telefono aperto)
  var tick = 0;
  function tickTimer() {
    var n = body.querySelector("[data-ct-timer]");
    if (n) n.textContent = U.time(ctLeft());
  }

  LSSC.registerView("phone", {
    show: function (arg) {
      root.hidden = false;
      go(arg || "home");
      if (!tick) tick = setInterval(tickTimer, 1000);
    },
    hide: function () {
      root.hidden = true;
      if (confirmBox) { confirmBox.close(); confirmBox = null; }
      if (tick) { clearInterval(tick); tick = 0; }
    },
    onEscape: function () {
      if (confirmBox) { confirmBox.close(); confirmBox = null; return true; }
      return false;
    },
    onBack: function () {
      if (confirmBox) { confirmBox.close(); confirmBox = null; return; }
      go(app === "project" ? "district" : "home");
    }
  });
})();
