// Game.h: interface for the CGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAME_H__0089D9E3_74E6_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_GAME_H__0089D9E3_74E6_11D2_A8E6_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <iostream>
#include <map>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mmsystem.h>
#include <winbase.h>
#include <memory.h>
#include <string.h>
#include <tlhelp32.h>
#include <process.h>
#include <direct.h>
#include <chrono>
#include <vector>

#include "../Motor/DXC_ddraw.h"
#include "../Motor/DXC_dinput.h"
#include "../Library/YWSound.h"
#include "../Headers/SoundBuffer.h"
#include "../Headers/XSocket.h"
#include "../Headers/Sprite.h"
#include "../Headers/Misc.h"
#include "../Headers/StrTok.h"
#include "../Headers/Msg.h"
#include "../Headers/Effect.h"
#include "../Headers/MapData.h"
#include "../Headers/MouseInterface.h"
#include "../Headers/CharInfo.h"
#include "../Headers/Item.h"
#include "../Headers/Magic.h"
#include "../Headers/Skill.h"
#include "../Headers/GameMonitor.h"
#include "../Headers/BuildItem.h"
#include "../Headers/ItemName.h"
#include "../Headers/Curse.h"
#include "XSocketUDP.h"
#ifdef DEF_USING_WIN_IME
	#include <RICHEDIT.H>
#endif

//Snoopy: Implementation JPG sreenshots
#include "../cximage/ximage.h" // Snoopy
#include "../cximage/ximajpg.h" // Snoopy


#include "../General/ActionID.h"
#include "../General/DynamicObjectID.h"
#include "../General/GlobalDef.h"
#include "../General/NPC_Config.h"
#include "../General/SpriteID.h"

/*#include "../General/NucleoMessage.h"*/
#include "../../Shared/netmsg.h"

#include "../Headers/Event.h"

#define DEF_MAXRANKLEVEL 30   // tope de rango: debe coincidir con Rank.h del servidor

extern Event g_ev;


#define C800x600 1
#define C640x480 2
#define C1024x768 3

#define DEF_OWNERTYPE_PLAYER			1
#define DEF_OWNERTYPE_NPC				2
#define DEF_OWNERTYPE_PLAYER_INDIRECT	3


//v2.18
#define DEF_BTNSZX				74
#define DEF_BTNSZY				20
#define DEF_LBTNPOSX			30
#define DEF_RBTNPOSX			154
#define DEF_BTNPOSY				292

#define DEF_INDEX4_MSGID		0
#define DEF_INDEX2_MSGTYPE		4

#define DEF_SOCKETBLOCKLIMIT	300

//HeatoN modified
#define DEF_MAXSPRITES			27500 //25000HBARG DEFAULT // 20000 // Snoopy: Adjusted!
#define DEF_MAXTILES			1000//1000	// 800 // Snoopy: Adjusted!
#define DEF_MAXEFFECTSPR		300
#define DEF_MAXSOUNDEFFECTS		200		// 110   //Snoopy: Adjusted!
#define DEF_MAXCHATMSGS			500
#define DEF_MAXWHISPERMSG		5
#define DEF_MAXCHATSCROLLMSGS	80
#define DEF_MAXEFFECTS			300//300	//600 <- original

#define DEF_CHATTIMEOUT_BA		2500
#define DEF_CHATTIMEOUT_CA		2500

#define DEF_CHATTIMEOUT_A		4000
#define DEF_CHATTIMEOUT_B		500
#define DEF_CHATTIMEOUT_C		2000
#define DEF_MAXITEMS			50
#define DEF_MAXBANKITEMS		201 // WH Page - ZeroEoyPk
#define DEF_MAXGUILDSMAN		32
#define DEF_MAXMENUITEMS		300 //v2.15  120 ->140 ->300
#define DEF_TEXTDLGMAXLINES		300 //v2.18 3000->300

#define DEF_MAXMAGICTYPE		100
#define DEF_MAXSKILLTYPE		60
#define DEF_MAXWHETHEROBJECTS	600
#define DEF_MAXBUILDITEMS		150
#define DEF_MAXGAMEMSGS			300
#define DEF_MAXITEMNAMES		1200
#define DEF_MAXGUILDNAMES		100
#define DEF_MAXSELLLIST			12
#define DEF_MAXSELLITEMSLIST	19
#define DEF_MAXACHIVEMENTS		30

#define BUFFER_SIZE 200

#define WM_USER_GAMESOCKETEVENT	WM_USER + 2000
#define WM_USER_LOGSOCKETEVENT	WM_USER + 2001

#define DEF_GAMEMODE_NULL					-2
#define DEF_GAMEMODE_ONQUIT					-1
#define DEF_GAMEMODE_ONMAINMENU				0
#define DEF_GAMEMODE_ONCONNECTING			1
#define DEF_GAMEMODE_ONLOADING				2
#define DEF_GAMEMODE_ONWAITINGINITDATA		3
#define DEF_GAMEMODE_ONMAINGAME				4
#define DEF_GAMEMODE_ONCONNECTIONLOST		5
#define DEF_GAMEMODE_ONMSG					6
#define DEF_GAMEMODE_ONCREATENEWACCOUNT		7
#define DEF_GAMEMODE_ONLOGIN				8
#define DEF_GAMEMODE_ONQUERYFORCELOGIN		9
#define DEF_GAMEMODE_ONSELECTCHARACTER		10
#define DEF_GAMEMODE_ONCREATENEWCHARACTER	11
#define DEF_GAMEMODE_ONWAITINGRESPONSE		12
#define DEF_GAMEMODE_ONQUERYDELETECHARACTER 13
#define DEF_GAMEMODE_ONLOGRESMSG			14
#define DEF_GAMEMODE_ONCHANGEPASSWORD		15
#define DEF_GAMEMODE_ONVERSIONNOTMATCH		17
#define DEF_GAMEMODE_ONINTRODUCTION			18
#define DEF_GAMEMODE_ONAGREEMENT			19
#define DEF_GAMEMODE_ONSELECTSERVER			20
#define DEF_GAMEMODE_ONINPUTKEYCODE			21

#define DEF_SERVERTYPE_GAME			1
#define DEF_SERVERTYPE_LOG			2

#define DEF_CURSORSTATUS_NULL		0
#define DEF_CURSORSTATUS_PRESSED	1
#define DEF_CURSORSTATUS_SELECTED	2
#define DEF_CURSORSTATUS_DRAGGING	3

#define DEF_SELECTEDOBJTYPE_DLGBOX	1
#define DEF_SELECTEDOBJTYPE_ITEM	2
#define	DEF_SELECTEDOBJTYPE_MAGIC	3

#define DEF_DOUBLECLICKTIME			300
#define DEF_MAXPARTYMEMBERS			15
#define DEF_MAXCRUSADESTRUCTURES	300

// ZeroEoyPnk - WH Page
#define BANK_ITEMS_PAGE			21

#define DEF_MAXPORTALS			100

#define DEF_MAXDIALOGBOX		100

#define	DEF_MAXITEMEXCHANGED	40
#define DEF_MAXLISTNAMES		19

#define DEF_ASSASAINPOINTS				5000
#define DEF_PURCHASE_ASSASAINPOINTS		5000
#define DEF_AP_EKNEED					150000
#define DEF_AP_CONTRIBNEED				0
#define DEF_AP_COINSNEED				0
#define DEF_AP_EXPRANKNEED				0


enum TradeType
{
	DEF_TRADEEK,
	DEF_TRADECONTRIB,
	DEF_TRADECOIN,
	DEF_TRADEMAJS,
	DEF_TRADEBALLS,
	DEF_NOTSELECTED
};

enum Rankings
{
	DEF_TOPNONE,
	DEF_TOPDM,
};

typedef unsigned long uint32;

static void memcpy_secure(void * cIn, const void * cOut, size_t n) {
	if ((cIn == NULL) || (cOut == NULL) || (n <= 0) || IsBadReadPtr(cIn, n) || IsBadReadPtr(cOut, n)) return;
	memcpy(cIn, cOut, n);
}

static char * strcpy_secure(char * s1, const char * s2) {
	memcpy_secure(s1, s2, strlen(s2));
	return s1;
}

class CGame
{
public:
	CGame();
	virtual ~CGame();


	//floor items start
	struct FItem
	{
		FItem()
		{
			ptr = nullptr;
		}
		void Clear()
		{
			tm = 0;
			spr = 0;
			frame = 0;
		}
		uint32 tm;
		CItem * ptr;
		short spr, frame;
	} f_item;
	
	void n_flooitem(char * data);//x pdata = data
	void getrankexp(char * data);
	void ranklevelup(char * data);
	void GetObjectsData(char* data);
	void getrankstats(char* data);
	void getcinsertstatus(char* data);
	void getcdeletestatus(char* data);
	void get_object_levels(int client, int &level, int &rebirthlevel);
	short get_object_rank(int client);
	short get_object_side(int client);
	void DrawDialogBox_RankLevelUpSetting(short msX, short msY);
	void DlgBoxClick_RankLevelUpSettings(short msX, short msY);
	void Drawflooritem(CItem* item, int x = 0, int y = 0, int mx = 0, int my = 0);
	void DrawMarketItem(CItem* item, int x = 0, int y = 0, int mx = 0, int my = 0);

	short m_sRankAddStr;
	short m_sRankAddVit;
	short m_sRankAddInt;
	short m_sRankAddDex;
	short m_sRankAddMag;

	//bool IsCommonItem(char * itemname);
	//bool IsRareItem(char * itemname);
	
	//floor items end
	bool reqfree;
	bool b800x600;
	bool b640x480;
	bool b1024x768;

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

	//rank system hbarg
	int m_iRankExp;
	short m_sRankLevel;
	int m_iRankPoints;

	int m_iMaxRankExp;

	bool m_bCandyInsert;
	bool m_bCandyDelete;
	short m_sTempRankPoints;
	short m_sTmpRankAddStr;
	short m_sTmpRankAddInt;
	short m_sTmpRankAddDex;
	short m_sTmpRankAddMag;

	//HeatoN rebirth

	int m_iRebirthLevel;
	int m_sRebirthStatus;
	short m_sRebirthEnabled;
	int m_iBackRebirthLevel;

	struct st_fighters
	{
		st_fighters() {}
		char fighter[12];
		int kills;
		int deaths;
	};

	vector<st_fighters> vec_fighters;

	void * operator new (size_t size)
	{
		return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
	};

	void operator delete(void * mem)
	{
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, mem);
	};	

	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point endTime;

	void _CalcSocketClosed();
	void UpdateScreen_OnLoading(bool bActive);
	void UpdateScreen();
	void RequestFullObjectData(WORD wObjectID);
	void PlaySound(char cType, int iNum, int iDist, long lPan = 0);
	void OnTimer();
	void OnSysKeyUp(WPARAM wParam);
	void OnSysKeyDown(WPARAM wParam);
	void OnLogSocketEvent(WPARAM wParam, LPARAM lParam);
	void OnKeyUp(WPARAM wParam);
	void request_ranking(short mode);
	void click_top10(short msX, short msY);
	void draw_top10(short msX, short msY);
	void getNewItemWeight(char* data);
	void getNewItemEndurance(char* data);
	void get_top_deathmatch(char* data);
	void DrawCords(short x, short y);
	BOOL m_windowsKey;

	CItem* market_item;
	int market_item_index;
	DWORD market_coins;
	std::string market_name;
	struct MarketItem
	{
		int index;
		CItem item;
		int coins;
	};
	MarketItem* market_item_ptr;
	std::vector<MarketItem> market_items;
	std::vector<std::string> market_names;

	//char m_cLogOutCount;
	//Cambio LogOutLaloRamos

	//heaton dm points
	int m_iDMKills;
	int m_iMaxDMKills;
	int m_iDMDeaths;
	
	bool reqhp;
	int m_iMobHP;
	int m_iMobMaxHP;
	bool showbar;
	bool DontShowOwner(short sOwnerType);
	char templog[100];
	bool m_bShinning;
	void shinning_status_onlyclient(char * data);
	void shinning_update(char * cp);
	void shinning_clear(char * cp);
	int getRebirthStats();
	void get_dm_points(char * data);
	void mouselog(char * log);
	void MarketNames(char* data);
	void MarketItems(char* data);
	void DlgBoxClick_Market(short msX, short msY);
	void draw_questlist(short msX, short msY, short msZ, char cLB);
	void click_questlist(short msX, short msY);
	void draw_charquests(short msX, short msY, short msZ, char cLB);
	void click_charquests(short msX, short msY);
	void Draw_NetGraph();
	void DrawQuest();
	void DrawMenu_Box(short sX, short sY, short x, short y);
	void get_questlist(char * data);
	void get_charquests(char * data);
	void get_charquests_byindex(char * data);
	void draw_trades(short msX, short msY, short msZ, char cLB);
	void click_trades(short msX, short msY);
	void DrawShadowMenu_Borders2(short sX, short sY, short szX, short szY);
	void DrawShadowPixels2(int mode, int count, short x, short y);
	void get_trade_ek(char * data);
	void get_trade_contrib(char * data);
	void get_trade_coin(char * data);
	void get_trade_majs(char * data);
	void DrawShadowPixels(int mode, int count, short x, short y);
	void DrawMenu_TextBox(short sX, short sY, short x, short y);
	void draw_confirm(short msX, short msY, char cLB);
	void click_confirm(short msX, short msY);
	bool isValidAmount(const std::string& str);
	double calculatePing();
	void DrawDialogBox_Market(short msX, short msY, short msZ, char cLB);
	//heaton market
	bool bItemDrop_Market();
	void DrawItemInfo(CItem* item, int x = 0, int y = 0, int mx = 0, int my = 0);
	void draw_trade_contrib(short msX, short msY, short msZ, char cLB);
	void click_trade_contrib(short msX, short msY);
	void draw_trade_coin(short msX, short msY, short msZ, char cLB);
	void click_trade_coin(short msX, short msY);
	int confirmoperation;

	char m_cAmmount[8];
	short selectedoption;
	int tmpquest;
	int tmptrade;
	int tmpquant;
	short trade_mode;

	struct st_trade_ek
	{
		st_trade_ek() {}
		short index;
		char itemname[21];
		int cost;
		short sprite;
		short spriteframe;
		short itemcolor;
	};

	vector<st_trade_ek> vec_trades;

	struct st_trade_contrib
	{
		st_trade_contrib() {}
		short index;
		char itemname[21];
		int cost;
		short sprite;
		short spriteframe;
		short itemcolor;
	};

	vector<st_trade_contrib> vec_trade_contrib;

	struct st_trade_coin
	{
		st_trade_coin() {}
		short index;
		char itemname[21];
		int cost;
		short sprite;
		short spriteframe;
		short itemcolor;
	};

	vector<st_trade_coin> vec_trade_coin;

	struct st_quest
	{
		st_quest() {}
		short m_sQuestID;
		char  m_cNpcName[21];
		char  m_cMapName[12];
		char  m_cRealMapName[12];
		int	  m_iMaxKills;
		int   m_iRewardContribution;
		char   m_cQuestReward1[21];
		char   m_cQuestReward2[21];
		char   m_cQuestReward3[21];
	};

	vector<st_quest> vec_questlist;

	struct st_charquest
	{
		st_charquest() {}
		short m_sParentQuestID;
		char  m_cNpcName[21];
		char  m_cMapName[12];
		char  m_cRealMapName[12];
		int   m_iKills;
		int	  m_iMaxKills;
		int   m_iRewardContribution;
		char  m_cQuestReward1[21];
		char  m_cQuestReward2[21];
		char  m_cQuestReward3[21];
		short m_sStatus;
		short m_sCompleted;
	};

	vector<st_charquest> vec_charquest;

	void DrawPixelsWhite(int mode, int count, short x, short y);
	void NotifyMsg_ItemRepaired2(char* pData);
	bool CGame::isInMarketMap(char* mapname);
	void shinning_status(char* data);
	char m_cSalirDelChar;
	char ConteoChangeCity;
	char m_cGameMode;
	BOOL m_bIsRedrawPDBGS;
	BOOL bCheckImportantFile();
	BOOL bInit(HWND hWnd, HINSTANCE hInst, char * pCmdLine);
	void ChangeGameMode(char cMode);
	bool GetText(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
	BOOL m_bEnterPressed, m_bEscPressed, m_bCtrlPressed, m_bRunningMode, m_bShiftPressed;
	BOOL m_bIsProgramActive;
	CInt m_bForceDisconn;//was BOOL
	void Quit();
	void OnKeyDown(WPARAM wParam);
	void OnGameSocketEvent(WPARAM wParam, LPARAM lParam);
	HWND m_hWnd;
	unsigned char m_cSkillMastery[DEF_MAXSKILLTYPE]; // v1.4
	void bAddNewEffect(short sType, int sX, int sY, int dX, int dY, char cStartFrame, int iV1 = 1);

	// MapData
	short m_sVDL_X, m_sVDL_Y;
	short m_sPlayerObjectID;
	int bHasHeroSet(short Appr3, short Appr4, char OwnerType);
	char m_cWhetherEffectType;
	short m_sViewPointX, m_sViewPointY;

	class CMisc      m_Misc;
	class DXC_dinput m_DInput;
	class DXC_ddraw  m_DDraw;

	class CMsg    * m_pChatMsgList[DEF_MAXCHATMSGS];
	char * m_pInputBuffer;
	char m_cEdit[4];


	/* Functions */
	void LoadFriendList();
	void SaveFriendList();
	void DrawDialogBox_FriendList(short msX, short msY, short msZ, char cLB);
	void DlgBoxClick_FriendList(short msX, short msY); 
	void AddFriend(char CharacterName[10]);
	void DeleteFriend(short Position);

	void LoadMuteList();
	void SaveMuteList();
	void DeleteMute(short Position);

	void ClearGuildList();

	void CheckActiveAura(short sX, short sY, DWORD dwTime, short sOwnerType);
	void CheckActiveAura2(short sX, short sY, DWORD dwTime, short sOwnerType);

	void DrawDialogBox_ChangeStatsMajestic(short msX, short msY);
	void DlgBoxClick_ChangeStatsMajestic(short msX, short msY);

	void ResponseTeleportList(char * pData);
	void ResponseChargedTeleport(char * pData);

	void ItemEquipHandler(char cItemID);
	void ReleaseEquipHandler(char cEquipPos);

	void ReadSettings();
	void WriteSettings();

	int  iGetManaCost(int iMagicNo);
	void UseMagic(int iMagicNo);
	BOOL _bCheckMoveable(short sx, short sy);
	BOOL FindGuildName(char* pName, int* ipIndex);
	void bItemDrop_SkillDialog();
	void bItemDrop_Character();
	void bItemDrop_Inventory(short msX, short msY);
	void bItemDrop_ItemUpgrade();
	void bItemDrop_SellList(short msX, short msY);
	void bItemDrop_ExchangeDialog(short msX, short msY);
	void bItemDrop_Bank(short msX, short msY);
	void bItemDrop_ExternalScreen(char cItemID, short msX, short msY);
	void CreateScreenShot();
	void CrusadeWarResult(int iWinnerSide);
	void CrusadeContributionResult(int iWarContribution);
	void CannotConstruct(int iCode);
	void DrawTopMsg();
	void SetTopMsg(char * pString, unsigned char iLastSec);
	void DrawObjectFOE(int ix, int iy, int iFrame, short sOwnerType);
	void GrandMagicResult(char * pMapName, int iV1, int iV2, int iV3, int iV4, int iHP1, int iHP2, int iHP3, int iHP4);
	void MeteorStrikeComing(int iCode);
	void _Draw_OnLogin(char * pAccount, char * pPassword, int msX, int msY, int iFrame = 60000);
	void DrawNewDialogBox(char cType, int sX, int sY, int iFrame, BOOL bIsNoColorKey = FALSE, BOOL bIsTrans = FALSE);
	void AddMapStatusInfo(char * pData, BOOL bIsLastData);
	void _RequestMapStatus(char * pMapName, int iMode);
	int  GetCharKind(char *str, int index);
	void ReceiveString(char * pString);
	void EndInputString();
	void ClearInputString();
	void ShowReceivedString(BOOL bIsHide = FALSE);
	BOOL bReadItemNameConfigFile();
	void DrawDialogBoxs(short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_Character(short msX, short msY);
	void DrawDialogBox_Inventory(int msX, int msY);
	void DrawDialogBox_Magic(short msX, short msY, short msZ);
	void PutString4(int iX, int iY, char* pString, COLORREF color);
	void DrawDialogBox_GuildMenu(short msX, short msY);
	void DrawDialogBox_GuildOperation(short msX, short msY);
	void DrawDialogBox_GuideMap(short msX, short msY, char cLB);
	void DlgBoxClick_GuideMap(short msX, short msY);
	void DrawDialogBox_Chat(short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_Shop(short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_LevelUpSetting(short msX, short msY);
	void DrawDialogBox_CityHallMenu(short msX, short msY);
	void DrawDialogBox_Bank(short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_Skill(short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_MagicShop(short msX, short msY, short msZ);
	void DrawDialogBox_QueryDropItemAmount();
	void DrawDialogBox_Text(short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_SysMenu(short msX, short msY);
	void DrawDialogBox_NpcActionQuery(short msX, short msY);
	void DrawDialogBox_NpcTalk(short msX, short msY, char cLB);
	void DrawDialogBox_Map();
	void DrawDialogBox_SellorRepairItem(short msX, short msY);
	void DrawDialogBox_Fishing(short msX, short msY);
	void DrawDialogBox_ShutDownMsg(short msX, short msY);
	void DrawDialogBox_SkillDlg(short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_Exchange(short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_Quest(int msX, int msY);
	void DrawDialogBox_IconPannel(short msX, short msY);
	void DrawDialogBox_IconPannel2(short msX, short msY);
	void DrawDialogBox_IconPannel3(short msX, short msY);
	void DrawDialogBox_SellList(short msX, short msY);
	void DrawDialogBox_Party(short msX, short msY);
	void DrawDialogBox_CrusadeJob(short msX, short msY);
	void DrawDialogBox_ItemUpgrade(int msX, int msY);
	void DrawDialogBox_Help(int msX, int msY);
	void DrawDialogBox_Commander(int msX, int msY);
	void DrawDialogBox_Constructor(int msX, int msY);
	void DrawDialogBox_Soldier(int msX, int msY);
	void DrawDialogBox_ItemDrop(short msX, short msY);
	void DrawDialogBox_WarningMsg(short msX, short msY);
	void DrawDialogBox_ChangeInt(short msX, short msY);
	void DrawDialogBox_FeedBackCard(short msX, short msY);
	void DisplayCommaNumber_G_cTxt(int iGold);

	void DrawInventoryShadowMenu(short x, short y, short x2, short y2);
	void DrawDialogBox_ConfirmExchange(short msX, short msY);

	void bItemDrop_Slates();
	void DlgBoxClick_Slates(short msX, short msY);
	void DrawDialogBox_Slates(short msX, short msY, short msZ, char cLB);

	BOOL _bCheckDlgBoxClick(short msX, short msY);
	void DlgBoxClick_WarningMsg(short msX, short msY);
	void DlgBoxClick_ChangeInt(short msX, short msY);
	void DlgBoxClick_ItemDrop(short msX, short msY);
	void DlgBoxClick_Character(short msX, short msY);
	void DlgBoxClick_Inventory(short msX, short msY);
	void DlgBoxClick_Magic(short msX, short msY);
	void DlgBoxClick_GuildMenu(short msX, short msY);
	void DlgBoxClick_GuildOp(short msX, short msY);
	void DlgBoxClick_Shop(short msX, short msY);
	void DlgBoxClick_LevelUpSettings(short msX, short msY);
	void DlgBoxClick_CityhallMenu(short msX, short msY);
	void DlgBoxClick_Bank(short msX, short msY);
	void DlgBoxClick_Skill(short msX, short msY);
	void DlgBoxClick_MagicShop(short msX, short msY);
	void DlgBoxClick_FeedBackCard(short msX, short msY);
	void DlgBoxClick_Text(short msX, short msY);
	void DlgBoxClick_SysMenu(short msX, short msY);
	void getNpcHealth(char* data);
	void send_object_data(WORD object);
	void DlgBoxClick_NpcActionQuery(short msX, short msY);
	void DlgBoxClick_NpcTalk(int msX, int msY);
	void DrawShadowDialog(short sX, short sY, int dialognum);
	void DrawShadowDialog2(short sX, short sY, int dialognum);
	void DlgBoxClick_ItemSellorRepair(short msX, short msY);
	void DlgBoxClick_Fish(short msX, short msY);
	void DlgBoxClick_ShutDownMsg(short msX, short msY);
	void DlgBoxClick_SkillDlg(short msX, short msY);
	void DlgBoxClick_Exchange(short msX, short msY);

	void DlgBoxClick_Quest(int msX, int msY);
	void DlgBoxClick_SellList(short msX, short msY);
	void DlgBoxClick_IconPannel(short msX, short msY);
	void DlgBoxClick_IconPannel3(short msX, short msY);
	void DlgBoxClick_Party(short msX, short msY);
	void DlgBoxClick_CrusadeJob(short msX, short msY);
	void DlgBoxClick_ItemUpgrade(int msX, int msY);
	void DlgBoxClick_Help(int msX, int msY);
	void DlgBoxClick_Commander(int msX, int msY);
	void DlgBoxClick_Constructor(int msX, int msY);
	void DlgBoxClick_Soldier(int msX, int msY);

	void NotifyMsgHandler(char * pData);
	void NotifyMsg_GlobalAttackMode(char * pData);
	void NotifyMsg_QuestReward(char * pData);
	void NotifyMsg_QuestContents(char * pData);
	void NotifyMsg_ItemColorChange(char * pData);
	void NotifyMsg_DropItemFin_CountChanged(char * pData);
	void NotifyMsg_CannotGiveItem(char * pData);
	void NotifyMsg_GiveItemFin_CountChanged(char * pData);
	void NotifyMsg_SetExchangeItem(char * pData);
	void NotifyMsg_OpenExchageWindow(char * pData);
	void NotifyMsg_DownSkillIndexSet(char * pData);
	void NotifyMsg_AdminInfo(char * pData);
	void NotifyMsg_WhetherChange(char * pData);
	void NotifyMsg_FishChance(char * pData);
	void NotifyMsg_EventFishMode(char * pData);
	void NotifyMsg_NoticeMsg(char * pData);
	void NotifyMsg_RatingPlayer(char * pData);
	void NotifyMsg_CannotRating(char * pData);
	void NotifyMsg_PlayerShutUp(char * pData);
	void NotifyMsg_TimeChange(char * pData);
	void NotifyMsg_Hunger(char * pData);
	void NotifyMsg_PlayerProfile(char * pData);
	void NotifyMsg_WhisperMode(BOOL bActive, char * pData);
	void NotifyMsg_PlayerStatus(BOOL bOnGame, char * pData);
	void NotifyMsg_Charisma(char * pData);
	void NotifyMsg_ItemRepaired(char * pData);
	void NotifyMsg_RepairItemPrice(char * pData);
	void NotifyMsg_CannotRepairItem(char * pData);
	void NotifyMsg_CannotSellItem(char * pData);
	void NotifyMsg_SellItemPrice(char * pData);
	void NotifyMsg_ShowMap(char * pData);
	void NotifyMsg_SkillUsingEnd(char * pData);
	void NotifyMsg_TotalUsers(char * pData);
	void NotifyMsg_MagicEffectOff(char * pData);
	void NotifyMsg_MagicEffectOn(char * pData);
	void NotifyMsg_SetItemCount(char * pData);
	void NotifyMsg_ItemDepleted_EraseItem(char * pData);
	void NotifyMsg_ServerChange(char * pData);
	void NotifyMsg_Skill(char * pData);
	void NotifyMsg_DropItemFin_EraseItem(char * pData);
	void NotifyMsg_GiveItemFin_EraseItem(char * pData);
	void NotifyMsg_EnemyKillReward(char * pData);
	void NotifyMsg_PKcaptured(char * pData);
	void NotifyMsg_PKpenalty(char * pData);
	void NotifyMsg_ItemToBank(char * pData);
	void NotifyMsg_ItemLifeSpanEnd(char * pData);
	void NotifyMsg_ItemReleased(char * pData);
	void NotifyMsg_LevelUp(char * pData);
	void NotifyMsg_SettingSuccess(char * pData);
	void NotifyMsg_MP(char * pData);
	void NotifyMsg_SP(char * pData);
	void NotifyMsg_SkillTrainSuccess(char * pData);
	void NotifyMsg_MagicStudyFail(char * pData);
	void NotifyMsg_MagicStudySuccess(char * pData);
	void NotifyMsg_DismissGuildsMan(char * pData);
	void NotifyMsg_NewGuildsMan(char * pData);
	void NotifyMsg_CannotJoinMoreGuildsMan(char * pData);
	void NotifyMsg_GuildDisbanded(char * pData);
	void NotifyMsg_Exp(char * pData);
	void NotifyMsg_Killed(char * pData);
	void NotifyMsg_HP(char * pData);
	void NotifyMsg_ItemPurchased(char * pData);
	void NotifyMsg_DismissGuildReject(char * pData);
	void NotifyMsg_DismissGuildApprove(char * pData);
	void NotifyMsg_JoinGuildReject(char * pData);
	void NotifyMsg_JoinGuildApprove(char * pData);
	void NotifyMsg_QueryDismissGuildPermission(char * pData);
	void NotifyMsg_QueryJoinGuildPermission(char * pData);
	void NotifyMsg_ItemObtained(char * pData);
	void NotifyMsg_ForceDisconn(char * pData);
	void NotifyMsg_BanGuildMan(char * pData);

	void ResponsePanningHandler(char * pData);
	void UpdateScreen_OnSelectServer();
	void StartInputString(int sX, int sY, unsigned char iLen, char * pBuffer, BOOL bIsHide = FALSE);
	void _SetIlusionEffect(int iOwnerH);
	int _iGetFOE(int iStatus, short sOwnerType);
	void NoticementHandler(char * pData);
	void GetItemName(char * cItemName, DWORD dwAttribute, char *pStr1, char *pStr2, char *pStr3, int iValue2 = 0);
	void GetItemName(class CItem * pItem, char * pStr1, char * pStr2, char * pStr3);
	void _InitOnCreateNewCharacter();
	void _LoadGameMsgTextContents();
	BOOL _bCheckCurrentBuildItemStatus();
	BOOL _bCheckBuildItemStatus();
	BOOL _bDecodeBuildItemContents();
	BOOL _bCheckBadWords(char * pMsg);
	void GetNpcName(short sType, char * pName);

	void GetMarketItemName(CItem* pItem, char* pStr1, char* pStr2, char* pStr3, char* pStr4);
	void _LoadAgreementTextContents(char cType);
	void UpdateScreen_OnAgreement();
	void UpdateScreen_OnCreateNewAccount();

	void UseShortCut(int num);
	void UpdateScreen_OnMainMenu();
	void UpdateScreen_OnGame();
	void UpdateScreen_OnConnecting();
	void UpdateScreen_OnWaitInitData();
	void MakeSprite(char* FileName, short sStart, short sCount, bool bAlphaEffect = TRUE, bool Pak = TRUE);
	void MakeTileSpr(char* FileName, short sStart, short sCount, bool bAlphaEffect = TRUE, bool Pak = TRUE);
	void MakeEffectSpr(char* FileName, short sStart, short sCount, bool bAlphaEffect = TRUE, bool Pak = TRUE);
	void UpdateScreen_OnConnectionLost();
	void UpdateScreen_OnLogin();
	void UpdateScreen_OnMsg();
	void UpdateScreen_OnQuit();
	void UpdateScreen_OnQueryForceLogin();
	void UpdateScreen_OnQueryDeleteCharacter();
	void UpdateScreen_OnWaitingResponse();
	void UpdateScreen_OnCreateNewCharacter();
	void UpdateScreen_OnSelectCharacter();
	void UpdateScreen_OnLogResMsg();
	void UpdateScreen_OnSelectCharacter(short sX, short sY, short msX, short msY, BOOL bIgnoreFocus = FALSE);
	void UpdateScreen_OnChangePassword();
	void UpdateScreen_OnLoading_Progress();
	void DrawShadowMenu_DialogBordersNoTile(short sX, short sY, short szX, short szY);
	void DrawShadow_Box(short x, short y, short x2, short y2);
	void UpdateScreen_OnVersionNotMatch();
	void NpcTalkHandler(char * pData);
	int  _iGetWeaponSkillType();
	void SetCameraShakingEffect(short sDist, int iMul = 0);
	BOOL bDlgBoxPress_SkillDlg(short msX, short msY);
	BOOL bDlgBoxPress_Inventory(short msX, short msY);
	BOOL bDlgBoxPress_Character(short msX, short msY);
	void ClearSkillUsingStatus();
	BOOL bCheckItemOperationEnabled(char cItemID);
	void _DrawThunderEffect(int sX, int sY, int dX, int dY, int rX, int rY, char cType);
	void DrawLine2(int x0, int y0, int x1, int y1, int iR, int iG, int iB);
	void DrawLine(int x0, int y0, int x1, int y1, int iR, int iG, int iB);
	void SetWhetherStatus(BOOL bStart, char cType);
	void WhetherObjectFrameCounter();
	void DrawWhetherEffects();
	BOOL bCheckExID(char * pName);
	BOOL bCheckLocalChatCommand(char * pMsg);
	char GetOfficialMapName(char * pMapName, char * pName);
	int iGetLevelExp(int iLevel);
	int _iCalcTotalWeight();
	void DrawVersion(BOOL bAuthor = FALSE);
	BOOL _bIsItemOnHand();
	void DynamicObjectHandler(char * pData);
	BOOL _bCheckItemByType(char cType);
	void _DrawBlackRect(int iSize);
	void DrawNpcName(short sX, short sY, short sOwnerType, int iStatus, WORD tmpobject = -1);
	void DrawObjectName(short sX, short sY, char * pName, int iStatus, int iObjectID = -1);
	void _RemoveChatMsgListByObjectID(int iObjectID);
	void _LoadTextDlgContents(int cType);
	int  _iLoadTextDlgContents2(int iType);
	void DrawChatMsgs(short sX, short sY, short dX, short dY);
	BOOL bInitSkillCfgList();
	void DlbBoxDoubleClick_Inventory(short msX, short msY);
	void DlbBoxDoubleClick_Character(short msX, short msY);
	void DlbBoxDoubleClick_GuideMap(short msX, short msY);
	BOOL _bCheckDlgBoxDoubleClick(short msX, short msY);
	void EraseItem(char cItemID);
	void RetrieveItemHandler(char * pData);
	void CivilRightAdmissionHandler(char * pData);
	void _Draw_CharacterBody(short sX, short sY, short sType);
	void ClearContents_OnSelectCharacter();
	void ClearContents_OnCreateNewAccount();
	void _Draw_UpdateScreen_OnCreateNewAccount();
	BOOL _bDraw_OnCreateNewCharacter(char * pName, short msX, short msY, int iPoint);
	BOOL _bGetIsStringIsNumber(char * pStr);
	BOOL bInitMagicCfgList();
	BOOL __bDecodeContentsAndBuildItemForSaleList(char * pBuffer);
	void _LoadShopMenuContents(char cType);
	void PutChatScrollList(char * pMsg, char cType);
	void RequestTeleportAndWaitData();
	void DrawEffectLights();
	void PointCommandHandler(int indexX, int indexY, char cItemID = -1);
	void DrawEffects();
	void AddEventList(char * pTxt, char cColor = 0, BOOL bDupAllow = TRUE);
	void ShowEventList(DWORD dwTime);
	void SetItemCount(char * pItemName, DWORD dwCount);
	void _ShiftGuildOperationList();
	void _PutGuildOperationList(char * pName, char cOpMode);
	void DisbandGuildResponseHandler(char * pData);
	void InitPlayerCharacteristics(char * pData);
	void CreateNewGuildResponseHandler(char * pData);
	void _GetHairColorRGB(int iColorType, int * pR, int * pG, int * pB);
	void InitGameSettings();
	void CommonEventHandler(char * pData);
	BOOL _bCheckDraggingItemRelease(short msX, short msY);
	void _SetItemOrder(char cWhere, char cItemID);
	int iGetTopDialogBoxIndex();
	void DisableDialogBox(int iBoxID);
	void EnableDialogBox(int iBoxID, int cType, int sV1, int sV2, char * pString = NULL);
	void InitItemList(char * pData);
	int _iCheckDlgBoxFocus(short msX, short msY, char cButtonSide);
	void GetPlayerTurn();
	static BOOL NotepadFunc(HWND hwnd, LPARAM lParam);
	/*BOOL __fastcall DrawObject_OnDead(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnDying(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnMagic(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnAttack(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnAttackMove(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnStop(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnMove_ForMenu(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnMove(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnDamageMove(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnRun(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnDamage(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnGetItem(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
*/

	BOOL __fastcall DrawObject_OnDead(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID);
	BOOL __fastcall DrawObject_OnDying(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);
	BOOL __fastcall DrawObject_OnMagic(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID);
	BOOL __fastcall DrawObject_OnAttack(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID);
	BOOL __fastcall DrawObject_OnAttackMove(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID);
	BOOL __fastcall DrawObject_OnStop(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID);

	BOOL __fastcall DrawObject_OnMove_ForMenu(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY);

	BOOL __fastcall DrawObject_OnMove(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID, const bool frame_omision = false);
	BOOL __fastcall DrawObject_OnDamageMove(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID, const bool frame_omision = false);
	BOOL __fastcall DrawObject_OnRun(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID, const bool frame_omision = false);

	int CalculateSteps(short startX, short startY, short targetX, short targetY);
	BOOL __fastcall DrawObject_OnDamage(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID);
	BOOL __fastcall DrawObject_OnGetItem(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID);

	BOOL __fastcall DrawObject(int indexX, int indexY, int sX, int sY, BOOL bTrans, DWORD dwTime, int msX, int msY, int ObjectID, char _tmp_cAction, const bool frame_omision = false);
	BOOL bEffectFrameCounter();
	void ClearGuildNameList();
	void DrawBackground(short sDivX, short sModX, short sDivY, short sModY);
	void DrawChatMsgBox(short sX, short sY, int iChatIndex, BOOL bIsPreDC);
	void ReleaseTimeoverChatMsg();
	void ChatMsgHandler(char * pData);
	void ReleaseUnusedSprites();
	BOOL bReadIp();
	void PutString(int iX, int iY, char * pString, COLORREF color);
	void PutString(int iX, int iY, char * pString, COLORREF color, BOOL bHide, char cBGtype, BOOL bIsPreDC = FALSE);
	void PutString2(int iX, int iY, char * pString, short sR, short sG, short sB);
	void PutAlignedString(int iX1, int iX2, int iY, char * pString, short sR = 0, short sG = 0, short sB = 0);
	void PutAlignedString2(int iX1, int iX2, int iY, char * pString, short sR = 0, short sG = 0, short sB = 0);
	void PutString_SprFont(int iX, int iY, char * pStr, short sR, short sG, short sB);
	void PutString_SprFont2(int iX, int iY, char * pStr, short sR, short sG, short sB);
	void PutString_SprFont3(int iX, int iY, char * pStr, short sR, short sG, short sB, BOOL bTrans = FALSE, int iType = 0);
	void PutString_SprNum(int iX, int iY, char * pStr, short sR, short sG, short sB);
	void LogRecvMsgHandler(char * pData);
	void LogResponseHandler(char * pData);
	void LogEventHandler(char * pData);
	void _ReadMapData(short sPivotX, short sPivotY, char * pData);
	void MotionEventHandler(char * pData);
	void InitDataResponseHandler(char * pData);
	void InitPlayerResponseHandler(char * pData);

	void ConnectionEstablishHandler(char cWhere);
	void ConnectionEstablishHandler2();
	void MotionResponseHandler(char * pData);
	void GameRecvMsgHandler(DWORD dwMsgSize, char * pData);
	void DrawObjects(short sPivotX, short sPivotY, short sDivX, short sDivY, short sModX, short sModY, short msX, short msY);
	BOOL bSendCommand(DWORD dwMsgID, WORD wCommand, char cDir, int iV1, int iV2, int iV3, char * pString, int iV4 = NULL); // v1.4
	char cGetNextMoveDir(short sX, short sY, short dstX, short dstY, BOOL bMoveCheck = FALSE, BOOL bMIM = FALSE);
	char cGetNextMoveDirMVT(short sX, short sY, short dstX, short dstY, BOOL bMoveCheck = FALSE, BOOL bMIM = FALSE);
	void RestoreSprites();
	void CommandProcessor(short msX, short msY, short indexX, short indexY, char cLB, char cRB);
	//BOOL CheckProcesses();
	BOOL CheckSarpado();
	void CalcViewPoint();

	void ReserveFightzoneResponseHandler(char * pData);
	int _iGetAttackType();
	BOOL __bDecodeBuildItemContents(char * pBuffer);
	int _iGetBankItemCount();
	int _iGetTotalItemNum();
	LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
	void GoHomepage(short type);
	void StartBGM(bool bLogin);

	void DebugLog(char * cStr);
	BOOL bReadLoginConfigFile(char * cFn);
	void ShowHeldenianVictory(short sSide);
	void DrawDialogBox_Resurect(short msX, short msY);
	void DlgBoxClick_Resurect(short msX, short msY);
	void DrawDialogBox_CMDHallMenu(short msX, short msY);
	void DlgBoxClick_CMDHallMenu(short msX, short msY);
	void ResponseHeldenianTeleportList(char *pData);
	void DKGlare(short iWeaponColor, int iWeaponIndex, short *iWeaponGlare);
	void DrawDruncncity();
	void DlgBoxClick_ConfirmExchange(short msX, short msY);
	void Abaddon_corpse(int sX, int sY);
	void DrawAngel(int iSprite, short sX, short sY, char cFrame, DWORD dwTime);
	char m_cLogServerAddrBuffer[100];

	BOOL _ItemDropHistory(char * ItemName);

	void GetIPByDNS();
	//bool CGame::bCheckItemEquiped(char itemName[]);
	bool bCheckItemEquiped(char itemName[]);
	void ThunderEffectAbaddonMap(short sX, short sY);

	void LoadTextNewPanel(short sX, short sY, short msX, short msY, short msZ, char cLB);
	void DrawDialogBox_NewPanelConfirmation(short msX, short msY);
	void DlgBoxClick_NewPanelConfirmation(short msX, short msY);
	void OpenNoticement();

	void DrawDialogBox_Event(short msX, short msY);
	void DlgBoxClick_Event(short msX, short msY);
	void DrawFlag(short sX, short sY, DWORD dwTime);

	void DlgBoxClick_Status(short msX, short msY);
	void DrawDialogBox_Status(short msX, short msY);

	void DrawDialogBox_QuestList(short msX, short msY, short msZ, char cLB);
	void DlgBoxClick_QuestList(short msX, short msY);
	BOOL bReadQuestCFG(char CFG[], short Type);
	void DlgBoxClick_Chat(short msX, short msY);

	void ArmorGlare(short iArmorColor, int iArmArmorIndex, short *iArmorGlare);
	void LegginsGlare(short iArmorColor, int iBodyArmorIndex, short *iArmorGlare);
	void HauberkGlare(short iArmColor, int iArmArmorIndex, short *iArmGlare);
	void HelmGlare(short iHelmColor, int iHelmIndex, short *iHelmGlare);
	void MantleGlare(short iMantleColor, int iMantleIndex, short *iMantleGlare);
	void BootsGlare(short iBootsColor, int iBootsIndex, short *iBootsGlare);
	void ShieldGlare(short iShieldColor, int iShieldIndex, short *iShieldGlare);

	void LoadItemList(char *pData);
	BOOL bCheckOpenMarket();
	void DrawDialogBox_PurchaseAssasainsPoints(short msX, short msY);
	void DlgBoxClick_PurchaseAssasainsPoints(short msX, short msY);
	int EncriptInt(int var);
	int DecriptInt(int var);
	void EncriptString(char var[]);
	void DecriptString(char var[]);

	void NotifyMsg_LuPool(char * pData);
	void ShowAdmin(short sX, short sY, char Admin);
	int iDice(int iThrow, int iRange);
	void MyBar(short sX, short sY, DWORD dwTime);
	void PartyBar(short sX, short sY, short i, DWORD dwTime);
	void DrawDialogBox_TradeItems(short msX, short msY, short msZ, char cLB);
	void DlgBoxClick_TradeItems(short msX, short msY);
	void ChargeTopList(char *pData);
	void ShowItemDetail(short Dialog, short sX, short sY, short msX, short msY, short msZ);
	void ClicItemDetail(short Dialog, short sX, short sY, short msX, short msY);
	void ShowItemList(short Dialog, short sX, short sY, short msX, short msY, short msZ, char cLB, BOOL Bar = TRUE);
	void ClicItemList(short Dialog, short sX, short sY, short msX, short msY);
	void ShowMenuItems(short sX, short sY, short msX, short msY);
	void ClicMenuItems(short Dialog, short sX, short sY, short msX, short msY);
	void DeleteItems();

	void DrawDialogBox_NewPanel(short msX, short msY, short msZ, char cLB);
	void AnimateSizeY();
	void DlgBoxClick_NewPanel(short msX, short msY);
	void ViewPanelButtons(short sX, short sY, short msX, short msY, BOOL Down);
	void ClicPanelButtons(short Dialog, short sX, short sY, short msX, short msY, BOOL Down);
	void ShowTopEk(short Dialog, short sX, short sY, short msX, short msY, short msZ);
	void ClicTopEk(short Dialog, short sX, short sY, short msX, short msY);
	void RemoveItemExchange(short ItemPosition);
	void ShowItemExchange(short Dialog, short sX, short sY, short Item, short Show);
	void FExchangeLog(BOOL Client, BOOL Add, short Position);
	void DlgBoxClick_Achivements(short msX, short msY);
	void DrawDialogBox_Achivements(short msX, short msY);
	BOOL bReadAchivements();
	int SpaceChar(char String[]);
	void DrawDialogBox_Barrita(short msX, short msY);
	void DlgBoxClick_Barrita(short msX, short msY);
	BOOL bDlgBoxPress_Barrita(short msX, short msY);
	BOOL bDlgBoxPress_MagicWindow(short msX, short msY);
	BOOL _bCheckDraggingMagicRelease(short msX, short msY);
	void bItemDrop_Shortcut(short msX, short msY);
	void bMagicDrop_Panel(short msX, short msY);
	void ShowSetting(short Dialog, short msX, short msY, char cLB);
	void ClicSetting(short Dialog, short msX, short msY);
	void ShowSetting2(short Dialog, short msX, short msY, char cLB);
	void ClicSetting2(short Dialog, short msX, short msY);
	void DrawDialogBox_Coins(short msX, short msY, short msZ, char cLB);
	void DlgBoxClick_Coins(short msX, short msY);
	void CreateSettingsCFG();
	void ReadSettingsCFG();
	void NotifyMsg_EkAnnoucer(char * pData);
	void DrawDialogBox_EKPKLog(short msX, short msY, short msZ, char cLB);
	void DlgBoxClick_EKPKLog(short msX, short msY);
	//void ReadBloquedProcess(char * pData);
	void TeSentisSarpado(char * pData);
	void SaveShortCutsList();
	void LoadShortCutsList(); //BOOL CheckHacks();
	BOOL CheckPalmado();
	void SendProcess();
	void SendModule(DWORD PID);
	void ChargeTopEventList(char *pData);
	void ChargeMyDataEvent(char *pData);
	BOOL CheckItemStatReq(char iItemID);
	void startReceiver();
	void notifyLevelChange();
	void DrawDialogBox_CharSell(short msX, short msY, short msZ, char cLB); // 49
	void DlgBoxClick_CharSell(short msX, short msY); // 49
	void DrawDialogBox_OpenCharSell(short msX, short msY, short msZ, char cLB); // 54
	void DlgBoxClick_OpenCharSell(short msX, short msY); // 54
	void SendItemToSellList();
	void bSetItemToSellList(short msX, short msY);
	char EncriptBool(bool EncriptBool);
	BOOL DecriptBool(char var);	

	void DrawDialogBox_IconPannel800x600(short msX, short msY);
	void DlgBoxClick_IconPannel800x600(short msX, short msY);
	void DrawDialogBox_IconPannel1024x768(short msX, short msY);
	void DlgBoxClick_IconPannel1024x768(short msX, short msY);

	//HeatoN 2024
	enum Lang{
		LANG_ESP,
		LANG_ENG
	};
	void startUDP();
	double test_request_ping();
	void InitDialogs();
	void DrawShadowMenu(short x, short y, short x2, short y2);
	void DrawDialogShadowPixels(int mode, int count, short x, short y);
	void DrawShadowMenu_DialogBorders(short sX, short sY, short szX, short szY);
	void DrawShadowMenu_DialogBordersNoTitle(short sX, short sY, short szX, short szY);
	void DrawNewDialogBox2(int cType, int sX, int sY, int iFrame, BOOL bIsNoColorKey = FALSE, BOOL bIsTrans = FALSE);
	void PutCenterString_SpriteFont(int iX1, int iX2, int iY, const char * pStr, short sR, short sG, short sB);
	void draw_enchanting(short msX, short msY, short msZ, char cLB);
	bool PermitedType1(short type);
	bool PermitedType2(short type);
	void click_enchanting(short msX, short msY);
	string GetStatNameType2(short type);
	string GetStatNameType1(short type);
	void bItemDrop_enchanting();
	void getCommandHandler(char * data);
	void procCommandHandler(char * command, int val1, int val2);
	void draw_rebirth(short msX, short msY);
	void DrawDialogCord(short sX, short sY, short msX, short msY);
	void click_rebirth(short msX, short msY);
	void MakeCommonSprite(char* FileName, int sStart, int sCount, bool bAlphaEffect = TRUE);
	void DrawEnhancement_Tab(short sX, short sY, short msX, short msY, char * text, bool active);
	void DrawMenu_EnhBox(short sX, short sY, short width, short height);
	void DrawDialogShadowPixelsEnc(int mode, int count, short x, short y);
	void DrawShadowMenu_ExtMenuBorders(short sX, short sY, short szX, short szY);
	void DrawComboBoxEnc(short x, short y, short width, short height, const std::vector<std::string>& options, int selectedIndex, bool isOpen, short msX, short msY, bool isEnabled);
	void HandleComboBoxClick(short msX, short msY, short comboX, short comboY, short comboWidth, short comboHeight, const std::vector<std::string>& options, char cLB, int dnumber, bool isEnabled);
	void DrawMenu_SmallButton(short sX, short sY, short msX, short msY, short x, short y, char * text);
	
	void DrawNewPanelButton(short sX, short sY, short msX, short msY, short x, short y, char * text, bool istrue);
	void DrawMenu_LongBut(short sX, short sY, short msX, short msY, short x, short y, const char* text);

	void SendChat(char * str);
	void DrawActiveEvents();
	void click_requestrevive(short msX, short msY);
	void draw_requestrevive(short msX, short msY);

	bool isInMap(char * name)
	{
		return !strcmp(m_cMapName, name);
	}

	bool showsetts;
	short m_sLang;
	bool isComboBoxOpen; 
	int selectedOptionIndex;
	bool isClickProcessed; 
	short selectedType;
	short seltype;
	short SelectedType1;
	short SelectedType2;
	char costMessage[200];
	char displayStr[200];

	int eventcount;
	//heaton max level client side
	int m_iPlayerMaxLevel;


	int shard_light;
	int shard_strong;

	int fragment_hp;
	int fragment_dr;
	int fragment_hprec;
	int fragment_sprec;
	int fragment_mprec;
	int fragment_mr;
	int fragment_pa;
	int fragment_ma;
	int fragment_exp;

	/* Structures */
	struct {
		char m_cFriends[10];
		BOOL status;
	} ListFriends[DEF_MAXLISTNAMES];

	struct {
		char m_cMuteName[10];
		BOOL status;
	} ListMute[DEF_MAXLISTNAMES];

	struct {
		char GuildMemberName[11];
		char Rank;
	} GuildMembersOnline[100];

	struct {
		short sX;
		short sY;
		short sCursorFrame;
		char  cPrevStatus;
		char  cSelectedObjectType;
		short sSelectedObjectID;
		short sPrevX, sPrevY, sDistX, sDistY;
		DWORD dwSelectClickTime;
		short sClickX, sClickY;
	} m_stMCursor;

	struct Dialog {
		int   sV1, sV2, sV3, sV4, sV5, sV6, sV7, sV8, sV9, sV10, sV11, sV12, sV13, sV14; // v1.4 short
		DWORD dwV1, dwV2, dwT1;
		BOOL  bFlag;
		short sX, sY;
		short sSizeX, sSizeY;
		short sView;
		char  cStr[32], cStr2[32], cStr3[32], cStr4[32];
		short cMode;
		BOOL  bIsScrollSelected;

		void ClearMarketInput()
		{
			sV11 = 0;
		}

		bool IsMarketInput()
		{
			return sV11 > 0;
		}
		
		int m_iEnchantingItem[20];

	} m_stDialogBoxInfo[DEF_MAXDIALOGBOX];

	struct {
		int   sV1, sV2, sV3, sV4, sV5, sV6, sV7, sItemID;
		DWORD dwV1;
		char  cStr1[32], cStr2[32], cValueA;
	} m_stDialogBoxExchangeInfo[DEF_MAXITEMEXCHANGED];

	struct {
		int iIndex;
		int iAmount;
	} m_stSellItemList[DEF_MAXSELLLIST];

	struct {
		char cName[22];
		char cOpMode;
	} m_stGuildOpList[100];

	struct {
		DWORD dwTime;
		char  cColor;
		char  cTxt[96];
	} m_stEventHistory[6];

	struct {
		DWORD dwTime;
		char  cColor;
		char  cTxt[96];
	} m_stEventHistory2[6];

	struct {
		short sX, sY, sBX;
		char cStep;
	} m_stWhetherObject[DEF_MAXWHETHEROBJECTS];

	struct {
		BOOL bIsQuestCompleted;
		short sWho, sQuestType, sContribution, sTargetType, sX, sY, sRange;
		unsigned int sCurrentCount, sTargetCount; // by Snoopy
		char cTargetName[22];
	} m_stQuest[2];

	struct {
		char cStatus;
		char cName[12];
	} m_stPartyMember[DEF_MAXPARTYMEMBERS];

	struct {
		short sX, sY;
		char cType;
		char cSide;
	} m_stCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];

	struct {
		DWORD dwRefTime;
		int iGuildRank;
		char cCharName[12];
		char cGuildName[24];
	} m_stGuildName[DEF_MAXGUILDNAMES];

	struct {
		int iIndex;
		char mapname[12];
		int iX;
		int iY;
		int iCost;
		BOOL Event;
		BOOL TPEvent;
	} m_stTeleportList[20];

	struct {
		DWORD dwTime;
		char  cColor;
		char  cTxt[96];
	} m_stEventHistory3[6];

	struct {
		char cName[12];
	} m_stPartyMemberNameList[DEF_MAXPARTYMEMBERS + 1];

	struct {
		char Notice[26];
	} SNotice[250];

	struct {
		short ID;
		short City;
		short Bicho;
		int CantNPC;
		short MinL;
		short MaxL;
		int Price;
		char Map[20];
		char ItemPrice[50];
		short Type;
		short ItemType;
	} m_sQuest[500];

	struct {
		short ID;
		char Msg[75];
		DWORD cType;
	} m_sChatBox[DEF_MAXCHATSCROLLMSGS];

	struct {
		int iIndex;
		int iAmount;
		unsigned int uiPrice; // Price :P
		char cStr1[64], cStr2[64], cStr3[64];
	} m_stItemListToSell[DEF_MAXSELLITEMSLIST];

	struct {
		char CharName[11];
		short Kills, Death, Nivel;
	} TopEvent[10];

	struct {
		int iPartyHp, iParty_sX, iParty_sY;
		char PartyMapName[12];
	} PartyStruct[DEF_MAXPARTYMEMBERS];

	struct {
		int Init, Finish;
		char Title[30];
	} TitleShop[DEF_MAXITEMS];

	struct {
		char Name[11];
		char side;
		char City[8];
		int Points;
	} TopList[90];

	struct {
		BOOL Me;
		char ItemName[80];
		BOOL Added;
	} ExchangeLog[3];

	struct {
		BOOL Active;
		char Porcent;
		char Title[25];
		char Description[100];
		char Reward[50];
	} Achivement[DEF_MAXACHIVEMENTS];

	/* Class */
	class YWSound		  m_DSound;
	class CSoundBuffer	* m_pCSound[DEF_MAXSOUNDEFFECTS];
	class CSoundBuffer	* m_pMSound[DEF_MAXSOUNDEFFECTS];
	class CSoundBuffer	* m_pESound[DEF_MAXSOUNDEFFECTS];
	class CSoundBuffer	* m_pBGM;
	class CSprite		* m_pSprite[DEF_MAXSPRITES];
	class CSprite		* m_pTileSpr[DEF_MAXTILES];
	class CSprite		* m_pEffectSpr[DEF_MAXEFFECTSPR];
	class CMapData		* m_pMapData;
	class XSocket		* m_pGSock;
	class XSocket		* m_pLSock;
	int udpPort;
	SOCKET clientSocket;
	sockaddr_in serverAddr;
	double m_dPing;
	void initializeWinsock();
	void setupServerAddress(const std::string& serverIp, int port);
	void createSocket();
	void sendMessageToServer(int msgid);/*
	void sendMessage(const std::string& message);*/
	void receiveMessageFromServer();
	void request_ping();
	void startUDPClient(int port);
	void destroyUDPClient();
	class CMsg			* m_pChatScrollList[DEF_TEXTDLGMAXLINES];
	class CMsg			* m_pEnemyKillList[DEF_TEXTDLGMAXLINES];
	class CMsg			* m_pWhisperMsg[DEF_MAXWHISPERMSG];
	class CEffect		* m_pEffectList[DEF_MAXEFFECTS];
	class CItem			* m_pItemList[DEF_MAXITEMS];
	class CItem			* m_pBankList[DEF_MAXBANKITEMS];
	class CItem			* m_pItemSellList[DEF_MAXSELLITEMSLIST];
	class CMagic		* m_pMagicCfgList[DEF_MAXMAGICTYPE];
	class CSkill		* m_pSkillCfgList[DEF_MAXSKILLTYPE];
	class CMsg			* m_pMsgTextList[DEF_TEXTDLGMAXLINES];
	class CMsg			* m_pMsgTextList2[DEF_TEXTDLGMAXLINES];
	class CMsg			* m_pAgreeMsgTextList[DEF_TEXTDLGMAXLINES];
	class CBuildItem	* m_pBuildItemList[DEF_MAXBUILDITEMS];
	class CBuildItem	* m_pDispBuildItemList[DEF_MAXBUILDITEMS];
	class CGameMonitor	* m_pCGameMonitor;
	class CItem			* m_pItemForSaleList[DEF_MAXMENUITEMS];
	class CCharInfo		* m_pCharList[4];
	class CMsg			* m_pGameMsgList[DEF_MAXGAMEMSGS];
	class CItemName		* m_pItemNameList[DEF_MAXITEMNAMES];
	class CCurse		  m_curse;
	class CBuildItem	* m_pCraftItemList[DEF_MAXBUILDITEMS];
	class CBuildItem	* m_pDispCraftItemList[DEF_MAXBUILDITEMS];
	
	/* Vars */
	/* short */
	// Encripted
	short m_iHP;
	short m_iMP;
	short m_iSP;
	short m_iLU_Point;
	short m_iGizonItemUpgradeLeft;
	short m_sPlayerX;
	short m_sPlayerY;
	short m_cLU_Str;
	short m_cLU_Vit;
	short m_cLU_Dex;
	short m_cLU_Int;
	short m_cLU_Mag;
	short m_cLU_Char;
	short m_iReputation;
	short ItemsInExchange;
	short ItemsInExchange2;
	short TotalItemsSell;
	short SellType;

	// Sin Encriptar
	short m_iTotalGMembes;
	short m_iTotalFriends;
	short m_iTotalMutes;
	short QuestShow;
	short m_sMonsterID;
	short m_sEventX;
	short m_sEventY;
	short m_sMagicShortCut;
	short m_sRecentShortCut;
	short m_sPlayerType;
	short m_sPlayerAppr1;
	short m_sPlayerAppr2;
	short m_sPlayerAppr3;
	short m_sPlayerAppr4;
	short m_sMCX, m_sMCY;
	short m_sCommX;
	short m_sCommY;
	short m_sDamageMove;
	short m_sAppr1_IE;
	short m_sAppr2_IE;
	short m_sAppr3_IE;
	short m_sAppr4_IE;
	short m_sZerk;
	short m_sInv;
	short m_sPfm;
	short m_sPfa;
	short m_sShield;
	short m_sTiming;
	//short sTiming;//lalo
	short GolAresden;
	short GolElvine;
	short BigBar;
	short Resolution;
	short m_sDrawFlag;
	short m_sDrawFlag3;
	short m_sGrid;
	short TempMajestic;
	short TempDiscount;
	short TempPage;
	short m_iRoundCount;
	short ActiveEvent;
	short iBarEight;
	short ItemGround;
	short MAABS;
	short PAABS;
	short HPREC;
	short MPREC;
	short DR;
	short MR;
	short PR;
	short SP;
	//laloMerien
	short DF;
	short Defense;
	short Hitting;
	short HitMag;
	short PhysicalDamage;
	short MagicDamage;
	short MyKills;
	short MyDeaths;
	short MyNivel;
	short _tmp_sOwnerType;
	short _tmp_sAppr1;
	short _tmp_sAppr2;
	short _tmp_sAppr3;
	short _tmp_sAppr4;
	short sFocus_dX;
	short sFocus_dY;
	short sFocusX;
	short sFocusY;
	short sFocusOwnerType;
	short sFocusAppr1;
	short sFocusAppr2;
	short sFocusAppr3;
	short sFocusAppr4;
	short m_sViewDstX;
	short m_sViewDstY;
	short AchivementsComplete;
	short m_iHPAres;
	short m_iHPElv;


	/* short [X]*/
	short m_sShortCut[6];
	short m_sItemEquipmentStatus[DEF_MAXITEMEQUIPPOS];
	short StatedQuest[10];
	short ChatActive[25];
	short NewShortcuts[9];
	
	/* ex-char -> New short */
	// Encriptado
	short cStateChange1;
	short cStateChange2;
	short cStateChange3;
	short m_cRestartCount;
	short m_cPlayerDir;
	short m_cMapIndex;
	short m_cGender;
	short m_cSkinCol;
	short m_cHairStyle;
	short m_cHairCol;
	short m_cUnderCol;
	short m_ccStr;
	short m_ccVit;
	short m_ccDex;
	short m_ccInt;
	short m_ccMag;
	short m_ccChr;

	/* Ex-Bool -> char */
	// Encriptado
	//char m_bHackMoveBlocked;
	char m_bPackSarpadBlock;
	char m_bIllusionMVT;
	char MarketOpen;

	char m_bHunter;
	char m_bAresden;
	char m_bElvine;
	char m_bCitizen;
	char m_bIsCombatMode;
	char m_bIsObserverMode;
	char m_bIsObserverCommanded;
	char m_bIsConfusion;
	char m_bParalyze;
	char m_bIsSafeAttackMode;

	// Sin encriptar
	char m_cGameModeCount;
	char m_cWhisperIndex;
	char m_cPlayerTurn;
	char m_cCommand;
	char m_cCurFocus;
	char m_cMaxFocus;
	char m_cEnterCheck;
	char m_cTabCheck;
	char m_cLeftArrowCheck;
	char m_cArrowPressed;	
	char m_cDetailLevel;
	char m_cMenuDir;
	char m_cMenuDirCnt;
	char m_cMenuFrame;
	char m_cSoundVolume;
	char m_cMusicVolume;
	char m_cWhetherStatus;
	char m_cIlusionOwnerType;
	char m_sViewDX;
	char m_sViewDY;
	char m_cCommandCount;
	char m_cLoading;
	char m_cDiscount;	
	char ElvineFlags;
	char AresdenFlags;
	char AssasainFlags;
	char m_iTowerAresden;
	char m_iTowerElvine;
	char m_iTowerDefenseAresden;
	char m_iTowerDefenseElvine;
	char MyAdminLevel;
	char m_iActualRound;
	char m_iTotalRound;
	char ListType;
	char LoadContent;
	char ExchangeError;
	char ShowBarTotal;
	char MinPlaying;
	char ConfigIP;
	char _tmp_cAction;
	char _tmp_cDir;
	char _tmp_cFrame;
	char _tmp_AdminLevel;
	char cDynamicObjectData1;
	char cDynamicObjectData2;
	char cDynamicObjectData3;
	char cDynamicObjectData4;
	char fFocusAdminLevel;
	char cFocusAction;
	char cFocusFrame;
	char cFocusDir;	

	/* char [X]*/
	// Encriptado 
	char m_cAccountName[12];
	char m_cAccountPassword[12];
	char m_cPlayerName[12];
	char m_cLocation[12];
	char m_cCurLocation[12];
	char m_cGuildName[22];
	char m_cMCName[12];
	char m_cMapName[12];
	char m_cLogServerAddr[32];
	char m_cWorldServerName[32];
	char m_cGameServerName[22];
	char m_cConstructMapName[12];
	char OpenSellName[12];
	char Process[100][50];
	char Dll[100][50];

	// Sin Encriptar
	char m_cNewPassword[12];
	char m_cNewPassConfirm[12];
	char m_cEmailAddr[52];
	char m_cDialogBoxOrder[DEF_MAXDIALOGBOX];
	char G_cTxt[128];
	char m_cBGMmapName[12];
	char m_cItemOrder[DEF_MAXITEMS];
	char m_cAmountString[12];
	char m_cAccountAge[12];
	char m_cAccountCountry[18];
	char m_cAccountSSN[32];
	char m_cAccountQuiz[46];
	char m_cAccountAnswer[22];
	char m_cMsg[200];
	char m_cMapMessage[32];
	char m_cBackupChatMsg[64];
	char m_cMagicMastery[DEF_MAXMAGICTYPE];
	char m_cName_IE[12];
	char m_cStatusMapName[12];
	char m_cTopMsg[64];
	char m_cTeleportMapName[12];
	char m_cItemDrop[25][25];
	char m_cGateMapName[DEF_MAXPORTALS][10];
	char m_cTakeHeroItemName[100];
	char TradeType[20];
	char PlayerNameExc[10];
	char _tmp_cName[12];
	char cFocusName[12];
	char m_cChatMsg[64];


	/* int */
	// Encripted
	int m_iTeleportMapCount;
	int m_iAgreeView;
	int m_iFightzoneNumber;
	int m_iFightzoneNumberTemp;
	int m_iLevel;
	int m_iContribution;
	int m_iAngelicStr;
	int m_iAngelicInt;
	int m_iAngelicDex;
	int m_iAngelicMag;
	int m_iEnemyKillCount;
	int m_iPKCount;
	int m_iRewardGold;
	int m_iGuildRank;
	int m_iPointCommandType;
	int m_iTotalChar;
	int m_iSuperAttackLeft;
	int m_iAccntYear;
	int m_iAccntMonth;
	int m_iAccntDay;
	int m_iIpYear;
	int m_iIpMonth;
	int m_iIpDay;
	int m_iDownSkillIndex;
	int m_iIlusionOwnerH;
	int	m_iSpecialAbilityTimeLeftSec;
	int m_iSpecialAbilityType;
	int m_iTimeLeftSecAccount;
	int m_iTimeLeftSecIP;
	int m_iCrusadeDuty;
	int m_iLogServerPort;
	int m_iPrevMoveX;
	int m_iPrevMoveY;
	int m_iBlockYear;
	int m_iBlockMonth;
	int m_iBlockDay;
	int m_iConstructionPoint;
	int m_iWarContribution;
	int m_iConstructLocX;
	int m_iConstructLocY;
	int m_iTeleportLocX;
	int m_iTeleportLocY;
	int m_iPartyStatus;
	int m_iCastingMagicType;
	int m_iDamageMoveAmount;
	int m_iItemDropCnt;
	int m_iContributionPrice;
	int m_iHpReliquiaAresden;
	int m_iHpReliquiaElvine;
	int AresdenCount;
	int ElvineCount;
	int AssasainCount;
	int TotalEvent;
	int AresdenWinEvent;
	int ElvineWinEvent;
	int DrawEvents;
	int PartyId;
	int AssasainPoints;

	// No se encripta
	int m_iFrameCount;
	int m_sFPS;
	int m_iCameraShakingDegree;
	int m_iInputX;
	int m_iInputY;
	int m_iPDBGSdivX;
	int m_iPDBGSdivY;
	int m_iNetLagCount;
	int m_iGameServerMode;
	int m_iDrawFlag;
	int m_iApprColor_IE;
	int m_iApprColor_IE2;
	int m_iStatus_IE;
	int _tmp_iChatIndex;
	int _tmp_dx;
	int _tmp_dy;
	int _tmp_dX;
	int _tmp_dY;
	int _tmp_iApprColor;
	int _tmp_iApprColor2;
	int _tmp_iEffectType;
	int _tmp_iEffectFrame;
	int iFocusApprColor;
	int iFocusApprColor2;
	int _tmp_iStatus;
	int iFocuiStatus;
	int m_iPlayerApprColor;
	int m_iPlayerApprColor2;
	int m_iPlayerStatus;
	int m_iExp;

	/* int [X]*/
	int m_iGatePositX[DEF_MAXPORTALS];
	int m_iGatePositY[DEF_MAXPORTALS];
	int AresdenWin[10];
	int ElvineWin[10];
	int Draw[10];
	int LastWin[10];

	/* BOOL */
	// No se encripta
	bool CandySize;
	bool StaggerDmg;
	bool bPlayerShines;
	bool Roofs;
	bool RedSteeps;
	bool BarraNpc;
	bool LowSprites;
	bool m_bSetting;
	bool ShowShin; 
	
	DWORD g_lastUpdateTime; // Última actualización del tiempo
	short g_animationSpeed; // Incremento por frame

	BOOL m_bZoomMap;
	BOOL m_bSoundFlag;
	BOOL m_bSoundStat;
	BOOL m_bMusicStat;
	BOOL m_bDialogTrans;
	BOOL m_bIsWhetherEffect;
	BOOL m_bSuperAttackMode;
	BOOL m_bIsFirstConn;
	BOOL m_bDrawFlagDir;
	BOOL m_bIsCrusadeMode;
	BOOL m_bInputStatus;
	BOOL m_bToggleScreen;
	BOOL m_bIsSpecial;
	BOOL m_bIsF1HelpWindowEnabled;
	BOOL m_bIsPrevMoveBlocked;
	BOOL m_bIsHideLocalCursor;
	BOOL m_bForceAttack;
	BOOL m_bShowFPS;
	BOOL m_bWhisper;
	BOOL m_bShout;
	BOOL m_bItemDrop;
	BOOL m_bIsXmas;
	BOOL m_bUsingSlate;
	BOOL m_bThunder;
	BOOL m_bAfk;
	BOOL bZerk;
	BOOL bInvi;
	BOOL bPfm;
	BOOL bPfa;
	BOOL bShield;
	BOOL FuryWar;
	bool m_bShowTiming;
	bool m_bOldCamera;
	bool m_bQuestHelper;
	BOOL m_bShowQuestData;
	BOOL FuryDayMode;
	BOOL m_bBackSpacePressed;
	BOOL m_bSpecialBarActivated;
	BOOL CaptureTheFlagMode;
	BOOL ElvineFlagStatus;
	BOOL AresdenFlagStatus;
	BOOL AllVsAll;
	BOOL AllEks;
	BOOL AllDkEks;
	BOOL TotalActive;
	BOOL m_bDrawFlagDir2;
	BOOL m_bDrawFlagDir3;
	BOOL m_bGrid;
	BOOL m_bGridTrans;
	BOOL AutoSSEK;
	BOOL m_bCreateScreen;
	BOOL ChangeStat;
	BOOL Disconect;
	BOOL CVCEvent;
	BOOL WWEvent;
	BOOL WorldEvent;
	BOOL WWEventII;
	BOOL WorldEventII;
	BOOL SummonEvent;
	BOOL SummonTimer;
	BOOL Assasain;
	BOOL OpenUpper;
	BOOL OpenDown;
	BOOL SubePanel;
	BOOL ExchangeErrors;
	BOOL ConfirmExchange;
	BOOL MeConfirmed;
	BOOL Glares;
	BOOL Stars;
	BOOL Shadows;
	BOOL Colors;
	BOOL Trees;
	BOOL Steeps;
	BOOL Afks;
	BOOL ShadowsBox;
	BOOL LostShortCut;
	BOOL NewAchivement;
	BOOL ShowTop;
	BOOL Questx2;
	BOOL Questx3;
	BOOL DKEvent;

	/* BOOL [X]*/
	BOOL ComandosActivados[3];

	/* CInt */
	//LogOut Lalito
	//CInt m_dwLogOutCountTime;
	CInt m_dwEncriptCtO;
	CInt m_dwChangeCity;
	CInt m_bCommandAvailable;	
	CInt m_bIsGetPointingMode;
	CInt m_bSkillUsingStatus;
	CInt m_bItemUsingStatus;
	CInt m_bIsPoisoned;
	CInt m_bIsSpecialAbilityEnabled;
	CInt m_bIsTeleportRequested;

	/* CInt [X]*/
	CInt m_bIsDialogEnabled[DEF_MAXDIALOGBOX];
	CInt m_bIsItemEquipped[DEF_MAXITEMS];
	CInt m_bIsItemDisabled[DEF_MAXITEMS];

	WORD  _tmp_wObjectIDOld;

	/* DWORD */
	DWORD G_dwGlobalTime;
	DWORD m_dwCommandTime; 
	DWORD m_dwConnectMode;
	DWORD m_dwTime;
	DWORD m_dwCurTime;
	DWORD m_dwCheckConnTime;
	DWORD m_dwCheckSprTime;
	DWORD m_dwCheckChatTime;
	DWORD reqmobhptime;
	DWORD m_dwCheckSpeed;
	DWORD m_dwFreeCommandTime;
	DWORD m_dwDialogCloseTime;
	DWORD m_dwRestartCountTime;
	DWORD m_dwWOFtime; 
	DWORD m_dwObserverCamTime;
	DWORD m_dwDamagedTime;
	DWORD m_dwSpecialAbilitySettingTime;
	DWORD m_dwCommanderCommandRequestedTime;
	DWORD m_dwTopMsgTime;
	DWORD m_dwEnvEffectTime;
	DWORD m_dwMonsterEventTime;
	DWORD m_dwFPStime;
	DWORD m_dwAuraTime;
	//DWORD m_dwChangeCity;
	DWORD m_dwTimingCountTime;
	DWORD m_dwSpecialBarTimer;
	DWORD m_dwTimeScreen;
	DWORD dwPanel;
	DWORD TimeMagicSelected;
	DWORD TimeSCSelected;

	/* unsigned char */
	unsigned char m_iStr;
	unsigned char m_iInt;
	unsigned char m_iVit;
	unsigned char m_iDex;
	unsigned char m_iMag;
	unsigned char m_iCharisma;
	unsigned char m_iTopMsgLastSec;
	unsigned char m_cInputMaxLen;

	/* unsigned int */
	//unsigned int Coins;
	unsigned int m_iCoins;
	unsigned int HourPlaying;
	unsigned int MobsKilled;
	unsigned int TotalDeath;
	unsigned int TotalClientKilled;
	unsigned int TotalQuestComplete;

	/* WORD */
	WORD m_wCommObjectID;
	WORD m_wEnterGameType;
	WORD _tmp_wObjectID;
	WORD wFocusObjectID;

	/* WORD  [X]*/
	WORD m_wR[45];
	WORD m_wG[45];
	WORD m_wB[45];
	WORD m_wWR[45];
	WORD m_wWG[45];
	WORD m_wWB[45];

	/* Varios */
	RECT m_rcPlayerRect, m_rcBodyRect;
	HANDLE m_hPakFile;	

	class Minimap {
	public:
		Minimap() {}
		~Minimap() {}

		struct Unit {
			Unit() {}
			short x, y;
			int id;
			DWORD time;
		};

		std::vector<Unit> list;
		void Clear();
		void Remove(int handle);
	} m_minimap;
};

#endif // !defined(AFX_GAME_H__0089D9E3_74E6_11D2_A8E6_00001C7030A6__INCLUDED_)

