new g_UiSkinGender[MAX_PLAYERS];

UI_ShowRegisterEx(playerid, error[])
{
	if (CefUI_Active(playerid))
	{
		CefUI_AuthStep(playerid, "register", error[0] != 0 ? "პაროლი არ გამოდგება: შეამოწმე სიგრძე." : "", 0);
		return 0;
	}
	new body[1024];
	format(body, 1024, "{FFFFFF}მოგესალმებით {33AA33}Los Santos Street Crime's{FFFFFF}-ზე!\nეს სახელი ჯერ არ არის რეგისტრირებული.\n\n%sაირჩიე პაროლი (%d-%d სიმბოლო):", error, 6, 32);
	Lang_ShowPlayerDialog(playerid, 9001, 3, "რეგისტრაცია", body, "რეგისტრაცია", "გასვლა");
}

UI_ShowRegister(playerid)
{
	UI_ShowRegisterEx(playerid, "");
}

UI_ShowLogin(playerid, bool:wrongPassword)
{
	if (CefUI_Active(playerid))
	{
		new left = 3 - gPlayer[playerid][pLoginAttempts];
		CefUI_AuthStep(playerid, "login", wrongPassword ? "პაროლი არასწორია." : "", left < 0 ? 0 : left);
		return 0;
	}
	new body[1024];
	format(body, 1024, "{FFFFFF}კეთილი იყოს შენი დაბრუნება {33AA33}Los Santos Street Crime's{FFFFFF}-ზე!\n\n%sშეიყვანე პაროლი:", wrongPassword ? "{FF4444}პაროლი არასწორია.\n{FFFFFF}" : "");
	Lang_ShowPlayerDialog(playerid, 9002, 3, "შესვლა", body, "შესვლა", "გასვლა");
}

UI_ShowGenderSelect(playerid)
{
	if (CefUI_Active(playerid))
	{
		CefUI_AuthStep(playerid, "gender", "", 0);
		return 0;
	}
	Lang_ShowPlayerDialog(playerid, 9003, 2, "პერსონაჟის შექმნა - სქესი", "მამაკაცი\nქალი", "შემდეგი", "");
}

UI_ShowSkinSelect(playerid, gender)
{
	if (playerid < 0 || playerid >= MAX_PLAYERS)
	{
		return 0;
	}
	new g = gender == 1 ? 1 : 0;
	g_UiSkinGender[playerid] = g;
	if (CefUI_Active(playerid))
	{
		for (new i = 0; i < 7; i++)
		{
			CefUI_AuthSkin(playerid, i, Valid_SkinAt(g, i));
		}
		CefUI_AuthStep(playerid, "skin", "", g);
		return 0;
	}
	new body[1024], line[96];
	for (new i = 0; i < 7; i++)
	{
		format(line, 96, "ვარიანტი %d (სკინი %d)\n", i + 1, Valid_SkinAt(g, i));
		strcat(body, line, 1024);
	}
	Lang_ShowPlayerDialog(playerid, 9004, 2, "პერსონაჟის შექმნა - გარეგნობა", body, "დადასტურება", "");
}

UI_CloseDialogs(playerid)
{
	HidePlayerDialog(playerid);
}

bool:UI_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	switch (dialogid)
	{
		case 9001:
		{
			if (!response)
			{
				UI_OnDialogCancel(playerid, dialogid);
				return 1;
			}
			new len = strlen(inputtext);
			if (len < 6 || len > 32)
			{
				new err[256];
				format(err, 256, "{FF4444}პაროლი უნდა შეიცავდეს %d-დან %d-მდე სიმბოლოს.\n{FFFFFF}", 6, 32);
				UI_ShowRegisterEx(playerid, err);
				return 1;
			}
			UI_OnRegisterSubmit(playerid, inputtext);
			return 1;
		}
		case 9002:
		{
			if (!response)
			{
				UI_OnDialogCancel(playerid, dialogid);
				return 1;
			}
			if (strlen(inputtext) < 1)
			{
				UI_ShowLogin(playerid, 0);
				return 1;
			}
			UI_OnLoginSubmit(playerid, inputtext);
			return 1;
		}
		case 9003:
		{
			if (!response || listitem < 0 || listitem > 1)
			{
				UI_ShowGenderSelect(playerid);
				return 1;
			}
			UI_OnGenderChosen(playerid, listitem);
			return 1;
		}
		case 9004:
		{
			if (!response || listitem < 0 || listitem >= 7)
			{
				UI_ShowSkinSelect(playerid, g_UiSkinGender[playerid]);
				return 1;
			}
			new skin = Valid_SkinAt(g_UiSkinGender[playerid], listitem);
			if (skin == -1)
			{
				UI_ShowSkinSelect(playerid, g_UiSkinGender[playerid]);
				return 1;
			}
			UI_OnSkinChosen(playerid, skin);
			return 1;
		}
	}
	return 0;
}

UI_CefRegisterSubmit(playerid, password[])
{
	new len = strlen(password);
	if (len < 6 || len > 32)
	{
		UI_ShowRegisterEx(playerid, "x");
		return 0;
	}
	UI_OnRegisterSubmit(playerid, password);
}

UI_CefLoginSubmit(playerid, password[])
{
	if (password[0] == 0)
	{
		UI_ShowLogin(playerid, 0);
		return 0;
	}
	UI_OnLoginSubmit(playerid, password);
}

UI_CefSkinPick(playerid, idx)
{
	if (idx < 0 || idx >= 7)
	{
		UI_ShowSkinSelect(playerid, g_UiSkinGender[playerid]);
		return 0;
	}
	new skin = Valid_SkinAt(g_UiSkinGender[playerid], idx);
	if (skin == -1)
	{
		UI_ShowSkinSelect(playerid, g_UiSkinGender[playerid]);
		return 0;
	}
	UI_OnSkinChosen(playerid, skin);
}
