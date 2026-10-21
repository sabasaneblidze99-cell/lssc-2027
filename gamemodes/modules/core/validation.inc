new g_ValidSkins[2][7] = {
	{7, 14, 17, 23, 29, 60, 72},
	{9, 10, 11, 12, 13, 31, 39}
};

Valid_SkinAt(gender, index)
{
	if (index < 0 || index >= 7)
	{
		return -1;
	}
	return g_ValidSkins[gender == 1 ? 1 : 0][index];
}

bool:Valid_SkinAllowed(gender, skin)
{
	for (new i = 0; i < 7; i++)
	{
		if (Valid_SkinAt(gender, i) == skin)
		{
			return true;
		}
	}
	return false;
}

bool:Valid_PasswordLength(const pw[])
{
	new len = strlen(pw);
	return len >= 6 && len <= 32;
}

Valid_PayAmount(amount, senderMoney, receiverMoney)
{
	if (amount <= 0)
	{
		return 1;
	}
	if (senderMoney < amount)
	{
		return 2;
	}
	if (receiverMoney > 0x7FFFFFFF - amount)
	{
		return 4;
	}
	if (receiverMoney + amount > ECON_MONEY_MAX)
	{
		return 3;
	}
	return 0;
}

bool:Valid_Email(const s[])
{
	new len = strlen(s);
	if (len < 6 || len > 254)
	{
		return false;
	}
	new at = -1;
	for (new i = 0; i < len; i++)
	{
		if (s[i] == 64)
		{
			if (at != -1)
			{
				return false;
			}
			at = i;
		}
	}
	if (at < 1 || at > 64 || at >= len - 4)
	{
		return false;
	}
	for (new i = 0; i < at; i++)
	{
		new c = s[i];
		if (!((c >= 97 && c <= 122) || (c >= 65 && c <= 90) || (c >= 48 && c <= 57) || c == 46 || c == 95 || c == 37 || c == 43 || c == 45))
		{
			return false;
		}
	}
	if (s[0] == 46 || s[at - 1] == 46)
	{
		return false;
	}
	new dots = 0, label = 0, lastDot = -1;
	for (new i = at + 1; i < len; i++)
	{
		new c = s[i];
		if (c == 46)
		{
			if (label == 0)
			{
				return false;
			}
			dots++;
			label = 0;
			lastDot = i;
			continue;
		}
		if (!((c >= 97 && c <= 122) || (c >= 65 && c <= 90) || (c >= 48 && c <= 57) || c == 45))
		{
			return false;
		}
		label++;
	}
	if (dots < 1 || label < 2)
	{
		return false;
	}
	for (new i = lastDot + 1; i < len; i++)
	{
		new c = s[i];
		if (!((c >= 97 && c <= 122) || (c >= 65 && c <= 90)))
		{
			return false;
		}
	}
	return true;
}
