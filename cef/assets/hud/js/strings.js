/* strings.js - testi dell'HUD in georgiano (Mkhedruli). Le chiavi sono in inglese per il team.
   BOZZA da far rileggere a un madrelingua, come i18n.js. */
(function () {
  "use strict";
  var HUD = window.HUD = window.HUD || {};
  HUD.S = {
    online: "ონლაინ",
    id: "ID",
    hp: "ჯანმრთელობა", armor: "ჯავშანი", hunger: "სიმაძღრე", thirst: "წყურვილი",
    cash: "ნაღდი", bank: "ბანკი",
    level: "დონე",
    maxLevel: "მაქსიმალური დონე",
    levelUp: "ახალი დონე",

    notifications: "შეტყობინებები",
    newCount: function (n) { return n + " ახალი"; },
    ntTitle: {
      success: "წარმატება", info: "ინფორმაცია", warn: "ყურადღება", error: "შეცდომა",
      "money-in": "თანხა მიღებულია", "money-out": "თანხა დაიხარჯა",
      mission: "დავალება", job: "სამსახური", rp: "როლური ქმედება"
    },
    ago: function (s) {
      if (s < 45) return "ახლა";
      if (s < 3600) return Math.round(s / 60) + " წთ წინ";
      return Math.round(s / 3600) + " სთ წინ";
    },

    speedUnit: "კმ/სთ", fuel: "საწვავი", engine: "ძრავა", lights: "ფარები", locked: "დაკეტილია", damage: "დაზიანება",

    city: "ლოს-სანტოსი",
    districts: ["ლოს-სანტოსი", "აიდლვუდი", "განტონი", "ჯეფერსონი"],
    indoor: "შენობაში",
    safe: "უსაფრთხო ზონა", neutral: "მშვიდი ზონა", danger: "საშიში ზონა",
    voiceOff: "ხმოვანი ჩატი არ არის",
    voiceNoMic: "მიკროფონი ვერ მოიძებნა",
    voiceReady: function (k) { return "საუბარი: " + k; },

    chatTabs: [
      ["all", "ყველა"], ["ic", "IC"], ["ooc", "OOC"], ["faction", "ფრაქცია"], ["job", "სამსახური"],
      ["ads", "განცხადება"], ["system", "სისტემა"], ["emergency", "911"], ["admin", "ადმინი"]
    ],
    chatTag: {
      ic: "", ooc: "[OOC]", faction: "[ფრაქცია]", job: "[სამსახური]", ads: "[განცხადება]",
      system: "[ინფო]", emergency: "[911]", admin: "[ადმინი]"
    },
    chatPlaceholder: "დაწერე შეტყობინება ან /ბრძანება…",
    chatHint: "Enter - გაგზავნა · Esc - დახურვა · PgUp/PgDn - გადახვევა",

    interact: "ურთიერთქმედება",

    quickTitle: "სწრაფი მოქმედებები",
    quickSub: "აირჩიე მოქმედება",
    quickFoot: ["N - გახსნა", "Esc - დახურვა"],
    quick: {
      inventory: "ინვენტარი", phone: "ტელეფონი", map: "რუკა",
      anims: "ანიმაციები", vehicles: "მანქანები", settings: "პარამეტრები"
    },

    settingsTitle: "HUD-ის პარამეტრები",
    settingsSub: "ინახება მხოლოდ ამ კომპიუტერზე",
    chatLines: "ჩატის ხაზები",
    chatOpacity: "ჩატის სიმკვრივე",
    timestamps: "დრო ჩატში",
    chatFade: "ჩატის ავტომატური ჩაქრობა",
    hudScale: "HUD-ის ზომა",
    save: "შენახვა", close: "დახურვა", reset: "საწყისი",

    contract: function (n) { return "კონტრაქტი #" + n; },
    accepted: "მიღებულია", delivering: "მიწოდება",
    timeLeft: "დარჩენილი დრო", integrity: "ტვირთის მთლიანობა"
  };
})();
