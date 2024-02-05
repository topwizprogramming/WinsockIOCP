//
// Functions.cpp
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
   SOCKET Accept = INVALID_SOCKET;
   LPPER_CONN_DATA PerConnData;
   int RequestNbr = 0;

   // Loop forever accepting connections from clients
   while (true)
   {
      RequestNbr++;

      Accept = WSAAccept(g.ListenSocket, NULL, NULL, NULL, 0);
      if (Accept == SOCKET_ERROR) {
         return HandleError(L"WSAAccept", WSAGetLastError());
      }

      PerConnData = (LPPER_CONN_DATA)GlobalAlloc(GPTR, sizeof(PER_CONN_DATA));
      if (PerConnData == NULL) {
         return HandleError(L"GlobalAlloc", GetLastError());
      }
      PerConnData->Socket = Accept;
      PerConnData->RequestNbr = RequestNbr;

      if (CreateIoCompletionPort((HANDLE)Accept, g.CompletionPort, (DWORD)PerConnData, 0) == NULL) {
         return HandleError(L"CreateIoCompletionPort", GetLastError());
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

DWORD WINAPI WorkerThread(LPVOID lpParam)
{
   return 0;
}
