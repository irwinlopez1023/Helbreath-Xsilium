#include "../Header/MainLog.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include "..\..\shared\shared.h"

extern void	PutLogList(char * cMsg);
extern void	AccountLogList(char * cMsg);
extern void ConfigList(char * cMsg);
extern void ErrorLogList(char * cMsg);
extern void PutLogAtaque(char * cStr);
extern void UpdateConfigList(char * cMsg);
extern void CommandList(char * cMsg);

extern char	G_cTxt[500];
extern char	G_cData50000[50000];

// ---------------------------------------------------------------------------
// Deteccion de intentos de tirar el Nucleo
//
// Un paquete con tamano imposible (el que antes tumbaba el servidor sin
// necesidad de cuenta) se avisa en rojo en la consola y se escribe en
// ..\ServerLogs\Ataques\Ataques Nucleo [dd-mm-aaaa].txt con la IP, las cuentas
// conectadas ahora desde esa IP y las que entraron desde ella antes.
// ---------------------------------------------------------------------------

// Historial IP -> cuentas que entraron desde esa IP. Solo en memoria.
static std::map<std::string, std::vector<std::string> > G_mapIPCuentas;

static void _RegistrarIPCuenta(const char * cIP, const char * cAccountName)
{
	char cCuenta[12];

	if ((cIP == NULL) || (cAccountName == NULL)) return;
	if ((cIP[0] == 0) || (cAccountName[0] == 0)) return;

	ZeroMemory(cCuenta, sizeof(cCuenta));
	strncpy(cCuenta, cAccountName, 10);

	if (G_mapIPCuentas.size() >= 20000) G_mapIPCuentas.clear();

	std::vector<std::string> & lista = G_mapIPCuentas[std::string(cIP)];
	if (std::find(lista.begin(), lista.end(), std::string(cCuenta)) != lista.end()) return;
	if (lista.size() >= 10) lista.erase(lista.begin());
	lista.push_back(std::string(cCuenta));
}

static void _ReportarAtaqueNucleo(class CMainLog * pMain, int iClientH, char * cMotivo)
{
	static char  s_cUltimaIP[21] = { 0 };
	static DWORD s_dwUltimoAviso = 0;
	static int   s_iOmitidos = 0;

	char  cMsg[1024], cQuien[40], cOtros[400], cHistorial[400], cOmitidos[80], cIP[21], cEntrada[20];
	DWORD dwNow;
	int   i;

	if ((pMain == NULL) || (cMotivo == NULL)) return;
	if ((iClientH <= 0) || (iClientH >= DEF_MAXCLIENTSOCK)) return;
	if (pMain->m_pClientList[iClientH] == NULL) return;

	ZeroMemory(cIP, sizeof(cIP));
	strncpy(cIP, pMain->m_pClientList[iClientH]->m_cIPaddress, sizeof(cIP) - 1);

	// Anti-spam: de la misma IP, como mucho un aviso cada 5 segundos
	dwNow = timeGetTime();
	if ((strcmp(s_cUltimaIP, cIP) == 0) && ((dwNow - s_dwUltimoAviso) < 5000)) {
		s_iOmitidos++;
		return;
	}

	ZeroMemory(cOmitidos, sizeof(cOmitidos));
	if (s_iOmitidos > 0) {
		_snprintf(cOmitidos, sizeof(cOmitidos) - 1, " | +%d intentos anteriores de %s sin mostrar", s_iOmitidos, s_cUltimaIP);
	}
	strncpy(s_cUltimaIP, cIP, sizeof(s_cUltimaIP) - 1);
	s_dwUltimoAviso = dwNow;
	s_iOmitidos = 0;

	ZeroMemory(cQuien, sizeof(cQuien));
	if (pMain->m_pClientList[iClientH]->m_cAccountName[0] != 0)
		_snprintf(cQuien, sizeof(cQuien) - 1, "cuenta %.10s", pMain->m_pClientList[iClientH]->m_cAccountName);
	else strcpy(cQuien, "sin login");

	// Otras conexiones abiertas ahora desde la misma IP con cuenta
	ZeroMemory(cOtros, sizeof(cOtros));
	for (i = 1; i < DEF_MAXCLIENTSOCK; i++) {
		if ((i == iClientH) || (pMain->m_pClientList[i] == NULL)) continue;
		if (pMain->m_pClientList[i]->m_cAccountName[0] == 0) continue;
		if (strcmp(pMain->m_pClientList[i]->m_cIPaddress, cIP) != 0) continue;

		ZeroMemory(cEntrada, sizeof(cEntrada));
		_snprintf(cEntrada, sizeof(cEntrada) - 1, "%s%.10s", (cOtros[0] != 0) ? ", " : "", pMain->m_pClientList[i]->m_cAccountName);
		if (strlen(cOtros) + strlen(cEntrada) < sizeof(cOtros) - 1) strcat(cOtros, cEntrada);
	}
	if (cOtros[0] == 0) strcpy(cOtros, "nadie");

	// Cuentas que entraron antes desde esa IP
	ZeroMemory(cHistorial, sizeof(cHistorial));
	std::map<std::string, std::vector<std::string> >::iterator it = G_mapIPCuentas.find(std::string(cIP));
	if (it != G_mapIPCuentas.end()) {
		for (size_t k = 0; k < it->second.size(); k++) {
			if (strlen(cHistorial) + it->second[k].size() + 3 >= sizeof(cHistorial)) break;
			if (cHistorial[0] != 0) strcat(cHistorial, ", ");
			strcat(cHistorial, it->second[k].c_str());
		}
	}
	if (cHistorial[0] == 0) strcpy(cHistorial, "ninguna desde que arranco el Nucleo");

	ZeroMemory(cMsg, sizeof(cMsg));
	_snprintf(cMsg, sizeof(cMsg) - 1, "NUCLEO: %s | IP %s | Conexion <%d>: %s | Cuentas conectadas con esa IP: %s | Cuentas que entraron antes desde esa IP: %s%s",
		cMotivo, cIP, iClientH, cQuien, cOtros, cHistorial, cOmitidos);
	PutLogAtaque(cMsg);
}

CMainLog::CMainLog(HWND hWnd)
{
	int i;
	for (i = 0; i < DEF_MAXCLIENTSOCK; i++) m_pClientList[i] = NULL;
	for (i = 0; i < DEF_MSGQUEUESIZE; i++) m_pMsgQueue[i] = NULL;
	for (i = 0; i < DEF_MAXGAME; i++) m_pGameList[i] = NULL;
	for (i = 0; i < DEF_MAXMAPS; i++) m_pMapList[i] = NULL;
	for (i = 0; i < DEF_MAXACCOUNTS; i++) m_pAccountList[i] = NULL;
	for (i = 0; i < DEF_MAXCHARACTER; i++) m_pCharList[i] = NULL;
	for (i = 0; i < DEF_MAXMAPSERVERS; i++) m_pMapServerList[i] = NULL;

	m_hWnd = hWnd;
	ZeroMemory(m_cMainServerAddress, sizeof(m_cMainServerAddress));
	m_iQueueHead = 0;
	m_iQueueTail = 0;
	m_iMainServerInternalPort = 0;
	m_iValidAccounts = m_iValidCharacters = m_iTotalGame = 0;
	m_iTotalMapServerSock = 0;
	bShutDown = FALSE;
	m_dwMinute = timeGetTime();
}

CMainLog::~CMainLog(void)
{
	int i;
	for (i = 0; i < DEF_MAXCLIENTSOCK; i++)
		if (m_pClientList[i] != NULL) delete m_pClientList[i];
	for (i = 0; i < DEF_MSGQUEUESIZE; i++)
		if (m_pMsgQueue[i] != NULL) delete m_pMsgQueue[i];
	for (i = 0; i < DEF_MAXGAME; i++)
		if (m_pGameList[i] != NULL) delete m_pGameList[i];
	for (i = 0; i < DEF_MAXMAPS; i++)
		if (m_pMapList[i] == NULL) delete m_pMapList[i];
	for (i = 0; i < DEF_MAXACCOUNTS; i++)
		if (m_pAccountList[i] == NULL) delete m_pAccountList[i];
	for (i = 0; i < DEF_MAXCHARACTER; i++)
		if (m_pCharList[i] == NULL) delete m_pCharList[i];

	m_bGameServerList = FALSE;
}

bool CMainLog::bInit()
{
	//if (bReadServerConfigFile("Server.cfg") == FALSE) return FALSE;
	if (bReadServerConfigFile("LoginServer.cfg") == FALSE) return TRUE;

/*
	wsprintf(G_cTxt, "Actual Accounts: %d", m_iValidAccounts);
	ConfigList(G_cTxt);

	wsprintf(G_cTxt, "Actual Characters: %d", m_iValidCharacters);
	ConfigList(G_cTxt);

	G_cClientCheckCount = 0;
	CommandList("List Comand: '/help' ");*/
	return TRUE;
}

void CMainLog::MsgProcess()
{
/*#ifdef DEF_DEBUG
	try {
#endif*/
		char * pData = NULL, cFrom, cKey, cTemp[50];
		DWORD dwMsgSize, *dwpMsgID;
		int iClientH;
		char cDump[1000];

		ZeroMemory(cDump, sizeof(cDump));
		ZeroMemory(cTemp, sizeof(cTemp));

		ZeroMemory(m_pMsgBuffer, DEF_MSGBUFFERSIZE + 1);
		pData = (char *)m_pMsgBuffer;
		while (bGetMsgQuene(&cFrom, pData, &dwMsgSize, &iClientH, &cKey) == TRUE) {
			switch (cFrom) {
				//----------From Client-------------//
				case DEF_MSGFROM_CLIENT:
					dwpMsgID = (DWORD *)(pData + DEF_INDEX4_MSGID);
					switch (*dwpMsgID) {
						case NUCLEO_REQUEST_LOGIN: //client request login
							RequestLogin(iClientH, pData);
							break;
							//heaton change password
						case MSGID_REQUEST_CHANGEPASSWORD:
							ChangePassword(iClientH, pData);
							break;

						case NUCLEO_REQUEST_CREATEACC: //client request login
							CreateNewAccount(iClientH, pData); //heaton
							break;

						case NUCLEO_REQUEST_CREATENEWCHARACTER: //message from client
							ResponseCharacter(iClientH, pData);
							break;

						case NUCLEO_REQUEST_DELETECHARACTER:
							DeleteCharacter(iClientH, pData);
							break;

						case NUCLEO_REQUEST_ENTERGAME:
							RequestEnterGame(iClientH, pData);
							break;

						default:
							wsprintf(cDump, "client: 0x%.8X", *dwpMsgID);
							ErrorLogList(cDump);
							break;
					}
					break;

					//---------FROM MapServer------------//
				case DEF_MSGFROM_MAPSERVER:
					dwpMsgID = (DWORD *)(pData + DEF_INDEX4_MSGID);
					switch (*dwpMsgID) {
						case NUCLEO_REQUEST_REGISTERGAMESERVER:
							RegisterGameServer(iClientH, pData, TRUE);
							break;

						case NUCLEO_REQUEST_PLAYERDATA:
							RequestPlayerData(iClientH, pData);
							break;

						case NUCLEO_ENTERGAMECONFIRM:
							EnterGameConfirm(iClientH, pData);
							break;

						case NUCLEO_REQUEST_SAVEPLAYERDATA:
							RequestSavePlayerData(iClientH, pData, dwMsgSize, 0, 0);
							break;

						case NUCLEO_REQUEST_SAVEPLAYERDATA_REPLY:
							RequestSavePlayerData(iClientH, pData, dwMsgSize, 1, 1);
							break;

						case NUCLEO_REQUEST_SAVEPLAYERDATALOGOUT:
							RequestSavePlayerData(iClientH, pData, dwMsgSize, 1, 0);
							break;

						case NUCLEO_REQUEST_NOSAVELOGOUT:
							RequestNoSaveLogout(iClientH, pData);
							break;

						case NUCLEO_REQUEST_DELETEACCOUNT:
							RequestDeleteAccount(pData);
							break;

						case NUCLEO_REQUEST_SETACCOUNTWAITSTATUS:
							RequestSetAccountWaitStatus(pData);
							break;

						case NUCLEO_REQUEST_CREATENEWGUILD:
							RequestCreateNewGuild(iClientH, pData);
							break;

						case NUCLEO_REQUEST_DISBANDGUILD:
							RequestDisbandGuild(iClientH, pData);
							break;

						case NUCLEO_REQUEST_NEWGUILDSMAN:
							UpdateGuildInfoNewGuildsman(iClientH, pData);
							break;

						case NUCLEO_REQUEST_DELGUILDSMAN:
							UpdateGuildInfoDeleteGuildman(iClientH, pData);
							break;

						default:
							wsprintf(cDump, "map: 0x%.8X", *dwpMsgID);
							ErrorLogList(cDump);
							break;
					}
					break;

				default: break;
			}
		}
		UpdateScreen();
/*#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: MsgProcess");
	}
#endif*/
}

BOOL CMainLog::bGetMsgQuene(char * pFrom, char * pData, DWORD * pMsgSize, int * pIndex, char * pKey)
{
#ifdef DEF_DEBUG
	try {
#endif
		if (m_pMsgQueue[m_iQueueHead] == NULL) return FALSE;

		m_pMsgQueue[m_iQueueHead]->Get(pFrom, pData, pMsgSize, pIndex, pKey);

		delete m_pMsgQueue[m_iQueueHead];
		m_pMsgQueue[m_iQueueHead] = NULL;

		m_iQueueHead++;
		if (m_iQueueHead >= DEF_MSGQUEUESIZE) m_iQueueHead = 0;

		return TRUE;
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: bGetMsgQuene");
	}
#endif
	return FALSE;
}

BOOL CMainLog::bPutMsgQuene(char cFrom, char * pData, DWORD dwMsgSize, int iIndex, char cKey)
{
#ifdef DEF_DEBUG
	try {
#endif
		if (m_pMsgQueue[m_iQueueTail] != NULL) return FALSE;

		m_pMsgQueue[m_iQueueTail] = new class CMsg;
		if (m_pMsgQueue[m_iQueueTail] == NULL) return FALSE;

		if (m_pMsgQueue[m_iQueueTail]->bPut(cFrom, pData, dwMsgSize, iIndex, cKey) == FALSE) return FALSE;

		m_iQueueTail++;
		if (m_iQueueTail >= DEF_MSGQUEUESIZE) m_iQueueTail = 0;

		return TRUE;
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: bPutMsgQuene");
	}
#endif
	return FALSE;
}

void CMainLog::OnTimer()
{
#ifdef DEF_DEBUG
	try {
#endif
		DWORD dwTime;
		dwTime = timeGetTime();

		MsgProcess();
		DeleteAccountTimmer();

		/*if ((dwTime - m_dwMinute) > 1000 * 60) {
			wsprintf(G_cTxt, "Actual Accounts: %d", m_iValidAccounts);
			UpdateConfigList(G_cTxt);
			wsprintf(G_cTxt, "Actual Characters: %d", m_iValidCharacters);
			UpdateConfigList(G_cTxt);

			m_dwMinute = dwTime;
		}*/
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: OnTimer");
	}
#endif
}

//send message to WLserver
void CMainLog::SendEventToClient(DWORD dwMsgID, WORD wMsgType, char * pData, DWORD dwMsgSize, int iClientH)
{
#ifdef DEF_DEBUG
	try {
#endif
		int iRet;
		DWORD * dwp;
		char * cp;
		WORD * wp;

		if (iClientH == NULL) return;

		ZeroMemory(G_cData50000, sizeof(G_cData50000));

		dwp = (DWORD *)(G_cData50000 + DEF_INDEX4_MSGID);
		*dwp = dwMsgID;
		wp = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
		*wp = wMsgType;

		cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure((char *)cp, pData, dwMsgSize);

		if ((iClientH != NULL)) {
			iRet = m_pClientList[iClientH]->m_pXSock->iSendMsg(G_cData50000, dwMsgSize + 6, DEF_USE_ENCRYPTION); //send anyway..

			switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					delete m_pClientList[iClientH];
					m_pClientList[iClientH] = NULL;
					wsprintf(G_cTxt, " Connection Lost (%d) - (%s)", iClientH, m_pClientList[iClientH]->m_cWorldName);
					PutLogList(G_cTxt);
					break;
			}
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: SendEventToClient");
	}
#endif
}

void CMainLog::OnClientSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam) //From Wl or client
{
#ifdef DEF_DEBUG
	try {
#endif
		register int iClientH, iRet, iTmp, iTotalSock;
		char  * pData, cKey;
		DWORD  dwMsgSize;
		iTotalSock = 0;

		iTmp = WM_ONCLIENTSOCKETEVENT;
		iClientH = message - iTmp;
		if (m_pClientList[iClientH] == NULL) return;
		//Cambio lalo por crash aca
		if (!iClientH) return;

		iRet = m_pClientList[iClientH]->m_pXSock->iOnSocketEvent(wParam, lParam);
		switch (iRet) {

			case DEF_XSOCKEVENT_CONNECTIONESTABLISH:
				break;

			case DEF_XSOCKEVENT_READCOMPLETE:
				pData = m_pClientList[iClientH]->m_pXSock->pGetRcvDataPointer(&dwMsgSize, &cKey);
				if (pData == NULL) {
					// Cabecera que declara menos de 7 bytes: se avisa y se corta la conexion
					char cMotivo[120];
					wsprintf(cMotivo, "Paquete malformado (tamano declarado %d, minimo 7)", m_pClientList[iClientH]->m_pXSock->wGetRcvHeaderSize());
					_ReportarAtaqueNucleo(this, iClientH, cMotivo);
					delete m_pClientList[iClientH];
					m_pClientList[iClientH] = NULL;
					break;
				}
				if (bPutMsgQuene(DEF_MSGFROM_CLIENT, pData, dwMsgSize, iClientH, cKey) == FALSE) {
					ErrorLogList("CRITICAL ERROR in LOGSERVER MsgQuene!!!");
				}
				break;

			case DEF_XSOCKEVENT_BLOCK:
				break;

			case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
				break;

			case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
				// Tamano de paquete imposible: se avisa y se desconecta (sigue abajo)
				{
					char cMotivo[120];
					wsprintf(cMotivo, "Paquete con tamano invalido (declara %d bytes)", m_pClientList[iClientH]->m_pXSock->wGetRcvHeaderSize());
					_ReportarAtaqueNucleo(this, iClientH, cMotivo);
				}
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
			case DEF_XSOCKEVENT_QUENEFULL:
				delete m_pClientList[iClientH];
				m_pClientList[iClientH] = NULL;
				break;
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: OnClientSubLogSocketEvent");
	}
#endif
}

BOOL CMainLog::bClientRegisterMaps(int iClientH, char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		int i;

		for (i = 0; i < DEF_MAXMAPS; i++) {
			if (m_pMapList[i] != NULL) {
				if (memcmp(m_pMapList[i]->m_cMapName, pData, 10) == 0) {
					/*wsprintf(G_cTxt, "CRITICAL ERROR! Map(%s) duplicated!", pData);
					ErrorLogList(G_cTxt);*/
					return FALSE;
				}
			}
		}

		for (i = 0; i < DEF_MAXMAPS; i++) {
			if (m_pMapList[i] == NULL) {
				m_pMapList[i] = new class CMap(iClientH, pData);
				/*wsprintf(G_cTxt, "(%d) Map(%s) registration: %s", i, pData, m_pGameList[iClientH]->m_cGameName);
				PutLogList(G_cTxt);*/
				return TRUE;
			}
		}

		wsprintf(G_cTxt, "No more map space left. Map(%s) cannot be added.", pData);
		ErrorLogList(G_cTxt);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: bClientRegisterMaps");
	}
#endif
	return FALSE;
}

void CMainLog::RequestLogin(int iClientH, char *pData)
{
/*#ifdef DEF_DEBUG
	try {
#endif*/
		int  iMessage, iAccount, i;
		char cMesg, cTotalChar;
		DWORD *dwp;
		SYSTEMTIME SysTime;
		char *cp, cAccountName[11], cAccountPass[11], cWorldName[30], cTotalCharNames[10][11];//cTotalCharNames[5][11]
		//Agregado
		char cCharacterName[11];
		char cData[3000], cCharName[11], cMapName[11];
		WORD *wp;
		short sAppr1, sAppr2, sAppr3, sAppr4; // , iUperVersion, iLowerVersion, iArgentVersion;
		char cSex, cSkin;
		int iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iExp, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
		int *ip;
		int iUperVersion, iLowerVersion, iArgentVersion;

		iUperVersion = DEF_UPPERVERSION;
		iLowerVersion = DEF_LOWERVERSION;
		iArgentVersion = DEF_ARGENTVERSION;

		ZeroMemory(cAccountName, sizeof(cAccountName));
		ZeroMemory(cAccountPass, sizeof(cAccountPass));
		ZeroMemory(cWorldName, sizeof(cWorldName));

		cMesg = 0;
		cTotalChar = 0;
		iAccount = -1;

		if (m_pClientList[iClientH] == NULL) return;

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cAccountName, cp, 10);
		cp += 10;
		memcpy_secure(cAccountPass, cp, 10);
		cp += 10;
		memcpy_secure(cWorldName, cp, 30);
		cp += 30;

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount);
		switch (iMessage) {
			case 0: //0 errors check password and send success message
				if (m_pAccountList[iAccount] == NULL) cMesg = 1;
				else if (strcmp(m_pAccountList[iAccount]->cAccountName, cAccountName) != 0) cMesg = 1;
				else if (strcmp(m_pAccountList[iAccount]->cWorldName, cWorldName) != 0) {
					wsprintf(G_cTxt, "Account(%s) World(%s) none exist", cWorldName, cWorldName);
					AccountLogList(G_cTxt);
					cMesg = 2;
				}
				else if (strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
					wsprintf(G_cTxt, "Account(%s) Password Wrong(%s)(%s)", cAccountName, m_pAccountList[iAccount]->cPassword, cAccountPass);
					AccountLogList(G_cTxt);
					cMesg = 3;
				}
				break;

			case 1:
				SendEventToClient(NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, cData, 0, iClientH);
				return;
				break;

			case 2:
				SendEventToClient(NUCLEO_RESPONSE_PASSWORDMISMATCH, NUCLEO_RESPONSE_PASSWORDMISMATCH, cData, 0, iClientH);
				return;
				break;

			case 3:
				return;
				break;
		}

		switch (cMesg) {
			case 1:
				SendEventToClient(NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, cData, 0, iClientH);
				break;

			case 2:
				break;

			case 3:
				SendEventToClient(NUCLEO_RESPONSE_PASSWORDMISMATCH, NUCLEO_RESPONSE_PASSWORDMISMATCH, cData, 0, iClientH);
				break;

			case 0: //successful log on

				wsprintf(G_cTxt, "Account Login: (%s)", cAccountName);
				AccountLogList(G_cTxt);

				GetLocalTime(&SysTime);
				m_pAccountList[iAccount]->dAccountID = (int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime());

				cTotalChar = 0;
				for (i = 0; i < DEF_MAXCHARACTER; i++) {
					if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccount)) {
						ZeroMemory(cTotalCharNames[cTotalChar], sizeof(cTotalCharNames[cTotalChar]));
						memcpy_secure(cTotalCharNames[cTotalChar], m_pCharList[i]->cCharacterName, 11);//10 cCharName
						cTotalChar++;
					}
				}

				// LaloMiraAca
				if (cTotalChar > 4) {
					wsprintf(G_cTxt, "Hack Account: (%s)", cAccountName);
					ErrorLogList(G_cTxt);					
					SendEventToClient(NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, cData, 0, iClientH);
					return;
				}

				ZeroMemory(cData, sizeof(cData));
				cp = (char *)(cData);

				//Lalov9 login
				ip = (int *)cp;
				*ip = iUperVersion;
				cp += 4;
				ip = (int *)cp;
				*ip = iLowerVersion;
				cp += 4;
				ip = (int *)cp;
				*ip = iArgentVersion;
				cp += 4;

				// Account Status?
				cp++;

				dwp = (DWORD *)cp;
				*dwp = m_pAccountList[iAccount]->m_iAccntYear;
				cp += 2;

				dwp = (DWORD *)cp;
				*dwp = m_pAccountList[iAccount]->m_iAccntMonth;
				cp += 2;

				dwp = (DWORD *)cp;
				*dwp = m_pAccountList[iAccount]->m_iAccntDay;
				cp += 2;

				dwp = (DWORD *)cp;
				*dwp = m_pAccountList[iAccount]->m_iPassYear;
				cp += 2;

				dwp = (DWORD *)cp;
				*dwp = m_pAccountList[iAccount]->m_iPassMonth;
				cp += 2;

				dwp = (DWORD *)cp;
				*dwp = m_pAccountList[iAccount]->m_iPassDay;
				cp += 2;

				*cp = cTotalChar;
				cp++;

				for (i = 0; i < cTotalChar; i++) {				
					ZeroMemory(cMapName, sizeof(cMapName));
					ZeroMemory(cCharName, sizeof(cCharName));
					memcpy_secure(cCharName, cTotalCharNames[i], 11);//10

					memcpy_secure(cp, cCharName, 10);
					cp += 10;
					
					//if ((iGetCharacterData(cCharName, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr,
					if ((iGetCharacterData(cCharName, (char*)&cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr,						
						&iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE ) {
						*cp = 0;
						cp += 40;
					//	wsprintf(G_cTxt, "Hack Account: (%s)", cAccountName);
						ErrorLogList("Hack Account 1");
						SendEventToClient(NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, cData, 0, iClientH);
						return;
					}
					else {
						*cp = 1;
						cp++;

						wp = (WORD *)cp;
						*wp = (int)sAppr1;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (int)sAppr2;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (int)sAppr3;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (int)sAppr4;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (int)cSex;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (int)cSkin;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iLevel;
						cp += 2;

						dwp = (DWORD *)cp;
						*dwp = (WORD)iExp;
						cp += 4;

						wp = (WORD *)cp;
						*wp = (WORD)iStr;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iVit;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iDex;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iInt;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iMag;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iCharisma;
						cp += 2;

						ip = (int *)cp;
						*ip = iApprColor;
						cp += 4;

						wp = (WORD *)cp;
						*wp = (WORD)iSaveYear;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iSaveMonth;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iSaveDay;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iSaveHour;
						cp += 2;

						wp = (WORD *)cp;
						*wp = (WORD)iSaveMinute;
						cp += 2;

						memcpy_secure(cp, cMapName, 11);
						cp += 10;
					}
				}

				dwp = (DWORD *)cp; ///valid time
				*dwp = 11758870; // is number of valid time
				cp += 4;

				dwp = (DWORD *)cp; //ip
				*dwp = 11758874; // number to check valid time....
				cp += 4;

				SendEventToClient(DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_CONFIRM, cData, 26 + (cTotalChar * 65), iClientH);
			//	SendEventToClient(DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_CONFIRM, cData, 32 + (cTotalChar * 71), iClientH);
				break;
		}
/*#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestLogin");
	}
#endif*/
}

void CMainLog::ResponseCharacter(int iClientH, char *pData)
{
/*#ifdef DEF_DEBUG
	try {
#endif*/
		char cAccountPass[11], cWorldName[30]; //cp2 out going message
		int iMessage, iAccount, iTracker;
		SYSTEMTIME SysTime;
		//cTotalCharNames[5][11]
		char cData[3000], cFileName[255], cCharacterName[11], cTotalCharNames[10][11], cNewCharName[11], cAccountName[11], cTxt[11], cDir[255], cMapName[11];
		char *cp;
		DWORD *dwp;
		WORD *wp;
		short sAppr1, sAppr2, sAppr3, sAppr4;
		short iTempAppr2, iTempAppr3, iTempAppr1;
		char cSex, cSkin, cTotalChar, cNewGender, cNewSkin, cNewStr, cNewVit, cNewDex, cNewInt, cNewMag, cNewChr;

		int i, iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iExp, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
		int *ip;
		FILE * pFile;

		ZeroMemory(cData, sizeof(cData));

		cTotalChar = 0;
		iTracker = -1;
		iAccount = -1;
		pFile = 0;
		sAppr1 = 0;
		sAppr2 = 0;
		sAppr3 = 0;
		sAppr4 = 0;

		if (m_pClientList[iClientH] == NULL) return;
			
		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		ZeroMemory(cNewCharName, sizeof(cNewCharName));
		memcpy_secure(cNewCharName, cp, 10);
		cp += 10;

		ZeroMemory(cAccountName, sizeof(cAccountName));
		memcpy_secure(cAccountName, cp, 10);
		cp += 10;

		ZeroMemory(cAccountPass, sizeof(cAccountPass));
		memcpy_secure(cAccountPass, cp, 10);
		cp += 10;

		ZeroMemory(cWorldName, sizeof(cWorldName));
		memcpy_secure(cWorldName, cp, 30);
		cp += 30;

		if ((string(cNewCharName) == "Coins") || (string(cNewCharName) == "Coin") || (string(cNewCharName) == "coins") || (string(cNewCharName) == "coin"))
		{
			SendEventToClient(NUCLEO_RESPONSE_NEWCHARACTERFAILED, NUCLEO_RESPONSE_NEWCHARACTERFAILED, 0, 0, iClientH);
			return;
		}

		wsprintf(G_cTxt, "Request create new character(%s). Account(%s) Password(%s) OtherChars(%s)", cNewCharName, cAccountName, cAccountPass, cWorldName);
		AccountLogList(G_cTxt);
		
		cNewGender = *cp;
		cp++;

		cNewSkin = *cp;
		cp++;

		iTempAppr2 = *cp;
		cp++;

		iTempAppr3 = *cp;
		cp++;

		iTempAppr1 = *cp;
		cp++;

		cNewStr = *cp;
		cp++;

		cNewVit = *cp;
		cp++;

		cNewDex = *cp;
		cp++;

		cNewInt = *cp;
		cp++;

		cNewMag = *cp;
		cp++;

		cNewChr = *cp;
		cp++;

		wsprintf(G_cTxt, "CreateNewChar:(%s) Account:(%s) Str(%d)Int(%d)Vit(%d)Dex(%d)Mag(%d)Chr(%d)Gender(%d)", cNewCharName, cAccountName, cNewStr, cNewInt, cNewVit, cNewDex, cNewMag, cNewChr, cNewGender - 1);
		AccountLogList(G_cTxt);

		cTotalChar = 0;
		sAppr1 = 0;
		sAppr1 = sAppr1 | iTempAppr1;
		sAppr1 = sAppr1 | (iTempAppr2 << 8);
		sAppr1 = sAppr1 | (iTempAppr3 << 4);

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount);

		if (iMessage > 0) {
			SendEventToClient(NUCLEO_RESPONSE_NEWCHARACTERFAILED, NUCLEO_RESPONSE_NEWCHARACTERFAILED, 0, 0, iClientH);
			return;
		}		
		
		if (strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
			SendEventToClient(NUCLEO_RESPONSE_NEWCHARACTERFAILED, NUCLEO_RESPONSE_NEWCHARACTERFAILED, 0, 0, iClientH);
			return;
		}

		for (i = 0; i < DEF_MAXCHARACTER; i++)//DEF_MAXCHARACTER
			if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccount)) cTotalChar++;

		//lalo aca
		if (cTotalChar >= 4) {
			SendEventToClient(NUCLEO_RESPONSE_NEWCHARACTERFAILED, NUCLEO_RESPONSE_NEWCHARACTERFAILED, 0, 0, iClientH);
		//	ErrorLogList("(X) 5to Char no creado");
			ErrorLogList("(X) Cannot create character number 5");
			return;
		}

		GetLocalTime(&SysTime);
		m_pAccountList[iAccount]->dAccountID = (int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime());

		// Creating Character -> ZeroEoyPnk
		ZeroMemory(cFileName, sizeof(cFileName));
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");

		ZeroMemory(cTxt, sizeof(cTxt));
		wsprintf(cTxt, "AscII%d", *cNewCharName);
		strcat(cFileName, cTxt);

		ZeroMemory(cDir, sizeof(cDir));
		strcpy(cDir, cFileName);

		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cNewCharName);
		strcat(cFileName, ".txt");

		_mkdir("Character");
		_mkdir(cDir);

		ZeroMemory(cData, sizeof(cData));
		if (fopen(cFileName, "rt") != NULL) {
			wsprintf(G_cTxt, "(X) Cannot create new character - Already existing character(%s)", cFileName);
			AccountLogList(G_cTxt);

			SendEventToClient(NUCLEO_RESPONSE_ALREADYEXISTINGCHARACTER, NUCLEO_RESPONSE_ALREADYEXISTINGCHARACTER, 0, 0, iClientH);

			if (pFile != 0) fclose(pFile);
			return;
		}

		pFile = fopen(cFileName, "wt");
		if (pFile == NULL) {
			wsprintf(cTxt, "Cannot create new character - cannot create file : Name(%s)", cFileName);
			AccountLogList(cTxt);

			SendEventToClient(NUCLEO_RESPONSE_NEWCHARACTERFAILED, NUCLEO_RESPONSE_NEWCHARACTERFAILED, 0, 0, iClientH);
			return;
		}
		else {
			CreateNewCharacterFile(pData, cNewCharName, cAccountName, cAccountPass, cNewGender, cNewSkin, iTempAppr1, iTempAppr2, iTempAppr3, cNewStr, cNewVit, cNewDex, cNewInt, cNewMag, cNewChr, sAppr1, sAppr2, sAppr3, sAppr4);
			fwrite(pData, 1, strlen(pData), pFile);
			if (pFile != NULL) fclose(pFile);
		}

		if (SaveAccountInfo(iAccount, cAccountName, m_pAccountList[iAccount]->cWorldName, cNewCharName, 1, iClientH) == FALSE) { PutLogList("Cancelado"); }
		else GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount);

		cTotalChar = 0;
		for (i = 0; i < DEF_MAXCHARACTER; i++) {
			if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccount)) {
				ZeroMemory(cTotalCharNames[cTotalChar], sizeof(cTotalCharNames[cTotalChar]));
				memcpy_secure(cTotalCharNames[cTotalChar], m_pCharList[i]->cCharacterName, 11);//11
				cTotalChar++;
			}
		}

		cp = (char *)(pData);

		memcpy_secure(cp, cNewCharName, 10);
		cp += 10;

		*cp = (char)cTotalChar;
		cp++;

		for (i = 0; i < cTotalChar; i++) {
			memcpy_secure(cp, &cTotalCharNames[i], 11);//10
			cp += 10;

			ZeroMemory(cCharacterName, sizeof(cCharacterName));
			memcpy_secure(cCharacterName, &cTotalCharNames[i], 11);//10

		//	if ((iGetCharacterData(cCharacterName, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit,
			if ((iGetCharacterData(cCharacterName, (char*)&cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr, &iVit,
				&iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE) {
				*cp = 0;
				cp += 40;
			}
			else {
				*cp = 1;
				cp++;

				wp = (WORD *)cp;
				*wp = (int)sAppr1;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (int)sAppr2;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (int)sAppr3;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (int)sAppr4;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (int)cSex;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (int)cSkin;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iLevel;
				cp += 2;

				dwp = (DWORD *)cp;
				*dwp = (WORD)iExp;
				cp += 4;

				wp = (WORD *)cp;
				*wp = (WORD)iStr;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iVit;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iDex;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iInt;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iMag;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iCharisma;
				cp += 2;

				ip = (int *)cp;
				*ip = iApprColor;
				cp += 4;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveYear;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveMonth;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveDay;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveHour;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveMinute;
				cp += 2;

				memcpy_secure(cp, cMapName, 11);
				cp += 10;
			}
		}
		SendEventToClient(NUCLEO_RESPONSE_NEWCHARACTERCREATED, NUCLEO_RESPONSE_NEWCHARACTERCREATED, pData, 11 + (cTotalChar * 65), iClientH);
/*#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: ResponseCharacter");
	}
#endif*/
}

void CMainLog::DeleteCharacter(int iClientH, char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char *cp, cAccountName[11], cAccountPass[11], cWorldName[30], cData[500], cTotalChar, cCharName[11], cTotalCharNames[10][11]; //cTotalCharNames[5][11]
		DWORD *dwp;
		int i, iMessage, iAccount, iTracker;
		SYSTEMTIME SysTime;
		WORD *wp;
		short sAppr1, sAppr2, sAppr3, sAppr4;
		char cMapName[11], cSex, cSkin;
		int iLevel, iStr, iVit, iInt, iDex, iMag, iCharisma, iExp, iApprColor, iSaveYear, iSaveMonth, iSaveDay, iSaveHour, iSaveMinute;
		int *ip;

		ZeroMemory(cAccountName, sizeof(cAccountName));
		ZeroMemory(cAccountPass, sizeof(cAccountPass));
		ZeroMemory(cWorldName, sizeof(cWorldName));
		ZeroMemory(cCharName, sizeof(cCharName));
		ZeroMemory(cData, sizeof(cData));

		iAccount = -1;
		iTracker = -1;
		cTotalChar = 0;

		if (m_pClientList[iClientH] == NULL) return;

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cCharName, cp, 10);
		cp += 10;
		memcpy_secure(cAccountName, cp, 10);
		cp += 10;
		memcpy_secure(cAccountPass, cp, 10);
		cp += 10;
		memcpy_secure(cWorldName, cp, 30);
		cp += 30;

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount);
		if (iMessage > 0) {
			SendEventToClient(NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, 0, 0, iClientH);
			return;
		}

		if (strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
			SendEventToClient(NUCLEO_RESPONSE_PASSWORDMISMATCH, NUCLEO_RESPONSE_PASSWORDMISMATCH, 0, 0, iClientH);
			return;
		}

		//delete charcter in account file
		if (SaveAccountInfo(iAccount, NULL, m_pAccountList[iAccount]->cWorldName, cCharName, 3, iClientH) == FALSE) {
			SendEventToClient(NUCLEO_RESPONSE_NOTEXISTINGCHARACTER, NUCLEO_RESPONSE_NOTEXISTINGCHARACTER, 0, 0, iClientH);
			return;
		}

		for (i = 0; i < DEF_MAXCHARACTER; i++) {
			if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccount)) {
				if (memcmp(m_pCharList[i]->cCharacterName, cCharName, strlen(cCharName)) == 0) {
					m_iValidCharacters--;
					delete m_pCharList[i];
					m_pCharList[i] = NULL;
				}
			}
		}

		GetLocalTime(&SysTime);
		m_pAccountList[iAccount]->dAccountID = (int)(SysTime.wYear + SysTime.wMonth + SysTime.wDay + SysTime.wHour + SysTime.wMinute + timeGetTime());

		cTotalChar = 0;
		for (i = 0; i < DEF_MAXCHARACTER; i++) {
			if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccount)) {
				ZeroMemory(cTotalCharNames[cTotalChar], sizeof(cTotalCharNames[cTotalChar]));
				memcpy_secure(cTotalCharNames[cTotalChar], m_pCharList[i]->cCharacterName, 11);//10
				cTotalChar++;
			}
		}

		cp = (char *)(cData);

		*cp = 0;
		cp++;

		*cp = cTotalChar;
		cp++;

		for (i = 0; i < cTotalChar; i++) {
			ZeroMemory(cMapName, sizeof(cMapName));
			ZeroMemory(cCharName, sizeof(cCharName));
			memcpy_secure(cCharName, &cTotalCharNames[i], 11);//10

			memcpy_secure(cp, cCharName, 10);
			cp += 10;

			//if ((iGetCharacterData(cCharName, cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr,
			if ((iGetCharacterData(cCharName, (char*)&cMapName, &sAppr1, &sAppr2, &sAppr3, &sAppr4, &iApprColor, &cSex, &cSkin, &iLevel, &iExp, &iStr,
				&iVit, &iDex, &iInt, &iMag, &iCharisma, &iSaveYear, &iSaveMonth, &iSaveDay, &iSaveHour, &iSaveMinute)) != TRUE)
			{
				*cp = 0;
				cp += 40;
			}
			else {
				*cp = 1;
				cp++;

				wp = (WORD *)cp;
				*wp = (WORD)sAppr1;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)sAppr2;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)sAppr3;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)sAppr4;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)cSex;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)cSkin;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iLevel;
				cp += 2;

				dwp = (DWORD *)cp;
				*dwp = (DWORD)iExp;
				cp += 4;

				wp = (WORD *)cp;
				*wp = (WORD)iStr;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iVit;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iDex;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iInt;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iMag;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iCharisma;
				cp += 2;

				ip = (int *)cp;
				*ip = iApprColor;
				cp += 4;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveYear;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveMonth;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveDay;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveHour;
				cp += 2;

				wp = (WORD *)cp;
				*wp = (WORD)iSaveMinute;
				cp += 2;

				memcpy_secure(cp, cMapName, 11);
				cp += 10;
			}
		}
		SendEventToClient(NUCLEO_RESPONSE_CHARACTERDELETED, NUCLEO_RESPONSE_CHARACTERDELETED, cData, 2 + (cTotalChar * 65), iClientH);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: DeleteCharacter");
	}
#endif
}

void CMainLog::RequestEnterGame(int iClientH, char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char *cp, *cp2, cData[100], cData2[100], cAccountName[11], cAccountPass[11], cWorldName[30], m_cMapName[31], cCharName[15], G_cCmdLineTokenA[120];
		int  i, x, m_iLevel, iMessage, iAccount, iGameServer, iRet, MapServer;
		DWORD *dwp;
		WORD * wpMsgType, *wp, wCount = 5;
		BOOL bMapOnline = FALSE;

		char cGameServerName[11];

		ZeroMemory(cAccountName, sizeof(cAccountName));
		ZeroMemory(cAccountPass, sizeof(cAccountPass));
		ZeroMemory(cWorldName, sizeof(cWorldName));
		ZeroMemory(m_cMapName, sizeof(m_cMapName));
		ZeroMemory(cData, sizeof(cData));
		ZeroMemory(cData2, sizeof(cData2));
		ZeroMemory(G_cCmdLineTokenA, sizeof(G_cCmdLineTokenA));

		if (m_pClientList[iClientH] == NULL) return;
		wpMsgType = (WORD *)(pData + DEF_INDEX2_MSGTYPE);

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cCharName, cp, 10);
		cp += 10;

		memcpy_secure(m_cMapName, cp, 10);
		cp += 10;

		memcpy_secure(cAccountName, cp, 10);
		cp += 10;

		memcpy_secure(cAccountPass, cp, 10);
		cp += 10;

		dwp = (DWORD *)cp;
		m_iLevel = *dwp;
		cp += 4;

		memcpy_secure(cWorldName, cp, 30);
		cp += 30;

		memcpy_secure(cp, G_cCmdLineTokenA, 120);
		cp += 120;

		if (*wpMsgType == NUCLEO_REQUEST_NOENTER_FORCEDISCONN) {
			for (i = 1; i < DEF_MAXACCOUNTS; i++) {
				if ((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cWorldName, cWorldName) == 0)
					&& (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) && (m_pAccountList[i]->InGame == true)) {

					ZeroMemory(cGameServerName, sizeof(cGameServerName));
					memcpy_secure(cGameServerName, m_pAccountList[i]->m_cOnGameServer, 11);

					if (strlen(cGameServerName) < 5) {
						m_pAccountList[i]->InGame = false;
						OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[i]->cAccountName, 0, 0, 0);
					}

					for (x = 0; x < DEF_MAXGAME; x++) {
						if (m_pGameList[x] != NULL) {
							if (strcmp(m_pGameList[x]->m_cGameServerName, cGameServerName) == TRUE) {
								MapServer = m_pGameList[x]->m_iTrackSock;

								ZeroMemory(cData, sizeof(cData));

								dwp = (DWORD *)cData;
								*dwp = NUCLEO_REQUEST_FORCEDISCONECTACCOUNT;

								wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
								*wp = wCount;

								cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
								memcpy_secure(cp, cAccountName, 10);
								cp += 10;

								wsprintf(G_cTxt, "Force Disconnect Account(%s) in Game Server(%s)... Count(%d)", cAccountName, cGameServerName, wCount);
								AccountLogList(G_cTxt);

								if (m_pMapServerList[MapServer] == NULL) return;
								iRet = m_pMapServerList[MapServer]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);

								switch (iRet) {
									case DEF_XSOCKEVENT_QUENEFULL:
									case DEF_XSOCKEVENT_SOCKETERROR:
									case DEF_XSOCKEVENT_CRITICALERROR:
									case DEF_XSOCKEVENT_SOCKETCLOSED:
										delete m_pClientList[MapServer];
										m_pClientList[MapServer] = NULL;
								}
							}
						}
					}
					SendEventToClient(NUCLEO_RESPONSE_FORCEDISCONN, NUCLEO_RESPONSE_FORCEDISCONN, 0, 0, iClientH);
					return;
				}
			}
		}

		for (i = 0; i < DEF_MAXACCOUNTS; i++) {
			if ((m_pAccountList[i] != NULL)) {
				if ((strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) && (m_pAccountList[i]->InGame == true)) {
					cp2 = (char *)(cData); //outgoing messag - to WLserv
					*cp2 = 1;
					SendEventToClient(NUCLEO_RESPONSE_PLAYING, NUCLEO_RESPONSE_PLAYING, cData, 1, iClientH);
					return;
				}
			}
		}

		if (bShutDown == TRUE) {
			cp2 = (char *)(cData); //outgoing messag - to WLserv
			*cp2 = 5;
			SendEventToClient(NUCLEO_RESPONSE_REJECT, NUCLEO_RESPONSE_REJECT, cData, 1, iClientH);
			return;
		}

		iMessage = GetAccountInfo(iClientH, cAccountName, cAccountPass, cWorldName, &iAccount);
		if (iMessage > 0) {
			cp2 = (char *)(cData); //outgoing messag - to WLserv
			*cp2 = 4;
			SendEventToClient(NUCLEO_RESPONSE_REJECT, NUCLEO_RESPONSE_REJECT, cData, 1, iClientH);
			return;
		}

		if (strcmp(m_pAccountList[iAccount]->cPassword, cAccountPass) != 0) {
			cp2 = (char *)(cData); //outgoing messag - to WLserver
			*cp2 = 4;
			SendEventToClient(NUCLEO_RESPONSE_REJECT, NUCLEO_RESPONSE_REJECT, cData, 1, iClientH);
			return;
		}

		for (i = 0; i < DEF_MAXGAME; i++) {
			if ((m_pGameList[i] != NULL) && (strcmp(m_pGameList[i]->m_cWorldName, cWorldName) == 0)) {
				for (x = 0; x < DEF_MAXMAPS; x++) {
					if ((m_pMapList[x] != NULL) && (m_pMapList[x]->iIndex == i) && (strcmp(m_pMapList[x]->m_cMapName, m_cMapName) == 0)) {
						iGameServer = i;
						bMapOnline = TRUE;
					}
				}
			}
		}

		if (bMapOnline == FALSE) {
			cp2 = (char *)(cData); //outgoing messag - to WLserver
			*cp2 = 3;
			SendEventToClient(NUCLEO_RESPONSE_REJECT, NUCLEO_RESPONSE_REJECT, cData, 1, iClientH);
			wsprintf(G_cTxt, "GameServer(%s-%s) for %s is not registered.", cWorldName, m_cMapName, cAccountName);
			AccountLogList(G_cTxt);
			return;
		}

		cp2 = (char *)(cData); //outgoing messag - to WLserver

		memcpy_secure(cp2, cAccountName, 10);
		cp2 += 10;

		memcpy_secure(cp2, cAccountPass, 10);
		cp2 += 10;

		dwp = (DWORD *)cp2;
		*dwp = m_iLevel;
		cp2 += 4;

		cp2 = (char *)(cData2); //outgoing messag - to Client

		memcpy_secure(cp2, m_pGameList[iGameServer]->m_cGameServerAddress, 16);
		cp2 += 16;

		dwp = (DWORD *)cp2;
		*dwp = m_pGameList[iGameServer]->m_iGamePort;
		cp2 += 2;

		memcpy_secure(cp2, m_pGameList[iGameServer]->m_cGameName, 20);
		cp2 += 20;


		memcpy_secure(m_pClientList[iClientH]->m_cAccountName, cAccountName, 11);
		memcpy_secure(m_pClientList[iClientH]->m_cWorldName, cWorldName, 30);
		_RegistrarIPCuenta(m_pClientList[iClientH]->m_cIPaddress, cAccountName); // para los logs de ataque

		if (OnPlayerAccountMessage(DEF_MSGACCOUNTSET_INIT, cAccountName, cAccountPass, 0, m_iLevel)) {
			SendEventToClient(NUCLEO_RESPONSE_CONFIRM, NUCLEO_RESPONSE_CONFIRM, cData2, 38, iClientH);
		}
		else OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestEnterGame");
	}
#endif
}

//BOOL CMainLog::SaveAccountInfo(int iAccount, char cAccountName[11], char cTemp[11], char cCharName[30], char cMode, int iClientH)
BOOL CMainLog::SaveAccountInfo(int iAccount, char cAccountName[11], char cTemp[11], char cCharName[11], char cMode, int iClientH)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cFileName[255], cDir[63], cTxt[50], cAccount[11], cTxt2[2000], cData[2000];
		int i, iSize, iTest = -1;
		//HANDLE hFile;
		FILE * pFile;

		if (m_pClientList[iClientH] == NULL) return FALSE;

		memset(cData, 0, 2000);
		memset(cTxt2, 0, 2000);

		ZeroMemory(cAccount, sizeof(cAccount));
		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTxt2, sizeof(cTxt2));
		ZeroMemory(cData, sizeof(cData));

		if ((cAccountName != NULL) && (iAccount == -1)) {
			for (i = 0; i < DEF_MAXACCOUNTS; i++) {
				if ((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
					memcpy_secure(cAccount, cAccountName, 11);
					iAccount = i;
					break;
				}
			}
		}
		else if (iAccount != -1) memcpy_secure(cAccount, m_pAccountList[iAccount]->cAccountName, 11);

		if ((iAccount == -1)) return FALSE;

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Account");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cAccount);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cAccount);
		strcat(cFileName, ".txt");

		/*hFile = CreateFileA(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		iSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);*/

		switch (cMode) {
			case 1: //save new character
				pFile = fopen(cFileName, "at");
				if (pFile == NULL) {
					wsprintf(G_cTxt, "(X) Account none exist : Name(%s)", cAccount);
					AccountLogList(G_cTxt);
					if (pFile != NULL) fclose(pFile);
					return FALSE;
				}

				wsprintf(cTxt, "account-character-%s  = %s\n", cTemp, cCharName);

				fwrite(cTxt, 1, strlen(cTxt), pFile);
				if (pFile != NULL) fclose(pFile);
				return TRUE;
				break;

			case 2: //password change
				break;

			case 3: //delete character
				wsprintf(G_cTxt, "(X) Character Delete(%s)", cCharName);
				AccountLogList(G_cTxt);

				wsprintf(cTxt, "account-character-%s  = %s\n", cTemp, cCharName);

				pFile = fopen(cFileName, "rt");
				if (pFile == NULL) {
					SendEventToClient(NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, NUCLEO_RESPONSE_NOTEXISTINGACCOUNT, 0, 0, iClientH);
					if (pFile != NULL) fclose(pFile);
					return FALSE;
				}

				char linea[100];
				while (!feof(pFile)) {
					ZeroMemory(linea, sizeof(linea));
					fgets(linea, sizeof(linea), pFile);
					if (memcmp(linea, cTxt, strlen(cTxt)) != 0)
						strcat(cTxt2, linea);
					else iTest = 1;
				}
				fclose(pFile);

				if (iTest == -1) {
					SendEventToClient(NUCLEO_RESPONSE_NOTEXISTINGCHARACTER, NUCLEO_RESPONSE_NOTEXISTINGCHARACTER, 0, 0, iClientH);
					fclose(pFile);
					return FALSE;
				}
				else {
					SaveInfo(cFileName, cTxt2, 1);

					ZeroMemory(cFileName, sizeof(cFileName));
					strcat(cFileName, "Character");
					strcat(cFileName, "\\");
					strcat(cFileName, "\\");
					wsprintf(cTxt, "AscII%d", *cCharName);
					strcat(cFileName, cTxt);
					strcpy(cDir, cFileName);
					strcat(cFileName, "\\");
					strcat(cFileName, "\\");
					strcat(cFileName, cCharName);
					strcat(cFileName, ".txt");

					DeleteFile(cFileName);

					wsprintf(G_cTxt, "Character(%s) deleted! Account(%s)", cCharName, cAccountName);
					AccountLogList(G_cTxt);
				}

				if (pFile != NULL) fclose(pFile);
				return TRUE;
				break;
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: SaveAccountInfo");
	}
#endif
	return FALSE;
}

void CMainLog::SaveInfo(char cFileName[255], char *pData, DWORD dwStartSize)
{
#ifdef DEF_DEBUG
	try {
#endif
		FILE * pFile;

		pFile = fopen(cFileName, "wt");
		if (pFile != NULL) {
			if (strlen(pData) > 0) fwrite(pData, dwStartSize, strlen(pData), pFile);
			fclose(pFile);
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: SaveInfo");
	}
#endif
}

int CMainLog::GetAccountInfo(int iClientH, char cAccountName[11], char cAccountPass[11], char cWorldName[30], int * iAccount)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cFileName[255], cDir[63], cTxt[50], seps[] = "= \t\n";
		char *cp, *token, cReadModeA = 0, cReadModeB = 0;
		class CStrTok * pStrTok;
		int i, iAccountid = 0;
		//HANDLE hFile;
		DWORD dwFileSize;
		FILE * pFile;
		BOOL InClass = false;

		ZeroMemory(cTxt, sizeof(cTxt));
		wsprintf(cTxt, "account-character-%s", cWorldName);

		if (strlen(cAccountName) == 0) return 1; // if account blank
		if (strlen(cAccountPass) == 0) return 2; // if password blank
		if ((strlen(cWorldName) == 0)) return 3; // if world dosnt exist

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Account");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cAccountName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cAccountName);
		strcat(cFileName, ".txt");

		cReadModeA = 0;
		cReadModeB = 0;

		/*hFile = CreateFileA(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);

		if (hFile != INVALID_HANDLE_VALUE)  CloseHandle(hFile);*/
		pFile = fopen(cFileName, "rt");

		if (pFile == NULL) {
			wsprintf(G_cTxt, "Account Does not Exist (%s)", cFileName);
			AccountLogList(G_cTxt);

			wsprintf(G_cTxt, "Error Num: %d", GetLastError());
			ErrorLogList(G_cTxt);
			return 1;
		}

		fseek(pFile, 0, SEEK_END);
		dwFileSize = ftell(pFile);
		rewind(pFile);

		cp = (char*)malloc(dwFileSize + 1);
		size_t rd_cp = fread(cp, 1, dwFileSize, pFile);
		cp[rd_cp] = '\0';
		pStrTok = new class CStrTok(cp, seps);

		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
					case 1:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							return 1;
						}

						for (i = 1; i < DEF_MAXACCOUNTS; i++) {
							if (m_pAccountList[i] != NULL) {
								if (strcmp(m_pAccountList[i]->cAccountName, token) == 0 && strcmp(m_pAccountList[i]->cPassword, cAccountPass) == 0) {
									strcpy(m_pClientList[iClientH]->m_cAccountName, token);
									iAccountid = i;
									*iAccount = i;
									InClass = true;
								}
							}
						}

						if (!InClass) {
							for (i = 1; i < DEF_MAXACCOUNTS; i++) {
								if (m_pAccountList[i] == NULL) {
									m_pAccountList[i] = new class CAccount(token, cWorldName, iClientH);
									strcpy(m_pClientList[iClientH]->m_cAccountName, token); // save account name
									m_iValidAccounts++;
									iAccountid = i;
									*iAccount = i;
									break;
								}
							}
						}
						cReadModeA = 0;
						break;

					case 2:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							delete m_pAccountList[iAccountid];
							m_pAccountList[iAccountid] = NULL;
							return 1;
						}
						strcpy(m_pAccountList[iAccountid]->cPassword, token);
						cReadModeA = 0;
						break;

					case 3:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							delete m_pAccountList[iAccountid];
							m_pAccountList[iAccountid] = NULL;
							return 1;
						}
						m_pAccountList[iAccountid]->iAccountValid = atoi(token);
						cReadModeA = 0;
						break;

					case 4:
						switch (cReadModeB) {
							case 1:
								if (strlen(token) <= 0) {
									delete pStrTok;
									free(cp);
									delete m_pAccountList[iAccountid];
									m_pAccountList[iAccountid] = NULL;
									return 1;
								}
								m_pAccountList[iAccountid]->m_iAccntYear = atoi(token);
								cReadModeB = 2;
								break;

							case 2:
								if (strlen(token) <= 0) {
									delete pStrTok;
									free(cp);
									delete m_pAccountList[iAccountid];
									m_pAccountList[iAccountid] = NULL;
									return 1;
								}
								m_pAccountList[iAccountid]->m_iAccntMonth = atoi(token);
								cReadModeB = 3;
								break;

							case 3:
								if (strlen(token) <= 0) {
									delete pStrTok;
									free(cp);
									delete m_pAccountList[iAccountid];
									m_pAccountList[iAccountid] = NULL;
									return 1;
								}
								m_pAccountList[iAccountid]->m_iAccntDay = atoi(token);
								cReadModeB = 0;
								cReadModeA = 0;
								break;
						}
						break;

					case 5:
						switch (cReadModeB) {
							case 1:
								if (strlen(token) <= 0) {
									delete pStrTok;
									free(cp);
									delete m_pAccountList[iAccountid];
									m_pAccountList[iAccountid] = NULL;
									return 1;
								}
								m_pAccountList[iAccountid]->m_iPassYear = atoi(token);
								cReadModeB = 2;
								break;

							case 2:
								if (strlen(token) <= 0) {
									delete pStrTok;
									free(cp);
									delete m_pAccountList[iAccountid];
									m_pAccountList[iAccountid] = NULL;
									return 1;
								}
								m_pAccountList[iAccountid]->m_iPassMonth = atoi(token);
								cReadModeB = 3;
								break;

							case 3:
								if (strlen(token) <= 0) {
									delete pStrTok;
									free(cp);
									delete m_pAccountList[iAccountid];
									m_pAccountList[iAccountid] = NULL;
									return 1;
								}
								m_pAccountList[iAccountid]->m_iPassDay = atoi(token);
								cReadModeB = 0;
								cReadModeA = 0;
								break;
						}
						break;

					case 6:
						for (i = 0; i < DEF_MAXCHARACTER; i++) {
							if (m_pCharList[i] == NULL) {
								m_pCharList[i] = new class CCharacter(token, iAccountid);
								m_iValidCharacters++;
								break;
							}
						}
						cReadModeA = 0;
						break;
				}
			}
			else {
				if (memcmp(token, "account-name", 12) == 0)			cReadModeA = 1;
				if (memcmp(token, "account-password", 16) == 0)			cReadModeA = 2;
				if (memcmp(token, "account-valid-time", 18) == 0)			cReadModeA = 3;
				if (memcmp(token, "account-valid-date", 18) == 0) {
					cReadModeB = 1;
					cReadModeA = 4;
				}

				if (memcmp(token, "account-change-password", 23) == 0) {
					cReadModeB = 1;
					cReadModeA = 5;
				}

				if (memcmp(token, "account-character", 17) == 0)			cReadModeA = 6;
				else if ((memcmp(token, cTxt, strlen(cTxt)) == 0))			cReadModeA = 6;
			}
			token = pStrTok->pGet();
		}

		delete pStrTok;
		free(cp);
		if (pFile != NULL) fclose(pFile);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: GetAccountInfo");
	}
#endif
	return 0;
}

BOOL CMainLog::bReadServerConfigFile(char *cFn)
{
	FILE * pFile;
	HANDLE hFile;
	DWORD  dwFileSize;
	char * cp, *token, cReadMode = 0;
	char seps[] = "= \t\n";
	class CStrTok * pStrTok;

	pFile = NULL;

	hFile = CreateFileA(cFn, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);

	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	pFile = fopen(cFn, "rt");
	if (pFile == NULL) {
		ErrorLogList("Cannot open configuration file Server.cfg");
		return FALSE;
	}
	else {
		PutLogList("Reading configuration file...");
		cp = new char[dwFileSize + 1];
		ZeroMemory(cp, dwFileSize + 1);
		fread(cp, dwFileSize, 1, pFile);
		pStrTok = new class CStrTok(cp, seps);

		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadMode != 0) {
				switch (cReadMode) {
					case 1: // Mainserver addresss
						strncpy(m_cMainServerAddress, token, 15);
						wsprintf(G_cTxt, "Server Address : %s", m_cMainServerAddress);
						ConfigList(G_cTxt);
						cReadMode = 0;
						break;

					case 3: // internal-main-server-port
						m_iMainServerInternalPort = atoi(token);
						wsprintf(G_cTxt, "Server Internal Port : %d", m_iMainServerInternalPort);
						ConfigList(G_cTxt);
						cReadMode = 0;
						break;

					case 4:	// log server port
						m_iMainServerPort = atoi(token);
						wsprintf(G_cTxt, "Conexion Server Port : %d", m_iMainServerPort);
						ConfigList(G_cTxt);
						cReadMode = 0;
						break;

					case 5:	// world-server-name
						strncpy(m_cWorldName, token, 10);
						wsprintf(G_cTxt, "Server Name : %s", m_cWorldName);
						ConfigList(G_cTxt);
						cReadMode = 0;
						break;

					case 6: //game-server-address
						strncpy(m_cMapServerAddress, token, 16);//15
						wsprintf(G_cTxt, "MapServer-Address : %s", m_cMapServerAddress);
						ConfigList(G_cTxt);
						cReadMode = 0;
						break;
				}
			}
			else {
				if (memcmp(token, "log-server-address", 18) == 0)			cReadMode = 1;
				if (memcmp(token, "internal-log-server-port", 24) == 0)		cReadMode = 3;
				if (memcmp(token, "log-server-port", 15) == 0)				cReadMode = 4;
				if (memcmp(token, "world-server-name", 20) == 0)			cReadMode = 5;
				if (memcmp(token, "MapServer-address", 17) == 0)			cReadMode = 6;
			}
			token = pStrTok->pGet();
		}
		delete pStrTok;
		delete [] cp;
	}

	if (pFile != NULL) fclose(pFile);
	return TRUE;
}

void CMainLog::CreateNewCharacterFile(char *pData, char * cNewCharName, char * cAccountName, char * cPassword, char cNewGender, char cNewSkin, int iTempAppr1, int iTempAppr2, int iTempAppr3, char cNewStr, char cNewVit, char cNewDex, char cNewInt, char cNewMag, char cNewChr, short sAppr1, short sAppr2, short sAppr3, short sAppr4)
{
#ifdef DEF_DEBUG
	try {
#endif
		SYSTEMTIME SysTime;
		char cTxt[120];
		int i, iRand;

		ZeroMemory(cTxt, sizeof(cTxt));
		GetLocalTime(&SysTime);
		ZeroMemory(pData, sizeof(pData));
		strcat(pData, "[FILE-DATE]\n\n");

		wsprintf(cTxt, "file-saved-date:  Time(%d/%d/%d/%d/%d)", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
		strcat(pData, cTxt);
		strcat(pData, "\n\n");

		strcat(pData, "[NAME-ACCOUNT]\n\n");

		strcat(pData, "character-name     = ");
		strcat(pData, cNewCharName);
		strcat(pData, "\n");

		strcat(pData, "account-name       = ");
		strcat(pData, cAccountName);
		strcat(pData, "\n");

		strcat(pData, "account-password   = ");
		strcat(pData, cPassword);
		strcat(pData, "\n\n");

		strcat(pData, "[STATUS]\n\n");

		strcat(pData, "character-location   = NONE\n");
		strcat(pData, "character-guild-name = NONE\n");
		strcat(pData, "character-guild-rank = -1\n");
		strcat(pData, "\n");

		strcat(pData, "character-loc-map  = default\n");
		strcat(pData, "character-loc-x\t  = -1\n");
		strcat(pData, "character-loc-y\t  = -1\n\n");

		wsprintf(cTxt, "character-HP       = %d", 37);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-MP       = %d", 27);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-SP       = %d", 22);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "character-LEVEL    = 1\n");

		wsprintf(cTxt, "character-STR      = %d", cNewStr);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-INT      = %d", cNewInt);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-VIT      = %d", cNewVit);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-DEX      = %d", cNewDex);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-MAG      = %d", cNewMag);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "character-CHARISMA = %d", cNewChr);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		strcat(pData, "character-LUCK     = 10");
		strcat(pData, "\n");

		strcat(pData, "character-EXP      = 0");
		strcat(pData, "\n");

		strcat(pData, "character-LU_Pool    = 0");
		strcat(pData, "\n");


		strcat(pData, "character-EK-Count  = 0");
		strcat(pData, "\n");

		strcat(pData, "character-PK-Count  = 0");
		strcat(pData, "\n");

		wsprintf(cTxt, "sex-status       = %d", cNewGender);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "skin-status      = %d", cNewSkin);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "hairstyle-status = %d", iTempAppr2);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "haircolor-status = %d", iTempAppr3);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "underwear-status = %d", iTempAppr1);
		strcat(pData, cTxt);
		strcat(pData, "\n\n");

		wsprintf(cTxt, "appr1 = %d", sAppr1);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "appr2 = %d", sAppr2);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "appr3 = %d", sAppr3);
		strcat(pData, cTxt);
		strcat(pData, "\n");

		wsprintf(cTxt, "appr4 = %d", sAppr4);
		strcat(pData, cTxt);
		strcat(pData, "\n\n");

		strcat(pData, "[ITEMLIST]\n\n");
		strcat(pData, "character-item = BarrackSword		 1 0 0 0 0 0 0 0 0 500 0 0 -4\n");//prueba
		//strcat(pData, "character-item = Dagger				 1 0 0 0 0 0 0 0 0 300 0 0 -4\n");
		strcat(pData, "character-item = RecallScroll         1 0 0 0 0 0 0 0 0 120 0 113 49\n");
		strcat(pData, "character-item = RedCandy			 1 0 0 0 0 0 0 0 0 300 0 22 -1\n");
		strcat(pData, "character-item = BlueCandy		     1 0 0 0 0 0 0 0 0 300 0 50 -1\n");
		strcat(pData, "character-item = GreenCandy			 1 0 0 0 0 0 0 0 0 300 0 78 0\n");
		strcat(pData, "character-item = WoodShield			 1 0 0 0 0 0 0 0 0 300 0 150 2\n");
		strcat(pData, "character-item = PowerGreenPotion	 1 0 0 0 0 0 0 0 0 600 0 119 0\n");
		strcat(pData, "character-item = Gold				 5000 0 0 0 0 0 0 0 0 600 0 170 57\n");//cambiado
		strcat(pData, "character-item = MagicWand(MS0)		 1 0 0 0 0 0 0 0 0 1200 0 105 -4\n");
		strcat(pData, "\n");
		strcat(pData, "\n");

		strcat(pData, "[MAGIC-SKILL-MASTERY]\n\n");
		strcat(pData, "magic-mastery     = 0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
		strcat(pData, "\n\n");

		strcat(pData, "skill-mastery     = ");
		for (i = 0; i < 60; i++) {
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "%d ", 0);
			strcat(pData, cTxt);
		}
		strcat(pData, "\n");

		strcat(pData, "skill-SSN     = ");
		for (i = 0; i < 60; i++) strcat(pData, "0 ");
		strcat(pData, "\n\n");

		strcat(pData, "[ITEM-EQUIP-STATUS]\n\n");
		strcat(pData, "item-equip-status = 00000000000000000000000000000000000000000000000000");
		strcat(pData, "\n\n");

		strcat(pData, "[EOF]");
		strcat(pData, "\n\n\n\n");

#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: CreateNewCharacterFile");
	}
#endif
}

int CMainLog::iGetCharacterData(char * cCharName, char * cMapName, short * sAppr1, short * sAppr2, short * sAppr3, short * sAppr4, int * iApprColor, char * cSex, char * cSkin, int * iLevel, int * iExp, int * iStr, int * iVit, int * iDex, int * iInt, int * iMag, int * iCharisma, int * iSaveYear, int * iSaveMonth, int * iSaveDay, int * iSaveHour, int * iSaveMinute)
{
#ifdef DEF_DEBUG
	try {
#endif
		DWORD dwFileSize, lpNumberOfBytesRead;
		char * cp, *token, cReadModeA, cReadModeB, cFileName[255], cDir[11];
		char seps[] = "= \t\n";
		class CStrTok * pStrTok;
		//HANDLE hFile;
		FILE * pFile;

		*iApprColor = 0;
		if (iSaveYear != 0) *iSaveYear = 0;
		if (iSaveMonth != 0) *iSaveMonth = 0;
		if (iSaveDay != 0) *iSaveDay = 0;
		if (iSaveHour != 0) *iSaveHour = 0;
		if (iSaveMinute != 0) *iSaveMinute = 0;


		ZeroMemory(cFileName, sizeof(cFileName));
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharName);
		strcat(cFileName, ".txt");

		/*hFile = CreateFileA(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) return -1;
		dwFileSize = GetFileSize(hFile, NULL);

		cp = new char[dwFileSize+1];
		ZeroMemory(cp, dwFileSize+1);

		ReadFile(hFile, cp, dwFileSize, &lpNumberOfBytesRead, NULL);
		CloseHandle(hFile);*/


		pFile = fopen(cFileName, "rt");

		if (pFile == NULL) return -1;

		fseek(pFile, 0, SEEK_END);
		dwFileSize = ftell(pFile);
		rewind(pFile);

		cp = (char*)malloc(dwFileSize + 1);
		size_t rd_cp = fread(cp, 1, dwFileSize, pFile);
		cp[rd_cp] = '\0';
		pStrTok = new class CStrTok(cp, seps);

		cReadModeA = 0;
		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
					case 1: // appr1
						*sAppr1 = (short)atoi(token);
						cReadModeA = 0;
						break;

					case 2: // appr2
						*sAppr2 = (short)atoi(token);
						cReadModeA = 0;
						break;

					case 3: // appr3
						*sAppr3 = (short)atoi(token);
						cReadModeA = 0;
						break;

					case 4: // appr4
						*sAppr4 = (short)atoi(token);
						cReadModeA = 0;
						break;

					case 5: // sex-status
						*cSex = (char)atoi(token);
						cReadModeA = 0;
						break;

					case 6: // skin-status
						*cSkin = (char)atoi(token);
						cReadModeA = 0;
						break;

					case 7: // character-LEVEL
						*iLevel = atoi(token);
						cReadModeA = 0;
						break;

					case 8: // character-STR
						*iStr = atoi(token);
						cReadModeA = 0;
						break;

					case 9: // character-VIT
						*iVit = atoi(token);
						cReadModeA = 0;
						break;

					case 10: // character-DEX
						*iDex = atoi(token);
						cReadModeA = 0;
						break;

					case 11: // character-INT
						*iInt = atoi(token);
						cReadModeA = 0;
						break;

					case 12: // character-MAG
						*iMag = atoi(token);
						cReadModeA = 0;
						break;

					case 13: // character-CHARISMA
						*iCharisma = atoi(token);
						cReadModeA = 0;
						break;

					case 14: // character-EXP
						*iExp = atoi(token);
						cReadModeA = 0;
						break;

					case 15: // appr-color
						*iApprColor = atoi(token);
						cReadModeA = 0;
						break;

					case 16: // file-saved-date
						switch (cReadModeB) {
							case 1:
								if (iSaveYear != 0) *iSaveYear = atoi(token);
								cReadModeB = 2;
								break;

							case 2:
								if (iSaveMonth != 0) *iSaveMonth = atoi(token);
								cReadModeB = 3;
								break;

							case 3:
								if (iSaveDay != 0) *iSaveDay = atoi(token);
								cReadModeB = 4;
								break;

							case 4:
								if (iSaveHour != 0) *iSaveHour = atoi(token);
								cReadModeB = 5;
								break;

							case 5:
								if (iSaveMinute != 0) *iSaveMinute = atoi(token);
								cReadModeA = 0;
								cReadModeB = 0;
								break;
						}
						break;

					case 17: // character-loc-map
						strcpy(cMapName, token);
						cReadModeA = 0;
						break;
				}
			}
			else {
				if (memcmp(token, "appr1", 5) == 0)					cReadModeA = 1;
				if (memcmp(token, "appr2", 5) == 0)					cReadModeA = 2;
				if (memcmp(token, "appr3", 5) == 0)					cReadModeA = 3;
				if (memcmp(token, "appr4", 5) == 0)					cReadModeA = 4;
				if (memcmp(token, "sex-status", 10) == 0)			cReadModeA = 5;
				if (memcmp(token, "skin-status", 11) == 0)			cReadModeA = 6;
				if (memcmp(token, "character-LEVEL", 15) == 0)		cReadModeA = 7;
				if (memcmp(token, "character-STR", 13) == 0)		cReadModeA = 8;
				if (memcmp(token, "character-VIT", 13) == 0)		cReadModeA = 9;
				if (memcmp(token, "character-DEX", 13) == 0)		cReadModeA = 10;
				if (memcmp(token, "character-INT", 13) == 0)		cReadModeA = 11;
				if (memcmp(token, "character-MAG", 13) == 0)		cReadModeA = 12;
				if (memcmp(token, "character-CHARISMA", 18) == 0)	cReadModeA = 13;
				if (memcmp(token, "character-EXP", 13) == 0)		cReadModeA = 14;
				if (memcmp(token, "appr-color", 10) == 0)			cReadModeA = 15;
				if (memcmp(token, "file-saved-date:", 16) == 0) {
					cReadModeA = 16;
					cReadModeB = 1;
				}
				if (memcmp(token, "character-loc-map", 17) == 0)	cReadModeA = 17;
			}
			token = pStrTok->pGet();
		}

		if (pFile != NULL) fclose(pFile);
		delete pStrTok;
		free(cp);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: iGetCharacterData");
	}
#endif
	return TRUE;
}

void CMainLog::RegisterGameServer(int iClientH, char *pData, bool bRegister)
{
#ifdef DEF_DEBUG
	try {
#endif
		char *cp, *cMapNames, cData[100], m_cGameName[11], m_cGameAddress[16];
		DWORD * dwp;
		int m_iGamePort, m_iTotalMaps, i, x;

		ZeroMemory(cData, sizeof(cData));
		ZeroMemory(m_cGameName, sizeof(m_cGameName));
		ZeroMemory(m_cGameAddress, sizeof(m_cGameAddress));

		if (m_pMapServerList[iClientH] == NULL) return;

		switch (bRegister) {
			case TRUE:
				cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

				memcpy_secure(m_cGameName, cp, 10);
				cp += 10;

				memcpy_secure(m_cGameAddress, cp, 16);
				cp += 16;

				dwp = (DWORD *)cp;
				m_iGamePort = *dwp;
				cp += 2;

				m_iTotalMaps = *cp;
				cp++;

				wsprintf(G_cTxt, "%s-Game Server(%s) Total Maps: %d", m_cWorldName, m_cGameName, m_iTotalMaps);
				PutLogList(G_cTxt);

				wsprintf(G_cTxt, "IP: %s - registration success", m_cGameAddress, m_iGamePort);
				PutLogList(G_cTxt);

				for (i = 0; i < DEF_MAXGAME; i++) {
					if (m_pGameList[i] == NULL) {
						cMapNames = (char *)cp;
						m_pGameList[i] = new class CGame(iClientH, m_cGameName, m_cGameAddress, m_iGamePort, m_cWorldName, m_iTotalMaps);
						for (x = 0; x < m_iTotalMaps; x++) {
							if (bClientRegisterMaps(i, cp) == 0) {}
							cp += 11;
						}
						m_iTotalGame++;
						ResponseRegisterGameServer(iClientH, true);
						return;
					}
				}
				ResponseRegisterGameServer(iClientH, false);
				break;

			case FALSE:
				cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

				memcpy_secure(m_cWorldName, cp, 30);
				cp += 30;

				memcpy_secure(m_cGameName, cp, 10);
				cp += 10;

				for (i = 0; i < DEF_MAXGAME; i++)
					if ((m_pGameList[i] != NULL) && (strcmp(m_pGameList[i]->m_cWorldName, m_cWorldName) == 0) &&
						(strcmp(m_pGameList[i]->m_cGameName, m_cGameName) == 0)) {
						wsprintf(G_cTxt, "Delete Game Server(%s) in World Server(%s)", m_pGameList[i]->m_cGameName, m_cWorldName);
						PutLogList(G_cTxt);

						//delete maps on game server
						for (x = 0; x < DEF_MAXMAPS; x++) {
							if ((m_pMapList[x] != NULL) && (m_pMapList[x]->iIndex == i)) {
								wsprintf(G_cTxt, "Delete Map(%s) in Game Server(%s)", m_pMapList[x]->m_cMapName, m_cGameName);
								PutLogList(G_cTxt);
								delete m_pMapList[x];
								m_pMapList[x] = NULL;
							}
							delete m_pGameList[i];
							m_pGameList[i] = NULL;
							m_iTotalGame--;
						}
					}
				break;
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RegisterGameServer");
	}
#endif
}

void CMainLog::ResponseRegisterGameServer(int iClientH, BOOL bSuccesfull)
{
#ifdef DEF_DEBUG
	try {
#endif
		int iRet;
		DWORD * dwp;
		WORD * wp;
		char cData[20];

		if (m_pMapServerList[iClientH] == NULL) return;
		ZeroMemory(cData, sizeof(cData));

		dwp = (DWORD *)(cData + DEF_INDEX4_MSGID);
		*dwp = NUCLEO_RESPONSE_REGISTERGAMESERVER;
		wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);

		if (bSuccesfull) *wp = DEF_MSGTYPE_CONFIRM;
		else *wp = DEF_MSGTYPE_REJECT;

		iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cData, 6, DEF_USE_ENCRYPTION);

		switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				delete m_pMapServerList[iClientH];
				m_pMapServerList[iClientH] = NULL;
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: ResponseRegisterGameServer");
	}
#endif
}

int CMainLog::OnPlayerAccountMessage(DWORD dwMsgID, char * cAccountName, char * cPassword, char * pData3, int iLevel)
{
#ifdef DEF_DEBUG
	try {
#endif
		int i;

		switch (dwMsgID) {
			case DEF_MSGACCOUNTSET_NULL:
				DeleteAccount(cAccountName);
				return 0;
				break;

			case DEF_MSGACCOUNTSET_INIT:
				for (i = 0; i < DEF_MAXACCOUNTS; i++) {
					if (m_pAccountList[i] != NULL) {
						if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
							if (m_pAccountList[i]->InGame == false) {

								ZeroMemory(m_pAccountList[i]->cPassword, sizeof(m_pAccountList[i]->cPassword));
								strcpy(m_pAccountList[i]->cPassword, cPassword);
								m_pAccountList[i]->m_iLevel = iLevel;
								m_pAccountList[i]->dwLogInTime = timeGetTime();
								m_pAccountList[i]->InGame = true;

								wsprintf(G_cTxt, "Set account(%s) connection status INIT...", cAccountName);
								AccountLogList(G_cTxt);
								return 1;
							}
						}
					}
				}
				return 0;
				break;

			case DEF_MSGACCOUNTSET_WAIT:
				for (i = 0; i < DEF_MAXACCOUNTS; i++) {
					if (m_pAccountList[i] != NULL) {
						if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
							m_pAccountList[i]->InGame = true;
							m_pAccountList[i]->m_iLevel = iLevel;
							m_pAccountList[i]->dwLogInTime = timeGetTime();
							wsprintf(G_cTxt, "Set account(%s) connection status WAIT...", cAccountName);
							AccountLogList(G_cTxt);
							return 1;
						}
					}
				}
				return 0;
				break;

			case DEF_MSGACCOUNTSET_PLAYING:
				for (i = 0; i < DEF_MAXACCOUNTS; i++) {
					if (m_pAccountList[i] != NULL) {
						if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
							m_pAccountList[i]->InGame = true;
							ZeroMemory(m_pAccountList[i]->m_cOnGameServer, sizeof(m_pAccountList[i]->m_cOnGameServer));
							memcpy_secure(m_pAccountList[i]->m_cOnGameServer, pData3, 10);
							wsprintf(G_cTxt, "Set account(%s) connection status PLAYING...", cAccountName);
							AccountLogList(G_cTxt);
							return 1;
						}
					}
				}
				return 0;
				break;
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: OnPlayerAccountMessage");
	}
#endif
	return 0;
}

void CMainLog::OnMapServerSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef DEF_DEBUG
	try {
#endif
		register int iClientH, iRet, iTmp;

		iTmp = WM_ONMAPSERVERSOCKETEVENT;
		iClientH = message - iTmp;
		if (m_pMapServerList[iClientH] == NULL) return;
		iRet = m_pMapServerList[iClientH]->m_pXSock->iOnSocketEvent(wParam, lParam);
		switch (iRet) {

			case DEF_XSOCKEVENT_READCOMPLETE:
				OnMapServerRead(iClientH);
				break;

			case DEF_XSOCKEVENT_BLOCK:
				//m_iBlockCount++;
				wsprintf(G_cTxt, "<%d> Client Socket BLOCKED! (total blocks: )", iClientH/*, m_iBlockCount*/);
				ErrorLogList(G_cTxt);
				break;

			case DEF_XSOCKEVENT_CONFIRMCODENOTMATCH:
				wsprintfA(G_cTxt, "<%d> Confirmcode notmatch!", iClientH);
				ErrorLogList(G_cTxt);
				delete m_pMapServerList[iClientH];
				m_pMapServerList[iClientH] = NULL;
				break;

			case DEF_XSOCKEVENT_MSGSIZETOOLARGE:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
			case DEF_XSOCKEVENT_QUENEFULL:
				wsprintf(G_cTxt, "<%d> Connection Lost! Delete Client.", iClientH);
				PutLogList(G_cTxt);
				//DeleteClient(iClientH);

				for (int i = 0; i < DEF_MAXMAPS; i++) {
					if (m_pMapList[i] != NULL) m_pMapList[i] = NULL;
				}

				delete m_pMapServerList[iClientH];
				m_pMapServerList[iClientH] = NULL;
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: OnMapServerSubLogSocketEvent");
	}
#endif
}

void CMainLog::OnMapServerRead(int iClientH)
{
#ifdef DEF_DEBUG
	try {
#endif
		char  * pData, cKey;
		DWORD  dwMsgSize;

		if (m_pMapServerList[iClientH] == NULL) return;

		pData = m_pMapServerList[iClientH]->m_pXSock->pGetRcvDataPointer(&dwMsgSize, &cKey);
		if (pData == NULL) return;

		if (bPutMsgQuene(DEF_MSGFROM_MAPSERVER, pData, dwMsgSize, iClientH, cKey) == FALSE) {
			ErrorLogList("CRITICAL ERROR in CLIENT MsgQuene!!!");
		}
		return;
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: OnMapServerRead");
	}
#endif
}

BOOL CMainLog::bAccept(class XSocket * pXSock)
{
#ifdef DEF_DEBUG
	try {
#endif
		register int i, x;
		int sTotalConnection;
		class XSocket * pTmpSock;
		BOOL bFlag;
		sTotalConnection = 0;

		bFlag = FALSE;
		for (i = 1; i < DEF_MAXCLIENTSOCK; i++) {
			if (m_pClientList[i] == NULL) { //if client null
				m_pClientList[i] = new class CClient(m_hWnd);
				pXSock->bAccept(m_pClientList[i]->m_pXSock, WM_ONCLIENTSOCKETEVENT + i);
				m_pClientList[i]->m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
				ZeroMemory(m_pClientList[i]->m_cIPaddress, sizeof(m_pClientList[i]->m_cIPaddress));
				m_pClientList[i]->m_pXSock->iGetPeerAddress(m_pClientList[i]->m_cIPaddress);

				for (x = 1; x < DEF_MAXCLIENTSOCK; x++) {
					if (m_pClientList[x] != NULL) {
						if (strcmp(m_pClientList[x]->m_cIPaddress, m_pClientList[i]->m_cIPaddress) == 0) sTotalConnection++;
					}
				}

				if (sTotalConnection >= 20) { //20 cambio lalo
					delete m_pClientList[i];
					m_pClientList[i] = NULL;
					return FALSE;
				}
				return TRUE;
			}
		}

		pTmpSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		pXSock->bAccept(pTmpSock, NULL);
		delete pTmpSock;
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: bAccept");
	}
#endif
	return FALSE;
}

BOOL CMainLog::bAccept_MapServer(class XSocket * pXSock)
{
#ifdef DEF_DEBUG
	try {
#endif
		register int i;
		int sTotalConnection;
		class XSocket * pTmpSock;
		BOOL bFlag;
		sTotalConnection = 0;

		bFlag = FALSE;
		for (i = 0; i < DEF_MAXMAPSERVERSOCK; i++) {
			if (m_pMapServerList[i] == NULL) {
				m_pMapServerList[i] = new class CMapServer(m_hWnd);
				pXSock->bAccept(m_pMapServerList[i]->m_pXSock, WM_ONMAPSERVERSOCKETEVENT + i);
				m_pMapServerList[i]->m_pXSock->bInitBufferSize(DEF_MSGBUFFERSIZE);
				ZeroMemory(m_pMapServerList[i]->m_cIPaddress, sizeof(m_pMapServerList[i]->m_cIPaddress));
				m_pMapServerList[i]->m_pXSock->iGetPeerAddress(m_pMapServerList[i]->m_cIPaddress);

				if (strcmp(m_pMapServerList[i]->m_cIPaddress, m_cMapServerAddress) != 0) {
					delete m_pMapServerList[i];
					m_pMapServerList[i] = NULL;
				}

				bFlag = TRUE;
				return TRUE;
			}
		}

		pTmpSock = new class XSocket(m_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		pXSock->bAccept(pTmpSock, NULL);
		delete pTmpSock;
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: bAccept_MapServer");
	}
#endif
	return FALSE;
}

void CMainLog::RequestPlayerData(int iClientH, char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cCharacterName[11], cAccountName[11], cFileName[255], cDir[63];
		char *cp, *cp2;
		DWORD *dwp;
		WORD *wp;
		char cVerifyAccountName[11];
		char cGuildName[21], cGuildRank;
		int iVerifyLevel;
		FILE * pFile;
		int iRet;
		//HANDLE hFile;
		DWORD dwFileSize, dwVerifyGuildGUID, dwGuildGUID, lpNumberOfBytesRead;
		BOOL bGuildCheck, validChar = FALSE;
		short iAccount = -1, i;

		pFile = NULL;
		bGuildCheck = FALSE;

		if (m_pMapServerList[iClientH] == NULL) return;

		ZeroMemory(G_cData50000, sizeof(G_cData50000));
		ZeroMemory(cCharacterName, sizeof(cCharacterName));
		ZeroMemory(cAccountName, sizeof(cAccountName));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cCharacterName, cp, 10);
		cp += 10;

		memcpy_secure(cAccountName, cp, 10);
		cp += 10;

		if (strlen(cCharacterName) == 0) AccountLogList("(X) CharName NULL!");
		if (strlen(cAccountName) == 0) AccountLogList("(X) AccountName NULL!");

		for (i = 0; i < DEF_MAXACCOUNTS; i++) {
			if (m_pAccountList[i] != NULL) {
				if (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) {
					iAccount = i;
				}
			}
		}

		if (iAccount != -1) {
			for (i = 0; i < DEF_MAXCHARACTER; i++) {
				if ((m_pCharList[i] != NULL) && (m_pCharList[i]->iTracker == iAccount)) {
					if (memcmp(cCharacterName, m_pCharList[i]->cCharacterName, 10) == 0) {
						validChar = TRUE;
					}
				}
			}
		}

		if (!validChar) {
			wsprintf(G_cTxt, "(Hack) RequestPlayerData Error! ");
			ErrorLogList(G_cTxt);
			wsprintf(G_cTxt, "Account: %s Char : %s", cAccountName, cCharacterName);
			ErrorLogList(G_cTxt);
			//return;
		}

		VerifyCharacterIntegrity(cCharacterName, cVerifyAccountName, &iVerifyLevel, cGuildName, &cGuildRank, &dwGuildGUID);

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharacterName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharacterName);
		strcat(cFileName, ".txt");

		/*hFile = CreateFileA(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);*/

		pFile = fopen(cFileName, "rt");
		if ((pFile != NULL) /*&& (hFile != NULL) */ && (strcmp(cAccountName, cVerifyAccountName) == 0)) {
			fseek(pFile, 0, SEEK_END);
			dwFileSize = ftell(pFile);
			rewind(pFile);

			dwp = (DWORD *)(G_cData50000);
			*dwp = NUCLEO_RESPONSE_PLAYERDATA;

			wp = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM;

			cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy_secure(cp, cCharacterName, 10);
			cp += 10;

			// HGServer 2.0+ doesnt use this anymore
			*cp = 2;
			cp++;

			if (cGuildRank >= 0) {
				VerifyGuildIntegrity(cGuildName, &dwVerifyGuildGUID);
				if (dwVerifyGuildGUID == dwGuildGUID) {
					bGuildCheck = TRUE;
				}
			}

			*cp = (char)bGuildCheck;
			cp++;

			// can cause hgserver to crash because of this packet packet size!!!
			// need to make fix in hgserver in regards to MSGID_RESPONSE_PLAYERDATA's dwSize
			// eg if (dwSize > BUFFER) return; or something this is extremely exploitable
			// all you have to do is send a huge txt file or packet to the server and crash
			/*
			cp2 = new char[dwFileSize + 1];
			ZeroMemory(cp2, dwFileSize + 1);
			ReadFile(hFile, cp2, dwFileSize, &lpNumberOfBytesRead, NULL);
			CloseHandle(hFile);
			*/
			fseek(pFile, 0, SEEK_END);
			dwFileSize = ftell(pFile);
			rewind(pFile);
			cp2 = (char*)malloc(dwFileSize + 1);
			size_t rd_cp2 = fread(cp2, 1, dwFileSize, pFile);
			cp2[rd_cp2] = '\0';
			fclose(pFile);

			memcpy_secure(cp, cp2, dwFileSize + 1);
			free(cp2);
			cp += dwFileSize + 1;

			iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(G_cData50000, dwFileSize + 19, DEF_USE_ENCRYPTION);

			wsprintf(G_cTxt, "Send character(%s) data...", cCharacterName);
			AccountLogList(G_cTxt);
		}
		else {
			wsprintf(G_cTxt, "RequestPlayerData Error! Fn(%s) Account(%s, %s)", cFileName, cAccountName, cVerifyAccountName);
			AccountLogList(G_cTxt);

			dwp = (DWORD *)(G_cData50000);
			*dwp = NUCLEO_RESPONSE_PLAYERDATA;

			wp = (WORD *)(G_cData50000 + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char *)(G_cData50000 + DEF_INDEX2_MSGTYPE + 2);
			memcpy_secure(cp, cCharacterName, 10);
			cp += 10;

			iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(G_cData50000, 16, DEF_USE_ENCRYPTION);
		}

		switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				//DeleteClient(iClientH);
				delete m_pMapServerList[iClientH];
				m_pMapServerList[iClientH] = NULL;
				return;
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestPlayerData");
	}
#endif
}

void CMainLog::VerifyCharacterIntegrity(char *cCharacterName, char *cAccountName, int *iLevel, char * cGuildName, char *cGuildRank, DWORD *dwGuildGUID)
{
#ifdef DEF_DEBUG
	try {
#endif
		FILE * pFile;
		//HANDLE hFile;
		DWORD  dwFileSize;
		char * cp, *token, cReadMode, cTotalList;
		char seps[] = "= \t\n";
		class CStrTok * pStrTok;
		char cFileName[255];
		char cDir[63];
		char cCharacterMap[11];

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharacterName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharacterName);
		strcat(cFileName, ".txt");

		cTotalList = 0;
		cReadMode = 0;
		/*hFile = CreateFileA(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);*/
		
		pFile = fopen(cFileName, "r");
		if (pFile != NULL) {

			fseek(pFile, 0, SEEK_END);
			dwFileSize = ftell(pFile);
			rewind(pFile);

			cp = (char*)malloc(dwFileSize + 1);
			size_t rd_cp = fread(cp, 1, dwFileSize, pFile);
			cp[rd_cp] = '\0';
			pStrTok = new class CStrTok(cp, seps);

			token = pStrTok->pGet();
			while (token != NULL) {
				if (cReadMode != 0) {
					switch (cReadMode) {
						case 1:
							ZeroMemory(cCharacterMap, sizeof(cCharacterMap));
							memcpy_secure(cCharacterMap, token, 10);
							//strcpy(cCharacterMap, );
							cReadMode = 0;
							break;

						case 2:
							ZeroMemory(cAccountName, sizeof(cAccountName));
							memcpy_secure(cAccountName, token, 11); //11
							//strcpy(cAccountName, token);
							cReadMode = 0;
							break;

						case 3:
							*iLevel = atoi(token);
							cReadMode = 0;
							break;

						case 4:
							ZeroMemory(cGuildName, sizeof(cGuildName));
							memcpy_secure(cGuildName, token, 21);
							//strcpy(cGuildName, token);
							cReadMode = 0;
							break;

						case 5:
							*cGuildRank = (char)atoi(token);
							cReadMode = 0;
							break;

						case 6:
							*dwGuildGUID = atoi(token);
							cReadMode = 0;
							break;

						case 7:
							break;
					}
				}
				else {
					if (memcmp(token, "character-loc-map", 17) == 0)			cReadMode = 1;
					if (memcmp(token, "account-name", 12) == 0)					cReadMode = 2;
					if (memcmp(token, "character-LEVEL", 15) == 0)				cReadMode = 3;
					if (memcmp(token, "character-guild-name", 20) == 0)			cReadMode = 4;
					if (memcmp(token, "character-guild-rank", 20) == 0)			cReadMode = 5;
					if (memcmp(token, "character-guild-GUID", 20) == 0)			cReadMode = 6;
					if (memcmp(token, "[EOF]", 5) == 0)							cReadMode = 7;
				}
				token = pStrTok->pGet();
			}
			delete pStrTok;
			free(cp);
		}
		if (pFile != NULL) fclose(pFile);
		return;

#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: VerifyCharacterIntegrity");
	}
#endif
}

void CMainLog::VerifyGuildIntegrity(char *cGuildName, DWORD *dwGuildGUID)
{
#ifdef DEF_DEBUG
	try {
#endif
		//HANDLE hFile;
		DWORD  dwFileSize;
		char * cp, *token, cReadMode, cTotalList;
		char seps[] = "= \t\n";
		class CStrTok * pStrTok;
		char cFileName[255];
		char cDir[63];
		FILE * pFile;

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cGuildName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		wsprintf(G_cTxt, "Verify Guild Integrity (%s)", cFileName);
		AccountLogList(G_cTxt);

		cTotalList = 0;
		cReadMode = 0;

		/*hFile = CreateFileA(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);*/

		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			fseek(pFile, 0, SEEK_END);
			dwFileSize = ftell(pFile);
			rewind(pFile);

			cp = (char*)malloc(dwFileSize + 1);
			size_t rd_cp = fread(cp, 1, dwFileSize, pFile);
			cp[rd_cp] = '\0';
			pStrTok = new class CStrTok(cp, seps);
			token = pStrTok->pGet();
			while (token != NULL) {
				if (cReadMode != 0) {
					switch (cReadMode) {
						case 1:
							*dwGuildGUID = atoi(token);
							cReadMode = 0;
							break;
					}
				}
				else {
					if (memcmp(token, "guild-GUID", 10) == 0)			cReadMode = 1;
				}
				token = pStrTok->pGet();
			}
			delete pStrTok;
			free(cp);
		}
		if (pFile != NULL) fclose(pFile);
		return;
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: VerifyGuildIntegrity");
	}
#endif
}

void CMainLog::EnterGameConfirm(int iClientH, char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cGameServerName[11], cCharacterName[11], cCharNameCheck[11], cAccountName[11], cAddress[16];
		char cTemp[100];
		int iLevel, iCheckLevel;
		char *cp;
		int *ip, iRet;
		DWORD *dwp;
		WORD *wp;

		if (m_pMapServerList[iClientH] == NULL) return;

		ZeroMemory(cAccountName, sizeof(cAccountName));
		ZeroMemory(cCharacterName, sizeof(cCharacterName));
		ZeroMemory(cGameServerName, sizeof(cGameServerName));
		ZeroMemory(cAddress, sizeof(cAddress));
		ZeroMemory(cCharNameCheck, sizeof(cCharNameCheck));
		ZeroMemory(cTemp, sizeof(cTemp));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cAccountName, cp, 10);
		cp += 10;

		memcpy_secure(cCharacterName, cp, 10);
		cp += 10;

		memcpy_secure(cGameServerName, cp, 10);
		cp += 10;

		memcpy_secure(cAddress, cp, 16);
		cp += 16;

		ip = (int *)cp;
		iLevel = *ip;
		cp += 4;

		switch (iGetCharacterInformation(cAccountName, cCharNameCheck, &iCheckLevel)) {
			case 1:
			case 2:
				if ((strcmp(cCharacterName, cCharNameCheck) != FALSE) || (iCheckLevel != iLevel)) {
					wsprintf(G_cTxt, "ERROR! Account(%s) Password or level mismatch. Request Game Server to disconnect without data save...", cAccountName);
					AccountLogList(G_cTxt);
					dwp = (DWORD *)(cTemp);
					*dwp = NUCLEO_REQUEST_FORCEDISCONECTACCOUNT;
					wp = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
					*wp = DEF_MSGTYPE_CONFIRM;
					cp = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
					memcpy_secure(cp, cAccountName, 10);
					cp += 10;
					iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
					switch (iRet) {
						case DEF_XSOCKEVENT_QUENEFULL:
						case DEF_XSOCKEVENT_SOCKETERROR:
						case DEF_XSOCKEVENT_CRITICALERROR:
						case DEF_XSOCKEVENT_SOCKETCLOSED:
							//DeleteClient(iClientH);
							delete m_pClientList[iClientH];
							m_pClientList[iClientH] = NULL;
							return;
					}
					return;
				}
				break;
		}
		OnPlayerAccountMessage(DEF_MSGACCOUNTSET_PLAYING, cAccountName, NULL, cGameServerName, iLevel);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: EnterGameConfirm");
	}
#endif
}

int CMainLog::iGetCharacterInformation(char * cAccountName, char * cCharacterName, int * iLevel)
{
#ifdef DEF_DEBUG
	try {
#endif
		int i;

		for (i = 0; i < DEF_MAXPLAYERACCOUNTS; i++) {
			if ((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
				memcpy_secure(cCharacterName, m_pAccountList[i]->cPassword, 10);
				*iLevel = m_pAccountList[i]->m_iLevel;
				return m_pAccountList[i]->InGame;
			}
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: iGetCharacterInformation");
	}
#endif
	return 0;
}

void CMainLog::RequestSavePlayerData(int iClientH, char *pData, DWORD dwMsgSize, BOOL bVar1, BOOL bVar2)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cAccountName[11], cCharName[11], cAccountPassword[11], cData[256];
		char *cp;
		DWORD *dwp;
		WORD *wp;
		int iRet;
		FILE * pFile;
		char cFileName[255];
		char cDir[63];
		//HANDLE hFile;
		DWORD  dwFileSize;
		BOOL bFlag;

		if (m_pMapServerList[iClientH] == NULL) return;
		ZeroMemory(cAccountName, sizeof(cAccountName));
		ZeroMemory(cCharName, sizeof(cCharName));
		ZeroMemory(cAccountPassword, sizeof(cAccountPassword));
		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cCharName, cp, 10);
		cp += 10;

		memcpy_secure(cAccountName, cp, 10);
		cp += 10;

		memcpy_secure(cAccountPassword, cp, 10);
		cp += 10;

		bFlag = (BOOL)*cp;
		cp++;

		if ((dwMsgSize - 40) <= 0) {
			AccountLogList("(X) Character data body empty: Cannot create & save data file.");
			return;
		}

		//No save character from core
		/*
		strcat(cFileName, "Character");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cCharName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cCharName);
		strcat(cFileName, ".txt");


		pFile = fopen(cFileName, "wt");
		if (pFile == NULL) {
			wsprintf(G_cTxt, "(X) Cannot create character data file : Name(%s)", cCharName);
			AccountLogList(G_cTxt);
		}
		else {
			wsprintf(G_cTxt, "Character data file saved : Name(%s)", cCharName);
			AccountLogList(G_cTxt);
			fwrite(cp, 1, strlen(cp), pFile);
			if (pFile != NULL) fclose(pFile);
		}*/

		if ((bVar1 == TRUE) && (bVar2 == FALSE)) {
			wsprintf(G_cTxt, "Account(%s) log out request...", cAccountName);
			AccountLogList(G_cTxt);

			OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
		}

		if (bVar2 == TRUE) {
			ZeroMemory(cData, sizeof(cData));

			dwp = (DWORD *)(cData);
			*dwp = NUCLEO_RESPONSE_SAVEPLAYERDATA_REPLY;

			wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_CONFIRM;

			cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
			memcpy_secure(cp, cCharName, 10);
			cp += 10;

			iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);

			switch (iRet) {
				case DEF_XSOCKEVENT_QUENEFULL:
				case DEF_XSOCKEVENT_SOCKETERROR:
				case DEF_XSOCKEVENT_CRITICALERROR:
				case DEF_XSOCKEVENT_SOCKETCLOSED:
					//DeleteClient(iClientH);
					delete m_pMapServerList[iClientH];
					m_pMapServerList[iClientH] = NULL;
					return;
			}
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestSavePlayerData");
	}
#endif
}

void CMainLog::RequestNoSaveLogout(int iClientH, char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char *cp, cTotalChar;
		char cAccountName[11], cCharacterName[11], cGameServerName[11];

		if (m_pClientList[iClientH] == NULL) return;
		ZeroMemory(cAccountName, sizeof(cAccountName));
		ZeroMemory(cCharacterName, sizeof(cCharacterName));
		ZeroMemory(cGameServerName, sizeof(cGameServerName));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cCharacterName, cp, 10);
		cp += 10;

		memcpy_secure(cAccountName, cp, 10);
		cp += 10;

		memcpy_secure(cGameServerName, cp, 10);
		cp += 10;

		cTotalChar = *cp;
		cp++;

		wsprintf(G_cTxt, "Account(%s) log out request to MLS 1...", cAccountName);
		AccountLogList(G_cTxt);

		OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestNoSaveLogout");
	}
#endif
}

void CMainLog::ClearAccountStatus(char cAccountName[11])
{
#ifdef DEF_DEBUG
	try {
#endif
		for (int i = 1; i < DEF_MAXCLIENTSOCK; i++) {
			if ((m_pClientList[i] != NULL) && (strcmp(m_pClientList[i]->m_cAccountName, cAccountName) == 0)) {
				wsprintf(G_cTxt, "Clear Client Account (%s)", cAccountName);
				AccountLogList(G_cTxt);

				delete m_pClientList[i];
				m_pClientList[i] = NULL;

				return;
			}
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: ClearAccountStatus");
	}
#endif
}

int CMainLog::RequestSetAccountWaitStatus(char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cAccountName[11];
		char *cp;
		int iLevel, *ip;
		int iRet;

		ZeroMemory(cAccountName, sizeof(cAccountName));
		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cAccountName, cp, 10);
		cp += 10;

		ip = (int *)cp;
		iLevel = *ip;
		cp += 4;

		iRet = OnPlayerAccountMessage(DEF_MSGACCOUNTSET_WAIT, cAccountName, 0, 0, iLevel);
		return iRet;

#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestSetAccountWaitStatus");
		return 0;
	}
#endif
}

/** GUILD FUNCTIONS **/
void CMainLog::RequestCreateNewGuild(int iClientH, char *pData)
{
/*#ifdef DEF_DEBUG
	try {
#endif*/
		char cFileName[255], cData[500];
		char cTxt[500];
		//char cTxt2[100];
		char cTxt2[500];
		char cGuildMasterName[11], cGuildLocation[11], cDir[255], cGuildName[21];
		char *cp;
		DWORD *dwp, dwGuildGUID;
		WORD *wp;
		int iRet;
		SYSTEMTIME SysTime;
		FILE * pFile;

		if (m_pMapServerList[iClientH] == NULL) return;
		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTxt2, sizeof(cTxt2));
		ZeroMemory(cDir, sizeof(cDir));
		ZeroMemory(cData, sizeof(cData));
		ZeroMemory(cGuildMasterName, sizeof(cGuildMasterName));
		ZeroMemory(cGuildName, sizeof(cGuildName));
		ZeroMemory(cGuildLocation, sizeof(cGuildLocation));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
		memcpy_secure(cGuildMasterName, cp, 10);
		cp += 30;

		memcpy_secure(cGuildName, cp, 20);
		cp += 20;

		memcpy_secure(cGuildLocation, cp, 10);
		cp += 10;

		dwp = (DWORD *)(pData + 66);
		dwGuildGUID = *dwp;
		cp += 4;

		if (strcmp(cGuildName, "NONE") == 0) {
			cp = (char *)(cData + 16);

			dwp = (DWORD *)cData;
			*dwp = NUCLEO_RESPONSE_CREATENEWGUILD;

			wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
			memcpy_secure(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
		}

		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		wsprintf(cTxt2, "AscII%d", *cGuildName);
		strcat(cFileName, cTxt2);
		strcat(cDir, cFileName);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		_mkdir("Guild");
		_mkdir(cDir);

		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			wsprintf(cTxt2, "(X) Cannot create new guild - Already existing guild name: Name(%s)", cFileName);
			AccountLogList(cTxt2);

			cp = (char *)(cData + 16);

			dwp = (DWORD *)cData;
			*dwp = NUCLEO_RESPONSE_CREATENEWGUILD;

			wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
			memcpy_secure(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
			if (pFile != NULL) fclose(pFile);
		}
		else {
			pFile = fopen(cFileName, "wt");
			if (pFile == NULL) {
				wsprintf(cTxt2, "(X) Cannot create new guild - cannot create file : Name(%s)", cFileName);
				AccountLogList(cTxt2);

				cp = (char *)(cData + 16);

				dwp = (DWORD *)cData;
				*dwp = NUCLEO_RESPONSE_CREATENEWGUILD;

				wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
				memcpy_secure(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
			}
			else {
				wsprintf(cTxt2, "(O) New guild created : Name(%s)", cFileName);
				AccountLogList(cTxt2);

				ZeroMemory(cTxt2, sizeof(cTxt2));
				ZeroMemory(cTxt, sizeof(cTxt));
				GetLocalTime(&SysTime);

				wsprintf(cTxt, ";Guild file - Updated %4d/%2d/%2d/%2d/%2d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
				strcat(cTxt, "\n");

				strcat(cTxt, "[GUILD-INFO]\n\n");

				strcat(cTxt, "guildmaster-name     = ");
				strcat(cTxt, cGuildMasterName);
				strcat(cTxt, "\n");

				strcat(cTxt, "guild-GUID           = ");
				wsprintf(cTxt2, "%d", dwGuildGUID);
				strcat(cTxt, cTxt2);
				strcat(cTxt, "\n");

				strcat(cTxt, "guild-location       = ");
				strcat(cTxt, cGuildLocation);
				strcat(cTxt, "\n\n");

				strcat(cTxt, "[GUILDSMAN]\n");

				fwrite(cTxt, 1, strlen(cTxt), pFile);
				dwp = (DWORD *)cData;
				*dwp = NUCLEO_RESPONSE_CREATENEWGUILD;

				wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_CONFIRM;

				cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
				memcpy_secure(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);
				if (pFile != NULL) fclose(pFile);
			}
		}
		switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				delete m_pMapServerList[iClientH];
				m_pMapServerList[iClientH] = NULL;
				return;
		}
/*#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestCreateNewGuild");
	}
#endif*/
}

void CMainLog::RequestDisbandGuild(int iClientH, char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cTemp[500];
		DWORD *dwp;
		WORD *wp;
		char *cp;
		char cFileName[255], cTxt[100], cDir[100];
		char cGuildMasterName[11], cGuildName[21];
		int iRet;
		FILE * pFile;

		if (m_pMapServerList[iClientH] == NULL) return;
		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cDir, sizeof(cDir));
		ZeroMemory(cTemp, sizeof(cTemp));
		ZeroMemory(cGuildMasterName, sizeof(cGuildMasterName));
		ZeroMemory(cGuildName, sizeof(cGuildName));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
		memcpy_secure(cGuildMasterName, cp, 10);
		cp += 30;

		memcpy_secure(cGuildName, cp, 20);
		cp += 20;

		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cTxt, "AscII%d", *cGuildName);
		strcat(cFileName, cTxt);
		strcat(cDir, cFileName);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			fclose(pFile);
			wsprintf(G_cTxt, "(O) Disband Guild - Deleting guild file... : Name(%s)", cFileName);
			AccountLogList(G_cTxt);
			if (DeleteFile(cFileName) != NULL) {
				dwp = (DWORD *)(cTemp);
				*dwp = NUCLEO_RESPONSE_DISBANDGUILD;

				wp = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_CONFIRM;

				cp = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
				memcpy_secure(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
			}
			else {
				dwp = (DWORD *)(cTemp);
				*dwp = NUCLEO_RESPONSE_DISBANDGUILD;

				wp = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
				*wp = DEF_MSGTYPE_REJECT;

				cp = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
				memcpy_secure(cp, cGuildMasterName, 10);
				cp += 10;

				iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
			}
		}
		else {
			dwp = (DWORD *)(cTemp);
			*dwp = NUCLEO_RESPONSE_DISBANDGUILD;

			wp = (WORD *)(cTemp + DEF_INDEX2_MSGTYPE);
			*wp = DEF_MSGTYPE_REJECT;

			cp = (char *)(cTemp + DEF_INDEX2_MSGTYPE + 2);
			memcpy_secure(cp, cGuildMasterName, 10);
			cp += 10;

			iRet = m_pMapServerList[iClientH]->m_pXSock->iSendMsg(cTemp, 16, DEF_USE_ENCRYPTION);
		}

		switch (iRet) {
			case DEF_XSOCKEVENT_QUENEFULL:
			case DEF_XSOCKEVENT_SOCKETERROR:
			case DEF_XSOCKEVENT_CRITICALERROR:
			case DEF_XSOCKEVENT_SOCKETCLOSED:
				//DeleteClient(iClientH);
				delete m_pMapServerList[iClientH];
				m_pMapServerList[iClientH] = NULL;
				return;
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestDisbandGuild");
	}
#endif
}

void CMainLog::UpdateGuildInfoNewGuildsman(int iClientH, char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cFileName[255];
		char cTxt[100];
		char cTxt2[100];
		char cGuildsManName[11], cDir[255], cGuildName[21];
		char *cp;
		FILE * pFile;

		if (m_pMapServerList[iClientH] == NULL) return;
		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTxt2, sizeof(cTxt2));
		ZeroMemory(cDir, sizeof(cDir));
		ZeroMemory(cGuildsManName, sizeof(cGuildsManName));
		ZeroMemory(cGuildName, sizeof(cGuildName));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
		memcpy_secure(cGuildsManName, cp, 10);
		cp += 10;

		memcpy_secure(cGuildName, cp, 20);
		cp += 20;

		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		wsprintf(cTxt2, "AscII%d", *cGuildName);
		strcat(cFileName, cTxt2);
		strcat(cDir, cFileName);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		_mkdir("Guild");
		_mkdir(cDir);

		pFile = fopen(cFileName, "at");
		if (pFile != NULL) {
			wsprintf(cTxt, "guildsman-name       = %s\n", cGuildsManName);

			fwrite(cTxt, 1, strlen(cTxt), pFile);
			if (pFile != NULL) fclose(pFile);

			wsprintf(cTxt2, "(O) New Guildsman Add: %s, In Guild: %s", cGuildsManName, cGuildName);
			AccountLogList(cTxt2);
		}
		else {
			wsprintf(cTxt2, "(X) Cannot Add Guildsman, Guild: %s Not Found", cGuildName);
			AccountLogList(cTxt2);
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: UpdateGuildInfoNewGuildsman");
	}
#endif
}

void CMainLog::UpdateGuildInfoDeleteGuildman(int iClientH, char *pData)
{
	// Está funcion habia deshabilitado :P //Zero Pelotudo...
#ifdef DEF_DEBUG
	try {
#endif
		char cFileName[255];
		char cTxt[100];
		char cTxt2[5000];
		char cGuildsManName[11], cDir[255], cGuildName[21];
		char *cp;
		FILE * pFile;
		short iTest = -1;

		if (m_pMapServerList[iClientH] == NULL) return;
		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cTxt, sizeof(cTxt));
		ZeroMemory(cTxt2, sizeof(cTxt2));
		ZeroMemory(cDir, sizeof(cDir));
		ZeroMemory(cGuildsManName, sizeof(cGuildsManName));
		ZeroMemory(cGuildName, sizeof(cGuildName));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
		memcpy_secure(cGuildsManName, cp, 10);
		cp += 10;

		memcpy_secure(cGuildName, cp, 20);
		cp += 20;

		strcat(cFileName, "Guild");
		strcat(cFileName, "\\");
		wsprintf(cTxt, "AscII%d", *cGuildName);
		strcat(cFileName, cTxt);
		strcat(cDir, cFileName);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cGuildName);
		strcat(cFileName, ".txt");

		_mkdir("Guild");
		_mkdir(cDir);

		pFile = fopen(cFileName, "rt");
		if (pFile != NULL) {
			ZeroMemory(cTxt, sizeof(cTxt));
			wsprintf(cTxt, "guildsman-name       = %s\n", cGuildsManName);

			char linea[100];
			while (!feof(pFile)) {
				fgets(linea, sizeof(linea), pFile);
				if (memcmp(linea, cTxt, strlen(cTxt)) != 0)
					strcat(cTxt2, linea);
				else iTest = 1;
			}

			if (iTest == -1) {
				wsprintf(cTxt, "(X) Guildsman Not Found: %s, In Guild: %s", cGuildsManName, cGuildName);
				AccountLogList(cTxt);
			}
			else {
				SaveInfo(cFileName, cTxt2, 1);
				wsprintf(cTxt, "(O) Guildsman: %s, Removed of Guild: %s", cGuildsManName, cGuildName);
				AccountLogList(cTxt);
			}
		}
		else {
			wsprintf(cTxt, "(X) Cannot Remove Guildsman, Guild: %s Not Found", cGuildName);
			AccountLogList(cTxt);
		}

		if (pFile != NULL) fclose(pFile);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: UpdateGuildInfoDeleteGuildman");
	}
#endif
}

void CMainLog::DeleteAccount(char cAccountName[11])
{
#ifdef DEF_DEBUG
	try {
#endif
		int i, x;

		for (i = 0; i < DEF_MAXACCOUNTS; i++) {
			if ((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0)) {
				ClearAccountStatus(cAccountName);

				for (x = 0; x < DEF_MAXCHARACTER; x++) {
					if ((m_pCharList[x] != NULL) && (m_pCharList[x]->iTracker == i)) {
						m_iValidCharacters--;
						delete m_pCharList[x];
						m_pCharList[x] = NULL;
					}
				}
				wsprintf(G_cTxt, "Clear Account (%s)", m_pAccountList[i]->cAccountName);
				AccountLogList(G_cTxt);
				m_iValidAccounts--;

				delete m_pAccountList[i];
				m_pAccountList[i] = NULL;

				return;
			}
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: DeleteAccount");
	}
#endif
}

void CMainLog::DeleteAccountTimmer()
{
#ifdef DEF_DEBUG
	try {
#endif
		int i, x;

		for (i = 0; i < DEF_MAXACCOUNTS; i++) {
			if ((m_pAccountList[i] != NULL) && (m_pAccountList[i]->InGame == false)) {
				for (x = 0; x < DEF_MAXCHARACTER; x++) {
					if ((m_pCharList[x] != NULL) && (m_pCharList[x]->iTracker == i)) {
						m_iValidCharacters--;
						delete m_pCharList[x];
						m_pCharList[x] = NULL;
					}
				}
				wsprintf(G_cTxt, "Account Deleted(%s)", m_pAccountList[i]->cAccountName);
				m_iValidAccounts--;

				delete m_pAccountList[i];
				m_pAccountList[i] = NULL;
			}
		}
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: DeleteAccountTimmer");
	}
#endif
}

void CMainLog::RequestDeleteAccount(char *pData)
{
#ifdef DEF_DEBUG
	try {
#endif
		char *cp;
		char cAccountName[11];

		ZeroMemory(cAccountName, sizeof(cAccountName));

		cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);

		memcpy_secure(cAccountName, cp, 10);
		cp += 10;

		wsprintf(G_cTxt, "Account(%s) log out request to MLS 2...", cAccountName);
		AccountLogList(G_cTxt);

		OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, cAccountName, 0, 0, 0);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: RequestDeleteAccount");
	}
#endif
}

void CMainLog::DeslogAcc(char cAccountName[11])
{
#ifdef DEF_DEBUG
	try {
#endif
		char *cp, cData[100], cGameServerName[11];
		int  i, x, iRet, MapServer;
		DWORD *dwp;
		WORD * wpMsgType, *wp, wCount = 5;

		for (i = 1; i < DEF_MAXACCOUNTS; i++) {
			if ((m_pAccountList[i] != NULL) && (strcmp(m_pAccountList[i]->cAccountName, cAccountName) == 0) && (m_pAccountList[i]->InGame == true)) {

				ZeroMemory(cGameServerName, sizeof(cGameServerName));
				memcpy_secure(cGameServerName, m_pAccountList[i]->m_cOnGameServer, 11);

				if (strlen(cGameServerName) < 5) {
					m_pAccountList[i]->InGame = false;
					OnPlayerAccountMessage(DEF_MSGACCOUNTSET_NULL, m_pAccountList[i]->cAccountName, 0, 0, 0);

					wsprintf(G_cTxt, "Forced Delete Account(%s)", cAccountName);
					CommandList(G_cTxt);
					return;
				}

				for (x = 0; x < DEF_MAXGAME; x++) {
					if (m_pGameList[x] != NULL) {
						if (strcmp(m_pGameList[x]->m_cGameServerName, cGameServerName) == TRUE) {
							MapServer = m_pGameList[x]->m_iTrackSock;

							ZeroMemory(cData, sizeof(cData));

							dwp = (DWORD *)cData;
							*dwp = NUCLEO_REQUEST_FORCEDISCONECTACCOUNT;

							wp = (WORD *)(cData + DEF_INDEX2_MSGTYPE);
							*wp = wCount;

							cp = (char *)(cData + DEF_INDEX2_MSGTYPE + 2);
							memcpy_secure(cp, cAccountName, 10);
							cp += 10;

							wsprintf(G_cTxt, "Force Disconnect Account(%s)", cAccountName);
							CommandList(G_cTxt);

							wsprintf(G_cTxt, "in Game Server(%s)", cGameServerName);
							CommandList(G_cTxt);

							if (m_pMapServerList[MapServer] == NULL) return;
							iRet = m_pMapServerList[MapServer]->m_pXSock->iSendMsg(cData, 16, DEF_USE_ENCRYPTION);

							switch (iRet) {
								case DEF_XSOCKEVENT_QUENEFULL:
								case DEF_XSOCKEVENT_SOCKETERROR:
								case DEF_XSOCKEVENT_CRITICALERROR:
								case DEF_XSOCKEVENT_SOCKETCLOSED:
									delete m_pClientList[MapServer];
									m_pClientList[MapServer] = NULL;
							}
							return;
						}
					}
				}
			}
		}

		wsprintf(G_cTxt, "Account Not Found (%s)", cAccountName);
		CommandList(G_cTxt);

#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: DeslogAcc");
	}
#endif
}

BOOL CMainLog::bCheckValidName(char *pStr)
{
	int i, iLen;
	iLen = strlen(pStr);
	for (i = 0; i < iLen; i++)
	{
		if (pStr[i] < 0)	return FALSE;
		if ((pStr[i] == ',') || (pStr[i] == '=') || (pStr[i] == ' ') || (pStr[i] == '\n') ||
			(pStr[i] == '\t') || (pStr[i] == '.') || (pStr[i] == '\\') || (pStr[i] == '/') ||
			(pStr[i] == ':') || (pStr[i] == '*') || (pStr[i] == '?') || (pStr[i] == '<') ||
			(pStr[i] == '>') || (pStr[i] == '|') || (pStr[i] == '"') || (pStr[i] == '`') ||
			(pStr[i] == ';') || (pStr[i] == '=') || (pStr[i] == '@') || (pStr[i] == '[') ||
			(pStr[i] == ']') || (pStr[i] == '^') || (pStr[i] == '_') || (pStr[i] == '\'')) return FALSE;
		if ((pStr[i] < '0') || (pStr[i] > 'z')) return FALSE;
	}
	return TRUE;
}

void CMainLog::CreateNewAccount(int iClientH, char* pData)
{
	char* cp;
	FILE* pFile;
	HANDLE hFile;
	DWORD  dwFileSize;
	SYSTEMTIME SysTime;
	char cFile[20000], cBuffer[1024], cFn[1024];
	char cName[12] = {};
	char cPassword[12] = {};
	char cEmailAddr[52] = {};
	char cQuiz[45] = {};
	char cAnswer[20] = {};

	if (m_pClientList[iClientH] == NULL) return;

	GetLocalTime(&SysTime);

	cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	Pop(cp, cName, 12);
	Pop(cp, cPassword, 12);
	Pop(cp, cEmailAddr, 52);
	Pop(cp, cQuiz, 45);
	Pop(cp, cAnswer, 20);

	if ((strlen(cName) == 0) || (strlen(cPassword) == 0) ||
		(strlen(cEmailAddr) == 0) ||
		(strlen(cQuiz) == 0) ||
		(strlen(cAnswer) == 0))
		return;

	wsprintf(G_cTxt, "Request create new Account: %s", cName);
	AccountLogList(G_cTxt);

	if (!bCheckValidName(cName) || !bCheckValidName(cPassword))
		return;

	ZeroMemory(cFn, sizeof(cFn));
	wsprintf(cFn, "Account\\AscII%d\\%s.txt", cName[0], cName);
	hFile = CreateFile(cFn, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
	else
	{
		wsprintf(G_cTxt, "Account: %s create error.", cName);
		AccountLogList(G_cTxt);

		CloseHandle(hFile);
		SendEventToClient(DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, 0, 0, iClientH);
		return;
	}

	mkdir("Account");
	char Aux = 0;
	Aux = cName[0];
	ZeroMemory(cFn, sizeof(cFn));
	wsprintf(cFn, "Account\\AscII%d", Aux);
	_mkdir(cFn);

	ZeroMemory(cFn, sizeof(cFn));
	wsprintf(cFn, "Account\\AscII%d\\%s.txt", Aux, cName);
	pFile = fopen(cFn, "wt");
	if (pFile == NULL)
	{
		wsprintf(G_cTxt, "Account: %s create error.", cName);
		AccountLogList(G_cTxt);

		SendEventToClient(DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, 0, 0, iClientH);
		return;
	}

	wsprintf(G_cTxt, "Account: %s created successfully.", cName);
	AccountLogList(G_cTxt);

	SendEventToClient(DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED, DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED, 0, 0, iClientH);

	ZeroMemory(cFile, sizeof(cFile));

	strcat(cFile, "[Account Info]");
	strcat(cFile, "\n");

	strcat(cFile, "account-name     = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	memcpy(cBuffer, cName, 10);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-password = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	memcpy(cBuffer, cPassword, 10);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-Email    = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	wsprintf(cBuffer, "%s", cEmailAddr);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-Quiz = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	wsprintf(cBuffer, "%s", cQuiz);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-Answer = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	wsprintf(cBuffer, "%s", cAnswer);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-change-password = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	wsprintf(cBuffer, "%d %d %d", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");
	strcat(cFile, "\n");

	strcat(cFile, "[CHARACTERS]");
	strcat(cFile, "\n");

	fwrite(cFile, 1, strlen(cFile), pFile);
	fclose(pFile);
}
//heaton
/*void CMainLog::CreateNewAccount(int iClientH, char* pData)
{
	char * cp;
	FILE * pFile;
	HANDLE hFile;
	DWORD  dwFileSize;
	SYSTEMTIME SysTime;
	char cFile[20000], cBuffer[1024], cFn[1024];
	char cName[12] = {};
	char cPassword[12] = {};
	char cEmailAddr[52] = {};
	char cQuiz[45] = {};
	char cAnswer[20] = {};

	if (m_pClientList[iClientH] == NULL) return;

	GetLocalTime(&SysTime);

	cp = (char *)(pData + DEF_INDEX2_MSGTYPE + 2);
	Pop(cp, cName, 12);
	Pop(cp, cPassword, 12);
	Pop(cp, cEmailAddr, 52);
	Pop(cp, cQuiz, 45);
	Pop(cp, cAnswer, 20);

	if ((strlen(cName) == 0) || (strlen(cPassword) == 0) ||
		(strlen(cEmailAddr) == 0) ||
		(strlen(cQuiz) == 0) ||
		(strlen(cAnswer) == 0))
		return;

	wsprintf(G_cTxt, "Request create new Account: %s", cName);
	AccountLogList(G_cTxt);

	if (!bCheckValidName(cName) || !bCheckValidName(cPassword))
		return;

	ZeroMemory(cFn, sizeof(cFn));
	wsprintf(cFn, "Account\\AscII%d\\%s.cfg", cName[0], cName);
	hFile = CreateFile(cFn, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
	else
	{
		wsprintf(G_cTxt, "Account: %s create error.", cName);
		AccountLogList(G_cTxt);

		CloseHandle(hFile);
		SendEventToClient(DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, 0, 0, iClientH);
		return;
	}

	mkdir("Account");
	char Aux = 0;
	Aux = cName[0];
	ZeroMemory(cFn, sizeof(cFn));
	wsprintf(cFn, "Account\\AscII%d", Aux);
	_mkdir(cFn);

	ZeroMemory(cFn, sizeof(cFn));
	wsprintf(cFn, "Account\\AscII%d\\%s.txt", Aux, cName);
	pFile = fopen(cFn, "wt");
	if (pFile == NULL)
	{
		wsprintf(G_cTxt, "Account: %s create error.", cName);
		AccountLogList(G_cTxt);

		SendEventToClient(DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, DEF_LOGRESMSGTYPE_NEWACCOUNTFAILED, 0, 0, iClientH);
		return;
	}

	wsprintf(G_cTxt, "Account: %s created successfully.", cName);
	AccountLogList(G_cTxt);

	SendEventToClient(DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED, DEF_LOGRESMSGTYPE_NEWACCOUNTCREATED, 0, 0, iClientH);

	ZeroMemory(cFile, sizeof(cFile));

	strcat(cFile, "[Account Info]");
	strcat(cFile, "\n");

	strcat(cFile, "account-name     = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	memcpy(cBuffer, cName, 10);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-password = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	memcpy(cBuffer, cPassword, 10);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-Email    = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	wsprintf(cBuffer, "%s", cEmailAddr);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-Quiz = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	wsprintf(cBuffer, "%s", cQuiz);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-Answer = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	wsprintf(cBuffer, "%s", cAnswer);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");

	strcat(cFile, "account-change-password = ");
	ZeroMemory(cBuffer, sizeof(cBuffer));
	wsprintf(cBuffer, "%d %d %d", SysTime.wYear, SysTime.wMonth, SysTime.wDay);
	strcat(cFile, cBuffer);
	strcat(cFile, "\n");
	strcat(cFile, "\n");

	strcat(cFile, "[CHARACTERS]");
	strcat(cFile, "\n");

	fwrite(cFile, 1, strlen(cFile), pFile);
	fclose(pFile);
}*/

int CMainLog::GetAccountInfo2(int iClientH, char cAccountName[11], char cEmail[52], char cAccountPass[11], char cWorldName[30], int* iAccount)
{
#ifdef DEF_DEBUG
	try {
#endif
		char cFileName[255], cDir[63], cTxt[50], seps[] = "= \t\n";
		char* cp, * token, cReadModeA = 0, cReadModeB = 0;
		class CStrTok* pStrTok;
		int i, iAccountid = 0;
		//HANDLE hFile;
		DWORD dwFileSize;
		FILE* pFile;
		BOOL InClass = false;

		ZeroMemory(cTxt, sizeof(cTxt));
		wsprintf(cTxt, "account-character-%s", cWorldName);

		if (strlen(cAccountName) == 0) return 1; // if account blank
		if (strlen(cAccountPass) == 0) return 2; // if password blank
		if ((strlen(cWorldName) == 0)) return 3; // if world dosnt exist

		ZeroMemory(cFileName, sizeof(cFileName));
		ZeroMemory(cDir, sizeof(cDir));
		strcat(cFileName, "Account");
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		wsprintf(cDir, "AscII%d", *cAccountName);
		strcat(cFileName, cDir);
		strcat(cFileName, "\\");
		strcat(cFileName, "\\");
		strcat(cFileName, cAccountName);
		strcat(cFileName, ".txt");

		cReadModeA = 0;
		cReadModeB = 0;

		/*hFile = CreateFileA(cFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		dwFileSize = GetFileSize(hFile, NULL);

		if (hFile != INVALID_HANDLE_VALUE)  CloseHandle(hFile);*/
		pFile = fopen(cFileName, "rt");

		if (pFile == NULL) {
			wsprintf(G_cTxt, "Account Does not Exist (%s)", cFileName);
			AccountLogList(G_cTxt);

			wsprintf(G_cTxt, "Error Num: %d", GetLastError());
			ErrorLogList(G_cTxt);
			return 1;
		}

		fseek(pFile, 0, SEEK_END);
		dwFileSize = ftell(pFile);
		rewind(pFile);

		cp = (char*)malloc(dwFileSize + 1);
		size_t rd_cp = fread(cp, 1, dwFileSize, pFile);
		cp[rd_cp] = '\0';
		pStrTok = new class CStrTok(cp, seps);

		token = pStrTok->pGet();
		while (token != NULL) {
			if (cReadModeA != 0) {
				switch (cReadModeA) {
				case 1:
					if (strlen(token) <= 0) {
						delete pStrTok;
						free(cp);
						return 1;
					}

					for (i = 1; i < DEF_MAXACCOUNTS; i++) {
						if (m_pAccountList[i] != NULL) {
							if (strcmp(m_pAccountList[i]->cAccountName, token) == 0 && strcmp(m_pAccountList[i]->cPassword, cAccountPass) == 0) {
								strcpy(m_pClientList[iClientH]->m_cAccountName, token);
								iAccountid = i;
								*iAccount = i;
								InClass = true;
							}
						}
					}

					if (!InClass) {
						for (i = 1; i < DEF_MAXACCOUNTS; i++) {
							if (m_pAccountList[i] == NULL) {
								m_pAccountList[i] = new class CAccount(token, cWorldName, iClientH);
								strcpy(m_pClientList[iClientH]->m_cAccountName, token); // save account name
								m_iValidAccounts++;
								iAccountid = i;
								*iAccount = i;
								break;
							}
						}
					}
					cReadModeA = 0;
					break;

				case 2:
					if (strlen(token) <= 0) {
						delete pStrTok;
						free(cp);
						delete m_pAccountList[iAccountid];
						m_pAccountList[iAccountid] = NULL;
						return 1;
					}
					strcpy(m_pAccountList[iAccountid]->cPassword, token);
					cReadModeA = 0;
					break;

				case 3:
					if (strlen(token) <= 0) {
						delete pStrTok;
						free(cp);
						delete m_pAccountList[iAccountid];
						m_pAccountList[iAccountid] = NULL;
						return 1;
					}
					strcpy(m_pAccountList[iAccountid]->cEmail, token);
					cReadModeA = 0;
					break;

				case 4:
					if (strlen(token) <= 0) {
						delete pStrTok;
						free(cp);
						delete m_pAccountList[iAccountid];
						m_pAccountList[iAccountid] = NULL;
						return 1;
					}
					m_pAccountList[iAccountid]->iAccountValid = atoi(token);
					cReadModeA = 0;
					break;

				case 5:
					switch (cReadModeB) {
					case 1:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							delete m_pAccountList[iAccountid];
							m_pAccountList[iAccountid] = NULL;
							return 1;
						}
						m_pAccountList[iAccountid]->m_iAccntYear = atoi(token);
						cReadModeB = 2;
						break;

					case 2:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							delete m_pAccountList[iAccountid];
							m_pAccountList[iAccountid] = NULL;
							return 1;
						}
						m_pAccountList[iAccountid]->m_iAccntMonth = atoi(token);
						cReadModeB = 3;
						break;

					case 3:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							delete m_pAccountList[iAccountid];
							m_pAccountList[iAccountid] = NULL;
							return 1;
						}
						m_pAccountList[iAccountid]->m_iAccntDay = atoi(token);
						cReadModeB = 0;
						cReadModeA = 0;
						break;
					}
					break;

				case 6:
					switch (cReadModeB) {
					case 1:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							delete m_pAccountList[iAccountid];
							m_pAccountList[iAccountid] = NULL;
							return 1;
						}
						m_pAccountList[iAccountid]->m_iPassYear = atoi(token);
						cReadModeB = 2;
						break;

					case 2:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							delete m_pAccountList[iAccountid];
							m_pAccountList[iAccountid] = NULL;
							return 1;
						}
						m_pAccountList[iAccountid]->m_iPassMonth = atoi(token);
						cReadModeB = 3;
						break;

					case 3:
						if (strlen(token) <= 0) {
							delete pStrTok;
							free(cp);
							delete m_pAccountList[iAccountid];
							m_pAccountList[iAccountid] = NULL;
							return 1;
						}
						m_pAccountList[iAccountid]->m_iPassDay = atoi(token);
						cReadModeB = 0;
						cReadModeA = 0;
						break;
					}
					break;

				case 7:
					for (i = 0; i < DEF_MAXCHARACTER; i++) {
						if (m_pCharList[i] == NULL) {
							m_pCharList[i] = new class CCharacter(token, iAccountid);
							m_iValidCharacters++;
							break;
						}
					}
					cReadModeA = 0;
					break;
				}
			}
			else {
				if (memcmp(token, "account-name", 12) == 0)			cReadModeA = 1;
				if (memcmp(token, "account-password", 16) == 0)			cReadModeA = 2;
				if (memcmp(token, "account-Email", 13) == 0)			cReadModeA = 3;
				if (memcmp(token, "account-valid-time", 18) == 0)			cReadModeA = 4;
				if (memcmp(token, "account-valid-date", 18) == 0) {
					cReadModeB = 1;
					cReadModeA = 5;
				}

				if (memcmp(token, "account-change-password", 23) == 0) {
					cReadModeB = 1;
					cReadModeA = 6;
				}

				if (memcmp(token, "account-character", 17) == 0)			cReadModeA = 7;
				else if ((memcmp(token, cTxt, strlen(cTxt)) == 0))			cReadModeA = 7;
			}
			token = pStrTok->pGet();
		}

		delete pStrTok;
		free(cp);
		if (pFile != NULL) fclose(pFile);
#ifdef DEF_DEBUG
	}
	catch (...) {
		ErrorLogList("Crash Evitado en: GetAccountInfo");
	}
#endif
	return 0;
}

//heaton change password
void CMainLog::ChangePassword(int iClientH, char* pData)
{
	char cAcc[11] = {};
	char cPassword[11] = {};
	char cNewPw[11] = {};
	char cNewPwConf[11] = {};
	char cWorldName[31] = {};
	char cEmail[52] = {};

	auto cp = (char*)(pData + DEF_INDEX2_MSGTYPE + 2);
	Pop(cp, cAcc, 10);
	Pop(cp, cPassword, 10);
	Pop(cp, cNewPw, 10);
	Pop(cp, cNewPwConf, 10);
	Pop(cp, cWorldName, 30);
	Pop(cp, cEmail, 52);

	wsprintf(G_cTxt, "(!) Request change password: %s", cAcc);
	PutLogList(G_cTxt);

	int iAccount = -1;
	bool nochange = false;
	int iMessage = GetAccountInfo2(iClientH, cAcc,cEmail, cPassword, cWorldName, &iAccount);
	switch (iMessage) {
	case 0: //0 errors check password and send success message
		if (strcmp(m_pAccountList[iAccount]->cAccountName, cAcc) == 0 && m_pAccountList[iAccount] != NULL && strcmp(m_pAccountList[iAccount]->cWorldName, cWorldName) != 0) {
			wsprintf(G_cTxt, "Account(%s) World(%s) none exist", cWorldName, cWorldName);
			AccountLogList(G_cTxt);
		}
		else if (strcmp(m_pAccountList[iAccount]->cAccountName, cAcc) != 0) nochange = true;
		else if (strcmp(m_pAccountList[iAccount]->cPassword, cPassword) != 0) {
			wsprintf(G_cTxt, "Account(%s) Password Wrong(%s)(%s)", cAcc, m_pAccountList[iAccount]->cPassword, cPassword);
			AccountLogList(G_cTxt);
			nochange = true;
		}
		else if (strcmp(m_pAccountList[iAccount]->cEmail, cEmail) != 0 || m_pAccountList[iAccount] == NULL)
		{
			nochange = true;
		}
		break;
	default:
		SendEventToClient(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
		return;
		break;
	}

	if (string(cNewPw) != cNewPwConf || nochange)
	{
		SendEventToClient(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
		return;
	}

	char cBuffer[1024] = {};
	char cBuffer2[1024] = {};
	char cTmp[1024] = {};
	char cTxt[1024] = {};
	char cTxt2[1024] = {};
	int iTest = -1;
	wsprintf(cTmp, "Account\\AscII%d\\%s.txt ", cAcc[0], cAcc);
	HANDLE  hFile = CreateFile(cTmp, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		SendEventToClient(DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, DEF_LOGRESMSGTYPE_PASSWORDCHANGEFAIL, 0, 0, iClientH);
		CloseHandle(hFile);
		return;
	}
	auto iSize = GetFileSize(hFile, NULL);
	CloseHandle(hFile);

	char cFileName[512] = {};
	char cDir[112] = {};
	ZeroMemory(cFileName, sizeof(cFileName));
	ZeroMemory(cDir, sizeof(cDir));
	strcat(cFileName, "Account");
	strcat(cFileName, "\\");
	wsprintf(cDir, "AscII%d", *cAcc);
	strcat(cFileName, cDir);
	strcat(cFileName, "\\");
	strcat(cFileName, cAcc);
	strcat(cFileName, ".txt");

	ifstream in(cFileName, ios::in);
	if (!in.is_open())
	{
		PutLogList("in is_opem = false");
		return;
	}
	string wordToReplace("account-password = ");
	wordToReplace.append(cPassword);

	string wordToReplaceWith("account-password = ");
	wordToReplaceWith.append(cNewPw);

	stringstream ss;
	bool done = false;
	string line;
	size_t len = wordToReplace.length();
	while (getline(in, line))
	{
		string repl = wordToReplace;
		size_t pos = line.find(repl);
		len = repl.length();

		if (pos != string::npos)
		{
			line.replace(pos, len, wordToReplaceWith);
			done = true;
		}

		ss << line << '\n';
	}
	in.close();

	ofstream out(cFileName);
	out << ss.str();
	if (done)
	{
		SendEventToClient(DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS, DEF_LOGRESMSGTYPE_PASSWORDCHANGESUCCESS, 0, 0, iClientH);
	}
}