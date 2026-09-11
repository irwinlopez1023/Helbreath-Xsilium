#include "XSocket.h"

LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL InitApplication( HINSTANCE hInstance);
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow );
int EventLoop();
void Initialize();
void UpdateScreen();
void OnDestroy();
void OnPaint();
void ParseCommand(char* pMsg);

void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2);
MMRESULT _StartTimer(DWORD dwTime);
void _StopTimer(MMRESULT timerid);
void OnAccept();
void OnKeyUp(WPARAM wParam, LPARAM lParam);

//Global Defs'
BOOL G_bIsMainLogActive;
char G_cData50000[50000];

//Socket to listen for clients!
class XSocket * G_pXMainLogSock=NULL;

bool LoginEnabled;

void ConfigList(char * cMsg);
void PutLogList(char * cMsg);
void AccountLogList(char * cMsg);
void ErrorLogList(char * cMsg);
void PutLogAtaque(char * cStr);
void CommandList(char * cMsg);
void UpdateConfigList(char * cMsg);

short G_sLogCounter;
DWORD G_dwLogTime;
char  G_cLogBuffer[30000];

short G_sAccountCounter;
DWORD G_dwAccountTime;
char  G_cAccountBuffer[30000];

