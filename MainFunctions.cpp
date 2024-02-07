//
// MainFunctions.cpp
//

#include "WinsockIOCP.h"

extern GlobalVariables g;

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   MainProcessing                                                  */
/*                                                                                */
/*   Purpose  :   This is the main processing function.                           */
/*                                                                                */
/* ****************************************************************************** */

bool MainProcessing(void)
{
   SOCKET AcceptSocket = INVALID_SOCKET;
   LPPER_CONNECTION_DATA PerConnData = NULL;
   LPPER_IO_OPERATION_DATA PerIoData = NULL;
   int RequestNbr = 0;
   DWORD dwFlags = 0;
   DWORD dwRecvBytes = 0;

   // Accept connections and assign to the completion port
   while (true)
   {
      // Accept a connection
      AcceptSocket = WSAAccept(g.ListenSocket, NULL, NULL, NULL, 0);
      if (AcceptSocket == SOCKET_ERROR) {
         return HandleError(L"WSAAccept", WSAGetLastError());
      }

      // Allocate per connection structure
      if ((PerConnData = (LPPER_CONNECTION_DATA)GlobalAlloc(GPTR, sizeof(PER_CONNECTION_DATA))) == NULL) {
         return HandleError(L"GlobalAlloc", GetLastError());
      }

      // Initialize per connection structure
      RequestNbr++;
      PerConnData->ClientSocket = AcceptSocket;
      PerConnData->RequestNbr = RequestNbr;

      // Associate the socket with the completion port
      if (CreateIoCompletionPort((HANDLE)AcceptSocket, g.CompletionPort, (DWORD_PTR)PerConnData, 0) == NULL) {
         return HandleError(L"CreateIoCompletionPort", GetLastError());
      }

      // Allocate per I/O structure
      if ((PerIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATION_DATA))) == NULL) {
         return HandleError(L"GlobalAlloc", GetLastError());
      }

      // Initialize per I/O structure
      ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));
      PerIoData->BytesSend = 0;
      PerIoData->BytesRecv = 0;
      PerIoData->DataBuf.len = DATA_BUFSIZE;
      PerIoData->DataBuf.buf = PerIoData->Buffer;

      // Initial receive on the socket
      dwFlags = 0;
      if (WSARecv(AcceptSocket, &(PerIoData->DataBuf), 1,
         &dwRecvBytes, &dwFlags, &(PerIoData->Overlapped), NULL) == SOCKET_ERROR) {
         return HandleError(L"WSARecv", WSAGetLastError());
      }
   }

   return true;
}

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   WorkerThread                                                    */
/*                                                                                */
/*   Purpose  :   Server worker thread main function.                             */
/*                                                                                */
/* ****************************************************************************** */

DWORD WINAPI WorkerThread(LPVOID lpParameter)
{
   HANDLE CompletionPort = (HANDLE)lpParameter;
   LPPER_CONNECTION_DATA PerConnData = NULL;
   LPPER_IO_OPERATION_DATA PerIoData = NULL;
   DWORD dwBytesTransferred = 0;
   string HttpHeaders;
   LPBYTE lpPostedData = NULL;
   int ContentLength = 0;
   TCHAR szMessage[MAX_PATH];

   wmemset(szMessage, 0x00, _countof(szMessage));

   while (true)
   {
      // Get completion packet from the queue
      if (GetQueuedCompletionStatus(CompletionPort, &dwBytesTransferred,
         (LPDWORD)&PerConnData, (LPOVERLAPPED*)&PerIoData, INFINITE) == FALSE) {
         return HandleError(L"GetQueuedCompletionStatus", GetLastError());
      }

      // Check to see if an error has occurred on the socket
      if (dwBytesTransferred == 0)
      {
         closesocket(PerConnData->ClientSocket);
         GlobalFree(PerConnData);
         GlobalFree(PerIoData);
         continue;
      }

      _stprintf_s(szMessage, _countof(szMessage),
         L"RequestNbr: %d, dwBytesTransferred: %d",
         PerConnData->RequestNbr, dwBytesTransferred);
      WriteToLogFile(szMessage);

      /*

      The received data will be a web request consisting of HTTP headers in Ansi
      format. Each individual header has \r\n characters at the end. Followed by
      that will be another \r\n and possibly some amount of binary data. The size
      of the binary data will be in the header ContentLength. Most requests will
      be less than the 8192 buffer size but we need to support posting large
      files to the server.

      All headers should end up in the string variable HttpHeaders. The binary
      data should end up in lpPostedData.
      
      */

   }

   return 1;
}
