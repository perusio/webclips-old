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

#ifndef	__WebCLIPS_H__
#define __WebCLIPS_H__

//Library functions to include
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/dir.h>
#include "clips.h"

//Debugging
#define WebCLIPS_DEBUG

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

//Version Information
#define WEBCLIPS "WebCLIPS"
#define WEBCLIPS_VERSION "2.0.3"

//WebCLIPS Constants
#define MAX_SCREEN_NAME_SIZE 128
#define FACT_BUFFER_SIZE 4096
#define MAX_COOKIE_SIZE 4096
#define MAX_COOKIE_TIME_SIZE 40
#define MAX_ROUTER_NAME_LEN 32

//MAX_SETTINGS_LINE_LEN
//	The longest single input parameter should be re-routing IO for a router. for example :
//		NuclearReactorValveStatusPortEcho=/Reactor/Reports/Status/Critical/MostRecent.txt
//
//	MAX_ROUTER_NAME_LEN +
//	4 ('Echo')
//	1 (equals sign)
//	MAXNAMLEN (Maximum File Name Length)
#define MAX_SETTINGS_LINE_LEN MAX_ROUTER_NAME_LEN + 4 + 1 + MAXNAMLEN

//Convenient CLIPS Values
//Load
#define LOAD_OK				1
#define LOAD_PARSE_ERROR	-1
#define LOAD_FILE_IO_ERROR	0

//Fact Types
#define FACT_TYPE_SCRIPT	"scpt"
#define FACT_TYPE_FACTGROUP	"fgrp"
#define FACT_TYPE_BINIMAGE	"bimg"

//WebCLIPS Router Info
struct WebCLIPSRouter
{
	char					*szRouter;
	char					cDestination;
	char					*szRouterFileName;
	char					cRouterType;
	FILE					*fp;
	struct WebCLIPSRouter	*pNextRouter;
};

//Router Types
#define WCRouter_System			's'
#define WCRouter_Dribble		'd'
#define WCRouter_Userdefined	'u'

//Router destinations
#define	WCRouter_ToScreen		's'
#define	WCRouter_ToFile			'f'

//Router Names
#define USERDEFINED_ROUTER	"UDFWebCLIPSRouter"
#define STANDARD_ROUTER		"StdWebCLIPSRouter"

//wcroute.cpp
int			SetupRouters(void);
void		FlushAllRouters(void);
int			CloseAllRouters(void);
int			DisplayScreenRouters(void);
int			InitializeStdCLIPSRouters(void);
int			CreateStdWebCLIPSRouter(struct WebCLIPSRouter *pWCRouter, char *szRouter);
int			StdWCQuery(char *szRouterName);
int			StdWCPrint(char *szRouterName, char *strRouterOutput);
int			StdWCExit(int iExitCode);
int			UDFWCQuery(char *szRouterName);
int			UDFWCPrint(char *szRouterName, char *strRouterOutput);
int			UDFWCExit(int iExitCode);

//wccookie.cpp
int			ProcessCookies(void);
int			GetFactsFileFromCookie(void);
void		ExpireCookie(void);
long		ToSeconds(long lDays, int iHours, int iMinutes, int iSeconds);

//wcmisc.cpp
void		ProcessErrorCode(char *szErrCode, char *szAdditionalData, int cHeader, int cUseStdError);
void		FormatError(char *szErrCode, char *szErrMsg, char *szAdditionalData, int cUseStdError);
int			SetTempDir(void);
int			GetWebCLIPSSettings(char *SectionName, char *EntryName, char *DefaultValue,
                            char *EntryValue, unsigned int MaxLength, char *INIFileName);
void		CleanUpWebServer(void);
void		CleanUpRouters(void);
void		Trim(char *pIn, char *szOut);

//webio.cpp
void		PrintMIMEHeader(void);
void		PrintHTMLHeader(char *szAppName, char *szVersion);
void		PrintHTMLTrailer(void);
char		*GetInputMethod(void);
int			GetPOSTData(char **szWebBuffer);
void		ProcessPair(char *pVariable, unsigned int iCount);
int			ProcessWebServerData(void);
int			ProcessPOSTData(void);
int			ProcessURLData(void);
void		SplitUpWebServerData(char *szBuffer);
int			IntFromHex(char *pChars);

//wccore.cpp
int			Init(void);
void		SaveScreenName(char *szPrevScreen);
int			ClassifyWebServerData(void);
int			ProcessAllFacts(void);
int			PreserveFactState(char *szPrevScreen);
int			UpdateScreenName(void);
int			ProcessAllScripts(void);
int			LoadCLIPSScript(char *szFileName);
int			ProcessBinImage(void);
int			BLoadCLIPSScript(char *szFileName);
int			LoadHelper(void);
int			ProcessAllFactGroups(void);
int			LoadCLIPSFactGroup(char *szFileName);
int			GetScreenDisplay(char **szFileBuffer);
void		Reset(void);
void		Go(void);
long int	GetAgendaLimit(void);

int			ReadCompleteFile(const char *szFileName, char **buf);
int			GenerateTempFileName(char **szTempFileName);
void		SwapChar(char * pOriginal, char cBad, char cGood);
void		FilterHTML (char *szData);
void		URLDecode(char *pEncoded);
void		PrintURLEncode(const char *szInString);
void		FindEndBodyTag(char *szFileBuffer, char **pEndBodyTag);
void		FindEndFormTag(char *szFileBuffer, char **pEndFormTag);
void		UserFunctions(void);

#endif // __WebCLIPS_H__