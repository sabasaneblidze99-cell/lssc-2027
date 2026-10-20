// Harness CEF: guida l'NPC Test_Bot nel percorso CEF (login -> personaggi -> spawn -> HUD) chiamando le public
// che la pagina samp-cef chiamerebbe. Da usare con il gamemode compilato contro fakecef/cef.inc.
#define FILTERSCRIPT
#include <open.mp>
#define BOT_NAME "Test_Bot"
new gBot = INVALID_PLAYER_ID, gT, gTimer;
forward Bot_Create();
forward Bot_Tick();
public OnFilterScriptInit() { print("[CBOT] harness CEF"); SetTimer("Bot_Create", 4000, false); return 1; }
public Bot_Create() { gBot = NPC_Create(BOT_NAME); gTimer = SetTimer("Bot_Tick", 1000, true); return 1; }
C(const fn[], const arg[]) { new r = CallRemoteFunction(fn, "ds", gBot, arg); printf("[CBOT] t=%d %s(%s) -> %d", gT, fn, arg, r); }
public Bot_Tick()
{
	gT++;
	if (!IsPlayerConnected(gBot)) { printf("[CBOT] bot non connesso t=%d", gT); KillTimer(gTimer); return 1; }
	new dlg = GetPlayerDialogID(gBot);
	if (dlg != -1 && dlg != 65535) printf("[CBOT] t=%d dialog aperto %d", gT, dlg);
	switch (gT)
	{
		case 2: { new r = CallRemoteFunction("OnPlayerRequestClass", "ii", gBot, 0); printf("[CBOT] RequestClass -> %d", r); }
		case 3: { new r = CallRemoteFunction("OnCefInitialize", "ii", gBot, 1); printf("[CBOT] OnCefInitialize -> %d", r); }
		case 4: C("Cef_OnReady", "");
		case 6: C("Cef_OnLogin", "426f74546573745f32303236");
		case 9: { C("Cef_OnViewOk", "chars"); C("Cef_OnCharPick", "0"); }
		case 12: { C("Cef_OnViewOk", "spawn"); C("Cef_OnSpawnPick", "0"); }
		case 15: { if (!NPC_IsSpawned(gBot)) NPC_Spawn(gBot); }
		case 20: { C("Cef_OnQuick", "1"); C("Cef_OnChatSend", "e18392e18390e1839be18390e183a0e183afe1839de18391e1839020776f726c64"); }
		case 24: { print("[CBOT] fine"); NPC_Destroy(gBot); KillTimer(gTimer); }
	}
	return 1;
}
