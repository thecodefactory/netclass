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

/***********************************************************************
 * description: a platform independent network class for transferring
 *   arbitrary data.
 ***********************************************************************/

#ifndef __NETCLASS_H
#define __NETCLASS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ncconsts.h"
#include "ncenums.h"
#include "ncdebug.h"
#include "ncarch.h"
#include "ncsocket.h"
#include "ncthread.h"
#include "udpdata.h"
#include "ncsocklist.h"
#include "ncmutex.h"

class netClass
{
  public:
   netClass();
   ~netClass();

   /*
     win32 users MUST call initialize on startup
     before using any of the ncSocket calls.
   */
   static int initialize();
};



#endif // __NETCLASS_H
