#pragma warning (disable : 4474)
#include "cApplicationStartup.h"
#include <windows.h>
#include "../Header/Wmain.h"
#include "../Header/MainLog.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include "../General/GlobalDef.h"
#include <thread>

char		szAppClass[1000];
WNDCLASS	wnd;
HWND		G_hWnd;
WSADATA		wsInfo;

BOOL G_cMsgUpdated = FALSE;
char G_cMsgList[120 * 50];
char            G_cTxt[500];
class CMainLog *G_pMainLog;
//probando laloramos
//char			GcTxt2[500];

//Timer
MMRESULT        G_mmTimer = NULL;
BOOL			G_bIsThread = TRUE;
static HINSTANCE BCX_hInstance;
static int     BCX_ScaleX, BCX_ScaleY;
static char    BCX_ClassName[2048];

static HWND    List1;
static HWND    List2;
static HWND    List3;
static HWND    List4;
static HWND    List5;
static HWND    Button1;
static HWND    Button2;
static HWND    Edit1;

HWND hSv;
HWND BCX_Listbox(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND BCX_Listbox2(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND BCX_Listbox3(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND BCX_Listbox4(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND BCX_Listbox5(char*, HWND, int, int, int, int, int, int = 0, int = -1);

HWND    BCX_Button(char*, HWND, int, int, int, int, int, int = 0, int = -1);
HWND    BCX_Button2(char*, HWND, int, int, int, int, int, int = 0, int = -1);

HWND    BCX_Editbox(char*, HWND, int, int, int, int, int, int = 0, int = -1);

HINSTANCE hInst;

int ItemCount = 0, ItemCount2 = 0, ItemCount3 = 0, ItemCount5 = 0;
class XSocket * G_pInternalLogSock = NULL;

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

void Initialize()
{
	WSADATA data;

	auto version = MAKEWORD(2, 2);
	auto err = WSAStartup(version, &data);
	if (err)
		return;

	return;
}

void Deinitialize()
{
	WSACleanup();
}

void OnDestroy()
{
	if (G_pMainLog != NULL) delete G_pMainLog;
	if (G_pXMainLogSock != NULL) delete G_pXMainLogSock;
	if (G_pInternalLogSock != NULL) delete G_pInternalLogSock;
	_TermWinsock();
	if (G_mmTimer != NULL) _StopTimer(G_mmTimer);
	PostQuitMessage(0);
}

void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2)
{
	PostMessage(G_hWnd, WM_USER_TIMERSIGNAL, wID, NULL);
}

LRESULT CALLBACK BackgroundWindowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg){
		//---------Buttons---------//
	case WM_COMMAND:
		switch (wParam) {
		case 1010:
			G_cMsgUpdated = TRUE;
			SendMessage(List1, (UINT)LB_RESETCONTENT, 0, 0);
			ItemCount = 0;
			PutLogList("Limpio!");
			break;

		case 1011:
			if (!LoginEnabled) {
				Button1 = BCX_Button("Enabled", G_hWnd, 1011, 730, 8, 90, 20); // Server Logs Button
				PutLogList("Login Enabled!");
				LoginEnabled = true;
				G_pXMainLogSock = new class XSocket(G_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
				G_pXMainLogSock->bListen(G_pMainLog->m_cMainServerAddress, G_pMainLog->m_iMainServerPort, WM_USER_ACCEPT);
			}
			break;

		case 1013:
			char cWhat[256];
			GetWindowText(Edit1, cWhat, 256);
			if (cWhat != NULL) {
				SetWindowText(Edit1, NULL);
				SetFocus(Edit1);
				ParseCommand(cWhat);
			}
			break;

		}
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd, "Quit Main log server?", "Main-LOG", MB_YESNO + MB_ICONINFORMATION) == IDYES) {
			return DefWindowProc(hWnd, WM_CLOSE, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		OnDestroy();
		break;

	case WM_PAINT:
		//OnPaint();
		break;

	case WM_USER_TIMERSIGNAL:
		G_pMainLog->OnTimer();
		break;

	case WM_ONMAPSERVERSOCKETEVENT: // From MapServer
		G_pMainLog->bAccept_MapServer(G_pInternalLogSock);
		break;

	case WM_USER_ACCEPT: //from anything
		G_pMainLog->bAccept(G_pXMainLogSock);
		break;

	default:
		if ((uMsg >= WM_ONCLIENTSOCKETEVENT) && (uMsg <= WM_ONCLIENTSOCKETEVENT + DEF_MAXCLIENTSOCK)) {
			G_pMainLog->OnClientSubLogSocketEvent(uMsg, wParam, lParam);
		}
		else if ((uMsg >= WM_ONMAPSERVERSOCKETEVENT) && (uMsg <= WM_ONMAPSERVERSOCKETEVENT + DEF_MAXCLIENTS)) {
			G_pMainLog->OnMapServerSubLogSocketEvent(uMsg, wParam, lParam);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
	return 0;
}

void cApplicationStartup::Startup()
{
	printf("(!) Starting LoginServer \n");
	static unsigned short _usCnt = 0;
	register MSG msg;

	InitializeSockets();	
	StartLoginServer();

	G_mmTimer = this->StartTimer(1);

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
	Initialize();
}

void cApplicationStartup::StartLoginServer()
{	
	G_hWnd = CreateBackgroundWindow();
	
	G_pMainLog = new class CMainLog(G_hWnd);
	if (G_pMainLog == NULL) {
		MessageBox(G_hWnd, "Init fail - Invalid WorldLServer!", "ERROR", MB_OK + MB_ICONERROR);
		return;
	}

	if (G_pMainLog->bInit() == FALSE) {
		MessageBox(G_hWnd, "Init fail - Missing required .cfg files!", "ERROR", MB_OK + MB_ICONERROR);
		return;
	}

	G_bIsMainLogActive = TRUE;
	G_pInternalLogSock = new class XSocket(G_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	G_pInternalLogSock->bListen(G_pMainLog->m_cMainServerAddress, G_pMainLog->m_iMainServerInternalPort, WM_ONMAPSERVERSOCKETEVENT);


	std::thread timerThread([this]() {
		std::this_thread::sleep_for(std::chrono::seconds(15));
		LoginEnabled = true;
		G_pXMainLogSock = new class XSocket(G_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
		G_pXMainLogSock->bListen(G_pMainLog->m_cMainServerAddress, G_pMainLog->m_iMainServerPort, WM_USER_ACCEPT);
		PutLogList("Login Enabled");
	});
	timerThread.detach();


}

int EventLoop()
{
	static unsigned short _usCnt = 0;
	register MSG msg;

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				return msg.wParam;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			UpdateScreen();
		}
		else WaitMessage();
	}
}

void ThreadProc(void *ch)
{
	while (G_bIsThread == TRUE) {
		if (G_pMainLog = NULL) G_pMainLog->OnTimer();
		Sleep(1);
	}
	ExitThread(0);
}


void ConfigList(char * cMsg)
{
	std::cout << cMsg << std::endl;
}

void UpdateConfigList(char * cMsg)
{
	std::cout << cMsg << std::endl;
}

void CommandList(char * cMsg)
{
	std::cout << cMsg << std::endl;
}

// ---------------------------------------------------------------------------
// Logs del Nucleo. Antes tenian doble fclose y strcat sin limite sobre
// buffers de 30.000 bytes; ahora pasan por _LogBuffered.
// ---------------------------------------------------------------------------

// Escribe el buffer al archivo del dia y lo vacia. Si el archivo no abre, se descarta.
static void _LogFlush(char * pBuffer, size_t iBufSize, short * pCounter, const char * cDir, const char * cFileFmt)
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

	*pCounter = 0;
	ZeroMemory(pBuffer, iBufSize);
}

// Acumula una linea con hora; vuelca cada 100 lineas, cada 10 segundos o si el buffer se llena.
static void _LogBuffered(char * pBuffer, size_t iBufSize, short * pCounter, DWORD * pTime, const char * cDir, const char * cFileFmt, const char * cMsg)
{
	char cLine[600];
	SYSTEMTIME SysTime;
	DWORD dwTime;

	if (cMsg == NULL) return;

	dwTime = timeGetTime();
	if (*pCounter == 0) *pTime = dwTime;
	(*pCounter)++;

	GetLocalTime(&SysTime);
	ZeroMemory(cLine, sizeof(cLine));
	_snprintf(cLine, sizeof(cLine) - 1, "%02d:%02d:%02d\t%s\n", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cMsg);
	cLine[sizeof(cLine) - 1] = 0;

	if (strlen(pBuffer) + strlen(cLine) >= iBufSize) _LogFlush(pBuffer, iBufSize, pCounter, cDir, cFileFmt);
	if (strlen(pBuffer) + strlen(cLine) < iBufSize) strcat(pBuffer, cLine);

	if ((*pCounter >= 100) || (dwTime - *pTime > 10 * 1000))
		_LogFlush(pBuffer, iBufSize, pCounter, cDir, cFileFmt);
}

void PutLogList(char * cMsg)
{
	std::cout << cMsg << std::endl;
	_LogBuffered(G_cLogBuffer, sizeof(G_cLogBuffer), &G_sLogCounter, &G_dwLogTime, "..\\ServerLogs\\Nucleo",
		"..\\ServerLogs\\Nucleo\\NucleoLog [%02d-%02d-%04d].log", cMsg);
}

void AccountLogList(char * cMsg)
{
	std::cout << cMsg << std::endl;
	_LogBuffered(G_cAccountBuffer, sizeof(G_cAccountBuffer), &G_sAccountCounter, &G_dwAccountTime, "..\\ServerLogs\\Accounts",
		"..\\ServerLogs\\Accounts\\AccountLog [%02d-%02d-%04d].log", cMsg);
}

// Aviso de intento de ataque al servidor: en rojo en la consola y al momento
// en ..\ServerLogs\Ataques\Ataques Nucleo [dd-mm-aaaa].txt
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
	_snprintf(cFileName, sizeof(cFileName) - 1, "..\\ServerLogs\\Ataques\\Ataques Nucleo [%02d-%02d-%04d].txt", SysTime.wDay, SysTime.wMonth, SysTime.wYear);

	ZeroMemory(cLine, sizeof(cLine));
	_snprintf(cLine, sizeof(cLine) - 1, "%02d:%02d:%02d\t%s\n", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cStr);
	cLine[sizeof(cLine) - 1] = 0;

	pLogFile = fopen(cFileName, "at");
	if (pLogFile == NULL) return;
	fwrite(cLine, 1, strlen(cLine), pLogFile);
	fclose(pLogFile);
	pLogFile = NULL;
}

void ErrorLogList(char * cMsg)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	//FILE * pErrorFile;

	std::cout << cMsg << std::endl;

	/*dwTime = timeGetTime();

	if (G_sAccountCounter == 0) G_dwAccountTime = dwTime;

	G_sAccountCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	wsprintf(cTempBuffer, "%02d:%02d:%02d\t", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cTempBuffer, cMsg);
	strcat(cTempBuffer, "\n");

	strcat(G_cAccountBuffer, cTempBuffer);

	if (G_sAccountCounter >= 100 || (dwTime - G_dwAccountTime  >  10 * 1000)) {
	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_mkdir("..\\ServerLogs\\NucleoErrors");

	wsprintf(cTempBuffer, "..\\ServerLogs\\NucleoErrors\\ErrosLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);

	pErrorFile = fopen(cTempBuffer, "at");
	if (pErrorFile == NULL) return;
	fwrite(G_cAccountBuffer, 1, strlen(G_cAccountBuffer), pErrorFile);
	fclose(pErrorFile);

	G_sAccountCounter = 0;
	ZeroMemory(G_cAccountBuffer, sizeof(G_cAccountBuffer));

	if (pAccountFile != NULL) fclose(pAccountFile);
	}*/

}


void ParseCommand(char* pMsg)
{
	if (pMsg == NULL) return;

	char   seps[] = "= \t\n";
	char   * token, *token2;
	class  CStrTok * pStrTok;
	char * cName[11];
	char buff[256];
	BOOL bFlag;
	int i;
	pStrTok = new class CStrTok(pMsg, seps);
	token = pStrTok->pGet();
	token2 = pStrTok->pGet();
	bFlag = false;

	ZeroMemory(cName, sizeof(cName));

	SendMessage(List5, (UINT)LB_RESETCONTENT, 0, 0);
	ItemCount5 = 0;

	if (memcmp(pMsg, "/help", 5) == 0) {
		bFlag = TRUE;
		wsprintf(buff, "Command List:");
		CommandList(buff);
		wsprintf(buff, "****************************************************");
		CommandList(buff);
		wsprintf(buff, "/deslog [Acc]");
		CommandList(buff);
		wsprintf(buff, "* Force Login Out Account");
		CommandList(buff);
		wsprintf(buff, "----------------------------------------------------------------------");
		CommandList(buff);

		wsprintf(buff, "/help");
		CommandList(buff);
		wsprintf(buff, "* List of commands");
		CommandList(buff);
		return;
	}

	if (memcmp(pMsg, "/deslog ", 8) == 0) {
		bFlag = TRUE;
		wsprintf(buff, "Login Out Acc: %s", token);
		CommandList(buff);
		G_pMainLog->DeslogAcc(token);
		return;
	}

	if ((!bFlag) && (pMsg != NULL)) {
		wsprintf(buff, "%s - Comando Invalido", pMsg);
		CommandList(buff);
		CommandList("List Comand: '/help' ");
		return;
	}
}

void UpdateScreen()
{
	if (G_cMsgUpdated == TRUE) {
		InvalidateRect(G_hWnd, NULL, TRUE);
		G_cMsgUpdated = FALSE;
	}
}


HWND BCX_Button(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_CHILD | WS_VISIBLE | WS_BORDER;

	A = CreateWindowEx(0, "Button", Text, Style, X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY, hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}