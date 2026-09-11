// winmain.h
BOOL InitApplication( HINSTANCE hInstance);
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK fnMessageProcessor( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam );
void OnAccept();
void PutLogList(char * cMsg);
void CharacterLogList(char * cMsg);
//void NpcDropLogList(char * cMsg);
void ConfigList(char * cMsg);
void ErrorList(char * cMsg);
void ChatLogs(char * cMsg);
void OnKeyUp(WPARAM wParam, LPARAM lParam);
void UpdateScreen();
int  EventLoop();
void OnPaint();
void OnDestroy();
void CALLBACK _TimerFunc(UINT wID, UINT wUser, DWORD dwUSer, DWORD dw1, DWORD dw2);
MMRESULT _StartTimer(DWORD dwTime);
void _StopTimer(MMRESULT timerid);
void UpdateConfigList(char * cMsg);


void Borrado();
void PutLogTradeFileList(char * cStr);
void PutLogCoinsFileList(char * cStr);
void PutLogHacksFileList(char * cStr);
void PutLogAtaque(char * cStr);
void PutLogOnlinesFileList(char * cStr);
//lalov9 drops
void PutLogDrops(char * cStr);


void PutGMLogData(char * cStr);
void PutEkAndPkLogFileList(char * cStr);
void PutLogItemsList(char * cStr);