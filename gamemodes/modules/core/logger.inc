
Log_Write(const category[], const text[])
{
	new h = 0, mi = 0, s = 0, y = 0, mo = 0, d = 0;
	new clean[320], line[320];
	gettime(h, mi, s);
	getdate(y, mo, d);
	strmid(clean, text, 0, 319, 320);
	for (new i = 0; clean[i] != 0; i++)
	{
		if (clean[i] == 13 || clean[i] == 10)
		{
			clean[i] = 32;
		}
	}
	format(line, 320, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s", y, mo, d, h, mi, s, category, clean);
	printf("%s", line);
	new File:f = fopen("lsrp.log", filemode:3);
	if (f)
	{
		fwrite(f, line);
		fwrite(f, "\r\n");
		fclose(f);
	}
}
