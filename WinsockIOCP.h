// WinsockIOCP.h

#pragma warning (disable: 4309)

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>
using std::wstring;

struct GlobalVariables
{
   wstring ServicePort;
   HANDLE CompletionPort = INVALID_HANDLE_VALUE;
   SOCKET ListenSocket = INVALID_SOCKET;
};

// WinsockIOCP.cpp
DWORD WINAPI WorkerThread(LPVOID lpParam);

// Functions.cpp
bool ReadCommandLine(int argc, TCHAR* argv[]);
bool ThreadSetup();
bool WinsockSetup();

// Utility.cpp
bool FileExists(LPCTSTR lpszFileName);
bool HandleError(LPCTSTR lpszErrorMsg);
bool HandleError(LPCTSTR lpszFunction, int iErrorNum);
void WriteToEventLog(WORD wEventType, LPCTSTR lpszMsg);
void WriteToLogFile(LPCTSTR lpszMsg);
void WriteToLogFileEx(LPCTSTR lpszLogfile, LPCTSTR lpszMsg);
