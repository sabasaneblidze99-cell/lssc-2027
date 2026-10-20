#if !defined CEFSAMP_INC_
	#define CEFSAMP_INC_

	enum CEF_ValueType
	{
		CEF_STRING,
		CEF_INTEGER,
		CEF_FLOAT
	};

	#define CEFSTR(%0) CEF_STRING, %0
	#define CEFINT(%0) CEF_INTEGER, %0
	#define CEFFLOAT(%0) CEF_FLOAT, %0

	#define CEF_DEFAULT_MAX_DIST 50.0
	#define CEF_DEFAULT_REF_DIST 15.0

	// ===== FINTO samp-cef per i test: niente rete, controlla che ogni stringa sia UTF-8 valido =====
	stock cef_create_browser(player_id, browser_id, const url[], bool:hidden, bool:focused) { printf("[FAKECEF] create_browser %d %d %s", player_id, browser_id, url); return 1; }
	stock cef_destroy_browser(player_id, browser_id) { return 1; }
	stock cef_on_player_connect(player_id, const ip[]) { return 1; }
	stock cef_on_player_disconnect(player_id) { return 1; }
	stock FakeCef_Utf8Ok(const s[])
	{
		new i = 0;
		while (s[i] != 0)
		{
			new c = s[i] & 0xFF, n = 0;
			if (c < 0x80) n = 0;
			else if ((c & 0xE0) == 0xC0) n = 1;
			else if ((c & 0xF0) == 0xE0) n = 2;
			else if ((c & 0xF8) == 0xF0) n = 3;
			else return 0;
			for (new k = 1; k <= n; k++)
			{
				if ((s[i + k] & 0xC0) != 0x80) return 0;
			}
			i += n + 1;
		}
		return 1;
	}
	stock cef_emit_event(player_id, const event[], {CEF_ValueType, Float, _}:...)
	{
		new n = numargs(), line[512], tmp[300];
		format(line, sizeof line, "[FAKECEF] emit %d %s", player_id, event);
		if ((n - 2) % 2 != 0) printf("[FAKECEF] ERRORE coppie argomenti %s", event);
		for (new i = 2; i + 1 < n; i += 2)
		{
			new t = getarg(i);
			if (t == 0)
			{
				new j = 0;
				while (j < sizeof tmp - 1 && getarg(i + 1, j) != 0) { tmp[j] = getarg(i + 1, j); j++; }
				tmp[j] = 0;
				if (!FakeCef_Utf8Ok(tmp)) printf("[FAKECEF] *** CRASH REALE: stringa NON UTF-8 in %s arg %d: %s", event, i, tmp);
				format(line, sizeof line, "%s |s:%s", line, tmp);
			}
			else if (t == 1) format(line, sizeof line, "%s |i:%d", line, getarg(i + 1));
			else format(line, sizeof line, "%s |f:%f", line, Float:getarg(i + 1));
		}
		print(line);
		return 1;
	}
	stock cef_subscribe(const event[], const callback[]) { return 1; }
	stock cef_hide_browser(player_id, browser_id, bool:hide) { return 1; }
	stock cef_create_ext_browser(player_id, browser_id, const texture[], const url[], scale) { return 1; }
	stock cef_append_to_object(player_id, browser_id, object_id) { return 1; }
	stock cef_remove_from_object(player_id, browser_id, object_id) { return 1; }
	stock cef_player_has_plugin(player_id) { return 1; }
	stock cef_toggle_dev_tools(player_id, browser_id, bool:enabled) { return 1; }
	stock cef_set_audio_settings(player_id, browser_id, Float:max_distance = CEF_DEFAULT_MAX_DIST, Float:reference_distance = CEF_DEFAULT_REF_DIST) { return 1; }
	stock cef_focus_browser(player_id, browser_id, bool:focused) { printf("[FAKECEF] focus %d %d", player_id, focused); return 1; }
	stock cef_always_listen_keys(player_id, browser_id, bool:listen) { return 1; }
	stock cef_load_url(player_id, browser_id, const url[]) { return 1; }

	forward OnCefInitialize(player_id, success);
	forward OnCefBrowserCreated(player_id, browser_id, status_code);

	public OnPlayerConnect(playerid)
	{
		new ip[16];
		GetPlayerIp(playerid, ip, sizeof(ip));
		cef_on_player_connect(playerid, ip);

		#if defined CEF_OnPlayerConnect
			CEF_OnPlayerConnect(playerid);
		#endif

		return 1;

	}

	#if defined _ALS_OnPlayerConnect
		#undef OnPlayerConnect
	#else
		#define _ALS_OnPlayerConnect
	#endif

	#define OnPlayerConnect CEF_OnPlayerConnect

	#if defined CEF_OnPlayerConnect
		forward CEF_OnPlayerConnect(playerid);
	#endif

	public OnPlayerDisconnect(playerid, reason)
	{
		cef_on_player_disconnect(playerid);

		#if defined CEF_OnPlayerDisconnect
			CEF_OnPlayerDisconnect(playerid, reason);
		#endif

		return 1;
	}

	#if defined _ALS_OnPlayerDisconnect
		#undef OnPlayerDisconnect
	#else
		#define _ALS_OnPlayerDisconnect
	#endif

	#define OnPlayerDisconnect CEF_OnPlayerDisconnect

	#if defined CEF_OnPlayerDisconnect
		forward CEF_OnPlayerDisconnect(playerid, reason);
	#endif
#endif // !defined CEFSAMP_INC_
