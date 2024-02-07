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
using std::string;

struct GlobalVariables
{
   wstring ServicePort;
   HANDLE CompletionPort = INVALID_HANDLE_VALUE;
   SOCKET ListenSocket = INVALID_SOCKET;
};

#define DATA_BUFSIZE 8192

typedef struct
{
   OVERLAPPED Overlapped;
   WSABUF DataBuf;
   CHAR Buffer[DATA_BUFSIZE];
   DWORD BytesSend;
   DWORD BytesRecv;
} PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;

typedef struct _PER_CONNECTION_DATA {
   SOCKET ClientSocket;
   int RequestNbr;
} PER_CONNECTION_DATA, * LPPER_CONNECTION_DATA;

// MainFunctions.cpp
bool MainProcessing(void);
DWORD WINAPI WorkerThread(LPVOID lpParam);

// Setup.cpp
bool CmdLineSetup(int argc, TCHAR* argv[]);
bool ThreadSetup();
bool WinsockSetup();

// Utility.cpp
bool FileExists(LPCTSTR lpszFileName);
bool HandleError(LPCTSTR lpszErrorMsg);
bool HandleError(LPCTSTR lpszFunction, int iErrorNum);
void WriteToEventLog(WORD wEventType, LPCTSTR lpszMsg);
void WriteToLogFile(LPCTSTR lpszMsg);
void WriteToLogFileEx(LPCTSTR lpszLogfile, LPCTSTR lpszMsg);
