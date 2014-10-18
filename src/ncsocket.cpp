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
 *   supports UDP and TCPIP sockets. this class is a 'client socket'.
 ***********************************************************************/

#include "ncconsts.h"
#include "ncenums.h"
#include "ncarch.h"
#include "ncdebug.h"
#include "ncsocket.h"

ncSocket::ncSocket()
{
    m_sockfd = -1;
    m_stdiofd = (FILE *)0;
    m_timeout = 10000;
    m_polltime = 100;
    m_port = 0;
    m_socktype = 0;
    m_ipaddr = 0;
    memset(&m_to,0,sizeof(m_to));
    memset(&m_listener,0,sizeof(m_listener));
    memset(m_hostname,0,MAX_HOSTNAME_LEN);
}

ncSocket::ncSocket(int sockfd)
{
    m_sockfd = sockfd;
    m_stdiofd = (FILE *)0;
    m_timeout = 10000;
    m_polltime = 100;
    m_port = 0;
    m_socktype = 0;
    m_ipaddr = 0;
    memset(&m_to,0,sizeof(m_to));
    memset(&m_listener,0,sizeof(m_listener));
    memset(m_hostname,0,MAX_HOSTNAME_LEN);
}

ncSocket::ncSocket(char *host, unsigned short port, int sockType)
{
    m_sockfd = 0;
    m_stdiofd = (FILE *)0;
    m_timeout = 10000;
    m_polltime = 100;
    m_port = port;
    m_socktype = sockType;
    m_ipaddr = 0;
    memset(&m_to,0,sizeof(m_to));
    memset(&m_listener,0,sizeof(m_listener));
    memset(m_hostname,0,MAX_HOSTNAME_LEN);
    memcpy(m_hostname,host,MIN(strlen(host),MAX_HOSTNAME_LEN-1));
}

ncSocket::~ncSocket()
{
    if (m_stdiofd)
    {
        fclose(m_stdiofd);

        m_sockfd = -1;
        m_stdiofd = (FILE *)0;
    }
    else if (m_sockfd > 0)
    {
        ncCloseSocket(m_sockfd);
        m_sockfd = -1;
    }
}


RetCode ncSocket::flush()
{
    RetCode ret = NC_OK;
    if (!m_stdiofd)
    {
        m_stdiofd = fdopen(m_sockfd,"w");
    }
    if ((m_stdiofd == (FILE *)0) || (fflush(m_stdiofd)))
    {
        ret = NC_FAILED;
    }
    return ret;
}

void ncSocket::setPollTime(int millisecs)
{
    m_polltime = millisecs;
}

void ncSocket::setTimeout(int millisecs)
{
    m_timeout = millisecs;
}

unsigned long ncSocket::getIpAddr()
{
    return m_ipaddr;
}

void ncSocket::setIpAddr(unsigned long ipaddr)
{
    m_ipaddr = ipaddr;
}

ncSocketHandle ncSocket::getSockfd()
{
    return m_sockfd;
}

RetCode ncSocket::checkTimeout(unsigned int *timeout, 
                               unsigned int *timedout, 
                               int haveAllData)
{
    RetCode ret = NC_OK;
    *timeout += m_polltime;
    if (*timeout >= m_timeout)
    {
        /* if we've reached the timeout limit */
        *timedout = 1;

        /* check if we have all the data */
        switch(haveAllData)
        {
            /*
              if we don't know if we have all the
              data, just return a time out
            */
            case -1:
                ret = NC_TIMEDOUT;
                break;
            /*
              if we know we don't have all the data,
              return a 'more data' value
            */
            case 0:
                ret = NC_MOREDATA;
                break;
            /*
              if we know we have all the data,
              return success
            */
            case 1:
                ret = NC_OK;
                break;
        }
    }
    return ret;
}

RetCode ncSocket::connect(char *host,
                          unsigned short port,
                          int sockType)
{
    RetCode ret = NC_FAILED;

    if (host && port)
    {
        m_port = port;
        m_socktype = sockType;
        memset(m_hostname,0,MAX_HOSTNAME_LEN);
        memcpy(m_hostname,host,MIN(strlen(host),MAX_HOSTNAME_LEN-1));

        ret = connect();
    }
    return ret;
}

void ncSocket::close()
{
    if (m_sockfd != -1)
    {
        ncCloseSocket(m_sockfd);
        m_sockfd = -1;
    }
}

RetCode ncSocket::connect()
{
    RetCode ret = NC_OK;
    unsigned long timeout = 0;
    int err = 0;
    ncSockAddrIn tmp;
    ncSockAddrIn *servaddr =
        ((m_socktype == SOCKTYPE_TCPIP) ? &tmp : &m_to);
    int sockType =
        ((m_socktype == SOCKTYPE_TCPIP) ? SOCK_STREAM : SOCK_DGRAM);

    m_sockfd = socket(AF_INET,sockType,0);
    if (!m_sockfd)
    {
        return NC_FAILED;
    }

    memset(servaddr,0,sizeof(servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_port = htons(m_port);

    /* resolve dotted ip address */
    if (!ncValidInetAddr(
            ncGetInetAddr(m_hostname,&servaddr->sin_addr.s_addr)))
    {
        /* otherwise, resolve host name */
        ncHostent *pHostent = ncGetHostByName(m_hostname);
        if (!pHostent)
        {
            err = ncGetLastError();
            DEBUG("host address resolution failed for " << m_hostname);
            DEBUG("Error Code: " << err);
            return NC_FAILED;
        }
        else 
        {
            servaddr->sin_addr.s_addr =
                *((unsigned long *)pHostent->h_addr_list[0]);
        }
    }
   
    /* stash the ip address as an unsigned long in host byte order */
    m_ipaddr = servaddr->sin_addr.s_addr;
    DEBUG("about to connect to " << m_ipaddr << " (numeric format)");

    /* set socket to non-blocking mode */
    ncSetSocketNonBlocking(m_sockfd);

    /* attempt the connect until we timeout */
    while (::connect(m_sockfd,(struct sockaddr *)servaddr,
                     sizeof(*servaddr)) < 0)
    {
        err = ncGetLastError();
        DEBUG("connect error = " << err);
        if (ncIsBlockingError(err))
        {
            timeout += 100;
            if (timeout > m_timeout)
            {
                DEBUG("connect to host timed out (" << err << ").");
                ret = NC_TIMEDOUT;
                break;
            }
        }
        else if (ncIsConnected(err))
        {
            DEBUG("already connected!");
            break;
        }
        else
        {
            DEBUG("connect to host failed (" << err << ").");
            ret = NC_FAILED;
            break;
        }
        ncSleep(100);
    }
    return ret;
}


/***********************************************************
 * Generic read/write methods using select
 ***********************************************************/
#if (defined WIN32) || (defined USE_SELECT_READ_WRITE)

RetCode ncSocket::writeData(void *buf, int bufLen, int*numActualBytes)
{
    RetCode ret = NC_OK;
    char *pPtr = (char *)buf;
    int nRemaining = bufLen;
    int nSent = 0;
    int err = 0;
    int selectRet = 0;
    unsigned int timeout = 0;
    unsigned int timedout = 0;
    int haveAllData = 0;

    struct timeval timeVal;
    long timeoutSeconds = (long)((m_polltime*1000) / 100000);
    long timeoutUSeconds = (long)((m_polltime*1000) % 100000);

    DEBUG("writeData select timeouts: "
          << timeoutSeconds << " " << timeoutUSeconds);

    if (numActualBytes)
    {
        *numActualBytes = 0;
    }

    while((nRemaining > 0) && (!timedout))
    {
        FD_ZERO(&m_listener);
        FD_SET(m_sockfd,&m_listener);
        timeVal.tv_sec  = timeoutSeconds;
        timeVal.tv_usec = timeoutUSeconds;
        haveAllData = -1;

        selectRet = select(m_sockfd+1,(fd_set *)0,
                           &m_listener,(fd_set *)0,&timeVal);
        DEBUG("writeData: select returned " << selectRet);
        switch(selectRet)
        {
            case NCSOCKET_ERROR:
                DEBUG("writeData 'select' call failed "
                      "(" << ncGetLastError() << ")");
                return NC_FAILED;
            case 0:
                if (numActualBytes && (*numActualBytes > 0))
                {
                    haveAllData = ((*numActualBytes == bufLen) ? 1 : 0);
                }
                ret = checkTimeout(&timeout,&timedout,haveAllData);
                continue;
            default:
                if (!FD_ISSET(m_sockfd,&m_listener))
                {
                    DEBUG("writeData: fd not set!");
                    return NC_FAILED;
                }

                nSent = ((m_socktype == SOCKTYPE_TCPIP) ? 
                         send(m_sockfd,(char *)pPtr,
                              nRemaining,ncRecvFlag) :
                         sendto(m_sockfd,(char *)pPtr,nRemaining,0,
                                (struct sockaddr *)&m_to,sizeof(m_to)));
                DEBUG("writeData: send returned " << nSent);
                err = ncGetLastError();
                DEBUG("writeData: err is " << err);
                if (ncSocketError(nSent) && ncSocketReset(err))
                {
                    nRemaining = 0;
                    shutdown(m_sockfd,NCSD_SEND);
                    ret = NC_CLIENTDISCONNECT;
                }
                else if ((nSent > 0) || ncIsBlockingError(err))
                {
                    nRemaining -= nSent;
                    pPtr += nSent;
                    if (numActualBytes)
                    {
                        *numActualBytes += nSent;
                        if (*numActualBytes > 0)
                        {
                            haveAllData =
                                ((*numActualBytes == bufLen) ? 1 : 0);
                        }
                    }
                    ret = checkTimeout(&timeout,&timedout,haveAllData);
                }
                else
                {
                    DEBUG("writeData 'write' call failed ("
                          << err << ")");
                    nRemaining = 0;
                    shutdown(m_sockfd,NCSD_SEND);
                    ret = NC_FAILED;
                }
                break;
        }
    }
    return ret;
}

RetCode ncSocket::readData(void *buf, int bufLen, int *numActualBytes)
{
    RetCode ret = NC_OK;
    char *pPtr = (char *)buf;
    int nRemaining = bufLen;
    int nRead = 0;
    int err = 0;
    int selectRet = 0;
    unsigned int timeout = 0;
    unsigned int timedout = 0;
    int haveAllData = 0;
    ncSockLenType sLen = sizeof(m_to);

    struct timeval timeVal;
    long timeoutSeconds = (long)((m_polltime*1000) / 100000);
    long timeoutUSeconds = (long)((m_polltime*1000) % 100000);

    DEBUG("readData select timeouts: "
          << timeoutSeconds << " " << timeoutUSeconds);

    if (numActualBytes)
    {
        *numActualBytes = 0;
    }

    while((nRemaining > 0) && (!timedout))
    {
        FD_ZERO(&m_listener);
        FD_SET(m_sockfd,&m_listener);
        timeVal.tv_sec  = timeoutSeconds;
        timeVal.tv_usec = timeoutUSeconds;
        haveAllData = -1;

        selectRet = select(m_sockfd+1,&m_listener,
                           (fd_set *)0,(fd_set *)0,&timeVal);
        DEBUG("readData: select returned " << selectRet);
        switch(selectRet)
        {
            case NCSOCKET_ERROR:
                DEBUG("readData 'select' call failed ("
                      << ncGetLastError() << ")");
                return NC_FAILED;
            case 0:
                if (numActualBytes && (*numActualBytes > 0))
                {
                    haveAllData = ((*numActualBytes == bufLen) ? 1 : 0);
                }
                ret = checkTimeout(&timeout, &timedout, haveAllData);
                continue;
            default:
                if (!FD_ISSET(m_sockfd, &m_listener))
                {
                    DEBUG("readData: fd not set!");
                    return NC_FAILED;
                }

                nRead = ((m_socktype == SOCKTYPE_TCPIP) ?
                         recv(m_sockfd,(char *)pPtr,
                              nRemaining,ncRecvFlag) :
                         recvfrom(m_sockfd,(char *)pPtr,nRemaining,0,
                                  (struct sockaddr *)&m_to,&sLen));
                DEBUG("readData: recv returned " << nRead);
                err = ncGetLastError();
                DEBUG("readData: err is " << err);
                if (ncSocketError(nRead) && ncSocketReset(err))
                {
                    nRemaining = 0;
                    shutdown(m_sockfd,NCSD_READ);
                    ret = NC_CLIENTDISCONNECT;
                }
                else if ((nRead > 0) ||
                         ((nRead == 0) && ncIsBlockingError(err)))
                {
                    nRemaining -= nRead;
                    pPtr += nRead;
                    if (numActualBytes)
                    {
                        *numActualBytes += nRead;
                        if (*numActualBytes > 0)
                        {
                            haveAllData =
                                ((*numActualBytes == bufLen) ? 1 : 0);
                        }
                    }
                    ret = checkTimeout(&timeout,&timedout,haveAllData);
                }
                else
                {
                    DEBUG("writeData 'write' call failed ("
                          << err << ")");
                    nRemaining = 0;
                    shutdown(m_sockfd,NCSD_READ);
                    ret = NC_FAILED;
                }
                break;
        }
    }
    return ret;
}
#endif /* WIN32 || USE_SELECT_READ_WRITE */


/***********************************************************
 * Generic read/write methods using poll
 ***********************************************************/
#if (!defined WIN32) && (defined HAVE_POLL) && (!defined USE_SELECT_READ_WRITE)

RetCode ncSocket::writeData(void *buf, int bufLen, int *numActualBytes)
{
    RetCode ret = NC_OK;
    char *pPtr = (char *)buf;
    int nRemaining = bufLen;
    int nSent = 0;
    int err = 0;
    int pollRet = 0;
    unsigned int timeout = 0;
    unsigned int timedout = 0;
    int haveAllData = 0;

    if (numActualBytes)
    {
        *numActualBytes = 0;
    }

    while((nRemaining > 0) && (!timedout))
    {
        memset(&m_listener,0,sizeof(ncPollObject));
        m_listener.fd = m_sockfd;
        m_listener.events = POLLOUT;
        haveAllData = -1;

        pollRet = poll(&m_listener,1,m_polltime);
        DEBUG("writeData: Poll returned " << pollRet);
        switch(pollRet)
        {
            case -1:
                /*
                  linux:
                  should check if errno == EBADF, if so
                  return NC_CLIENTDISCONNECT

                  handle other platforms accordingly (?)
                */
                DEBUG("writeData 'poll' call failed ("
                      "errno = " << errno << ").");
                return NC_FAILED;
            case 0:
                if (numActualBytes && (*numActualBytes > 0))
                {
                    haveAllData = ((*numActualBytes == bufLen) ? 1 : 0);
                }
                ret = checkTimeout(&timeout,&timedout,haveAllData);
                continue;
            case 1:
                if (m_listener.revents & POLLERR)
                {
                    DEBUG("writeData: POLLERR set in revents flag");
                    return NC_FAILED;
                }
                else if (!(m_listener.revents & POLLOUT))
                {
                    DEBUG("writeData: POLLOUT not set in revent flags");
                    return NC_FAILED;
                }

                nSent = ((m_socktype == SOCKTYPE_TCPIP) ? 
                         send(m_sockfd,(ncRecvPtr)pPtr,
                              nRemaining,ncRecvFlag) :
                         sendto(m_sockfd,(ncRecvPtr)pPtr,nRemaining,0,
                                (struct sockaddr *)&m_to,sizeof(m_to)));
                DEBUG("writeData: send returned " << nSent);
                if (nSent < 0)
                {
                    err = ncGetLastError();
                    DEBUG("WRITE RETVAL = "
                          << nSent << " | err = " << err);
                    ret = NC_FAILED;
                    if (ncSocketReset(err))
                    {
                        nRemaining = 0;
                        shutdown(m_sockfd,NCSD_SEND);
                        ret = NC_CLIENTDISCONNECT;
                    }
                    else if (ncIsBlockingError(err))
                    {
                        if (numActualBytes && (*numActualBytes > 0))
                        {
                            haveAllData =
                                ((*numActualBytes == bufLen) ? 1 : 0);
                        }
                        ret = checkTimeout(&timeout,
                                           &timedout,haveAllData);
                        if (ret == NC_OK)
                        {
                            continue;
                        }
                        nRemaining = 0;
                    }
                    else
                    {
                        nRemaining = 0;
                        shutdown(m_sockfd,NCSD_SEND);
                    }
                    break;
                }
                nRemaining -= nSent;
                pPtr += nSent;
                if (numActualBytes)
                {
                    *numActualBytes += nSent;
                    if (*numActualBytes > 0)
                    {
                        haveAllData =
                            ((*numActualBytes == bufLen) ? 1 : 0);
                    }
                }
                ret = checkTimeout(&timeout,&timedout,haveAllData);
                break;
            default:
                break;
        }
    }
    return ret;
}

RetCode ncSocket::readData(void *buf, int bufLen, int *numActualBytes)
{
    RetCode ret = NC_OK;
    char *pPtr = (char *)buf;
    int nRemaining = bufLen;
    int nRead = 0;
    int err = 0;
    int pollRet = 0;
    unsigned int timeout = 0;
    unsigned int timedout = 0;
    int haveAllData = 0;
    socklen_t sLen = sizeof(m_to);

    if (numActualBytes)
    {
        *numActualBytes = 0;
    }

    while((nRemaining > 0) && (!timedout))
    {
        memset(&m_listener,0,sizeof(ncPollObject));
        m_listener.fd = m_sockfd;
        m_listener.events = POLLIN;
        haveAllData = -1;

        pollRet = poll(&m_listener,1,m_polltime);
        DEBUG("readData: Poll returned " << pollRet);
        switch(pollRet)
        {
            case -1:
                /*
                  linux:
                  should check if errno == EBADF, if so
                  return NC_CLIENTDISCONNECT

                  handle other platforms accordingly (?)
                */
                DEBUG("readData 'poll' call failed.");
                return NC_FAILED;
            case 0:
                if (numActualBytes && (*numActualBytes > 0))
                {
                    haveAllData = ((*numActualBytes == bufLen) ? 1 : 0);
                }
                ret = checkTimeout(&timeout, &timedout, haveAllData);
                continue;
            case 1:
                if (m_listener.revents & POLLERR)
                {
                    DEBUG("readData: POLLERR set in revents flag");
                    return NC_FAILED;
                }
                else if (!(m_listener.revents & POLLIN))
                {
                    DEBUG("readData: POLLIN not set in revents flag");
                    return NC_FAILED;
                }

                nRead = ((m_socktype == SOCKTYPE_TCPIP) ?
                         recv(m_sockfd,(ncRecvPtr)pPtr,
                              nRemaining,ncRecvFlag) :
                         recvfrom(m_sockfd,(ncRecvPtr)pPtr,nRemaining,0,
                                  (struct sockaddr *)&m_to,&sLen));

                DEBUG("readData: recv returned " << nRead);
                if (nRead < 0)
                {
                    err = ncGetLastError();
                    DEBUG("READ RETVAL = "
                          << nRead << " | err = " << err);
                    ret = NC_FAILED;
                    if (ncSocketReset(err))
                    {
                        nRemaining = 0;
                        shutdown(m_sockfd,NCSD_READ);
                        ret = NC_CLIENTDISCONNECT;
                    }
                    else if (ncIsBlockingError(err))
                    {
                        if (numActualBytes && (*numActualBytes > 0))
                        {
                            haveAllData =
                                ((*numActualBytes == bufLen) ? 1 : 0);
                        }
                        ret = checkTimeout(&timeout,
                                           &timedout,haveAllData);
                        if (ret == NC_OK)
                        {
                            continue;
                        }
                        nRemaining = 0;
                    }
                    else
                    {
                        nRemaining = 0;
                        shutdown(m_sockfd,NCSD_READ);
                    }
                    break;
                }
                nRemaining -= nRead;
                pPtr += nRead;
                if (numActualBytes)
                {
                    *numActualBytes += nRead;
                    if (*numActualBytes > 0)
                    {
                        haveAllData =
                            ((*numActualBytes == bufLen) ? 1 : 0);
                    }
                }
                ret = checkTimeout(&timeout, &timedout, haveAllData);
                break;
            default:
                break;
        }
    }
    return ret;
}

#endif /* !WIN32 && HAVE_POLL && !USE_SELECT_READ_WRITE */
