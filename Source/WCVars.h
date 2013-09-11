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

extern char*	g_szBuffer;
extern char**	g_strScreenDataType;
extern char**	g_strScreenData;
extern struct WebCLIPSRouter*	g_pStdWebCLIPSRouter;
extern struct WebCLIPSRouter*	g_pUDFWebCLIPSRouter;

extern char	g_szWebCLIPSINI[MAXNAMLEN + 1];
extern char	g_szSaveCookiesFile[MAXNAMLEN + 1];
extern char	g_szGetCookiesFile[MAXNAMLEN + 1];
extern char	g_szTempPath[MAXNAMLEN + 1];
extern char	g_szScreenName[MAX_SCREEN_NAME_SIZE + 1];
extern int	g_bScriptFound;
