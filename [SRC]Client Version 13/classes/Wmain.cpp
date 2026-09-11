// -------------------------------------------------------------- 
//                      Helbreath Client 						  
//
//                      1998.10 by Soph
//
// --------------------------------------------------------------


#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h> 
#include <winbase.h>
#include <mmsystem.h>
#include <process.h>
#include <DbgHelp.h>     // depuracion: pila de llamadas y volcados
#include <signal.h>
#include "../Ressources/resource.h"
#include "../Headers/XSocket.h"
#include "../Headers/winmain.h"
#include "../Headers/Game.h"
#include "../Headers/Event.h"
#include "../Headers/DebugCliente.h"
#include <fstream>
#include <iostream>
#include <iosfwd>
#include <vector>
#include <sstream>
//added resolution
#include "..\Resolution\Resolution.h"
extern class cResolution * c_reso;
Event g_ev;
using namespace std;

extern "C" __declspec( dllimport) int __FindHackingDll__(char *);

// --------------------------------------------------------------

#define WM_USER_TIMERSIGNAL		WM_USER + 500
#define WM_USER_CALCSOCKETEVENT WM_USER + 600

int				G_iAddTable31[64][510], G_iAddTable63[64][510]; 
int				G_iAddTransTable31[510][64], G_iAddTransTable63[510][64]; 

long    G_lTransG100[64][64], G_lTransRB100[64][64];
long    G_lTransG70[64][64], G_lTransRB70[64][64];
long    G_lTransG50[64][64], G_lTransRB50[64][64];
long    G_lTransG25[64][64], G_lTransRB25[64][64];
long    G_lTransG2[64][64], G_lTransRB2[64][64];

char			szAppClass[32];
HWND			G_hWnd = NULL;
HWND			G_hEditWnd = NULL;
HINSTANCE       G_hInstance = NULL;
MMRESULT		G_mmTimer;
char   G_cSpriteAlphaDegree;
class CGame * G_pGame;
class XSocket * G_pCalcSocket = NULL;
BOOL  G_bIsCalcSocketConnected = TRUE;
DWORD G_dwCalcSocketTime = NULL, G_dwCalcSocketSendTime = NULL;

char G_cCmdLine[256], G_cCmdLineTokenA[120], G_cCmdLineTokenA_Lowercase[120], G_cCmdLineTokenB[120], G_cCmdLineTokenC[120], G_cCmdLineTokenD[120], G_cCmdLineTokenE[120];

HHOOK		g_keyboardHook;


enum
{
	Client,
	Reso,
	Screen,
	Lang
};


bool Init()
{
	LPWSTR *szArglist;
	int nArgs = 0;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	std::string a;
	for (int i = 0; i < nArgs; i++)
	{
		a = (LPCSTR)szArglist[i];
		if (i == Reso)
		{
			if (string((LPCSTR)a.c_str()) == "1")
			{				
				G_pGame->b640x480 = true;
				G_pGame->b800x600 = false;
				G_pGame->b1024x768 = false;
			}
			else if (string((LPCSTR)a.c_str()) == "2")
			{
				G_pGame->b800x600 = true;
				G_pGame->b640x480 = false;
				G_pGame->b1024x768 = false;
			}
			else if (string((LPCSTR)a.c_str()) == "3")
			{
				G_pGame->b800x600 = false;
				G_pGame->b640x480 = false;
				G_pGame->b1024x768 = true;

			}
			else 
			{
				G_pGame->b800x600 = true;
				G_pGame->b640x480 = false;
				G_pGame->b1024x768 = false;			//true	
			}
		}
	}



	G_pGame->b1024x768 = true;
	return true;
}


int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
               LPSTR lpCmdLine, int nCmdShow )
{
	HINSTANCE hDll;
	char cSearchDll[] = "rd`qbg-ckk";
	char cRealName[12];

	// Registro de caidas y congelamientos (carpeta Depuracion junto al .exe)
	DebugCliente_Iniciar();

	srand((unsigned)time(NULL));
	char *pJammer = new char[(rand() % 100) +1];
	G_pGame = new class CGame;
	ZeroMemory(cRealName, sizeof(cRealName));
	strcpy(cRealName, cSearchDll);
	for (WORD i = 0; i < strlen(cRealName); i++)
	if (cRealName[i] != NULL) cRealName[i]++;

	hDll = LoadLibrary(cRealName);
	if( hDll == NULL ) 
	{	MessageBox(NULL, "don't find search.dll", "ERROR!", MB_OK);
		return 0;
	}
	//Launcher
	if (DEF_USING_LAUNCHER && CheckUpdater() != TRUE)
	{
		MessageBox(NULL, "Only excute Helbreath Xsilium Client, in the launcher.", "ERROR!", MB_OK);
		//MessageBox(NULL, "ERROR!", "ERROR!", MB_OK);
		return 0;
	}
	
#ifdef DEF_USING_WIN_IME
	HINSTANCE hRichDll = LoadLibrary( "Riched20.dll" );
#endif

	typedef int (MYPROC)(char *) ;
	MYPROC *pFindHook; 
	pFindHook = (MYPROC *) GetProcAddress(hDll, "__FindHackingDll__") ;

	if (pFindHook== NULL) 
	{	MessageBox(NULL, "can't find search.dll", "ERROR!", MB_OK);
		return 0 ;
	}else if ((*pFindHook)("CRCCHECK") != 1) 
	{	return 0 ;
	}
	FreeLibrary(hDll);

	g_keyboardHook = NULL;
	
	sprintf( szAppClass, "Client-I%d", (int)hInstance);

	HKEY key;
	DWORD dwDisp;
	char myPath[_MAX_PATH+1];
	
	GetModuleFileName(NULL, myPath, _MAX_PATH);

	// Obtener la versión de Windows
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	if (osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion != 1) { // Windows 8 o mayor
	//if (osvi.dwMajorVersion >= 6) { // Windows 8 o mayor
		//lalowin
		RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);
		RegSetValueEx(key, myPath, 0, REG_SZ, (const BYTE*)"$ DWM8And16BitMitigation", sizeof("$ DWM8And16BitMitigation"));
		RegCloseKey(key);
	}
	else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) { // Windows 7
		//lalowin
		RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);
		RegSetValueEx(key, myPath, 0, REG_SZ, (const BYTE*)"16BITCOLOR DWM8And16BitMitigation Layer_ForceDirectDrawEmulation", sizeof("16BITCOLOR DWM8And16BitMitigation Layer_ForceDirectDrawEmulation"));
		RegCloseKey(key);
	}

	if (!InitApplication( hInstance))		return (FALSE);
    if (!InitInstance(hInstance, nCmdShow)) return (FALSE);


	if (Init())
	{
		Initialize((char *)lpCmdLine);
		EventLoop();
		delete[] pJammer;
		delete G_pGame;
#ifdef DEF_USING_WIN_IME
		FreeLibrary(hRichDll);
#endif
	}

	return 0;
}

void OnDestroy()
{	DebugCliente_Cierre("ventana cerrada (WM_DESTROY)");
	G_pGame->m_bIsProgramActive = FALSE;		
	_StopTimer(G_mmTimer);
	G_pGame->Quit();
	WSACleanup();
	SetKeyboardHook(false);
	PostQuitMessage(0);
}

void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2)
{	PostMessage(G_hWnd, WM_USER_TIMERSIGNAL, wID, NULL);
}


MMRESULT _StartTimer(DWORD dwTime)
{TIMECAPS caps;
	timeGetDevCaps(&caps, sizeof(caps));
	timeBeginPeriod(caps.wPeriodMin);
	return timeSetEvent(dwTime,0,_TimerFunc,0, (UINT)TIME_PERIODIC);
}


void _StopTimer(MMRESULT timerid)
{TIMECAPS caps;
	if (timerid != 0) 
	{	timeKillEvent(timerid);
		timerid = 0;
		timeGetDevCaps(&caps, sizeof(caps));
		timeEndPeriod(caps.wPeriodMin);
	}
}



void Initialize(char * pCmdLine)
{int iX, iY, iSum;
 int     iErrCode;
 WORD	 wVersionRequested;
 WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 2 ); 
	iErrCode = WSAStartup( wVersionRequested, &wsaData );
	if ( iErrCode ) 
	{	MessageBox(G_hWnd, "Winsock-V1.1 not found! Cannot execute program.","ERROR",MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(0);
		return;
	}
	if (G_pGame->bInit(G_hWnd, G_hInstance, pCmdLine) == FALSE) 
	{	PostQuitMessage(0);
		return;
	}	
	//G_mmTimer = _StartTimer(1000);
	//velocidad
	G_mmTimer = _StartTimer(200);
	for (iX = 0; iX < 64; iX++)
	for (iY = 0; iY < 510; iY++) 
	{	iSum = iX + (iY - 255);
		if (iSum <= 0)  iSum = 1;
		if (iSum >= 31) iSum = 31;
		G_iAddTable31[iX][iY] = iSum; 
		iSum = iX + (iY - 255);
		if (iSum <= 0)  iSum = 1;
		if (iSum >= 63) iSum = 63;
		G_iAddTable63[iX][iY] = iSum; 
		if ((iY - 255) < iX) G_iAddTransTable31[iY][iX] = iX;
		else if ((iY - 255) > 31) G_iAddTransTable31[iY][iX] = 31;
		else G_iAddTransTable31[iY][iX] = iY-255;
		if ((iY - 255) < iX) G_iAddTransTable63[iY][iX] = iX;
		else if ((iY - 255) > 63) G_iAddTransTable63[iY][iX] = 63;
		else G_iAddTransTable63[iY][iX] = iY-255;
	}
	if(!G_pGame->m_windowsKey) SetKeyboardHook(true);
	G_pGame->InitDialogs();
}

LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{   HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
    if (retval == ERROR_SUCCESS) 
	{	long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        lstrcpy(retdata,data);
        RegCloseKey(hkey);
    }
    return retval;
}

void GoHomepage() 
{	LPCTSTR	url = MSG_HOMEPAGE;

#ifdef _DEBUG
	return;
#endif

	int		showcmd = SW_SHOW;
	char	key[MAX_PATH + MAX_PATH];	
    // First try ShellExecute()
    HINSTANCE result = ShellExecute(NULL, "open", url, NULL,NULL, showcmd);

    // If it failed, get the .htm regkey and lookup the program
    if ((UINT)result <= HINSTANCE_ERROR) 
	{  if (GetRegKey(HKEY_CLASSES_ROOT, ".htm", key) == ERROR_SUCCESS) 
		{  lstrcat(key, "\\shell\\open\\command");
            if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS)
			{   char *pos;
                pos = strstr(key, "\"%1\"");
                if (pos == NULL) {                     // No quotes found
                    pos = strstr(key, "%1");           // Check for %1, without quotes 
                    if (pos == NULL)                   // No parameter at all...
                          pos = key+lstrlen(key)-1;
                    else *pos = '\0';                   // Remove the parameter
                }else    *pos = '\0';                   // Remove the parameter
                lstrcat(pos, " ");
                lstrcat(pos, url);
                result = (HINSTANCE) WinExec(key,showcmd);
    }	}	}
}

BOOL CheckUpdater()
{
	HANDLE hSnapShot;
	PROCESSENTRY32 uProcess;

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    uProcess.dwSize = (DWORD)sizeof(PROCESSENTRY32);
    Process32First(hSnapShot, &uProcess);

	do
    {   
		if(strstr(uProcess.szExeFile, "HB Xsilium - Launcher") != NULL) 
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD) uProcess.th32ProcessID);
            if (hProcess != NULL)
            {
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
			return TRUE;
		}
    }
    while (Process32Next(hSnapShot, &uProcess));
    
	CloseHandle(hSnapShot);
    return FALSE;
}

void SetKeyboardHook(bool enable)
{
	if(enable) g_keyboardHook = SetWindowsHookEx( WH_KEYBOARD_LL, KeyboardProc, G_hInstance, 0);
	else if(g_keyboardHook != NULL)
		UnhookWindowsHookEx(g_keyboardHook);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || nCode != HC_ACTION)
		return CallNextHookEx( g_keyboardHook, nCode, wParam, lParam);

	bool bPreventKeypress = false;
	KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
	switch(wParam)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			bPreventKeypress = (G_pGame->m_bIsProgramActive && ((p->vkCode == VK_LWIN) || (p->vkCode == VK_RWIN)) && !G_pGame->m_windowsKey);
			break;
		}
	}

	if (bPreventKeypress) return 1;
	else return CallNextHookEx( g_keyboardHook, nCode, wParam, lParam);
}

//lalowin
bool IsWin8()//usar windows 10
{
	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);
	if ((osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 2) ||
		(osvi.dwMajorVersion == 10 && osvi.dwMinorVersion >= 0)) {

		return true;
	}
	return false;
}

/*bool IsWin8() { //usar windows 7
	OSVERSIONINFO osvi;
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion >= 2) {
		return true;
	}
	return false;
}*/

LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam, LPARAM lParam)
{ 

	
	if(G_pGame->GetText( hWnd, message, wParam, lParam)) return 0;

	switch (message) {
	case WM_USER_CALCSOCKETEVENT:
		G_pGame->_CalcSocketClosed();
		break;
	
	case WM_CLOSE:
		if ( (G_pGame->m_cGameMode == DEF_GAMEMODE_ONMAINGAME) && ( G_pGame->m_bForceDisconn == FALSE ) )
		{

#ifdef _DEBUGS
			if (G_pGame->m_cSalirDelChar == -1 || G_pGame->m_cSalirDelChar > 2) G_pGame->m_cSalirDelChar = 1;
#else
			if (G_pGame->m_cSalirDelChar == -1 || G_pGame->m_cSalirDelChar > 11) G_pGame->m_cSalirDelChar = 11;
#endif

		}
			else if (G_pGame->m_cGameMode == DEF_GAMEMODE_ONLOADING) return (DefWindowProc(hWnd, message, wParam, lParam));
			else if (G_pGame->m_cGameMode == DEF_GAMEMODE_ONMAINMENU) G_pGame->ChangeGameMode(DEF_GAMEMODE_ONQUIT);
		break;
	
	case WM_SYSCOMMAND:
		if((wParam&0xFFF0)==SC_SCREENSAVE || (wParam&0xFFF0)==SC_MONITORPOWER) 
			return 0; 
		return DefWindowProc(hWnd, message, wParam, lParam);
			
	case WM_USER_TIMERSIGNAL:
		DebugCliente_Latido();   // llega cada 200 ms aunque haya un MessageBox abierto
		G_pGame->OnTimer();
		break;

	case WM_KEYDOWN:
		G_pGame->OnKeyDown(wParam);
		return (DefWindowProc(hWnd, message, wParam, lParam));
		
	case WM_KEYUP:
		G_pGame->OnKeyUp(wParam);
		return (DefWindowProc(hWnd, message, wParam, lParam));

	case WM_SYSKEYDOWN:
		G_pGame->OnSysKeyDown(wParam);
		return (DefWindowProc(hWnd, message, wParam, lParam));
		break;

	case WM_SYSKEYUP:
		G_pGame->OnSysKeyUp(wParam);
		return (DefWindowProc(hWnd, message, wParam, lParam));
		break;

	case WM_ACTIVATEAPP:
		if( wParam == 0 ) 
		{	G_pGame->m_bIsProgramActive = FALSE;
			G_pGame->m_DInput.SetAcquire(FALSE);
		}else 
		{	G_pGame->m_bIsProgramActive = TRUE;
			G_pGame->m_DInput.SetAcquire(TRUE);
			G_pGame->m_bCtrlPressed = FALSE;
			if (IsWin8())
			{
				G_pGame->m_bIsRedrawPDBGS = TRUE;
				G_pGame->m_DDraw.ChangeDisplayMode(G_hWnd);

			}
		if (G_pGame->bCheckImportantFile() == FALSE) 
			{	MessageBox(G_pGame->m_hWnd, "File checksum error! Get Update again please!", "ERROR1", MB_ICONEXCLAMATION | MB_OK);
				PostQuitMessage(0);
				return 0;
			}

			if (__FindHackingDll__("CRCCHECK") != 1) 
			{	G_pGame->ChangeGameMode(DEF_GAMEMODE_ONQUIT);
				return NULL;
			}
			
		}
		return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_SETCURSOR:
		SetCursor(NULL);
		return TRUE;

	case WM_DESTROY:
		OnDestroy();
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
		
	case WM_USER_GAMESOCKETEVENT:
		G_pGame->OnGameSocketEvent(wParam, lParam);
		break;

	case WM_USER_LOGSOCKETEVENT:
		G_pGame->OnLogSocketEvent(wParam, lParam);
		break;

	default: 
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}	
	return NULL;
}

BOOL InitApplication( HINSTANCE hInstance)
{WNDCLASS  wc;
	wc.style = (CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS);
	wc.lpfnWndProc   = (WNDPROC)WndProc;             
	wc.cbClsExtra    = 0;                            
	wc.cbWndExtra    = sizeof (int);
	wc.hInstance     = hInstance;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = szAppClass;        
	return (RegisterClass(&wc));
	
}


BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{	
	int cx = GetSystemMetrics(SM_CXFULLSCREEN)/2;
	int cy = GetSystemMetrics(SM_CYFULLSCREEN)/2;
	//if (cy>280) cy -= 40;

	if (c_reso->IsResolution() == C800x600)
	{
	if (cy>340) cy -= 40;
	G_hWnd = CreateWindowEx(WS_EX_TOPMOST, szAppClass, "Xsilium", WS_VISIBLE | WS_POPUP, cx - 400, cy - 300,
	800, 600, NULL, NULL, hInstance, NULL);
	}
	else if (c_reso->IsResolution() == C640x480)
	{
	if (cy>280) cy -= 40;
	G_hWnd = CreateWindowEx(WS_EX_TOPMOST, szAppClass, "Xsilium", WS_VISIBLE | WS_POPUP, cx - 320, cy - 240,
	640, 480, NULL, NULL, hInstance, NULL);
	}
	else if (c_reso->IsResolution() == C1024x768)
	{
	if (cy>424) cy -= 40;
	G_hWnd = CreateWindowEx(WS_EX_TOPMOST, szAppClass, "Xsilium", WS_VISIBLE | WS_POPUP, cx - 512, cy - 384,
	1024, 768, NULL, NULL, hInstance, NULL);
	}

    if (!G_hWnd) return FALSE;
    G_hInstance	= hInstance;
	ShowWindow(G_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(G_hWnd);
	return TRUE;
}


void EventLoop()
{ register MSG msg;
	while( 1 ) 
	{	DebugCliente_Latido();
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
		{	if( !GetMessage( &msg, NULL, 0, 0 ) ) return;// msg.wParam;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
		}
		else if (G_pGame->m_bIsProgramActive) G_pGame->UpdateScreen();
		else if (G_pGame->m_cGameMode == DEF_GAMEMODE_ONLOADING) G_pGame->UpdateScreen_OnLoading( FALSE );
		else WaitMessage();
		
	}
}


// ===========================================================================
// Depuracion del cliente (ver Headers/DebugCliente.h)
//
// Sin dependencias nuevas en el proyecto: dbghelp.dll se carga en tiempo de
// ejecucion. Con el .pdb junto al .exe (Archive\Helbreath Xsilium.pdb) los
// reportes salen con nombre de funcion, archivo y linea.
// ===========================================================================

#define DBGC_MAXEVENTOS     128
#define DBGC_MAXFRAMES      48
#define DBGC_CONGELADO_MS   3000

struct stDbgEvento {
	DWORD dwTiempo;
	int   iTipo;
	DWORD a, b, c;
	char  cTexto[64];
};

typedef BOOL  (WINAPI * tDbgSymInitialize)(HANDLE, PCSTR, BOOL);
typedef DWORD (WINAPI * tDbgSymSetOptions)(DWORD);
typedef BOOL  (WINAPI * tDbgSymFromAddr)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);
typedef BOOL  (WINAPI * tDbgSymGetLineFromAddr64)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
typedef BOOL  (WINAPI * tDbgSymRefreshModuleList)(HANDLE);
typedef BOOL  (WINAPI * tDbgStackWalk64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64,
	PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);
typedef BOOL  (WINAPI * tDbgMiniDumpWriteDump)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION,
	PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);

static stDbgEvento    G_stDbgEventos[DBGC_MAXEVENTOS];
static volatile LONG  G_lDbgIndice = 0;
static volatile DWORD G_dwDbgLatido = 0;
static volatile LONG  G_lDbgEnCrash = 0;
static volatile LONG  G_lDbgExcepciones = 0;
static BOOL           G_bDbgIniciado = FALSE;
static volatile BOOL  G_bDbgCerrado = FALSE;     // tras un cierre normal el vigia deja de escribir
static HANDLE         G_hDbgHiloPrincipal = NULL;
static DWORD          G_dwDbgIdHiloPrincipal = 0;
static char           G_cDbgCarpeta[MAX_PATH];
static char           G_cDbgExe[MAX_PATH];
static CRITICAL_SECTION G_csDbg;

static HMODULE                    G_hDbgHelp = NULL;
static BOOL                       G_bDbgSimbolos = FALSE;
static tDbgSymFromAddr            G_pfDbgSymFromAddr = NULL;
static tDbgSymGetLineFromAddr64   G_pfDbgSymGetLine = NULL;
static tDbgSymRefreshModuleList   G_pfDbgSymRefresh = NULL;
static tDbgStackWalk64            G_pfDbgStackWalk = NULL;
static PFUNCTION_TABLE_ACCESS_ROUTINE64 G_pfDbgFuncTable = NULL;
static PGET_MODULE_BASE_ROUTINE64 G_pfDbgModuleBase = NULL;
static tDbgMiniDumpWriteDump      G_pfDbgMiniDump = NULL;

// --- utilidades de escritura (sin memoria dinamica, sirven dentro de un crash) ---

static void _DbgPrintf(HANDLE hFile, const char * cFmt, ...)
{
	char cBuf[1024];
	DWORD dwEscrito;
	va_list args;
	int iLen;

	if (hFile == INVALID_HANDLE_VALUE) return;
	va_start(args, cFmt);
	iLen = _vsnprintf(cBuf, sizeof(cBuf) - 1, cFmt, args);
	va_end(args);
	if (iLen < 0) iLen = sizeof(cBuf) - 1;
	cBuf[iLen] = 0;
	WriteFile(hFile, cBuf, (DWORD)strlen(cBuf), &dwEscrito, NULL);
}

static void _DbgNombreArchivo(char * cDest, int iSize, const char * cPrefijo, const char * cExt)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	_snprintf(cDest, iSize - 1, "%s\\%s %04d-%02d-%02d %02d-%02d-%02d.%s", G_cDbgCarpeta, cPrefijo,
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, cExt);
	cDest[iSize - 1] = 0;
}

static int _DbgLeerModo()
{
	int iModo = -1;
	__try {
		if (G_pGame != NULL) iModo = (int)(unsigned char)G_pGame->m_cGameMode;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		iModo = -2;
	}
	return iModo;
}

static const char * _DbgNombreExcepcion(DWORD dwCodigo)
{
	switch (dwCodigo) {
	case EXCEPTION_ACCESS_VIOLATION:      return "ACCESS_VIOLATION (puntero invalido)";
	case EXCEPTION_STACK_OVERFLOW:        return "STACK_OVERFLOW (recursion infinita o array enorme en la pila)";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:    return "INT_DIVIDE_BY_ZERO (division entre cero)";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "ARRAY_BOUNDS_EXCEEDED";
	case EXCEPTION_ILLEGAL_INSTRUCTION:   return "ILLEGAL_INSTRUCTION";
	case EXCEPTION_PRIV_INSTRUCTION:      return "PRIV_INSTRUCTION";
	case EXCEPTION_IN_PAGE_ERROR:         return "IN_PAGE_ERROR";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:    return "FLT_DIVIDE_BY_ZERO";
	case 0xC0000374:                      return "HEAP_CORRUPTION (memoria corrompida)";
	case 0xC0000409:                      return "STACK_BUFFER_OVERRUN (buffer local desbordado)";
	case 0xE06D7363:                      return "excepcion C++ no atrapada";
	case 0xE0584331:                      return "reporte manual del cliente";
	}
	return "desconocida";
}

static void _DbgModuloDeDireccion(DWORD64 dwDir, char * cModulo, int iSize, DWORD64 * pOffset)
{
	HMODULE hMod = NULL;
	char cRuta[MAX_PATH];
	char * cp;

	strcpy(cModulo, "?");
	*pOffset = dwDir;
	if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)(ULONG_PTR)dwDir, &hMod) && (hMod != NULL)) {
		ZeroMemory(cRuta, sizeof(cRuta));
		GetModuleFileNameA(hMod, cRuta, sizeof(cRuta) - 1);
		cp = strrchr(cRuta, '\\');
		strncpy(cModulo, (cp != NULL) ? cp + 1 : cRuta, iSize - 1);
		cModulo[iSize - 1] = 0;
		*pOffset = dwDir - (DWORD64)(ULONG_PTR)hMod;
	}
}

static void _DbgCargarDbgHelp()
{
	char cRutaExe[MAX_PATH];
	char * cp;
	tDbgSymInitialize pfInit;
	tDbgSymSetOptions pfOpt;

	G_hDbgHelp = LoadLibraryA("dbghelp.dll");
	if (G_hDbgHelp == NULL) return;

	pfInit = (tDbgSymInitialize)GetProcAddress(G_hDbgHelp, "SymInitialize");
	pfOpt = (tDbgSymSetOptions)GetProcAddress(G_hDbgHelp, "SymSetOptions");
	G_pfDbgSymFromAddr = (tDbgSymFromAddr)GetProcAddress(G_hDbgHelp, "SymFromAddr");
	G_pfDbgSymGetLine = (tDbgSymGetLineFromAddr64)GetProcAddress(G_hDbgHelp, "SymGetLineFromAddr64");
	G_pfDbgSymRefresh = (tDbgSymRefreshModuleList)GetProcAddress(G_hDbgHelp, "SymRefreshModuleList");
	G_pfDbgStackWalk = (tDbgStackWalk64)GetProcAddress(G_hDbgHelp, "StackWalk64");
	G_pfDbgFuncTable = (PFUNCTION_TABLE_ACCESS_ROUTINE64)GetProcAddress(G_hDbgHelp, "SymFunctionTableAccess64");
	G_pfDbgModuleBase = (PGET_MODULE_BASE_ROUTINE64)GetProcAddress(G_hDbgHelp, "SymGetModuleBase64");
	G_pfDbgMiniDump = (tDbgMiniDumpWriteDump)GetProcAddress(G_hDbgHelp, "MiniDumpWriteDump");

	if ((pfInit == NULL) || (pfOpt == NULL)) return;

	// Los simbolos (.pdb) se buscan junto al .exe
	strcpy(cRutaExe, G_cDbgExe);
	cp = strrchr(cRutaExe, '\\');
	if (cp != NULL) *cp = 0;

	pfOpt(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_FAIL_CRITICAL_ERRORS);
	G_bDbgSimbolos = pfInit(GetCurrentProcess(), cRutaExe, TRUE);
}

// --- pila de llamadas ---

static void _DbgEscribirPila(HANDLE hFile, HANDLE hHilo, CONTEXT * pCtx)
{
	STACKFRAME64 sf;
	CONTEXT ctx;
	DWORD dwMaquina;
	int i;
	char cSymBuf[sizeof(SYMBOL_INFO) + 256];
	PSYMBOL_INFO pSym = (PSYMBOL_INFO)cSymBuf;
	IMAGEHLP_LINE64 linea;
	DWORD64 dwDesp64, dwOffset;
	DWORD dwDesp;
	char cModulo[64];

	_DbgPrintf(hFile, "\r\n--- Pila de llamadas (lo de arriba es donde fallo) ---\r\n");

	if ((pCtx == NULL) || (G_pfDbgStackWalk == NULL) || (G_pfDbgFuncTable == NULL) || (G_pfDbgModuleBase == NULL)) {
		_DbgPrintf(hFile, "(no disponible: falta dbghelp.dll)\r\n");
		return;
	}
	if (G_pfDbgSymRefresh != NULL) G_pfDbgSymRefresh(GetCurrentProcess());

	memcpy(&ctx, pCtx, sizeof(CONTEXT));
	ZeroMemory(&sf, sizeof(sf));
#if defined(_M_X64)
	dwMaquina = IMAGE_FILE_MACHINE_AMD64;
	sf.AddrPC.Offset = ctx.Rip;
	sf.AddrFrame.Offset = ctx.Rbp;
	sf.AddrStack.Offset = ctx.Rsp;
#else
	dwMaquina = IMAGE_FILE_MACHINE_I386;
	sf.AddrPC.Offset = ctx.Eip;
	sf.AddrFrame.Offset = ctx.Ebp;
	sf.AddrStack.Offset = ctx.Esp;
#endif
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;

	for (i = 0; i < DBGC_MAXFRAMES; i++) {
		if (!G_pfDbgStackWalk(dwMaquina, GetCurrentProcess(), hHilo, &sf, &ctx, NULL, G_pfDbgFuncTable, G_pfDbgModuleBase, NULL)) break;
		if (sf.AddrPC.Offset == 0) break;

		_DbgModuloDeDireccion(sf.AddrPC.Offset, cModulo, sizeof(cModulo), &dwOffset);
		_DbgPrintf(hFile, "%2d  %s+0x%I64X", i, cModulo, dwOffset);

		if (G_bDbgSimbolos && (G_pfDbgSymFromAddr != NULL)) {
			ZeroMemory(cSymBuf, sizeof(cSymBuf));
			pSym->SizeOfStruct = sizeof(SYMBOL_INFO);
			pSym->MaxNameLen = 255;
			dwDesp64 = 0;
			if (G_pfDbgSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &dwDesp64, pSym))
				_DbgPrintf(hFile, "  %s+%I64u", pSym->Name, dwDesp64);

			if (G_pfDbgSymGetLine != NULL) {
				ZeroMemory(&linea, sizeof(linea));
				linea.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
				dwDesp = 0;
				if (G_pfDbgSymGetLine(GetCurrentProcess(), sf.AddrPC.Offset, &dwDesp, &linea))
					_DbgPrintf(hFile, "  [%s : linea %lu]", linea.FileName, linea.LineNumber);
			}
		}
		_DbgPrintf(hFile, "\r\n");
	}
}

// --- ultimos eventos ---

static void _DbgEscribirEventos(HANDLE hFile)
{
	LONG lTotal, lInicio, l;
	DWORD dwAhora;
	stDbgEvento ev;

	dwAhora = GetTickCount();
	lTotal = G_lDbgIndice;
	lInicio = (lTotal > DBGC_MAXEVENTOS) ? (lTotal - DBGC_MAXEVENTOS) : 0;

	_DbgPrintf(hFile, "\r\n--- Ultimos %ld eventos (el ultimo es lo mas reciente; ms antes del reporte) ---\r\n", lTotal - lInicio);
	for (l = lInicio; l < lTotal; l++) {
		memcpy(&ev, &G_stDbgEventos[(DWORD)l % DBGC_MAXEVENTOS], sizeof(ev));
		ev.cTexto[sizeof(ev.cTexto) - 1] = 0;
		switch (ev.iTipo) {
		case DBGC_RECV:
			_DbgPrintf(hFile, "[-%6lu ms] RECIBIDO  msg 0x%08lX  tipo 0x%04lX  tamano %lu\r\n", dwAhora - ev.dwTiempo, ev.a, ev.c, ev.b);
			break;
		case DBGC_SEND:
			_DbgPrintf(hFile, "[-%6lu ms] ENVIADO   msg 0x%08lX  comando %lu  valor %ld\r\n", dwAhora - ev.dwTiempo, ev.a, ev.b, (long)ev.c);
			break;
		case DBGC_MODO:
			_DbgPrintf(hFile, "[-%6lu ms] MODO      %lu -> %lu\r\n", dwAhora - ev.dwTiempo, ev.b, ev.a);
			break;
		default:
			_DbgPrintf(hFile, "[-%6lu ms] %s\r\n", dwAhora - ev.dwTiempo, ev.cTexto);
			break;
		}
	}
}

// --- reporte completo ---

static void _DbgEscribirReporte(const char * cTipo, EXCEPTION_POINTERS * pEx, HANDLE hHilo, DWORD dwIdHilo, CONTEXT * pCtx, BOOL bDump, const char * cMotivo)
{
	char cTxt[MAX_PATH], cDmp[MAX_PATH], cModulo[64];
	HANDLE hFile, hDump;
	SYSTEMTIME st;
	DWORD64 dwOffset;
	EXCEPTION_RECORD * pRec;
	MINIDUMP_EXCEPTION_INFORMATION mei;

	EnterCriticalSection(&G_csDbg);

	_DbgNombreArchivo(cTxt, sizeof(cTxt), cTipo, "txt");
	hFile = CreateFileA(cTxt, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	GetLocalTime(&st);
	_DbgPrintf(hFile, "Helbreath Xsilium - reporte de %s\r\n", cTipo);
	_DbgPrintf(hFile, "Fecha: %04d-%02d-%02d %02d:%02d:%02d\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	_DbgPrintf(hFile, "Ejecutable: %s\r\n", G_cDbgExe);
	_DbgPrintf(hFile, "Modo de juego: %d   Hilo: %lu (principal: %lu)\r\n", _DbgLeerModo(), dwIdHilo, G_dwDbgIdHiloPrincipal);
	_DbgPrintf(hFile, "Simbolos (.pdb): %s\r\n", G_bDbgSimbolos ? "cargados" : "NO disponibles");
	if (cMotivo != NULL) _DbgPrintf(hFile, "Motivo: %s\r\n", cMotivo);

	if ((pEx != NULL) && (pEx->ExceptionRecord != NULL)) {
		pRec = pEx->ExceptionRecord;
		_DbgModuloDeDireccion((DWORD64)(ULONG_PTR)pRec->ExceptionAddress, cModulo, sizeof(cModulo), &dwOffset);
		_DbgPrintf(hFile, "\r\nExcepcion: 0x%08lX  %s\r\n", pRec->ExceptionCode, _DbgNombreExcepcion(pRec->ExceptionCode));
		_DbgPrintf(hFile, "Direccion: 0x%p  (%s+0x%I64X)\r\n", pRec->ExceptionAddress, cModulo, dwOffset);
		if ((pRec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) && (pRec->NumberParameters >= 2)) {
			_DbgPrintf(hFile, "Intento de %s en la direccion 0x%p%s\r\n",
				(pRec->ExceptionInformation[0] == 0) ? "LEER" : ((pRec->ExceptionInformation[0] == 1) ? "ESCRIBIR" : "EJECUTAR"),
				(void *)pRec->ExceptionInformation[1],
				(pRec->ExceptionInformation[1] < 0x10000) ? "  <- puntero NULL o casi NULL" : "");
		}
	}

	if (pCtx != NULL) {
#if defined(_M_X64)
		_DbgPrintf(hFile, "\r\nRegistros: RIP=%016I64X RSP=%016I64X RBP=%016I64X\r\n           RAX=%016I64X RBX=%016I64X RCX=%016I64X RDX=%016I64X\r\n",
			pCtx->Rip, pCtx->Rsp, pCtx->Rbp, pCtx->Rax, pCtx->Rbx, pCtx->Rcx, pCtx->Rdx);
#else
		_DbgPrintf(hFile, "\r\nRegistros: EIP=%08lX ESP=%08lX EBP=%08lX\r\n           EAX=%08lX EBX=%08lX ECX=%08lX EDX=%08lX ESI=%08lX EDI=%08lX\r\n",
			pCtx->Eip, pCtx->Esp, pCtx->Ebp, pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx, pCtx->Esi, pCtx->Edi);
#endif
	}

	_DbgEscribirPila(hFile, hHilo, pCtx);
	_DbgEscribirEventos(hFile);

	// Volcado de memoria: se abre con Visual Studio (Archivo > Abrir) y muestra la linea exacta
	if (bDump && (G_pfDbgMiniDump != NULL)) {
		_DbgNombreArchivo(cDmp, sizeof(cDmp), cTipo, "dmp");
		hDump = CreateFileA(cDmp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hDump != INVALID_HANDLE_VALUE) {
			mei.ThreadId = dwIdHilo;
			mei.ExceptionPointers = pEx;
			mei.ClientPointers = FALSE;
			G_pfDbgMiniDump(GetCurrentProcess(), GetCurrentProcessId(), hDump,
				(MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory),
				(pEx != NULL) ? &mei : NULL, NULL, NULL);
			CloseHandle(hDump);
			_DbgPrintf(hFile, "\r\nVolcado de memoria: %s\r\n", cDmp);
		}
	}

	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	LeaveCriticalSection(&G_csDbg);
}

// UltimosEventos.txt: se reescribe cada 2 segundos. Si el proceso desaparece sin
// dejar reporte, este archivo queda con "EN EJECUCION" y lo ultimo que paso.
static void _DbgEscribirUltimos(const char * cEstado)
{
	char cRuta[MAX_PATH];
	HANDLE hFile;
	SYSTEMTIME st;

	EnterCriticalSection(&G_csDbg);
	_snprintf(cRuta, sizeof(cRuta) - 1, "%s\\UltimosEventos.txt", G_cDbgCarpeta);
	cRuta[sizeof(cRuta) - 1] = 0;
	hFile = CreateFileA(cRuta, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		GetLocalTime(&st);
		_DbgPrintf(hFile, "Estado: %s\r\n", cEstado);
		_DbgPrintf(hFile, "Actualizado: %04d-%02d-%02d %02d:%02d:%02d   Modo de juego: %d\r\n", st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond, _DbgLeerModo());
		_DbgEscribirEventos(hFile);
		CloseHandle(hFile);
	}
	LeaveCriticalSection(&G_csDbg);
}

// --- manejadores ---

struct stDbgArgsCrash {
	EXCEPTION_POINTERS * pEx;
	HANDLE hHilo;
	DWORD  dwIdHilo;
	const char * cMotivo;
};

// El reporte se escribe desde otro hilo: si la caida fue por falta de pila
// (STACK_OVERFLOW) el hilo que fallo ya no tiene espacio para nada.
static DWORD WINAPI _DbgHiloCrash(LPVOID pParam)
{
	stDbgArgsCrash * pArgs = (stDbgArgsCrash *)pParam;
	_DbgEscribirReporte("Crash", pArgs->pEx, pArgs->hHilo, pArgs->dwIdHilo,
		(pArgs->pEx != NULL) ? pArgs->pEx->ContextRecord : NULL, TRUE, pArgs->cMotivo);
	_DbgEscribirUltimos("CRASH (ver el archivo Crash de la misma hora)");
	return 0;
}

static void _DbgReportarCrash(EXCEPTION_POINTERS * pEx, const char * cMotivo)
{
	stDbgArgsCrash args;
	HANDLE hThread;

	if (InterlockedExchange(&G_lDbgEnCrash, 1) != 0) return;   // ya hay uno en curso

	args.pEx = pEx;
	args.dwIdHilo = GetCurrentThreadId();
	args.cMotivo = cMotivo;
	args.hHilo = NULL;
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &args.hHilo, 0, FALSE, DUPLICATE_SAME_ACCESS);

	hThread = CreateThread(NULL, 256 * 1024, _DbgHiloCrash, &args, 0, NULL);
	if (hThread != NULL) {
		WaitForSingleObject(hThread, 30000);
		CloseHandle(hThread);
	}
	else _DbgHiloCrash(&args);

	if (args.hHilo != NULL) CloseHandle(args.hHilo);
}

static LONG WINAPI _DbgFiltroCrash(EXCEPTION_POINTERS * pEx)
{
	_DbgReportarCrash(pEx, NULL);
	return EXCEPTION_EXECUTE_HANDLER;   // el proceso termina
}

// Errores del runtime de C que no pasan por el filtro de arriba
static void _DbgReporteManual(const char * cMotivo)
{
	EXCEPTION_RECORD rec;
	CONTEXT ctx;
	EXCEPTION_POINTERS ep;

	ZeroMemory(&rec, sizeof(rec));
	ZeroMemory(&ctx, sizeof(ctx));
	ctx.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext(&ctx);
	rec.ExceptionCode = 0xE0584331;
#if defined(_M_X64)
	rec.ExceptionAddress = (PVOID)ctx.Rip;
#else
	rec.ExceptionAddress = (PVOID)ctx.Eip;
#endif
	ep.ExceptionRecord = &rec;
	ep.ContextRecord = &ctx;
	_DbgReportarCrash(&ep, cMotivo);
	TerminateProcess(GetCurrentProcess(), 3);
}

static void _DbgParametroInvalido(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t)
{
	_DbgReporteManual("Parametro invalido en una funcion del runtime de C (strcpy, sprintf, fopen...)");
}

static void _DbgLlamadaPuraVirtual()
{
	_DbgReporteManual("Llamada a funcion virtual pura (objeto ya destruido)");
}

static void _DbgSenalAbort(int)
{
	_DbgReporteManual("abort() llamado");
}

// Excepciones graves en su primera oportunidad: se anotan aunque luego alguien
// las atrape, porque un catch(...) puede esconder el fallo real.
static LONG WINAPI _DbgVectorExcepcion(EXCEPTION_POINTERS * pEx)
{
	char cRuta[MAX_PATH], cModulo[64];
	DWORD dwCodigo;
	DWORD64 dwOffset;
	HANDLE hFile;
	SYSTEMTIME st;

	if ((pEx == NULL) || (pEx->ExceptionRecord == NULL)) return EXCEPTION_CONTINUE_SEARCH;
	dwCodigo = pEx->ExceptionRecord->ExceptionCode;

	if ((dwCodigo != EXCEPTION_ACCESS_VIOLATION) && (dwCodigo != EXCEPTION_INT_DIVIDE_BY_ZERO) &&
		(dwCodigo != EXCEPTION_ILLEGAL_INSTRUCTION) && (dwCodigo != EXCEPTION_PRIV_INSTRUCTION) &&
		(dwCodigo != EXCEPTION_ARRAY_BOUNDS_EXCEEDED) && (dwCodigo != 0xC0000374)) return EXCEPTION_CONTINUE_SEARCH;

	if (InterlockedIncrement(&G_lDbgExcepciones) > 30) return EXCEPTION_CONTINUE_SEARCH;   // no llenar el disco

	_DbgModuloDeDireccion((DWORD64)(ULONG_PTR)pEx->ExceptionRecord->ExceptionAddress, cModulo, sizeof(cModulo), &dwOffset);
	DebugCliente_Texto("excepcion grave (ver Excepciones.txt)");

	_snprintf(cRuta, sizeof(cRuta) - 1, "%s\\Excepciones.txt", G_cDbgCarpeta);
	cRuta[sizeof(cRuta) - 1] = 0;
	hFile = CreateFileA(cRuta, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		GetLocalTime(&st);
		_DbgPrintf(hFile, "%04d-%02d-%02d %02d:%02d:%02d  0x%08lX %s en %s+0x%I64X  hilo %lu  modo %d\r\n",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, dwCodigo, _DbgNombreExcepcion(dwCodigo),
			cModulo, dwOffset, GetCurrentThreadId(), _DbgLeerModo());
		CloseHandle(hFile);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

// Vigia: detecta cuando el bucle principal deja de latir (congelamiento) y
// mantiene UltimosEventos.txt al dia.
static DWORD WINAPI _DbgHiloVigia(LPVOID)
{
	DWORD dwAhora, dwLatido, dwUltimoVolcado = 0;
	BOOL bCongelado = FALSE;
	int iCongelados = 0;
	CONTEXT ctx;
	char cMotivo[160];

	while (TRUE) {
		Sleep(500);
		if ((G_lDbgEnCrash != 0) || G_bDbgCerrado) continue;

		dwAhora = GetTickCount();
		dwLatido = G_dwDbgLatido;

		if ((dwLatido != 0) && ((dwAhora - dwLatido) > DBGC_CONGELADO_MS)) {
			if ((bCongelado == FALSE) && (iCongelados < 3)) {
				bCongelado = TRUE;
				iCongelados++;
				ZeroMemory(&ctx, sizeof(ctx));
				ctx.ContextFlags = CONTEXT_FULL;
				// Se congela el hilo principal solo el tiempo de leer donde esta
				if (SuspendThread(G_hDbgHiloPrincipal) != (DWORD)-1) {
					GetThreadContext(G_hDbgHiloPrincipal, &ctx);
					ResumeThread(G_hDbgHiloPrincipal);
					_snprintf(cMotivo, sizeof(cMotivo) - 1, "El bucle principal no responde desde hace %lu ms. La pila muestra donde estaba atascado.", dwAhora - dwLatido);
					cMotivo[sizeof(cMotivo) - 1] = 0;
					_DbgEscribirReporte("Congelado", NULL, G_hDbgHiloPrincipal, G_dwDbgIdHiloPrincipal, &ctx, (iCongelados == 1), cMotivo);
				}
			}
		}
		else if (bCongelado) {
			bCongelado = FALSE;
			_snprintf(cMotivo, sizeof(cMotivo) - 1, "el bucle principal volvio a responder");
			cMotivo[sizeof(cMotivo) - 1] = 0;
			DebugCliente_Texto(cMotivo);
		}

		if ((dwAhora - dwUltimoVolcado) > 2000) {
			dwUltimoVolcado = dwAhora;
			_DbgEscribirUltimos(bCongelado ? "EN EJECUCION - CONGELADO AHORA MISMO" : "EN EJECUCION");
		}
	}
	return 0;
}

// --- API publica ---

void DebugCliente_Iniciar()
{
	char cRuta[MAX_PATH], cNueva[MAX_PATH], cLinea[64];
	char * cp;
	HANDLE hFile;
	DWORD dwLeido;

	if (G_bDbgIniciado) return;
	G_bDbgIniciado = TRUE;
	InitializeCriticalSection(&G_csDbg);

	ZeroMemory(G_cDbgExe, sizeof(G_cDbgExe));
	GetModuleFileNameA(NULL, G_cDbgExe, sizeof(G_cDbgExe) - 1);
	strcpy(G_cDbgCarpeta, G_cDbgExe);
	cp = strrchr(G_cDbgCarpeta, '\\');
	if (cp != NULL) *cp = 0;
	strcat(G_cDbgCarpeta, "\\Depuracion");
	CreateDirectoryA(G_cDbgCarpeta, NULL);

	// Si la sesion anterior no llego a cerrarse, se guarda su ultimo estado
	_snprintf(cRuta, sizeof(cRuta) - 1, "%s\\UltimosEventos.txt", G_cDbgCarpeta);
	cRuta[sizeof(cRuta) - 1] = 0;
	hFile = CreateFileA(cRuta, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		ZeroMemory(cLinea, sizeof(cLinea));
		ReadFile(hFile, cLinea, sizeof(cLinea) - 1, &dwLeido, NULL);
		CloseHandle(hFile);
		if (strncmp(cLinea, "Estado: EN EJECUCION", 20) == 0) {
			_DbgNombreArchivo(cNueva, sizeof(cNueva), "Cierre inesperado", "txt");
			MoveFileA(cRuta, cNueva);
		}
	}

	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &G_hDbgHiloPrincipal, 0, FALSE, DUPLICATE_SAME_ACCESS);
	G_dwDbgIdHiloPrincipal = GetCurrentThreadId();

	_DbgCargarDbgHelp();

	SetUnhandledExceptionFilter(_DbgFiltroCrash);
	AddVectoredExceptionHandler(1, _DbgVectorExcepcion);
	_set_invalid_parameter_handler(_DbgParametroInvalido);
	_set_purecall_handler(_DbgLlamadaPuraVirtual);
	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
	signal(SIGABRT, _DbgSenalAbort);

	DebugCliente_Texto("cliente iniciado");
	_DbgEscribirUltimos("EN EJECUCION");
	CloseHandle(CreateThread(NULL, 0, _DbgHiloVigia, NULL, 0, NULL));
}

void DebugCliente_Latido()
{
	G_dwDbgLatido = GetTickCount();
}

void DebugCliente_Evento(int iTipo, DWORD a, DWORD b, DWORD c)
{
	LONG l = InterlockedIncrement(&G_lDbgIndice) - 1;
	stDbgEvento * p = &G_stDbgEventos[(DWORD)l % DBGC_MAXEVENTOS];
	p->dwTiempo = GetTickCount();
	p->iTipo = iTipo;
	p->a = a;
	p->b = b;
	p->c = c;
	p->cTexto[0] = 0;
}

void DebugCliente_Texto(const char * cTexto)
{
	LONG l = InterlockedIncrement(&G_lDbgIndice) - 1;
	stDbgEvento * p = &G_stDbgEventos[(DWORD)l % DBGC_MAXEVENTOS];
	p->dwTiempo = GetTickCount();
	p->iTipo = DBGC_TEXTO;
	p->a = p->b = p->c = 0;
	ZeroMemory(p->cTexto, sizeof(p->cTexto));
	if (cTexto != NULL) strncpy(p->cTexto, cTexto, sizeof(p->cTexto) - 1);
}

void DebugCliente_Cierre(const char * cMotivo)
{
	char cEstado[128];

	if (!G_bDbgIniciado) return;
	if (G_lDbgEnCrash != 0) return;
	if (G_bDbgCerrado) return;
	G_bDbgCerrado = TRUE;
	DebugCliente_Texto(cMotivo);
	_snprintf(cEstado, sizeof(cEstado) - 1, "CERRADO NORMAL (%s)", (cMotivo != NULL) ? cMotivo : "");
	cEstado[sizeof(cEstado) - 1] = 0;
	_DbgEscribirUltimos(cEstado);
}
