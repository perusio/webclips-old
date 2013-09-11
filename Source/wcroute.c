//-------------------------------------------------------------------
//- WebCLIPS Router functions                                       -
//-                                                                 -
//- This module contains functions to setup nd process standard     -
//- CLIPS routers as well as user defined routers.                  -
//-                                                                 -
//- Source file : wcroute.cpp                                       -
//-                                                                 -
//- Who           |   Date    | Desc                                -
//- --------------+-----------+------------------------------------ -
//- M.Giordano    | 09-Dec-97 | Create                              -
//-               |           |                                     -
//- M.Giordano    | 09-Feb-97 | Converted to MFC App                -
//-               |           |                                     -
//- M.Giordano    | 19-Nov-99 | Converted to HP-UX                  -
//-               |           |                                     -
//-                                                                 -
//-------------------------------------------------------------------

/*
WebCLIPS: A CGI implementation of CLIPS.
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
#include "WCVars.h"

//Standard CLIPS routers are stdout, wdialog, wwarning, werror

//Setup all routers. Parse through the WebCLIPS.INI and open
//	all required routers.
int	SetupRouters(void)
{
	char szRouterList[MAX_SETTINGS_LINE_LEN], *p, szEchoEntry[MAX_ROUTER_NAME_LEN + 4 + 1]; //<router> "Echo" = 4
	char szRouterFile[MAXNAMLEN + 1];
	int iMaxPriority = 40;
	char szRouterTemp[MAX_ROUTER_NAME_LEN + 1], szRouterTemp2[MAX_ROUTER_NAME_LEN + 1];
	struct WebCLIPSRouter *pWCRouter;
	int bSetRouterResult, bResult;

	//Get the list of routers
	GetWebCLIPSSettings(
		g_szScreenName,		// section name
		"RouterList",		// entry name
		"",				// default value
		szRouterList,		// return value
		sizeof(szRouterList),	// max length
		g_szWebCLIPSINI
        );

	//Set up the standard CLIPS routers : stdout, wdialog, wwarning, werror
	if(InitializeStdCLIPSRouters() != 0)
		return(-1);

	//If no additional routers requested then exit
	if(strlen(szRouterList) == 0)
	{
		return(0);
	}

	//Search for '|'. Set each router name in the string array
	p = strtok(szRouterList, "|");
	g_pUDFWebCLIPSRouter = (struct WebCLIPSRouter *)calloc(1, sizeof(struct WebCLIPSRouter));
	pWCRouter = g_pUDFWebCLIPSRouter;
	while(p)
	{
		strcpy(szRouterTemp2, p);
		Trim(szRouterTemp2, szRouterTemp);

		strcpy(szEchoEntry, szRouterTemp);
		strcat(szEchoEntry, "Echo");

		//Get <router>Echo entry
		GetWebCLIPSSettings(
			g_szScreenName,			// section name
			szEchoEntry,			// entry name
			"",					// default value
			szRouterFile,			// return value
			sizeof(szRouterFile),		// max length
			g_szWebCLIPSINI
			);

		//If router is set to 'no' or is absent then get the next router. Also, if
		//	there is a request for stdout, wdialog, wwarning, werror to be directed
		//	to the screen, then ignore it. These will be handled by automatically.
		//
		//	Also, ignore requests to 'dribble' to screen. The reason is that because
		//	there are no </form> tags any facts that are asserted from a screen actually
		//	get asserted TWICE..this is undesirable
		if(strcasecmp(szRouterFile, "no") == 0 || strlen(szRouterFile) == 0 ||
		  (strcasecmp(szRouterFile, "screen") == 0 && 
			(strcasecmp(szRouterTemp, "stdout") == 0 ||
			 strcasecmp(szRouterTemp, "dribble") == 0 ||
			 strcasecmp(szRouterTemp, "wdialog") == 0 ||
			 strcasecmp(szRouterTemp, "wwarning") == 0||
			 strcasecmp(szRouterTemp, "werror") == 0)))
		{
			p = strtok(NULL, "|");
			continue;
		}

		//Set the name of the router.
		pWCRouter->szRouter = (char *)malloc(strlen(szRouterTemp) + 1);
		if (!pWCRouter->szRouter)
		{
			ProcessErrorCode("ROUT0003", szRouterTemp, 'n', 'n');
			return(-1);
		}
		strcpy(pWCRouter->szRouter, szRouterTemp);

		//If router is to be directed to the screen, then get a
		//	temporary file name else use the entry in WebCLIPS.INI
		if(strcasecmp(szRouterFile, "screen") == 0)
		{
			pWCRouter->cDestination = WCRouter_ToScreen;

			//Generate temporary filename.
			bResult = GenerateTempFileName(&pWCRouter->szRouterFileName);
			if(bResult == FALSE)
			{
				ProcessErrorCode("ROUT0001", szRouterFile, 'n', 'n');
				return(-1);
			}
		}
		else
		{
			pWCRouter->cDestination = WCRouter_ToFile;

			pWCRouter->szRouterFileName = (char *)malloc(strlen(szRouterFile) + 1);
			if (!pWCRouter->szRouterFileName)
			{
				ProcessErrorCode("ROUT0003", szRouterFile, 'n', 'n');
				return(-1);
			}
			strcpy(pWCRouter->szRouterFileName, szRouterFile);
		}


		//Router request for dribble.
		if(strcasecmp(szRouterTemp, "dribble") == 0)
		{
			bSetRouterResult = DribbleOn(pWCRouter->szRouterFileName);
			if(bSetRouterResult == FALSE) //Error, process error and end program
			{
				ProcessErrorCode("DRBL0002", szRouterFile, 'n', 'n');
				return(-1);
			}
			pWCRouter->cRouterType = WCRouter_Dribble;
		}
		else
		{
			if((pWCRouter->fp = fopen(pWCRouter->szRouterFileName, "wb")) == NULL)
			{
				ProcessErrorCode("ROUT0003", pWCRouter->szRouterFileName, 'n', 'n');
				return(FALSE);
			}
			pWCRouter->cRouterType = WCRouter_Userdefined;
		}

		//System-defined router. Set priority appropriately
		if(strcasecmp(szRouterTemp, "wclips") == 0  || strcasecmp(szRouterTemp, "wtrace") == 0 ||
		   strcasecmp(szRouterTemp, "wagenda") == 0 || strcasecmp(szRouterTemp, "stdout") == 0 ||
		   strcasecmp(szRouterTemp, "wdialog") == 0 || strcasecmp(szRouterTemp, "wwarning") == 0 ||
		   strcasecmp(szRouterTemp, "werror") == 0  || strcasecmp(szRouterTemp, "wdisplay") == 0)
		   iMaxPriority = 20;

		//Get next router
		p = &p[strlen(p) + 1];
		p = strtok(p, "|");

		//If there is another router to process, then allocate space for it
		if(p != NULL)
		{
			pWCRouter->pNextRouter = (struct WebCLIPSRouter *)calloc(1, sizeof(struct WebCLIPSRouter));
			pWCRouter = pWCRouter->pNextRouter;
		}

	} // while(p)

	if(AddRouter(USERDEFINED_ROUTER, iMaxPriority, UDFWCQuery, UDFWCPrint, NULL, NULL, UDFWCExit) == 0)
	{
		ProcessErrorCode("ROUT0005", "User-Defined WebCLIPS Routers", 'n', 'n');
		return(-1);
	}

	return(0);
}

int	UDFWCQuery(char *szRouterName)
{
	struct WebCLIPSRouter *pWCRouter;

	//Traverse linked list to see if we want this output 
	pWCRouter = g_pUDFWebCLIPSRouter;
	while(pWCRouter)
	{
		if(strcmp(szRouterName, pWCRouter->szRouter) == 0)
			return 1;

		//Check next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	return 0;
}

int UDFWCPrint(char *szRouterName, char *strRouterOutput)
{
	struct WebCLIPSRouter *pWCRouter;

	//Traverse linked list to see if we want this output 
	pWCRouter = g_pUDFWebCLIPSRouter;
	while(pWCRouter)
	{
		//Since we know that for standard router we only capture the output one
		//	as soon as we find it ... we're done
		if(strcmp(szRouterName, pWCRouter->szRouter) == 0)
		{
			fprintf(pWCRouter->fp, "%s", strRouterOutput);
			return 1;
		}

		//Check next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	return 0;
}

int	UDFWCExit(int iExitCode)
{
	//Close all files for the routers
	struct WebCLIPSRouter *pWCRouter;

	//Traverse linked list to see if we want this output 
	pWCRouter = g_pUDFWebCLIPSRouter;
	while(pWCRouter)
	{
		fclose(pWCRouter->fp);
		pWCRouter = pWCRouter->pNextRouter;
	}

	return 0;
}

int InitializeStdCLIPSRouters(void)
{
	struct WebCLIPSRouter *pWCRouter;

	//Create a linked list of WebCLIPS routers
	g_pStdWebCLIPSRouter = (struct WebCLIPSRouter *)calloc(1, sizeof(struct WebCLIPSRouter));
	pWCRouter = g_pStdWebCLIPSRouter;

	// stdout
	//Populate this router for stdout
	if(CreateStdWebCLIPSRouter(pWCRouter, "stdout") == FALSE)
	{
		ProcessErrorCode("ROUT0005", "stdout", 'n', 'n');
		return(-1);
	}
	pWCRouter->pNextRouter = (struct WebCLIPSRouter *)calloc(1, sizeof(struct WebCLIPSRouter));
	pWCRouter = pWCRouter->pNextRouter;

	// wdialog
	//Populate this router for wdialog
	if(CreateStdWebCLIPSRouter(pWCRouter, "wdialog") == FALSE)
	{
		ProcessErrorCode("ROUT0005", "wdialog", 'n', 'n');
		return(-1);
	}
	pWCRouter->pNextRouter = (struct WebCLIPSRouter *)calloc(1, sizeof(struct WebCLIPSRouter));
	pWCRouter = pWCRouter->pNextRouter;

	// wwarning
	//Populate this router for wwarning
	if(CreateStdWebCLIPSRouter(pWCRouter, "wwarning") == FALSE)
	{
		ProcessErrorCode("ROUT0005", "wwarning", 'n', 'n');
		return(-1);
	}
	pWCRouter->pNextRouter = (struct WebCLIPSRouter *)calloc(1, sizeof(struct WebCLIPSRouter));
	pWCRouter = pWCRouter->pNextRouter;

	// werror
	//Populate this router for werror
	if(CreateStdWebCLIPSRouter(pWCRouter, "werror") == FALSE)
	{
		ProcessErrorCode("ROUT0005", "werror", 'n', 'n');
		return(-1);
	}

	//Add a router to catch output from the 'standard' CLIPS routers
	if(AddRouter(STANDARD_ROUTER, 20, StdWCQuery, StdWCPrint, NULL, NULL, StdWCExit) == 0)
	{
		ProcessErrorCode("ROUT0005", "Standard WebCLIPS Routers", 'n', 'n');
		return(-1);
	}

	return(0);
}

int	StdWCQuery(char *szRouterName)
{
	struct WebCLIPSRouter *pWCRouter;

	//Traverse linked list to see if we want this output 
	pWCRouter = g_pStdWebCLIPSRouter;
	while(pWCRouter)
	{
		if(strcmp(szRouterName, pWCRouter->szRouter) == 0)
			return 1;

		//Check next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	return 0;
}

int StdWCPrint(char *szRouterName, char *strRouterOutput)
{
	struct WebCLIPSRouter *pWCRouter;

	//Traverse linked list to see if we want this output 
	pWCRouter = g_pStdWebCLIPSRouter;
	while(pWCRouter)
	{
		//Since we know that for standard router we only capture the output one
		//	as soon as we find it ... we're done
		if(strcmp(szRouterName, pWCRouter->szRouter) == 0)
		{
			fprintf(pWCRouter->fp, "%s", strRouterOutput);
			return 1;
		}

		//Check next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	return 0;
}

int	StdWCExit(int iExitCode)
{
	//Close all files for the routers
	struct WebCLIPSRouter *pWCRouter;

	//Traverse linked list to see if we want this output 
	pWCRouter = g_pStdWebCLIPSRouter;
	while(pWCRouter)
	{
		fclose(pWCRouter->fp);
		pWCRouter = pWCRouter->pNextRouter;
	}

	return 0;
}

//Close all open routers. If router type is system, skip it.
//	If the router type is 'dribble' then close the dribble file.
//	If the router type is 'user', the issue a '(close <router>)' command.
int	CloseAllRouters(void)
{
	struct WebCLIPSRouter *pWCRouter;

	//Close the standard routers first
	pWCRouter = g_pStdWebCLIPSRouter;
	while(pWCRouter)
	{
		if(pWCRouter->cRouterType == WCRouter_System || pWCRouter->cRouterType == WCRouter_Userdefined)
		{
			fclose(pWCRouter->fp);
			pWCRouter->fp = NULL;
		}

		if(pWCRouter->cRouterType == WCRouter_Dribble)
		{
			if(DribbleOff() == 0)
			{
				ProcessErrorCode("DRBL0003", pWCRouter->szRouterFileName, 'n', 'n');
				return(-1);
			}
		}

		//Next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	//Now do the User defined routers
	pWCRouter = g_pUDFWebCLIPSRouter;
	while(pWCRouter)
	{
		if(pWCRouter->cRouterType == WCRouter_System || pWCRouter->cRouterType == WCRouter_Userdefined)
		{
			fclose(pWCRouter->fp);
			pWCRouter->fp = NULL;
		}

		if(pWCRouter->cRouterType == WCRouter_Dribble)
		{
			if(DribbleOff() == 0)
			{
				ProcessErrorCode("DRBL0003", pWCRouter->szRouterFileName, 'n', 'n');
				return(-1);
			}
		}

		//Next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	return(0);
}

//This function loops through all the defined routers targeted for 'screen'
//	echoing and prints it to the screen.
int DisplayScreenRouters(void)
{
	struct WebCLIPSRouter *pWCRouter;

	int iErrCode;
	char *szRouterOutput, szWCErrorCode[9], cStdError;
	int bEmptyFileOK = FALSE;
	char szErrorMsg[4096];

	//Close the standard routers first
	pWCRouter = g_pStdWebCLIPSRouter;
	while(pWCRouter)
	{
		if(pWCRouter->cDestination == WCRouter_ToScreen)
		{
			iErrCode = ReadCompleteFile(pWCRouter->szRouterFileName, &szRouterOutput);
			//Check for file I/O problem
			if(iErrCode != 0)
			{
				//Check Error Code
				switch (iErrCode)
				{
					case -1 :   // Open Error
						if((strcasecmp(pWCRouter->szRouter, "stdout") == 0 ||
						    strcasecmp(pWCRouter->szRouter, "wdialog") == 0 ||
						    strcasecmp(pWCRouter->szRouter, "wwarning") == 0 ||
						    strcasecmp(pWCRouter->szRouter, "werror") == 0) &&
						    errno == 2)
						{
							bEmptyFileOK = TRUE;
						    break; //Empty file -- this is OK
						}

						strcpy(szWCErrorCode, "ROUT0007");
						cStdError = 'y';
						break;

					case -2 :  //Memory Allocation Error
						strcpy(szWCErrorCode, "ROUT0008");
						cStdError = 'n';
						break;

					case -3 :  //Read Error
						strcpy(szWCErrorCode, "ROUT0009");
						cStdError = 'y';
						break;

					case -4 : //Empty File
						bEmptyFileOK = TRUE;
						break;
				}

				if(bEmptyFileOK == FALSE)
				{
					strcpy(szErrorMsg, "Router : ");
					strcat(szErrorMsg, pWCRouter->szRouter);
					strcat(szErrorMsg, " File : ");
					strcat(szErrorMsg, pWCRouter->szRouterFileName);
					ProcessErrorCode(szWCErrorCode, szErrorMsg, 'n', cStdError);
					return(-1);
				}
			}
			else //iErrCode == 0
			{
				if(strcasecmp(pWCRouter->szRouter, "stdout") == 0) //Just print out "stdout"
				{
					printf("%s", szRouterOutput);
				}
				else //Print the name of the router
				{
					
					printf("<hr><strong><center>%s</center></strong><p>", pWCRouter->szRouter);
					printf("<pre>%s</pre>", szRouterOutput);
				}

				free(szRouterOutput);
	
				//Remove the temporary file associated with router for screen
				iErrCode = remove(pWCRouter->szRouterFileName);
				if(iErrCode != 0)
				{
					ProcessErrorCode("ROUT0010", pWCRouter->szRouterFileName, 'n', 'y');
					return(-1);
				}
			}
		}

		//Next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	//Now do the User defined routers
	pWCRouter = g_pUDFWebCLIPSRouter;
	while(pWCRouter)
	{
		if(pWCRouter->cDestination == WCRouter_ToScreen)
		{
			iErrCode = ReadCompleteFile(pWCRouter->szRouterFileName, &szRouterOutput);
			//Check for file I/O problem
			if(iErrCode != 0)
			{
				//Check Error Code
				switch (iErrCode)
				{
					case -1 :   // Open Error
						if((strcasecmp(pWCRouter->szRouter, "stdout") == 0 ||
						    strcasecmp(pWCRouter->szRouter, "wdialog") == 0 ||
						    strcasecmp(pWCRouter->szRouter, "wwarning") == 0 ||
						    strcasecmp(pWCRouter->szRouter, "werror") == 0) &&
						    errno == 2)
						{
							bEmptyFileOK = TRUE;
						    break; //Empty file -- this is OK
						}

						strcpy(szWCErrorCode, "ROUT0007");
						cStdError = 'y';
						break;

					case -2 :  //Memory Allocation Error
						strcpy(szWCErrorCode, "ROUT0008");
						cStdError = 'n';
						break;

					case -3 :  //Read Error
						strcpy(szWCErrorCode, "ROUT0009");
						cStdError = 'y';
				}

				if(bEmptyFileOK == FALSE)
				{
					strcpy(szErrorMsg, "Router : ");
					strcat(szErrorMsg, pWCRouter->szRouter);
					strcat(szErrorMsg, " File : ");
					strcat(szErrorMsg, pWCRouter->szRouterFileName);
					ProcessErrorCode(szWCErrorCode, szErrorMsg, 'n', cStdError);
					return(-1);
				}
			}
			else //iErrCode == 0
			{
				printf("<hr><strong><center>%s</center></strong><p>", pWCRouter->szRouter);
				printf("<pre>%s</pre>", szRouterOutput);
	
				free(szRouterOutput);
	
				//Remove the temporary file associated with router for screen
				iErrCode = remove(pWCRouter->szRouterFileName);
				if(iErrCode != 0)
				{
					ProcessErrorCode("ROUT0010", pWCRouter->szRouterFileName, 'n', 'y');
					return(-1);
				}
			}
		}

		//Next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	return(0);
}

//Dump the output of all screen routers. NO Error processing
void FlushAllRouters(void)
{
	int iErrCode;
	char *szRouterOutput;

	struct WebCLIPSRouter *pWCRouter;

	//Close the standard routers first
	CloseAllRouters();

	//Loop through all routers
	pWCRouter = g_pStdWebCLIPSRouter;
	while(pWCRouter)
	{
		if(pWCRouter->cDestination == WCRouter_ToScreen)
		{
			iErrCode = ReadCompleteFile(pWCRouter->szRouterFileName, &szRouterOutput);

			//Check for file I/O problem
			if(iErrCode == 0)
			{
				printf("<hr><strong><center>Flushing Router : %s</center></strong><p>", pWCRouter->szRouter);
				printf("<pre>%s</pre>", szRouterOutput);
				free(szRouterOutput);
			}

			//Remove the temporary file associated with router for screen
			remove(pWCRouter->szRouterFileName);
		}

		//Next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	//Now do the User defined routers
	pWCRouter = g_pUDFWebCLIPSRouter;
	while(pWCRouter)
	{
		if(pWCRouter->cDestination == WCRouter_ToScreen)
		{
			iErrCode = ReadCompleteFile(pWCRouter->szRouterFileName, &szRouterOutput);

			//Check for file I/O problem
			if(iErrCode == 0)
			{
				printf("<hr><strong><center>Flushing Router : %s</center></strong><p>", pWCRouter->szRouter);
				printf("<pre>%s</pre>", szRouterOutput);
				free(szRouterOutput);
			}

			//Remove the temporary file associated with router for screen
			remove(pWCRouter->szRouterFileName);
		}

		//Next router
		pWCRouter = pWCRouter->pNextRouter;
	}
}

int CreateStdWebCLIPSRouter(struct WebCLIPSRouter *pWCRouter, char *szRouter)
{
	int bResult;

	//Make space for the name of router being listened to
	pWCRouter->szRouter = (char *)malloc(strlen(szRouter) + 1);
	if(pWCRouter->szRouter == NULL)
	{
		ProcessErrorCode("ROUT0005", szRouter, 'n', 'n');
		return(FALSE);
	}
	strcpy(pWCRouter->szRouter, szRouter);

	pWCRouter->cDestination = WCRouter_ToScreen;

	//Screen output requires a temporary filename
	bResult = GenerateTempFileName(&pWCRouter->szRouterFileName);
	if(bResult == FALSE)
	{
		ProcessErrorCode("ROUT0004", szRouter, 'n', 'n');
		return(FALSE);
	}

	pWCRouter->cRouterType = WCRouter_System;

	if((pWCRouter->fp = fopen(pWCRouter->szRouterFileName, "wb")) == NULL)
	{
		ProcessErrorCode("ROUT0005", szRouter, 'n', 'n');
		return(FALSE);
	}

	return(TRUE);
}
