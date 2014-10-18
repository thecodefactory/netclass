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

#include "ncenums.h"
#include "ncconsts.h"
#include "ncarch.h"
#include "ncmutex.h"

/***********************************************************
 * WIN32 specific methods
 ***********************************************************/
#ifdef WIN32

ncMutex::ncMutex()
{
   InitializeCriticalSection(&m_mutex);
   m_locked = 0;
}

ncMutex::~ncMutex()
{
   if (m_locked)
   {
       unlock();
   }
   DeleteCriticalSection(&m_mutex);
}

void ncMutex::lock()
{
   EnterCriticalSection(&m_mutex);
   m_locked = 1;
}

RetCode ncMutex::trylock()
{
   RetCode ret = NC_NOTSUPPORTED;
#ifdef _WIN32_WINNT
#if(_WIN32_WINNT >= 0x0400)
   if (TryEnterCriticalSection(&m_mutex))
   {
      m_locked = 1;
      ret = NC_OK;
   }
   else
   {
      m_locked = 0;
      ret = NC_FAILED;
   }
#endif
#endif
   return ret;
}

void ncMutex::unlock()
{
   LeaveCriticalSection(&m_mutex);
   m_locked = false;
}

#endif /* WIN32 */

/***********************************************************
 * GNU/Linux and FreeBSD (x86, PPC) specific methods
 ***********************************************************/
#if ((defined __linux__) || (defined __FreeBSD__) || (defined __OpenBSD__))

ncMutex::ncMutex()
{
   pthread_mutex_init(&m_mutex,(pthread_mutexattr_t *)0);
   m_locked = 0;
}

ncMutex::~ncMutex()
{
   if (m_locked)
   {
       unlock();
   }
   pthread_mutex_destroy(&m_mutex);
}

void ncMutex::lock()
{
   pthread_mutex_lock(&m_mutex);
   m_locked = 1;
}

RetCode ncMutex::trylock()
{
   RetCode ret = NC_FAILED;
   if (pthread_mutex_trylock(&m_mutex) != EBUSY)
   {
      m_locked = 1;
      ret = NC_OK;
   }
   return ret;
}

void ncMutex::unlock()
{
   pthread_mutex_unlock(&m_mutex);
   m_locked = 0;
}

#endif /* __linux__ || __FreeBSD__ || __OpenBSD__ */

int ncMutex::isLocked()
{
   return m_locked;
}
