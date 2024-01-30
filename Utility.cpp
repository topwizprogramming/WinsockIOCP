//
// Utility.cpp
//

#include "WinsockIOCP.h"

extern GlobalVariables g;

/* ****************************************************************************** */
/*                                                                                */
/*  Function : FileExists                                                         */
/*                                                                                */
/*  Purpose  : This function determines if a file exists.                         */
/*                                                                                */
/* ****************************************************************************** */

bool FileExists(LPCTSTR lpszFileName)
{
   if (GetFileAttributes(lpszFileName) == INVALID_FILE_ATTRIBUTES) {
      return false;
   }

   return true;
}

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   HandleError                                                     */
/*                                                                                */
/*   Purpose  :   Write the passed error message to the event log.                */
/*                                                                                */
/* ****************************************************************************** */

bool HandleError(LPCTSTR lpszErrorMsg)
{
   // Winsock Cleanup
   if (g.ListenSocket > 0) {
      closesocket(g.ListenSocket);
   }
   WSACleanup();

   WriteToEventLog(EVENTLOG_ERROR_TYPE, lpszErrorMsg);

   return false;
}

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   HandleError                                                     */
/*                                                                                */
/*   Purpose  :   Get the error message and write it to the event log.            */
/*                                                                                */
/* ****************************************************************************** */

bool HandleError(LPCTSTR lpszFunction, int iErrorNum)
{
   TCHAR szMessage[MAX_PATH];
   TCHAR szErrorMsg[MAX_PATH];

   // Winsock Cleanup
   if (g.ListenSocket > 0) {
      closesocket(g.ListenSocket);
   }
   WSACleanup();

   wmemset(szMessage, 0x00, _countof(szMessage));
   wmemset(szErrorMsg, 0x00, _countof(szErrorMsg));

   // retrieve the error message text
   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
      iErrorNum, LANG_NEUTRAL, szMessage, MAX_PATH, 0);

   _stprintf_s(szErrorMsg, _countof(szErrorMsg),
      L"Function '%s' failed with error %d: %s", lpszFunction, iErrorNum, szMessage);

   WriteToEventLog(EVENTLOG_ERROR_TYPE, szErrorMsg);

   return false;
}

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   WriteToEventLog                                                 */
/*                                                                                */
/*   Purpose  :   Write a message to the event log.                               */
/*                                                                                */
/* ****************************************************************************** */

void WriteToEventLog(WORD wEventType, LPCTSTR lpszMsg)
{
   HANDLE hEventLog = 0;
   LPCTSTR lpszStrings[1];

   lpszStrings[0] = lpszMsg;

   hEventLog = RegisterEventSource(NULL, L"WinsockIOCP");
   if (hEventLog != NULL) {
      ReportEvent(hEventLog, wEventType,
         0, 0, 0, 1, 0, lpszStrings, 0);
      DeregisterEventSource(hEventLog);
   }
   else {
      WriteToLogFile(L"RegisterEventSource Failed");
   }
}

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   WriteToLogFile                                                  */
/*                                                                                */
/*   Purpose  :   Write a message to the log file.                                */
/*                                                                                */
/* ****************************************************************************** */

void WriteToLogFile(LPCTSTR lpszMsg)
{
   SYSTEMTIME lt;
   TCHAR szFileName[MAX_PATH];
   TCHAR szTempPath[MAX_PATH];

   wmemset(szFileName, 0x00, _countof(szFileName));
   wmemset(szTempPath, 0x00, _countof(szTempPath));

   // determine the current time
   GetLocalTime(&lt);

   GetTempPath(_countof(szTempPath), szTempPath);

   _stprintf_s(szFileName, _countof(szFileName),
      L"%sWinsockIOCP_%d-%02d-%02d.log",
      szTempPath, lt.wYear, lt.wMonth, lt.wDay);

   WriteToLogFileEx(szFileName, lpszMsg);
}

/* ****************************************************************************** */
/*                                                                                */
/*   Function :   WriteToLogFileEx                                                */
/*                                                                                */
/*   Purpose  :   Write a message to the specified log file.                      */
/*                                                                                */
/* ****************************************************************************** */

void WriteToLogFileEx(LPCTSTR lpszLogfile, LPCTSTR lpszMsg)
{
   SYSTEMTIME lt;
   HANDLE hFile;
   DWORD dwBytesToWrite = 0;
   DWORD dwBytesWritten = 0;
   char sMarker[2];
   TCHAR szMsgText[MAX_PATH];
   TCHAR szTemp[MAX_PATH];
   BOOL bFileExists = FALSE;

   memset(sMarker, 0x00, sizeof(sMarker));
   wmemset(szMsgText, 0x00, _countof(szMsgText));
   wmemset(szTemp, 0x00, _countof(szTemp));

   // determine the current time
   GetLocalTime(&lt);

   // format the date into a localized string
   GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &lt, NULL, szTemp, _countof(szTemp));
   _tcscpy_s(szMsgText, szTemp);
   _tcscat_s(szMsgText, L" ");

   // format the time into a localized string
   GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOTIMEMARKER + TIME_FORCE24HOURFORMAT, &lt, NULL, szTemp, _countof(szTemp));
   _tcscat_s(szMsgText, szTemp);
   _stprintf_s(szTemp, _countof(szTemp), L".%03d", lt.wMilliseconds);
   _tcscat_s(szMsgText, szTemp);

   // determine if file exists
   bFileExists = FileExists(lpszLogfile);

   // write message to logfile
   hFile = CreateFile(lpszLogfile,
      FILE_APPEND_DATA, FILE_SHARE_WRITE, NULL,
      OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
   if (hFile != INVALID_HANDLE_VALUE) {
      // write BOM
      if (bFileExists == FALSE) {
         sMarker[0] = 0xFF;
         sMarker[1] = 0xFE;
         WriteFile(hFile, sMarker, 2, &dwBytesWritten, 0);
      }
      // write current time
      dwBytesToWrite = (DWORD)_tcslen(szMsgText) * sizeof(TCHAR);
      WriteFile(hFile, szMsgText, dwBytesToWrite, &dwBytesWritten, 0);
      WriteFile(hFile, L"\t", 2, &dwBytesWritten, 0);

      // write the message
      dwBytesToWrite = (DWORD)_tcslen(lpszMsg) * sizeof(TCHAR);
      WriteFile(hFile, lpszMsg, dwBytesToWrite, &dwBytesWritten, 0);
      WriteFile(hFile, L"\r\n", 4, &dwBytesWritten, 0);

      // close the file
      CloseHandle(hFile);
   }
}
