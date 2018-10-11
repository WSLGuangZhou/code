///////////////////////////////////////////////////////////////////////////////////////////
// Broadband Integrated Switching Platform(BISP) Basic Component
// Copyright(c) 2000-2008 Network Communications Company, 
//						  Haige Communications Group Incorporated Company.
// All rights reserved.
//
// This file is a part of the Broadband Integrated Switching Platform(BISP).
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
///////////////////////////////////////////////////////////////////////////////////////////
//跨平台的socket类封装
#ifndef __BISPSOCKET_H_INCLUDED
#define __BISPSOCKET_H_INCLUDED

//#ifndef BISP_CONFIG_NO_BISP_SOCKET_CLASS

/*lint -save -e1773 */

//#if defined(WIN32)||defined(_WINDOWS)
//#if(BISPOS_TYPE == BISPOS_WIN32)
#ifdef NSL_OS_WINDOWS
	#include <winsock2.h>
	//#include <errno.h>
// 	#define EWOULDBLOCK             WSAEWOULDBLOCK
// 	#define EINPROGRESS             WSAEINPROGRESS
// 	#define EALREADY                WSAEALREADY
// 	#define ENOTSOCK                WSAENOTSOCK
// 	#define EDESTADDRREQ            WSAEDESTADDRREQ
// 	#define EMSGSIZE                WSAEMSGSIZE
// 	#define EPROTOTYPE              WSAEPROTOTYPE
// 	#define ENOPROTOOPT             WSAENOPROTOOPT
// 	#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
// 	#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
// 	#define EOPNOTSUPP              WSAEOPNOTSUPP
// 	#define EPFNOSUPPORT            WSAEPFNOSUPPORT
// 	#define EAFNOSUPPORT            WSAEAFNOSUPPORT
// 	#define EADDRINUSE              WSAEADDRINUSE
// 	#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
// 	#define ENETDOWN                WSAENETDOWN
// 	#define ENETUNREACH             WSAENETUNREACH
// 	#define ENETRESET               WSAENETRESET
// 	#define ECONNABORTED            WSAECONNABORTED
// 	#define ECONNRESET              WSAECONNRESET
// 	#define ENOBUFS                 WSAENOBUFS
// 	#define EISCONN                 WSAEISCONN
// 	#define ENOTCONN                WSAENOTCONN
// 	#define ESHUTDOWN               WSAESHUTDOWN
// 	#define ETOOMANYREFS            WSAETOOMANYREFS
// 	#define ETIMEDOUT               WSAETIMEDOUT
// 	#define ECONNREFUSED            WSAECONNREFUSED
// 	#define ELOOP                   WSAELOOP
// 	//#define ENAMETOOLONG            WSAENAMETOOLONG
// 	#define EHOSTDOWN               WSAEHOSTDOWN
// 	#define EHOSTUNREACH            WSAEHOSTUNREACH
// 	//#define ENOTEMPTY               WSAENOTEMPTY
// 	#define EPROCLIM                WSAEPROCLIM
// 	#define EUSERS                  WSAEUSERS
// 	#define EDQUOT                  WSAEDQUOT
// 	#define ESTALE                  WSAESTALE
// 	#define EREMOTE                 WSAEREMOTE
	#pragma comment(lib,"WS2_32.lib")
	#pragma warning(disable:4042)
	#pragma warning(disable:4127)
	
	#define SOCK_IOCTRL(s,o,l) ioctlsocket(s,o,(unsigned long*)(l))
	#define SOCK_CLOSE(s) closesocket(s)
	
#endif //(BISPOS_TYPE == BISPOS_WIN32)

//#if(BISPOS_TYPE == BISPOS_VXWORKS)
#ifdef NSL_OS_VXWORKS
	#include "stdio.h"
	#include "errnoLib.h"
	#include "hostLib.h"
	#include "sockLib.h"
	#include "socket.h"
	#include "inetLib.h"
	#include "in.h"
	#include "selectLib.h"
	#include "ioLib.h"
	
	typedef int SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;

	#ifdef INVALID_SOCKET
	#undef INVALID_SOCKET
	#endif //INVALID_SOCKET

	#define INVALID_SOCKET  (SOCKET)(~0)

	#ifdef SOCKET_ERROR
	#undef SOCKET_ERROR
	#endif //SOCKET_ERROR

	#define SOCKET_ERROR            (-1)

	#define FD_READ         0x01
	#define FD_WRITE        0x02
	#define FD_OOB          0x04
	#define FD_ACCEPT       0x08
	#define FD_CONNECT      0x10
	#define FD_CLOSE        0x20

	#define SOCK_IOCTRL(s,o,l) ioctl(s,o,(int)(l))
	#define SOCK_CLOSE(s) close(s)

#endif //(BISPOS_TYPE == BISPOS_VXWORKS)

//#if(BISPOS_TYPE == BISPOS_LINUX)
#ifdef NSL_OS_LINUX
	typedef int SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;
	#ifdef INVALID_SOCKET
	#undef INVALID_SOCKET
	#endif //INVALID_SOCKET

	#define INVALID_SOCKET  (SOCKET)(~0)

	#ifdef SOCKET_ERROR
	#undef SOCKET_ERROR
	#endif //SOCKET_ERROR

	#define SOCKET_ERROR            (-1)
	#define ERROR					(-1)

	#define FD_READ         0x01
	#define FD_WRITE        0x02
	#define FD_OOB          0x04
	#define FD_ACCEPT       0x08
	#define FD_CONNECT      0x10
	#define FD_CLOSE        0x20

	#define SOCK_IOCTRL(s,o,l) ioctl(s,o,(int)(l))
	#define SOCK_CLOSE(s) close(s)

	typedef struct sockaddr SOCKADDR;
	typedef struct sockaddr_in SOCKADDR_IN;

#endif //(BISPOS_TYPE == BISPOS_LINUX)

// 
// #include "BISPDebug.h"
// 
// 
// template<bool t_bManaged>
// class CSocketT
// {
// public:
// 	SOCKET	m_hSocket;
// 	CSocketT(SOCKET hSocket = INVALID_SOCKET) :m_hSocket(hSocket)
// 	{
// 	}
// 	~CSocketT()
// 	{
// 		if(t_bManaged)
// 		{
// 			Close();
// 		}
// 	}
// 	void Close()
// 	{
// 		if (m_hSocket != INVALID_SOCKET)
// 		{
// #if defined(WIN32)||defined(_WINDOWS)
// 			/*int nError = */
// 			closesocket(m_hSocket);
// //			BISPASSERT(SOCKET_ERROR != nError);
// #else //VxWorks
// 			close(m_hSocket);
// #endif //defined(WIN32)||defined(_WINDOWS)
// 			m_hSocket = INVALID_SOCKET;
// 		}
// 	}
// 	SOCKET Attach(SOCKET hSocket)
// 	{
// 		BISPASSERT(hSocket!=INVALID_SOCKET);
// 		SOCKET hOld = m_hSocket;
// 		m_hSocket = hSocket;
// 		return hOld;
// 	}
// 	SOCKET Detach()
// 	{
// 		SOCKET hSocket = m_hSocket;
// 		m_hSocket = INVALID_SOCKET;
// 		return hSocket;
// 	}
// 	operator SOCKET() const
// 	{ 
// 		return m_hSocket; 
// 	}
// 	CSocketT& operator = (const SOCKET hSocket)
// 	{
// 		m_hSocket = hSocket;
// 		return *this;
// 	}
// 	CSocketT& operator = (const CSocketT& other)
// 	{
// 		if(this != &other)
// 		{
// 			m_hSocket = other.hSocket;
// 		}
// 		return *this;
// 	}
// 	BOOL Socket(int nAddressFormat = PF_INET,int nSocketType = SOCK_STREAM,
// 		int nProtocolType = 0 )
// 	{
// 		BISPASSERT(m_hSocket == INVALID_SOCKET);
// 		m_hSocket = socket(nAddressFormat,nSocketType,nProtocolType);
// 		if (m_hSocket != INVALID_SOCKET)
// 		{
// 			return TRUE;
// 		}
// 		return FALSE;	
// 	}
// 	BOOL Bind (const SOCKADDR* lpSockAddr, int nSockAddrLen)
// 	{
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return (SOCKET_ERROR != bind(m_hSocket,(SOCKADDR*)lpSockAddr,nSockAddrLen))? TRUE:FALSE;
// 	}
// 	BOOL Bind(UINT nSocketPort, LPCSTR lpszSocketAddress)
// 	{
// 		SOCKADDR_IN sockAddr = {0};
// 		sockAddr.sin_family = AF_INET;
// 
// //#if !(defined(WIN32)||defined(_WINDOWS))
// #if (BISPOS_TYPE == BISPOS_VXWORKS)
// 		sockAddr.sin_len = sizeof(SOCKADDR_IN);
// #endif //!(defined(WIN32)||defined(_WINDOWS))
// 
// 		if (lpszSocketAddress == NULL)
// 		{
// 			sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
// 		}
// 		else
// 		{
// 			DWORD lResult = inet_addr((char*)lpszSocketAddress);
// 			if (lResult == INADDR_NONE)
// 			{
// 				return FALSE;
// 			}
// 			sockAddr.sin_addr.s_addr = lResult;
// 		}
// 		sockAddr.sin_port = htons((u_short)nSocketPort);
// 		return Bind((SOCKADDR*)&sockAddr, sizeof(sockAddr));
// 	}
// 	BOOL Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
// 	{
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return (SOCKET_ERROR!=connect(m_hSocket,(SOCKADDR*)lpSockAddr,nSockAddrLen));
// 	}
// 	BOOL Connect(LPCSTR lpszHostAddress, UINT nHostPort)
// 	{
// 		BISPASSERT(lpszHostAddress != NULL);
// 		SOCKADDR_IN sockAddr = {0};
// 		sockAddr.sin_family = AF_INET;
// 		sockAddr.sin_addr.s_addr = inet_addr((char*)lpszHostAddress);
// //#if !(defined(WIN32)||defined(_WINDOWS))
// #if (BISPOS_TYPE == BISPOS_VXWORKS)
// 		sockAddr.sin_len = sizeof(SOCKADDR_IN);
// #endif //!(defined(WIN32)||defined(_WINDOWS))
// 		//计算地址
// 		if (sockAddr.sin_addr.s_addr == INADDR_NONE)
// 		{
// 			sockAddr.sin_addr.s_addr = (u_long)GetHostByName(lpszHostAddress);
// 			if (sockAddr.sin_addr.s_addr == INADDR_NONE)
// 			{
// 				return FALSE;
// 			}
// 		}
// 		sockAddr.sin_port = htons((u_short)nHostPort);
// 		return Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
// 	}
// 	SOCKET Accept(SOCKADDR* lpSockAddr,int* lpSockAddrLen)
// 	{
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return accept(m_hSocket,lpSockAddr,lpSockAddrLen);
// 	}
// 	BOOL GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
// 	{ 
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return (SOCKET_ERROR != getpeername(m_hSocket, lpSockAddr, lpSockAddrLen)); 
// 	}
// 	BOOL GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
// 	{ 
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return (SOCKET_ERROR != getsockname(m_hSocket, lpSockAddr, lpSockAddrLen)); 
// 	}
// 	DWORD GetPeerIP()
// 	{
// 		SOCKADDR_IN sa_in;
// 		int nLen = sizeof(sa_in);
// 		if(GetPeerName((SOCKADDR*)&sa_in,&nLen))
// 		{
// 			return sa_in.sin_addr.s_addr;
// 		}
// 		return 0;
// 	}
// 	DWORD GetSockIP()
// 	{
// 		SOCKADDR_IN sa_in;
// 		int nLen = sizeof(sa_in);
// 		if(GetSockName((SOCKADDR*)&sa_in,&nLen))
// 		{
// 			return sa_in.sin_addr.s_addr;
// 		}
// 		return 0;		
// 	}
// 	BOOL SetSockOpt(int nOptionName, const void* lpOptionValue,
// 		int nOptionLen, int nLevel = SOL_SOCKET)
// 	{
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return setsockopt(m_hSocket,nLevel,nOptionName,(char*)lpOptionValue,nOptionLen)!=SOCKET_ERROR;
// 	}
// 	BOOL GetSockOpt(int nOptionName, void* lpOptionValue,
// 		int* lpOptionLen, int nLevel = SOL_SOCKET)
// 	{
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return getsockopt(m_hSocket,nLevel,nOptionName,(char*)lpOptionValue,lpOptionLen)!=SOCKET_ERROR;
// 	}
// 	BOOL IOCtl(long lCommand, DWORD* lpArgument)
// 	{ 
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// //#if defined(WIN32)||defined(_WINDOWS)
// #if (BISPOS_TYPE == BISPOS_WIN32)
// 		return (SOCKET_ERROR != ioctlsocket(m_hSocket, lCommand, lpArgument)); 
// #else
// 		return ERROR  != ioctl(m_hSocket,lCommand,(int)lpArgument);
// #endif //defined(WIN32)||defined(_WINDOWS)
// 	}
// 	BOOL Listen(int nConnectionBacklog = 5)
// 	{ 
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return (SOCKET_ERROR != listen(m_hSocket, nConnectionBacklog)); 
// 	}
// 	enum { receives = 0, sends = 1, both = 2 };
// 	BOOL ShutDown(int nHow = sends)
// 	{ 
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return (SOCKET_ERROR != shutdown(m_hSocket,nHow)); 
// 	}
// 	int Send(const void* lpBuf, int nBufLen, int nFlags = 0)
// 	{
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return send(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
// 	}
// 	int Receive(void* lpBuf, int nBufLen, int nFlags = 0)
// 	{
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return recv(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags);
// 	}
// 	int SendTo(const void* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = 0)
// 	{ 
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return sendto(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, (SOCKADDR*)lpSockAddr, nSockAddrLen);
// 	}
// 	int ReceiveFrom(void* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = 0)
// 	{
// 		BISPASSERT(m_hSocket != INVALID_SOCKET);
// 		return recvfrom(m_hSocket, (LPSTR)lpBuf, nBufLen, nFlags, lpSockAddr, lpSockAddrLen);
// 	}
// 	BOOL SetBlock(BOOL bBlock = TRUE)
// 	{
// 		DWORD dwFlags = bBlock? 0:1;
// 		return IOCtl(FIONBIO,(DWORD*)&dwFlags);
// 	}
// 	static int GetHostByName(LPCSTR lpszHostName)
// 	{
// //#if defined(WIN32)||defined(_WINDOWS)
// #if (BISPOS_TYPE == BISPOS_WIN32)
// 			LPHOSTENT lphost = gethostbyname(lpszHostName);
// 			return (int)((lphost != NULL)? (((LPIN_ADDR)lphost->h_addr)->s_addr):INADDR_NONE);
// #elif (BISPOS_TYPE == BISPOS_VXWORKS)
// 			return hostGetByName((char*)lpszHostName);
// #elif (BISPOS_TYPE == BISPOS_LINUX)
// 		hostent* lphost = gethostbyname(lpszHostName);
// 		return (int)((lphost != NULL)? (((in_addr*)lphost->h_addr)->s_addr):INADDR_NONE);
// #endif //defined(WIN32)||defined(_WINDOWS)
// 	}
// 	static int GetHostName (char *name, int nameLen)
// 	{
// 		return gethostname (name,  nameLen);
// 	}
// 	static DWORD GetLastError()
// 	{
// #if defined(WIN32)||defined(_WINDOWS)
// 		return WSAGetLastError();
// #else
// 		return (DWORD)errno;
// #endif //defined(WIN32)||defined(_WINDOWS)
// 	}
// 
// };
// 
// #define BISP_GET_LAST_ERROR CSocketHandle::GetLastError()
// 
// typedef CSocketT<true>	CBISPSocket;
// typedef CSocketT<false> CSocketHandle;
// #ifndef __AFXSOCK_H__
// typedef CBISPSocket CSocket;
// #endif //__AFXSOCK_H__
// 
// 
// /*lint -restore */
// 
// #endif /* BISP_CONFIG_NO_BISP_SOCKET_CLASS */



#endif //__BISPSOCKET_H_INCLUDED

