/*
 * File: include/nsl/core/nsl_socket.h
 *
 * Defines of socket in Cross-Platform Support.
 *
 * Copyright (C) 2012-2014 sl.cai(noirscsl@163.com). All rights reserved.
 *
 * This file is a part of the nsl toolkit.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * modification history
 *--------------------
 * 06/29/2012,      1.0,  sl.cai   Created
 * 05/22/2014,      1.1,  sl.cai   Add nsl_setsockopt_XXX api
 * 07/02/2014,      1.2,  sl.cai   Add nsl_ethernet_send_raw_packet
 * 
 */

#ifndef NSL_INCLUDE_CORE_NSL_SOCKET_H_
#define NSL_INCLUDE_CORE_NSL_SOCKET_H_

#ifdef NSL_OS_WINDOWS

typedef SOCKET nsl_socket_t;

#define NSL_SOCK_STARTUP nsl_winsock_startup()

#define NSL_SOCK_CLEANUP nsl_winsock_cleanup()

#define NSL_SOCK_IOCTRL(s,o,l) ioctlsocket(s,o,(unsigned long*)(l))

#define NSL_SOCK_CLOSE(s) closesocket(s)

#define NSL_SOCK_IS_VALID(s) (SOCKET_ERROR != (s))

#define NSL_SOCK_IS_INVALID(s) (SOCKET_ERROR == (s))

#define NSL_SOCK_ERRNO (GetLastError())

#else

typedef int nsl_socket_t;

#define NSL_SOCK_STARTUP

#define NSL_SOCK_CLEANUP

#define NSL_SOCK_IOCTRL(s,o,l) ioctl(s,o,(int)(l))

#define NSL_SOCK_CLOSE(s) close(s)

#define NSL_SOCK_IS_VALID(s) ((s) >= 0)

#define NSL_SOCK_IS_INVALID(s) ((s) < 0)

#define NSL_SOCK_ERRNO (errno)

#define SOCKET_ERROR -1

#define INVALID_SOCKET SOCKET_ERROR

#endif /* NSL_OS_WINDOWS */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#ifdef NSL_OS_WINDOWS
char* nsl_winsock_startup(void);
void nsl_winsock_cleanup(void);
#endif /* NSL_OS_WINDOWS */

int nsl_setsockopt_ipv4_multicast(int sock,
                                  int optname,
                                  unsigned int mcast_addr,
                                  unsigned int ifindex,
                                  unsigned int ifaddr);

int nsl_setsockopt_ipv4_multicast_if(int sock, unsigned int ifindex);

int nsl_setsockopt_ipv4_multicast_loop(int sock, int on);

int nsl_setsockopt_ipv4_multicast_ttl(int sock, int ttl);

int nsl_setsockopt_broadcast(int sock, int on);

int nsl_setsockopt_reuseaddr(int sock);

int nsl_setsockopt_reuseport(int sock);

int nsl_setsockopt_ipv4_tos(int sock, int tos);

int nsl_setsockopt_so_recvbuf(int sock, int size);

int nsl_setsockopt_so_sendbuf(int sock, int size);

int nsl_setsockopt_pktinfo(int sock);

int nsl_setsockopt_bindtodevice(int sock, const char *ifname);

/* Ethernet Protocol ID */
#ifdef NSL_OS_LINUX
#ifndef NSL_ETHER_SEND_RAW_PACKET
#define NSL_ETHER_SEND_RAW_PACKET
/******************************************************************************
NAME
    nsl_ethernet_send_raw_packet.

DESCRIPTION
    The nsl_ethernet_send_raw_packet() function shall send raw packet on
    ethernet interface.

    The nsl_ethernet_send_raw_packet() function takes the following arguments:
    raw
        Points to a buffer containing the raw message to be sent. Note raw packet
        without ethernet head.
    raw_len
        Specifies the size of the raw message in bytes.
    mac
        Points to a buffer containing the MAC address of destination.
    protocol_id
        Specifies the Ethernet Protocol ID in Host order.
        #include <linux/if_ether.h>
    ifindex
        Specifies the index of ethernet interface.

RETURN VALUE
    Upon successful completion, nsl_ethernet_send_raw_packet() shall return
    the number of bytes sent. Otherwise, -1 shall be returned and errno set
    to indicate the error.

******************************************************************************/
int nsl_ethernet_send_raw_packet(uint8_t *raw, int raw_len, uint8_t *mac, uint16_t protocol_id, int ifindex);
#endif /* NSL_ETHER_SEND_RAW_PACKET */
#endif /* NSL_OS_LINUX */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* NSL_INCLUDE_CORE_NSL_SOCKET_H_ */

