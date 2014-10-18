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
 * description: a platform independent thread implementation
 ***********************************************************************/

#ifndef __NCTHREAD_H
#define __NCTHREAD_H

class ncThread
{
  public:
   ncThread();
   ~ncThread();

   RetCode start(ncCallBack fnptr,void *ptr);
   RetCode join();
   RetCode detach();
   static void testCancel();
   void stop(int exitStatus);
   void exit();
   ncThreadHandle getThreadHandle();

  private:
   void *m_retVal;
   ncThreadHandle ncthread;
};

#endif /* __NCTHREAD_H */
