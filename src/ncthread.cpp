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
 * description: a platform independent thread implementation used in
 *   the netclass code base.
 ***********************************************************************/

#include <iostream>
#include "ncenums.h"
#include "ncdebug.h"
#include "ncarch.h"
#include "ncthread.h"

ncThread::ncThread()
{
#ifdef HAVE_THREADS
#if ((defined __linux__) && (!defined __CYGWIN__))
   pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,(int *)0);
   pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,(int *)0);
#endif
   m_retVal = NULL;
#endif
}

ncThread::~ncThread()
{
#ifdef WIN32
   CloseHandle(ncthread);
#endif
}

ncThreadHandle ncThread::getThreadHandle()
{
   return ncthread;
}

#ifdef WIN32
RetCode ncThread::start(ncCallBack fnptr, void *ptr)
{
   RetCode retVal = NC_OK;
   DWORD dwID = 0;
   if ((ncthread = CreateThread(NULL,NULL,fnptr,ptr,NULL,&dwID)) == NULL)
   {
      DEBUG("thread creation failed");
      retVal = NC_FAILED;
   }
   return retVal;
}

RetCode ncThread::join()
{
   RetCode retVal = NC_OK;
   if (WaitForSingleObject(ncthread,INFINITE) == WAIT_FAILED)
   {
      DEBUG("thread join failed.");
      retVal = NC_FAILED;
   }
   return retVal;
}

RetCode ncThread::detach()
{
   RetCode ret = NC_OK;

   /* I don't believe win32 has a concept of thread detachment.
      please let me know if otherwise (or insert your patch here!) */
   return ret;
}

void ncThread::testCancel()
{
   return;
}

void ncThread::stop(int exitStatus)
{
   /*
     this is a well known dangerous win32 call that causes
     memory leaks.  please let me know if there is a way around
     this shoddy call
   */
   TerminateThread(ncthread,(DWORD)exitStatus);
}

void ncThread::exit()
{
   /* need to consult some docs before filling this in */
}

#else
RetCode ncThread::start(ncCallBack fnptr, void *ptr)
{
   RetCode retVal = NC_OK;
#ifdef HAVE_THREADS
   if (pthread_create(&ncthread,NULL,fnptr,ptr))
   {
      DEBUG("thread creation failed");
      retVal = NC_FAILED;
   }
#endif
   return retVal;
}

RetCode ncThread::join()
{
   RetCode retVal = NC_OK;
#ifdef HAVE_THREADS
   if (pthread_join(ncthread,&m_retVal))
   {
      DEBUG("thread join failed.");
      retVal = NC_FAILED;
   }
#endif
   return retVal;
}

RetCode ncThread::detach()
{
   RetCode retVal = NC_OK;
#ifdef HAVE_THREADS
   if (pthread_detach(ncthread))
   {
      DEBUG("thread detach failed.");
      retVal = NC_FAILED;
   }
#endif
   return retVal;
}

void ncThread::testCancel()
{
#ifdef HAVE_THREADS
   pthread_testcancel();
#endif
}
void ncThread::stop(int exitStatus)
{
#ifdef HAVE_THREADS
   pthread_cancel(ncthread);
   pthread_join(ncthread,&m_retVal);
#endif
}

void ncThread::exit()
{
#ifdef HAVE_THREADS
   pthread_exit(m_retVal);
#endif
}

#endif /* WIN32 */
