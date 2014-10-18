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
 * description: an object representing a single UDP datagram
 ***********************************************************************/

#ifndef __UDPDATA_H
#define __UDPDATA_H

class udpData
{
  public:
   udpData();
   udpData(int sockfd, struct sockaddr_in *clientaddr, char *buf, int len)
   {
      m_sockfd = sockfd;
      m_length = MIN(len,MAX_DGRAM_BUF_LEN);
      memcpy(&m_clientaddr,clientaddr,sizeof(struct sockaddr_in));
      memcpy(m_data,buf,m_length*sizeof(char));
   }
   ~udpData() { }

   inline struct sockaddr_in *getClientAddr() 
                                     { return &m_clientaddr; }
   inline ncSocketHandle getSockfd() { return m_sockfd;      }
   inline int getLength()            { return m_length;      }
   inline char *getData()            { return m_data;        }

  private:
   int m_sockfd;
   int m_length;
   struct sockaddr_in m_clientaddr;
   char m_data[MAX_DGRAM_BUF_LEN];
};

#endif /* __UDPDATA_H */
