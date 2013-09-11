//-------------------------------------------------------------------
//- WebCLIPS Core functions                                         -
//-                                                                 -
//- This module contains the core functions that drive WebCLIPS.    -
//-                                                                 -
//- Source file : wccore.cpp                                        -
//-                                                                 -
//- Who           |   Date    | Desc                                -
//- --------------+-----------+------------------------------------ -
//- M.Giordano    | 09-Dec-97 | Create                              -
//-               |           |                                     -
//- M.Giordano    | 18-Dec-97 | Added logic to PreserveFactState    -
//-               |           |   to now allow facts saved to disk  -
//-               |           |                                     -
//- M.Giordano    | 09-Jan-98 | Added logic to PreserveFactState    -
//-               |           |   to exit if there are no facts to  -
//-               |           |   save.                             -
//-               |           |                                     -
//- M.Giordano    | 09-Feb-98 | Converted to run as an MFC App      -
//-               |           |                                     -
//- M.Giordano    | 10-Feb-98 | Expanded ODBC error reporting       -
//-               |           |                                     -
//- M.Giordano    | 16-Jun-99 | Updated PreserveFactState to strip  -
//-               |           | extra LF chars from fuzzyCLIPS.     -
//-               |           |                                     -
//- M.Giordano    | 08-Nov-99 | Ported to UNIX                      -
//-               |           |                                     -
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

//This function takes the webserver data and classifies facts into
//	their respectie categories (scripts, ScreenName etc.)
int	ClassifyWebServerData(void)
{
	register char *pStart;
	char *pEnd;
	char **pDataType = g_strScreenDataType;
	char **pData = g_strScreenData;

	//
	// use Case INSENSITIVE compares for classification
	//
	//Run through webserver data and classify it
	//Look for 'facts' in input from Web Server
	while(*pDataType)
	{
		if(strcasecmp(*pDataType, "fact") == 0)
		{
			//Look for 'ScreenName' in facts from screen.
			pStart = strstr(*pData, "ScrnName ");
			if(pStart)
			{
				pStart = strchr(*pData, ' ');
				pStart++;	//Get past the space
				pStart = strchr(pStart, ' ');
				pStart++;
				strcpy(g_szScreenName, pStart);
				pEnd = strpbrk(g_szScreenName, "))");
				*pEnd = NULL;
				pDataType++;
				pData++;
				continue;
			}

			//Look for 'script' in facts from screen.
			//Overwrite the DataType so as not to assert it again
			pStart = strstr(*pData, "script");
			if(pStart)
			{
				pStart = strchr(pStart, ' ');
				pStart++;
				*pData = pStart;
				strtok(pStart, ")");
				strcpy(*pDataType, FACT_TYPE_SCRIPT);
				pDataType++;
				pData++;
				continue;
			}

			//Look for 'factgroup' in facts from screen.
			//Overwrite the DataType so as not to assert it again
			pStart = strstr(*pData, "factgroup");
			if(pStart)
			{
				pStart = strchr(pStart, ' ');
				pStart++;
				*pData = pStart;
				strtok(pStart, ")");
				strcpy(*pDataType, FACT_TYPE_FACTGROUP);
				pDataType++;
				pData++;
				continue;
			}

			//Look for 'binimage' in facts from screen. If one is found,
			//	update the Data Type to "binimage" for later processing.
			//	This type of fact uses the BLoad function to load a 
			//	binary image file.
			//Overwrite the DataType so as not to assert it again
			pStart = strstr(*pData, "binimage");
			if(pStart)
			{
				pStart = strchr(pStart, ' ');
				pStart++;
				*pData = pStart;
				strtok(pStart, ")");
				strcpy(*pDataType, FACT_TYPE_BINIMAGE);
				pDataType++;
				pData++;
				continue;
			}

		}  // if(strcasecmp(pDataType, "fact") == 0)
	
		//Next web server data element
		pData++;
		pDataType++;

	} //while(*pDataType)

	//If ScreenName is not found, send a message and terminate
	if(strlen(g_szScreenName) == 0)
	{
		ProcessErrorCode("WBIO0005", NULL, 'n', 'n');
		return(-1);
	}

	return(0);
}

//This function asserts all facts from screen input and WEBCLIPS.INI
int ProcessAllFacts(void)
{
	char **pDataType = g_strScreenDataType;
	char **pData = g_strScreenData;
	char **pData2ndHalfType;
	char **pData2ndHalf;
	unsigned int i, iFactCount = 1;
	char szFactFormatBuffer[24], szFactData[FACT_BUFFER_SIZE + 1];

	//First Assert all complete facts from the screen
	while(*pDataType)
	{
		if(strcasecmp(*pDataType, "fact") == 0)
		{
			if(AssertString(*pData) == NULL)
			{
				ProcessErrorCode("IFCT0001", *pData, 'n', 'n');
				return(-1);
			}

			pData++;
			pDataType++;
			continue;
		}

		//Look for split up facts in the web server data -- Loop through fact names
		sprintf(szFactFormatBuffer, "factname%d", iFactCount);
		if(strcasecmp(*pDataType, szFactFormatBuffer) == 0)
		{
			strcpy(szFactData, "(");
			strcat(szFactData, *pData);

			//If fact name is found, then look for corresponding
			//	fact value data item from screen
			sprintf(szFactFormatBuffer, "factvalue%d", iFactCount);
			pData2ndHalfType = g_strScreenDataType;
			pData2ndHalf = g_strScreenData;
			while(*pData2ndHalfType)
			{
				//Fact Value data item is found, assert it.
				if(strcasecmp(*pData2ndHalfType, szFactFormatBuffer) == 0)
				{
					strcat(szFactData, " ");
					strcat(szFactData, *pData2ndHalf);
					strcat(szFactData, ")");

					if(AssertString(szFactData) == NULL)
					{
						strcat(szFactData, " for ");
						strcat(szFactData, *pData2ndHalfType);
						ProcessErrorCode("IFCT0003", szFactData, 'n', 'n');
						return(-1);
					}

					//Fact assertion successful, get out and look for the next fact.
					break;
				}
				pData2ndHalfType++;
				pData2ndHalf++;
			}

			//Bump the fact name count.
			iFactCount++;
			continue;
		}

		pData++;
		pDataType++;
		continue;
	}	//for

	//Next Assert all facts from WebCLIPS.INI. Stop looking when the
	//sequence is broken.
	i = 1;
	while(TRUE)
	{
		sprintf(szFactFormatBuffer, "Fact%d", i);
		//Get the FactN entry from WebCLIPS.INI, exit if not found
		GetWebCLIPSSettings(
			g_szScreenName,         // section name
			szFactFormatBuffer,     // entry name
			"",                     // default value
			szFactData,				// return value
			sizeof(szFactData),		// max length
			g_szWebCLIPSINI
			);

		if(strlen(szFactData) == 0)
			break;

		//Assert the fact
		if(AssertString(szFactData) == NULL)
		{
			ProcessErrorCode("IFCT0002", szFactData, 'n', 'n');
			return(-1);
		}

		//Move on to the next fact.
		i++;
	}

	return(0);
}

//Function to write all facts as hidden <INPUT> items, if desired.
int PreserveFactState(char *szPrevScreen)
{
	struct fact * factptr;
	char szCurrFact[FACT_BUFFER_SIZE + 1];
	char *pStart;

	int bPreserveFactsUsingTags = FALSE, bPreserveFactsUsingDisk = FALSE;
	int bPreserveFactsUsingCookie = FALSE, bResult;
	char szKeepFacts[7], *szFileName;
	FILE *fp;

	//Determine if *previous* screen wants to preserve ALL facts for
	//	further processing.
	GetWebCLIPSSettings(
		szPrevScreen,				// section name
		"PreserveFacts",			// entry name
		"no",						// default value
		szKeepFacts,				// return value
		sizeof(szKeepFacts),		// max length
		g_szWebCLIPSINI
		);

	//Set bPreserveFact flags appropriately
	if(strcasecmp(szKeepFacts, "yes") == 0)
	{
		bPreserveFactsUsingTags = TRUE;
	}

	if(strcasecmp(szKeepFacts, "disk") == 0 ||
	   strcasecmp(szKeepFacts, "cookie") == 0)
	{
		//We are preserving facts for just this session
		if(strcasecmp(szKeepFacts, "disk") == 0)
		{
			bPreserveFactsUsingDisk = TRUE;

			//Generate unique filename
			bResult = GenerateTempFileName(&szFileName);
			if(bResult == FALSE)
			{
				ProcessErrorCode("PSRV0001", NULL, 'y', 'n');
				return(-1);
			}
		}

		//We are preserving facts across sessions
		if(strcasecmp(szKeepFacts, "cookie") == 0)
		{
			bPreserveFactsUsingCookie = TRUE;
			szFileName = g_szSaveCookiesFile;
		}

		//Open the file
		if((fp = fopen(szFileName, "wb")) == NULL)
		{
			free(szFileName);
			ProcessErrorCode("PSRV0002", szFileName, 'y', 'y');
			return(-1);
		}
	}

	//For each fact preserve it in the appropriate manner
	factptr = GetNextFact(NULL);
	while(factptr)
	{
		memset(szCurrFact, '\0', FACT_BUFFER_SIZE + 1);
		GetFactPPForm(szCurrFact, FACT_BUFFER_SIZE, factptr);

		pStart = strchr(szCurrFact, '(');
		//Skip (initial-fact)
		if(strcmp(szCurrFact, "(initial-fact)") == 0)
		{
			factptr = GetNextFact(factptr);
			continue;
		}

		//Whether or not fact preservation is desired, the "ScreenName"
		//	fact must be kept so WebCLIPS knows what to do next time.
		if(strstr(pStart, "(ScreenName ") != NULL) //ScreenName found!
		{
			printf("<INPUT TYPE=\"hidden\" NAME=\"fact\" VALUE=\"%s\">\n", pStart);
			factptr = GetNextFact(factptr);
			continue;
		}

		//If fact preservation is desired and the facts are to be
		//	preserved using <hidden> HTML tags then, the keep the fact.
		if(bPreserveFactsUsingTags == TRUE)
		{
			printf("<INPUT TYPE=\"hidden\" NAME=\"fact\" VALUE=\"");
			PrintURLEncode(pStart);
			printf("\">\n");
			factptr = GetNextFact(factptr);
			continue;
		}

		//If we want to save facts using disk then write the fact to
		//	to the file.
		if(bPreserveFactsUsingDisk == TRUE ||
		   bPreserveFactsUsingCookie == TRUE)
		{
			fprintf(fp, "%s\n", pStart);
			factptr = GetNextFact(factptr);
			continue;
		}

		factptr = GetNextFact(factptr);
	} // while(factptr)

	//Close the file, format <hidden> tag with filename
	if(bPreserveFactsUsingDisk == TRUE)
	{
		//Write out a <hidden> tag containing the filename for the next
		//	program
		printf("<INPUT type=\"hidden\" name=\"WC_SavedFacts\" value=\"%s\">", szFileName);
		free(szFileName); //Free up space from GenerateTempFileName()
		fclose(fp);
	}

	//Close the file
	if(bPreserveFactsUsingCookie == TRUE)
	{
		fclose(fp);
	}

	return(0);
}

//Look through the facts from the previous CLIPS run and determine
//	the screen name for the next run.
int UpdateScreenName(void)
{
	struct fact * factptr;
	char szCurrFact[FACT_BUFFER_SIZE + 1];
	char *pStart, *pEnd;

	char szCurrentScreen[] = "(ScreenName ";
	char szNextScreenINI[MAX_SCREEN_NAME_SIZE + 1];

	//Start at first fact.
	factptr = GetNextFact(NULL);
	while(factptr)
	{
		//Look for ScreenName, copy to g_szScreenName if found and get out
		memset(szCurrFact, '\0', FACT_BUFFER_SIZE + 1);
		GetFactPPForm(szCurrFact, FACT_BUFFER_SIZE, factptr);
		pStart = strchr(szCurrFact, '(');
		if(strstr(pStart, szCurrentScreen) != NULL)	//Got the screen name
		{
			pStart = strchr(pStart, ' ');	//Get past (ScreenName
			pStart++;						//Get past space
			pStart = strchr(pStart, ' ');	//Get past (ScrnName
			pStart++;
			pEnd = strstr(pStart, "))");
			*pEnd = NULL;
			strcpy(g_szScreenName, pStart);
			return(0);
		}

		//Next fact
		factptr = GetNextFact(factptr);
	}

	//If we get here, that means that No ScreenName fact was asserted
	//   and we must check to see if there is a screen specified in WebCLIPS.INI
	GetWebCLIPSSettings(
		g_szScreenName,				// section name
		"NextScreenName",			// entry name
		"",							// default value
		szNextScreenINI,			// return value
		sizeof(szNextScreenINI),	// max length
		g_szWebCLIPSINI
		);

	//If there is an entry, copy it to g_szScreenName and assert a fact
	//	reflecting the new ScreenName
	if(strlen(szNextScreenINI) > 0)
	{
		strcpy(g_szScreenName, szNextScreenINI);
		memset(szCurrFact, '\0', FACT_BUFFER_SIZE + 1);
		strcpy(szCurrFact, "(ScreenName (ScrnName ");
		strcat(szCurrFact, g_szScreenName);
		strcat(szCurrFact, "))");
		if(AssertString(szCurrFact) == NULL)
		{
			ProcessErrorCode("NAVG0001", szCurrFact, 'n', 'n');
			return(-1);
		}
	}

	return(0);
}

//Load the file containing WebCLIPS HTML helper functions and
//	deftemplates for ODBC queries and Screen Names
int	LoadHelper(void)
{
	char szLoadHelper[4];
	char szFileName[MAXNAMLEN + 1];
	char szWCErrorCode[9];
	int iErrCode;

	//First load the deftemplates, get the file path/name from
	//	WebCLIPS.INI
	GetWebCLIPSSettings(
		"System",					// section name
		"Deftemplates",				// entry name
		"",							// default value
		szFileName,					// return value
		sizeof(szFileName),			// max length
		g_szWebCLIPSINI
		);
	iErrCode = Load(szFileName);
	if(iErrCode != LOAD_OK)
	{
		switch (iErrCode)
		{
			case LOAD_FILE_IO_ERROR :
				strcpy(szWCErrorCode, "DFTM0001");
				break;
			case LOAD_PARSE_ERROR :
				strcpy(szWCErrorCode, "DFTM0004");
		}
		ProcessErrorCode(szWCErrorCode, szFileName, 'n', 'n');
		return(-1);
	}

	//Next, determine if screen wants it loaded
	GetWebCLIPSSettings(
		g_szScreenName,				// section name
		"LoadHelper",				// entry name
		"",							// default value
		szLoadHelper,				// return value
		sizeof(szLoadHelper),		// max length
		g_szWebCLIPSINI
		);

	//Helper not wanted, exit
	if(strcasecmp(szLoadHelper, "yes") != 0)
		return(0);

	//Get the name of theHTML helper file.
	GetWebCLIPSSettings(
		"System",					// section name
		"HelperFileName",			// entry name
		"",							// default value
		szFileName,					// return value
		sizeof(szFileName),			// max length
		g_szWebCLIPSINI
		);

	iErrCode = Load(szFileName);
	if(iErrCode != LOAD_OK)
	{
		switch (iErrCode)
		{
			case LOAD_FILE_IO_ERROR :
				strcpy(szWCErrorCode, "HTML0001");
				break;
			case LOAD_PARSE_ERROR :
				strcpy(szWCErrorCode, "HTML0004");
				break;
		}
		ProcessErrorCode(szWCErrorCode, szFileName, 'n', 'n');
		return(-1);
	}

	return(0);
}

//Process all Scripts from WebCLIPS.INI and previously asserted
//   facts.
int ProcessAllScripts(void)
{
	char szFileName[MAXNAMLEN + 1], szScriptFormat[24];
	char **pDataType = g_strScreenDataType;
	char **pData = g_strScreenData;
	unsigned int i;

	//First Load any scripts from previously asserted facts.
	while(*pDataType)
	{
		if(strcasecmp(*pDataType, FACT_TYPE_SCRIPT) == 0)
		{
			//Load for 'script' in facts from screen.
			if(LoadCLIPSScript(*pData) != 0)
			{
				return(-1);
			}

			g_bScriptFound = TRUE;
		}

		pDataType++;
		pData++;
	}

	//Next, loop through and look for all scripts from WebCLIPS.INI
	//	Stop when the sequence is broken.
	i = 1;
	while(TRUE)
	{
		sprintf(szScriptFormat, "script%d", i);

		//Get name of CLIPS script from WebCLIPS.ini
		GetWebCLIPSSettings(
			g_szScreenName,					// section name
			szScriptFormat,					// entry name
			"",								// default value
			szFileName,						// return value
			sizeof(szFileName),				// max length
			g_szWebCLIPSINI
			);

		//No more scripts
		if(strlen(szFileName) == 0)
			break;

		//Load the script, exit if error.
		if(LoadCLIPSScript(szFileName) != 0)
		{
			return(-1);
		}
		g_bScriptFound = TRUE;
		
		//Get next script
		i++;
	}

	return(0);
}

//Performs the actual load of CLIPS script
int LoadCLIPSScript(char *szFileName)
{
	int iErrCode;
	char szWCErrorCode[9];

	//Load the CLIPS script(facts/rules etc.)
	//If Load fails, display message to browser.
	iErrCode = Load(szFileName);
	if(iErrCode != LOAD_OK)
	{
		switch (iErrCode)
		{
			case LOAD_FILE_IO_ERROR :
				strcpy(szWCErrorCode, "SCPT0002");
				break;
			case LOAD_PARSE_ERROR :
				strcpy(szWCErrorCode, "SCPT0003");
				break;
		}
		ProcessErrorCode(szWCErrorCode, szFileName, 'n', 'n');
		return(-1);
	}

	return(0);
}

//Process one Binary Image file from either WebCLIPS.INI or previously 
//	asserted facts.
int ProcessBinImage(void)
{
	char szBinFileName[MAXNAMLEN + 1];
	char **pDataType = g_strScreenDataType;
	char **pData = g_strScreenData;

	//First if a binary image file is present in previously asserted facts,
	//	then BLoad it and get out.
	while(*pDataType)
	{
		if(strcasecmp(*pDataType, FACT_TYPE_BINIMAGE) == 0)
		{
			//Look for any Binary Image files in facts
			if(BLoadCLIPSScript(*pData) != 0)
			{
				return(-1);
			}
			else
			{
				return(0);
			}
		}

		pDataType++;
		pData++;
	}

	//Next, look to see if there is a Binary Image file in WebCLIPS.INI
	//	If there is, then BLoad it and exit.

	//Get name of CLIPS Binary Image file from WebCLIPS.ini
	GetWebCLIPSSettings(g_szScreenName,	
						"binimage",	
						"", 
						szBinFileName, 
						sizeof(szBinFileName), 
						g_szWebCLIPSINI);

	//If WebCLIPS can't find either a script or a Binary Image file
	//	to run.
	if(strlen(szBinFileName) == 0 && g_bScriptFound == FALSE)
	{
		ProcessErrorCode("SCPT0001", NULL, 'n', 'n');
		return(-1);
	}
	
	//WebCLIPS did not find a Binary Image file to load but it did
	//	find a script to execute, then exit
	if(strlen(szBinFileName) == 0)
		return(0);

	//BLoad the script, exit if error.
	if(BLoadCLIPSScript(szBinFileName) != 0)
	{
		return(-1);
	}
		
	return(0);
}

//Performs the actual BLoad of CLIPS script
int	BLoadCLIPSScript(char *szFileName)
{
	int iErrCode;

	//Load the CLIPS Binary Image file(facts/rules etc.)
	//If Load fails, display message to browser.
	iErrCode = Bload(szFileName);
	if(iErrCode != TRUE)
	{
		ProcessErrorCode("BLOD0001", szFileName, 'n', 'n');
		return(-1);
	}

	return(0);
}

//Process all separate factgroups from WebCLIPS.INI and previously asserted
//	facts. Also, if there is an entry (environment) for WC_SavedFacts,
//	then load these facts. These facts were 'preserved' using the 'disk'
//	option from a 'PreserveFacts' entry in WebCLIPS.INI
int ProcessAllFactGroups()
{
	char szFileName[MAXNAMLEN + 1], szFactGroupFormat[24];
	int iErrCode, i;

	//First process any Fact Groups from previously asserted facts.
	char **pDataType = g_strScreenDataType;
	char **pData = g_strScreenData;

	//First if a binary image file is present in previously asserted facts,
	//	then BLoad it and get out.
	while(*pDataType)
	{
		if(strcasecmp(*pDataType, FACT_TYPE_FACTGROUP) == 0)
		{
			//Look for any files containing facts to load
			if(LoadCLIPSFactGroup(*pData) != 0)
			{
				return(-1);
			}

			pDataType++;
			pData++;
			continue;
		}

		//If there is an entry for WC_SavedFacts, load them. They are from a 
		//	previous screen.
		if(strcasecmp(*pDataType, "wc_savedfacts") == 0)
		{
			if(LoadCLIPSFactGroup(*pData) != 0)
			{
				return(-1);
			}

			//Delete the (temporary) file containing the facts from the Previous screen
			iErrCode = remove(*pData);
			if(iErrCode != 0)
			{
				ProcessErrorCode("GFCT0005", *pData, 'n', 'y');
				return(-1);
			}
		}

		pDataType++;
		pData++;
	}

	//Next, loop through and look for all fact groups from WebCLIPS.INI
	//	Stop when the sequence is broken.
	i = 1;
	while(TRUE)
	{
		sprintf(szFactGroupFormat, "factgroup%d", i);

		//Get filename of CLIPS fact group from WebCLIPS.ini
		GetWebCLIPSSettings(
			g_szScreenName,					// section name
			szFactGroupFormat,				// entry name
			"",								// default value
			szFileName,						// return value
			sizeof(szFileName),				// max length
			g_szWebCLIPSINI
			);

		//No more factgroups.
		if(strlen(szFileName) == 0)
			break;

		//Load the factgroup, exit if error.
		if(LoadCLIPSFactGroup(szFileName) != 0)
		{
			return(-1);
		}
		
		//Get next factgroup
		i++;
	}

	//Look in 'cookie' to see if there are facts to load.
	//Let the function process any errors. If there is an error, then just exit.
	if(GetFactsFileFromCookie() != 0)
	{
		return(-1);
	}

	if(strlen(g_szGetCookiesFile) > 0)
	{
		if(LoadCLIPSFactGroup(g_szGetCookiesFile) != 0)
		{
			return(-1);
		}

		//Eliminate the entry from the cookie
		ExpireCookie();

		//Delete the (temporary) file containing the facts from the Previous screen
		iErrCode = remove(g_szGetCookiesFile);
		if(iErrCode != 0)
		{
			ProcessErrorCode("GFCT0006", g_szGetCookiesFile, 'n', 'y');
			return(-1);
		}
	}

	return(0);
}

//Actual load of facts for CLIPS
int LoadCLIPSFactGroup(char *szFileName)
{
	//Load the Separate Fact base.
	if(LoadFacts(szFileName) == FALSE)
	{
		ProcessErrorCode("GFCT0002", szFileName, 'n', 'n');
		return(-1);
	}

	return(0);
}

//If there is a file containing the HTML layout of the screen, display it.
//	Also, if there is a '</body>' in the file, eliminate it.
int GetScreenDisplay(char **szFileBuffer)
{
	char szFileName[MAXNAMLEN + 1], *pBody, szWCErrorCode[9], cStdError;
	char *pForm;
	int iErrCode;

	//Get file name containing the description of problem from WebCLIPS.ini
	GetWebCLIPSSettings(
        g_szScreenName,					// section name
        "InitialScreen",				// entry name
        "",								// default value
        szFileName,						// return value
        sizeof(szFileName),				// max length
        g_szWebCLIPSINI
        );

	//If InitialScreen not found,then all HTML must have come from CLIPS script(s).
	if(strlen(szFileName) == 0)
	{
		return +1;
	}

	//Read in file containing Input screen
	iErrCode = ReadCompleteFile(szFileName, szFileBuffer);

	//Check Error Code
	if(iErrCode != 0)
	{
		switch (iErrCode)
		{
			case -1 :   // Open Error
				strcpy(szWCErrorCode, "ISCN0001");
				cStdError = 'y';
				break;

			case -2 :  //Memory Allocation Error
				strcpy(szWCErrorCode, "ISCN0002");
				cStdError = 'n';
				break;

			case -3 :  //Read Error
				strcpy(szWCErrorCode, "ISCN0003");
				cStdError = 'y';
		}

		ProcessErrorCode(szWCErrorCode, szFileName, 'n', cStdError);
		return(iErrCode);
	}

	//Eliminate </form> tag. This is to allow facts to be carried through to the next Form
	//	This does assume that for static screens, the last Form on the page will be receiving 
	//	the facts (i.e. <hidden> tags.
	//If no </form> tag is found, then look for an </body> tag.
	FindEndFormTag(*szFileBuffer, &pForm);
	if(pForm != NULL)
	{
		*pForm = '\0';
		return 0;
	}

	//Eliminate </body> and </html> tags. This will allow us to insert other HTML and data tags for later use.
	FindEndBodyTag(*szFileBuffer, &pBody);
	if(pBody != NULL)
		*pBody = '\0';
	else
		printf("&lt/body&gt tag not found");

	return 0;
}

//Search through buffer and find </body>
void FindEndBodyTag(char *szFileBuffer, char **pEndBodyTag)
{
	char szEndBody[] = "</body>";
	unsigned int iEndBodyLen;

	iEndBodyLen = strlen(szEndBody);
	*pEndBodyTag = &szFileBuffer[strlen(szFileBuffer)] - iEndBodyLen;

	while(*pEndBodyTag > szFileBuffer)
	{
		if(strncasecmp(*pEndBodyTag, szEndBody, iEndBodyLen) == 0)
			return;

		(*pEndBodyTag)--;
	}

	*pEndBodyTag = NULL;
}

//Search through buffer and find </form>
void FindEndFormTag(char *szFileBuffer, char **pEndFormTag)
{
	char szEndForm[] = "</form>";
	unsigned int iEndFormLen;

	iEndFormLen = strlen(szEndForm);
	*pEndFormTag = &szFileBuffer[strlen(szFileBuffer)] - iEndFormLen;

	while(*pEndFormTag > szFileBuffer)
	{
		if(strncasecmp(*pEndFormTag, szEndForm, iEndFormLen) == 0)
			return;

		(*pEndFormTag)--;
	}

	*pEndFormTag = NULL;
}


//This function will obtain the Agenda Limit for any particular screen.
//	If the value specified in WebCLIPS.INI is less than zero, -1 will
//	be returned. There is a default limit that is sepcified in the
//	[System] section and can be overridden on a screen-by-screen basis.
long int GetAgendaLimit()
{
	long int lAgendaLimit;
	char szAgendaLimitBuffer[24];
	char szAgendaLimitToken[] = "AgendaLimit";

	//Look to see if there is an override for this particular screen
	GetWebCLIPSSettings(
        g_szScreenName,					// section name
        szAgendaLimitToken,				// entry name
        "",								// default value
        szAgendaLimitBuffer,			// return value
        sizeof(szAgendaLimitBuffer),	// max length
        g_szWebCLIPSINI
        );

	//If a value is present, use it
	if(strlen(szAgendaLimitBuffer) > 0)
	{
		lAgendaLimit = atol(szAgendaLimitBuffer);
		if(lAgendaLimit < 0)
			return(-1L);
		else
			return(lAgendaLimit);
	}

	//Look to see if there is a default Agenda Limit, if there is no
	//	default Agenda limit return -1.
	GetWebCLIPSSettings(
        "System",						// section name
        szAgendaLimitToken,				// entry name
        "-1",							// default value
        szAgendaLimitBuffer,			// return value
        sizeof(szAgendaLimitBuffer),	// max length
        g_szWebCLIPSINI
        );

	//If a value is present, force to -1 if less than -1
	lAgendaLimit = atol(szAgendaLimitBuffer);
	if(lAgendaLimit < 0)
	{
		lAgendaLimit = (long) -1;
	}

	return(lAgendaLimit);
}

//Initializes the CLIPS Engine for the WebCLIPS environment
int Init(void)
{
	//Initialize the appropriate CLIPS Engine (Standard vs. Fuzzy)
	//	DLL Loaded here
	SetPrintWhileLoading(FALSE);
	InitializeEnvironment();
	return 0;
}

//Save the name of the invoking screen for later processing
void SaveScreenName(char *szPrevScreen)
{
	strcpy(szPrevScreen, g_szScreenName);
}

//Issue CLIPS 'run'
void Go(void)
{
	Run(GetAgendaLimit());
}