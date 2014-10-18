/*
  NetClass is a cross platform lightweight socket and thread library.
  Copyright (C) 2000-2003 Neill Miller 
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
 * description: a platform independent socket implementation that 
 *   supports either UDP or TCPIP sockets.
 *   this is a 'server socket'. (i.e. a socket listener)
 ***********************************************************************/

#ifndef __NCSOCKETLISTENER_H
#define __NCSOCKETLISTENER_H

class ncSocketListener
{
  public:
   ncSocketListener(unsigned short port, int sockType);
   ~ncSocketListener();

   /*
     if option is NC_THREADED, the thread related flags are checked:
     if flags is NC_DETACHED, the thread is detached, otherwise it's joined.
     the thread related flags are ignored if the option is not NC_THREADED.
     regardless, if the flags have the NC_REUSEADDR bit set, the socket
     option for reuse will be set.
   */
   RetCode startListening(ncCallBack fnptr,
                          int option = NC_THREADED,
                          int flags = NC_DETACHED);

   /* stops the listener after next (non-blocking) accept call */
   void stopListening();

   /*
      directly set or get the backlog value for the socket used within
      this class (i.e. handed to the internal "listen" call).
      To use this effectively, it must be called before startListening method.
   */
   int getBackLog();
   void setBackLog(int backlog);

  private:
   ncSocketHandle m_sockfd;
   int m_listening;
   int m_socktype;
   int m_backlog;
   unsigned short m_port;
};

#endif /* __NCSOCKETLISTENER_H */
