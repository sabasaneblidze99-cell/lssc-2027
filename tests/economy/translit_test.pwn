// ₾ nell'interfaccia SA-MP: Lang_Translit deve dare "GEL 1.250,50"
#include <core>
#include <console>
#include <string>
#define format(%0,%1, strformat(%0,%1,false,
#include "modules/economy/currency.inc"
#include "modules/core/lang.inc"
main()
{
	new s[64], out[96];
	Money_FormatSym(125050, s, sizeof s);
	strcat(s, " ლარი");
	Lang_Translit(s, out, sizeof out);
	printf("[%s]\n", out);
	Lang_Translit("ფასი: ₾85.000", out, sizeof out);
	printf("[%s]\n", out);
}
