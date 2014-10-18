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
 *   supports UDP and TCPIP sockets.
 ***********************************************************************/

#ifndef __NCSOCKET_H
#define __NCSOCKET_H

class ncSocket
{
  public:
   ncSocket();
   ncSocket(int sockfd);
   ncSocket(char *host,
            unsigned short port,
            int sockType = SOCKTYPE_UDP);
   ~ncSocket();

   /* 
      establishes a connection to the host/port specified
      on object construction.  returns NC_OK on success,
      NC_TIMEDOUT on timeout, and NC_FAILED on failure
   */
   RetCode connect();

   /* 
      establishes a connection to the host/port specified.
      returns NC_OK on success, NC_TIMEDOUT on timeout,
      and NC_FAILED on failure
   */
   RetCode connect(char *host,
                   unsigned short port,
                   int sockType = SOCKTYPE_UDP);

   /* close the current socket connection */
   void close();

   /*
      this method writes up to numBytes from buf and returns NC_OK in
      the simplest case.  If the ncSocket::setTimeout is called with
      a timeout that is shorter than the time it takes to write all
      of the requested data, this method will write as many bytes
      as possible (up to numBytes) from buf and store the actual
      number of bytes sent in the numActualBytes argument if it is
      included.  If numActualBytes != numBytes and the read times
      out, a value of NC_MOREDATA is returned.

      Thus, the easiest way to do a blocking write is to set the timeout
      to a large value (i.e. 60000ms which is 60 seconds).  On the
      other hand, the easiest way to do a non-blocking read is to set
      the timeout to a small value (i.e. 0ms, or 0 seconds).  The
      default timeout value is ~10 seconds - thus the sockets act as
      blocking sockets by default.

      If no data is written to the socket at all before the specified
      timeout period, this method returns a value of NC_TIMEDOUT.

      Note: NC_MOREDATA will never be returned unless the numActualBytes
      argument is non-null when calling this method.  NC_MOREDATA is
      returned if some of the data has been written, but the timeout
      occured before all of the data could be written.

      returns NC_CLIENTDISCONNECT if a client forcefully disconnects.
   */
   RetCode writeData(void *buf, int bufLen, int *numActualBytes = 0);

   /*
      this method reads up to numBytes into buf and returns NC_OK in
      the simplest case.  If the ncSocket::setTimeout is called with
      a timeout that is shorter than the time it takes to read all
      of the requested data, this method will read as many bytes
      as possible (up to numBytes) in buf and store the actual
      number of bytes read in the numActualBytes argument if it is
      included.  If numActualBytes != numBytes and the read times out,
      a value of NC_MOREDATA is returned.

      Thus, the easiest way to do a blocking read is to set the timeout
      to a large value (i.e. 60000ms, or 60 seconds).  On the
      other hand, the easiest way to do a non-blocking read is to set
      the timeout to a small value (i.e. 0ms, or 0 seconds).  The
      default timeout value is ~10 seconds - thus the sockets act as
      blocking sockets by default.

      If no data is read from the socket at all before the specified
      timeout period, this method returns a value of NC_TIMEDOUT.

      Note: NC_MOREDATA will never be returned unless the numActualBytes
      argument is non-null when calling this method.  NC_MOREDATA is
      returned if some of the data has been read, but the timeout
      occured before all of the data could be read.

      returns NC_CLIENTDISCONNECT if a client forcefully disconnects.
   */
   RetCode readData(void *buf, int bufLen, int *numActualBytes = 0);

   /*
     flushes the output data from the last call of writeData;
     returns NC_OK on success; NC_FAILED on error
   */
   RetCode flush();

   /* 
      sets the socket timeout value; default is ~10 seconds.
   */
   void setTimeout(int millisecs);

   /*
      do not use this function for now unless you know what you're doing.
   */
   void setPollTime(int millisecs);

   unsigned long getIpAddr();
   void setIpAddr(unsigned long ipaddr);

   ncSocketHandle getSockfd();

  private:
   RetCode checkTimeout(unsigned int *timeout,
                        unsigned int *timedout,
                        int haveAllData);

   ncSocketHandle m_sockfd;
   ncPollObject m_listener;
   FILE *m_stdiofd;
   ncSockAddrIn m_to;
   unsigned long m_ipaddr;
   int m_socktype;
   unsigned int m_timeout;
   unsigned int m_polltime;
   unsigned short m_port;
   char m_hostname[MAX_HOSTNAME_LEN];
};

#endif // __NCSOCKET_H
