new bool:g_CosWaitBoard[MAX_PLAYERS];
new g_CosRumor[3][8][] = {
	{"Idlewood-ის საცხობი ზაფხულის შემდეგ ისევ გაიხსნა: ცხელი პური ექვს საათზე გამოდის.", "უბნის სახლში ხუთშაბათის სკოლისშემდგომი მეცადინეობისთვის მოხალისეებს ეძებენ.", "სადგურის უკან მოედანს ისევ აქვს ბადეები: ვიღაცამ ხელით შეკერა.", "ერთი ძველი ხელოსანი ბავშვებს ველოსიპედებს უფასოდ უკეთებს — საკმარისია ყავა მიუტანო.", "ყვითელი კორპუსის ქალბატონები შაბათ საღამოს უბნის ვახშამს აწყობენ.", "კუთხის მაღაზიამ დაფა ჩამოკიდა: ვისაც დახმარება სჭირდება, ბარათს ტოვებს.", "ამბობენ, გაჩერებასთან მურალს იგივე მხატვრები აღადგენენ.", "ღამის ცვლიდან დაბრუნებულს უბნის სახლში შუქი ყოველთვის ანთია."},
	{"Ganton-ში თაღების ქვეშ დალაქი ბიჭებს გამოცდების წინ თმას კრეჭს.", "Ganton-ის მაღაზია ხილის ტვირთს ელოდება: ვინც მიიტანს, სასურველი სტუმარია.", "Grove-ზე სკამები თავიდან შეღებეს, თითოეული სხვადასხვა ფერად.", "დედების ჯგუფი ბავშვებს რიგრიგობით, ფეხით აცილებს სკოლაში.", "პარასკევის კალათბურთის ტურნირი წელს მოედნისთვის თანხას აგროვებს.", "ყოფილმა კურიერმა პატარა სახელოსნო გახსნა: ვისაც სწავლა სურს, სამსახურში იღებს.", "მთავარ ქუჩაზე შუქი დაბრუნდა: ვაჭრებმა ფული შეაგროვეს.", "ამბობენ, ძველი ღია კინოთეატრი ზაფხულის ერთი საღამოთი შეიძლება დაბრუნდეს."},
	{"Jefferson-ის საწყობს პუნქტუალური მძღოლები სჭირდება: კარგ მუშაკს არ ივიწყებენ.", "გორაზე მდებარე მოტელი საუზმეს სთავაზობს მათ, ვინც ტვირთის გადმოტვირთვაში ეხმარება.", "უბნის გუნდი ყოველ სამშაბათს ეკლესიის დარბაზში რეპეტიციას ატარებს.", "ავტომატური სამრეცხაო ამბების გაცვლის ადგილად იქცა.", "უბნის ახალგაზრდები პარკს ყოველი თვის პირველ კვირას ასუფთავებენ.", "Jefferson-ელი მცხობელი გაუყიდავ პურს საღამოობით სასადილოს ჩუქნის.", "ავტობუსის გაჩერებას ახალი ფარდული აქვს: ის მისმა მგზავრებმა ააშენეს.", "ამბობენ, ვინც Jefferson-ში ფრთხილად აწვდის ტვირთს, ყველგან ღია კარს ხვდება."}
};

bool:CityOS_Playing(playerid)
{
	return Util_IsPlaying(playerid) && gPlayer[playerid][pLivingReady] != 0 && LD_IsReady();
}

CityOS_Integrity(playerid)
{
	new veh = gPlayer[playerid][pFreightVehicle];
	if (veh <= 0 || veh >= 2000 || LD_VehModule(veh) != 3 || LD_VehOwner(veh) != playerid)
	{
		return -1;
	}
	new Float:h;
	GetVehicleHealth(veh, h);
	return LD_Clamp(floatround(h, floatround_method:1) / 10, 0, 100);
}

CityOS_NextDistance(playerid)
{
	new d = gPlayer[playerid][pContractPhase] == 2 ? gPlayer[playerid][pContractToDistrict] : gPlayer[playerid][pContractFromDistrict];
	new Float:tx, Float:ty, Float:px, Float:py, Float:pz;
	new bool:shop = gPlayer[playerid][pContractPhase] == 2;
	switch (d)
	{
		case 1:
		{
			tx = (shop ? 1927.0 : 1880.0);
			ty = Float:((shop ? 0xC4DFC000 : 0xC4DE8000));
		}
		case 2:
		{
			tx = Float:((shop ? 0x451BA000 : 0x45160000));
			ty = Float:((shop ? 0xC4D34000 : 0xC4D98000));
		}
		case 3:
		{
			tx = (shop ? 2190.0 : 2145.0);
			ty = Float:((shop ? -1155.0 : -1215.0));
		}
		default:
		{
			return -1;
		}
	}
	GetPlayerPos(playerid, px, py, pz);
	new Float:dx = (tx - px), Float:dy = (ty - py);
	return floatround(floatsqroot(((dx * dx) + (dy * dy))), floatround_method:1);
}

CityOS_ShowBoard(playerid)
{
	if (!CityOS_Playing(playerid))
	{
		return 0;
	}
	new now = gettime(), n = Director_BoardCount();
	UI_CityBoardBegin(playerid);
	new id = 0, fromD = 0, toD = 0, cargo = 0, reward = 0, meters = 0, expires = 0, minLevel = 0, minRep = 0, minLocal = 0, premium = 0, rows = 0;
	for (new i = 0; i < n && rows < 12; i++)
	{
		if (!Director_BoardVisible(playerid, i))
		{
			continue;
		}
		if (!Director_BoardGet(i, id, fromD, toD, cargo, reward, meters, expires, minLevel, minRep, minLocal, premium))
		{
			continue;
		}
		UI_CityBoardAdd(playerid, id, 1, fromD, toD, cargo, reward, meters, expires - now, premium != 0, minLevel, minRep, minLocal, Director_BlockCode(playerid, i));
		rows++;
	}
	UI_CityBoardShow(playerid, Hub_NearBoard(playerid) >= 0);
	return 0;
}

CityOS_ShowDistrict(playerid, districtId)
{
	if (!CityOS_Playing(playerid))
	{
		return 0;
	}
	if (districtId < 1 || districtId > 3)
	{
		UI_CityDistrictPick(playerid);
		return 0;
	}
	DB_LivingHistory(playerid, districtId);
	return 0;
}

CityOS_RenderDistrict(playerid, districtId, count)
{
	new now = gettime(), fest = DB_LivingDistrictGet(districtId, E_LD_DISTRICT:4), festH = 0;
	if (fest > now)
	{
		festH = (fest - now + 3600 - 1) / 3600;
	}
	UI_CityDistrictBegin(playerid, districtId, District_GetActivity(districtId), District_GetSecurity(districtId), District_GetCrime(districtId), LD_GetLocalRep(playerid, districtId), festH);
	new r = LD_HashMod(LD_Hash32(LD_Bucket(now), districtId, 3), 8);
	UI_CityDistrictRumor(playerid, g_CosRumor[districtId - 1][r]);
	for (new i = 0; i < count && i < 8; i++)
	{
		new ago = now - g_LdHist[i][lhTimestamp];
		UI_CityDistrictEvent(playerid, g_LdHist[i][lhType], ago > 0 ? ago : 0, g_LdHist[i][lhActDelta]);
	}
	UI_CityDistrictShow(playerid);
}

CityOS_OnInit()
{
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		g_CosWaitBoard[i] = false;
	}
	LD_Listen("CityOS_OnDistrictEvent", 30);
}

CityOS_OnExit()
{
}

CityOS_OnPlayerConnect(playerid)
{
	g_CosWaitBoard[playerid] = false;
}

CityOS_OnCharReady(playerid)
{
	g_CosWaitBoard[playerid] = false;
}

CityOS_OnPlayerDisconnect(playerid, reason)
{
	#pragma unused reason
	g_CosWaitBoard[playerid] = false;
}

public CityOS_OnDistrictEvent(type, districtId, charId, value, const opId[])
{
	if (type != 104)
	{
		return 1;
	}
	for (new i = 0; i < MAX_PLAYERS; i++)
	{
		if (!g_CosWaitBoard[i])
		{
			continue;
		}
		g_CosWaitBoard[i] = false;
		if (IsPlayerConnected(i))
		{
			CityOS_ShowBoard(i);
		}
	}
	return 1;
}

CityOS_CmdContratti(playerid, const params[])
{
	if (!CityOS_Playing(playerid))
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	new pos = 0, tok[72], num[72];
	Util_NextToken(params, pos, tok, 72);
	if (!strcmp(tok, "accetta", true, 0x7FFFFFFF) && tok[0] != 0)
	{
		new id = 0;
		Util_NextToken(params, pos, num, 72);
		if (!Util_ParseInt(num, id) || id <= 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /contratti accetta <კონტრაქტის ნომერი>");
			return 1;
		}
		Director_Accept(playerid, id);
		return 1;
	}
	CityOS_ShowBoard(playerid);
	return 1;
}

CityOS_CmdContratto(playerid, const params[])
{
	if (!CityOS_Playing(playerid))
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	new pos = 0, tok[72];
	Util_NextToken(params, pos, tok, 72);
	new ct = gPlayer[playerid][pContractId];
	new bool:active = ct != 0 && (gPlayer[playerid][pContractPhase] == 1 || gPlayer[playerid][pContractPhase] == 2);
	if (tok[0] != 0 && !strcmp(tok, "abbandona", true, 0x7FFFFFFF))
	{
		if (!active)
		{
			Util_Send(playerid, -10270721, "დასატოვებელი კონტრაქტი არ გაქვს.");
			return 1;
		}
		Director_Release(playerid, 1);
		return 1;
	}
	if (!active)
	{
		if (ct != 0)
		{
			Util_Send(playerid, -1, "შენი კონტრაქტის მოთხოვნა მუშავდება: წამით მოიცადე.");
		}
		else
		{
			UI_CityContractNone(playerid);
		}
		return 1;
	}
	new exp = Director_ExpiresAt(playerid), left = -1;
	if (exp > 0)
	{
		left = exp > gettime() ? exp - gettime() : 0;
	}
	UI_CityContractStatus(playerid, ct, gPlayer[playerid][pContractPhase], gPlayer[playerid][pContractFromDistrict], gPlayer[playerid][pContractToDistrict], gPlayer[playerid][pContractCargo], gPlayer[playerid][pContractReward], left, CityOS_Integrity(playerid), CityOS_NextDistance(playerid), -5);
	return 1;
}

CityOS_CmdReputazione(playerid, const params[])
{
	#pragma unused params
	if (!CityOS_Playing(playerid))
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	UI_CityRepBegin(playerid);
	UI_CityRepTrack(playerid, 1, LD_GetProPoints(playerid, 1), 0, 0, 0, 0, 0);
	new rating = DB_LivingPlayerGet(playerid, E_LD_PLAYER:3);
	UI_CityRepTrack(playerid, 2, LD_GetProPoints(playerid, 2), 0, 0, rating, rating > 0 ? 100 : 0, 0);
	for (new d = 1; d <= 3; d++)
	{
		UI_CityRepLocal(playerid, d, LD_GetLocalRep(playerid, d));
	}
	UI_CityRepShow(playerid);
	return 1;
}

CityOS_CmdQuartiere(playerid, const params[])
{
	if (!CityOS_Playing(playerid))
	{
		LD_SendBlock(playerid, 1);
		return 1;
	}
	new pos = 0, tok[72], d = 0;
	Util_NextToken(params, pos, tok, 72);
	if (tok[0] != 0)
	{
		d = LD_ParseDistrict(tok);
		if (d == 0)
		{
			Util_Send(playerid, 0x99CCFFFF, "გამოყენება: /quartiere [idlewood|ganton|jefferson]");
			return 1;
		}
	}
	else
	{
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);
		d = GetPlayerInterior(playerid) == 0 ? District_At(x, y) : 0;
	}
	CityOS_ShowDistrict(playerid, d);
	return 1;
}

public Living_OnDBHistory(playerid, result, districtId, count)
{
	if (!CityOS_Playing(playerid) || districtId < 1 || districtId > 3)
	{
		return 1;
	}
	if (result == 10)
	{
		Util_Send(playerid, -10270721, "შენს მონაცემებს ჯერ კიდევ ვტვირთავ: წამში სცადე.");
		return 1;
	}
	CityOS_RenderDistrict(playerid, districtId, result == 1 ? count : 0);
	return 1;
}

public UI_OnContractAccept(playerid, contractId)
{
	if (!CityOS_Playing(playerid))
	{
		return 1;
	}
	Director_Accept(playerid, contractId);
	return 1;
}

public UI_OnContractAbandon(playerid, contractId)
{
	if (!CityOS_Playing(playerid))
	{
		return 1;
	}
	if (contractId <= 0 || gPlayer[playerid][pContractId] != contractId)
	{
		Util_Send(playerid, -10270721, "ეს კონტრაქტი აღარ არის აქტიური.");
		return 1;
	}
	Director_Release(playerid, 1);
	return 1;
}

public UI_OnContractBoardRefresh(playerid)
{
	if (!CityOS_Playing(playerid))
	{
		return 1;
	}
	if (!DB_IsReady())
	{
		CityOS_ShowBoard(playerid);
		return 1;
	}
	g_CosWaitBoard[playerid] = true;
	DB_LivingBoard(65535);
	return 1;
}

public UI_OnDistrictPick(playerid, districtId)
{
	if (!CityOS_Playing(playerid))
	{
		return 1;
	}
	CityOS_ShowDistrict(playerid, districtId);
	return 1;
}
