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
 * description: constant values used in netclass code base.
 ***********************************************************************/

#ifndef __NCCONSTS_H
#define __NCCONSTS_H

#define MAX_HOSTNAME_LEN   256
#define SOCKTYPE_TCPIP       0
#define SOCKTYPE_UDP         1
#define NC_THREADED          0
#define NC_NONTHREADED       1
#define NC_DETACHED          2
#define NC_JOINED            4
#define NC_REUSEADDR         8
#define MAX_DGRAM_BUF_LEN  512

#endif // __NCCONSTS_H
