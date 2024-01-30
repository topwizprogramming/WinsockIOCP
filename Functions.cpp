//
// Functions.cpp
//

#include "WinsockIOCP.h"

extern GlobalVariables g;

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   ReadCommandLine                                                 */
/*                                                                                */
/*   Purpose  :   Get the port from the command line.                             */
/*                                                                                */
/* ****************************************************************************** */

bool ReadCommandLine(int argc, TCHAR* argv[])
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
   HANDLE ThreadHandle = NULL;
   DWORD dwIndex = 0;

   // Determine how many processors are on the system
   GetSystemInfo(&sysInfo);

   // Create an I/O completion port
   g.CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

   // Create worker threads based on the number of processors.
   for (dwIndex = 0; dwIndex < sysInfo.dwNumberOfProcessors * 2; dwIndex++)
   {
      // Create a server worker thread, and pass the completion port.
      ThreadHandle = CreateThread(NULL, 0, WorkerThread, g.CompletionPort, 0, NULL);

      if (ThreadHandle == NULL) {
         return HandleError(L"CreateThread", GetLastError());
      }
      else {
         // Close the thread handle
         CloseHandle(ThreadHandle);
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
      WSACleanup();
      return HandleError(L"GetAddrInfoW", iResult);
   }
   if (addrlocal == NULL) {
      WSACleanup();
      return HandleError(L"Function GetAddrInfoW failed to resolve the interface.");
   }

   // Create a listening socket
   g.ListenSocket = WSASocket(addrlocal->ai_family, addrlocal->ai_socktype,
      addrlocal->ai_protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
   if (g.ListenSocket == INVALID_SOCKET) {
      WSACleanup();
      return HandleError(L"WSASocket", WSAGetLastError());
   }

   // Bind the socket
   iResult = bind(g.ListenSocket, addrlocal->ai_addr, (int)addrlocal->ai_addrlen);
   if (iResult == SOCKET_ERROR) {
      closesocket(g.ListenSocket);
      WSACleanup();
      return HandleError(L"bind", WSAGetLastError());
   }

   // Release the local address structure
   FreeAddrInfoW(addrlocal);

   // Listen for connections
   iResult = listen(g.ListenSocket, SOMAXCONN);
   if (iResult == SOCKET_ERROR) {
      closesocket(g.ListenSocket);
      WSACleanup();
      return HandleError(L"listen", WSAGetLastError());
   }

   // Disable send buffering on the socket.
   iResult = setsockopt(g.ListenSocket, SOL_SOCKET, SO_SNDBUF,
      (char*)&iZero, sizeof(iZero));
   if (iResult == SOCKET_ERROR) {
      closesocket(g.ListenSocket);
      WSACleanup();
      return HandleError(L"setsockopt", WSAGetLastError());
   }

   return true;
}
