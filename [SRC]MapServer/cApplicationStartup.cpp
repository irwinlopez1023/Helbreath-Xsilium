#pragma warning (disable : 4474)
#include "cApplicationStartup.h"
#include <stdio.h>
#include "sql.h"
#include "Header/winmain.h"
#include "Header/Game.h"
#include "General/UserMessages.h"
#include "language/resource.h"
#include <shellapi.h>
#include <shlobj.h>
#include "Header/Event.h"
#include "Header/XSocket.h"

Event::Handler g_ev;

#define WM_USER_TIMERSIGNAL		WM_USER + 500
#pragma warning(disable : 4996)

#define ID_BUTTON 501
#define clNavy							0x800000
#define clWhite							0xFFFFFF

static HWND    List1;
static HWND    List2;
static HWND    List3;
static HWND    List4;
static HWND    List5;
static HWND    Edit1;
static HWND    Button1;
static HWND    Button2;
static HWND    Button4;
static HWND    Button5;
static HWND    Button6;
static HINSTANCE BCX_hInstance;
static int     BCX_ScaleX;
static int     BCX_ScaleY;
static char    BCX_ClassName[2048];
HWND    BCX_Listbox(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Listbox2(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Listbox3(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Listbox4(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Listbox5(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Editbox(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Button(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Button2(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Button4(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Button5(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Button6(char*, HWND, int, int, int, int, int, int = 0, int = -1);
int ItemCount = 0, ItemCount2 = 0, ItemCount3 = 0, ItemCount4 = 0, ItemCount5 = 0;
int J = 0, i = 0;

char			szAppClass[32];
HWND			G_hWnd = NULL;
char			G_cMsgList[120 * 50];
BOOL            G_cMsgUpdated = FALSE;
char            G_cTxt[512];
char			G_cData50000[50000];
MMRESULT        G_mmTimer = NULL;

char			G_cLogBuffer[30000];
char			G_cLogBuffer2[30000];
char			G_cLogBuffer3[30000];
char			G_cLogBuffer4[30000];
char			G_cLogBuffer5[30000];
char			G_cLogBuffer6[30000];
char			G_cLogBuffer7[30000];
char			G_cLogBuffer8[30000];
char			G_cLogBuffer9[30000];
char			G_cLogBuffer10[30000];
//lalov9 drops
char			G_cLogBuffer11[30000];

short			G_sLogCounter;
DWORD			G_dwLogTime;

class XSocket * G_pListenSock = NULL;
class XSocket * G_pLogSock = NULL;
class CMapServer *   c_map = NULL;

int             G_iQuitProgramCount = 0;
BOOL			G_bIsThread = TRUE;
BOOL            G_bShutdown = FALSE;
char			G_cCrashTxt[50000];

MMRESULT G_mmTimerCopy = NULL;
bool started;

void Borrado()
{
	G_cMsgUpdated = TRUE;
	SendMessage(List5, (UINT)LB_RESETCONTENT, 0, 0);
	SendMessage(List5, (UINT)LB_SETCURSEL, ItemCount5, 0);
	ItemCount5 = 0;
	ChatLogs("Limpio!");
}

void _StopTimer(MMRESULT timerid)
{
	TIMECAPS caps;

	if (timerid != 0) {
		timeKillEvent(timerid);
		timerid = 0;
		timeGetDevCaps(&caps, sizeof(caps));
		timeEndPeriod(caps.wPeriodMin);
	}
}

// ---------------------------------------------------------------------------
// Logs del servidor
//
// Antes cada funcion de log tenia su propia copia del mismo codigo, con doble
// fclose, strcat sin limite sobre buffers de 30.000 bytes y un contador
// (G_sLogCounter) compartido por todas. Ahora todas pasan por _LogBuffered,
// y cada log lleva su propio contador y su propia hora de volcado.
// ---------------------------------------------------------------------------

struct stLogState {
	short sCounter;
	DWORD dwTime;
	stLogState() : sCounter(0), dwTime(0) {}
};

static stLogState G_stLogMapServer, G_stLogGM, G_stLogCharacters, G_stLogErrors, G_stLogChats;
static stLogState G_stLogTrade, G_stLogCoins, G_stLogHacks, G_stLogDrops, G_stLogItems, G_stLogEkPk;

// Escribe el buffer al archivo del dia y lo vacia. Si el archivo no abre, se descarta.
static void _LogFlush(char * pBuffer, size_t iBufSize, stLogState * pState, const char * cDir, const char * cFileFmt)
{
	char cFileName[256];
	SYSTEMTIME SysTime;
	FILE * pLogFile;

	if (pBuffer[0] != 0) {
		GetLocalTime(&SysTime);
		_mkdir("..\\ServerLogs");
		_mkdir(cDir);

		ZeroMemory(cFileName, sizeof(cFileName));
		_snprintf(cFileName, sizeof(cFileName) - 1, cFileFmt, SysTime.wDay, SysTime.wMonth, SysTime.wYear);

		pLogFile = fopen(cFileName, "at");
		if (pLogFile != NULL) {
			fwrite(pBuffer, 1, strlen(pBuffer), pLogFile);
			fclose(pLogFile);
			pLogFile = NULL;
		}
	}

	pState->sCounter = 0;
	ZeroMemory(pBuffer, iBufSize);
}

// Acumula una linea con hora y la vuelca cada 100 lineas, cada 10 segundos,
// cuando el buffer se llena o cuando bFlushNow es TRUE.
static void _LogBuffered(char * pBuffer, size_t iBufSize, stLogState * pState, const char * cDir, const char * cFileFmt, const char * cMsg, BOOL bFlushNow)
{
	char cLine[600];
	SYSTEMTIME SysTime;
	DWORD dwTime;

	if (cMsg == NULL) return;

	dwTime = timeGetTime();
	if (pState->sCounter == 0) pState->dwTime = dwTime;
	pState->sCounter++;

	GetLocalTime(&SysTime);
	ZeroMemory(cLine, sizeof(cLine));
	_snprintf(cLine, sizeof(cLine) - 1, "%02d:%02d:%02d\t%s\n", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cMsg);
	cLine[sizeof(cLine) - 1] = 0;

	// Si no cabe, primero se vuelca lo acumulado
	if (strlen(pBuffer) + strlen(cLine) >= iBufSize) _LogFlush(pBuffer, iBufSize, pState, cDir, cFileFmt);
	if (strlen(pBuffer) + strlen(cLine) < iBufSize) strcat(pBuffer, cLine);

	if (bFlushNow || (pState->sCounter >= 100) || (dwTime - pState->dwTime > 10 * 1000))
		_LogFlush(pBuffer, iBufSize, pState, cDir, cFileFmt);
}

void PutLogList(char * cMsg)
{
	std::cout << cMsg << std::endl;
	_LogBuffered(G_cLogBuffer, sizeof(G_cLogBuffer), &G_stLogMapServer, "..\\ServerLogs\\MapServer",
		"..\\ServerLogs\\MapServer\\MapServerLogs [%02d-%02d-%04d].log", cMsg, FALSE);
}

void PutGMLogData(char * cStr)
{
	_LogBuffered(G_cLogBuffer4, sizeof(G_cLogBuffer4), &G_stLogGM, "..\\ServerLogs\\GM",
		"..\\ServerLogs\\GM\\GMLogs [%02d-%02d-%04d].log", cStr, FALSE);
}

void CharacterLogList(char * cMsg)
{
	std::cout << cMsg << std::endl;
	_LogBuffered(G_cLogBuffer7, sizeof(G_cLogBuffer7), &G_stLogCharacters, "..\\ServerLogs\\Characters",
		"..\\ServerLogs\\Characters\\CharactersLogs [%02d-%02d-%04d].log", cMsg, FALSE);
}

void ConfigList(char * cMsg)
{
}

void UpdateConfigList(char * cMsg)
{
	std::cout << cMsg << std::endl;
}

void ErrorList(char * cMsg)
{
	std::cout << cMsg << std::endl;
	_LogBuffered(G_cLogBuffer9, sizeof(G_cLogBuffer9), &G_stLogErrors, "..\\ServerLogs\\MapServer Errors",
		"..\\ServerLogs\\MapServer Errors\\MS Errors [%02d-%02d-%04d].log", cMsg, FALSE);
}

void ChatLogs(char * cMsg)
{
	std::cout << cMsg << std::endl;
	_LogBuffered(G_cLogBuffer3, sizeof(G_cLogBuffer3), &G_stLogChats, "..\\ServerLogs\\Chats",
		"..\\ServerLogs\\Chats\\ChatsLogs [%02d-%02d-%04d].log", cMsg, FALSE);
}

void PutLogTradeFileList(char * cStr)
{
	_LogBuffered(G_cLogBuffer10, sizeof(G_cLogBuffer10), &G_stLogTrade, "..\\ServerLogs\\TradeLogs",
		"..\\ServerLogs\\TradeLogs\\LogTrade [%02d-%02d-%04d].log", cStr, FALSE);
}

void PutLogCoinsFileList(char * cStr)
{
	_LogBuffered(G_cLogBuffer5, sizeof(G_cLogBuffer5), &G_stLogCoins, "..\\ServerLogs\\Coins",
		"..\\ServerLogs\\Coins\\CoinsLogs [%02d-%02d-%04d].log", cStr, FALSE);
}

void PutLogHacksFileList(char * cStr)
{
	std::cout << cStr << std::endl;
	// Los hacks se escriben al momento, sin esperar a juntar 100
	_LogBuffered(G_cLogBuffer6, sizeof(G_cLogBuffer6), &G_stLogHacks, "..\\ServerLogs\\Hacks",
		"..\\ServerLogs\\Hacks\\HacksLogs [%02d-%02d-%04d].log", cStr, TRUE);
}

//Agregado LogDrop Lalov9
void PutLogDrops(char * cStr)
{
	std::cout << cStr << std::endl;
	_LogBuffered(G_cLogBuffer11, sizeof(G_cLogBuffer11), &G_stLogDrops, "..\\ServerLogs\\Drops",
		"..\\ServerLogs\\Drops\\DropsLogs [%02d-%02d-%04d].log", cStr, FALSE);
}

void PutLogOnlinesFileList(char * cStr)
{
	FILE * pLogFile;

	if (cStr == NULL) return;

	_mkdir("..\\ServerLogs");
	_mkdir("..\\ServerLogs\\OnlineUsers");

	pLogFile = fopen("..\\ServerLogs\\OnlineUsers\\OnlineUsers.log", "w");
	if (pLogFile == NULL) return;

	fwrite(cStr, 1, strlen(cStr), pLogFile);
	fclose(pLogFile);
	pLogFile = NULL;
}

void PutLogItemsList(char * cMsg)
{
	_LogBuffered(G_cLogBuffer2, sizeof(G_cLogBuffer2), &G_stLogItems, "..\\ServerLogs\\Items",
		"..\\ServerLogs\\Items\\ItemLogs [%02d-%02d-%04d].log", cMsg, FALSE);
}

void PutEkAndPkLogFileList(char * cMsg)
{
	_LogBuffered(G_cLogBuffer8, sizeof(G_cLogBuffer8), &G_stLogEkPk, "..\\ServerLogs\\EksAndPks",
		"..\\ServerLogs\\EksAndPks\\EksAndPksLogs [%02d-%02d-%04d].log", cMsg, FALSE);
}

// Aviso de intento de ataque al servidor: en rojo en la consola y al momento
// en ..\ServerLogs\Ataques\Ataques [dd-mm-aaaa].txt
void PutLogAtaque(char * cStr)
{
	char cLine[1100], cFileName[256];
	SYSTEMTIME SysTime;
	FILE * pLogFile;
	HANDLE hConsole;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	BOOL bColor;

	if (cStr == NULL) return;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	bColor = ((hConsole != NULL) && (hConsole != INVALID_HANDLE_VALUE) && GetConsoleScreenBufferInfo(hConsole, &csbi));
	if (bColor) SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::cout << "[ATAQUE] " << cStr << std::endl;
	if (bColor) SetConsoleTextAttribute(hConsole, csbi.wAttributes);

	GetLocalTime(&SysTime);
	_mkdir("..\\ServerLogs");
	_mkdir("..\\ServerLogs\\Ataques");

	ZeroMemory(cFileName, sizeof(cFileName));
	_snprintf(cFileName, sizeof(cFileName) - 1, "..\\ServerLogs\\Ataques\\Ataques [%02d-%02d-%04d].txt", SysTime.wDay, SysTime.wMonth, SysTime.wYear);

	ZeroMemory(cLine, sizeof(cLine));
	_snprintf(cLine, sizeof(cLine) - 1, "%02d:%02d:%02d\t%s\n", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cStr);
	cLine[sizeof(cLine) - 1] = 0;

	pLogFile = fopen(cFileName, "at");
	if (pLogFile == NULL) return;
	fwrite(cLine, 1, strlen(cLine), pLogFile);
	fclose(pLogFile);
	pLogFile = NULL;
}

bool Initialize()
{
	WSADATA data;

	auto version = MAKEWORD(2, 2);
	auto err = WSAStartup(version, &data);
	if (err)
		return false;

	return true;
}

void Deinitialize()
{
	WSACleanup();
}

void OnDestroy()
{
	if (G_pListenSock != NULL) delete G_pListenSock;
	if (G_pLogSock != NULL) delete G_pLogSock;

	if (c_map != NULL) {
		c_map->Quit();
		delete c_map;
	}

	if (G_mmTimer != NULL) _StopTimer(G_mmTimer);
	///_TermWinsock();
	Deinitialize();

	PostQuitMessage(0);
}


void OnAccept()
{
	c_map->bAccept(G_pListenSock);
}

void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2)
{
	PostMessage(G_hWnd, WM_USER_TIMERSIGNAL, wID, NULL);
}

LRESULT CALLBACK BackgroundWindowProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND:
		switch (wParam) {
		case 1012:	
			break;
		case 1013:	
			break;
		case 1010:	
			char commandtxt[100];
			if (commandtxt != NULL) {
				
			}
			break;
		}
		break;

	case WM_CREATE:
		break;

	case WM_KEYDOWN:
		c_map->OnKeyDown(wParam, lParam);
		return (DefWindowProc(hWnd, message, wParam, lParam));
		break;

	case WM_KEYUP:
		c_map->OnKeyUp(wParam, lParam);
		return (DefWindowProc(hWnd, message, wParam, lParam));
		break;

	case WM_USER_STARTGAMESIGNAL:
		c_map->OnStartGameSignal();
		break;

	case WM_USER_TIMERSIGNAL:
		if (!G_bShutdown) {
			c_map->OnTimer(NULL);
		}
		break;

	case WM_USER_ACCEPT:
		OnAccept();
		break;

	case WM_PAINT:
		//OnPaint();
		break;

	case WM_DESTROY:
		G_bShutdown = TRUE;
		OnDestroy();
		break;

	case WM_CLOSE:
		if (c_map->bOnClose()) {
			return (DefWindowProc(hWnd, message, wParam, lParam));
		}
		break;
	case WM_ONGATESOCKETEVENT:
		if (G_bShutdown == TRUE) break;
		break;

	case WM_ONLOGSOCKETEVENT:
		if (G_bShutdown == TRUE) break;
		c_map->OnMainLogSocketEvent(message, wParam, lParam);
		break;

	default:
		if (G_bShutdown == TRUE) break;
		if ((message >= WM_ONLOGSOCKETEVENT + 1) && (message <= WM_ONLOGSOCKETEVENT + DEF_MAXSUBLOGSOCK))
			c_map->OnSubLogSocketEvent(message, wParam, lParam);

		if ((message >= WM_ONCLIENTSOCKETEVENT) && (message < WM_ONCLIENTSOCKETEVENT + DEF_MAXCLIENTS))
			c_map->OnClientSocketEvent(message, wParam, lParam);

		return (DefWindowProc(hWnd, message, wParam, lParam));
	}

	return NULL;
}

void cApplicationStartup::Startup()
{
	started = false;

	printf("(!) Starting CoreServer \n");
	static unsigned short _usCnt = 0;
	register MSG msg;

	InitializeSockets();	
	StartCoreserver();

	G_mmTimerCopy = this->StartTimer(1);

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else WaitMessage();
	}
}

cApplicationStartup::cApplicationStartup()
{
}

cApplicationStartup::~cApplicationStartup()
{
}

MMRESULT cApplicationStartup::StartTimer(DWORD dwTime)
{
	TIMECAPS caps;
	MMRESULT timerid;

	timeGetDevCaps(&caps, sizeof(caps));
	timeBeginPeriod(caps.wPeriodMin);
	timerid = timeSetEvent(dwTime, 0, _TimerFunc, 0, (UINT)TIME_PERIODIC);

	return timerid;
}

HWND cApplicationStartup::CreateBackgroundWindow()
{
	WNDCLASS wndclass;
	CHAR *ProviderClass = "AsyncSelect";
	HWND Window;
	char cTitle[100];

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = &BackgroundWindowProcess;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = ProviderClass;

	if (RegisterClass(&wndclass) == 0)
	{
		printf("RegisterClass() failed with error %d\n", GetLastError());
		return NULL;
	}
	else {
		//printf("RegisterClass() is OK!\n");
	}

	Window = CreateWindowEx(
		0,
		ProviderClass,
		cTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		NULL,
		NULL);

	if (Window == NULL)
	{
		printf("CreateWindow() failed with error %d\n", GetLastError());
		return NULL;
	}
	else {
		//printf("CreateWindow() is OK!\n");
	}
	return Window;
}

void cApplicationStartup::InitializeSockets()
{
	if (Initialize() == FALSE) {
		printf("Socket 2.2 not found! Cannot execute program.", "ERROR", MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(0);
		return;
	}
}

void cApplicationStartup::StartCoreserver()
{	
	G_hWnd = CreateBackgroundWindow();
	
	c_map = new CMapServer(G_hWnd);
	if (c_map->bInit() == FALSE) {
		printf("(!!!) STOPPED!");
		PutLogList("(!!!) STOPPED!");
		return;
	}
	
	started = true;

	G_pListenSock = new class XSocket(G_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	G_pListenSock->bListen(c_map->m_cGameServerAddr, c_map->m_iGameServerPort, WM_USER_ACCEPT);

}
