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
   LPPER_HANDLE_DATA PerHandleData;

   // Loop forever accepting connections from clients
   while (true)
   {
      Accept = WSAAccept(g.ListenSocket, NULL, NULL, NULL, 0);
      if (Accept == SOCKET_ERROR) {
         return HandleError(L"WSAAccept", WSAGetLastError());
      }

      PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
      if (PerHandleData == NULL) {
         return HandleError(L"GlobalAlloc", GetLastError());
      }
      PerHandleData->Socket = Accept;

      if (CreateIoCompletionPort((HANDLE)Accept, g.CompletionPort, (DWORD)PerHandleData, 0) == NULL) {
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
