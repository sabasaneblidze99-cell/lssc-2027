enum E_PLAYER_DATA
{
	pLoggedIn,
	pSpawned,
	pAccountId,
	pCharId,
	pPassHash[65],
	pPassSalt[17],
	pAdminLevel,
	pLoginAttempts,
	pMoney,
	pBank,
	pLevel,
	pExp,
	Float:pHealth,
	Float:pArmor,
	Float:pPosX,
	Float:pPosY,
	Float:pPosZ,
	Float:pAngle,
	pInterior,
	pWorld,
	pSkin,
	pJob,
	pFaction,
	pFactionRank,
	pCivilSkin,
	pGender,
	pAuthState,
	pClassRequested,
	pHudCreated,
	pSpawnCount,
	pKickTimer,
	pPaycheckMs,
	pSessionMs,
	pPlayedMs,
	pLastActiveTick,
	pLastReportTick,
	pHashAlgo,
	pAuthSeq,
	pMutedUntil,
	pFrozen,
	pLastGiveMoneyTick,
	pOnDuty,
	pCuffed,
	pWounded,
	pInvState,
	pInvHasBag,
	pInvItem[20],
	pInvQty[20],
	pInvCount,
	pInvOpBusy,
	pInvReloadQueued,
	pInvOp,
	pInvOpItem,
	pInvOpQty,
	pInvOpAux,
	pInvOpPeer,
	pInvOpId[33],
	pInvUndoOpId[33],
	pInvItemCdUntil[11],
	pInvLastActionTick,
	pInvGiveTo,
	pInvGiveFrom,
	pInvGiveItem,
	pInvGiveQty,
	pInvGiveSeq,
	pInvGiveStartTick,
	pInvLastGiveTick,
	pInvGiveHourStart,
	pInvGiveHourCount,
	pDocRequestTick,
	pFridgeHouse,
	pFridgeDir,
	pFridgeFoodBefore,
	pBankOpBusy,
	pBankOp,
	pBankOpAmount,
	pBankOpAux,
	pBankOpTarget,
	pBankOpPoint,
	pBankOpTick,
	pBankOpToken,
	pBankOpId[37],
	pBankPendTarget,
	pBankPendAmount,
	pBankPendCharId,
	pBankPendTick,
	pBankLastXferTick,
	pBankLastOpTick,
	pBankConfirmFails,
	pBankXferBlockUntil,
	pBankLimitsLoaded,
	pBankWithdrawnToday,
	pBankXferOutToday,
	pBankAtCounter,
	pStoryOrigin,
	pStoryMotive,
	pIntroSeen,
	pIntroTimer,
	pIntroScene,
	pIntroVehicle,
	pIntroActor,
	pIntroSeq,
	pIntroStartTick,
	pIntroFlags,
	pStoryDraftOrigin,
	pStoryDraftMotive,
	pStorySetMode,
	pStorySaveRetries,
	pStoryShowTarget,
	pStoryLastShowTick,
	pLivingReady,
	pDistrictRep[4],
	pProPoints[3],
	_PLAYER_DATA_pad346[1],
	pContractId,
	pContractPhase,
	pFreightVehicle,
	pContractFromDistrict,
	pContractToDistrict,
	pContractCargo,
	pContractDistance,
	pContractReward,
	pContractStartTick,
	pFreightLegTick,
	pFreightDwellTick,
	pFreightLeftVehTick,
	pContractCooldownUntil,
	pFreightWelcome,
	pContractPendOp,
	pContractOpId[41],
	pRentalId,
	pRentalVehicle,
	pRentalClass,
	pRentalKind,
	pRentalDueAt,
	pRentalWarnFlags,
	pRentalPendOp,
	pRentalOpId[41],
	pTaxiOnDuty,
	pTaxiRideId,
	pTaxiRideKind,
	pTaxiRideState,
	pTaxiMeterM,
	pTaxiMeterOn,
	Float:pTaxiLastX,
	Float:pTaxiLastY,
	pTaxiPeer,
	pTaxiFromDistrict,
	pTaxiToDistrict,
	pTaxiCallTick,
	pTaxiNpcOfferAt,
	pTaxiOpSeq,
	pTaxiPendOp,
	pTaxiOpId[41],
	pOnboardStep,
	pHintsOff,
	pLastHintTick,
	pDonationPending,
	bool:pChatSamp	// preferenza: chat SA-MP invece della chat CEF (HUD Glass UI)
}

enum E_VEH_ROW
{
	vrId,
	vrModel,
	vrPlate[8],
	vrColor1,
	vrColor2,
	vrPricePaid,
	Float:vrPosX,
	Float:vrPosY,
	Float:vrPosZ,
	Float:vrPosA,
	Float:vrParkX,
	Float:vrParkY,
	Float:vrParkZ,
	Float:vrParkA,
	vrInterior,
	vrWorld,
	Float:vrHealth,
	vrFuelCl,
	vrLocked,
	vrDestroyed
}

enum E_VEH_NEW
{
	vnOwnerId,
	vnModel,
	vnColor1,
	vnColor2,
	vnPricePaid,
	vnFuelCl,
	Float:vnPosX,
	Float:vnPosY,
	Float:vnPosZ,
	Float:vnPosA
}

enum E_HOUSE_ROW
{
	hrId,
	hrIntGame,          // interno GTA personalizzato (case Arizona), -1 = tabella LSSC
	Float:hrIntX,
	Float:hrIntY,
	Float:hrIntZ,
	hrOwnerId,
	hrOwnerName[25],
	hrTier,
	hrInteriorIdx,
	Float:hrExtX,
	Float:hrExtY,
	Float:hrExtZ,
	Float:hrExtA,
	hrPrice,
	hrPricePaid,
	hrLocked,
	hrLuceOn,
	hrGasOn,
	hrLuceCut,
	hrGasCut,
	hrLuceMin,
	hrGasMin,
	hrCycleMin,
	hrFridgeUnits,
	hrFridgeOffMin,
	hrKeyCount,
	hrBills,
	hrKeyChars[3]
}

enum E_HOUSE_BILL
{
	hbId,
	hbKind,
	hbAmount,
	hbCyclesAge
}

enum E_HOUSE_KEY
{
	hkCharId,
	hkName[25]
}

enum E_HOUSE_REQ
{
	hqHouseId,
	hqTargetId,
	hqKind
}

enum E_FAC_MEMBER
{
	fmCharId,
	fmName[25],
	fmRank,
	fmLevel,
	fmLastLogin
}

enum E_FINE_ROW
{
	frId,
	frFaction,
	frAmount,
	frTimestamp,
	frReason[65],
	frIssuer[25]
}

enum E_JF_REQ
{
	jqPayoutJobId,
	jqPayoutAmount,
	jqSetJobId,
	jqFacTargetChar,
	jqFacCmd,
	jqFacFaction,
	jqFacNewRank,
	jqFindTag,
	jqMembersFaction
}

enum E_FINE_INFL
{
	fiActive,
	fiToken,
	fiIssuerPlayerId,
	fiIssuerName[25],
	fiIssuerChar,
	fiTargetChar,
	fiAmount
}

enum E_INV_ROW
{
	irSlot,
	irItem,
	irQty
}

enum E_INV_REQ
{
	iqToken,
	iqOp,
	iqItemId,
	iqQty,
	iqDir,
	iqHouseId,
	iqRecvPlayerId,
	iqRecvChar,
	iqRecvName[25],
	iqOpId[33]
}

enum E_BANK_STMT
{
	bsTimestamp,
	bsKind[13],
	bsDelta,
	bsBalance,
	bsOther[25],
	bsRef[33]
}

enum E_BANK_REQ
{
	bqToken,
	bqOp,
	bqAmount,
	bqGen,
	bqXferPending,
	bqOpId[37]
}

enum E_BANK_INFO
{
	biToken,
	biInfoOp,
	biPage
}

enum E_STORY_REQ
{
	sqToken,
	sqGen,
	sqCreation,
	sqOpId[37]
}

enum E_STORY_INFO
{
	siToken,
	siKind,
	siAccountId,
	siCharId,
	siTargetId,
	siTargetChar,
	siTargetGen,
	siPurpose
}

enum E_LD_BOARD
{
	lbId,
	lbFromDistrict,
	lbToDistrict,
	lbFromPoint,
	lbToPoint,
	lbCargo,
	lbDistanceM,
	lbReward,
	lbExpiresAt,
	lbMinLevel,
	lbMinRep,
	lbMinLocalRep,
	lbPremium,
	lbState
}

enum E_LD_HIST
{
	lhType,
	lhTimestamp,
	lhValue,
	lhActDelta,
	lhMine
}

enum E_LD_DISTRICT
{
	ldLoaded,
	ldActivity,
	ldSecurity,
	ldCrime,
	ldFestUntil,
	ldLastTickAt,
	ldProjectId,
	ldProjectGoal,
	ldProjectFund,
	ldProjectEndsAt,
	ldProjectName[65]
}

enum E_LD_PLAYER
{
	lpLoaded,
	lpWorkEarned,
	lpPayoutsLastHour,
	lpTaxiRating,
	lpOnboardStep,
	lpOnboardDone,
	lpHintsOff,
	lpSecsSincePayout
}

enum E_LD_REQ
{
	lqToken,
	lqGen,
	lqAction,
	lqCharId,
	lqRef,
	lqAux,
	lqOpId[37]
}

enum E_ADM_PEND
{
	apActive,
	apOp,
	apTargetAcc,
	apStaffAcc,
	apValue,
	apTargetName[25],
	apStaffName[25],
	apReason[65]
}

enum E_VSLOT
{
	vsUsed,
	vsDbId,
	vsModel,
	vsPlate[8],
	vsColor1,
	vsColor2,
	vsPricePaid,
	Float:vsPosX,
	Float:vsPosY,
	Float:vsPosZ,
	Float:vsPosA,
	Float:vsParkX,
	Float:vsParkY,
	Float:vsParkZ,
	Float:vsParkA,
	Float:vsHealth,
	vsFuelCl,
	vsLocked,
	vsDestroyed,
	vsEngineOn,
	vsLowFuelWarned,
	vsSelling,
	vsVehicleId
}

enum E_VPLAYER
{
	vpSession,
	vpLoadState,
	vpBuyPending,
	vpBuyCatalogIdx,
	vpBuyWithBank,
	vpBuyPrice,
	vpSellPending,
	vpSellRollback,
	vpSellSlot,
	vpSellDbId,
	vpSellRefund,
	vpCmdTick,
	vpFindTimer,
	vpLastVehicle
}

enum E_INV_GROUND
{
	igState,
	igItemId,
	igQty,
	igObject,
	Float:igPosX,
	Float:igPosY,
	Float:igPosZ,
	igExpireTick,
	igDropper,
	igDropperChar,
	igPicker
}

enum E_HPLAYER
{
	hpSession,
	hpInsideHouse,
	hpPendingOp,
	hpOpHouseId,
	hpPayWithBank,
	hpOpAmount,
	hpBillIntent,
	hpKeyTargetChar,
	hpBillKindMask,
	hpBillKind,
	hpBillPaidTotal,
	hpBillsHouseId,
	hpBillCount,
	hpHungerState,
	hpHunger,
	_HPLAYER_pad15[1],
	hpHungerAccumMs,
	hpCapHealthOnLoad,
	hpCooking,
	hpCookHouseId,
	hpCookCooldown,
	hpCmdTick,
	hpKnockTick,
	hpInfoMsgTick,
	hpKeyToken[32]
}

enum E_DIR_BOARD
{
	dbContractId,
	dbFromDistrict,
	dbToDistrict,
	dbCargo,
	dbDistanceM,
	dbReward,
	dbExpiresAt,
	dbMinLevel,
	dbMinRep,
	dbMinLocalRep,
	dbPremium
}
