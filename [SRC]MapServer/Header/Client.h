#if !defined(AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_)
#define AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include "XSocket.h"
#include "Item.h"
#include "GuildsMan.h"
#include "XSocketUDP.h"
#include "Magic.h"
#include "../General/GlobalDef.h"

using namespace std;

enum MissionStatus {
	Inactive,
	Active
};


typedef signed char i8;
#define DEF_CLIENTSOCKETBLOCKLIMIT	15

#define DEF_MSGBUFFERSIZE	200000//1
#define DEF_MAXITEMS		50
#define DEF_MAXBANKITEMS	200
#define DEF_MAXMARKETITEMS	100
#define DEF_MAXBANKITEMPAGE	20
#define DEF_MAXGUILDSMAN	128
#define DEF_MAXSELLITEMS	19

#define	DEF_MAXMAGICTYPE	100
#define DEF_MAXSKILLTYPE	60
#define DEF_MAXACHIVEMENTS	30

#define DEF_MAXPARTYMEMBERS	15

#define DEF_SPECABLTYTIMESEC	1200

#define DEF_PARTYSTATUS_NULL		0
#define DEF_PARTYSTATUS_PROCESSING	1
#define DEF_PARTYSTATUS_CONFIRM		2

#define DEF_MAXITEMSEXCHANGE		20

enum Citizenship
{
	Neutral,
	Aresden,
	Elvine,
	GM
};

class CClient  
{
public:
	SOCKET serverSocket;
	sockaddr_in serverAddr;
	sockaddr_in clientAddr;
	int clientAddrLen;


	char m_rejectedMoves;
	uint32 m_moveTime[7];
	uint32 m_moveTurn;
	uint32 m_runTime[7];
	uint32 m_runTurn;

	bool running;
	void EnableItem(i8 handle);
	void read_quest_data();
	void save_quest_data();
	void send_charquest_data();
	void req_dismiss_quest(char * data);
	void req_pause_quest(char * data);
	void req_active_quest(char * data);
	void active_quest(int index);
	void req_reward_quest(char * data);
	void reward_quest(int index, short rew);
	void pause_quest(int index);
	void remove_quest(int index);
	void CreateItem(char * itemname);
	void send_charquest_data_byindex(int index);
	void initializeWinsock();
	void bindSocket(int port);
	void receiveMessageFromClient(char* buffer, int bufferLen);
	void startServer(int port);
	void stopServer();
	void stop();
	void run();
	void StartListening(XSocketUDP& socketUDP, bool& running);
	void InitUDP(const std::string& address, unsigned short port);
	void InitUDP();
	void SendUDPMessage(const Message& msg, const std::string& targetAddress, unsigned short targetPort);
	void ReceiveUDPMessage();
	BOOL bCreateNewParty();
	BOOL m_bIsPlayerCivil;
	CClient(HWND hWnd, int handle);
	short sResolution;
	int client;
	bool m_bUdpSocket;
	//new dynamic resolution prlucas
	int res_sX;
	int res_sY;
	int res_tilX;
	int res_tilY;

	//rank system hbarg

	short m_sRankLevel;
	int m_iRankPoints;
	int m_iRankExp;
	int m_iMaxRankExp;

	short m_sCandyInsert;
	short m_sCandyDelete;
	short m_sNoDrop;

	short m_sRankAddStr;
	short m_sRankAddVit;
	short m_sRankAddInt;
	short m_sRankAddDex;
	short m_sRankAddMag;

	//heaton enchanting 2024
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

	//HeatoN rebirth 2024
	int m_iRebirthLevel;
	int m_sRebirthStatus;
	short m_sRebirthEnabled;
	int m_iBackRebirthLevel;

	int m_iBackExp;
	int m_iBackStatsPoint;
	int m_iBackStr;
	int m_iBackVit;
	int m_iBackInt;
	int m_iBackDex;
	int m_iBackChr;
	int m_iBackMag;
	int m_iBackLevel;

	//heaton prevent lammers quest and exp
	int m_iCheckCount;

	//tolerancia speed hack move
	int m_iFailedAttempts;

	void AsignSocket(HWND hWnd);
	BOOL m_bRealClient;
	virtual ~CClient();
	BOOL m_bActivarComandos;
    BOOL bPHackWPE;
	BOOL m_bIsOnlyClientAvailable;
	BOOL m_bIsAdminOrderGoto;
	int  m_iAngelicStr; 
	int  m_iAngelicInt;
	int  m_iAngelicDex;
	int  m_iAngelicMag;
	char m_cCharName[12];
	char m_cAccountName[12];
	char m_cAccountPassword[12];
	BOOL  m_bIsInitComplete;
	BOOL  m_bIsMsgSendAvailable;
	BOOL  m_bIsCheckingWhisperPlayer;

	//heaton fake limbo
	bool m_bForceDisconnect;
	bool m_bLimbo;

	char  m_cMapName[12];
	char  m_cMapIndex;
	short m_sX, m_sY;
	
	char  m_cGuildName[22];
	char  m_cLocation[12];
	int   m_iGuildRank;
	int   m_iGuildGUID;
	short TotalGuildMembers;
	
	char  m_cDir;
	short m_sType;
	short m_sOriginalType;
	short m_sAppr1;
	short m_sAppr2;
	short m_sAppr3;
	short m_sAppr4;
	int   m_iApprColor;	
	// New Colors -> ZeroEoyPnk
	int   m_iApprColor2;
	int   m_iStatus;

	bool m_sNoCheckSpeed;
	short m_sCheckCount;

	DWORD m_dwTime, m_dwHPTime, m_dwMPTime, m_dwSPTime, m_dwAutoSaveTime, m_dwHungerTime;

	// Player
	char m_cSex, m_cSkin, m_cHairStyle, m_cHairColor, m_cUnderwear;

	short  m_iHP;						// Hit Point
	int  m_iHPstock;
	int  m_iHPStatic_stock;
	short  m_iMP;
	short  m_iSP;
	int  m_iExp, m_iNextLevelExp;
	BOOL m_bIsKilled;

	int  m_iDefenseRatio;		// Defense Ratio
	int  m_iHitRatio;			// Hit Ratio

	int  m_iDamageAbsorption_Armor[DEF_MAXITEMEQUIPPOS];		// Damage
	int  m_iDamageAbsorption_Shield;	// Parrying Damage

	int m_iLevel;
	unsigned char  m_iStr, m_iInt, m_iVit, m_iDex, m_iMag, m_iCharisma;
	int  m_iLU_Pool;
	int  m_iLuck; 
	int  m_iEnemyKillCount, m_iPKCount, m_iRewardGold, m_iEnemyKillTotalCount, m_iCityFriendKilled, AssasainPoint;
	int  m_iCurWeightLoad;
	char m_cHeroBonus;

	char m_cSide;
    BOOL m_b5MinutosFinales;
	DWORD m_dwHeldenianGUID;
	//No recall por 10 segundos - ZeroEoyPnk
	BOOL m_bRecall;
	DWORD RecallTime;
	char m_cAttackDiceThrow_SM;
	char m_cAttackDiceRange_SM;
	char m_cAttackDiceThrow_L;
	char m_cAttackDiceRange_L;
	char m_cAttackBonus_SM;
	char m_cAttackBonus_L;

	struct st_charquest
	{
		st_charquest()
		{
			m_sParentQuestID = 0;
			m_iKills = 0;
			m_iMaxKills = 0;
			m_iRewardContribution = 0;
			m_iRewardReputation = 0;
			m_sStatus = 0;
			m_sCompleted = 0;
			ZeroMemory(m_cNpcName, sizeof(m_cNpcName));
			ZeroMemory(m_cMapName, sizeof(m_cMapName));
			ZeroMemory(m_cRealMapName, sizeof(m_cRealMapName));
			ZeroMemory(m_cQuestReward1, sizeof(m_cQuestReward1));
			ZeroMemory(m_cQuestReward2, sizeof(m_cQuestReward2));
			ZeroMemory(m_cQuestReward3, sizeof(m_cQuestReward3));
		}
		short m_sParentQuestID;
		char  m_cNpcName[21];
		char  m_cMapName[12];
		char  m_cRealMapName[12];
		int   m_iKills;
		int	  m_iMaxKills;
		int   m_iRewardContribution;
		int   m_iRewardReputation;
		char  m_cQuestReward1[21];
		char  m_cQuestReward2[21];
		char  m_cQuestReward3[21];
		short m_sStatus;
		short m_sCompleted;
	};

	vector<st_charquest> vec_charquest;

	class CItem * m_pItemList[DEF_MAXITEMS];
	POINT m_ItemPosList[DEF_MAXITEMS];
	class CItem * m_pItemInBankList[DEF_MAXBANKITEMS];
	class CItem * m_pMarketItem[DEF_MAXMARKETITEMS];
	
	BOOL  m_bIsItemEquipped[DEF_MAXITEMS];
	short m_sItemEquipmentStatus[DEF_MAXITEMEQUIPPOS];
	char  m_cArrowIndex;

	char           m_cMagicMastery[DEF_MAXMAGICTYPE];
	unsigned char  m_cSkillMastery[DEF_MAXSKILLTYPE]; // v1.4

	int   m_iSkillSSN[DEF_MAXSKILLTYPE];
	BOOL  m_bSkillUsingStatus[DEF_MAXSKILLTYPE];
	int   m_iSkillUsingTimeID[DEF_MAXSKILLTYPE]; //v1.12

	char  m_cMagicEffectStatus[DEF_MAXMAGICEFFECTS];
	
	
	int   m_iWhisperPlayerIndex;
	char  m_cWhisperPlayerName[12];
	char  m_cProfile[256];

	int   m_iHungerStatus;

	DWORD m_dwWarBeginTime;
	BOOL  m_bIsWarLocation;

	BOOL  m_bIsPoisoned;
	int   m_iPoisonLevel;
	DWORD m_dwPoisonTime;
	
	int   m_iPenaltyBlockYear, m_iPenaltyBlockMonth, m_iPenaltyBlockDay;

	int   m_iFightzoneNumber , m_iReserveTime, m_iFightZoneTicketNumber ; 

	class XSocket * m_pXSock;
	XSocketUDP* m_pXSocketUDP;

	int   m_iAdminUserLevel;
	int   m_iRating;

	int   m_iTimeLeft_ShutUp;
	int   m_iTimeLeft_Rating;
	int   m_iTimeLeft_ForceRecall;
	int   m_iTimeLeft_FirmStaminar;

	BOOL  m_bIsOnServerChange;

	int   m_iExpStock;
	DWORD m_dwExpStockTime;

	int   m_iAutoExpAmount;
	DWORD m_dwAutoExpTime;

	DWORD m_dwRecentAttackTime;
	DWORD m_dwLastActionTime;

	int   m_iAllocatedFish;
	int   m_iFishChance;
	
	char  m_cIPaddress[21];
	char  m_cIP2[21];
	BOOL  m_bIsSafeAttackMode;

	BOOL  m_bIsOnWaitingProcess;
	
	int   m_iSuperAttackLeft;
	int   m_iSuperAttackCount;

	short m_sUsingWeaponSkill;

	int   m_iManaSaveRatio;
	
	BOOL  m_bIsLuckyEffect;
	int   m_iSideEffect_MaxHPdown;

	int   m_iComboAttackCount;
	int   m_iDownSkillIndex;

	int   m_iMagicDamageSaveItemIndex;

	short m_sCharIDnum1, m_sCharIDnum2, m_sCharIDnum3;

	int   m_iPartyRank;
	int   m_iPartyMemberCount;
	int   m_iPartyGUID;
	struct {
		int  iIndex;
		char cName[11];

	} m_stPartyMemberName[DEF_MAXPARTYMEMBERS];

	int   m_iAbuseCount;
	
	int m_dwLogoutHackCheck;//LALORAMOS

	BOOL  m_bIsExchangeMode;			// Is In Exchange Mode? 
	int   m_iExchangeH;					// Client ID to Exchanging with 
	char  m_cExchangeName[11];			// Name of Client to Exchanging with 
	char  m_cExchangeItemName[DEF_MAXITEMSEXCHANGE][21];	// Name of Item to exchange 

	char  m_cExchangeItemIndex[DEF_MAXITEMSEXCHANGE];		// ItemID to Exchange
	int   m_iExchangeItemAmount[DEF_MAXITEMSEXCHANGE];		// Ammount to exchange with

	BOOL  m_bIsExchangeConfirm;			// Has the user hit confirm? 
	int	  iExchangeCount;				//Keeps track of items which are on list

	int   m_iQuest;
	int   m_iQuestID;
	int	  m_iItemQuest;
	int	  m_iItemQuestID;
	int   m_iStatedQuest;
	int   m_iStatedQuestID;
	int   m_iAskedQuest;
	int   m_iCurQuestCount;
	int   m_iItemQuestCount;
	int   m_iStatedQuestCount;
	
	int   m_iQuestRewardType;
	int   m_iQuestRewardAmount;
	int   m_iItemQuestRewardType;
	int   m_iStatedQuestRewardType;

	int   m_iContribution;

	BOOL  m_bQuestMatchFlag_Loc;
	BOOL  m_bIsQuestCompleted[3];
	
	int   m_iCustomItemValue_Attack;
	int   m_iCustomItemValue_Defense;

	int   m_iMinAP_SM;
	int   m_iMinAP_L;

	int   m_iMaxAP_SM;
	int   m_iMaxAP_L;

	BOOL  m_bIsNeutral;
	BOOL  m_bIsObserverMode;
	
	//Auto Save ZeroEoyPnk
	BOOL  b_AutoSaveTime;

	int   m_iSpecialEventID;
	int   m_iSpecialWeaponEffectType;
	int   m_iSpecialWeaponEffectValue;
	
	int   m_iAddHP, m_iAddSP, m_iAddMP; 
	int   m_iAddAR, m_iAddPR, m_iAddDR;
	int   m_iAddMR, m_iAddAbsPD, m_iAddAbsMD; 
	int   m_iAddCD, m_iAddExp, m_iAddGold;

	//ZeroEoyPnk - New Hit and Dr system
/*	int  m_iDRHitRatio;			// Hit Ratio
	int  m_iDRDefenseRatio;		// Defense Ratio*/

	int   m_iAddTransMana, m_iAddChargeCritical;

	int   m_iAddResistMagic;
	int	  m_iAddMagicHitting;
	int   m_iAddPhysicalDamage;
	int   m_iAddMagicalDamage;	

	int   m_iAddAbsAir;
	int   m_iAddAbsEarth;
	int   m_iAddAbsFire;
	int   m_iAddAbsWater;
	
	int   m_iLastDamage;

	int   m_iMoveMsgRecvCount, m_iAttackMsgRecvCount, m_iRunMsgRecvCount, m_iSkillMsgRecvCount;
	DWORD m_dwMoveLAT, m_dwRunLAT, m_dwAttackLAT;

	int   m_iSpecialAbilityTime;
	BOOL  m_bIsSpecialAbilityEnabled;
	DWORD m_dwSpecialAbilityStartTime;
	int   m_iSpecialAbilityLastSec;

	int   m_iSpecialAbilityType;
	int   m_iSpecialAbilityEquipPos;
	int   m_iAlterItemDropIndex;

	int   m_iWarContribution;

	DWORD m_dwSpeedHackCheckTime;
	int   m_iSpeedHackCheckExp;
	
	DWORD m_dwInitCCTimeRcv;
	DWORD m_dwInitCCTime;

	char  m_cLockedMapName[12];
	int   m_iLockedMapTime;
	int   m_iDeadPenaltyTime;

	int   m_iCrusadeDuty;
	DWORD m_dwCrusadeGUID;

	struct {
		char cType;
		char cSide;
		short sX, sY;
	} m_stCrusadeStructureInfo[DEF_MAXCRUSADESTRUCTURES];
	int m_iCSIsendPoint;

	char m_cSendingMapName[12];
	BOOL m_bIsSendingMapStatus;

	int  m_iConstructionPoint;

	char m_cConstructMapName[12];
	int  m_iConstructLocX, m_iConstructLocY;

	DWORD m_dwFightzoneDeadTime;

	BOOL m_bIsBankModified ;

	DWORD m_dwCharID ;

	int m_iPartyID;
	int m_iPartyStatus;
	int m_iReqJoinPartyClientH;
	char m_cReqJoinPartyName[12];

	short m_iGizonItemUpgradeLeft;

	DWORD m_dwAttackFreqTime, m_dwMagicFreqTime, m_dwMoveFreqTime;
	BOOL m_bIsMoveBlocked;
	BOOL m_bIsAttackModeChange;
	int  m_iIsOnTown;
	BOOL m_bIsOnShop;
	BOOL m_bIsHunter;
	BOOL m_bIsOnTower;
	BOOL m_bIsOnWarehouse;
	BOOL m_bIsInBuilding;
	BOOL m_bIsOnCombatantZone;
	BOOL m_bIsImpossibleZone;

	DWORD m_dwWarmEffectTime;
	
	BOOL m_bInhibition; //v2.95 31/05/06 18:39 Inhibition-Casting 
	BOOL m_bIsBeingResurrected; //v2.95 02/06/06 16:18 Resurrection 
	BOOL m_iActivity;

	BOOL AdminChat;

	bool m_bManaMe;
	DWORD m_dwManaMe;

	DWORD Save;
	//Reflex Effect
//	BOOL Reflex_PA, Reflex_MA, HP_REC, MP_REC;
//	int Damage_MA, Damage_PA, DEF_HP, DEF_MP;

	int GetWearedItemIndex(short ItemID); // Retorna el indice del item en caso de que lo tenga puesto
	short GetWearedItemIndex(int EquipPoss);
	bool WearingItem(short ItemID);
	bool WearingItem(short ItemID, int EquipPoss);
//	int GetItemIndex(short ItemID); // Retorna el indice en el arreglo de items del item con ese ID. Error: -1
//	int GetItemLastIndex(short ItemID); // Retorna el indice en el arreglo de items del ultimo con ese ID. Error: -1
	int RecallCoordX, RecallCoordY;
	DWORD TimeCoord;

	short WHPage;

	BOOL NoClear;
	DWORD TimerMove;
	short Moves;
	short Active;

	DWORD KilledClientAFK;
	BOOL Assasain;
	BOOL FirstPosition;

	int m_iCoins;

	struct 
	{
		int Contador;
		BOOL Complete;
	} Achivements[DEF_MAXACHIVEMENTS];

	bool requestrevive;
	unsigned int TotalMobsKilled;
	unsigned int TotalDeath;
	unsigned int TotalClientKilled;
	int TotalQuestCompleted;
	char TotalStructuresKilled;

	int HourPlaying, MinPlaying, EventMinPlaying;
	DWORD LastTime;

	int RndEventID;

	DWORD ActiveAvA;
	BOOL CheckProcess;

	class CItem * m_pEventItemList[DEF_MAXITEMS];
	POINT m_EventItemPosList[DEF_MAXITEMS];
	BOOL  m_bIsEventItemEquipped[DEF_MAXITEMS];

	BOOL Heredado;

	short Event_Killed, Event_Death, EventLevel;
	int Temporal_Rating;

	DWORD TimeFlagCarrier;

	short EventID;

	BOOL NoDelete, LogOut;
	DWORD TimeToDelete;

	int m_iLastPing;

	DWORD dw_LastMsg;

	// New System -> Market Place -> HB Arg v4.4
	BOOL OpenSell;
	//class CItem * m_pItemInSellList[DEF_MAXSELLITEMS];
	short SellType;
	short BuyTo;

	short TotalItemSell;

	struct {
		int sIndex;
		DWORD m_wPrice;
	} MarketItem[DEF_MAXSELLITEMS];
	void Notify(WORD msg, DWORD sV1 = 0, DWORD sV2 = 0, DWORD sV3 = 0, char * pString = nullptr, DWORD sV4 = 0, DWORD sV5 = 0, DWORD sV6 = 0, DWORD sV7 = 0, DWORD sV8 = 0, DWORD sV9 = 0, char * pString2 = nullptr, char cValueA = 0);
	void Send(int h, WORD msg, DWORD sV1 = 0, DWORD sV2 = 0, DWORD sV3 = 0,
		char * pString = 0, DWORD sV4 = 0, DWORD sV5 = 0,
		DWORD sV6 = 0, DWORD sV7 = 0, DWORD sV8 = 0, DWORD sV9 = 0, char * pString2 = 0);
	bool IsInsideDM();
	bool IsLimboMap();
	bool IsLocation(char* location);
	bool IsInMap(char * mapname);
	void ResetClient();
	int GetStr();
	int GetMag();
	int GetVit();
	int GetDex();
	int GetInt();
	int getRebirthStats();
	int GetMaxMP() const;
	int GetMaxSP() const;
	bool IsInsideCh();
	int GetStr()		const { return m_iStr + m_iAngelicStr; }
	int GetMag()		const { return m_iMag + m_iAngelicMag; }
	int GetInt()		const { return m_iInt + m_iAngelicInt; }
	int GetDex()		const { return m_iDex + m_iAngelicDex; }
	int GetVit()		const { return m_iVit; }
	//timer sg
	DWORD dw_LastSG;
};

#endif // !defined(AFX_CLIENT_H__39CC7700_789F_11D2_A8E6_00001C7030A6__INCLUDED_)
