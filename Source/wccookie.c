//-------------------------------------------------------------------
//- WebCLIPS Cookie-related functions                               -
//-                                                                 -
//- This module contains all functions that relate to processing    -
//- Cookies for WebCLIPS. Cookies are used to track 'temporary'     -
//- files containing facts from previous program executions.        -
//-                                                                 -
//- Source file : wccookie.cpp                                      -
//-                                                                 -
//- Who           |   Date    | Desc                                -
//- --------------+-----------+------------------------------------ -
//- M.Giordano    | 22-Dec-97 | Create                              -
//-               |           |                                     -
//- M.Giordano    | 09-Feb-98 | Converted to MFC App                -
//-               |           |                                     -
//- M.Giordano    | 03-Dec-98 | Converted to UNIX                   -
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
#include <time.h>

int ProcessCookies(void)
{
	char szKeepFacts[7], *szFileName;
	char szTargetScreen[MAX_SCREEN_NAME_SIZE + 1], *p;
	long ltime, lDays;
	struct tm *GMTime;
	int iHours, iMins;
	int bResult;

	//19 = "Set-Cookie: " + " Facts="
	char szCookieParm[MAXNAMLEN + MAX_SCREEN_NAME_SIZE + 19 + 1];

	//7 = "Date | "
	char szCookieExpireParm[MAX_COOKIE_TIME_SIZE + 7 + 1];

	char szCookieTime[MAX_COOKIE_TIME_SIZE + 1];

	//Determine if the screen wants to preserve facts using the 
	//	'cookie' option
	GetWebCLIPSSettings(
        g_szScreenName,			// section name
        "PreserveFacts",		// entry name
        "no",					// default value
        szKeepFacts,			// return value
        sizeof(szKeepFacts),	// max length
        g_szWebCLIPSINI
        );

	//No cookies requested, get out
	if(strcasecmp(szKeepFacts, "cookie") != 0)
		return(0);

	//Determine which screen the facts are being saved for
	GetWebCLIPSSettings(
        g_szScreenName,			// section name
        "SaveFactsFor",			// entry name
        "",						// default value
        szTargetScreen,			// return value
        sizeof(szTargetScreen),	// max length
        g_szWebCLIPSINI
        );

	if(strlen(szTargetScreen) == 0)
	{
		PrintMIMEHeader();
		ProcessErrorCode("COOK0001", g_szScreenName, 'n', 'n');
		return(-1);
	}

	//Append ' Facts'. This is where the target screen will look to
	//	get the name of the file to load.
	strcpy(szCookieParm, "Set-Cookie: ");
	strcat(szCookieParm, szTargetScreen);
	strcat(szCookieParm, " Facts=");

	//Get a unique filename to store facts
	bResult = GenerateTempFileName(&szFileName);
	if(bResult == FALSE)
	{
		ProcessErrorCode("COOK0002", g_szScreenName, 'n', 'n');
		return(-1);
	}
	else
	{
		//Copy the file for the PreserveFacts function call
		strcpy(g_szSaveCookiesFile, szFileName);

		//Add the filename to the CookieParm string
		strcat(szCookieParm, szFileName);
		strcat(szCookieParm, "; ");  //Append a semicolon and a space
		free(szFileName);
	}

	//Get any screen-specific cookie expiration information
	GetWebCLIPSSettings(
        g_szScreenName,				// section name
        "CookieExpiration",			// entry name
        "",							// default value
        szCookieExpireParm,			// return value
        sizeof(szCookieExpireParm),	// max length
        g_szWebCLIPSINI
        );

	//If there is none then check to see if there is any
	//	default expiration information for the cookie
	if(strlen(szCookieExpireParm) == 0)
	{
		GetWebCLIPSSettings(
			"System",					// section name
			"CookieExpiration",			// entry name
			"",							// default value
			szCookieExpireParm,			// return value
			sizeof(szCookieExpireParm),	// max length
			g_szWebCLIPSINI
			);
	}

	//If there is still no expiration info found then the cookie will
	//	be written without an 'expires'. This will have the effect that
	//	the cookie will be present until the end of the session. This 
	//	means the cookie will be present until IE or NN is stopped/closed
	//	terminated ... whatever.
	if(strlen(szCookieExpireParm) == 0)
	{
		printf("%s\n", szCookieParm);
		return(0);
	}

	//We have an expiration parameter. Check to see if it is of the
	//	type : Date | 'Standard Cookie expiration'. If we find a '|'
	//	then we assume that anything to the right of it is a properly
	//	formed 'Standard Cookie expiration' as given in the document :
	//	http://home.netscape.com/newsref/std/cookie_spec.html.
	p = strtok(szCookieExpireParm, " \t");
	if(strcasecmp(p, "date") == 0)
	{
		p = strchr(p, NULL);
		p++;	//Get past NULL
		p = strchr(p, '|');	//Up to the pipe ( '|' )
		p++;	//Past the pipe
		while(!isalpha(*p))	//Past the white space
				p++;
		strcat(szCookieParm, "expires=");
		strcat(szCookieParm, p);
		strcat(szCookieParm, "; "); //append a semicolon and a space
		printf("%s\n", szCookieParm);
		return(0);
	}

	//If we are here then the expires parameter is of the
	//	form Days, Hours, Mins. This will be used to modify the
	//	current time (GMT) and format an expiration date.
	//Construct a CTimeSpan object given the entry in WebCLIPS.INI

	//Days parameter first
	p = strtok(szCookieExpireParm, ",");
	lDays = atol(p);

	//Hours next
	p = strtok(NULL, ",");
	iHours = atoi(p);

	//Minutes next
	p = strtok(NULL, ",");
	iMins = atoi(p);

	//Get the current time
	time(&ltime);

	//Add Days, Hours, Mins in terms of seconds to ltime
	ltime += ToSeconds(lDays, iHours, iMins, 0);

	//Convert to GMT/UTC time
	GMTime = localtime(&ltime);

	//Clear the buffer
	memset(szCookieTime, '\0', sizeof(szCookieTime) + 1);
	strftime(szCookieTime, MAX_COOKIE_TIME_SIZE, "expires=%A, %d-%b-%Y %H:%M:%S GMT; ", GMTime);

	//Append it to current cookie contents and print it.
	strcat(szCookieParm, szCookieTime);	//append the GMT time
	printf("%s\n", szCookieParm);

	return(0);
}

int GetFactsFileFromCookie(void)
{
	char *pScreen, *pValue, *p;

	//Entry for Screen Name Facts file. 6 = " Facts"
	char szScreenNameFacts[MAXNAMLEN + 6 + 1];

	char szCookieScreenName[MAX_SCREEN_NAME_SIZE + 1];

	//Clear out any previous file named
	memset(g_szGetCookiesFile, '\0', MAXNAMLEN + 1);

	p = getenv("HTTP_COOKIE");
	if(p == NULL)
		return 0;

	strcpy(szScreenNameFacts, g_szScreenName);
	strcat(szScreenNameFacts, " Facts");

	//Get first 'name=value' pair in cookie
	pScreen = strtok(p, ";");

	//Look for ScreenName
	while(pScreen != NULL)
	{
		//End the 'name' portion for compare
		pValue = strchr(pScreen, '=');

		if(pValue != NULL)
			*pValue++ = '\0';

		//Trim whitespace and compare ScreenNames
		Trim(pScreen, szCookieScreenName);

		//Case INSENSITIVE match for screen names like GetWebCLIPSSettings
		if(strcasecmp(szCookieScreenName, szScreenNameFacts) == 0)
		{
			Trim(pValue, g_szGetCookiesFile);
			return 0;
		}

		//Look at next cookie parameter
		pScreen = strtok(NULL, ";");
	}

	return 0;
}

//This function expires a cookie for a ScreenName by setting its expires
//	parameter to 1 hour in the past
void ExpireCookie(void)
{
	long ltime;
	struct tm *GMTime;

	//19 = "Set-Cookie: " + " Facts="
	char szCookieParm[MAXNAMLEN + MAX_SCREEN_NAME_SIZE + 19 + 1];
	char szCookieTime[MAX_COOKIE_TIME_SIZE + 1];

	strcpy(szCookieParm, "Set-Cookie: ");
	strcat(szCookieParm, g_szScreenName);
	strcat(szCookieParm, " Facts=");
	strcat(szCookieParm, g_szGetCookiesFile);
	strcat(szCookieParm, "; ");
	
	//Get the current time
	time(&ltime);

	//subtract 1 hour
	ltime -= ToSeconds(0L, 1, 0, 0);

	//Convert to GMT/UTC time
	GMTime = localtime(&ltime);

	//Format for the cookie
	memset(szCookieTime, '\0', sizeof(szCookieTime) + 1);
	strftime(szCookieTime, MAX_COOKIE_TIME_SIZE, "expires=%A, %d-%b-%Y %H:%M:%S GMT; ", GMTime);

	//Append it to current cookie contents and print it.
	strcat(szCookieParm, szCookieTime);	//append the GMT time
	printf("%s\n", szCookieParm);

}

long ToSeconds(long lDays, int iHours, int iMinutes, int iSeconds)
{
	return (lDays * 24 * 60 * 60) + (iHours * 60 * 60) + (iMinutes * 60) + iSeconds;
}