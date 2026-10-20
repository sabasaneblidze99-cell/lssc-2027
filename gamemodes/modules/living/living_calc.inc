
LD_Clamp(v, lo, hi)
{
	if (v < lo)
	{
		return lo;
	}
	if (v > hi)
	{
		return hi;
	}
	return v;
}

LD_Hash32(a, b, c)
{
	new h = 0x811C9DC5, v[3];
	v[0] = a;
	v[1] = b;
	v[2] = c;
	for (new i = 0; i < 3; i++)
	{
		for (new s = 0; s < 32; s += 8)
		{
			h ^= v[i] >>> s & 255;
			h *= 0x01000193;
		}
	}
	return h;
}

LD_HashMod(h, n)
{
	if (n <= 0)
	{
		return 0;
	}
	return (h & 0x7FFFFFFF) % n;
}

LD_Bucket(unixTs)
{
	if (unixTs <= 0)
	{
		return 0;
	}
	return unixTs / 1800;
}

bool:LD_ContractSlot(bucket, slot, &fromD, &toD, &cargo)
{
	fromD = 0;
	toD = 0;
	cargo = 0;
	new h = LD_Hash32(bucket, slot, 1);
	fromD = LD_HashMod(h, 3) + 1;
	new off = LD_HashMod(h >>> 8, 2) + 1;
	toD = (fromD - 1 + off) % 3 + 1;
	cargo = LD_HashMod(h >>> 16, 3) + 1;
	return 1;
}

LD_ProTier(points)
{
	if (points >= 600)
	{
		return 3;
	}
	if (points >= 300)
	{
		return 2;
	}
	if (points >= 100)
	{
		return 1;
	}
	return 0;
}

LD_ProBonusPct(points)
{
	switch (LD_ProTier(points))
	{
		case 1:
		{
			return 5;
		}
		case 2:
		{
			return 10;
		}
		case 3:
		{
			return 15;
		}
	}
	return 0;
}

LD_LocalRepLevel(rep)
{
	if (rep >= 40)
	{
		return 2;
	}
	if (rep >= 10)
	{
		return 1;
	}
	return 0;
}

LD_DiscountPct(rep)
{
	switch (LD_LocalRepLevel(rep))
	{
		case 1:
		{
			return 5;
		}
		case 2:
		{
			return 10;
		}
	}
	return 0;
}

LD_ApplyDiscount(amount, rep)
{
	if (amount <= 0)
	{
		return 0;
	}
	new a = LD_Clamp(amount, 0, 10000000);
	return a - a * LD_DiscountPct(rep) / 100;
}

LD_FreightMinSeconds(distM)
{
	new d = LD_Clamp(distM, 0, 100000);
	new s = (d + 40 - 1) / 40;
	return s > 25 ? s : 25;
}

LD_FreightTimeLimit(distM)
{
	new d = LD_Clamp(distM, 0, 100000);
	return d * 10 / 60 + 120;
}

LD_TaxiFare(meters)
{
	new m = LD_Clamp(meters, 0, 100000);
	return m * 12 / 100 + 15;
}

LD_TaxiFareFor(meters, rep)
{
	return LD_ApplyDiscount(LD_TaxiFare(meters), rep);
}

LD_TaxiNpcPay(fare, proPoints)
{
	if (fare <= 0)
	{
		return 0;
	}
	return LD_Clamp(fare, 0, 10000000) * (LD_ProBonusPct(proPoints) + 100) / 100;
}

LD_RentalModel(vkind)
{
	switch (vkind)
	{
		case 1:
		{
			return 462;
		}
		case 2:
		{
			return 436;
		}
		case 3:
		{
			return 410;
		}
		case 4:
		{
			return 420;
		}
	}
	return 0;
}

LD_RentalDeposit(vkind)
{
	switch (vkind)
	{
		case 1:
		{
			return 200;
		}
		case 2:
		{
			return 400;
		}
		case 3:
		{
			return 450;
		}
		case 4:
		{
			return 300;
		}
	}
	return 0;
}

LD_RentalFee(vkind, blocks)
{
	if (blocks < 1 || blocks > 3)
	{
		return 0;
	}
	switch (vkind)
	{
		case 1:
		{
			return blocks * 30;
		}
		case 2:
		{
			return blocks * 60;
		}
		case 3:
		{
			return blocks * 70;
		}
	}
	return 0;
}

LD_RentalFeeFor(vkind, blocks, rep)
{
	return LD_ApplyDiscount(LD_RentalFee(vkind, blocks), rep);
}

LD_RentalRefund(deposit, health)
{
	if (deposit <= 0)
	{
		return 0;
	}
	new dep = LD_Clamp(deposit, 0, 10000);
	if (health >= 1000)
	{
		return dep;
	}
	if (health <= 300)
	{
		return 0;
	}
	new span = 700;
	new r = (dep * 2 * (health - 300) + span) / (span * 2);
	return LD_Clamp(r, 0, dep);
}

bool:LD_DonationValid(amount, donatedToday)
{
	if (amount < 50 || amount > 500)
	{
		return 0;
	}
	if (donatedToday < 0)
	{
		return 0;
	}
	return donatedToday + amount <= 1000;
}

LD_FloatKey(Float:f)
{
	new i = f;
	return i >= 0 ? i : cellmin - i;
}

bool:LD_InBox(kx, ky, Float:x1, Float:y1, Float:x2, Float:y2)
{
	return kx >= LD_FloatKey(x1) && kx <= LD_FloatKey(x2) && ky >= LD_FloatKey(y1) && ky <= LD_FloatKey(y2);
}

District_At(Float:x, Float:y)
{
	new kx = LD_FloatKey(x), ky = LD_FloatKey(y);
	if (LD_InBox(kx, ky, 1812.0, -1880.0, 1971.0, -1742.0))
	{
		return 1;
	}
	if (LD_InBox(kx, ky, 2222.0, -1852.0, 2632.0, -1628.0))
	{
		return 2;
	}
	if (LD_InBox(kx, ky, 2056.0, -1287.0, 2281.0, -1115.0))
	{
		return 3;
	}
	return 0;
}

bool:LD_StrEqI(const a[], const b[])
{
	new i = 0;
	while (a[i] != 0 && b[i] != 0)
	{
		if (tolower(a[i]) != tolower(b[i]))
		{
			return 0;
		}
		i++;
	}
	return a[i] == 0 && b[i] == 0;
}

LD_ParseDistrict(const name[])
{
	if (name[0] >= 49 && name[0] < 52 && name[1] == 0)
	{
		return name[0] - 48;
	}
	if (LD_StrEqI(name, "idlewood"))
	{
		return 1;
	}
	if (LD_StrEqI(name, "ganton"))
	{
		return 2;
	}
	if (LD_StrEqI(name, "jefferson"))
	{
		return 3;
	}
	return 0;
}

LD_CopyStr(dest[], size, src[])
{
	if (size <= 0)
	{
		return 0;
	}
	new i = 0;
	while (src[i] != 0 && i < size - 1)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
	return i;
}

LD_DistrictName(id, dest[], size)
{
	switch (id)
	{
		case 1:
		{
			return LD_CopyStr(dest, size, "Idlewood");
		}
		case 2:
		{
			return LD_CopyStr(dest, size, "Ganton");
		}
		case 3:
		{
			return LD_CopyStr(dest, size, "Jefferson");
		}
	}
	return LD_CopyStr(dest, size, "-");
}
