/*
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <winbase.h>
#include <mmsystem.h>
#include <time.h>				   
#include "../Header/winmain.h"
#include "../Header/Game.h"
#include "../General/UserMessages.h"
#include "../language/resource.h"
#include <shellapi.h>
#include <shlobj.h>
#include "../Header/Event.h"

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

void ThreadProc(void *ch)
{
	while (G_bIsThread == TRUE) {
		if (c_map = NULL)	c_map->OnTimer(NULL);
		Sleep(1);
	}

	ExitThread(0);
}

LRESULT CALLBACK fnMessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_COMMAND:
			switch (wParam) {

				case 1010:
					G_cMsgUpdated = TRUE;
					SendMessage(List1, (UINT)LB_RESETCONTENT, 0, 0);
					SendMessage(List1, (UINT)LB_SETCURSEL, ItemCount, 0);
					ItemCount = 0;
					PutLogList("Limpio!");
					break;

				case 1011:
					G_cMsgUpdated = TRUE;
					SendMessage(List4, (UINT)LB_RESETCONTENT, 0, 0);
					SendMessage(List4, (UINT)LB_SETCURSEL, ItemCount4, 0);
					ItemCount4 = 0;
					ErrorList("Limpio!");
					break;

				case 1013:
					G_cMsgUpdated = TRUE;
					SendMessage(List2, (UINT)LB_RESETCONTENT, 0, 0);
					SendMessage(List2, (UINT)LB_SETCURSEL, ItemCount2, 0);
					//ItemCount2 = 0;
					CharacterLogList("Limpio!");
					break;

				case 1017:
					G_cMsgUpdated = TRUE;
					SendMessage(List5, (UINT)LB_RESETCONTENT, 0, 0);
					SendMessage(List5, (UINT)LB_SETCURSEL, ItemCount5, 0);
					ItemCount5 = 0;
					ChatLogs("Limpio!");
					break;

				case 1019:
					char cWhat[256];
					GetWindowText(Edit1, cWhat, 256);
					if (cWhat != NULL) {
						SetWindowText(Edit1, NULL);
						SetFocus(Edit1);
						c_map->ParseCommand(cWhat);
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
			if (G_bShutdown == FALSE) c_map->OnTimer(NULL);
			break;

		case WM_USER_ACCEPT:
			OnAccept();
			break;

		case WM_PAINT:
			OnPaint();
			break;

		case WM_DESTROY:
			G_bShutdown = TRUE;
			OnDestroy();
			break;

		case WM_CLOSE:
			if (c_map->bOnClose() == TRUE) return (DefWindowProc(hWnd, message, wParam, lParam));
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

BOOL CALLBACK lpCrashDialogFunc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HANDLE outHand;
	char cCrashFileName[MAX_PATH];
	char cLF[] = { 0x0d,0x0a };
	char cDash = '-';
	SYSTEMTIME sysTime;
	DWORD written;

	switch (uMsg) {
		case WM_CLOSE:
			EndDialog(hDlg, TRUE);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_CLOSE:
					EndDialog(hDlg, TRUE);
					break;
			}
			break;

		case WM_INITDIALOG:
			//Show Crash Data
			SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), G_cCrashTxt);
			GetLocalTime(&sysTime);
			wsprintf(cCrashFileName, "CrashData - %d-%d-%d.txt", sysTime.wDay, sysTime.wMonth, sysTime.wYear);
			SetWindowText(GetDlgItem(hDlg, IDC_EDITPATH), cCrashFileName);
			//Open File For Writing
			outHand = CreateFile(cCrashFileName, GENERIC_READ + GENERIC_WRITE, FILE_SHARE_READ + FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			SetFilePointer(outHand, 0, 0, FILE_END);
			WriteFile(outHand, G_cCrashTxt, strlen(G_cCrashTxt), &written, NULL);
			for (int i = 0; i < 80; i++)
				WriteFile(outHand, &cDash, 1, &written, NULL);
			WriteFile(outHand, cLF, 2, &written, NULL);
			WriteFile(outHand, cLF, 2, &written, NULL);
			CloseHandle(outHand);
			break;

	}
	return 0;
}
LONG lpTopLevelExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo) {

	//Shutdown everything
	/ *G_bIsThread = FALSE;
	_StopTimer(G_mmTimer);

	try {
		delete G_pGame;
		G_pGame = NULL;
	}
	catch (...) {
	}
	* /
	// Show Dialog* /
	//DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), NULL, (DLGPROC)lpCrashDialogFunc);
	PutLogList("Excepcion - Puta Madre!");
	return EXCEPTION_CONTINUE_EXECUTION;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	/ *SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)lpTopLevelExceptionFilter);* /
	//sprintf( szAppClass, "GameServer%d", hInstance);
	sprintf(szAppClass, "MapServer");
	if (!InitApplication(hInstance))		return (FALSE);
	if (!InitInstance(hInstance, nCmdShow)) return (FALSE);

	Initialize();
	EventLoop();
	return 0;
}

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASS  wc;

	wc.style = (CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS);
	//wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.lpfnWndProc = fnMessageProcessor;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(int);
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(7);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppClass;

	return (RegisterClass(&wc));
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	char cTitle[100];
	SYSTEMTIME SysTime;

	GetLocalTime(&SysTime);
	wsprintf(cTitle, "Helbreath Cursed V%s.%s %d (Executed at: %d %d %d)", DEF_UPPERVERSION, DEF_LOWERVERSION, DEF_BUILDDATE, SysTime.wMonth, SysTime.wDay, SysTime.wHour);

	G_hWnd = CreateWindowEx(0,  // WS_EX_TOPMOST,
		szAppClass,
		cTitle,
		WS_VISIBLE | // so we don't have to call ShowWindow
		//WS_POPUP |   // non-app window
		//WS_CAPTION | // so our menu doesn't look ultra-goofy
		WS_SYSMENU |  // so we get an icon in the tray
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000, //1000, //GetSystemMetrics(SM_CXSCREEN),
		600, //600, //GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!G_hWnd) return (FALSE);

	WNDCLASS Wc;
	strcpy(BCX_ClassName, "ListBox1");
	BCX_ScaleX = 1;
	BCX_ScaleY = 1;
	BCX_hInstance = hInstance;
	Wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//Wc.lpfnWndProc = WndProc;
	Wc.lpfnWndProc = fnMessageProcessor;
	Wc.cbClsExtra = 0;
	Wc.cbWndExtra = 0;
	Wc.hInstance = hInstance;
	Wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	Wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	Wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	Wc.lpszMenuName = NULL;
	Wc.lpszClassName = BCX_ClassName;
	RegisterClass(&Wc);

	List1 = BCX_Listbox("", G_hWnd, 1009, 10, 205, 290, 330); // Server Logs 
	List2 = BCX_Listbox2("", G_hWnd, 1012, 690, 35, 290, 230); // Character Logs
	List3 = BCX_Listbox3("", G_hWnd, 1014, 10, 35, 290, 150); // Configs List
	List4 = BCX_Listbox4("", G_hWnd, 1015, 690, 305, 290, 230); // Error List 
	List5 = BCX_Listbox5("", G_hWnd, 1016, 315, 35, 360, 475); // Chat List
	Edit1 = BCX_Editbox("", G_hWnd, 1018, 315, 514, 295, 20); // Chat List
	Button1 = BCX_Button("", G_hWnd, 1010, 10, 540, 290, 20); // Server Logs Button
	Button2 = BCX_Button2("", G_hWnd, 1013, 690, 268, 290, 20); // Character Logs Button
	Button4 = BCX_Button4("", G_hWnd, 1011, 690, 540, 290, 20); // Error List Button
	Button5 = BCX_Button5("", G_hWnd, 1017, 315, 540, 360, 20); // Chat List Button
	Button6 = BCX_Button6("", G_hWnd, 1019, 617, 514, 60, 22); // Chat List Enviar Button

	ShowWindow(G_hWnd, nCmdShow);
	UpdateWindow(G_hWnd);

	return (TRUE);
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

void Initialize()
{
	char cTitle[100];
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);

	if (_InitWinsock() == FALSE) {
		MessageBox(G_hWnd, "Socket 1.1 not found! Cannot execute program.", "ERROR", MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(0);
		return;
	}

	c_map = new class CMapServer(G_hWnd);
	if (c_map->bInit() == FALSE) {
		PutLogList("(!!!) STOPPED!");
		return;
	}

	wsprintf(cTitle, "MapServer - Helbreath Cursed");
	SetWindowText(G_hWnd, cTitle);
	G_mmTimer = _StartTimer(1); //20//50

	G_pListenSock = new class XSocket(G_hWnd, DEF_SERVERSOCKETBLOCKLIMIT);
	G_pListenSock->bListen(c_map->m_cGameServerAddr, c_map->m_iGameServerPort, WM_USER_ACCEPT);

	G_sLogCounter = 0;
	ZeroMemory(G_cLogBuffer, sizeof(G_cLogBuffer));
	ZeroMemory(G_cLogBuffer2, sizeof(G_cLogBuffer2));
	ZeroMemory(G_cLogBuffer3, sizeof(G_cLogBuffer3));
	ZeroMemory(G_cLogBuffer4, sizeof(G_cLogBuffer4));
	ZeroMemory(G_cLogBuffer5, sizeof(G_cLogBuffer5));
	ZeroMemory(G_cLogBuffer6, sizeof(G_cLogBuffer6));
	ZeroMemory(G_cLogBuffer7, sizeof(G_cLogBuffer7));
	ZeroMemory(G_cLogBuffer8, sizeof(G_cLogBuffer8));
	ZeroMemory(G_cLogBuffer9, sizeof(G_cLogBuffer9));
	ZeroMemory(G_cLogBuffer10, sizeof(G_cLogBuffer10));
	ZeroMemory(G_cLogBuffer11, sizeof(G_cLogBuffer11)); //lalov9 drops
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
	_TermWinsock();


	PostQuitMessage(0);
}

void PutLogList(char * cMsg)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	G_cMsgUpdated = FALSE;
	SendMessage(List1, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)cMsg);
	SendMessage(List1, (UINT)LB_SETCURSEL, ItemCount, 0);
	ItemCount++;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_snprintf(cTempBuffer, sizeof(cTempBuffer) - 1, "%02d:%02d:%02d\t%s\n",
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cMsg);
	cTempBuffer[sizeof(cTempBuffer) - 1] = 0;

	if (strlen(G_cLogBuffer) + strlen(cTempBuffer) < sizeof(G_cLogBuffer))
		strcat(G_cLogBuffer, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\MapServer");

		wsprintf(cTempBuffer, "..\\ServerLogs\\MapServer\\MapServerLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);

		pLogFile = fopen(cTempBuffer, "at");
		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer, sizeof(G_cLogBuffer));
			return;
		}
		fwrite(G_cLogBuffer, 1, strlen(G_cLogBuffer), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;

		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer, sizeof(G_cLogBuffer));

	}
}

void CharacterLogList(char * cMsg)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	G_cMsgUpdated = FALSE;
	SendMessage(List2, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)cMsg);
	SendMessage(List2, (UINT)LB_SETCURSEL, ItemCount2, 0);
	ItemCount2++;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_snprintf(cTempBuffer, sizeof(cTempBuffer) - 1, "%02d:%02d:%02d\t%s\n",
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cMsg);
	cTempBuffer[sizeof(cTempBuffer) - 1] = 0;

	if (strlen(G_cLogBuffer7) + strlen(cTempBuffer) < sizeof(G_cLogBuffer7))
		strcat(G_cLogBuffer7, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\Characters");

		wsprintf(cTempBuffer, "..\\ServerLogs\\Characters\\CharactersLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);
		pLogFile = fopen(cTempBuffer, "at");

		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer7, sizeof(G_cLogBuffer7));
			return;
		}

		fwrite(G_cLogBuffer7, 1, strlen(G_cLogBuffer7), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;
		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer7, sizeof(G_cLogBuffer7));
	}
}

void ConfigList(char * cMsg)
{
	G_cMsgUpdated = FALSE;
	SendMessage(List3, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)cMsg);
	SendMessage(List3, (UINT)LB_SETCURSEL, ItemCount3, 0);
	ItemCount3++;
}

void UpdateConfigList(char * cMsg)
{
	G_cMsgUpdated = FALSE;
	ItemCount3 = ItemCount3 - 4;
	SendMessage(List3, (UINT)LB_DELETESTRING, ItemCount3, 0);
	SendMessage(List3, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)cMsg);
	SendMessage(List3, (UINT)LB_SETCURSEL, ItemCount3, 0);
	ItemCount3 = ItemCount3 + 4;
}

void ErrorList(char * cMsg)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	G_cMsgUpdated = FALSE;
	SendMessage(List4, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)cMsg);
	SendMessage(List4, (UINT)LB_SETCURSEL, ItemCount4, 0);
	ItemCount4++;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_snprintf(cTempBuffer, sizeof(cTempBuffer) - 1, "%02d:%02d:%02d\t%s\n",
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cMsg);
	cTempBuffer[sizeof(cTempBuffer) - 1] = 0;

	if (strlen(G_cLogBuffer9) + strlen(cTempBuffer) < sizeof(G_cLogBuffer9))
		strcat(G_cLogBuffer9, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\MapServer Errors");

		wsprintf(cTempBuffer, "..\\ServerLogs\\MapServer Errors\\MS Errors [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);
		pLogFile = fopen(cTempBuffer, "at");

		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer9, sizeof(G_cLogBuffer9));
			return;
		}

		fwrite(G_cLogBuffer9, 1, strlen(G_cLogBuffer9), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;
		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer9, sizeof(G_cLogBuffer9));
	}
}

void ChatLogs(char * cMsg)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	/ *G_cMsgUpdated = FALSE;
	SendMessage(List5, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)cMsg);
	SendMessage(List5, (UINT)LB_SETCURSEL, ItemCount5, 0);
	ItemCount5++;* /

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_snprintf(cTempBuffer, sizeof(cTempBuffer) - 1, "%02d:%02d:%02d\t%s\n",
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cMsg);
	cTempBuffer[sizeof(cTempBuffer) - 1] = 0;

	if (strlen(G_cLogBuffer3) + strlen(cTempBuffer) < sizeof(G_cLogBuffer3))
		strcat(G_cLogBuffer3, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\Chats");

		wsprintf(cTempBuffer, "..\\ServerLogs\\Chats\\ChatsLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);
		pLogFile = fopen(cTempBuffer, "at");

		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer3, sizeof(G_cLogBuffer3));
			return;
		}

		fwrite(G_cLogBuffer3, 1, strlen(G_cLogBuffer3), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;
		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer3, sizeof(G_cLogBuffer3));
	}
}

void Borrado()
{
	G_cMsgUpdated = TRUE;
	SendMessage(List5, (UINT)LB_RESETCONTENT, 0, 0);
	SendMessage(List5, (UINT)LB_SETCURSEL, ItemCount5, 0);
	ItemCount5 = 0;
	ChatLogs("Limpio!");
}

void UpdateScreen()
{
	if (G_cMsgUpdated == TRUE) {
		InvalidateRect(G_hWnd, NULL, TRUE);
		G_cMsgUpdated = FALSE;
	}
}


void OnPaint()
{
	HDC hdc;
	PAINTSTRUCT ps;
	register short i;
	char * cMsg;

	hdc = BeginPaint(G_hWnd, &ps);

	SetBkMode(hdc, TRANSPARENT);

	for (i = 0; i < 20; i++) {
		cMsg = (char *)(G_cMsgList + i * 120);
		TextOut(hdc, 5, 5 + 350 - i * 16, cMsg, strlen(cMsg));
	}

	if (c_map != NULL)
		c_map->DisplayInfo(hdc);

	EndPaint(G_hWnd, &ps);
}



void  OnKeyUp(WPARAM wParam, LPARAM lParam)
{
}


void OnAccept()
{
	c_map->bAccept(G_pListenSock);
}


void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2)
{
	PostMessage(G_hWnd, WM_USER_TIMERSIGNAL, wID, NULL);
}

MMRESULT _StartTimer(DWORD dwTime)
{
	TIMECAPS caps;
	MMRESULT timerid;

	timeGetDevCaps(&caps, sizeof(caps));
	timeBeginPeriod(caps.wPeriodMin);
	timerid = timeSetEvent(dwTime, 0, _TimerFunc, 0, (UINT)TIME_PERIODIC);

	return timerid;
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

//MAJOR - CLEROTH - 26/03/05
HWND BCX_Listbox(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_TABSTOP;
	if (Exstyle == -1) Exstyle = WS_EX_CLIENTEDGE;
	A = CreateWindowEx(Exstyle, "Listbox", NULL, Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

HWND BCX_Listbox2(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_TABSTOP;
	if (Exstyle == -1) Exstyle = WS_EX_CLIENTEDGE;
	A = CreateWindowEx(Exstyle, "Listbox", NULL, Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

HWND BCX_Listbox3(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_TABSTOP;
	if (Exstyle == -1) Exstyle = WS_EX_CLIENTEDGE;
	A = CreateWindowEx(Exstyle, "Listbox", NULL, Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

HWND BCX_Listbox4(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_TABSTOP;
	if (Exstyle == -1) Exstyle = WS_EX_CLIENTEDGE;
	A = CreateWindowEx(Exstyle, "Listbox", NULL, Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

HWND BCX_Listbox5(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_TABSTOP;
	if (Exstyle == -1) Exstyle = WS_EX_CLIENTEDGE;
	A = CreateWindowEx(Exstyle, "Listbox", NULL, Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

//MAJOR - CLEROTH - 26/03/05
HWND BCX_Button(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_CHILD | WS_VISIBLE | WS_BORDER;
	A = CreateWindowEx(0, "Button", "Limpiar Server Logs", Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}
HWND BCX_Button2(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_CHILD | WS_VISIBLE | WS_BORDER;
	A = CreateWindowEx(0, "Button", "Limpiar Characters Logs", Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

HWND BCX_Button4(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_CHILD | WS_VISIBLE | WS_BORDER;
	A = CreateWindowEx(0, "Button", "Limpiar Logs Errors", Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

HWND BCX_Button5(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_CHILD | WS_VISIBLE | WS_BORDER;
	A = CreateWindowEx(0, "Button", "Limpiar Chat Log", Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

HWND BCX_Button6(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_BORDER;
	A = CreateWindowEx(0, "Button", "Enviar", Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

HWND BCX_Editbox(char* Text, HWND hWnd, int id, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND  A;
	if (!Style) Style = WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_TABSTOP;
	if (Exstyle == -1) Exstyle = WS_EX_CLIENTEDGE;
	A = CreateWindowEx(Exstyle, "EDIT", NULL, Style,
		X*BCX_ScaleX, Y*BCX_ScaleY, W*BCX_ScaleX, H*BCX_ScaleY,
		hWnd, (HMENU)id, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject
	(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

void PutGMLogData(char * cStr)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	wsprintf(cTempBuffer, "%02d:%02d:%02d\t", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cTempBuffer, cStr);
	strcat(cTempBuffer, "\n");

	if (strlen(G_cLogBuffer4) + strlen(cTempBuffer) < sizeof(G_cLogBuffer4))
		strcat(G_cLogBuffer4, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\GM");

		wsprintf(cTempBuffer, "..\\ServerLogs\\GM\\GMLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);
		pLogFile = fopen(cTempBuffer, "at");

		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer4, sizeof(G_cLogBuffer4));
			return;
		}

		fwrite(G_cLogBuffer4, 1, strlen(G_cLogBuffer4), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;
		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer4, sizeof(G_cLogBuffer4));
	}
}

void PutLogTradeFileList(char * cStr)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	wsprintf(cTempBuffer, "%02d:%02d:%02d\t", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cTempBuffer, cStr);
	strcat(cTempBuffer, "\n");

	if (strlen(G_cLogBuffer10) + strlen(cTempBuffer) < sizeof(G_cLogBuffer10))
		strcat(G_cLogBuffer10, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\TradeLogs");

		wsprintf(cTempBuffer, "..\\ServerLogs\\TradeLogs\\LogTrade [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);

		pLogFile = fopen(cTempBuffer, "at");

		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer10, sizeof(G_cLogBuffer10));
			return;
		}

		fwrite(G_cLogBuffer10, 1, strlen(G_cLogBuffer10), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;
		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer10, sizeof(G_cLogBuffer10));
	}
}

void PutLogCoinsFileList(char * cStr)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	wsprintf(cTempBuffer, "%02d:%02d:%02d\t", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cTempBuffer, cStr);
	strcat(cTempBuffer, "\n");

	if (strlen(G_cLogBuffer5) + strlen(cTempBuffer) < sizeof(G_cLogBuffer5))
		strcat(G_cLogBuffer5, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\Coins");

		wsprintf(cTempBuffer, "..\\ServerLogs\\Coins\\CoinsLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);
		pLogFile = fopen(cTempBuffer, "at");

		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer5, sizeof(G_cLogBuffer5));
			return;
		}

		fwrite(G_cLogBuffer5, 1, strlen(G_cLogBuffer5), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;
		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer5, sizeof(G_cLogBuffer5));
	}
}

void PutLogHacksFileList(char * cStr)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	dwTime = timeGetTime();

	G_cMsgUpdated = FALSE;
	SendMessage(List4, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)cStr);
	SendMessage(List4, (UINT)LB_SETCURSEL, ItemCount4, 0);
	ItemCount4++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	wsprintf(cTempBuffer, "%02d:%02d:%02d\t", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cTempBuffer, cStr);
	strcat(cTempBuffer, "\n");

	if (strlen(G_cLogBuffer6) + strlen(cTempBuffer) < sizeof(G_cLogBuffer6))
		strcat(G_cLogBuffer6, cTempBuffer);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_mkdir("..\\ServerLogs\\Hacks");

	wsprintf(cTempBuffer, "..\\ServerLogs\\Hacks\\HacksLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);
	pLogFile = fopen(cTempBuffer, "at");

	if (pLogFile == NULL) {
		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer6, sizeof(G_cLogBuffer6));
		return;
	}

	fwrite(G_cLogBuffer6, 1, strlen(G_cLogBuffer6), pLogFile);
	fclose(pLogFile);
	pLogFile = NULL;
	ZeroMemory(G_cLogBuffer6, sizeof(G_cLogBuffer6));
}

//Agregado LogDrop Lalov9
void PutLogDrops(char * cStr)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;
	/ *G_cMsgUpdated = FALSE;
	SendMessage(List4, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)cStr);
	SendMessage(List4, (UINT)LB_SETCURSEL, ItemCount4, 0);
	ItemCount4++;* /

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	wsprintf(cTempBuffer, "%02d:%02d:%02d\t", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	strcat(cTempBuffer, cStr);
	strcat(cTempBuffer, "\n");

	if (strlen(G_cLogBuffer11) + strlen(cTempBuffer) < sizeof(G_cLogBuffer11))
		strcat(G_cLogBuffer11, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_mkdir("..\\ServerLogs\\Drops");

	wsprintf(cTempBuffer, "..\\ServerLogs\\Drops\\DropsLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);
	pLogFile = fopen(cTempBuffer, "at");

	if (pLogFile == NULL) {
		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer11, sizeof(G_cLogBuffer11));
		return;
	}

	fwrite(G_cLogBuffer11, 1, strlen(G_cLogBuffer11), pLogFile);
	fclose(pLogFile);
	pLogFile = NULL;

	G_sLogCounter = 0;

	ZeroMemory(G_cLogBuffer11, sizeof(G_cLogBuffer11));
	}
}

void PutLogOnlinesFileList(char * cStr)
{
	char cTempBuffer[512];
	FILE * pLogFile;

	_mkdir("..\\ServerLogs\\OnlineUsers");

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	wsprintf(cTempBuffer, "..\\ServerLogs\\OnlineUsers\\OnlineUsers.log");

	pLogFile = fopen(cTempBuffer, "w");

	if (pLogFile == NULL) return;

	fwrite(cStr, 1, strlen(cStr), pLogFile);
	fclose(pLogFile);
	pLogFile = NULL;
}

void PutLogItemsList(char * cMsg)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_snprintf(cTempBuffer, sizeof(cTempBuffer) - 1, "%02d:%02d:%02d\t%s\n",
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cMsg);
	cTempBuffer[sizeof(cTempBuffer) - 1] = 0;

	if (strlen(G_cLogBuffer2) + strlen(cTempBuffer) < sizeof(G_cLogBuffer2))
		strcat(G_cLogBuffer2, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\Items");

		wsprintf(cTempBuffer, "..\\ServerLogs\\Items\\ItemLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);

		pLogFile = fopen(cTempBuffer, "at");
		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer2, sizeof(G_cLogBuffer2));
			return;
		}
		fwrite(G_cLogBuffer2, 1, strlen(G_cLogBuffer2), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;

		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer2, sizeof(G_cLogBuffer2));

	}
}

void PutEkAndPkLogFileList(char * cMsg)
{
	char cTempBuffer[512];
	SYSTEMTIME SysTime;
	DWORD dwTime;
	FILE * pLogFile;

	dwTime = timeGetTime();

	if (G_sLogCounter == 0) G_dwLogTime = dwTime;

	G_sLogCounter++;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
	_snprintf(cTempBuffer, sizeof(cTempBuffer) - 1, "%02d:%02d:%02d\t%s\n",
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond, cMsg);
	cTempBuffer[sizeof(cTempBuffer) - 1] = 0;

	if (strlen(G_cLogBuffer8) + strlen(cTempBuffer) < sizeof(G_cLogBuffer8))
		strcat(G_cLogBuffer8, cTempBuffer);

	if (G_sLogCounter >= 100 || (dwTime - G_dwLogTime > 10 * 1000)) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer));
		_mkdir("..\\ServerLogs\\EksAndPks");

		wsprintf(cTempBuffer, "..\\ServerLogs\\EksAndPks\\EksAndPksLogs [%02d-%02d-%04d].log", SysTime.wDay, SysTime.wMonth, SysTime.wYear);
		pLogFile = fopen(cTempBuffer, "at");

		if (pLogFile == NULL) {
			G_sLogCounter = 0;
			ZeroMemory(G_cLogBuffer8, sizeof(G_cLogBuffer8));
			return;
		}
		fwrite(G_cLogBuffer8, 1, strlen(G_cLogBuffer8), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;

		G_sLogCounter = 0;
		ZeroMemory(G_cLogBuffer8, sizeof(G_cLogBuffer8));

	}
}
*/
