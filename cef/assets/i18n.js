/* i18n.js - testi dell'interfaccia CEF in georgiano (docs/LOCALIZATION_KA.md).
   BOZZA: da far rileggere a un madrelingua. Le chiavi restano in italiano per il team; i valori sono quello che vede il giocatore.
   Nelle pagine CEF il georgiano e' quello vero (Mkhedruli). Nella chat SA-MP il server lo traslittera in latino. */
(function () {
  "use strict";
  var LSSC = window.LSSC = window.LSSC || {};
  LSSC.lang = "ka";
  LSSC.T = {
    // marchio
    brandKicker: "ლოს-სანტოსი · 2027",
    brandSub: "ქართული როლური სერვერი",
    slogan: "ქალაქს არაფერი ავიწყდება",
    enterHint: "Enter - დასადასტურებლად",
    music: "მუსიკა",
    musicOff: "ხმა გამორთულია",
    // percorso
    steps: ["ანგარიში", "პერსონაჟი", "ისტორია", "ჩამოსვლა"],
    // accesso
    loginTitle: "შესვლა",
    loginSub: "კეთილი იყოს შენი დაბრუნება. აიდლვუდი გელოდება.",
    character: "პერსონაჟი",
    fromLauncher: "ლაუნჩერიდან",
    password: "პაროლი",
    show: "ჩვენება",
    hide: "დამალვა",
    enterCity: "ქალაქში შესვლა",
    attemptsLeft: "დარჩენილი მცდელობა: ",
    forgot: "დაგავიწყდა პაროლი?",
    forgotHelp: "მიმართე ადმინისტრაციას Discord-ზე - დაგეხმარებიან ანგარიშის აღდგენაში.",
    notYourName: "ეს შენი სახელი არ არის?",
    notYourNameSub: "შეცვალე SA-MP ლაუნჩერში და ხელახლა შემოდი.",
    quit: "გასვლა",
    typePassword: "შეიყვანე პაროლი.",
    badChars: "პაროლში დაუშვებელი სიმბოლოებია. გამოიყენე ლათინური ასოები, ციფრები და ჩვეულებრივი სიმბოლოები.",
    checking: "მოწმდება…",
    // registrazione
    registerTitle: "ახალი მოქალაქე",
    registerSub: "სახელი ლაუნჩერში აირჩიე. ახლა დაიცავი.",
    validName: "სახელი_გვარი ✓",
    choosePassword: function (a, b) { return "აირჩიე პაროლი (" + a + "-" + b + " სიმბოლო)"; },
    repeatPassword: "გაიმეორე პაროლი",
    emailOptional: "ელფოსტა (არასავალდებულო)",
    emailWhy: "აღდგენისთვის",
    emailBad: "ელფოსტის მისამართი არასწორია (ან დატოვე ცარიელი).",
    rules: "წავიკითხე წესები: აქ ყოველთვის როლურად ვთამაშობთ.",
    next: "გაგრძელება →",
    firstTime: "პირველად ხარ ლოს-სანტოსში?",
    firstTimeSub: "ავტობუსი იუნიტი სთეიშენისკენ მალე გადის.",
    pwLength: function (a, b) { return "პაროლი უნდა შეიცავდეს " + a + "-დან " + b + "-მდე სიმბოლოს."; },
    pwMismatch: "პაროლები არ ემთხვევა.",
    acceptRules: "დაადასტურე, რომ წაიკითხე წესები.",
    creating: "ანგარიში იქმნება…",
    latinOnly: "პაროლისთვის გამოიყენე ლათინური ასოები: ქართული ასოები SA-MP-ში არ მუშაობს.",
    // genere
    yourCharacter: "შენი პერსონაჟი",
    arriving: function (name) { return name + " მალე ჩამოვა ლოს-სანტოსში. ვინ არის?"; },
    man: "მამაკაცი",
    woman: "ქალი",
    forward: "შემდეგი",
    // aspetto
    lookTitle: "გარეგნობა",
    lookSub: "ასე დაგინახავენ, როცა ავტობუსიდან ჩამოხვალ.",
    look: "ვარიანტი",
    model: "მოდელი",
    confirm: "დადასტურება",
    // storia
    originTitle: "საიდან ხარ",
    originSub: "ყველას ისტორია შორს იწყება. შენი სად?",
    motiveTitle: "რატომ ლოს-სანტოსი",
    motiveSub: "რამ გადაგაწყვეტინა იმ ავტობუსში ჩაჯდომა?",
    storyTitle: "შენი ისტორია",
    storySub: "ეს მხოლოდ ისტორიაა: თამაშში შესაძლებლობებს არ ცვლის. ირჩევა მხოლოდ ერთხელ.",
    preferNot: "არ მინდა თქმა",
    preferNotSub: "წარსული შენი რჩება.",
    back: "უკან",
    boardBus: "ავტობუსში ჩაჯდომა",
    leaving: "გავდივართ…",
    pastUnknown: "წარსული უცნობია.",
    originUnknown: "წარმოშობა უცნობია."
  };
})();
