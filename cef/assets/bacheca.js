/* bacheca.js - Bacheca contratti (vista interattiva "board"): filtri, schede, dettaglio, Accetta con conferma.
   Stesse azioni dei dialog 9700-9703 di living_ui.inc. L'idoneita' (blockCode) e il luogo (atBoard) li decide il server;
   i filtri sono solo di visualizzazione. Il server rivalida tutto su lssc:accept. */
(function () {
  "use strict";
  var LSSC = window.LSSC, U = LSSC.util, N = LSSC.names;
  var root = U.el("view-board");

  var IC_CLOSE = '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"><path d="M6 6l12 12M18 6L6 18"/></svg>';
  var IC_REFRESH = '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 11a8 8 0 1 0-2.3 5.7"/><path d="M20 4v7h-7"/></svg>';

  root.innerHTML =
    '<div class="view-scrim"></div>' +
    '<section class="board glass" role="dialog" aria-modal="true" aria-labelledby="bd-title">' +
      '<header class="board-head">' +
        '<div><h1 class="board-title" id="bd-title">კონტრაქტების დაფა</h1><p class="board-sub" id="bd-sub"></p></div>' +
        '<div class="board-head-right"><span class="kbd">Esc</span>' +
          '<button class="icon-btn" id="bd-close" aria-label="დაფის დახურვა">' + IC_CLOSE + "</button></div>" +
      "</header>" +
      '<div class="board-filters">' +
        '<div class="filter-group" id="bd-f-district"><span>უბანი</span></div>' +
        '<div class="filter-group" id="bd-f-kind"><span>პროფესია</span></div>' +
        '<div class="filter-group"><button class="chip" id="bd-f-elig" aria-pressed="false">მხოლოდ ხელმისაწვდომი</button></div>' +
        '<div class="spacer"></div>' +
        '<button class="btn" id="bd-refresh">' + IC_REFRESH.replace("<svg", '<svg width="16" height="16"') + "განახლება</button>" +
      "</div>" +
      '<div class="board-body">' +
        '<div class="board-list" id="bd-list" role="listbox" aria-label="ხელმისაწვდომი კონტრაქტები"></div>' +
        '<aside class="board-detail" id="bd-detail"></aside>' +
      "</div>" +
    "</section>";

  var el = { sub: U.el("bd-sub"), list: U.el("bd-list"), detail: U.el("bd-detail"), elig: U.el("bd-f-elig") };

  var rows = [];            // cache dell'ultimo board_begin/add
  var atBoard = false, place = "";
  var flt = { district: 0, kind: 0, eligible: false };
  var selId = 0, pending = 0, confirmBox = null, tick = 0;

  // chip dei filtri
  function chips(hostId, labels, key) {
    var host = U.el(hostId);
    labels.forEach(function (label, i) {
      var b = document.createElement("button");
      b.className = "chip"; b.textContent = label; b.setAttribute("aria-pressed", String(i === flt[key]));
      b.addEventListener("click", function () {
        flt[key] = i;
        host.querySelectorAll(".chip").forEach(function (c, j) { c.setAttribute("aria-pressed", String(j === i)); });
        renderList();
      });
      host.appendChild(b);
    });
  }
  chips("bd-f-district", N.district, "district");
  chips("bd-f-kind", N.kind, "kind");
  el.elig.addEventListener("click", function () {
    flt.eligible = !flt.eligible;
    el.elig.setAttribute("aria-pressed", String(flt.eligible));
    renderList();
  });
  U.el("bd-close").addEventListener("click", function () { LSSC.closeView(true); });
  U.el("bd-refresh").addEventListener("click", function () { LSSC.emit("lssc:board_refresh"); });

  function match(r) {
    if (flt.district && r.from !== flt.district && r.to !== flt.district) return false;
    if (flt.kind && r.kind !== flt.kind) return false;
    if (flt.eligible && r.block !== 0) return false;
    return true;
  }
  function left(r) { return Math.max(0, Math.round((r.expAt - Date.now()) / 1000)); }
  function requirement(r) {
    if (r.minRep > 0) return "დონე " + r.minLevel + ", რეპუტაცია " + r.minRep;
    if (r.minLRep > 0) return "დონე " + r.minLevel + ", უბნის ნდობა " + r.minLRep;
    return "დონე " + r.minLevel;
  }
  function route(r) {
    return U.esc(U.name("district", r.from)) + '<span class="arrow">›</span>' + U.esc(U.name("district", r.to));
  }
  function find(id) { for (var i = 0; i < rows.length; i++) if (rows[i].id === id) return rows[i]; return null; }

  function renderList() {
    var shown = rows.filter(match);
    if (!shown.length) {
      el.list.innerHTML = '<div class="board-empty">' + (rows.length ? "ამ ფილტრებით კონტრაქტი ვერ მოიძებნა. სცადე რომელიმე ფილტრის მოხსნა." : "დაფა ახლა ცარიელია. რამდენიმე წუთში დააჭირე „განახლებას“.") + "</div>";
    } else {
      el.list.innerHTML = shown.map(function (r) {
        var ok = r.block === 0;
        return '<button class="ct' + (ok ? "" : " blocked") + '" role="option" data-id="' + r.id + '" aria-selected="' + (r.id === selId) + '">' +
          '<div class="ct-top"><span class="num">#' + r.id + "</span>" + (r.premium ? '<span class="ct-premium">პრემიუმ</span>' : "") + "</div>" +
          '<div class="ct-route">' + route(r) + "</div>" +
          '<div class="ct-cargo">' + U.esc(U.name("cargo", r.cargo)) + " · " + U.dist(r.dist) + "</div>" +
          '<div class="ct-foot"><span class="ct-reward num">' + U.money(r.reward) + '</span><span class="ct-time num" data-t="' + r.id + '">დარჩა ' + U.time(left(r)) + "</span></div>" +
          '<div class="ct-req">' + U.esc(ok ? requirement(r) : U.name("block", r.block)) + "</div>" +
        "</button>";
      }).join("");
    }
    if (selId && !find(selId)) selId = 0;
    renderDetail();
  }

  el.list.addEventListener("click", function (ev) {
    var card = ev.target.closest(".ct");
    if (!card) return;
    selId = U.int(card.getAttribute("data-id"));
    el.list.querySelectorAll(".ct").forEach(function (c) { c.setAttribute("aria-selected", String(c === card)); });
    renderDetail();
  });

  function renderDetail() {
    var r = find(selId);
    if (!r) {
      el.detail.innerHTML = '<p class="detail-empty">აირჩიე კონტრაქტი, რომ ნახო მარშრუტი, ანაზღაურება და მოთხოვნები.</p>';
      return;
    }
    var can = r.block === 0 && atBoard;
    var status, cls;
    if (r.block !== 0) { status = U.name("block", r.block) + "."; cls = "blocked"; }
    else if (!atBoard) { status = "კონტრაქტის აღება შეგიძლია დაფასთან: Idlewood-ის უბნის სახლში, Ganton-ის ან Jefferson-ის საწყობში."; cls = "away"; }
    else { status = "შეგიძლია ახლავე აიღო."; cls = "ok"; }
    el.detail.innerHTML =
      '<div><div class="detail-id num">კონტრაქტი #' + r.id + '</div><div class="detail-route">' + route(r) + "</div>" +
        '<div class="detail-kind">' + U.esc(U.name("kind", r.kind)) + (r.premium ? " · <b>პრემიუმ: უბანი გენდობა</b>" : "") + "</div></div>" +
      '<div class="detail-reward num">' + U.money(r.reward) + '<div class="detail-note">სავარაუდო ანაზღაურება</div></div>' +
      '<dl class="facts">' +
        "<dt>აღება</dt><dd>" + U.esc(U.name("district", r.from)) + "-ის საწყობი</dd>" +
        "<dt>მიწოდება</dt><dd>" + U.esc(U.name("district", r.to)) + "-ის მაღაზია</dd>" +
        "<dt>ტვირთი</dt><dd>" + U.esc(U.name("cargo", r.cargo)) + "</dd>" +
        '<dt>მანძილი</dt><dd class="num">' + U.dist(r.dist) + "</dd>" +
        '<dt>დარჩენილი დრო</dt><dd class="num" data-t="' + r.id + '">' + U.time(left(r)) + "</dd>" +
        "<dt>მოთხოვნები</dt><dd>" + U.esc(requirement(r)) + "</dd>" +
      "</dl>" +
      '<p class="detail-note">მანქანას საწყობი მოგცემს. საბოლოო ანაზღაურება იმაზეა დამოკიდებული, რა მდგომარეობაში მიიტან ტვირთს.</p>' +
      '<div class="detail-status ' + cls + '">' + U.esc(status) + "</div>" +
      '<div class="detail-actions"><button class="btn btn-primary" id="bd-accept"' + (can && !pending ? "" : " disabled") + ">" +
        (pending === r.id ? "ველოდებით დაფის პასუხს…" : "კონტრაქტის აღება") + "</button></div>";
    var btn = U.el("bd-accept");
    if (can) btn.addEventListener("click", function () { askAccept(r); });
  }

  function askAccept(r) {
    confirmBox = LSSC.confirm(root, {
      title: "აიღებ კონტრაქტს #" + r.id + "?",
      html: "ფურგონი გელოდება <b>" + U.esc(U.name("district", r.from)) + "</b>-ის საწყობთან. ტვირთი მშვიდად და ფრთხილად მიიტანე <b>" +
        U.esc(U.name("district", r.to)) + "</b>-ის მაღაზიაში.<br><br>ერთდროულად მხოლოდ ერთ კონტრაქტზე შეგიძლია მუშაობა. კონტრაქტი ძალაშია კიდევ " + U.time(left(r)) + ".",
      yes: "დადასტურება", no: "გაუქმება",
      onYes: function () {
        confirmBox = null;
        pending = r.id;
        LSSC.emit("lssc:accept", r.id);     // il server rivalida e risponde con lssc:contract + lssc:close, o lssc:reject
        renderDetail();
      },
      onNo: function () { confirmBox = null; }
    });
  }

  function tickTimes() {
    root.querySelectorAll("[data-t]").forEach(function (n) {
      var r = find(U.int(n.getAttribute("data-t")));
      if (!r) return;
      var t = U.time(left(r));
      n.textContent = n.classList.contains("ct-time") ? "დარჩა " + t : t;
    });
  }

  function renderHead() {
    el.sub.innerHTML = atBoard
      ? "დაფასთან ხარ" + (place ? " (<b>" + U.esc(place) + "</b>)" : "") + ": შეგიძლია კონტრაქტის აღება."
      : "დათვალიერების რეჟიმი: კონტრაქტის ასაღებად მიდი დაფასთან.";
  }

  // ---------------------------------------------------------------- eventi server
  // lssc:board_begin atBoard placeName   -> svuota la cache (max 12 righe, come UI_CityBoardBegin)
  LSSC.on("lssc:board_begin", function (at, name) {
    rows = []; atBoard = !!U.int(at); place = U.str(name); pending = 0;
  });
  // lssc:board_add id kind from to cargo reward distanceM secondsLeft premium minLevel minRep minLocalRep blockCode
  LSSC.on("lssc:board_add", function (id, kind, from, to, cargo, reward, dist, secLeft, premium, minLevel, minRep, minLRep, block) {
    id = U.int(id);
    if (id <= 0 || rows.length >= 12) return;
    rows.push({
      id: id, kind: U.int(kind, 1), from: U.int(from), to: U.int(to), cargo: U.int(cargo), reward: Math.max(0, U.int(reward)),
      dist: Math.max(0, U.int(dist)), expAt: Date.now() + Math.max(0, U.int(secLeft)) * 1000, premium: !!U.int(premium),
      minLevel: U.int(minLevel), minRep: U.int(minRep), minLRep: U.int(minLRep), block: U.int(block)
    });
  });
  // lssc:board_end -> ridisegna (la vista si apre con lssc:open board)
  LSSC.on("lssc:board_end", function () { renderHead(); renderList(); });
  // Contratto accettato: il server invia lssc:contract con lo stesso id -> la bacheca si chiude (e restituisce il focus)
  LSSC.on("lssc:contract", function (id) {
    if (pending && U.int(id) === pending) {
      pending = 0;
      if (LSSC.currentView() === "board") LSSC.closeView(true);
    }
  });
  LSSC.on("lssc:reject", function () {
    if (confirmBox) { confirmBox.close(); confirmBox = null; }
    if (pending) { pending = 0; renderDetail(); }
  });

  LSSC.registerView("board", {
    show: function () {
      renderHead(); renderList();
      root.hidden = false;
      if (!tick) tick = setInterval(tickTimes, 1000);
    },
    hide: function () {
      root.hidden = true;
      if (confirmBox) { confirmBox.close(); confirmBox = null; }
      pending = 0;
      if (tick) { clearInterval(tick); tick = 0; }
    },
    onEscape: function () {
      if (confirmBox) { confirmBox.close(); confirmBox = null; return true; }
      return false;
    }
  });
})();
