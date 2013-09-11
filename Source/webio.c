//-------------------------------------------------------------------
//- Web server I/O functions                                        -
//-                                                                 -
//- This module contains web server functions for both GET and POST -
//- methods as well as other related functions.                     -
//-                                                                 -
//- Source file : webio.cpp                                         -
//-                                                                 -
//- Who           |   Date    | Desc                                -
//- --------------+-----------+------------------------------------ -
//- M.Giordano    | 15-Jan-98 | Create                              -
//-               |           |                                     -
//- M.Giordano    | 30-Nov-99 | Ported to HP-UX                     -
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

// PrintMIMEHeader:  Prints content-type header
void PrintMIMEHeader(void)
{

    //This is a multi-line header,
    // including a "pragma: no-cache" directive.
    // This keeps the page from being cached,
    // and reduces the number of duplicate
    // entries from users who keep hitting the
    // submit button over and over
    printf("Content-type: text/html\n\n");
	
}

// PrintHTMLHeader:  Prints HTML page header
void PrintHTMLHeader(char *szAppName, char *szVersion)
{
    printf(
		"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n"
        "<html>\n"
        "<head><title>%s %s</title></head>\n"
        "<body bgcolor=#FFFFFF>\n", WEBCLIPS, WEBCLIPS_VERSION);
}

// PrintHTMLTrailer:  Prints closing HTML info
void PrintHTMLTrailer()
{
    printf(
        "</body>\n"
        "</html>\n"
        );
}

//General Routine to handle both GET and POST data input from the
//  web server.
int ProcessWebServerData(void)
{
	char *pRequestMethod;
	int iErrCode;
	
	pRequestMethod = getenv("REQUEST_METHOD");

	if (pRequestMethod == NULL)
	{
        //No request method. Must have been invoked from
        //command line.  Print a message and terminate.
		ProcessErrorCode("WBIO0001", NULL, 'n', 'n');
		return(-1);
	}

	if (strcasecmp(pRequestMethod,"GET") == 0)
	{
		if(ProcessURLData() == FALSE)
		{
			ProcessErrorCode("WBIO0002", NULL, 'n', 'n');
			return(-1);
		}
		else
		{
			return(0);
		}
	}
	
	if (strcasecmp(pRequestMethod,"POST")==0)
	{
		iErrCode = ProcessPOSTData();
		if(iErrCode != 0)
		{
			switch(iErrCode)
			{
				case -1 :
					ProcessErrorCode("WBIO0003", NULL, 'n', 'n');
					return(iErrCode);

				case -2 :
					ProcessErrorCode("WBIO0004", NULL, 'n', 'n');
					return(iErrCode);
			}
		}
	}
	else //Request_Method was neither GET or POST
	{
		ProcessErrorCode("WBIO0001", NULL, 'n', 'n');
		return(-1);
	}

	return(0);
}
	
//Setup for Method=POST data for processing
int ProcessPOSTData(void)
{
	int iErrCode;

	//Get Post Data
	iErrCode = GetPOSTData(&g_szBuffer);
	if(iErrCode != 0)
		return(iErrCode);

	//Process data.
	//  Capture screen data input into StringArrays g_strScreenDataType, g_strScreenData
	SplitUpWebServerData(g_szBuffer);

	return(0);
}

// GetPOSTData:  Read in data from POST operation
int GetPOSTData(char **szBuffer)
{
    char *pContentLength, szPostInputMax[24];
    int ContentLength;    
    int i, x;

    // Retrieve a pointer to the CONTENT_LENGTH variable

    pContentLength = getenv("CONTENT_LENGTH");

    // If the variable exists, convert its value to an integer
    // with atoi()
    if (pContentLength != NULL)
    {
    	ContentLength = atoi(pContentLength);
    }
	else
    {
    	ContentLength = 0; //Always have screen name data
		return -1;
    }

	//Get the POST Input Size Limit from WebCLIPS.ini
	GetWebCLIPSSettings(
        "System",               // section name
        "MaxPostInputSize",     // entry name
        "",                     // default value
        szPostInputMax,         // return value
        sizeof(szPostInputMax), // max length
        g_szWebCLIPSINI
        );

    // Make sure specified length isn't greater than the size
    // indicated in our INI file.
    if (ContentLength > (atoi(szPostInputMax)))
    {
    	ContentLength = (atoi(szPostInputMax));
    }

	//Allocate space based on Content Length
	*szBuffer = (char *)malloc(ContentLength + 1);
	if(!(*szBuffer))   //Unable to allocate space
	{
		return -2;
	}

    // Now read ContentLength bytes from STDIN
    i = 0;
    while (i < ContentLength)
    {
    	x = fgetc(stdin);
    	if (x==EOF) break;
    	(*szBuffer)[i++] = x;
    }

    // Terminate the string with a zero
    (*szBuffer)[i] = '\0';
	return 0;
}

//Setup Method=GET data for processing
int ProcessURLData(void)
{
	g_szBuffer = getenv("QUERY_STRING");

	if(!g_szBuffer)
		return(FALSE);

	SplitUpWebServerData(g_szBuffer);

	return(TRUE);
}

//Processes data.
//This routine splits up the Var & Val pairs into two
//arrays of strings. Returns the number of strings available.
void SplitUpWebServerData(char *szBuffer)
{
    char *pVariable;
	unsigned int iCount = 0;

	pVariable = szBuffer;

	//Count up the number of data items passed from the screen
	while(*pVariable)
	{
		if(*pVariable++ == '&')
			iCount++;
	}

	iCount++; //If none found, the one data item exists

	//Allocate the appropriate number of pointers to strings
	g_strScreenDataType = (char **)calloc(sizeof(char *), (iCount + 1));
	g_strScreenData = (char **)calloc(sizeof(char *), (iCount + 1));

    //Find the first "&" token in the string
    pVariable = strtok(szBuffer,"&");

    //If any tokens in the string, split them up and put the results
	//into a string array for later use.
	if(pVariable == NULL)  //Only one entry!
		ProcessPair(szBuffer, 0);
	else
	{
		iCount = 0;
		while (pVariable != NULL)
		{
			ProcessPair(pVariable, iCount);

			// And look for the next "&" token (variable)
			pVariable = strtok(NULL,"&");
			iCount++;
		}
	}
}

//Process Variable/Value pair
void ProcessPair(char *pVariable, unsigned int iCount)
{
	char *pValue, *pLower;

	URLDecode(pVariable);

	pValue = strchr(pVariable, '=');
	if(pValue)
	{
		*pValue++ = '\0';
		FilterHTML(pVariable);
		FilterHTML(pValue);			
		g_strScreenData[iCount] = pValue;
		g_strScreenDataType[iCount] = pVariable;

		//Make the datatypes lowercase
		pLower = pVariable;
		while(*pLower)
		{
			*pLower = tolower(*pLower);
			pLower++;
		}
	}
}

// URLDecode -- un-URL-Encode a string.  This routine loops
// through the string pEncoded, and decodes it in place.  It
// checks for escaped values, and changes all plus signs to
// spaces.  The result is a normalized string.  It calls the
// two subroutines directly above in this listing.
void URLDecode(char *pEncoded)
{
	char *pDecoded, *pOrig;

    //Loop through looking for escapes
    pDecoded = pOrig = pEncoded;
    while (*pEncoded)
	{
		if (*pEncoded=='%')
        {
            // A percent sign followed by two hex digits means
            // that the digits represent an escaped character.  We
            // must decode it.
            pEncoded++;
            if (isxdigit(pEncoded[0]) && isxdigit(pEncoded[1]))
            {
                *pDecoded++ = (char) IntFromHex(pEncoded);
                pEncoded += 2;
            }
        }
        else
        {
            *pDecoded ++ = *pEncoded++;
        }
    }
    *pDecoded = '\0';

	//Last, change those pesky plusses to spaces    
	SwapChar (pOrig, '+', ' ');

}

// FilterHTML -- removes any HTML tags from data
// This routine removes any <> tags found in the
// szData string.
void FilterHTML (char *szData)
{
	char *pOpenAngle;
    char *pCloseAngle;

    while (TRUE)
    {
        // Find an opening angle bracket
        pOpenAngle = strchr(szData,'<');
        
        // If none, all done here
        if (pOpenAngle==NULL) break;

        // Otherwise, look for the closing angle bracket
        pCloseAngle = strchr(pOpenAngle,'>');

        // If we found a closing angle bracket, snug
        // up all the characters after it, thus removing
        // the tag entirely from the string
        
        if (pCloseAngle)
        {
            strcpy(pOpenAngle,pCloseAngle+1);
        }
        
        // If no closing angle bracket, then the visitor
        // has provided invalid HTML, so truncate at the
        // opening bracket
        else
        {
            *pOpenAngle = '\0';
        }
    }
}

// SwapChar:  This routine swaps one character for another
void SwapChar(char *pOriginal, char cBad, char cGood)
{
    register int i;    

    // Loop through the input string (cOriginal), character by
    // character, replacing each instance of cBad with cGood
    i = 0;
    while (pOriginal[i])
	{
        if (pOriginal[i] == cBad)
		   pOriginal[i] = cGood;
        i++;
    }
}

// IntFromHex:  A subroutine to unescape escaped characters.
int IntFromHex(char *pChars)
{
    int Hi;        // holds high byte
    int Lo;        // holds low byte
    int Result;    // holds result

    // Get the value of the first byte to Hi
    Hi = pChars[0];
    if ('0' <= Hi && Hi <= '9')
	{
        Hi -= '0';
    } else
    if ('a' <= Hi && Hi <= 'f')
	{
        Hi -= ('a'-10);
    } else
    if ('A' <= Hi && Hi <= 'F')
	{
        Hi -= ('A'-10);
    }

    // Get the value of the second byte to Lo
    Lo = pChars[1];
    if ('0' <= Lo && Lo <= '9')
	{
        Lo -= '0';
    } else
    if ('a' <= Lo && Lo <= 'f')
	{
        Lo -= ('a'-10);
    } else
    if ('A' <= Lo && Lo <= 'F')
	{
        Lo -= ('A'-10);
    }
    Result = Lo + (16 * Hi);
    return (Result);
}

//Parameters :
//	*szInString : Pointer to the String to be Encoded
/*-
void PrintURLEncode(const char *szInString)
{
	const char *pIn = szInString;
	char *pEncode;

	char szExclude[65] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','+','/', '\0'};

	while(*pIn)
	{
		pEncode = szExclude;
		while(*pEncode)
		{
			if(*pIn == *pEncode)
			{
				printf("%c", *pIn);
				break;
			}

			pEncode++;
		}

		if(*pEncode == '\0') //Reached the end of the Encode table
		{
			printf("%c%02x", '%', *pIn);
		}
		pIn++;
	}
}
-*/
void PrintURLEncode(const char *szInString)
{
	const char *pIn;

	pIn = szInString;
	while(*pIn)
	{
		if(*pIn == '\"')
			printf("%22");
		else
			printf("%c", *pIn);

		pIn++;
	}
}