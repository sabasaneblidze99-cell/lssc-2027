new g_LangKa[33][3] = {
	"a",
	"b",
	"g",
	"d",
	"e",
	"v",
	"z",
	"t",
	"i",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"zh",
	"r",
	"s",
	"t",
	"u",
	"f",
	"q",
	"gh",
	"y",
	"sh",
	"ch",
	"c",
	"dz",
	"w",
	"ch",
	"x",
	"j",
	"h"
};

Lang_Utf8Next(const src[], &i)
{
	new c = src[i] & 255;
	if (c < 128)
	{
		i++;
		return c;
	}
	new need = 0, cp = 0;
	if ((c & 224) == 192)
	{
		need = 1;
		cp = c & 31;
	}
	else if ((c & 240) == 224)
	{
		need = 2;
		cp = c & 15;
	}
	else if ((c & 248) == 240)
	{
		need = 3;
		cp = c & 7;
	}
	else
	{
		i++;
		return -c;
	}
	for (new k = 1; k <= need; k++)
	{
		new b = src[i + k] & 255;
		if ((b & 192) != 128)
		{
			i++;
			return -c;
		}
		cp = cp << 6 | b & 63;
	}
	i += need + 1;
	return cp;
}

Lang_Translit(const src[], dest[], size)
{
	new i = 0, o = 0;
	while (src[i] != 0 && o < size - 1)
	{
		new cp = Lang_Utf8Next(src, i);
		if (cp < 0)
		{
			dest[o++] = -cp;
			continue;
		}
		if (cp < 128)
		{
			dest[o++] = cp;
			continue;
		}
		new idx = -1;
		if (cp >= 4304 && cp <= 4336)
		{
			idx = cp - 4304;
		}
		else if (cp >= 7312 && cp <= 7344)
		{
			idx = cp - 7312;
		}
		if (idx >= 0)
		{
			for (new k = 0; g_LangKa[idx][k] != 0 && o < size - 1; k++)
			{
				dest[o++] = g_LangKa[idx][k];
			}
			continue;
		}
		switch (cp)
		{
			case 8211, 8212, 8722:
			{
				dest[o++] = 45;
			}
			case 171, 187, 8220 .. 8222:
			{
				dest[o++] = 34;
			}
			case 8216, 8217:
			{
				dest[o++] = 39;
			}
			case 8230:
			{
				for (new k = 0; k < 3 && o < size - 1; k++)
				{
					dest[o++] = 46;
				}
			}
			case 160:
			{
				dest[o++] = 32;
			}
			default:
			{
				dest[o++] = 63;
			}
		}
	}
	dest[o] = 0;
}

bool:Lang_ShowPlayerDialog(playerid, dialogid, t_DIALOG_STYLE:style, const title[], const body[], const button1[], const button2[])
{
	static cap[128], b1[64], b2[64], out[4096];
	Lang_Translit(title, cap, 128);
	Lang_Translit(body, out, 4096);
	Lang_Translit(button1, b1, 64);
	Lang_Translit(button2, b2, 64);
	return ShowPlayerDialog(playerid, dialogid, style, cap, out, b1, b2);
}

PlayerText:Lang_CreatePlayerTextDraw(playerid, Float:x, Float:y, text[])
{
	static out[1024];
	Lang_Translit(text, out, 1024);
	return CreatePlayerTextDraw(playerid, x, y, "%s", out);
}

bool:Lang_PlayerTextDrawSetString(playerid, PlayerText:textid, text[])
{
	static out[1024];
	Lang_Translit(text, out, 1024);
	return PlayerTextDrawSetString(playerid, textid, "%s", out);
}

Text3D:Lang_Create3DTextLabel(text[], colour, Float:x, Float:y, Float:z, Float:drawDistance, virtualWorld, bool:testLOS)
{
	static out[512];
	Lang_Translit(text, out, 512);
	return Create3DTextLabel(out, colour, x, y, z, drawDistance, virtualWorld, testLOS);
}

bool:Lang_Update3DTextLabelText(Text3D:textid, colour, text[])
{
	static out[512];
	Lang_Translit(text, out, 512);
	return Update3DTextLabelText(textid, colour, out);
}
