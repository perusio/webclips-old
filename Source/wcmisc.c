//-------------------------------------------------------------------
//- WebCLIPS Miscellaneous functions                                -
//-                                                                 -
//- This module contains CLIPS specific functions as well as some   -
//- useful functions.                                               -
//-                                                                 -
//- Source file : wcmisc.cpp                                        -
//-                                                                 -
//- Who           |   Date    | Desc                                -
//- --------------+-----------+------------------------------------ -
//- M.Giordano    | 23-Jul-97 | Create                              -
//-               |           |                                     -
//- M.Giordano    | 28-Jul-97 | Added functions to process an       -
//-               |           |   arbitrary number of facts,        -
//-               |           |   scripts and fact groups.          -
//-               |           |                                     -
//- M.Giordano    | 05-Aug-97 | Convert error messages to use       -
//-               |           |   enhanced error reporting.         -
//-               |           |                                     -
//- M.Giordano    | 06-Aug-97 | Added processing for ODBC queries   -
//-               |           |                                     -
//- M.Giordano    | 14-Aug-97 | Added processing for Binary Image   -
//-               |           |   file loading and Added option to  -
//-               |           |   exclude all fact preservation     -
//-               |           |   except for the ScreenName fact.   -
//-               |           |                                     -
//- M.Giordano    | 23-Sep-97 | Changes made to accomodate new      -
//-               |           |   wrapper classes.                  -
//-               |           |                                     -
//- M.Giordano    | 14-Oct-97 | Correction to properly place the    -
//-               |           |   </form> tag for static screens.   -
//-               |           |                                     -
//- M.Giordano    | 29-Oct-97 | Enhancement to allow limit the      -
//-               |           |   number of rules to fire on the    -
//-               |           |   Agenda.                           -
//-               |           |                                     -
//- M.Giordano    | 06-Nov-97 | Small bug cleanup and mods made to  -
//-               |           |   accomoodate long file names.      -
//-               |           |                                     -
//- M.Giordano    | 07-Nov-97 | Bug cleanup to assert a fact for a  -
//-               |           |   new screen name from WebCLIPS.INI -
//-               |           |                                     -
//- M.Giordano    | 09-Dec-97 | Split out router code to separate   -
//-               |           |   file. Split out 'core' functions. -
//-               |           |                                     -
//- M.Giordano    | 09-Feb-98 | Converted to MFC App                -
//-               |           |                                     -
//- M.Giordano    | 29-Oct-99 | Added GetWebCLIPSSettings           -
//-               |           | Modified GenerateTempFileName to    -
//-               |           | use tempnam()                       -
//-               |           |                                     -
//- M.Giordano    | 16-Dec-99 | Eliminated MAX macro                -
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

//This function sets the global variable (used by tempnam) to
//	the entry [System] CLIPSOutput in WebCLIPS.INI
int	SetTempDir(void)
{
	//Get the name of the destination Dir to Generate the
	//	temporary filename.
	GetWebCLIPSSettings(
		"System",				// section name
        "CLIPSOutput",			// entry name
        "",						// default value
        g_szTempPath,			// return value
        sizeof(g_szTempPath),	// max length
        g_szWebCLIPSINI
        );

	if(strlen(g_szTempPath) == 0)	//Entry not found
	{
		ProcessErrorCode("TEMP0001", NULL, 'n', 'n');
		return(-1);
	}

	return(0);
}

//Process all error codes for WebCLIPS. Get short error message,
//	display a link to the full error description screen and
//	display a link to the WebCLIPS home URL if one exists.
void ProcessErrorCode(char *szErrCode, char *szAdditionalData, int cHeader, int cUseStdError)
{
	char szErrorMsg[256];
	char szErrorCategory[5];

	//Check to see if MIME and HTML Header has been printed. If it has
	//	not then print it.
	if(tolower(cHeader) == 'n')
	{

		PrintMIMEHeader();
		PrintHTMLHeader(WEBCLIPS, WEBCLIPS_VERSION);
	}

	//Get short error message
	GetWebCLIPSSettings(
        "Error Messages",				// section name
        szErrCode,						// entry name
        "",								// default value
        szErrorMsg,						// return value
        sizeof(szErrorMsg),				// max length
        g_szWebCLIPSINI
        );

	if(strlen(szErrorMsg) == 0)
	{
		strcpy(szErrorMsg, "Error message <b>NOT FOUND</b>");
		FormatError(szErrCode, szErrorMsg, NULL, 'n');
	}
	else
		FormatError(szErrCode, szErrorMsg, szAdditionalData, cUseStdError);

	//Dump output for all 'screen' routers. Note : this function calls
	//	'FormatError' directly to format any errors encoutered while re-routing
	//	'screen'-type output.
	strncpy(szErrorCategory, szErrCode, 4); //Copy in category (i.e. ROUT)
	szErrorCategory[4] = '\0'; //Terminating null

	//If the error was generated from a function dealing with routers. Then
	//	don't try to display them again. Chances are you'll receive the same error
	//	message again.
	if(strcmp(szErrorCategory, "ROUT") != 0)
	{
		//Dump output for all 'screen' routers. Note : this function calls
		//	'FormatError' directly to format any errors encoutered while re-routing
		//	'screen'-type output.
		FlushAllRouters();
	}

	//Print an HTML Trailer
	PrintHTMLTrailer();
}


//Actual format and display of error messages
void FormatError(char *szErrCode, char *szErrMsg, char *szAdditionalData, int cUseStdError)
{
	char szPath[MAXNAMLEN + 1];

	//Get path to error message file location. If none found assume root
	GetWebCLIPSSettings(
        "System",						// section name
        "ErrorPath",					// entry name
        "/",							// default value
        szPath,							// return value
        sizeof(szPath),					// max length
        g_szWebCLIPSINI
        );

	//Build the complete full description file name
	strcat(szPath, szErrCode);
	strcat(szPath, ".htm");

	printf("<center><b>WebCLIPS</b> %s -- Error Reporting Facility<hr></center>"
			"An error occurred while processing screen : <b><u>%s</u></b><br><br>"
			"The error number is : %s<br>"
			"The error message is : %s", WEBCLIPS_VERSION, g_szScreenName, szErrCode, szErrMsg
		  );

	if(szAdditionalData != NULL)
	{
		printf	(" for %s <br>", szAdditionalData);
	}
	else
	{
		printf	("<br>");
	}

	if(tolower(cUseStdError) == 'y')
	{
		printf("Other error information available : %s<br>", strerror(errno));
	}

	printf("<br>Please click <a href=\"%s\">here</a> for a full description of the problem", szPath);

	//Look for home URL for WebCLIPS
	GetWebCLIPSSettings(
        "System",						// section name
        "HomeURL",						// entry name
        "",								// default value
        szPath,							// return value
        sizeof(szPath),					// max length
        g_szWebCLIPSINI
        );

	if(strlen(szPath) != 0)
	{
		printf("<br><br>Please click <a href=\"%s\">here</a> to go to the "
				 "<b>WebCLIPS</b> Home Screen", szPath);
	}
}

//Generate temporary filename.
int GenerateTempFileName(char **szTempFileName)
{
	//Create temporary name. It is the calling function's responsibility
	//	to 'free' the pointer when it's done.
	if((*szTempFileName = tempnam(g_szTempPath, NULL)) == NULL)
		return FALSE;
	else
		return TRUE;
}

//This function reads the entire contents of a file and returns it
//in a buffer. iErrorCode indicates and errors that occurred during
//processing.
int ReadCompleteFile(const char *szFileName, char **buf)
{
	FILE *fp;
	unsigned long lFileSize, lNumBytesRead;

	//Open the file
	if((fp = fopen(szFileName, "r+b")) == NULL)
	{
		return -1;
	}

	//Set the file pointer to the end
	fseek(fp, 0L, SEEK_END);

	//Get the file size
	lFileSize = ftell(fp);

	//If file is empty -- nothing to read, just get out
	if(lFileSize == 0)
		return -4;

	//Set the pointer back to the beginning
	fseek(fp, 0L, SEEK_SET);

	*buf = NULL;
	*buf = (char *)calloc(1, lFileSize + 1);
	if(!*buf)
	{
		return -2;
	}

	lNumBytesRead = fread(*buf, sizeof(char), lFileSize, fp);
	if(lNumBytesRead != lFileSize)
	{
		free(*buf);
		fclose(fp);
		return -3;
	}
	
	//Close the file
	fclose(fp);
	return 0;
}

/*-

   Function : GetWebCLIPSSettings

   Purpose  : Emulate the Windows NT/95 system function call 
              GetWebCLIPSSettings. Convenient tool for easily
              setting up configuration files to pass parameters to a
              program without the use of the command line or 
              recompiling. By specifying a section desired along with
              an entry name the value immediately following the '=' is
              copied to a buffer supplied by the program.

   Parms    :
              char *SectionName - is the section name where the desired
                                  parameter resides. It is surrounded
                                  by brackets []. The left bracket
                                  must reside in column 1.
                                  See Example INI file layout.

              char *EntryName - is the entry where the desired
                                parameter resides. EntryName must
                                start in column 1.
                                Example INI file layout.

              char *DefaultValue - is a default value supplied by the
                                   program in the case where the
                                   section/entry is not found.

              char *EntryValue - is the pointer to the buffer where the
                                 desired parameter will be copied. If
                                 it is not found, the value pointed to
                                 by DefaultValue will be copied here.

              unsigned int MaxLength - is the maximum length of the 
                                       entry to copy. This useful to 
                                       prevent overflow when copying
                                       entry values from the INI to 
                                       a progam.

              char *INIFileName - is the name of the INI file.

Returns : the length of the string returned

Example INI file layout :

[System]
CLIPSOutput=/websrvr/cgi-bin/clpout.txt

[BaseBall Demo]  
file=/CLIPS/scripts/baseball.clp
desc=/WebCLIPS/descriptions/hwbb.txt
facts=/WebCLIPS/Facts/bbfacts.clp

[Monkey and Bananas Demo]
file=/CLIPS/scripts/webmab.clp
desc=/WebCLIPS/descriptions/Webmab.txt

-*/

int GetWebCLIPSSettings(char *SectionName, char *EntryName, char *DefaultValue,
                            char *EntryValue, unsigned int MaxLength, char *INIFileName)
{
      FILE *fp;
      char filebuffer[MAX_SETTINGS_LINE_LEN], *p;

      if((fp = fopen(INIFileName, "r")) == NULL)   return(-1);
      while(TRUE)
      {
         if(fgets(filebuffer, sizeof(filebuffer), fp) == NULL)  /*- EOF -*/
         {
            strcpy(EntryValue, DefaultValue);
            return(strlen(DefaultValue));
         }

	 if(filebuffer[0] == '[' && strchr(filebuffer, ']'))
         {
            p = strtok(filebuffer, "]");
            if(!strcasecmp(&filebuffer[1], SectionName))
            {
               while(TRUE)
               {
                  if(fgets(filebuffer, sizeof(filebuffer), fp) == NULL)  /*- EOF -*/
                  {
                     strcpy(EntryValue, DefaultValue);
                     return(strlen(DefaultValue));
                  }

		  /*- If Next Section reached then the individual Entry was not found -*/
                  if(filebuffer[0] == '[' && strchr(filebuffer, ']'))
                  {
                     strcpy(EntryValue, DefaultValue);
                     return(strlen(DefaultValue));
                  }

                  p = strtok(filebuffer, "=");
		  if(!strcasecmp(p, EntryName))
                  {
                     /*- Empty Entry. Example  load=    -*/
                     if (p[strlen(filebuffer) + 1] == '\0')
                     {
			p[strlen(filebuffer)] = '\0';	//Overwrite \n
                        *EntryValue = '\0';
                        return(0);
                     }

		     //Get past (former) equals sign
                     p = strchr(filebuffer, '\0') + 1;
		     p[strlen(p) - 1] = '\0';	//Overwrite \n
		     strncpy(EntryValue, p, MaxLength);	//Terminating NULL supplied by calling routine
                     if(MaxLength >= strlen(p)) return(MaxLength) else return(strlen(p));
                  }
               }
            }
            else
               continue;
         }
      }
}

void CleanUpWebServer(void)
{
	//Release all data gathered from the web server
	if(g_strScreenDataType)
		free(g_strScreenDataType);

	if(g_strScreenData)
		free(g_strScreenData);

	//if strlen(getenv("QUERY_STRING")) > 0 then we DONT free the buffer space
	//	as it is a static pointer
	if (g_szBuffer && strlen(getenv("QUERY_STRING")) == 0)
		free(g_szBuffer);
}

void CleanUpRouters(void)
{
	struct WebCLIPSRouter *pWCRouter;

	DeleteRouter(USERDEFINED_ROUTER);
	DeleteRouter(STANDARD_ROUTER);

	//Loop through all standard routers
	pWCRouter = g_pStdWebCLIPSRouter;
	while(pWCRouter)
	{
		//Free any allocated memory
		free(pWCRouter->szRouter);
		free(pWCRouter->szRouterFileName);

		//Close any open files 
		if(!pWCRouter->fp)
		{
			fclose(pWCRouter->fp);
			pWCRouter->fp = NULL;
		}

		//Next router
		pWCRouter = pWCRouter->pNextRouter;
	}

	//Loop through the User-Defined routers
	pWCRouter = g_pUDFWebCLIPSRouter;
	while(pWCRouter)
	{
		//Free any allocated memory
		free(pWCRouter->szRouter);
		free(pWCRouter->szRouterFileName);

		//Close any open files 
		if(!pWCRouter->fp)
		{
			fclose(pWCRouter->fp);
			pWCRouter->fp = NULL;
		}

		//Next router
		pWCRouter = pWCRouter->pNextRouter;
	}
}

void Trim(char *pIn, char *pOut)
{
	char *p;
	
	p = &pIn[strlen(pIn)];	//End of string
	p--;	//Back up off of the NULL
	while(isspace(*p))
		p--;
	p++;	//Move back to the last space

	*p = NULL;
	p = pIn;
	while(isspace(*p))
		p++;

	strcpy(pOut, p);
}
