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
 * description: a platform independent socket implementation that 
 *   supports point-to-point sockets using either UDP or TCPIP.
 *   this class is a 'server socket'. (i.e. a socket listener)
 ***********************************************************************/

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

ncSocketListener::ncSocketListener(unsigned short port,int sockType)
{
   m_port = port;
   m_socktype = sockType;
   m_backlog = 16;
   m_listening = 0;
   m_sockfd = 0;
}

ncSocketListener::~ncSocketListener()
{
   if (m_sockfd)
   {
      ncCloseSocket(m_sockfd);
   }
}

RetCode ncSocketListener::startListening(ncCallBack fnptr,
                                         int option,
                                         int flags)
{
   RetCode ret = NC_OK;
   int bindAttempts = 0;
   int sockType = (m_socktype == SOCKTYPE_TCPIP ?
                   SOCK_STREAM : SOCK_DGRAM);
   struct sockaddr_in servaddr;
   struct sockaddr_in clientaddr;
   memset(&servaddr,0,sizeof(servaddr));
   memset(&clientaddr,0,sizeof(clientaddr));

   if (!(m_sockfd = socket(AF_INET,sockType,0)))
   {
      DEBUG("socket call failed.");
      return NC_FAILED;
   }
   ncSocket *newSock = (ncSocket *)0;

   ncSocketHandle client = 0;
   unsigned int addrlen = sizeof(clientaddr);

   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(m_port);

   /* make sure incoming arguments make sense */
   if ((fnptr == (ncCallBack)NULL) ||
       ((option != NC_THREADED) && (option != NC_NONTHREADED)) ||
       (!(flags & NC_DETACHED) && !(flags & NC_JOINED) &&
	!(flags & NC_REUSEADDR)))
   {
      DEBUG("bad args were passed.");
      return NC_BADARGS;
   }

   /* signal that we should be listening */
   m_listening = 1;

   /* declare thread structure, even if not used */
   ncThread newThread;

   /* 
      set the reuse option so if we are terminated incorrectly, we can be 
      restarted quickly. (ignoring the TIME_WAIT status)
   */
   if (flags & NC_REUSEADDR)
   {
      int opt = 1;
      if (setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,
                     (char*)&opt,sizeof(opt)) == -1)
      {
         DEBUG("set socket option failed.");
         return NC_FAILED;
      }
   }

   /* bind socket to port */
   while ((bind(m_sockfd,(sockaddr *)&servaddr,sizeof(servaddr)) == -1) &&
          m_listening)
   {
      if (bindAttempts++ == 10)
      {
         DEBUG("giving up on bind attempts");
         return NC_FAILED;
      }
      else if (ncAddrInUse)
      {
         break;
      }
      else
      {
         DEBUG("Cannot bind to port. This is attempt " << bindAttempts);
         ncSleep(bindAttempts*1000);
      }
   }

   if (m_socktype == SOCKTYPE_TCPIP)
   {
      /* start listening */
      if (listen(m_sockfd,m_backlog) == -1)
      {
         DEBUG("listen failed.");
         return NC_FAILED;
      }
      
      /* set socket to non-blocking mode */
      ncSetSocketNonBlocking(m_sockfd);

      while(m_listening)
      {
         /* accept incoming connections */
         if ((client = accept(m_sockfd,
                              (sockaddr *)&clientaddr,
                              ncAcceptLenType(&addrlen))) != -1)
         {
            DEBUG("Accepted new connection " << client);
            if (option == NC_THREADED)
            {
               /* fnptr func must free ncSocket passed in */
               if ((newSock = new ncSocket(client)))
               {
                  newSock->setIpAddr(clientaddr.sin_addr.s_addr);
                  DEBUG("SPAWNING THREAD TO HANDLE NEW ");
                  DEBUG("CLIENT (" << client << ")");
                  if (newThread.start(fnptr,(void *)newSock) == NC_FAILED)
                  {
                     DEBUG("thread creation for client failed.");
                     delete newSock;
                     return NC_FAILED;
                  }
               }
               else
               {
                  DEBUG("ncsocket allocation failed.");
                  return NC_FAILED;
               }
               /* Under win32, ncThread::detach will always return NC_OK */
               if ((flags == NC_DETACHED) &&
                   (newThread.detach() == NC_FAILED))
               {
                  DEBUG("thread detach for client failed.");
                  return NC_FAILED;
               }
               else if ((flags == NC_JOINED) &&
                        (newThread.join() == NC_FAILED))
               {
                  DEBUG("thread join for client failed.");
                  return NC_FAILED;
               }
            }
            else
            {
               /* call fnptr with client socket here -
                  fnptr func must free ncSocket passed in */
               DEBUG("About to call callback");
               if ((newSock = new ncSocket(client)))
               {
                  newSock->setIpAddr(clientaddr.sin_addr.s_addr);
                  ((*fnptr)((void *)newSock));
               }
               else
               {
                  DEBUG("ncsocket allocation failed.");
                  return NC_FAILED;
               }
            }
         }
         else
         {
            ncSleep(100);
         }
         if (!m_listening)
         {
            break;
         }
      }
   }
   else if (m_socktype == SOCKTYPE_UDP)
   {
      int datasize = 0;
      char buf[MAX_DGRAM_BUF_LEN];
      ncSockLenType cliLen = sizeof(clientaddr);

      while(m_listening)
      {
         memset(buf,0,MAX_DGRAM_BUF_LEN*sizeof(char));
         datasize = recvfrom(m_sockfd,buf,MAX_DGRAM_BUF_LEN,0,
                          (struct sockaddr *)&clientaddr,&cliLen);
         DEBUG("Datagram received | size = " << datasize);

/*** thread support here has yet to be added */

         /* call fnptr with datagram data (type is udpdata) -
            fnptr func must free udpdata passed in */
         ((*fnptr)((void *)new udpData(client,&clientaddr,buf,datasize)));
         if (!m_listening)
         {
            break;
         }
      }
   }
   DEBUG("return ret val " << ret);
   return ret;
}

   
void ncSocketListener::stopListening()
{
   m_listening = 0;
   ncSleep(250);
   if (m_sockfd)
   {
      shutdown(m_sockfd,0);
      ncCloseSocket(m_sockfd);
      m_sockfd = 0;
   }
}

int ncSocketListener::getBackLog()
{
   return m_backlog;
}

void ncSocketListener::setBackLog(int backlog)
{
   m_backlog = backlog;
}
