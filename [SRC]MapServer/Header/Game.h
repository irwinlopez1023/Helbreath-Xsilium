// Game.h: interface for the CGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAME_H__C3D29FC5_755B_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_GAME_H__C3D29FC5_755B_11D2_A8E6_00001C7030A6__INCLUDED_
#pragma warning(disable : 4996)
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define CMD(x)  memcmp(cp, x, strlen(x)) == 0

#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <direct.h>
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard


#include "Winmain.h"
#include "StrTok.h"
#include "Xsocket.h"
#include "Client.h"
#include "Npc.h"
#include "Map.h"
#include "Misc.h"
#include "Msg.h"
#include "Magic.h"
#include "Skill.h"
#include "DynamicObject.h"
#include "DelayEvent.h"
#include "Fish.h"
#include "DynamicObject.h"
#include "Potion.h"
#include "Mineral.h"
#include "Quest.h"
#include "BuildItem.h"
#include "TeleportLoc.h"
#include "teleport.h"
#include "PartyManager.h"

#include "../General/ActionID.h"
#include "../../Shared/netmsg.h"
#include "../General/DynamicObjectID.h"
#include "../General/GlobalDef.h"
#include "../General/MessageIndex.h"
#include "../../Shared/netmsg.h"
#include "../General/UserMessages.h"
#include "../General/Version.h"

#include "../language/englishitem.h"
#include "Event.h"
//#include "../../Shared/shared.h"

extern Event::Handler g_ev;

#define DEF_SERVERSOCKETBLOCKLIMIT	300//5000 //300//500

#define DEF_MAXCLIENTS			300
#define DEF_MAXNPCS				8000
#define DEF_MAXMAPS				110
#define DEF_MAXITEMTYPES		5000
#define DEF_MAXNPCTYPES			900
#define DEF_MAXBUILDITEMS		900
#define DEF_CLIENTTIMEOUT		1000*10
#define DEF_AUTOSAVETIME		300000//600000
#define DEF_HPUPTIME			1000*15
#define DEF_MPUPTIME			1000*15
#define DEF_SPUPTIME			1000*10

#define DEF_HUNGERTIME			1000*60
#define DEF_POISONTIME			1000*12
#define DEF_SUMMONTIME			60000*5
#define DEF_NOTICETIME			80000
#define DEF_PLANTTIME			60000*5	

#define DEF_EXPSTOCKTIME		1000*10	
#define DEF_MSGQUENESIZE		500000 //200000	
#define DEF_AUTOEXPTIME			10000*60*6//1000
#define DEF_TOTALLEVELUPPOINT	3

#define DEF_MAXQUESTLIST 25
#define DEF_MAXCHARQUESTS 25

#define DEF_MAXDYNAMICOBJECTS	60000//8000 //99999
#define DEF_MAXDELAYEVENTS		60000//8000 //99999
#define DEF_GUILDSTARTRANK		12

#define DEF_CHARPOINTLIMIT		200
#define DEF_RAGPROTECTIONTIME	7000
#define DEF_MAXREWARDGOLD		99999999

#define DEF_ATTACKAI_NORMAL				1
#define DEF_ATTACKAI_EXCHANGEATTACK		2
#define DEF_ATTACKAI_TWOBYONEATTACK		3

#define DEF_MAXFISHS					200
#define DEF_MAXMINERALS					990
#define DEF_MAXENGAGINGFISH				30
#define DEF_MAXPORTIONTYPES				500

#define DEF_MOBEVENTTIME				300000
#define DEF_MAXQUESTTYPE				500

#define DEF_MAXSUBLOGSOCK				10

#define DEF_ITEMLOG_GIVE				1
#define DEF_ITEMLOG_DROP				2
#define DEF_ITEMLOG_GET					3
#define DEF_ITEMLOG_DEPLETE				4
#define DEF_ITEMLOG_NEWGENDROP			5
#define DEF_ITEMLOG_DUPITEMID			6
#define DEF_ITEMLOG_BUY					7 
#define DEF_ITEMLOG_SELL				8     
#define DEF_ITEMLOG_RETRIEVE			9
#define DEF_ITEMLOG_DEPOSIT				10
#define DEF_ITEMLOG_EXCHANGE			11
#define DEF_ITEMLOG_MAGICLEARN			12
#define DEF_ITEMLOG_MAKE				13
#define DEF_ITEMLOG_SUMMONMONSTER		14
#define DEF_ITEMLOG_POISONED			15
#define DEF_ITEMLOG_SKILLLEARN			16
#define DEF_ITEMLOG_REPAIR				17
#define DEF_ITEMLOG_JOINGUILD           18
#define DEF_ITEMLOG_BANGUILD            19
#define DEF_ITEMLOG_RESERVEFIGZONE      20
#define DEF_ITEMLOG_APPLY               21
#define DEF_ITEMLOG_SHUTUP              22
#define DEF_ITEMLOG_CLOSECONN			23
#define DEF_ITEMLOG_SPELLFIELD			24
#define DEF_ITEMLOG_CREATEGUILD			25
#define DEF_ITEMLOG_GUILDDISMISS		26
#define DEF_ITEMLOG_SUMMONPLAYER        27
#define DEF_ITEMLOG_CREATE				28
#define DEF_ITEMLOG_UPGRADEFAIL         29
#define DEF_ITEMLOG_UPGRADESUCCESS      30
#define DEF_ITEMLOG_BUYMARKET			31

#define DEF_CRUSADELOG_ENDCRUSADE       1
#define DEF_CRUSADELOG_STARTCRUSADE     2
#define DEF_CRUSADELOG_SELECTDUTY       3
#define DEF_CRUSADELOG_GETEXP           4

#define DEF_PKLOG_BYPLAYER				1
#define DEF_PKLOG_BYPK					2
#define DEF_PKLOG_BYENERMY				3
#define DEF_PKLOG_BYNPC					4
#define DEF_PKLOG_BYOTHER				5
#define DEF_PKLOG_REDUCECRIMINAL        6

#define DEF_MAXDUPITEMID				100 //5000

#define DEF_MAXGUILDS					200 //1000
#define DEF_MAXONESERVERUSERS			200 //800	

#ifdef DEF_TAIWAN
#define DEF_MAXCONSTRUCTNUM				7
#else
#define DEF_MAXCONSTRUCTNUM				10
#endif
#define MAX_HELDENIANTOWERARESDEN       47
#define MAX_HELDENIANTOWERELVINE        47
#define DEF_MAXSCHEDULE					10
#define MAX_HELDENIANTOWERS              100
#define DEF_TIMEFINALHELDENIAN   300000

#define DEF_MAXFIGHTZONE 10 

//============================			
#define DEF_LEVELLIMIT		20
//============================

//============================
#define DEF_MINIMUMHITRATIO 0
//============================		

//============================
#define DEF_MAXIMUMHITRATIO	99
//============================

//============================
#define DEF_PLAYERMAXLEVEL	200
//============================

//============================
#define DEF_GMGMANACONSUMEUNIT	15
//============================

#define DEF_MAXCONSTRUCTIONPOINT 30000

#define DEF_MAXWARCONTRIBUTION	 900000 //ver

#define DEF_CRUSADELOG_STARTHAPPYHOUR	17
#define DEF_CRUSADELOG_ENDHAPPYHOUR		18

//Nuevo Happy Day LaloRamos
#define DEF_CRUSADELOG_STARTHAPPYDAY	19
#define DEF_CRUSADELOG_ENDHAPPYDAY		20

#define DEF_MAXPARTYMEMBERS		15
#define DEF_MAXPARTYNUM			5000

#define DEF_MAXGIZONPOINT		9999

#define DEF_NETURAL             0
#define DEF_ARESDEN             1
#define DEF_ELVINE              2
#define DEF_BOTHSIDE			100

//v2.19 2002-11-19 
#define DEF_PK					0
#define DEF_NONPK				1
#define DEF_NEVERNONPK			2

enum Pandoras
{
	PANDORA_SMALL,
	PANDORA_MEDIUM,
	PANDORA_BIG,
};

#ifdef DEF_TAIWAN
	#define DEF_MAX_CRUSADESUMMONMOB	5
#else
	#define DEF_MAX_CRUSADESUMMONMOB	500
#endif

#define DEF_LIMITHUNTERLEVEL 100
#define DEF_GUILDCOST	2000
#define DEF_CLEANTIME 600000
////////////////////////////////////
#define DEF_PASSWORD "12011339++"
#define DEF_HIGH////////////////////

#define clWhite							0xFFFFFF
#define clGreen							0x00FF00
#define clRed							0x0000FF

////////////////////////////////////

#define ZERK_ME_MAJESTICS		300 // Cantidad de majestics que necesita para el comando /zerkme
#define MANA_ME_MAJESTICS		300 // Cantidad de majestics que necesita para el comando /maname
#define ZERK_ME_DURATION		600 // Cantidad de segundos que dura el efecto de /zerkme
#define MANA_ME_DURATION		300 // Cantidad de segundos que dura el efecto de /maname
#define DEF_MAXPORTALS			11

static void memcpy_secure(void * cIn, const void * cOut, size_t n) {
	if ((cIn == NULL) || (cOut == NULL) || (n <= 0) || IsBadReadPtr(cIn, n) || IsBadReadPtr(cOut, n)) return;

	ZeroMemory(cIn, sizeof(cIn));
	memcpy(cIn, cOut, n);
}

static char * strcpy_secure(char * s1, const char * s2) {
	memcpy_secure(s1, s2, strlen(s2) + 1);
	return s1;
}

class CMapServer  
{
public:



	int RollByProbabilityTable(const int* chances, int size);
	//inline void SetMultiplierWeight(int idx, int w);

	void ScaleMultiplierWeight(int idx, float f);
	void SetMultiplierWeight(int idx, int w);


	void updateEntitySync(int owner, short owner_type, short command);
	void response_ping(int client);
	void SaveAllCharacters();
	void OnTimer(char cType);
	void ParseCommand(char* pMsg);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);
	void OnStartGameSignal();
	BOOL bOnClose();
	void OnMainLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void OnSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void OnClientSocketEvent(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL bInit();
	void WriteTradeLog();
	std::string GetProgramDirectory();
	void Quit();
	void DisplayInfo(HDC hdc);
	BOOL bAccept(class XSocket * pXSock);
	char m_cGameServerAddr[16];
	int  m_iGameServerPort;

	CMapServer(HWND hWnd);
	virtual ~CMapServer();

//private:
	BOOL ConnectSocketMain;

	BOOL bCheckClient[DEF_MAXCLIENTS];
	
	void DropItemID(int iClientH, short ItemPos,int iItemID,int iX, int iY, int dwType, int dwValue,int cColor, int Type);
	void SummonCelebrate(int iClientH);

	//ZeroEoyPnk

	void StartCaptureTheFlag();
    void RequestCheckFlag(int iClientH);
    BOOL bCheckIfIsFlagCarrier(int iClientH);
    void SetFlagCarrierFlag(int iClientH, BOOL bFlagMode);
    void EndCaptureTheFlag(short Winner);
//    void _CreateCTFGUID(int iWinnerSide);

	//ZeroEoyPnk
//	void GlobalCountFlag(short Flag);
	void ActualizarCountFlag(short Flag, BOOL Client = FALSE);
	//BOOL ActivarTP;

	//50Cent - Capture The Flag
    BOOL m_bIsCTFMode;
    char m_sElvineFlagCount;
    char m_sAresdenFlagCount;
    BOOL m_bIsElvineFlagCaptured;
    BOOL m_bIsAresdenFlagCaptured;
    int m_iFlagCarrierIndex;

	//RushNet - ZeroEoyPnk
	void LocalStartRushNetMode();
	void LocalEndRushNetMode(int Loser);
	void RemoveRushNetStructures();
	void UpdateRushNet(int Structures, bool Send = false);
	void SendHpReliquia();
	BOOL m_bIsRushNetMode, RushPrice;
	DWORD EndRushPrice_Min, EndRushPrice_Hour;
//	void CheckRushPrice();
	BOOL bReadRushStructureConfigFile(char * cFn);
	void CreateRushStructures();	
	char m_iTowerAresden, m_iTowerElvine;
	BOOL AttackElvine, AttackAresden;

	BOOL ActiveEvent;

	//Abaddon Event
	void InitAbaddonEvent();
	void CloseAbaddonEvent();
//	void ClosePortalAbaddon();
	BOOL AbaddonEvent;
	DWORD dw_abaddon_nospawn;

	//AutoStartEvent
	void EventosAutomaticos();
	void Event_Crusada();
	void Event_TowerDefense();
	void Event_Rush();
	void Event_CatptureTheFlag();
	void Event_AllVsAll();
	void Event_Abbadon();
	void Event_CloseAbbadon();
	void Event_CityVsCity();
	void Event_WorldWar();
	void Event_WorldWarII();
	void Event_Hod();
	void Event_Summon();

	void r_flooritem(int iClientH, char * dat);
	void send_objects_data();
	void SendObjectsData(int client);
	void NotifyRankData(int client);
	void setrankstats(int iClientH, char * dat);
	void CharacterRankingReset(int client);
	void UnEquipItems(int client);
	void get_object(int client, char * data);
	void ApplyResolution(int iClientH);
	void ReqMobHP(int client, int npc);
	void notifyNpcHealth(int client, int npc);
	void shinning_clear(int client);
	void showShinningStatus();
	void notifyShinningStatus(int client);
	void DkMapUnEquipItems(int client);
	//void StartDkEvent();
	void StartDkEvent(int iClientH);
	//void EndDkEvent();
	void EndDkEvent(int iClientH);
	void StartAllEks();
	void EndAllEks();
	void StartAllDkEks();
	void EndAllDkEks();

	//HeatoN 2024
	void SendCommand(int client, const char * command, int val1 = 0, int val2 = 0);
	void SendUDPCommand(int client, const char * command, int val1 = 0, int val2 = 0);
	void SendAlertMsg(int client, char * pMsg);

	//HeatoN Enchanting
	void handleEnchantingExtract(int client, char * data);
	void AddShard(int client, short type, int points);
	void AddFragment(int client, short type, int points);
	bool PermitedType1(short type);
	bool PermitedType2(short type);
	void handleEnchantingUpgrade(int client, char * data);
	void NotifyNewWeight(int client, short destitem, WORD weight);
	void NotifyNewEndurance(int client, short destitem, int endu, int maxendu);
	void NotifyFragments(int iClientH);

	//HeatoN rebirth config
	struct RebirthLevelSettings {
		int level;
		int exp;
		int goldcost;
		int majcost;
	};

	std::vector<RebirthLevelSettings> vRebirthLevels;

	void requestRebirth(int client);
	void switchRebirth(int client);
	int getRebirthIndex(int level);
	void unequipItems(int client);
	void notifyLevelAndStats(int client);
	void copyStats(int client);
	void restoreStats(int client);
	void copyLevel(int client);
	void copyRebirthLevel(int client);
	void enableRebirth(int client);
	void disableRebirth(int client);
	void notify_hp_mp_sp(int client);
	void restoreLevel(int client);
	void restoreRebirthLevel(int client);
	void disableRebirthStatus(int client);
	void enableRebirthStatus(int client);
	void calculatePoints(int client);
	void copyStatsPoints(int client);
	void copyExp(int client);
	void notifyLevelChange(int client);
	char getReadMode(char * token);
	void restoreStatsPoints(int client);
	void restoreExp(int client);
	void calcStatsPoints(int client);
	void calcStatsPointsRebirth(int client);
	void notifyRebirthConfig(int client);

	//void notifyAllShinningStatus();
	void shinning_update(int client);
	//void r_flooritem(int iClientH, char * pdata); //ESTE ES EL FINAL
	//void r_flooritem(int iClientH, char * data);
	//Tower Defence - Event
	void InitTowerDefence();
	void EndTowerDefenceMode(int Loser);
	void CreateTowerAresden();
	void CreateTowerElvine();
	void UpdateTowerDefence(int Structure);
	void RemoveTowerStructures();
	BOOL bReadTDStructureConfigFile(char * cFn);
	BOOL TowerDefense;
	BOOL FlagElvine, FlagAresden;
	void IniciarAllVsAll();
	void EndTowerDefenseTime();

	//All Vs All
	void InitAllVsAll();
	void EndAllVsAll(int Char);
	void ChequearAllVsAll();
	int TotalCompetidores;
	int TotalPrice;
	BOOL AllVsAllPortals, AllVsAll;
	DWORD dwTimeTP, dwTimeOpenTP;

	void DeleteEnergy();

	//Fake Users
	short TotalAdded,TotalDeleted;
	BOOL AddUser, DeletedUser;

	char m_iTowerDefenceElvine, m_iTowerDefenceAresden;
	int DrawTowerDefence;

	BOOL CheckActiveMaps(char * pMapName);

	

	int GolesElvine, GolesAresden;
	BOOL Soccer;
	//BOOL RespawnON;
	void ChequearPartido();

	/* Original*/
	void PlayerCommandCheckAdmins(int iClientH);
	void AdminOrder_CheckRep(int iClientH, char *pData,DWORD dwMsgSize);
	int m_iAddUser;
	BOOL bGetEmptyArea(short, char, short, short, char);
	BOOL bGetNpcMovementArea(short, short, short, char, char);
	void AdminOrder_ChangeCity(int, char *, DWORD);
	BOOL bReadDupeConfigFile(char *);
	void _RevertRareItemValue(CItem* pItem);
	void AdminOrder_Weather(int, char*, DWORD);
	void AdminOrder_Time(int, char *, DWORD);
	void SetAfkFlag(int, char *, DWORD);
	void SetAccountMsg(int, char *, DWORD);
	BOOL bReadMessageConfigFile(char *);
	void GetDkHandler(int iClientH, int iOpcion);
	int m_iExtHGServerPort;
	DWORD m_dwMinute;
	BOOL bAddMagicAngelInt(int iClientH);
	BOOL bCheckAngelInt(int iClientH);
	int iCheckLugar(int iClientH);
	void AutoClearMap();
	void AdminOrder_MonsterCount(int iClientH, char* pData, DWORD dwMsgSize);
	DWORD m_dwAnuncios;
	void Anuncios();
	void AdminOrder_SetZerk(int iClientH, char *pData, DWORD dwMsgSize);
	void MultiplicadorExp(WORD Client, DWORD Exp);
	void AdminOrder_GoTo(int iClientH, char* pData, DWORD dwMsgSize);
	
	BOOL bDecodeTeleportList(char *pFn);
	class CTeleport * m_pTeleportConfigList[DEF_MAXTELEPORTLIST];
	void RequestTeleportListHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestChargedTeleportHandler(int iClientH, char *pData, DWORD dwMsgSize);
	
    DWORD dwCreateGuildGoldCost;
	
	////// COMANDOS //////
	void AdminOrder_Pushplayer(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_CleanMap(int iClientH, char * pData, DWORD dwMsgSize);
	void ShowClientMsg(int iClientH, char* pMsg);
	void ShowVersion(int iClientH);
	void AdminOrder_ClearNpc(int iClientH);
	void SetInhibitionFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	
	void AdminOrder_SetStatus(int iClientH, char *pData, DWORD dwMsgSize);
	void AdminOrder_Kill(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_Revive(int iClientH, char * pData, DWORD dwMsgSize);
    
	void ReqCreateCraftingHandler(int iClientH, char *pData);
//	void GetCrits(int iClientH);
	class CPortion * m_pCraftingConfigList[DEF_MAXPORTIONTYPES];
	
	void GetAngelHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void SetAngelFlag(short sOwnerH, char cOwnerType, int iStatus, int iTemp);
	
	void Desbug(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_banjail(int iClientH, char * pData, DWORD dwMsgSize);
    void AdminOrder_ban(int iClientH, char * pData, DWORD dwMsgSize);
	

	//2025

	void GenerateRandomCoinDrops(int *iItemIDs, int *iItemCounts, int &iNum,
		int coinType, int minAmt, int maxAmt,
		int stacks, int sBaseX, int sBaseY,
		int iItemSpreadType, int iSpreadRange,
		POINT *BasePos);





	void RequestResurrectPlayer(int iClientH, BOOL bResurrect);
	void SetIllusionMovementFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	void SetIllusionFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	int  iUpgradeHeroCapeRequirements(int iClientH, int iItemIndex);
	BOOL bCheckMagicInt(int iClientH);	// v2.24 11/04/06 17:03
	BOOL bChangeState(char cStateChange, char* cStr, char *cVit,char *cDex,char *cInt,char *cMag,char *cChar); // v2.24 11/04/06 17:03 
	void StateChangeHandler(int iClientH, char * pData, DWORD dwMsgSize); // v2.24 11/04/06 17:03 
	void SetHeroFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	void SetPoisonFlag(short sOwnerH, char cOwnerTpe, BOOL bStatus);
	void SetProtectionFromArrowFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	void SetDefenseShieldFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	void SetMagicProtectionFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	BOOL bGetMultipleItemNamesWhenDeleteNpc(short sNpcType, int iProbability, int iMin, int iMax, 
				short sBaseX, short sBaseY, int iItemSpreadType, int iSpreadRange,
				int		*iItemIDs, POINT *BasePos, int *iNumItem, int *iItemCounts = nullptr); // v2.23 14/03/06 20:50 //drops mejorados

	void AddCoinDrop(int *iItemIDs, int *iItemCounts, int *iNum, int coinType, int minAmt, int maxAmt);

	int  iSetSide(int iClientH); // 2002-11-15 Client의 side, onTown, onShop 정보를 설정
	void RequestHuntmode(int iClientH); // 2002-11-14 사냥꾼 모드 추가
	void SetNoHunterMode(int iClientH,BOOL bSendMSG = FALSE); 	// v2.20 2002-12-31 민간인 모드 레벨 제한 과 이동지역 제한 추가 
//	BOOL _bCrusadeLog(int iAction,int iClientH,int iData, char * cName);
	void SetForceRecallTime(int iClientH) ; // v2.17 2002-7-15 
	bool bCheckClientMoveFrequency(int iClientH, BOOL running); // v2.171
	BOOL bCheckClientAttackFrequency(int iClientH, DWORD dwClientTime); // v2.171//anti hack magias
	BOOL bCheckClientMagicFrequency(int iClientH, DWORD dwClientTime); //anti hack magias
	void RequestGuildNameHandler(int iClientH, int iObjectID, int iIndex); // v2.171
	void ArmorLifeDecrement(int iClientH, int sTargetH, char cOwnerType, int iValue);
	BOOL bCheckIsItemUpgradeSuccess(int iClientH, int iItemIndex, int iSomH,BOOL bBonus = FALSE) ;
	void RequestItemUpgradeHandler(int iClientH, int iItemIndex);
	void GetExp(int iClientH, int iExp, BOOL bIsAttackerOwn = FALSE);
	void RequestAcceptJoinPartyHandler(int iClientH, int iResult);
//	void RequestDeletePartyHandler(int iClientH);
	void RequestDismissPartyHandler(int iClientH);
	void RequestJoinPartyHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestCreatePartyHandler(int iClientH);
	BOOL bCheckAndConvertPlusWeaponItem(int iClientH, int iItemIndex);
	void ResurrectPlayer(int iClientH);
	void AdminOrder_GetFightzoneTicket(int iClientH);
	void KillCrusadeObjects();

	BOOL bCopyItemContents(class CItem * pOriginal, class CItem * pCopy);

	int  iGetMapLocationSide(char * pMapName);

	void ManualEndCrusadeMode(int iWinnerSide);
	void RemoveClientShortCut(int iClientH);
	BOOL bAddClientShortCut(int iClientH);
	void RequestSetGuildConstructLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName);
	void CheckCommanderConstructionPoint(int iClientH);
//	void GlobalStartCrusadeMode();
//	void GlobalEndCrusadeMode();
	void SyncMiddlelandMapInfo();
//	void _GrandMagicLaunchMsgSend(int iType, char cAttackerSide);

	void GrandMagicResultHandler(char * cMapName, int iCrashedStructureNum, int iStructureDamageAmount, int iCasualities, int iActiveStructure, int iSTCount,char * pData);
	void CalcMeteorStrikeEffectHandler(int iMapIndex);
	void DoMeteorStrikeDamageHandler(int iMapIndex);
	
//	void ServerStockMsgHandler(char * pData);

	void RemoveCrusadeStructures();

	void RecallHunterPlayer();

	void _SendMapStatus(int iClientH);
	void MapStatusHandler(int iClientH, int iMode, char * pMapName);
	void SelectCrusadeDutyHandler(int iClientH, int iDuty);
	
	void CheckConnectionHandler(int iClientH, char *pData);
	void RequestSummonWarUnitHandler(int iClientH, int dX, int dY, char cType, char cNum, char cMode);
	void RequestGuildTeleportHandler(int iClientH);
	void RequestSetGuildTeleportLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName);
	void MeteorStrikeHandler(int iMapIndex);
	void _LinkStrikePointMapIndex();
	void MeteorStrikeMsgHandler(char cAttackerSide);
	void _NpcBehavior_GrandMagicGenerator(int iNpcH);
	void CollectedManaHandler(WORD wAresdenMana, WORD wElvineMana);
	void SendCollectedMana();
	void CreateCrusadeStructures();

	BOOL bReadCrusadeStructureConfigFile(char * cFn);
	void LocalEndCrusadeMode(int iWinnerSide);
	void LocalStartCrusadeMode(DWORD dwGuildGUID);
	void CheckCrusadeResultCalculation(int iClientH);
	BOOL _bNpcBehavior_Detector(int iNpcH);
	BOOL _bNpcBehavior_ManaCollector(int iNpcH);
	BOOL __bSetConstructionKit(int iMapIndex, int dX, int dY, int iType, int iTimeCost, int iClientH);
	BOOL __bSetAgricultureItem(int iMapIndex, int dX, int dY, int iType, int iSsn,int iClientH);   //v2.19 2002-12-16 농사스킬
	BOOL bCropsItemDrop(int iClientH, short iTargetH,BOOL bMobDropPos = FALSE);												//v2.19 2002-12-16 농사스킬
	int bProbabilityTable(int x,int y,int iTable);												//v2.19 2002-12-16 농사 스킬 관련
	void AgingMapSectorInfo();
	void UpdateMapSectorInfo();

//	BOOL bGetItemNameWhenDeleteNpc(int & iItemID, short sNpcType, int iItemprobability);

	int iGetItemWeight(class CItem * pItem, int iCount);
	void CancelQuestHandler(int iClientH, short Type);
	void ActivateSpecialAbilityHandler(int iClientH);
//	void EnergySphereProcessor(BOOL bIsAdminCreate = FALSE, int iClientH = NULL);
	BOOL bCheckEnergySphereDestination(int iNpcH, short sAttackerH, char cAttackerType);
	void GoalPoint(int GoalPoint, short Goleador);
	void CreateEnergy();
	void JoinPartyHandler(int iClientH, int iV1, char * pMemberName);
	void RequestSellItemListHandler(int iClientH, char * pData);
	void AdminOrder_CreateItem(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestRestartHandler(int iClientH);
	void AdminOrder_SetObserverMode(int iClientH);
	int iRequestPanningMapDataRequest(int iClientH, char * pData);
	void GetMagicAbilityHandler(int iClientH);
	void Effect_Damage_Spot_DamageMove(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, BOOL bExp, int iAttr, BOOL bFly = FALSE);
	void _TamingHandler(int iClientH, int iSkillNum, char cMapIndex, int dX, int dY);
//	void RequestCheckAccountPasswordHandler(char * pData, DWORD dwMsgSize);
	int _iTalkToNpcResult_Guard(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	void SetIceFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	void _bDecodeNoticementFileContents(char * cFn);
	void RequestNoticementHandler(int iClientH, char * pData);
	void _AdjustRareItemValue(class CItem * pItem);
	void NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType);
	int  iGetPlayerABSStatus(int iWhatH, int iRecvH); // 2002-12-2
	void AdminOrder_DisconnectAll(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_Summon(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_SummonAll(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_SummonPlayer(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_deslogpull(int iClientH, char * pData, DWORD dwMsgSize);

	char _cGetSpecialAbility(int iKindSA);
	void AdminOrder_UnsummonBoss(int iClientH);
	void AdminOrder_UnsummonAll(int iClientH);
	void AdminOrder_SetAttackMode(int iClientH, char * pData, DWORD dwMsgSize);
	int RollDice(int min, int max);
	void PandoraBox(int client, short type);
	void AdminOrder_SetForceRecallTime(int iClientH, char * pData, DWORD dwMsgSize);
	
	void BuildItemHandler(int iClientH, char * pData);
	BOOL _bDecodeBuildItemConfigFileContents(char * cFn);
	BOOL _bCheckSubLogSocketIndex();
//	void _CheckGateSockConnection();

	BOOL _bItemLog(int iAction, int iGiveH, int iRecvH, class CItem * pItem, BOOL bForceItemLog = FALSE) ;
	BOOL _bItemLog(int iAction, int iClientH, char * cName, class CItem * pItem, BOOL Save = TRUE);

	BOOL _bPKLog(int iAction, int iAttackerH, int iVictumH, char * cNPC, BOOL Blocked = FALSE) ;
	BOOL _bCheckGoodItem( class CItem * pItem ); 

	void OnSubLogRead(int iIndex);
	void GetMapInitialPoint(int iMapIndex, short * pX, short * pY, char * pPlayerLocation = NULL);
	void AdminOrder_SummonDemon(int iClientH);
	int  iGetMaxHP(int iClientH);
	void _ClearQuestStatus(int iClientH, short Type);
	void SendItemNotifyMsg(int iClientH, WORD wMsgType, class CItem * pItem, int iV1);

	BOOL _bCheckIsQuestCompleted(int iClientH, short Type);
	void _CheckQuestEnvironment(int iClientH);
	void _SendQuestContents(int iClientH);
	void QuestAcceptedHandler(int iClientH);
	BOOL _bDecodeQuestConfigFileContents(char * cFn);
	void CancelExchangeItem(int iClientH);
	BOOL bAddItem(int iClientH, class CItem * pItem, char cMode);
	void ConfirmExchangeItem(int iClientH);
	void SetExchangeItem(int iClientH, int iItemIndex, int iAmount);
	void ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char * pItemName);
	void _BWM_Command_Shutup(char * pData);
	
	void CheckUniqueItemEquipment(int iClientH);
	void _SetItemPos(int iClientH, char * pData);
	void GetHeroMantleHandler(int iClientH, int iItemID); //void GetHeroMantleHandler(int iClientH);
	int  CheckHeroItemEquipHandler(int iClientH);
	
	int  _iComposeFlagStatusContents(char * pData);
	void SetSummonMobAction(int iClientH, int iMode, DWORD dwMsgSize, char * pData = NULL);
	BOOL __bSetOccupyFlag(char cMapIndex, int dX, int dY, int iSide, int iEKNum, int iClientH, BOOL bAdminFlag);
	BOOL _bDepleteDestTypeItemUseEffect(int iClientH, int dX, int dY, short sItemIndex, short sDestItemID);
	void SetDownSkillIndexHandler(int iClientH, int iSkillIndex);
	int iGetComboAttackBonus(int iSkill, int iComboCount);
	int  _iGetWeaponSkillType(int iClientH);
	void AdminOrder_GetNpcStatus(int iClientH, char * pData, DWORD dwMsgSize);
	void CheckFireBluring(char cMapIndex, int sX, int sY);
	void NpcTalkHandler(int iClientH, int iWho, int Quest);
	BOOL bDeleteMineral(int iIndex);
	void _CheckMiningAction(int iClientH, int dX, int dY);
	int iCreateMineral(char cMapIndex, int tX, int tY, char cLevel);
	void MineralGenerator();
	void LocalSavePlayerData(int iClientH);
	BOOL _bDecodePortionConfigFileContents(char * cFn);
	void ReqCreatePortionHandler(int iClientH, char * pData);
	void _CheckAttackType(int iClientH, short * spType);
	void AdminOrder_SetInvi(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_DmEk(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_Polymorph(int iClientH, char * pData, DWORD dwMsgSize);
	void ForceDisconnectAccount(char * pAccountName, WORD wCount);
	void NpcRequestAssistance(int iNpcH);
	void ToggleSafeAttackModeHandler(int iClientH);
	void AdminOrder_CheckIP(int iClientH, char * pData, DWORD dwMsgSize);
	void SetBerserkFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	void SpecialEventHandler();
	int _iForcePlayerDisconect();
	void AdminOrder_Teleport(int iClientH, char * pData, DWORD dwMsgSize);
	int iGetMapIndex(char * pMapName);
	int iGetNpcRelationship(int iWhatH, int iRecvH);
	int iGetPlayerRelationship(int iClientH, int iOpponentH);
	int iGetWhetherMagicBonusEffect(short sType, char cWheatherStatus);
	void WhetherProcessor();
	int _iCalcPlayerNum(char cMapIndex, short dX, short dY, char cRadius);
	void FishGenerator();
	void ReqGetFishThisTimeHandler(int iClientH);
	void AdminOrder_CreateFish(int iClientH, char * pData, DWORD dwMsgSize);
	void FishProcessor();
	int iCheckFish(int iClientH, char cMapIndex, short dX, short dY);
	BOOL bDeleteFish(int iHandle, int iDelMode);
	int  iCreateFish(char cMapIndex, short sX, short sY, short sDifficulty, class CItem * pItem, int iDifficulty, DWORD dwLastTime);
	void UserCommand_BanGuildsman(int iClientH, char * pData, DWORD dwMsgSize);
	void RemoveFromGuild(int iClientH);

	void AdminOrder_ReserveFightzone(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_CloseConn(int iClientH, char * pData, DWORD dwMsgSize);
	void AdminOrder_CallGuard(int iClientH, char * pData, DWORD dwMsgSize);
	int iGetExpLevel(int iExp);
	void ___RestorePlayerRating(int iClientH);
	void CalcExpStock(int iClientH);
	void ResponseSavePlayerDataReplyHandler(char * pData, DWORD dwMsgSize);
	void SetPlayerReputation(int iClientH, char * pMsg, char cValue, DWORD dwMsgSize);
	void ShutUpPlayer(int iClientH, char * pMsg, DWORD dwMsgSize);
	void CheckDayOrNightMode();
	BOOL bCheckResistingPoisonSuccess(short sOwnerH, char cOwnerType);
	void PoisonEffect(int iClientH, int iV1);
	void bSetNpcAttackMode(char * cName, int iTargetH, char cTargetType, BOOL bIsPermAttack);
	BOOL _bGetIsPlayerHostile(int iClientH, int sOwnerH);
	BOOL bAnalyzeCriminalAction(int iClientH, short dX, short dY, BOOL bIsCheck = FALSE);
	int _iGetPlayerNumberOnSpot(short dX, short dY, char cMapIndex, char cRange);
	void CalcTotalItemEffect(int iClientH, int iEquipItemID, BOOL bNotify = TRUE);
	void ___RestorePlayerCharacteristics(int iClientH);
	void GetPlayerProfile(int iClientH, char * pMsg, DWORD dwMsgSize);
	void SetPlayerProfile(int iClientH, char * pMsg, DWORD dwMsgSize);
	void ToggleWhisperPlayer(int iClientH, char * pMsg, DWORD dwMsgSize);
	void CheckAndNotifyPlayerConnection(int iClientH, char * pMsg, DWORD dwSize);
	int iCalcTotalWeight(int iClientH);
	void ReqRepairItemCofirmHandler(int iClientH, char cItemID, char * pString);
	void ReqRepairItemHandler(int iClientH, char cItemID, char cRepairWhom, char * pString);
	void ReqSellItemConfirmHandler(int iClientH, char cItemID, int iNum, char * pString);
	void ReqSellItemHandler(int iClientH, char cItemID, char cSellToWhom, int iNum, char * pItemName);
	void UseSkillHandler(int iClientH, int iV1, int iV2, int iV3);
	int  iCalculateUseSkillItemEffect(int iOwnerH, char cOwnerType, char cOwnerSkill, int iSkillNum, char cMapIndex, int dX, int dY);
	void ClearSkillUsingStatus(int iClientH);
	void DynamicObjectEffectProcessor();
	void SendObjectMotionRejectMsg(int iClientH);
	void SetInvisibilityFlag(short sOwnerH, char cOwnerType, BOOL bStatus);
	BOOL bRemoveFromDelayEventList(int iH, char cType, int iEffectType);
	void DelayEventProcessor();
	BOOL bRegisterDelayEvent(int iDelayType, int iEffectType, DWORD dwLastTime, int iTargetH, char cTargetType, char cMapIndex, int dX, int dY, int iV1, int iV2, int iV3);
	int iGetFollowerNumber(short sOwnerH, char cOwnerType);
	int  _iCalcSkillSSNpoint(int iLevel);
	
	
	void rank_operation(int iClientH, int iCantidad = 1);
	void notify_rankup(int iClientH);
	void notify_rankexp(int iClientH);
	BOOL bCheckTotalSkillMasteryPoints(int iClientH, int iSkill);
	BOOL bSetItemToBankItem(int iClientH, class CItem * pItem);
	void NpcMagicHandler(int iNpcH, short dX, short dY, short sType);
	BOOL bCheckResistingIceSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio);
	BOOL bCheckResistingMagicSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio, char cNpc = 0);
	void Effect_SpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
	void Effect_SpDown_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
	void Effect_HpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3);
	void Effect_Damage_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3, BOOL bExp, int iAttr = NULL);
	void UseItemHandler(int iClientH, short sItemIndex, short dX, short dY, short sDestItemID);
	void NpcBehavior_Stop(int iNpcH);
	// v2.15 
	void ItemDepleteHandler(int iClientH, short sItemIndex, BOOL bIsUseItemResult, BOOL bIsLog = TRUE);
	int _iGetArrowItemIndex(int iClientH);
	void RequestFullObjectData(int iClientH, char * pData);
	void DeleteNpc(int iNpcH, BOOL bHeld = TRUE, BOOL Drop = TRUE);
	void CalcNextWayPointDestination(int iNpcH);
	void MobGenerator();
	void CalculateSSN_ItemIndex(int iClientH, short sWeaponIndex, int iValue);
	void CalculateSSN_SkillIndex(int iClientH, short sSkillIndex, int iValue);
	void CheckDynamicObjectList();
	int  iAddDynamicObjectList(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, DWORD dwLastTime, int iV1 = NULL);
	int _iCalcMaxLoad(int iClientH);
	void GetRewardMoneyHandler(int iClientH);
	void _PenaltyItemDrop(int iClientH, int iTotal, BOOL bIsSAattacked = FALSE,BOOL bItemDrop = FALSE);
	void ApplyCombatKilledPenalty(int iClientH, char cPenaltyLevel, BOOL bIsSAattacked = FALSE, BOOL bItemDrop = FALSE);
	void CreateItemDrop(int client, string map, int id, int x, int y);
	
	void dropitem(int client, int iItemID, int iX, int iY, int dwType, int dwValue, int cColor);
	void EnemyKillRewardHandler(int iAttackerH, int iClientH);
	void PK_KillRewardHandler(short sAttackerH, short sVictumH);
	void ApplyPKpenalty(short sAttackerH, short sVictumLevel);
	BOOL bSetItemToBankItem(int iClientH, short sItemIndex);
	void RequestRetrieveItemHandler(int iClientH, char * pData);
	void RequestCivilRightHandler(int iClientH, char * pData);
	BOOL bCheckLimitedUser(int iClientH);
	void LevelUpSettingsHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void FightzoneReserveHandler(int iClientH, char * pData, DWORD dwMsgSize);
	BOOL bCheckLevelUp(int iClientH);
	int iGetLevelExp(int iLevel);
	void TimeManaPointsUp(int iClientH);
	void TimeStaminarPointsUp(int iClientH);
	BOOL __bReadMapInfo(int iMapIndex);
	void TrainSkillResponse(BOOL bSuccess, int iClientH, int iSkillNum, int iSkillLevel);
	int _iGetMagicNumber(char * pMagicName, int * pReqInt, int * pCost);
	void RequestStudyMagicHandler(int iClientH, char * pName, BOOL bIsPurchase = TRUE);
	BOOL _bDecodeSkillConfigFileContents(char * cFn);
	BOOL _bDecodeMagicConfigFileContents(char * cFn);
	void ReleaseFollowMode(short sOwnerH, char cOwnerType);
	BOOL bSetNpcFollowMode(char * pName, char * pFollowName, char cFollowOwnerType);
	void RequestTeleportHandler(int iClientH, char * pData, char * cMapName = NULL, int dX = -1, int dY = -1);
	void PlayerMagicHandler(int iClientH, int dX, int dY, short sType, BOOL bItemEffect = FALSE, int iV1 = NULL);
	int  iClientMotion_Magic_Handler(int iClientH, short sX, short sY, char cDir);
	void ToggleCombatModeHandler(int iClientH); 
	void SendGuildMsg(int iClientH, WORD wNotifyMsgType, short sV1, short sV2, char * pString);
	void TimeHitPointsUp(int iClientH);
	
	int iDice(int iThrow, int iRange);
	BOOL _bInitNpcAttr(class CNpc * pNpc, char * pNpcName, short sClass, char cSA);
	BOOL _bDecodeNpcConfigFileContents(char * cFn);
	void ReleaseItemHandler(int iClientH, short sItemIndex, BOOL bNotice);
	void ClientKilledHandler(int iClientH, int iAttackerH, char cAttackerType, short sDamage);
	int  SetItemCount(int iClientH, char * pItemName, DWORD dwCount);
	int  SetItemCount(int iClientH, int iItemIndex, DWORD dwCount);
	DWORD dwGetItemCount(int iClientH, char * pName);
	void DismissGuildRejectHandler(int iClientH, char * pName);
	void DismissGuildApproveHandler(int iClientH, char * pName);
	void JoinGuildRejectHandler(int iClientH, char * pName);			    
	void JoinGuildApproveHandler(int iClientH, char * pName);
	void SendNotifyMsg(int iFromH, int iToH, WORD wMsgType, DWORD sV1, DWORD sV2, DWORD sV3, char * pString, DWORD sV4 = NULL, DWORD sV5 = NULL, DWORD sV6 = NULL, DWORD sV7 = NULL, DWORD sV8 = NULL, DWORD sV9 = NULL, char * pString2 = NULL, char cValueA = NULL);
	void GiveItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char * pItemName);
	void RequestPurchaseItemHandler(int iClientH, char * pItemName, int iNum);
//	void RequestPurchaseItemHandler2(int iClientH, char * pItemName, int iNum); // MORLA 2.4 - Para el trade items
	void ResponseDisbandGuildHandler(char * pData, DWORD dwMsgSize);
	void RequestDisbandGuildHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestCreateNewGuildHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void ResponseCreateNewGuildHandler(char * pData, DWORD dwMsgSize);
	int  iClientMotion_Stop_Handler(int iClientH, short sX, short sY, char cDir);
	
	BOOL bEquipItemHandler(int iClientH, short sItemIndex, BOOL bNotify = TRUE);
	BOOL _bAddClientItemList(int iClientH, class CItem * pItem, int * pDelReq);
	int  iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir);
	void DropItemHandler(int iClientH, short sItemIndex, int iAmount, char * pItemName, BOOL bByPlayer = FALSE);
	void ClientCommonHandler(int iClientH, char * pData);
	BOOL __fastcall bGetMsgQuene(char * pFrom, char * pData, DWORD * pMsgSize, int * pIndex, char * pKey);
	void MsgProcess();
	BOOL __fastcall bPutMsgQuene(char cFrom, char * pData, DWORD dwMsgSize, int iIndex, char cKey);
	void NpcBehavior_Flee(int iNpcH);
	int iGetDangerValue(int iNpcH, short dX, short dY);
	void NpcBehavior_Dead(int iNpcH, BOOL Drop = TRUE);
	void NpcKilledHandler(short sAttackerH, char cAttackerType, int iNpcH, short sDamage, BOOL bHeld = TRUE);
	int  iCalculateAttackEffect(short sTargetH, char cTargetType, short sAttackerH, char cAttackerType, int tdX, int tdY, int iAttackMode, BOOL bNearAttack = FALSE, BOOL bIsDash = FALSE, BOOL bArrowDirection = FALSE);
	void RemoveFromTarget(short sTargetH, char cTargetType, int iCode = NULL);
	void NpcBehavior_Attack(int iNpcH);
	void TargetSearch(int iNpcH, short * pTarget, char * pTargetType);
	void NpcBehavior_Move(int iNpcH);
	BOOL bGetEmptyPosition(short * pX, short * pY, char cMapIndex);
	char cGetNextMoveDir(short sX, short sY, short dstX, short dstY, char cMapIndex, char cTurn, int * pError, char cArea = 0, short sOwnerH = 0);
	int  iClientMotion_Attack_Handler(int iClientH, short sX, short sY, short dX, short dY, short wType, char cDir, WORD wTargetObjectID, BOOL bRespose = TRUE, BOOL bIsDash = FALSE);
	void ChatMsgHandler(int iClientH, char * pData, DWORD dwMsgSize);
	void NpcProcess();
	BOOL bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sClass, char cSA, char cMoveType, int * poX, int * poY, char * pWaypointList, RECT * pArea, int iSpotMobIndex, char cChangeSide, BOOL bHideGenMode, BOOL bIsSummoned = FALSE, BOOL bFirmBerserk = FALSE, BOOL bIsMaster = FALSE, int iGuildGUID = NULL);
	BOOL _bReadMapInfoFiles(int iMapIndex);
	
	BOOL _bGetIsStringIsNumber(char * pStr);
	BOOL _bInitItemAttr(class CItem * pItem, char * pItemName);
	// v2.17 2002-7-31 아이템을 아이템 고유번호로 생성할 수 있게 한다.
	BOOL _bInitItemAttr(class CItem * pItem, int iItemID);
	BOOL bReadProgramConfigFile(char * cFn);
	void GameProcess();
	void InitPlayerData(int iClientH, char * pData, DWORD dwSize);
	void NotifyCInsert(int client);
	void NotifyCDelete(int client);
	//agregado lalov9
	BOOL bCheckClientStatLvl(int iClientH);

	// Deteccion de intentos de tirar el servidor (consola + ServerLogs/Ataques)
	void ReportarAtaque(int iClientH, char * cMotivo);
	void RegistrarIPPersonaje(int iClientH);
	void ResponsePlayerDataHandler(char * pData, DWORD dwSize);

	BOOL bSendMsgToLS(DWORD dwMsg, int iClientH, BOOL bFlag = TRUE,char * pData = NULL );
	void OnMainLogRead();
	void CheckClientResponseTime();
	
	void RemoveOccupyFlags(int iMapIndex);
	int iComposeMoveMapData(short sX, short sY, int iClientH, char cDir, char * pData);
	void SendEventToNearClient_TypeB(DWORD dwMsgID, WORD wMsgType, char cMapIndex, short sX, short sY, short sV1 = 0, short sV2 = 0, short sV3 = 0, short sV4 = 0);
	void SendEventToNearClient_TypeA(short sOwnerH, char cOwnerType, DWORD dwMsgID, WORD wMsgType, short sV1, short sV2, short sV3);
	void DeleteClient(int iClientH, BOOL bSave, BOOL bNotify, BOOL bCountLogout = TRUE, BOOL bForceCloseConn = FALSE);
	int  iComposeInitMapData(short sX, short sY, int iClientH, char * pData);
//	bool WriteTileData(char * buffer, int & iSize, int iClientH, CTile * srcTile, int ix, int iy);
	void RequestInitDataHandler(int iClientH, char * pData, char cKey, BOOL bIsNoNameCheck = FALSE);
	void RequestInitPlayerHandler(int iClientH, char * pData, char cKey);
	int  iClientMotion_Move_Handler(int iClientH, short sX, short sY, char cDir, BOOL bIsRun, BOOL BIsDamage = FALSE);
	
	//Nuevos Codigos ChatGPT
	bool isValidClient(int iClientH, short sX, short sY, char cDir);
	void updateClientActivity(int iClientH);
	void getNewCoordinates(short sX, short sY, char cDir, short& dX, short& dY);
	BOOL canMoveToLocation(int iClientH, short dX, short dY);
	void handleValidMove(int iClientH, short dX, short dY, char cDir, BOOL bIsRun, BOOL BIsDamage);
	void handleSpikeDamage(int iClientH);
	void sendMoveConfirmMessage(int iClientH, short dX, short dY, char cDir, BOOL bIsRun, BOOL BIsDamage);
	void handleRunStamina(int iClientH, BOOL bIsRun, char*& cp);
	void ClientMotionHandler(int iClientH, char* pData);


	void OnClientRead(int iClientH);

	void GetFightzoneTicketHandler(int iClientH);

	void FightzoneReserveProcessor() ;

	void AutoSkill(int iClientH);

	void ReqCreateSlateHandler(int iClientH, char* pData);
	void SetSlateFlag(int iClientH, short sType, bool bFlag);

	char m_cServerName[12];
	

	char m_cLogServerAddr[16];
	int  m_iLogServerPort;
	int  m_iWorldLogServerPort;

	int  m_iLimitedUserExp, m_iLevelExp20;

	BOOL _bDecodeItemConfigFileContents(char * cFn);
	int _iComposePlayerDataFileContents(int iClientH, char * pData);
	BOOL _bDecodePlayerDatafileContents(int iClientH, char * pData, DWORD dwSize);
	BOOL _bDecodePlayerDatafileContents2(int iClientH, char * pData, DWORD dwSize);
	bool BlockedTeleport(int client);
	void AdminCommand_Event(int client, char *pData, DWORD dwMsgSize);
	void handleCheckTimer(int client);
	void AdminOrder_UnLimbo(int client, char* pData, DWORD dwMsgSize);
	void EnableBot(int client);
	void DisableBot(int client);
	void SetLimboAfkFlag(int iClientH, int iStatus);
	void RequestMarketNames(int client);
	void RequestMarketItems(int client, char* pData);
	void handleMarketLog(char* seller, char* buyer, char* itemname, int coinval, DWORD attr);
	void RequestGetMarketItem(int client, char* pData);
	void ReqRepairItemCofirmHandler2(int client, char cItemID, char* pString);
	void RequestSetMarketItem(int client, char* pData);
	void SaveCharacterMarket(int client);
	void bDecodePlayerMarketItemContents(int client, char* pData, DWORD dwSize);
	void ReadMarket(int client);
	BOOL revertitemWeight(class CItem* pItem, char* pItemName);
	void handleNewReadModes(char*& token, char& cReadModeA);
	BOOL _bRegisterMap(char * pName);
	bool getitemname(int client, int cx, int cy);
	void getCriticals(int client);
	void SendMessageToClientUDP(int client);
	void sendMessageToClient(int client, int msgid);
	class CClient * m_pClientList[DEF_MAXCLIENTS];
	class CNpc    * m_pNpcList[DEF_MAXNPCS];
	class CMap    * m_pMapList[DEF_MAXMAPS];
	class CDynamicObject * m_pDynamicObjectList[DEF_MAXDYNAMICOBJECTS];
	class CDelayEvent    * m_pDelayEventList[DEF_MAXDELAYEVENTS];

	class CMsg    * m_pMsgQuene[DEF_MSGQUENESIZE];
	int             m_iQueneHead, m_iQueneTail;
	int             m_iTotalMaps;
	class XSocket * m_pMainLogSock, * m_pRAdminSock;
	class CMisc     m_Misc;
	BOOL			m_bIsGameStarted;
	BOOL            m_bIsLogSockAvailable, m_bIsWLServerAvailable;
	BOOL 			m_bIsOnlyClientAvailable;
	class CItem   * m_pItemConfigList[DEF_MAXITEMTYPES];
	class CNpc    * m_pNpcConfigList[DEF_MAXNPCTYPES];
	class CMagic  * m_pMagicConfigList[DEF_MAXMAGICTYPE];
	class CSkill  * m_pSkillConfigList[DEF_MAXSKILLTYPE];
	class CQuest  * m_pQuestConfigList[DEF_MAXQUESTTYPE];
	char            m_pMsgBuffer[DEF_MSGBUFFERSIZE+1];


	HWND  m_hWnd;
	int   m_iTotalClients, m_iMaxClients, m_iTotalGameServerClients, m_iTotalGameServerMaxClients;
	SYSTEMTIME m_MaxUserSysTime;

	BOOL  m_bF1pressed, m_bF4pressed, m_bF12pressed;
	BOOL  m_bOnExitProcess;
	DWORD m_dwExitProcessTime;

	DWORD m_dwWhetherTime, m_dwGameTime1, m_dwGameTime2, m_dwGameTime3, m_dwGameTime4, m_dwGameTime5, m_dwGameTime6, m_dwGameTime9, m_dwAutoSaveTime, m_dwGameTime8, m_dwFishTime, m_dwFakeTime, m_dwExitTime;
	//v13
	DWORD m_dwNewTime;
	bool m_bShutDownServer;
	bool m_bExitServer;
	BOOL  m_cDayOrNight;
 	int   m_iSkillSSNpoint[102];

	DWORD m_dwSpecialEventTime;
	BOOL  m_bIsSpecialEventTime;
	char  m_cSpecialEventType;

	int   m_iLevelExpTable[300];
 	class CFish * m_pFish[DEF_MAXFISHS];
	class CPortion * m_pPortionConfigList[DEF_MAXPORTIONTYPES];

	BOOL  m_bIsServerShutdowned;
	char  m_cShutDownCode;
	class CMineral * m_pMineral[DEF_MAXMINERALS];
    int   m_iWhouseMapIndex;
	int   m_iMiddlelandMapIndex;
	int   m_iAresdenMapIndex;
	int	  m_iElvineMapIndex;
	int   m_iAresdenOccupyTiles;
	int   m_iElvineOccupyTiles;
	int   m_iCurMsgs, m_iMaxMsgs;

	DWORD m_dwCanFightzoneReserveTime;
	int  m_iFightZoneReserve[DEF_MAXFIGHTZONE];
	int  m_iFightzoneNoForceRecall;

	struct {
		__int64 iFunds;
		__int64 iCrimes;
		__int64 iWins;

	} m_stCityStatus[3];
	
	int	  m_iStrategicStatus;
	
	class XSocket * m_pSubLogSock[DEF_MAXSUBLOGSOCK];
	int   m_iSubLogSockInitIndex;
	BOOL  m_bIsSubLogSockAvailable[DEF_MAXSUBLOGSOCK];
	int	  m_iCurSubLogSockIndex;
	int   m_iSubLogSockFailCount;
	int   m_iSubLogSockActiveCount;
	int   m_iAutoRebootingCount;

	class CBuildItem * m_pBuildItemList[DEF_MAXBUILDITEMS];
	class CItem * m_pDupItemIDList[DEF_MAXDUPITEMID];

	char * m_pNoticementData;
	DWORD  m_dwNoticementDataSize;

	DWORD  m_dwMapSectorInfoTime;
	int    m_iMapSectorInfoUpdateCount;

	int	   m_iCrusadeCount;
	BOOL   m_bIsCrusadeMode;

	struct {
		char cMapName[11];
		char cType;	
		int  dX, dY;

	} m_stCrusadeStructures[DEF_MAXCRUSADESTRUCTURES];

	BOOL	m_bIsApocalypseMode;//laloapoca

	int m_iCollectedMana[3];
	int m_iAresdenMana, m_iElvineMana;

	class CTeleportLoc m_pGuildTeleportLoc[DEF_MAXGUILDS];
	//

	WORD  m_wServerID_GSS;
	int   m_iIndexGSS;

	struct {
		int iCrashedStructureNum;
		int iStructureDamageAmount;
		int iCasualties;
	} m_stMeteorStrikeResult;

	struct {
		char cType;
		char cSide;
		short sX, sY;
	} m_stMiddleCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
	int m_iTotalMiddleCrusadeStructures;
 
	int m_iClientShortCut[DEF_MAXCLIENTS+1];

	int m_iNpcConstructionPoint[DEF_MAXNPCTYPES];
	DWORD m_dwCrusadeGUID;
	int   m_iCrusadeWinnerSide;   
	int	  m_iWinnerSide;
	int	  m_iNonAttackArea;

	int   m_iWorldMaxUser;

	short m_sForceRecallTime;

	BOOL  m_bIsCrusadeWarStarter;
	int   m_iFinalShutdownCount;

	struct {
		int iDay;
		int iHour;
		int iMinute;
	} m_stCrusadeWarSchedule[DEF_MAXSCHEDULE];

	class CItem * m_pGold;

	bool	m_bReceivedItemList;
	
	char m_cMsg[25][101];
	BOOL m_bAnunciosActivo;
	BOOL EventAuto;

	int __iSearchForQuest(int iClientH, int iWho, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	int _iTalkToNpcResult_Cityhall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, int * pX, int * pY, int * pRange);
	void _ClearExchangeStatus(int iClientH);
	int _iGetItemSpaceLeft(int iClientH);
	void AdminOrder_SummonGuild(int iClientH, char *pData, DWORD dwMsgSize);
	void AdminOrder_Assasain(int iClientH, char *pData, DWORD dwMsgSize);
	void ManualStartHappyHour(int iClientH, char *pData, DWORD dwMsgSize);
	void ManualStartHappyDay(int iClientH, char *pData, DWORD dwMsgSize);

	DWORD m_dwCleanMap;
	int HeldenianType2;

	struct {
		char cMapName[11];			
		char cType;					
		int  dX, dY;				

	} m_stRushStructures[MAX_RUSHSTRUCTURES];

	void GoldPorLevel(int iClientH);

	//New Command Portal - ZeroEoyPnk
	void AdminOrder_Portal(int iClientH, char * pData, DWORD dwMsgSize);
	void ClosePortal(int PortalNum);
	int dest_dx_portal[DEF_MAXPORTALS], ini_dx_portal[DEF_MAXPORTALS];
	int dest_dy_portal[DEF_MAXPORTALS], ini_dy_portal[DEF_MAXPORTALS];
	int LevelLimit_portal[DEF_MAXPORTALS], MaxPlayer_portal[DEF_MAXPORTALS], PortalPlayers[DEF_MAXPORTALS];
	DWORD dw_TimeOpenPortal[DEF_MAXPORTALS], dw_TimePortal[DEF_MAXPORTALS];
	char Portal_MapName[DEF_MAXPORTALS][256], Portal_Init[DEF_MAXPORTALS][11];
	BOOL PortalAbility[DEF_MAXPORTALS];
	
	int LastPortal;

	struct {
		char cMapName[11];			
		char cType;					
		int  dX, dY;				

	} m_stDefenceStructures[MAX_DEFENCESTRUCTURES];

	struct {
		char cMapName[11];			
		char cType;					
		int  dX, dY;				

	} m_stDefenceStructuresx[MAX_DEFENCESTRUCTURES];


	//Trade TOTAL hbnet 22/12
	void GetMajesticsItemsHandler(int iClientH,int iItemID,char * pString);
	void ZerkMe(int iClientH);
	void ManaMe(int iClientH);
	//Rep command
	void PlayerOrder_ChangeCity(int iClientH);

	BOOL VerificarItem(int iItemID);

	void NetBoxLotery(int iClientH, int ItemType);
	void HandleReward(int iClientH, int ItemType, int level);
	void DistributeReward(int iClientH, int ItemType, const std::vector<int>& probabilities);
	void ApplyReward(int iClientH, int ItemType, int rewardType);
	void Resultado1(int iClientH, int ItemType);
	void Resultado2(int iClientH, int ItemType);
	void Resultado3(int iClientH, int ItemType);
	void Resultado4(int iClientH, int ItemType);
	void Resultado5(int iClientH, int ItemType);

	BOOL Actualizar;

	int m_iPlayerMaxLevel;
	void _ArmarQuestContens(int iClientH, int iIndex);

	void PlayerReceivedCoords(int iClientH, int CoordX, int CoordY);

//	void DeleteItems(); // Test ZeroEoyPnk
	void DefineWHPage(int iClientH, short iV1);

	//NewCityVsCity
	void StartCityVsCity();
	void EndCityVsCity();
	void CloseEventMap();
	short ContadorElvine, ContadorAresden, ContadorAssesino;
	BOOL CityVsCity, PriceEvent;
	char SideWinner;
	DWORD cvcEventTime, EventTime, PriceTime;
	DWORD AllEksTime;
	DWORD AllDkEksTime;
	short LastContElv, LastContAres;

	//WorldWar LaloRamos
	void StartWorldWar();
	void EndWorldWar();
	void CloseWWEventMap();
	BOOL WorldWar, WWPriceEvent;
	void ActualizarWWContador();
	short WWContadorElvine, WWContadorAresden, WWContadorAssesino;
	char WWSideWinner;
	DWORD WorldWarTime, WWPriceTime;
	short WWLastContElv, WWLastContAres;

	//WorldWar II
	void StartWorldWarII();
	void EndWorldWarII();
	void CloseWWEventMapII();
	BOOL WorldWarII, WWIIPriceEvent;
	void ActualizarWWIIContador();
	short WWIIContadorElvine, WWIIContadorAresden, WWIIContadorAssesino;
	DWORD WorldWarTimeII, WWIIPriceTime;
	char WWIISideWinner;
	short WWIILastContElv, WWIILastContAres;


	bool m_bShinning;
	 
	BOOL bReadCVCStructureConfigFile(char * cFn);
	void CreateCVCStructures();
	void AbrirPortales(short Event);
	void AllClosePortals();
	void ActualizarContador();
//	void GlobalCvcCount(char CountType);

	struct {
		char cMapName[11];			
		char cType;					
		int  dX, dY;				

	} m_stCVCStructures[MAX_CVCSTRUCTURES];

	void DeleteCliente(int iClientH, char *pData, DWORD dwMsgSize);

	void CallMercenary(short Map);

	int m_iAFKUsers;
	int m_bAFKLimbo;

	//New data send - ZeroEoyPnk
	void SendPlayerData(int iClientH);
	void LoadPlayerItems(int iClientH);
	void SendRestPlayerData(int iClientH);
	void SendLasData(int iClientH);
	void SendEnemyKills(int iClientH);
	void SendContribution(int iClientH);
	void SendCriticals(int iClientH);
	void SendEventOpenData(int iClientH);

	//Crash Lalo
	short sTotalConnection;

	//Reemplazado por una funcion, asi reduzco codigo.. y anda mejor :3
	void HammerStrip(int sAttackerH, char cAttackerType, int sTargetH, int iHitPoint);

	// Summon Event 
	void LaunchSummonEvent();
	void CreateMSStructures();
	void SummonAresden();
	void SummonElvine();
	void SummonAssasain();
	void EndSummonEvent();
	void NextRound(short Side, short Type, short Total);
//	void GlobalNextRound(short Side, short Type, short Total);

	BOOL bReadSummonStructureConfigFile(char * cFn);
	BOOL bReadRoundSummonConfigFile(char * cFn, short CFG);

	BOOL RunSummonEvent, RunSummonEvent2, RunSummonEvent3, SummonLaunched;
	char MSTotal, TotalRounds;
	char AresdenRound, ElvineRound, AssasainRound;
	DWORD AresdenSummon, ElvineSummon, AssasainSummon, SummonEventTime;
	short SelectedRounds;

	void ReloadSummonEventConfig();

	struct {
		char cMapName[11];			
		char cType;					
		int  dX, dY;				

	} m_stSummonStructures[MAX_DEFENCESTRUCTURESA];

	struct {
		short Cant;	
		char cNpcName[21]; // VERGA!!!!			
	} SummonRound[DEF_MAXCRUSADESTRUCTURES];

	void SetFuryWarFlag(short sOwnerH, char cOwnerType, BOOL bStatus);

	//Assasain
	void ChangeToAssasain(int iClientH);
	void ApplyPkPoint(int iAttackerH, int iClientH, BOOL Disc = FALSE);
	void GetAssasainHero(int iClientH, int iV1);
	void ResetAssasain(int iClientH, int iV1);
	void ChangePkToEk(int iClientH, int iV1);
	void ChangeLocation(int iClientH, DWORD wResult);

	//Base defense event
	BOOL bReadHellOfDeathConfigFile(char * cFn);
	void CreateHellOfDeathStructures();

	struct {
		char cMapName[11];			
		char cType;					
		int  dX, dY;				

	} m_stHellOfDeathStructures[MAX_DEFENCESTRUCTURES];

	BOOL HellOfDeath, PriceHODEvent;
	DWORD dw_HODTime;
	DWORD dw_PriceHODTime;
	
	void InitHellOfDeath();
	void RemoveHODStructures();
	void EndHellOfDeathMode(int Loser);
	void ActualizarStructures();
	void CloseHODMap();

	void SearchAssasainPosition(int iClientH);

	BOOL VipMap;
	BOOL AllEks;
	BOOL AllDkEks;

//	void WrongClient(int iClient);

	struct {
		char m_cName[21];
		int m_wPrice;
		BOOL unique;
	} ItemsTrade[2000];
	BOOL ReadTradeItems(char * cFn);
	void RequestTradeItemHandler(int iClientH, char * pItemName, int iNum, short Type);
	BOOL TradeID[4];

	void RequestRemoveItemExchange(int iClientH, int iItemIndex);
	int CalcWeightExchange(int iClientH, int iExH);
	BOOL CalcTotalBagExchange(int iClientH, int iExH);

	void CheckItemStatedEquipment(int iClientH);

	BOOL Masacre;
	
	void CheckCompleteAchivement(int iClientH, short AchivementID, BOOL Check = FALSE, BOOL New = TRUE);
	void CompleteAchivementList(int iClientH);

	void ClientFakeMoveTimmer();
	void ClientFakeMove(int iClientH);

	void SearchStatusFriend(int iClientH, short Position, char CharName[10]);
	void RequestGuildMembers(int iClientH);

	void SendF5DataCharacter(int iClientH);
	int Random(int Minimo, int Maximo);

	void CheckProcess(int iClientH);
	void DataProcessReceived(int iClientH, char * pData, DWORD dwMsgSize);
	void CheckProcessClients();
	void CheckHability(int iClientH, char *pData, DWORD dwMsgSize);
	DWORD m_dwCheckProcess;

	BOOL ReadProcessAndDllBlockedFile(char * cFn, BOOL Type);
	void SendProcessBlocked(int iClientH);
	void SendDllsBlocked(int iClientH);
	char Process[100][50];
	char Dlls[100][50];
	void UpdateProcessClient();

	void ChangeHeroSide(int iClientH);
	void DeleteWhItem(int iClientH, short sItemIndex);
	BOOL SendBankItemData(int iClientH, class CItem * pItem, short Position, BOOL Actualizar = FALSE);

	void CheckHeroData(int iClientH, short ItemIndex, BOOL WH);

	BOOL _bReadItemsEventCFG(char * cFn);
	char ItemsEvent[100][21];
	void InitItemsClient(int iClientH);
	void AddCandys(int iClientH);
	void RestoreItems(int iClientH);

	void InitArenaEvent();
	void CloseArenaEvent();
	BOOL FinalFight;
	struct {
		char CharName[11];
		short Nivel;
		short Kills;
		short Deaths;
	} PvpContador[10];
	void ArmarEventTop();
	void CheckNewNivel(int iClientH, BOOL Init);
	void NivelObteined(int iClientH);

	void Event_FinalFight();
	void Close_FinalFight();
	short FinalFight_ID;

	void CreateFinalFightID();
	BOOL bReadFinalFightConfigFile(char * cFn);


	BOOL ReadQuestItemsCFG(char * cFn);

	void Event_EK40();
	void EndEvent_EK40();

	void Event_EK100();
	void EndEvent_EK100();

	void Event_Masacre();
	void EndEvent_Masacre();

	void Event_TowerHell();
	void EndEvent_TowerHell();

	void Event_HuntMap();
	void EndEvent_HuntMap();

	void Event_Questx2();
	void EndEvent_Questx2();
	void Event_Questx3();
	void EndEvent_Questx3();

	void EventAllEks();
	void EventAllDkEks();

	void EventDM();

	BOOL DKEvent;

	BOOL TowerHell;
	BOOL Questx2;
	BOOL Questx3;

	void RequestLogOutHandler(int iClientH);
	void RequestCancelLogOutHandler(int iClientH);

	//int MaxClientID;

	void CheckUsersIP (int iClientH, char *pData, DWORD dwMsgSize);
	void AdminOrder_BanIP(int iClientH, char *pData, DWORD dwMsgSize);
	void LoadBannedList(int iClientH);

	char BannedIP[100][21];

	short TotalAnnoucement;

//	void ReqPing(int iClientH, int pData);

	// Events Wins
	struct {
		short AresdenWin;
		short ElvineWin;
		short Draw;
		short LastWin;
	} EventCount[10];

	BOOL bReadEventResults(char * cFn, short Event);
	void SendEventData(int iClientH);
	DWORD dw_CrusadeTime, dw_TowerTime, dw_CaptureTime, dw_RushTime, dw_DmTime;

	void CreateEventResults(int iWinnerSide, short Event);

	void LocalStarHappyHour(BOOL StatusHappy);
	void LocalStarHappyDay(BOOL StatusHappy);

	// Adds Points Admin
	void AdminOrder_SetPoints (int iClientH, char *pData, DWORD dwMsgSize, short Type);

	// New System -> Market Place -> HB Arg v4.4
	void bSetItemToSellList(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestItemsList(int iClientH, char * pData, DWORD dwMsgSize);
	void SendDataListItems(int SendTo, int iClientH, short Position, short TotalSell);
	void RequestDefineSellList(int iClientH, char * pData, DWORD dwMsgSize);
	void RequestBuyItem(int iClientH, char * pData, DWORD dwMsgSize);
	void ActualizarSellingList(int BuyTo, int iClientH);
	
	// Summon Balls -> ZeroEoyPnk
/*	void SearchMapToSummon(short SummonBoss, short iClientH);
	void RequestCreateSummonBallNPC(short SummonBoss, short iClientH);
	void CreateSummonBallNPC(short SummonBoss, short ServerID, short iClientH, char * Guild);*/

	//new objects data (all clients)
	struct st_objects
	{
		st_objects() : client(0), m_sSide(0), m_sRankLevel(0), iLevel(0), iRebirthLevel(0) {}
		int client;
		short m_sSide;
		short m_sRankLevel;
		short iLevel;
		short iRebirthLevel;
	};

	vector<st_objects> vObjects;

	struct {
		DWORD dwOpenSummonMap;
		bool ActiveSummon;
		char GuildName[22];
	} SummonBalls[10];

	BOOL ReadMapCFGs(char * cFn);
	char MapList[DEF_MAXMAPS][12];

	struct {
		char Name[11];
		Citizenship Side;
		int Points;
	} Top[6][15];

	void HandleRequestMemoryTops(int iClientH, short Top, BOOL Send = TRUE, BOOL Actualizar = TRUE);

	void EkAnnoucer(char cAttackerName[12], char cVictimun[12], BOOL EK);
	void SendEkAnnoucer(char cEKMsg[30], BOOL EK);

//	void GlobalSendHPStructures(short Type);
	void SendHPStructures(short Type);
	short ReliquiaAresden, ReliquiaElvine;

//	void GlobalStartTpEvent(short Event);
//	void GlobalEndTpEvent(short Event);

	void RequestBuyAssasainPoints(int iClientH, char * pData, DWORD dwMsgSize);

	class PartyManager * m_pPartyManager[DEF_MAXPARTYNUM];

	void RefreshParty(int iClientH, short Type);

	void ObteinStarterKit(int iClientH);
	void RemoveItemUniqueID(int iClientH, short itempos, short ItemUse);
	//Nuevo Change Master ID
	void RemoveItemMasterID(int iClientH, short itempos, short ItemUse);
	//Sacar ID
	void RemoveTotalID(int iClientH, short itempos, short ItemUse);

	int RandomEventID;
	bool MarketEnabled;
};

#endif // !defined(AFX_GAME_H__C3D29FC5_755B_11D2_A8E6_00001C7030A6__INCLUDED_)
