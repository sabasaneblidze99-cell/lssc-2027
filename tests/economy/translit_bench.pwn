#include <core>
#include <console>
#include <string>

#define format(%0,%1, strformat(%0,%1,false,
#include "modules/economy/currency.inc"
#include "modules/core/lang.inc"
main()
{
	new src[600], out[1024];
	strcat(src, "{FFFFFF}შესვლა დასრულებულია. მოგესალმებით ლოს-სანტოსში! ბანკი: ₾1.250,50 - ნაღდი ფული: ₾300 - ");
	strcat(src, "Welcome to Los Santos Street Crime's, enjoy the roleplay and follow the rules /help /gps /casa ");
	strcat(src, "საბანკო ანგარიში, სამსახური, ტაქსი, სახლი, მანქანა, ბენზინგასამართი სადგური, სუპერმარკეტი.");

	for (new k = 0; k < 20000; k++) Lang_Translit(src, out, sizeof out);
	printf("len %d -> %d, 2000 translit: %d ms\n", strlen(src), strlen(out), 0);
	printf("%s\n", out);
}
