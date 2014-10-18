/*
  NetClass is a cross platform lightweight socket and thread library.
  Copyright (C) 2000-2002 Neill Miller 
  This file is part of NetClass.
  
  NetClass is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  NetClass is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "netclass.h"

netClass::netClass()
{
}

netClass::~netClass()
{
#ifdef WIN32
    WSACleanup();
#endif
}


/* This code is based on MSDN sample code */
int netClass::initialize()
{
    int ret = 0;
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err = 0;
    ret = 1;

    wVersionRequested = MAKEWORD(2, 2);
 
    if (WSAStartup(wVersionRequested, &wsaData))
    {
        DEBUG("WSAStartup Failed!");
        return ret;
    }
 
    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */
    if ((LOBYTE(wsaData.wVersion) != 2) ||
        (HIBYTE(wsaData.wVersion) != 2))
    {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        WSACleanup();
        DEBUG("No valid WinSock DLL found!");
        return ret; 
    }
    ret = 0;
#endif
    return ret;
}
