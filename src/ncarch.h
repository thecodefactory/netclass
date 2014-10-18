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

#ifndef __NCARCH_H
#define __NCARCH_H

/***********************************************************
 * WIN32 specific includes and constants
 ***********************************************************/
#ifdef WIN32
#ifndef __WIN32INCLUDED
#define __WIN32INCLUDED
// Also need to link with ws2_32.lib
#include <stdlib.h>
#include <winsock2.h>
#include <map>
#include <vector>
#include <iterator>
#include <windows.h>
#include <windef.h>
#include <winbase.h>
#define ncGetLastError() WSAGetLastError()
#define NCSOCKET_ERROR SOCKET_ERROR
#define NCSD_SEND SD_SEND
#define NCSD_READ SD_RECEIVE
#define ncRecvFlag 0
#define ncSocketHandle SOCKET
#define ncPollObject struct fd_set
#define ncHostent HOSTENT
#define ncGetHostByName gethostbyname /* should eventually be getaddrinfo (?) */
#define ncCloseSocket closesocket
#define ncSleep(a) Sleep(a)
#define ncSockAddrIn struct sockaddr_in
#define ncAddrInUse (ncGetLastError() == WSAEADDRINUSE)
#define ncAcceptLenType(a) (int *)a
#define ncSockLenType int
#define ncSocketError(a) (a == SOCKET_ERROR)
#define ncIsBlockingError(a) \
((a == WSAEALREADY) || (a == WSAEWOULDBLOCK) || (a == WSAEINVAL))
#define ncIsConnected(a) (a == WSAEISCONN)
#define ncSocketReset(a) ((a == WSAECONNRESET) || (a == WSAESHUTDOWN))
#define ncSetSocketNonBlocking(a) ioctlsocket(a,FIONBIO,(unsigned long *)0x01)
#define ncCallBackRetType DWORD WINAPI
typedef DWORD (WINAPI *ncCallBack)(LPVOID ptr);
#define ncThreadHandle HANDLE
#define ncMutexHandle CRITICAL_SECTION
#define ncGetInetAddr(host,addrPtr) *addrPtr = inet_addr(host)
#define ncValidInetAddr(addr) (addr != INADDR_NONE)
#endif
#endif /* WIN32 */

/***********************************************************
 * Cygwin specific includes and constants
 ***********************************************************/
#ifdef __CYGWIN__
#ifndef __CYGWININCLUDED
#define __CYGWININCLUDED
#define USE_LINUX
/* here we are overriding some defines drawn in from __linux__ */
#define ncRecvFlag 0
#ifndef HAVE_INET_PTON
  #define inet_pton(a,b,c) inet_addr(b)
#endif
#define ncRecvPtr char *
#define ncAcceptLenType(a) (int *)a
#endif
#endif /* __CYGWIN__ */

/***********************************************************
 * OpenBSD specific includes and constants
 ***********************************************************/
#ifdef __OpenBSD__
#ifndef __OPENBSDINCLUDED
#define __OPENBSDINCLUDED
#define USE_LINUX
#include <sys/types.h>
#include <netinet/in.h>
#define ncRecvFlag 0
#endif
#endif /* __OpenBSD__ */

/***********************************************************
 * FreeBSD specific includes and constants
 ***********************************************************/
#ifdef __FreeBSD__
#define USE_LINUX
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define ncGetInetAddr(host,addrPtr) *addrPtr = inet_addr(host)
#define ncValidInetAddr(addr) (addr != INADDR_NONE)
#define ncRecvFlag 0
#define ncSockLenType socklen_t
#define ncAcceptLenType(a) (socklen_t *)a
#endif

/***********************************************************
 * Apple/Darwin specific includes and constants
 ***********************************************************/
#ifdef __APPLE__
#ifndef __APPLEINCLUDED
#define __APPLEINCLUDED
#define USE_LINUX
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define ncGetInetAddr(host,addrPtr) *addrPtr = inet_addr(host)
#define ncValidInetAddr(addr) (addr != INADDR_NONE)
#define ncRecvFlag 0
#define ncSockLenType int
#define ncAcceptLenType(a) (int *)a
#endif
#endif /* __APPLE__ */

/***********************************************************
 * GNU/Linux (x86, PPC) specific includes and constants
 ***********************************************************/
#if ((defined __linux__) || (defined USE_LINUX))
#ifndef __LINUXINCLUDED
#define __LINUXINCLUDED
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#ifdef HAVE_SOCKET
  #include <netdb.h>
  #include <sys/socket.h>
  #include <sys/uio.h>
  #include <arpa/inet.h>
#else
  #error "FIXME (ncarch.h): NetClass requires a socket library"
#endif
#include <errno.h>
#ifdef HAVE_POLL
  #include <poll.h>
/* the following defines should be in <bits/poll.h> */
  #ifndef POLLRDNORM
  #define POLLRDNORM 0x040
  #endif
  #ifndef POLLWRNORM
  #define POLLWRNORM 0x100
  #endif
  #ifndef POLLERR
  #define POLLERR    0x008
  #endif
#else
  #ifdef HAVE_SELECT
    #define USE_SELECT_READ_WRITE 1
  #else
    #error "FIXME (ncarch.h): NetClass requires either select or poll call"
  #endif
#endif
#include <fcntl.h>
#include <map>
#include <vector>
#include <iterator>
#ifdef HAVE_THREADS
#include <pthread.h>
#endif
#define ncGetLastError() errno
#define NCSOCKET_ERROR -1
#define NCSD_SEND 1
#define NCSD_READ 0
#define ncSocketHandle int
#ifdef HAVE_POLL
  #define ncPollObject pollfd
#else
  #define ncPollObject fd_set
#endif
#define ncHostent struct hostent
#ifdef HAVE_GETHOSTBYNAME
  #define ncGetHostByName gethostbyname
#endif
#define ncCloseSocket ::close
#ifndef ncRecvFlag
  #define ncRecvFlag MSG_NOSIGNAL
#endif
#ifndef ncRecvPtr
  #define ncRecvPtr void *
#endif
#ifndef ncAcceptLenType
  #define ncAcceptLenType(a) a
#endif
#define ncSleep(a) usleep(a*1000)
#ifndef ncSockAddrIn
#define ncSockAddrIn struct sockaddr_in
#endif
#define ncAddrInUse (errno == EADDRINUSE)
#ifndef ncSockLenType
#define ncSockLenType socklen_t
#endif
#define ncSocketError(a) (a == -1)
#define ncIsBlockingError(a) ((a == EALREADY) || (a == EINPROGRESS) || (a == EAGAIN))
#define ncIsConnected(a) 0
#define ncSocketReset(a) ((a == EPIPE) || (a == ECONNRESET))
#define ncSetSocketNonBlocking(a) \
 fcntl(m_sockfd,F_SETFL,fcntl(a,F_GETFD) | O_NONBLOCK)
#define ncCallBackRetType void *
typedef void *(*ncCallBack)(void *ptr);
#define ncThreadHandle pthread_t
#define ncMutexHandle pthread_mutex_t
#ifndef ncGetInetAddr
#define ncGetInetAddr(host,addrPtr) inet_pton(AF_INET,host,addrPtr)
#endif
#ifndef ncValidInetAddr
#define ncValidInetAddr(addr) (addr > 0)
#endif
#endif
#endif /* __linux__ || USE_LINUX */

/***********************************************************
 * NetBSD specific includes and constants
 ***********************************************************/
#ifdef __NetBSD__
#error "FIXME (ncarch.h): NetBSD is not currently supported."
#endif

#if ((!defined WIN32) && (!defined __linux__) && (!defined __FreeBSD__))
#if ((!defined __OpenBSD__) && (!defined __NetBSD__) && (!defined __APPLE__))
#if (!defined __CYGWIN__)
#error "FIXME (ncarch.h): Unknown platform detected."
#endif
#endif
#endif

#if (!defined MIN)
#define MIN(a,b) ((a < b) ? a : b)
#endif

#endif /* __NCARCH_H */
