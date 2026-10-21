// Test di economy/currency.inc nella VM Pawn standard (pawnrun): formato, parsing, limiti.
// Compilazione: pawncc currency_test.pwn -i<pawn-src>/include -i../../gamemodes  ;  esecuzione: pawnrun currency_test.amx
#include <core>
#include <console>
#include <string>
#define format(%0,%1, strformat(%0,%1,false,
#include "modules/economy/currency.inc"
new g_Fail = 0, g_Ok = 0;
CheckFmt(v, const want[])
{
	new s[32];
	Money_FormatSym(v, s, sizeof s);
	if (strcmp(s, want) != 0) { printf("FAIL format %d -> '%s' (atteso '%s')\n", v, s, want); g_Fail++; } else g_Ok++;
}
CheckParse(const txt[], bool:okWant, want)
{
	new v;
	new bool:ok = Money_Parse(txt, v);
	if (ok != okWant || (ok && v != want)) { printf("FAIL parse '%s' -> %d/%d (atteso %d/%d)\n", txt, ok, v, okWant, want); g_Fail++; } else g_Ok++;
}
main()
{
	CheckFmt(0, "₾0");
	CheckFmt(5, "₾0,05");
	CheckFmt(550, "₾5,50");
	CheckFmt(125050, "₾1.250,50");
	CheckFmt(8500000, "₾85.000");
	CheckFmt(-4550, "-₾45,50");
	CheckFmt(1000000000, "₾10.000.000");
	CheckFmt(cellmin, "-₾21.474.836,47");
	CheckParse("25", true, 2500);
	CheckParse("12,5", true, 1250);
	CheckParse("12.50", true, 1250);
	CheckParse("0,05", true, 5);
	CheckParse("0", true, 0);
	CheckParse("10000000", true, 1000000000);
	CheckParse("10000000,01", false, 0);
	CheckParse("99999999999", false, 0);
	CheckParse("1.250,50", false, 0);
	CheckParse("12,", false, 0);
	CheckParse(",5", false, 0);
	CheckParse("12,505", false, 0);
	CheckParse("-5", false, 0);
	CheckParse("abc", false, 0);
	CheckParse("", false, 0);
	if (Money_GtaDisplay(125050) != 1250) { print("FAIL GtaDisplay\n"); g_Fail++; } else g_Ok++;
	if (Money_Pct(1000000000, 70) != 700000000) { print("FAIL Pct\n"); g_Fail++; } else g_Ok++;
	printf("currency: %d ok, %d falliti\n", g_Ok, g_Fail);
}
