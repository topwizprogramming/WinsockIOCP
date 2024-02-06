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
   DWORD RequestNbr = 0;

   // Loop forever accepting connections from clients
   while (true)
   {
      // Accept a connection
      Accept = WSAAccept(g.ListenSocket, NULL, NULL, NULL, 0);
      if (Accept == SOCKET_ERROR) {
         return HandleError(L"WSAAccept", WSAGetLastError());
      }

      RequestNbr++;

      // Associate the socket with the completion port
      if (CreateIoCompletionPort((HANDLE)Accept, g.CompletionPort, RequestNbr, 0) == NULL) {
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
   WriteToLogFile(L"WorkerThread Called!");

   /*
   Receive the incoming data, do processing, send a reply.
   */

   return 0;
}
