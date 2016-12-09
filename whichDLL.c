#define INCL_DOSMODULEMGR
#define INCL_DOSPROCESS
#include <os2.h>
#include <stdio.h>
#include <string.h>


/* ---------------------------------------------------------------------------
 Translatable text messages.
--------------------------------------------------------------------------- */

#define SZ_HELP                                                                 \
" WHICHDLL v. 0.2 - (c) 2001-2006 Alessandro Cantatore\n"                       \
" This program displays the full path name of a DLL.\n"                         \
" When multiple instances of a single DLL are present the program identifies\n" \
" which one is actually used by the system\n"                                   \
" Synopsis:\n"                                                                  \
" WHICHDLL <dllname[.DLL]>\n"

#define SZERR_DOSLOADMOD         "\nDosLoadModule was unsuccessful\n"
#define SZERR_QRYMODNAME         "\nDosQueryModuleName was unsuccessful\n"

#define SZ_RESULT   "The loaded instance of %s is:\n%s\n"                   \


/* --------------------------------------------------------------------------
 Display a system error message.
- Parameters -------------------------------------------------------------
 PSZ pszMsg : message reporting the failing API.
 APIRET rc  : error message.
- Return value -----------------------------------------------------------
 VOID
-------------------------------------------------------------------------- */
VOID printErrorMsg(PSZ pszMsg, APIRET rc) {
   RESULTCODES resc;
   CHAR parms[40];
   printf(pszMsg);
   sprintf(parms, "HELPMSG.EXE\x00");
   sprintf(parms + strlen(parms) + 1, "%d\x00", rc);
   DosExecPgm(NULL, 0, EXEC_SYNC, parms, NULL, &resc, "HELPMSG.EXE");
}


/* ---------------------------------------------------------------------------
 Entry point
--------------------------------------------------------------------------- */

int main(int argc, char** argv) {
   APIRET rc = 0;
   HMODULE hmod;
   CHAR achModuleName[260];
   BOOL bUnload = FALSE;

   /* check the input arguments */
   if (
         (argc != 2)
         ||
         (
            ((argv[1][0] == '-') || (argv[1][0] == '/'))
            &&
            ((argv[1][1] == 'h') || (argv[1][1] == 'H') || (argv[1][1] == '?'))
         )
      )
   {
      printf(SZ_HELP);
      return 0;
   } /* endif */

   /* check if the DLL is already loaded */
   rc = DosQueryModuleHandle(argv[1], &hmod);
   if (rc)
   {
      /* try to load the DLL */
      rc = DosLoadModule(NULL, 0, argv[1], &hmod);
      if (rc)
      {
         printErrorMsg(SZERR_DOSLOADMOD, rc);
         return rc;
      }
      bUnload = TRUE;
   }

   /* get the full path name of the DLL */
   rc = DosQueryModuleName(hmod, 260, achModuleName);
   if (rc)
   {
      printErrorMsg(SZERR_QRYMODNAME, rc);
      DosFreeModule(hmod);
      return rc;
   } /* endif */

   /* free the module */
   if (bUnload) DosFreeModule(hmod);

   /* show the name of the loaded DLL */
   printf(SZ_RESULT, argv[1], achModuleName);

   return rc;
}
