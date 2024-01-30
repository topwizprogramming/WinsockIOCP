//
// WinsockIOCP.cpp
//

#include "WinsockIOCP.h"

GlobalVariables g;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
   // Get the port from the command line
   if (!ReadCommandLine(argc, argv)) {
      return 0;
   }

   // Create threads and the I/O Completion Port
   if (!ThreadSetup()) {
      return 0;
   }

   // Intialize Winsock
   if (!WinsockSetup()) {
      return 0;
   }

   // Winsock Cleanup
   WSACleanup();

   return 0;
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
