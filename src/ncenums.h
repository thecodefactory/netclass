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
 * description: constants used in netclass code base
 ***********************************************************************/

#ifndef __NCENUMS_H
#define __NCENUMS_H

enum RetCode
{
   NC_FAILED = -1,
   NC_OK,
   NC_TIMEDOUT,
   NC_BADARGS,
   NC_MOREDATA,
   NC_CLIENTDISCONNECT,
   NC_NOTSUPPORTED
};

#endif // __NCENUMS_H
