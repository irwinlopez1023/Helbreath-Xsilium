#include <windows.h>
#include <direct.h> 
#include <string>
#include "../Header/Client.h"
#include "../Header/Game.h"
#include <iostream>
#include <sstream> // Para construir cadenas de texto
#include <cstring> // Para memset
#include <thread> // Para std::thread


extern CMapServer * c_map;
extern char            G_cTxt[512];
#pragma warning(disable : 4996)



CClient::CClient(HWND hWnd, int handle)  {
	client = handle;
	m_pXSock = nullptr;
	m_pXSock = new class XSocket(hWnd, DEF_CLIENTSOCKETBLOCKLIMIT);
	m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
	/*const int PORT = 8888 + client;
	startServer(PORT);

	std::thread serverThread([this]() {
		run();
	});

	serverThread.detach();*/

	register int i;

	client = handle;
	m_pXSock = NULL;
	m_pXSock = new class XSocket(hWnd, DEF_CLIENTSOCKETBLOCKLIMIT);
	m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
	//m_pXSocketUDP = new XSocketUDP();
	m_bRealClient = false;

	ZeroMemory(m_cProfile, sizeof(m_cProfile));
	strcpy(m_cProfile, "__________");
	m_bUdpSocket = false;
	//sResolution = C800x600;
	sResolution = C1024x768; //Cambio de resolucion xsilium?

	//heaton fake limbo
	m_bForceDisconnect = false;
	m_bLimbo = false;

	//new dynamic resolution prlucas
	res_sX = 0;
	res_sY = 0;
	res_tilX = 0;
	res_tilY = 0;

	m_sCandyInsert = 0;
	m_sCandyDelete = 0;
	m_sNoDrop = 0;

	//rank system hbarg

	m_sRankLevel = 0;
	m_iRankPoints = 0;
	m_iRankExp = 0;

	m_sRankAddStr = 0;
	m_sRankAddVit = 0;
	m_sRankAddInt = 0;
	m_sRankAddDex = 0;
	m_sRankAddMag = 0;

	//heaton 2024
	shard_light = 0;
	shard_strong = 0;

	fragment_hp = 0;
	fragment_dr = 0;
	fragment_hprec = 0;
	fragment_sprec = 0;
	fragment_mprec = 0;
	fragment_mr = 0;
	fragment_pa = 0;
	fragment_ma = 0;
	fragment_exp = 0;

	//HeatoN rebirth 2024
	m_iRebirthLevel = 0;
	m_sRebirthStatus = 0;
	m_sRebirthEnabled = 0;
	m_iBackRebirthLevel = 0;

	m_iBackExp = 0;
	m_iBackStatsPoint = 0;
	m_iBackLevel = 0;
	m_iBackStr = 0;
	m_iBackVit = 0;
	m_iBackInt = 0;
	m_iBackDex = 0;
	m_iBackChr = 0;
	m_iBackMag = 0;

	//heaton prevent lammers quest and exp
	m_iCheckCount = 0;

	//dm
	requestrevive = false;

	//tolerancia speed hack move
	m_iFailedAttempts = 0;

	vec_charquest.clear();

	//AntiHack
	m_sNoCheckSpeed = false;
	m_sCheckCount = 0;
	ZeroMemory(m_cCharName, sizeof(m_cCharName));
	ZeroMemory(m_cAccountName, sizeof(m_cAccountName));
	ZeroMemory(m_cAccountPassword, sizeof(m_cAccountPassword));

	ZeroMemory(m_cGuildName, sizeof(m_cGuildName));
	ZeroMemory(m_cLocation, sizeof(m_cLocation));
	ZeroMemory(m_cMapName, sizeof(m_cMapName));

	strcpy(m_cLocation, "NONE");
	m_iGuildRank = -1;
	m_iGuildGUID = -1;
	TotalGuildMembers = -1;

	m_bIsInitComplete = FALSE;
	m_bActivarComandos = FALSE;
	m_iEnemyKillCount = 0;
	m_iEnemyKillTotalCount = 0;
	m_iPKCount = 0;
	m_iCityFriendKilled = 0;
	AssasainPoint = 0;
	m_iRewardGold = 0;
	m_iCurWeightLoad = 0;
	m_bIsOnlyClientAvailable = FALSE;
	m_bIsSafeAttackMode = FALSE;
	//No recall por 10 segundos - ZeroEoyPnk
	m_bRecall = true;

	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++)
		m_sItemEquipmentStatus[i] = -1;

	for (i = 0; i < DEF_MAXITEMS; i++) {
		m_pItemList[i] = NULL;
		m_ItemPosList[i].x = 40;
		m_ItemPosList[i].y = 30;
		m_bIsItemEquipped[i] = FALSE;
	}

	for (i = 0; i < DEF_MAXITEMS; i++) {
		m_pEventItemList[i] = NULL;
		m_EventItemPosList[i].x = 40;
		m_EventItemPosList[i].y = 30;
		m_bIsEventItemEquipped[i] = FALSE;
	}

	m_cArrowIndex = -1;

	for (i = 0; i < DEF_MAXBANKITEMS; i++) {
		m_pItemInBankList[i] = NULL;
	}

	//heaton market
	for (i = 0; i < DEF_MAXMARKETITEMS; i++) {
		m_pMarketItem[i] = NULL;
	}

	// Magic - Skill 
	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
		m_cMagicMastery[i] = NULL;

	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
		m_cSkillMastery[i] = NULL;

	for (i = 0; i < DEF_MAXACHIVEMENTS; i++)
	{
		Achivements[i].Contador = 0;
		Achivements[i].Complete = FALSE;
	}

	for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
		m_bSkillUsingStatus[i] = FALSE;
		m_iSkillUsingTimeID[i] = NULL;
	}

	for (int i = 0; i < 7; i++)
		m_moveTime[i] = 540;
	m_moveTurn = 0;
	for (int i = 0; i < 7; i++)
		m_runTime[i] = 300;
	m_runTurn = 0;

	m_cMapIndex = -1;
	m_sX = -1;
	m_sY = -1;
	m_cDir = 5;
	m_sType = 0;
	m_sOriginalType = 0;
	m_sAppr1 = 0;
	m_sAppr2 = 0;
	m_sAppr3 = 0;
	m_sAppr4 = 0;
	m_iApprColor = 0; // v1.4
	m_iApprColor2 = 0; // New Colors -> ZeroEoyPnk
	m_iStatus = 0;
	m_iLU_Pool = 0;
	bPHackWPE = FALSE;
	m_cSex = 0;
	m_cSkin = 0;
	m_cHairStyle = 0;
	m_cHairColor = 0;
	m_cUnderwear = 0;

	m_cAttackDiceThrow_SM = 0;
	m_cAttackDiceRange_SM = 0;
	m_cAttackDiceThrow_L = 0;
	m_cAttackDiceRange_L = 0;
	m_cAttackBonus_SM = 0;
	m_cAttackBonus_L = 0;

	m_cSide = 0;
	m_iHitRatio = 0;
	m_iDefenseRatio = 0;
	m_iLastPing = 0;

	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++) m_iDamageAbsorption_Armor[i] = 0;
	m_iDamageAbsorption_Shield = 0;

	m_iHPstock = 0;
	m_iHPStatic_stock = 0;
	m_bIsKilled = FALSE;

	for (i = 0; i < DEF_MAXMAGICEFFECTS; i++)
		m_cMagicEffectStatus[i] = 0;

	m_iWhisperPlayerIndex = -1;
	ZeroMemory(m_cWhisperPlayerName, sizeof(m_cWhisperPlayerName));

	m_iHungerStatus = 100;
	m_bIsWarLocation = FALSE;

	m_bIsPoisoned = FALSE;
	m_iPoisonLevel = NULL;

	m_iAdminUserLevel = 0;
	m_iRating = 0;
	m_iTimeLeft_ShutUp = 0;
	m_iTimeLeft_Rating = 0;
	m_iTimeLeft_ForceRecall = 0;
	m_iTimeLeft_FirmStaminar = 0;

	m_bIsOnServerChange = FALSE;

	m_iExpStock = 0;

	m_iAllocatedFish = NULL;
	m_iFishChance = 0;

	ZeroMemory(m_cIPaddress, sizeof(m_cIPaddress));
	ZeroMemory(m_cIP2, sizeof(m_cIP2));
	m_b5MinutosFinales = FALSE;
	m_bIsOnWaitingProcess = FALSE;
	m_iSuperAttackLeft = 0;
	m_iSuperAttackCount = 0;

	m_sUsingWeaponSkill = 5;

	m_iManaSaveRatio = 0;
	m_iAddResistMagic = 0;
	m_iAddMagicHitting = 0;
	m_iAddPhysicalDamage = 0;
	m_iAddMagicalDamage = 0;
	m_bIsLuckyEffect = FALSE;
	m_iSideEffect_MaxHPdown = 0;

	m_iAddAbsAir = 0;
	m_iAddAbsEarth = 0;
	m_iAddAbsFire = 0;
	m_iAddAbsWater = 0;
	m_iAngelicStr = 0;
	m_iAngelicInt = 0;
	m_iAngelicDex = 0;
	m_iAngelicMag = 0;
	m_iComboAttackCount = 0;
	m_iDownSkillIndex = -1;

	m_iMagicDamageSaveItemIndex = -1;

	m_sCharIDnum1 = m_sCharIDnum2 = m_sCharIDnum3 = 0;

	m_iPartyRank = -1; // v1.42
	m_iPartyMemberCount = 0;
	m_iPartyGUID = 0;

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
		m_stPartyMemberName[i].iIndex = 0;
		ZeroMemory(m_stPartyMemberName[i].cName, sizeof(m_stPartyMemberName[i].cName));
	}

	m_iAbuseCount = 0;
	m_bIsExchangeMode = FALSE;
	m_iFightZoneTicketNumber = m_iFightzoneNumber = m_iReserveTime = 0;
	m_iPenaltyBlockYear = m_iPenaltyBlockMonth = m_iPenaltyBlockDay = 0;

	m_iExchangeH = NULL;
	for (i = 0; i < DEF_MAXITEMSEXCHANGE; i++) {
		m_cExchangeItemIndex[i] = -1;
		m_iExchangeItemAmount[i] = 0;
	}
	ZeroMemory(m_cExchangeName, sizeof(m_cExchangeName));
	ZeroMemory(m_cExchangeItemName, sizeof(m_cExchangeItemName));
	m_bIsExchangeConfirm = FALSE;

	m_iQuest = NULL;
	m_iQuestID = NULL;
	m_iAskedQuest = NULL;
	m_iCurQuestCount = NULL;
	m_iItemQuest = NULL;
	m_iItemQuestID = NULL;
	m_iItemQuestCount = NULL;
	m_iItemQuestRewardType = NULL;
	m_iStatedQuest = NULL;
	m_iStatedQuestID = NULL;
	m_iStatedQuestCount = NULL;
	m_iStatedQuestRewardType = NULL;

	m_iQuestRewardType = NULL;
	m_iQuestRewardAmount = NULL;

	m_iContribution = NULL;
	m_bQuestMatchFlag_Loc = FALSE;

	m_bIsQuestCompleted[0] = m_bIsQuestCompleted[1] = m_bIsQuestCompleted[2] = FALSE;

	m_bIsNeutral = FALSE;
	m_bIsObserverMode = FALSE;

	m_iSpecialEventID = 200081;

	m_iSpecialWeaponEffectType = 0;
	m_iSpecialWeaponEffectValue = 0;

	m_iAddHP = m_iAddSP = m_iAddMP = 0;
	m_iAddAR = m_iAddPR = m_iAddDR = 0;
	m_iAddMR = 0;
	m_iAddAbsPD = m_iAddAbsMD = 0;
	m_iAddCD = m_iAddExp = m_iAddGold = 0;

	m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;		// DEF_SPECABLTYTIMESEC ÃÊ¸¶´Ù ÇÑ¹ø¾¿ Æ¯¼ö ´É·ÂÀ» ¾µ ¼ö ÀÖ´Ù.
	m_iSpecialAbilityType = NULL;
	m_bIsSpecialAbilityEnabled = FALSE;
	m_iSpecialAbilityLastSec = 0;

	m_iSpecialAbilityEquipPos = 0;

	m_iMoveMsgRecvCount = 0;
	m_iAttackMsgRecvCount = 0;
	m_iRunMsgRecvCount = 0;
	m_iSkillMsgRecvCount = 0;

	m_iActivity = TRUE;

	m_iAlterItemDropIndex = -1;

	m_iAutoExpAmount = 0;
	m_iWarContribution = 0;

	m_dwMoveLAT = m_dwRunLAT = m_dwAttackLAT = 0;

	m_dwInitCCTimeRcv = 0;
	m_dwInitCCTime = 0;

	ZeroMemory(m_cLockedMapName, sizeof(m_cLockedMapName));
	strcpy(m_cLockedMapName, "NONE");
	m_iLockedMapTime = NULL;
	m_iDeadPenaltyTime = NULL;

	m_iCrusadeDuty = NULL;
	m_dwCrusadeGUID = NULL;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++) {
		m_stCrusadeStructureInfo[i].cType = NULL;
		m_stCrusadeStructureInfo[i].cSide = NULL;
		m_stCrusadeStructureInfo[i].sX = NULL;
		m_stCrusadeStructureInfo[i].sY = NULL;
	}
	m_iCSIsendPoint = NULL;

	m_bIsSendingMapStatus = FALSE;
	ZeroMemory(m_cSendingMapName, sizeof(m_cSendingMapName));

	m_iConstructionPoint = NULL;

	ZeroMemory(m_cConstructMapName, sizeof(m_cConstructMapName));
	m_iConstructLocX = m_iConstructLocY = -1;

	m_dwFightzoneDeadTime = NULL;

	m_iPartyID = NULL;
	m_iPartyStatus = DEF_PARTYSTATUS_NULL;

	m_iReqJoinPartyClientH = NULL;
	ZeroMemory(m_cReqJoinPartyName, sizeof(m_cReqJoinPartyName));

	m_dwLastActionTime = NULL;
	m_bIsCheckingWhisperPlayer = FALSE;
	m_bIsBankModified = FALSE;

	m_iGizonItemUpgradeLeft = 0;

	m_dwAttackFreqTime = m_dwMagicFreqTime = m_dwMoveFreqTime = NULL; // v2.171
	m_bIsMoveBlocked = FALSE; // v2.171
	m_bIsAttackModeChange = FALSE; // v2.172 2002-7-2
	m_iIsOnTown = FALSE;
	m_bIsOnShop = FALSE; // v2.182 2002-11-15 »ç°í ÆÈ¼ö ÀÖ´Â °÷ÀÎÁö Ã¼Å©ÇÏ´Â º¯¼ö Ãß°¡ 
	m_bIsOnTower = FALSE; // v2.20 2002-12-23 ¸¶¹ýÀ» ¹è¿ï¼ö ÀÖ´Â°÷ÀÎÁö Ã¼Å© ÇÑ´Ù.
	m_bIsOnWarehouse = FALSE; // v2.20 2002-12-23 Ã¢°íÀÎÁö Ã¼Å© ÇÑ´Ù.
	m_bIsHunter = FALSE; // v2.182 2002-11-15 »ç³É±ºÀÎÁö Ã¼Å©ÇÏ´Â º¯¼ö Ãß°¡ 
	m_dwWarmEffectTime = NULL; // v2.172 
	m_bIsInBuilding = FALSE;
	m_bIsOnCombatantZone = FALSE;
	m_cHeroBonus = 0;	// v2.23 18/02/06 17:15
	m_bInhibition = FALSE; // v2.95 31/05/06 18:40 Inhibition-Casting 
	m_bIsBeingResurrected = FALSE; // v2.95 02/06/06 16:19 Resurrection 
	m_bIsImpossibleZone = FALSE;

	this->m_bManaMe = FALSE;
	this->m_dwManaMe = NULL;

	//Auto Save ZeroEoyPnk
	b_AutoSaveTime = FALSE;

	RecallCoordX = -1;
	RecallCoordY = -1;
	TimeCoord = NULL;

	WHPage = 0;

	NoClear = FALSE;
	TimerMove = -1;
	Moves = 0;
	Active = 0;

	KilledClientAFK = -1;
	Assasain = FALSE;
	FirstPosition = FALSE;

	m_iCoins = 0;

	TotalMobsKilled = 0;
	TotalDeath = 0;
	TotalClientKilled = 0;
	TotalQuestCompleted = 0;
	TotalStructuresKilled = 0;

	HourPlaying = MinPlaying = EventMinPlaying = 0;

	ActiveAvA = -1;

	CheckProcess = FALSE;
	Heredado = FALSE;

	Event_Killed = 0;
	Event_Death = 0;
	EventLevel = 0;
	EventID = 0;

	RndEventID = 0;

	Temporal_Rating = 0;

	TimeFlagCarrier = -1;

	//Antipull
	NoDelete = TRUE;
	TimeToDelete = -1;
	LogOut = FALSE;
	dw_LastMsg = 0;

	// New System -> Market Place -> HB Arg v4.4
	OpenSell = FALSE;
	for (i = 0; i < DEF_MAXSELLITEMS; i++) {
		MarketItem[i].sIndex = -1;
		MarketItem[i].m_wPrice = -1;
	}
	SellType = -1;
	BuyTo = -1;
	TotalItemSell = 0;

	// --- Miembros de CClient que no se inicializaban (auditoria) ---
	m_rejectedMoves = 0;
	m_bIsPlayerCivil = 0;
	m_bIsAdminOrderGoto = 0;
	m_bIsMsgSendAvailable = 0;
	m_iHP = 0;
	m_iMP = 0;
	m_iSP = 0;
	m_iLevel = 0;
	m_iLuck = 0;
	m_iMaxRankExp = 0;
	m_iCustomItemValue_Attack = 0;
	m_iCustomItemValue_Defense = 0;
	m_iMinAP_SM = 0;
	m_iMinAP_L = 0;
	m_iMaxAP_SM = 0;
	m_iMaxAP_L = 0;
	m_iLastDamage = 0;
	m_iSpeedHackCheckExp = 0;
	m_dwLogoutHackCheck = 0;
	m_dwHeldenianGUID = 0;
	m_dwWarBeginTime = 0;
	m_dwPoisonTime = 0;
	m_dwExpStockTime = 0;
	m_dwAutoExpTime = 0;
	m_dwRecentAttackTime = 0;
	m_dwSpecialAbilityStartTime = 0;
	m_dwSpeedHackCheckTime = 0;
	m_dwCharID = 0;

	ZeroMemory(m_iSkillSSN, sizeof(m_iSkillSSN));
}

void CClient::initializeWinsock() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize Winsock. Error: " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}
	running = true;
	std::cout << "Winsock initialized successfully." << std::endl;
}

void CClient::bindSocket(int port) {
	serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY; // Usar una IP específica si es necesario
	serverAddr.sin_port = htons(port);

	if (::bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Failed to bind socket. Error: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	std::cout << "Socket bound to port " << port << "." << std::endl;
}

void CClient::run()
{
	std::cout << "Starting server loop..." << std::endl;
	char buffer[1024];

	while (running) 
	{
		clientAddrLen = sizeof(clientAddr); 
		int bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0,
			(sockaddr*)&clientAddr, &clientAddrLen);
		if (bytesReceived == SOCKET_ERROR) {
			if (!running) break; 
			std::cerr << "recvfrom failed. Error: " << WSAGetLastError() << std::endl;
			continue;
		}

		buffer[bytesReceived] = '\0';
		receiveMessageFromClient(buffer, bytesReceived);
	}
}

void CClient::receiveMessageFromClient(char* buffer, int bufferLen) {
	if (bufferLen < sizeof(int)) { 
		return;
	}

	char* cp = buffer;
	int msgid;

	Pop(cp, msgid);

	switch (msgid) {
	case SOCKMSG_WELCOME:
	{
		if (bufferLen < sizeof(int) + 5) { 
			return;
		}

		char msg[5];
		Pop(cp, msg, 5);
		m_bUdpSocket = true;
		c_map->sendMessageToClient(client, SOCKMSG_WELCOME);
		break;
	}

	case SOCKMSG_PING:
		c_map->sendMessageToClient(client, SOCKMSG_PING);
		break;

	default:
		std::cerr << "Unknown message ID: " << msgid << std::endl;
		break;
	}
}

void CClient::startServer(int port) {
	initializeWinsock();
	bindSocket(port);

	//std::cout << "Server started on port " << port << ". Waiting for clients..." << std::endl;

	std::thread serverThread([this]() {
		run();
	});
	serverThread.detach();
}

void CClient::stop() {
	if (running) {
		running = false;
		sendto(serverSocket, "", 1, 0, (sockaddr*)&serverAddr, sizeof(serverAddr)); 
		closesocket(serverSocket);
		WSACleanup();
	}
}

CClient::~CClient()
{
 int i;

	if (running) {
		running = false;

		// Enviar un paquete "dummy" para desbloquear recvfrom si está esperando
		sendto(serverSocket, "", 1, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

		// Cerrar el socket
		closesocket(serverSocket);
	}

	if (m_pXSock != NULL) delete m_pXSock;
	for (i = 0; i < DEF_MAXITEMS; i++)
	if (m_pItemList[i] != NULL) {
		if (!m_pItemList[i]) return;
		delete m_pItemList[i];
		m_pItemList[i] = NULL;
	}
	for (i = 0; i < DEF_MAXBANKITEMS; i++)
	if (m_pItemInBankList[i] != NULL) {
		delete m_pItemInBankList[i];
		m_pItemInBankList[i] = NULL;
	}

	for (i = 0; i < DEF_MAXMARKETITEMS; i++)
	{
		if (m_pMarketItem[i])
		{
			delete m_pMarketItem[i];
			m_pMarketItem[i] = NULL;
		}
	}
}

void CClient::AsignSocket(HWND hWnd) {
	m_pXSock = NULL;
	m_pXSock = new class XSocket(hWnd, DEF_CLIENTSOCKETBLOCKLIMIT);
	m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);

	m_bRealClient = true;
}

BOOL CClient::bCreateNewParty()
{
	int i;

	if (m_iPartyRank != -1) return FALSE;

	m_iPartyRank = 0;
	m_iPartyMemberCount = 0;
	m_iPartyGUID = (rand() % 999999) + timeGetTime();

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
		m_stPartyMemberName[i].iIndex = 0;
		ZeroMemory(m_stPartyMemberName[i].cName, sizeof(m_stPartyMemberName[i].cName));
	}

	return TRUE;
}

int CClient::GetWearedItemIndex(short ItemID)
{
	short ItemIndex = 0;

	for (int i = 0; i < DEF_MAXITEMEQUIPPOS; i++)
	{
		ItemIndex = m_sItemEquipmentStatus[i];

		if (ItemIndex > -1 && ItemIndex < DEF_MAXITEMEQUIPPOS)
			if (m_pItemList[ItemIndex] != NULL)
				if (m_pItemList[ItemIndex]->m_sIDnum == ItemID)
					return ItemIndex;
	}

	return -1;
}

short CClient::GetWearedItemIndex(int EquipPoss)
{
	return m_sItemEquipmentStatus[EquipPoss];
}

bool CClient::WearingItem(short ItemID)
{
	if (GetWearedItemIndex(ItemID) > -1) return true;
	return false;
}

bool CClient::WearingItem(short ItemID, int EquipPoss)
{
	short it = GetWearedItemIndex(EquipPoss);

	if (it > -1
		&& m_pItemList[it] != NULL
		&& m_pItemList[it]->m_sIDnum == ItemID)
		return true;

	return false;
}

void CClient::ResetClient() {
	register int i;

	m_pXSock = NULL;
	/*m_pXSock = new class XSocket(hWnd, DEF_CLIENTSOCKETBLOCKLIMIT);
	m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);*/

	m_bRealClient = false;

	ZeroMemory(m_cProfile, sizeof(m_cProfile));
	strcpy(m_cProfile, "__________");

	ZeroMemory(m_cCharName, sizeof(m_cCharName));
	ZeroMemory(m_cAccountName, sizeof(m_cAccountName));
	ZeroMemory(m_cAccountPassword, sizeof(m_cAccountPassword));

	ZeroMemory(m_cGuildName, sizeof(m_cGuildName));
	ZeroMemory(m_cLocation, sizeof(m_cLocation));
	ZeroMemory(m_cMapName, sizeof(m_cMapName));

	strcpy(m_cLocation, "NONE");
	m_iGuildRank = -1;
	m_iGuildGUID = -1;
	TotalGuildMembers = -1;

	m_bIsInitComplete = FALSE;
	m_bActivarComandos = FALSE;
	m_iEnemyKillCount = 0;
	m_iEnemyKillTotalCount = 0;
	m_iPKCount = 0;
	m_iCityFriendKilled = 0;
	AssasainPoint = 0;
	m_iRewardGold = 0;
	m_iCurWeightLoad = 0;
	m_bIsOnlyClientAvailable = FALSE;
	m_bIsSafeAttackMode = FALSE;
	//No recall por 10 segundos - ZeroEoyPnk
	m_bRecall = true;

	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++)
		m_sItemEquipmentStatus[i] = -1;

	for (i = 0; i < DEF_MAXITEMS; i++) {
		if (m_pItemList[i] != NULL) {
			delete m_pItemList[i];
			m_pItemList[i] = NULL;
		}
	}

	for (i = 0; i < DEF_MAXBANKITEMS; i++) {
		if (m_pItemInBankList[i] != NULL) {
			delete m_pItemInBankList[i];
			m_pItemInBankList[i] = NULL;
		}
	}

	m_cArrowIndex = -1;

	// Magic - Skill 
	for (i = 0; i < DEF_MAXMAGICTYPE; i++)
		m_cMagicMastery[i] = NULL;

	for (i = 0; i < DEF_MAXSKILLTYPE; i++)
		m_cSkillMastery[i] = NULL;

	for (i = 0; i < DEF_MAXACHIVEMENTS; i++)
	{
		Achivements[i].Contador = 0;
		Achivements[i].Complete = FALSE;
	}

	for (i = 0; i < DEF_MAXSKILLTYPE; i++) {
		m_bSkillUsingStatus[i] = FALSE;
		m_iSkillUsingTimeID[i] = NULL;
	}

	m_cMapIndex = -1;
	m_sX = -1;
	m_sY = -1;
	m_cDir = 5;
	m_sType = 0;
	m_sOriginalType = 0;
	m_sAppr1 = 0;
	m_sAppr2 = 0;
	m_sAppr3 = 0;
	m_sAppr4 = 0;
	m_iApprColor = 0; // v1.4
	m_iApprColor2 = 0; // New Colors -> ZeroEoyPnk
	m_iStatus = 0;
	m_iLU_Pool = 0;
	bPHackWPE = FALSE;
	m_cSex = 0;
	m_cSkin = 0;
	m_cHairStyle = 0;
	m_cHairColor = 0;
	m_cUnderwear = 0;

	m_cAttackDiceThrow_SM = 0;
	m_cAttackDiceRange_SM = 0;
	m_cAttackDiceThrow_L = 0;
	m_cAttackDiceRange_L = 0;
	m_cAttackBonus_SM = 0;
	m_cAttackBonus_L = 0;

	m_cSide = 0;
	m_iHitRatio = 0;
	m_iDefenseRatio = 0;
	m_iLastPing = 0;

	for (i = 0; i < DEF_MAXITEMEQUIPPOS; i++) m_iDamageAbsorption_Armor[i] = 0;
	m_iDamageAbsorption_Shield = 0;

	m_iHPstock = 0;
	m_iHPStatic_stock = 0;
	m_bIsKilled = FALSE;

	for (i = 0; i < DEF_MAXMAGICEFFECTS; i++)
		m_cMagicEffectStatus[i] = 0;

	m_iWhisperPlayerIndex = -1;
	ZeroMemory(m_cWhisperPlayerName, sizeof(m_cWhisperPlayerName));

	m_iHungerStatus = 100;
	m_bIsWarLocation = FALSE;

	m_bIsPoisoned = FALSE;
	m_iPoisonLevel = NULL;

	m_iAdminUserLevel = 0;
	m_iRating = 0;
	m_iTimeLeft_ShutUp = 0;
	m_iTimeLeft_Rating = 0;
	m_iTimeLeft_ForceRecall = 0;
	m_iTimeLeft_FirmStaminar = 0;

	m_bIsOnServerChange = FALSE;

	m_iExpStock = 0;

	m_iAllocatedFish = NULL;
	m_iFishChance = 0;

	ZeroMemory(m_cIPaddress, sizeof(m_cIPaddress));
	ZeroMemory(m_cIP2, sizeof(m_cIP2));
	m_b5MinutosFinales = FALSE;
	m_bIsOnWaitingProcess = FALSE;
	m_iSuperAttackLeft = 0;
	m_iSuperAttackCount = 0;

	m_sUsingWeaponSkill = 5;

	m_iManaSaveRatio = 0;
	m_iAddResistMagic = 0;
	m_iAddMagicHitting = 0;
	m_iAddPhysicalDamage = 0;
	m_iAddMagicalDamage = 0;
	m_bIsLuckyEffect = FALSE;
	m_iSideEffect_MaxHPdown = 0;

	m_iAddAbsAir = 0;
	m_iAddAbsEarth = 0;
	m_iAddAbsFire = 0;
	m_iAddAbsWater = 0;
	m_iAngelicStr = 0;
	m_iAngelicInt = 0;
	m_iAngelicDex = 0;
	m_iAngelicMag = 0;
	m_iComboAttackCount = 0;
	m_iDownSkillIndex = -1;

	m_iMagicDamageSaveItemIndex = -1;

	m_sCharIDnum1 = m_sCharIDnum2 = m_sCharIDnum3 = 0;

	m_iPartyRank = -1; // v1.42
	m_iPartyMemberCount = 0;
	m_iPartyGUID = 0;

	for (i = 0; i < DEF_MAXPARTYMEMBERS; i++) {
		m_stPartyMemberName[i].iIndex = 0;
		ZeroMemory(m_stPartyMemberName[i].cName, sizeof(m_stPartyMemberName[i].cName));
	}

	m_iAbuseCount = 0;
	m_bIsExchangeMode = FALSE;
	m_iFightZoneTicketNumber = m_iFightzoneNumber = m_iReserveTime = 0;
	m_iPenaltyBlockYear = m_iPenaltyBlockMonth = m_iPenaltyBlockDay = 0;

	m_iExchangeH = NULL;
	for (i = 0; i < DEF_MAXITEMSEXCHANGE; i++) {
		m_cExchangeItemIndex[i] = -1;
		m_iExchangeItemAmount[i] = 0;
	}
	ZeroMemory(m_cExchangeName, sizeof(m_cExchangeName));
	ZeroMemory(m_cExchangeItemName, sizeof(m_cExchangeItemName));
	m_bIsExchangeConfirm = FALSE;

	m_iQuest = NULL;
	m_iQuestID = NULL;
	m_iAskedQuest = NULL;
	m_iCurQuestCount = NULL;
	m_iItemQuest = NULL;
	m_iItemQuestID = NULL;
	m_iItemQuestCount = NULL;
	m_iItemQuestRewardType = NULL;
	m_iStatedQuest = NULL;
	m_iStatedQuestID = NULL;
	m_iStatedQuestCount = NULL;
	m_iStatedQuestRewardType = NULL;

	m_iQuestRewardType = NULL;
	m_iQuestRewardAmount = NULL;

	m_iContribution = NULL;
	m_bQuestMatchFlag_Loc = FALSE;

	m_bIsQuestCompleted[0] = m_bIsQuestCompleted[1] = m_bIsQuestCompleted[2] = FALSE;

	m_bIsNeutral = FALSE;
	m_bIsObserverMode = FALSE;

	m_iSpecialEventID = 200081;

	m_iSpecialWeaponEffectType = 0;
	m_iSpecialWeaponEffectValue = 0;

	m_iAddHP = m_iAddSP = m_iAddMP = 0;
	m_iAddAR = m_iAddPR = m_iAddDR = 0;
	m_iAddMR = 0;
	m_iAddAbsPD = m_iAddAbsMD = 0;
	m_iAddCD = m_iAddExp = m_iAddGold = 0;

	m_iSpecialAbilityTime = DEF_SPECABLTYTIMESEC;		// DEF_SPECABLTYTIMESEC ÃÊ¸¶´Ù ÇÑ¹ø¾¿ Æ¯¼ö ´É·ÂÀ» ¾µ ¼ö ÀÖ´Ù.
	m_iSpecialAbilityType = NULL;
	m_bIsSpecialAbilityEnabled = FALSE;
	m_iSpecialAbilityLastSec = 0;

	m_iSpecialAbilityEquipPos = 0;

	m_iMoveMsgRecvCount = 0;
	m_iAttackMsgRecvCount = 0;
	m_iRunMsgRecvCount = 0;
	m_iSkillMsgRecvCount = 0;

	m_iActivity = TRUE;

	m_iAlterItemDropIndex = -1;

	m_iAutoExpAmount = 0;
	m_iWarContribution = 0;

	m_dwMoveLAT = m_dwRunLAT = m_dwAttackLAT = 0;

	m_dwInitCCTimeRcv = 0;
	m_dwInitCCTime = 0;

	ZeroMemory(m_cLockedMapName, sizeof(m_cLockedMapName));
	strcpy(m_cLockedMapName, "NONE");
	m_iLockedMapTime = NULL;
	m_iDeadPenaltyTime = NULL;

	m_iCrusadeDuty = NULL;
	m_dwCrusadeGUID = NULL;

	for (i = 0; i < DEF_MAXCRUSADESTRUCTURES; i++) {
		m_stCrusadeStructureInfo[i].cType = NULL;
		m_stCrusadeStructureInfo[i].cSide = NULL;
		m_stCrusadeStructureInfo[i].sX = NULL;
		m_stCrusadeStructureInfo[i].sY = NULL;
	}
	m_iCSIsendPoint = NULL;

	m_bIsSendingMapStatus = FALSE;
	ZeroMemory(m_cSendingMapName, sizeof(m_cSendingMapName));

	m_iConstructionPoint = NULL;

	ZeroMemory(m_cConstructMapName, sizeof(m_cConstructMapName));
	m_iConstructLocX = m_iConstructLocY = -1;

	m_dwFightzoneDeadTime = NULL;

	m_iPartyID = NULL;
	m_iPartyStatus = DEF_PARTYSTATUS_NULL;

	m_iReqJoinPartyClientH = NULL;
	ZeroMemory(m_cReqJoinPartyName, sizeof(m_cReqJoinPartyName));

	m_dwLastActionTime = NULL;
	m_bIsCheckingWhisperPlayer = FALSE;
	m_bIsBankModified = FALSE;

	m_iGizonItemUpgradeLeft = 0;

	m_dwAttackFreqTime = m_dwMagicFreqTime = m_dwMoveFreqTime = NULL; // v2.171
	m_bIsMoveBlocked = FALSE; // v2.171
	m_bIsAttackModeChange = FALSE; // v2.172 2002-7-2
	m_iIsOnTown = FALSE;
	m_bIsOnShop = FALSE; // v2.182 2002-11-15 »ç°í ÆÈ¼ö ÀÖ´Â °÷ÀÎÁö Ã¼Å©ÇÏ´Â º¯¼ö Ãß°¡ 
	m_bIsOnTower = FALSE; // v2.20 2002-12-23 ¸¶¹ýÀ» ¹è¿ï¼ö ÀÖ´Â°÷ÀÎÁö Ã¼Å© ÇÑ´Ù.
	m_bIsOnWarehouse = FALSE; // v2.20 2002-12-23 Ã¢°íÀÎÁö Ã¼Å© ÇÑ´Ù.
	m_bIsHunter = FALSE; // v2.182 2002-11-15 »ç³É±ºÀÎÁö Ã¼Å©ÇÏ´Â º¯¼ö Ãß°¡ 
	m_dwWarmEffectTime = NULL; // v2.172 
	m_bIsInBuilding = FALSE;
	m_bIsOnCombatantZone = FALSE;
	m_cHeroBonus = 0;	// v2.23 18/02/06 17:15
	m_bInhibition = FALSE; // v2.95 31/05/06 18:40 Inhibition-Casting 
	m_bIsBeingResurrected = FALSE; // v2.95 02/06/06 16:19 Resurrection 
	m_bIsImpossibleZone = FALSE;

	this->m_bManaMe = FALSE;
	this->m_dwManaMe = NULL;

	//Auto Save ZeroEoyPnk
	b_AutoSaveTime = FALSE;

	RecallCoordX = -1;
	RecallCoordY = -1;
	TimeCoord = NULL;

	WHPage = 0;

	NoClear = FALSE;
	TimerMove = -1;
	Moves = 0;
	Active = 0;

	KilledClientAFK = -1;
	Assasain = FALSE;
	FirstPosition = FALSE;

	m_iCoins = 0;

	TotalMobsKilled = 0;
	TotalDeath = 0;
	TotalClientKilled = 0;
	TotalQuestCompleted = 0;
	TotalStructuresKilled = 0;

	HourPlaying = MinPlaying = EventMinPlaying = 0;

	ActiveAvA = -1;

	CheckProcess = FALSE;
	Heredado = FALSE;

	Event_Killed = 0;
	Event_Death = 0;
	EventLevel = 0;
	EventID = 0;

	RndEventID = 0;

	Temporal_Rating = 0;

	TimeFlagCarrier = -1;

	//Antipull
	NoDelete = TRUE;
	TimeToDelete = -1;
	LogOut = FALSE;
	dw_LastMsg = 0;

	// New System -> Market Place -> HB Arg v4.4
	OpenSell = FALSE;
	for (i = 0; i < DEF_MAXSELLITEMS; i++) {
		MarketItem[i].sIndex = -1;
		MarketItem[i].m_wPrice = -1;
	}
	SellType = -1;
	BuyTo = -1;
	TotalItemSell = 0;
	dw_LastSG = 0;
}

int CClient::GetStr()
{
	return m_iStr + m_iAngelicStr + m_sRankAddStr;
}

int CClient::GetMag()
{
	return m_iMag + m_iAngelicMag + m_sRankAddMag;
}

int CClient::GetVit()
{
	return m_iVit + m_sRankAddVit;
}

int CClient::GetDex()
{
	return m_iDex + m_iAngelicDex + m_sRankAddDex;
}

int CClient::GetInt()
{
	return m_iDex + m_iAngelicDex + m_sRankAddDex;
}

//HeatoN rebirth
int CClient::getRebirthStats()
{
	int result = 0;
	if (m_iRebirthLevel != 0)
	{
		result = m_iRebirthLevel * 3;
	}

	return result;
}


int CClient::GetMaxMP() const
{
	int iRet;

	iRet = (2 * (GetMag())) + (2 * m_iLevel) + ((GetInt()) / 2);

	return iRet;
}

int CClient::GetMaxSP() const
{
	int iRet;

	iRet = (2 * GetStr()) + (2 * m_iLevel);

	return iRet;
}

bool CClient::IsInsideCh()
{
	return strcmp(m_cMapName, "cityhall_1") == 0 || strcmp(m_cMapName, "cityhall_2") == 0;
}


void CClient::Notify(WORD msg, DWORD sV1 /*= 0*/, DWORD sV2 /*= 0*/, DWORD sV3 /*= 0*/, char * pString /*= nullptr*/, DWORD sV4 /*= 0*/, DWORD sV5 /*= 0*/, DWORD sV6 /*= 0*/, DWORD sV7 /*= 0*/, DWORD sV8 /*= 0*/, DWORD sV9 /*= 0*/, char * pString2 /*= nullptr*/, char cValueA /*= 0*/)
{
	c_map->SendNotifyMsg(NULL, client, msg, sV1, sV2, sV3, pString,
		sV4, sV5, sV6, sV7, sV8, sV9, pString2);
}

bool CClient::IsLocation(char * location)
{
	return string(m_cLocation) == location;
}


bool CClient::IsInMap(char * mapname)
{
	return string(m_cMapName) == mapname;
}

void CClient::Send(int h, WORD msg, DWORD sV1, DWORD sV2, DWORD sV3,
	char * pString, DWORD sV4, DWORD sV5,
	DWORD sV6, DWORD sV7, DWORD sV8, DWORD sV9, char * pString2)
{

	c_map->SendNotifyMsg(NULL, h, msg, sV1, sV2, sV3, pString,
		sV4, sV5, sV6, sV7, sV8, sV9, pString2);
}

bool CClient::IsInsideDM()
{
	return strcmp(m_cMapName, "dm") == 0 && g_ev.Is(EventID::Deathmatch);
}

//HeatoN Auto fake system (limbo player) 
bool CClient::IsLimboMap()
{
	return strcmp(m_cMapName, "Whouse") == 0
		|| strcmp(m_cMapName, "gshop_1") == 0 || strcmp(m_cMapName, "gshop_2") == 0
		|| strcmp(m_cMapName, "cityhall_1") == 0 || strcmp(m_cMapName, "cityhall_2") == 0
		|| strcmp(m_cMapName, "bsmith_1") == 0 || strcmp(m_cMapName, "bsmith_2") == 0
		|| strcmp(m_cMapName, "wrhus_1") == 0 || strcmp(m_cMapName, "wrhus_2") == 0
		|| strcmp(m_cMapName, "wzdtwr_1") == 0 || strcmp(m_cMapName, "wzdtwr_2") == 0;
}

void CClient::EnableItem(i8 handle)
{
	char data[56];
	char* cp = data;

	Push(cp, u32(NOTIFY_ENABLE_ITEM));
	Push(cp, handle);

	m_pXSock->iSendMsg(data, cp - data);
}



static void tokenize(string const &str, const char* delim,
	std::vector<string> &out)
{
	char *token = strtok(const_cast<char*>(str.c_str()), delim);
	while (token != nullptr)
	{
		out.push_back(string(token));
		token = strtok(nullptr, delim);
	}
}

static bool is_wanted_line(const string & line, string str)
{
	return (line.find(str) != string::npos);
}

static string get_line(string file, string value1)
{
	ifstream fin(file);

	string line;

	while (getline(fin, line))
	{
		if (is_wanted_line(line, value1))
			return line;
	}

	return "#";
}

static string getvalue(int client, string val)
{
	auto game = c_map;
	auto p = game->m_pClientList[client];

	if (!p) return "";

	char cFileName[112] = {};
	char cDir[112] = {};

	strcat(cFileName, "..\\Nucleo\\");
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	strcat(cFileName, "Quest");
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	wsprintf(cDir, "AscII%d", p->m_cCharName[0]);
	strcat(cFileName, cDir);
	strcat(cFileName, "\\");
	strcat(cFileName, "\\");
	strcat(cFileName, p->m_cCharName);
	strcat(cFileName, ".cfg");

	string result = get_line(cFileName, val);
	if (string(result) == "#") return result;
	else result.erase(0, val.length());
	return result;
}

void CClient::read_quest_data()
{
	vec_charquest.clear();
	for (int i = 0; i < DEF_MAXCHARQUESTS; i++)
	{
		char temp[512];
		wsprintf(temp, "quest-num-%d = ", i + 1);
		string token = getvalue(client, temp);
		if (string(token) == "#") continue;
		const char* delim = " ";
		vector<string> out;
		tokenize(token, delim, out);

		st_charquest u;

		int count = 0;
		for (auto &token : out) {
			count++;
			switch (count)
			{
			case 1:
				u.m_sParentQuestID = atoi((char*)token.c_str());
				break;

			case 2:
				ZeroMemory(u.m_cNpcName, sizeof(u.m_cNpcName));
				strcpy(u.m_cNpcName, (char*)token.c_str());
				break;

			case 3:
				ZeroMemory(u.m_cMapName, sizeof(u.m_cMapName));
				strcpy(u.m_cMapName, (char*)token.c_str());
				break;

			case 4:
				ZeroMemory(u.m_cRealMapName, sizeof(u.m_cRealMapName));
				strcpy(u.m_cRealMapName, (char*)token.c_str());
				break;

			case 5:
				u.m_iKills = atoi((char*)token.c_str());
				break;

			case 6:

				u.m_iMaxKills = atoi((char*)token.c_str());
				break;

			case 7:
				u.m_iRewardContribution = atoi((char*)token.c_str());
				break;

			case 8:
				ZeroMemory(u.m_cQuestReward1, sizeof(u.m_cQuestReward1));
				strcpy(u.m_cQuestReward1, (char*)token.c_str());
				break;

			case 9:
				ZeroMemory(u.m_cQuestReward2, sizeof(u.m_cQuestReward2));
				strcpy(u.m_cQuestReward2, (char*)token.c_str());
				break;

			case 10:
				ZeroMemory(u.m_cQuestReward3, sizeof(u.m_cQuestReward3));
				strcpy(u.m_cQuestReward3, (char*)token.c_str());
				break;

			case 11:
				u.m_sStatus = atoi((char*)token.c_str());
				break;

			case 12:
				u.m_sCompleted = atoi((char*)token.c_str());
				break;

			default: break;
			}
		}

		vec_charquest.push_back(u);
	}

	send_charquest_data();
}

void CClient::save_quest_data()
{
	auto game = c_map;
	auto p = game->m_pClientList[client];

	FILE *fp;
	if (!p) return;

	char cFileName[256] = {};
	char cDir[256] = {};
	strcat(cFileName, "..\\Nucleo\\");

	strcat(cFileName, "Quest\\");
	_mkdir("..\\Nucleo\\Quest");

	wsprintf(cDir, "AscII%d", p->m_cCharName[0]);
	strcat(cFileName, cDir);
	strcat(cFileName, "\\");
	_mkdir(cFileName);

	strcat(cFileName, p->m_cCharName);
	strcat(cFileName, ".cfg");

	fp = fopen(cFileName, "w");
	if (!fp) return;

	// Guardar datos de misiones en el archivo
	for (int i = 0; i < vec_charquest.size(); i++)
	{
		auto quest = vec_charquest[i];
		std::string m_sSave = "quest-num-";
		m_sSave.append(std::to_string(i + 1));
		m_sSave.append(" = ");
		m_sSave.append(std::to_string(quest.m_sParentQuestID));
		m_sSave.append(" ");
		m_sSave.append(quest.m_cNpcName);
		m_sSave.append(" ");
		m_sSave.append(quest.m_cMapName);
		m_sSave.append(" ");
		m_sSave.append(quest.m_cRealMapName);
		m_sSave.append(" ");
		m_sSave.append(std::to_string(quest.m_iKills));
		m_sSave.append(" ");
		m_sSave.append(std::to_string(quest.m_iMaxKills));
		m_sSave.append(" ");
		m_sSave.append(std::to_string(quest.m_iRewardContribution));
		m_sSave.append(" ");
		m_sSave.append(quest.m_cQuestReward1);
		m_sSave.append(" ");
		m_sSave.append(quest.m_cQuestReward2);
		m_sSave.append(" ");
		m_sSave.append(quest.m_cQuestReward3);
		m_sSave.append(" ");
		m_sSave.append(std::to_string(quest.m_sStatus));
		m_sSave.append(" ");
		m_sSave.append(std::to_string(quest.m_sCompleted));
		m_sSave.append("\n");

		fwrite((char *)m_sSave.c_str(), 1, m_sSave.size(), fp);
	}

	if (fp != NULL) fclose(fp);
}

void CClient::send_charquest_data()
{
	char data[4096];
	auto * cp = data;
	u32 msgid = -1;
	u16 nulled = 0;
	short iSize = 0;

	msgid = NOT_CHARQUESTS;
	iSize = vec_charquest.size();

	Push(cp, msgid);
	Push(cp, nulled);
	Push(cp, iSize);

	for (int i = 0; i < iSize; i++)
	{
		Push(cp, vec_charquest[i].m_sParentQuestID);
		Push(cp, vec_charquest[i].m_cNpcName, 21);
		Push(cp, vec_charquest[i].m_cMapName, 12);
		Push(cp, vec_charquest[i].m_cRealMapName, 12);
		Push(cp, vec_charquest[i].m_iKills);
		Push(cp, vec_charquest[i].m_iMaxKills);
		Push(cp, vec_charquest[i].m_iRewardContribution);
		Push(cp, vec_charquest[i].m_cQuestReward1, 21);
		Push(cp, vec_charquest[i].m_cQuestReward2, 21);
		Push(cp, vec_charquest[i].m_cQuestReward3, 21);
		Push(cp, vec_charquest[i].m_sStatus);
		Push(cp, vec_charquest[i].m_sCompleted);
	}

	c_map->m_pClientList[client]->m_pXSock->iSendMsg(data, cp - data);
}

void CClient::req_dismiss_quest(char * data)
{
	int index;
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	Pop(data, index);

	if (index > vec_charquest.size() || index < 0) return;

	remove_quest(index);
	c_map->ShowClientMsg(client, "The quest was successfully removed.");
	send_charquest_data();
}

void CClient::req_pause_quest(char * data)
{
	int index;
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	Pop(data, index);
	pause_quest(index);
	c_map->ShowClientMsg(client, "The quest was successfully paused.");
	send_charquest_data();
}

void CClient::req_active_quest(char * data)
{
	int index;
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	Pop(data, index);

	active_quest(index);
}

void CClient::active_quest(int index)
{
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	if (index > vec_charquest.size() || index < 0) return;

	for (int i = 0; i < vec_charquest.size(); i++)
	{
		if (i == index) continue;
		vec_charquest[i].m_sStatus = 0;
	}

	vec_charquest[index].m_sStatus = 1;
	c_map->ShowClientMsg(client, "The quest was successfully activated.");
	send_charquest_data();
}

void CClient::req_reward_quest(char * data)
{
	int index;
	short selected;
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	Pop(data, index);
	Pop(data, selected);
	reward_quest(index, selected);
}

static void deleteText(std::string& str, const std::string& toRemove) {
	size_t pos = str.find(toRemove);
	if (pos != std::string::npos) {
		str.erase(pos, toRemove.length());
	}
}

static bool startsWith(const std::string& str, const std::string& prefix) {
	return str.rfind(prefix, 0) == 0; // Verifica si `prefix` es el inicio de `str`
}

void CClient::reward_quest(int index, short rew)
{
	char tmp_rew[21];
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	if (vec_charquest.size() == 0) return;
	if (index > vec_charquest.size() || index < 0) return;
	if (vec_charquest[index].m_sCompleted == 0) return;

	switch (rew)
	{
	case 1:
		strcpy(tmp_rew, vec_charquest[index].m_cQuestReward1);
		break;

	case 2:
		strcpy(tmp_rew, vec_charquest[index].m_cQuestReward2);
		break;

	case 3:
		strcpy(tmp_rew, vec_charquest[index].m_cQuestReward3);
		break;

	default: return; break;
	}

	if (string(tmp_rew) == "None") return;

	string reward = tmp_rew;
	string contrib_text = "Contrib+";
	string rep_text = "Rep+";

	if (startsWith(reward, contrib_text))
	{
		deleteText(reward, contrib_text);
		int contrib = atoi(reward.c_str());

		if (contrib > 0) {
			wsprintf(G_cTxt, "You completed quest and received %d contribution.", contrib);
			c_map->ShowClientMsg(client, G_cTxt);
			p->m_iContribution += contrib; 
			c_map->SendCommand(client, "/contrib", p->m_iContribution);

			remove_quest(index);
			send_charquest_data();
			save_quest_data();
		}
	}
	else if (startsWith(reward, rep_text))
	{
		deleteText(reward, rep_text);
		int rep = atoi(reward.c_str());

		if (rep > 0) {
			wsprintf(G_cTxt, "You completed quest and received %d reputation.", rep);
			c_map->ShowClientMsg(client, G_cTxt);
			p->m_iRating += rep; 
			c_map->SendCommand(client, "/rep", p->m_iRating);

			remove_quest(index);
			send_charquest_data();
			save_quest_data();
		}
	}
	else
	{
		if (c_map->_iGetItemSpaceLeft(client) < 1)
		{
			c_map->SendItemNotifyMsg(client, CLIENT_NOTIFY_CANNOTCARRYMOREITEM, NULL, NULL);
			wsprintf(G_cTxt, "Error. Your inventory is full.", vec_charquest[index].m_iRewardContribution);
			c_map->ShowClientMsg(client, G_cTxt);
			return;
		}

		wsprintf(G_cTxt, "You have completed the quest and received %s.", (char*)reward.c_str());
		c_map->ShowClientMsg(client, G_cTxt);
		CreateItem((char*)reward.c_str());
		remove_quest(index);
		send_charquest_data();
		save_quest_data();
	}
}


void CClient::pause_quest(int index)
{
	auto p = c_map->m_pClientList[client];
	if (!p) return;

	if (index > vec_charquest.size() || index < 0) return;

	vec_charquest[index].m_sStatus = 0;

	c_map->ShowClientMsg(client, "The quest was successfully activated.");
	send_charquest_data();
}


void CClient::remove_quest(int index)
{
	if (index > vec_charquest.size() || index < 0) return;

	vec_charquest.erase(vec_charquest.begin() + index);
}

void CClient::CreateItem(char *  itemname)
{
	char   seps[] = "= \t\n";
	char   * cp, cItemName[256], cData[256], cTemp[256], cAttribute[256];
	SYSTEMTIME SysTime;
	class  CStrTok * pStrTok;
	class  CItem * pItem;
	short  * sp;
	int    iRet, iTemp, iEraseReq, iValue;
	DWORD * dwp;
	WORD  * wp, wTemp;
	double dV1, dV2, dV3;

	auto g = c_map;
	auto p = g->m_pClientList[client];
	if (!p) return;

	ZeroMemory(cItemName, sizeof(cItemName));
	strcpy(cItemName, itemname);

	ZeroMemory(cAttribute, sizeof(cAttribute));
	strcpy(cAttribute, std::to_string(0).c_str());

	pItem = new class CItem;

	if (g->_bInitItemAttr(pItem, cItemName) == FALSE) {
		delete pItem;
		return;
	}


	iValue = 0;
	if (strlen(cAttribute) > 0 && strlen(cAttribute) <= 11) {
		pItem->m_dwAttribute = (DWORD)std::stoull(string(cAttribute));
		if (pItem->m_dwAttribute == 1) {
			if ((iValue >= 1) && (iValue <= 200)) {

				pItem->m_cItemColor = 2;
				pItem->m_sItemSpecEffectValue2 = iValue - 100;

				dV2 = (double)pItem->m_sItemSpecEffectValue2;
				dV3 = (double)pItem->m_wMaxLifeSpan;
				dV1 = (dV2 / 100.0f)*dV3;

				iTemp = (int)pItem->m_wMaxLifeSpan;
				iTemp += (int)dV1;

				if (iTemp <= 0)
					wTemp = 1;
				else wTemp = (WORD)iTemp;

				if (wTemp <= pItem->m_wMaxLifeSpan * 2) {

					pItem->m_wMaxLifeSpan = wTemp;
					pItem->m_sItemSpecEffectValue1 = (short)wTemp;
					pItem->m_wCurLifeSpan = pItem->m_wMaxLifeSpan;
				}
				else pItem->m_sItemSpecEffectValue1 = (short)pItem->m_wMaxLifeSpan;

			}
			else pItem->m_dwAttribute = NULL;
		}
		else {

			if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK) {
				switch ((pItem->m_dwAttribute & 0xF00000) >> 20) {
				case 6:	pItem->m_cItemColor = 2; break;
				case 8: pItem->m_cItemColor = 3; break;
				case 1:	pItem->m_cItemColor = 5; break;
				case 5:	pItem->m_cItemColor = 1; break;
				case 3:	pItem->m_cItemColor = 7; break;
				case 2:	pItem->m_cItemColor = 4; break;
				case 7:	pItem->m_cItemColor = 6; break;
				case 9:	pItem->m_cItemColor = 8; break;
				}
			}
			else if (pItem->m_sItemEffectType == DEF_ITEMEFFECTTYPE_ATTACK_MANASAVE) {
				switch ((pItem->m_dwAttribute & 0xF00000) >> 20) {
				case 10: pItem->m_cItemColor = 5; break;
				}
			}
		}
	}

	switch (pItem->m_sIDnum) {
	case 511:
	case 513:
	case 515:
	case 517:
	case 530:
	case 531:
	case 532:
	case 533:
	case 534:
		GetLocalTime(&SysTime);
		pItem->m_sTouchEffectType = DEF_ITET_DATE;

		pItem->m_sTouchEffectValue1 = (short)SysTime.wMonth;
		pItem->m_sTouchEffectValue2 = (short)SysTime.wDay;
		pItem->m_sTouchEffectValue3 = 24;
		break;

	default:
		GetLocalTime(&SysTime);
		pItem->m_sTouchEffectType = DEF_ITET_ID;
		pItem->m_sTouchEffectValue1 = c_map->iDice(1, 100000);
		pItem->m_sTouchEffectValue2 = c_map->iDice(1, 100000);

		ZeroMemory(cTemp, sizeof(cTemp));
		wsprintf(cTemp, "%d%2d", (short)SysTime.wMonth, (short)SysTime.wDay);
		pItem->m_sTouchEffectValue3 = atoi(cTemp);
		break;
	}

	ZeroMemory(cData, sizeof(cData));

	if (g->_bAddClientItemList(client, pItem, &iEraseReq) == TRUE) {
		g->SendItemNotifyMsg(client, CLIENT_NOTIFY_ITEMOBTAINED, pItem, NULL);

		if (iEraseReq == 1) delete pItem;
		return;
	}
	else {
		delete pItem;
		return;
	}
}

void CClient::send_charquest_data_byindex(int index)
{
	char data[1024];
	auto * cp = data;
	u32 msgid = -1;
	u16 nulled = 0;

	msgid = NOT_CHARQUESTS2;

	Push(cp, msgid);
	Push(cp, nulled);

	Push(cp, index);

	Push(cp, vec_charquest[index].m_iKills);
	Push(cp, vec_charquest[index].m_sStatus);
	Push(cp, vec_charquest[index].m_sCompleted);

	c_map->m_pClientList[client]->m_pXSock->iSendMsg(data, cp - data);
}

