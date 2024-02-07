//
// Setup.cpp
//

#include "WinsockIOCP.h"

extern GlobalVariables g;

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   CmdLineSetup                                                    */
/*                                                                                */
/*   Purpose  :   Get the port from the command line.                             */
/*                                                                                */
/* ****************************************************************************** */

bool CmdLineSetup(int argc, TCHAR* argv[])
{
   // Verify command line argument
   if (argc < 2) {
      return HandleError(L"Missing port on command line.");
   }

   // Save the Port
   g.ServicePort = argv[1];

   return true;
}

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   ThreadSetup                                                     */
/*                                                                                */
/*   Purpose  :   Create threads and the I/O Completion Port.                     */
/*                                                                                */
/* ****************************************************************************** */

bool ThreadSetup()
{
   SYSTEM_INFO sysInfo;
   HANDLE hThread = INVALID_HANDLE_VALUE;
   DWORD dwThreadId = 0;
   DWORD dwCPU = 0;

   // Determine how many processors are on the system
   GetSystemInfo(&sysInfo);

   // Create an I/O completion port
   g.CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

   // Create worker threads based on the number of processors.
   for (dwCPU = 0; dwCPU < sysInfo.dwNumberOfProcessors * 2; dwCPU++)
   {
      // Create a server worker thread, and pass the completion port.
      hThread = CreateThread(NULL, 0, WorkerThread, g.CompletionPort, 0, &dwThreadId);

      if (hThread == NULL) {
         return HandleError(L"CreateThread", GetLastError());
      }
      else {
         // Close the thread handle
         CloseHandle(hThread);
      }
   }

   return true;
}

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   WinsockSetup                                                    */
/*                                                                                */
/*   Purpose  :   Initialize Winsock.                                             */
/*                                                                                */
/* ****************************************************************************** */

bool WinsockSetup()
{
   WSADATA wsaData;
   ADDRINFOW hints;
   ADDRINFOW* addrlocal = NULL;
   int iResult = 0;
   int iZero = 0;

   // Initialize Winsock
   iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iResult != NO_ERROR) {
      return HandleError(L"WSAStartup", WSAGetLastError());
   }

   // Initialize the structure
   ZeroMemory(&hints, sizeof(hints));
   hints.ai_flags = AI_PASSIVE;
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_IP;

   // Resolve the server address and port
   iResult = GetAddrInfoW(NULL, g.ServicePort.c_str(), &hints, &addrlocal);
   if (iResult != 0) {
      return HandleError(L"GetAddrInfoW", iResult);
   }
   if (addrlocal == NULL) {
      return HandleError(L"Function GetAddrInfoW failed to resolve the interface.");
   }

   // Create a listening socket
   g.ListenSocket = WSASocket(addrlocal->ai_family, addrlocal->ai_socktype,
      addrlocal->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
   if (g.ListenSocket == INVALID_SOCKET) {
      return HandleError(L"WSASocket", WSAGetLastError());
   }

   // Bind the socket
   iResult = bind(g.ListenSocket, addrlocal->ai_addr, (int)addrlocal->ai_addrlen);
   if (iResult == SOCKET_ERROR) {
      return HandleError(L"bind", WSAGetLastError());
   }

   // Release the local address structure
   FreeAddrInfoW(addrlocal);

   // Listen for connections
   iResult = listen(g.ListenSocket, SOMAXCONN);
   if (iResult == SOCKET_ERROR) {
      return HandleError(L"listen", WSAGetLastError());
   }

   // Disable send buffering on the socket.
   iResult = setsockopt(g.ListenSocket, SOL_SOCKET, SO_SNDBUF,
      (char*)&iZero, sizeof(iZero));
   if (iResult == SOCKET_ERROR) {
      return HandleError(L"setsockopt", WSAGetLastError());
   }

   return true;
}
