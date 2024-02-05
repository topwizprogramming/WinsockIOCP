//
// WinsockIOCP.cpp
//

#include "WinsockIOCP.h"

GlobalVariables g;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
   // Get the port from the command line
   if (!CmdLineSetup(argc, argv)) {
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

   // Perform main processing
   if (!MainProcessing()) {
      return 0;
   }

   // Winsock Cleanup
   if (g.ListenSocket > 0) {
      closesocket(g.ListenSocket);
   }
   WSACleanup();

   return 0;
}
