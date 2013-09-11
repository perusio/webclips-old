//-------------------------------------------------------------------
//- WebCLIPS Main Driver                                            -
//-                                                                 -
//- This module contains the main routine for executing a WebCLIPS  -
//- script.                                                         -
//-                                                                 -
//- Source file ; WebCLIPS.CPP                                      -
//-                                                                 -
//- Who | Version |   Date    | Desc                                -
//- ----+---------+-----------+------------------------------------ -
//- MJG |   1.0   | 17-Jan-97 | Hard coded program to execute       -
//-     |         |           |   Help Desk Assistant               -
//-     |         |           |                                     -
//- MJG |   1.1   | 14-Feb-97 | Added separate fact-base            -
//-     |         |           |                                     -
//- MJG |   1.2   | 17-Apr-97 | WebCLIPS create                     -
//-     |         |           |                                     -
//- MJG |   1.3   | 27-Jul-97 | Change input method to POST,        -
//-     |         |           |   'unlimited' number of facts,      -
//-     |         |           |   fact groups and scripts.          -
//-     |         |           |                                     -
//-     |         |           | Preserve fact-state between CLIPS   -
//-     |         |           |   screens.                          -
//-     |         |           |                                     -
//- MJG |  1.3.1  | 05-Aug-97 | Enhanced Error Reporting            -
//-     |         |           |                                     -
//- MJG |   1.4   | 06-Aug-97 | ODBC queries                        -
//-     |         |           |                                     -
//- MJG |  1.4.1  | 07-Aug-97 | Correct bug in dynamic screen       -
//-     |         |           |	  navigation.                       -
//-     |         |           |                                     -
//- MJG |  1.4.2  | 14-Aug-97 | Add BLoad option for a screen and   -
//-     |         |           |	  added option to exclude all facts -
//-     |         |           |   except the 'ScreenName' fact      -
//-     |         |           |                                     -
//- MJG |  1.4.3  | 14-Oct-97 | Correction to properly place the    -
//-     |         |           |		</form> tag for static screens. -
//-     |         |           |                                     -
//- MJG |  1.4.4  | 29-Oct-97 | Correction to URL-Encode any facts  -
//-     |         |           |   preserved between screens.        -
//-     |         |           |                                     -
//-     |         |           | Added enhancement to allow an       -
//-     |         |           |   optional limit on Agenda rules    -
//-     |         |           |   firing.                           -
//-     |         |           |                                     -
//- MJG |  1.4.5  | 06-Nov-97 | Corrections made to support long    -
//-     |         |           |   file names.                       -
//-     |         |           |                                     -
//- MJG |  1.4.6  | 07-Nov-97 | Corrections made to assert a fact   -
//-     |         |           |   for a new screen name from        -
//-     |         |           |   WebCLIPS.INI                      -
//-     |         |           |                                     -
//- MJG |  1.4.7  | 12-Nov-97 | Changed parameter MAX_FACT_LEN in   -
//-     |         |           |   Clipsmfc.h to allow facts to be   -
//-     |         |           |   up to 640 bytes long.             -
//-     |         |           |                                     -
//- MJG |   2.0   | 27-Nov-97 | Added Routers for standard CLIPS    -
//-     |         |           |   standard routers (wdisplay,       -
//-     |         |           |   wclips, wdialog etc.) and for     -
//-     |         |           |   user-defined routers. Also,       -
//-     |         |           |   added an option to preserve facts -
//-     |         |           |   to a disk file.                   -
//-     |         |           |                                     -
//- MJG |  2.0.1  | 22-Dec-97 | Added logic to process 'cookies'.   -
//-     |         |           |   This will allow users to preserve -
//-     |         |           |   facts across sessions. Also,      -
//-     |         |           |   added the missing system router   -
//-     |         |           |   'wdisplay'.                       -
//-     |         |           |                                     -
//- MJG |  2.0.2  | 09-Jan-98 | Split up the classification of fact -
//-     |         |           |   types into separate function.     -
//-     |         |           |                                     -
//-     |         |           | Now the entry for [System]          -
//-     |         |           |    CLIPSOutput is no longer a       -
//-     |         |           |    suggestion. This directory is    -
//-     |         |           |    used for temporary files.        -
//-     |         |           |                                     -
//- MJG |  2.0.3  | 29-Oct-99 | Cvt to UNIX                         -
//-                                                                 -
//-------------------------------------------------------------------

/*
WebCLIPS; A CGI implementation of CLIPS.
Copyright (C) 1999  Michael Giordano

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Any questions regarding this software should be directed to cissmjg@hotmail.com
*/

#include "WebCLIPS.h"

char*					g_szBuffer;
char**					g_strScreenDataType;
char**					g_strScreenData;
struct WebCLIPSRouter*	g_pStdWebCLIPSRouter;
struct WebCLIPSRouter*	g_pUDFWebCLIPSRouter;

char					g_szWebCLIPSINI[MAXNAMLEN + 1];
char					g_szSaveCookiesFile[MAXNAMLEN + 1];
char					g_szGetCookiesFile[MAXNAMLEN + 1];
char					g_szTempPath[MAXNAMLEN + 1];
char					g_szScreenName[MAX_SCREEN_NAME_SIZE + 1];
int						g_bScriptFound;

int main(int argc, char *argv[])
{
	char *szFileBuffer;
	int iErrCode; 
	char szPrevScreen[MAX_SCREEN_NAME_SIZE + 1];

	//Get the location of where we are running and obtain the name of the
	//configuration file.
	getcwd(g_szWebCLIPSINI, MAXNAMLEN);
	strcat(g_szWebCLIPSINI, "/WebCLIPS.ini");

	//Set stdout to unbuffered
	setvbuf(stdout, NULL, _IONBF, 0);

	//Assume no CLIPS script or CLIPS binary file has been submitted to run
	g_bScriptFound = FALSE;

	//Setup the TMP variable to point to a place the web server can
	//	access. If a 'gen-filename' request comes through to generate
	//	a GIF, the HTML generated in WebCLIPS can point to the temporary
	//	directory
	if(SetTempDir() != 0)
		return 0;

	//Obtain Input data from web server. If error exists, stop execution.
	//	Also populates the member variables ;
	//		- g_strScreenDataType
	//		- g_strScreenData
	//		- g_szScreenName

	if(ProcessWebServerData() != 0)
	{
		CleanUpWebServer();
		return 0;
	}

	if(ClassifyWebServerData() != 0)
	{
		CleanUpWebServer();
		return 0;
	}

	//Process any necessary cookie parameters
	//This means writing out a cookie if the Current screen is going to
	//save facts across sessions.
	if(ProcessCookies() != 0)
	{
		CleanUpWebServer();
		return 0;
	}

	//Initialize CLIPS for WebCLIPS
	if(Init() != 0)
	{
		CleanUpWebServer();
		return 0;
	}
	//Setup CLIPS standard routers and user-defined routers
	if(SetupRouters() != 0)
	{
		CleanUpWebServer();
		ExitCLIPS(0);
		return(0);
	}

	//Load the WebCLIPS helper functions and deftemplates
	if(LoadHelper() != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//Process all CLIPS Scripts from WebCLIPS.INI and from previously
	//	asserted facts.
	if(ProcessAllScripts() != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//Process all Binary Image files from WebCLIPS.INI and from previously
	//	asserted facts.
	if(ProcessBinImage() != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//Issue CLIPS 'reset' Command
	Reset();

	//Assert all Facts from the screen first and then any individual facts
	//   from WebCLIPS.INI
	if(ProcessAllFacts() != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//Process any separate factgroups from previously asserted facts or
	//	WebCLIPS.INI
	if(ProcessAllFactGroups() != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//User Defined Functions go here
	UserFunctions();

	//Run the CLIPS Engine with the Agenda Limit (if any)
	Go();

	//Close All routers
	if(CloseAllRouters() != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//Save ScreenName before potentially changing it
	SaveScreenName(szPrevScreen);

	//Screen Navigation ;
	//WebCLIPS will look for a fact called 'ScreenName' and use this value
	//	first to determine any further processing.
	//
	//If no fact called 'ScreenName' exists then WebCLIPS will look in
	//	WebCLIPS.INI for an entry called 'NextScreenName'.
	//
	//This means if a CLIPS script wants to dynamically determine the next
	//	screen to be processed, the script must retract the old 'ScreenName'
	//	fact and replace it with a new 'ScreenName' fact.
	//If the CLIPS script wants WebCLIPS.INI to determine the next screen
	//	to be processed, then just retract the 'ScreenName' fact.
	//If neither of the above actions is performed, then the current 'ScreenName'
	//	fact will automatically be reasserted and the current screen will
	//	be processed again.
	//
	//A non-zero return code means that the fact assertion for a new screen name
	//	from WebCLIPS.INI failed.
	if(UpdateScreenName() != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//If there is a file containing the HTML layout of the screen, display it.
	//	If there is no file return NULL.
	//	Also, if there is a '</body>' in the file, eliminate it.
	iErrCode = GetScreenDisplay(&szFileBuffer);
	if(iErrCode < 0) //Error
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//Print MIME header info for Web Server
	PrintMIMEHeader();

	//Print the contents of the header HTML file
	if(iErrCode == 0)
	{
		//Display the input screen and a horizontal line. This separates the input portion from
		//the output portion of the screen.
		printf("%s<hr>", szFileBuffer);
		free(szFileBuffer);
	}

	//Display and router output directed to the screen
	if(DisplayScreenRouters() != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	//Dump any remaining Facts -- Preserving the Fact State and updating
	//   the Global variable szScreenName
	if(PreserveFactState(szPrevScreen) != 0)
	{
		CleanUpWebServer();
		CleanUpRouters();
		ExitCLIPS(0);
		return(0);
	}

	if(iErrCode == 0)
	{
		//If we are here, then there is an Initial screen that must have been invoked
		//	with a <form> tag. We removed the </form> tag in GetScreenDisplay, so
		//	now we must replace it.
		printf("</form>");
	}

	CleanUpWebServer();
	CleanUpRouters();
	PrintHTMLTrailer();
	ExitCLIPS(0);
	return(1);
}

VOID UserFunctions()
{
}
