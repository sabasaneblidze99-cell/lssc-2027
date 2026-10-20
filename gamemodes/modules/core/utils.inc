new g_UtilSaltChars[63];

Util_Send(playerid, color, text[])
{
	static out[512];
	Lang_Translit(text, out, 512);
	return SendClientMessage(playerid, color, "%s", out);
}

bool:Util_IsPlaying(playerid)
{
	if (playerid < 0 || playerid >= 1000)
	{
		return 0;
	}
	if (!IsPlayerConnected(playerid) || IsPlayerNPC(playerid))
	{
		return 0;
	}
	return gPlayer[playerid][PLAYER_DATA_0] && gPlayer[playerid][PLAYER_DATA_1];
}

Util_GetAdmin(playerid)
{
	if (playerid < 0 || playerid >= 1000 || !gPlayer[playerid][PLAYER_DATA_0])
	{
		return 0;
	}
	return gPlayer[playerid][PLAYER_DATA_86];
}

Util_Trim(str[])
{
	new len = strlen(str);
	while (len > 0 && str[len - 1] <= 32)
	{
		str[--len] = 0;
	}
	new start = 0;
	while (str[start] != 0 && str[start] <= 32)
	{
		start++;
	}
	if (start > 0)
	{
		strdel(str, 0, start);
	}
}

bool:Util_IsHexChar(c)
{
	return (c >= 48 && c <= 57) || (c >= 97 && c <= 102) || (c >= 65 && c <= 70);
}

bool:Util_IsColorCodeAt(str[], pos)
{
	if (str[pos] != 123)
	{
		return 0;
	}
	for (new i = 1; i <= 6; i++)
	{
		if (!Util_IsHexChar(str[pos + i]))
		{
			return 0;
		}
	}
	return str[pos + 7] == 125;
}

Util_StripColorCodes(str[])
{
	new i = 0;
	while (str[i] != 0)
	{
		if (Util_IsColorCodeAt(str, i))
		{
			strdel(str, i, i + 8);
			i -= i > 8 ? 8 : i;
		}
		else
		{
			i++;
		}
	}
}

bool:Util_SanitizeText(src[], dest[], maxLen)
{
	new tmp[256];
	strmid(tmp, src, 0, 255, 256);
	Util_StripColorCodes(tmp);
	Util_Trim(tmp);
	if (strlen(tmp) > maxLen)
	{
		tmp[maxLen] = 0;
		Util_Trim(tmp);
	}
	strmid(dest, tmp, 0, maxLen, maxLen + 1);
	return dest[0] != 0;
}

bool:Util_ParseInt(str[], &value)
{
	new i = 0, digits = 0, bool:neg = 0;
	value = 0;
	if (str[0] == 45)
	{
		neg = 1;
		i++;
	}
	for (;str[i] != 0; i++)
	{
		if (str[i] < 48 || str[i] > 57)
		{
			return 0;
		}
		new d = str[i] - 48;
		if (value > (0x7FFFFFFF - d) / 10)
		{
			return 0;
		}
		value = value * 10 + d;
		if (++digits > 10)
		{
			return 0;
		}
	}
	if (digits == 0)
	{
		return 0;
	}
	if (neg)
	{
		value = -value;
	}
	return 1;
}

Util_NextToken(src[], &pos, dest[], size)
{
	while (src[pos] == 32)
	{
		pos++;
	}
	new n = 0;
	while (src[pos] != 0 && src[pos] != 32)
	{
		if (n < size - 1)
		{
			dest[n++] = src[pos];
		}
		pos++;
	}
	dest[n] = 0;
	return n;
}

bool:Util_ParsePlayerId(token[], &id)
{
	if (!Util_ParseInt(token, id))
	{
		return 0;
	}
	return id >= 0 && id < 1000;
}

bool:Util_IsValidRPName(name[])
{
	new us = -1, len = strlen(name);
	for (new i = 0; i < len; i++)
	{
		new c = name[i];
		if (c == 95)
		{
			if (us != -1)
			{
				return 0;
			}
			us = i;
			continue;
		}
		if (c < 65 || (c > 90 && c < 97) || c > 122)
		{
			return 0;
		}
	}
	if (us < 2 || len - us - 1 < 2)
	{
		return 0;
	}
	return name[0] >= 65 && name[0] <= 90 && name[us + 1] >= 65 && name[us + 1] <= 90;
}

Util_SplitText(text[], out[][], maxLines)
{
	new len = strlen(text), start = 0, count = 0;
	while (start < len && count < maxLines)
	{
		new take = len - start;
		if (take > 110)
		{
			take = 110;
			for (new i = 110; i > 55; i--)
			{
				if (text[start + i] == 32)
				{
					take = i;
					break;
				}
			}
		}
		strmid(out[count], text, start, start + take, 128);
		start += take;
		if (text[start] == 32)
		{
			start++;
		}
		count++;
	}
	return count;
}

Util_SendNear(sourceid, color, Float:radius, text[])
{
	new lines[3][128];
	new count = Util_SplitText(text, lines, 3);
	if (count < 1)
	{
		return 0;
	}
	new Float:x, Float:y, Float:z;
	GetPlayerPos(sourceid, x, y, z);
	new interior = GetPlayerInterior(sourceid), world = GetPlayerVirtualWorld(sourceid);
	for (new i = 0; i < 1000; i++)
	{
		if (!gPlayer[i][PLAYER_DATA_1] || !Util_IsPlaying(i))
		{
			continue;
		}
		if (GetPlayerInterior(i) != interior || GetPlayerVirtualWorld(i) != world)
		{
			continue;
		}
		if ((Float:(GetPlayerDistanceFromPoint(i, x, y, z)) > Float:(radius)))
		{
			continue;
		}
		for (new l = 0; l < count; l++)
		{
			SendClientMessage(i, color, "%s%s%s", l > 0 ? "... " : "", lines[l], l < count - 1 ? " ..." : "");
		}
	}
}

bool:Util_InRange(playerid, targetid, Float:radius)
{
	if (GetPlayerInterior(playerid) != GetPlayerInterior(targetid))
	{
		return 0;
	}
	if (GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(targetid))
	{
		return 0;
	}
	new Float:x, Float:y, Float:z;
	GetPlayerPos(targetid, x, y, z);
	return (Float:(GetPlayerDistanceFromPoint(playerid, x, y, z)) <= Float:(radius));
}
