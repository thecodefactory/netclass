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
 * description: a platform independent mutex implementation.
 ***********************************************************************/

#ifndef __NCMUTEX_H
#define __NCMUTEX_H

class ncMutex
{
  public:
   ncMutex();
   ~ncMutex();

   void lock();
   void unlock();

   /* 
      tries to acquire lock but will not block
      if it cannot be acquired at that time.
      returns NC_OK if the lock is acquired;
      returns NC_FAILED if the lock cannot be acquired

      If there is no equivalent functionality available
      on your operating system, NC_NOTSUPPORTED will
      be returned.
   */
   RetCode trylock();

   /* returns 1 if this mutex is locked; 0 otherwise */
   int isLocked();

  private:
   int m_locked;
   ncMutexHandle m_mutex;
};

#endif /* __NCMUTEX_H */
