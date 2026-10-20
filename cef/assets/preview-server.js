/* preview-server.js - SOLO ANTEPRIMA/TEST: simula il gamemode con dati finti.
   Non va caricato da index.html. Usato da preview.html (via postMessage) e dai test headless (finto window.cef).
   createLsscSim(send): send(nomeEvento, argomenti[]) consegna un evento server->pagina; sim.handle(nome, argomenti[]) riceve pagina->server
   (gli argomenti arrivano come li riceverebbe il Pawn: una stringa separata da spazi). */
(function (global) {
  "use strict";

  function createLsscSim(send) {
    var S = {
      money: 12450, bank: 48210, level: 7, expIn: 340, expStep: 520, hour: 19, minute: 42,
      contract: null, atBoard: true, accepted: {}, log: []
    };
    var BOARD = [
      // id kind from to cargo reward dist sec premium minLevel minRep minLRep block
      [1042, 1, 1, 2, 2, 640, 1850, 1260, 0, 1, 0, 0, 0],
      [1043, 1, 3, 1, 3, 910, 2600, 940, 1, 3, 0, 20, 0],
      [1044, 1, 2, 3, 1, 520, 1400, 1580, 0, 1, 0, 0, 0],
      [1045, 1, 1, 3, 3, 1180, 3100, 610, 0, 5, 100, 0, 2],
      [1046, 1, 2, 1, 2, 480, 1200, 1720, 0, 1, 0, 0, 0],
      [1047, 1, 3, 2, 1, 760, 2200, 820, 0, 9, 0, 0, 1],
      [1048, 1, 1, 2, 1, 590, 1700, 300, 0, 1, 0, 0, 9],
      [1049, 1, 2, 3, 3, 1340, 3400, 1100, 1, 6, 0, 25, 3]
    ];
    var DISTRICTS = {
      1: [1, 520, 64, 38, 24, 0, 1, 1, 5, 0, "La panetteria di Idlewood ha riaperto dopo l'estate.",
          [[1, 240, 12], [7, 1500, 0], [1, 3600, 12], [3, 7400, 1], [8, 20000, 100]]],
      2: [2, 290, 41, 74, -6, 0, 0, -1, 0, 1, "Al campetto di Grove Street stanno sistemando le reti.",
          [[6, 900, -8], [1, 2100, 12], [2, 5400, -5]]],
      3: [3, 690, 58, 45, 3, 48, 2, 0, 0, 0, "Il mercato del sabato a Jefferson e' tornato pieno.",
          [[1, 120, 12], [4, 1600, 0], [5, 3000, 0], [1, 4800, 12]]]
    };

    function out(name) { send(name, Array.prototype.slice.call(arguments, 1)); }
    function hud() { out("lssc:hud", S.money, S.bank, S.level, S.expIn, S.expStep, S.hour, S.minute); }
    function contract() {
      var c = S.contract;
      if (!c) { out("lssc:contract", 0, 0, "", "", -1, -1, -1, 0, 0, 0, 0, 0); return; }
      out("lssc:contract", c.id, c.state, c.objective, c.dest, c.dist, c.sec, c.integ, c.from, c.to, c.cargo, c.reward, 8);
    }
    function board(open) {
      out("lssc:board_begin", S.atBoard ? 1 : 0, S.atBoard ? "Casa del Quartiere di Idlewood" : "");
      BOARD.forEach(function (r) {
        var row = r.slice();
        if (S.contract && row[12] === 0) row[12] = 4;      // un solo contratto attivo
        if (S.accepted[row[0]]) row[12] = 9;
        out.apply(null, ["lssc:board_add"].concat(row));
      });
      out("lssc:board_end");
      if (open) out("lssc:open", "board", "");
    }
    function district(id) {
      var d = DISTRICTS[id] || DISTRICTS[1];
      out("lssc:district_begin", d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9]);
      out("lssc:district_rumor", d[10]);
      d[11].forEach(function (e) { out("lssc:district_event", e[0], e[1], e[2]); });
      out("lssc:district_end");
    }
    function rep() {
      out("lssc:rep_begin");
      out("lssc:rep_track", 1, 214, 31, 2, 0, 0, 12, 1, 300, 5);
      out("lssc:rep_track", 2, 64, 9, 0, 41, 9, 4, 0, 100, 0);
      out("lssc:rep_local", 1, 24, 1, 5, 1);
      out("lssc:rep_local", 2, -6, -1, 0, 0);
      out("lssc:rep_local", 3, 3, 0, 0, 0);
      out("lssc:rep_end");
    }
    function bankRows() {
      out("lssc:bank_begin");
      out("lssc:bank_row", "19/10 19:20", "Contratto", "Bacheca Idlewood", 640, 48210);
      out("lssc:bank_row", "19/10 18:05", "Bonifico", "Lucia_Russo", -250, 47570);
      out("lssc:bank_row", "19/10 17:40", "Stipendio", "Job Center", 420, 47820);
      out("lssc:bank_row", "18/10 22:12", "Prelievo", "ATM Idlewood", -500, 47400);
      out("lssc:bank_end");
    }

    var sim = {
      state: S,
      // pagina -> server (args = stringa separata da spazi, come cef_subscribe)
      handle: function (name, argString) {
        var a = String(argString || "").split(" ").filter(Boolean);
        S.log.push(name + (a.length ? " " + a.join(" ") : ""));
        switch (name) {
          case "lssc:ready":
            hud(); contract();
            out("lssc:profile", "Marco_Ferri", "Corriere");
            out("lssc:msg", "Officina di Ganton", "Il furgone e' pronto, passa quando vuoi.", 5400, 0);
            out("lssc:msg", "Casa del Quartiere", "Nuovi contratti premium per chi e' di casa a Idlewood.", 1800, 1);
            out("lssc:msg", "Lucia Russo", "Grazie per la consegna di ieri, il negozio ha riaperto!", 600, 1);
            break;
          case "lssc:board_refresh": board(true); break;
          case "lssc:accept": {
            var id = parseInt(a[0], 10), r = BOARD.filter(function (x) { return x[0] === id; })[0];
            if (!r || r[12] !== 0 || S.contract || !S.atBoard) { out("lssc:reject", "Contratto non disponibile: la bacheca e' cambiata."); board(false); break; }
            S.accepted[id] = true;
            S.contract = { id: id, state: 1, objective: "Raggiungi il magazzino di " + ["", "Idlewood", "Ganton", "Jefferson"][r[2]] + " e carica la merce",
              dest: "Magazzino di " + ["", "Idlewood", "Ganton", "Jefferson"][r[2]], dist: 640, sec: r[7], integ: -1, from: r[2], to: r[3], cargo: r[4], reward: r[5] };
            contract();
            out("lssc:close", "board");
            out("lssc:toast", "Contratto #" + id + " accettato: il furgone ti aspetta.", 1);
            break;
          }
          case "lssc:abandon":
            if (!S.contract) { out("lssc:reject", "Non hai nessun contratto da lasciare."); break; }
            S.contract = null; contract();
            out("lssc:toast", "Hai lasciato il contratto: -8 reputazione trasporti.", 2);
            break;
          case "lssc:district": district(parseInt(a[0], 10) || 1); break;
          case "lssc:project_open": {
            var did = parseInt(a[0], 10) || 1;
            if (did === 1) out("lssc:project", 7, 1, "Riparazione del campetto di Idlewood", 6000, 3920, 118, 50, 500, 400, 0);
            else if (did === 2) out("lssc:project", 8, 2, "Nuove luci in Grove Street", 6000, 1240, 150, 50, 500, 0, 2);
            else out("lssc:project", 0, 3, "", 0, 0, 0, 0, 0, 0, 0);
            break;
          }
          case "lssc:contribute":
            out("lssc:toast", "Grazie: hai donato $" + a[1] + " al progetto.", 1);
            out("lssc:project", 7, 1, "Riparazione del campetto di Idlewood", 6000, 3920 + (parseInt(a[1], 10) || 0), 118, 50, 500, Math.max(0, 400 - (parseInt(a[1], 10) || 0)), 0);
            break;
          case "lssc:bank_req": bankRows(); break;
          case "lssc:rep_req": rep(); break;
          case "lssc:job_waypoint": out("lssc:toast", "Job Center segnato sulla mappa.", 0); break;
          case "lssc:close": break;
          case "lssc:chat_send": {
            var hex = a[0] || "", bytes = [];
            for (var i = 0; i + 1 < hex.length; i += 2) bytes.push(parseInt(hex.substr(i, 2), 16));
            var txt = new TextDecoder().decode(new Uint8Array(bytes));
            if (txt.charAt(0) === "/") out("lssc:chat", 0, "{FF4D65}ბრძანება ვერ მოიძებნა: " + txt);
            else out("lssc:chat", 1, "{F6F3FA}Tyrese Wilson: " + txt);
            break;
          }
          case "lssc:quick":
            out("lssc:notify", 1, "", "მოქმედება: " + a[0], 2500);
            break;
        }
      },
      // ---- HUD Glass UI: dati dimostrativi (SOLO anteprima)
      hudDemo: function () {
        out("lssc:hud_visible", 1);
        out("lssc:identity", "Tyrese_Wilson", 24);
        out("lssc:clock", 20, 42, 20, 10, 2026);
        out("lssc:status", 100, 80, 72, -1);
        out("lssc:hud", 19354, 328930, 12, 320, 1000, 20, 42);
        out("lssc:zone", 1, 0, 0);
        out("lssc:voice", 2, "B");
        out("lssc:chat_mode", 1);
        var lines = [
          [0, "გადახდა მიღებულია: $250 (ხელფასი)."],
          [4, "{FFB547}Carlo_Russo:{F6F3FA} ტაქსი მზადაა, დარეკე /chiamataxi"],
          [5, "იყიდება Sultan სრული ტუნინგით - 245678"],
          [6, "ჩხუბი აიდლვუდში, სასწრაფოდ!"],
          [3, "{5BA8FF}Jamal_Brooks:{F6F3FA} ყველა ბლოკთან?"],
          [7, "Luca_Vitali: ადმინი, დამეხმარე გთხოვ"],
          [2, "(( Tyrese_Wilson: ბიჭებო, კარგი თამაშია ))"],
          [1, "Tyrese Wilson ამბობს: გამარჯობა, როგორ ხარ?"]
        ];
        lines.forEach(function (l) { out("lssc:chat", l[0], l[1]); });
        out("lssc:notify", 4, "", "ხელფასი · სამსახური", 9000);
        out("lssc:notify", 1, "უსაფრთხოების ღვედი", "ჩართულია", 9000);
        out("lssc:notify", 2, "მშიერი ხარ", "იპოვე ადგილი საჭმელად.", 9000);
        out("lssc:notify", 3, "არასწორი ბრძანება", "შეამოწმე სინტაქსი.", 9000);
      },
      vehicleDemo: function (on) {
        if (S.vehTimer) { clearInterval(S.vehTimer); S.vehTimer = 0; }
        if (on === false) { out("lssc:veh_show", 0, ""); return; }
        out("lssc:veh_show", 1, "Premier");
        var t = 0;
        out("lssc:veh", 86, 87, 1, 1, 92, 0);
        S.vehTimer = setInterval(function () {
          t += 0.25;
          var sp = Math.round(86 + 40 * Math.sin(t / 2));
          out("lssc:veh", sp, 87, 1, 1, 92, 0);
        }, 250);
      },
      chatOpen: function () { out("lssc:chat_open", ""); },
      quickOpen: function () { out("lssc:quick_open", 1 | 2 | 8 | 16); },
      interactDemo: function (on) {
        if (on === false) { out("lssc:interact", 0, "", "", -1); return; }
        out("lssc:interact", 1, "Y", "ბანკომატი", -1);
      },
      moneyDemo: function () { S.money += 250; out("lssc:hud", S.money, S.bank, S.level, S.expIn, S.expStep, 20, 43); out("lssc:money_delta", 250, 0); },
      levelDemo: function () { out("lssc:levelup", 13); },
      dangerDemo: function () { out("lssc:zone", 2, 2, 0); },
      // comandi dell'anteprima (equivalenti a comandi/tasti lato server)
      hud: hud, contract: contract, board: board, district: district, rep: rep,
      openPhone: function (appName) { out("lssc:open", "phone", appName || "home"); },
      startDelivery: function () {
        if (!S.contract) S.contract = { id: 1042, state: 1, objective: "", dest: "", dist: 0, sec: 1260, integ: -1, from: 1, to: 2, cargo: 2, reward: 640 };
        var c = S.contract;
        c.state = 2; c.objective = "Carico a bordo: consegnalo al negozio di " + ["", "Idlewood", "Ganton", "Jefferson"][c.to];
        c.dest = "Negozio di " + ["", "Idlewood", "Ganton", "Jefferson"][c.to]; c.dist = 1320; c.integ = 86; c.sec = Math.min(c.sec, 754);
        contract();
      },
      clearContract: function () { S.contract = null; contract(); },
      toast: function (t, k) { out("lssc:toast", t, k); },
      close: function () { out("lssc:close", ""); },
      setAtBoard: function (v) { S.atBoard = !!v; }
    };
    return sim;
  }

  global.createLsscSim = createLsscSim;
})(typeof window !== "undefined" ? window : this);
