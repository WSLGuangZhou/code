/*
 * File: nsl/src/core/nsl_socket.c
 *
 * Defines of socket in Cross-Platform Support.
 *
 * This file is a part of the nsl.
 *
 * Author: sl.cai(noirscsl@163.com).
 *
 * Use of this source code is governed by the MIT license.
 * that can be found in the License file.
 */

// #include <nsl/nsl.h>
// #include <sys/ioctl.h>
// #include <nsl/core/nsl_socket.h>

#include "DTSCommon.h"
#include "nsl_socket.h"

#ifdef NSL_OS_LINUX
	#include <sys/ioctl.h>
#elif defined NSL_OS_WINDOWS
#endif

#ifndef NSL_OS_VXWORKS
#include <fcntl.h>
#endif /* NSL_OS_VXWORKS */

#ifdef NSL_OS_WINDOWS

char* nsl_winsock_startup(void)
{
    WORD            VersionRequested;
    WSADATA         stWSAData;
    int             i;
    static char     errmsg[100];

    /* winsock 1: use MAKEWORD(1,1) */
    /* winsock 2: use MAKEWORD(2,2) */
    VersionRequested = MAKEWORD(2,2);
    i = WSAStartup(VersionRequested, &stWSAData);
    if (i != 0) {
        if (i == WSAVERNOTSUPPORTED) {
            sprintf(errmsg,
            "Unable to init. socket lib, does not support 1.1");
        } else {
            sprintf(errmsg, "Socket Startup error %d", i);
        }
        return (errmsg);
    }
    return (NULL);
}

void nsl_winsock_cleanup(void)
{
    WSACleanup();
}

#endif /* NSL_OS_WINDOWS */


/*
 * Process multicast socket options for IPv4 in an OS-dependent manner.
 * Supported options are IP_{ADD,DROP}_MEMBERSHIP.
 *
 * Many operating systems have a limit on the number of groups that
 * can be joined per socket (where each group and local address
 * counts).  This impacts OSPF, which joins groups on each interface
 * using a single socket.  The limit is typically 20, derived from the
 * original BSD multicast implementation.  Some systems have
 * mechanisms for increasing this limit.
 *
 * In many 4.4BSD-derived systems, multicast group operations are not
 * allowed on interfaces that are not UP.  Thus, a previous attempt to
 * leave the group may have failed, leaving it still joined, and we
 * drop/join quietly to recover.  This may not be necessary, but aims to
 * defend against unknown behavior in that we will still return an error
 * if the second join fails.  It is not clear how other systems
 * (e.g. Linux, Solaris) behave when leaving groups on down interfaces,
 * but this behavior should not be harmful if they behave the same way,
 * allow leaves, or implicitly leave all groups joined to down interfaces.
 */
int nsl_setsockopt_ipv4_multicast(int sock,
                                  int optname,
                                  unsigned int mcast_addr,
                                  unsigned int ifindex,
                                  unsigned int ifaddr)
{
#ifdef NSL_HAVE_RFC3678
    struct group_req gr;
    struct sockaddr_in *si;
    int ret;
    memset(&gr, 0, sizeof(gr));
    si = (struct sockaddr_in *)&gr.gr_group;
    gr.gr_interface = ifindex;
    si->sin_family = AF_INET;
#ifdef NSL_HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
    si->sin_len = sizeof(struct sockaddr_in);
#endif /* NSL_HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */
    si->sin_addr.s_addr = mcast_addr;
    ret = setsockopt(sock, IPPROTO_IP,
                     (optname == IP_ADD_MEMBERSHIP) ? MCAST_JOIN_GROUP : MCAST_LEAVE_GROUP,
                     (void *)&gr, sizeof(gr));
    if ((ret < 0) && (optname == IP_ADD_MEMBERSHIP) && (errno == EADDRINUSE)) {
        setsockopt(sock, IPPROTO_IP, MCAST_LEAVE_GROUP, (void *)&gr,
                   sizeof(gr));
        ret = setsockopt(sock, IPPROTO_IP, MCAST_JOIN_GROUP, (void *)&gr,
                         sizeof(gr));
    }

    if (ret < 0) {
        if (0 == ifaddr) {
        }
        printf("can't set sockopt multicast ifindex = %d to socket %d\n", ifindex, sock);
    }

    return ret;

#elif defined(HAVE_STRUCT_IP_MREQN_IMR_IFINDEX) && !defined(__FreeBSD__)
    struct ip_mreqn mreqn;
    int ret;

    assert(optname == IP_ADD_MEMBERSHIP || optname == IP_DROP_MEMBERSHIP);
    memset (&mreqn, 0, sizeof(mreqn));

    mreqn.imr_multiaddr.s_addr = mcast_addr;
    mreqn.imr_ifindex = ifindex;

    ret = setsockopt(sock, IPPROTO_IP, optname,
        (void *)&mreqn, sizeof(mreqn));
    if ((ret < 0) && (optname == IP_ADD_MEMBERSHIP) && (errno == EADDRINUSE))
    {
        /* see above: handle possible problem when interface comes back up */
        char buf[1][INET_ADDRSTRLEN];
        printf("setsockopt_ipv4_multicast attempting to drop and "
            "re-add (fd %d, mcast %s, ifindex %u)",
            sock,
            inet_ntop(AF_INET, &mreqn.imr_multiaddr,
                buf[0], sizeof(buf[0])), ifindex);
        setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
            (void *)&mreqn, sizeof(mreqn));
        ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
            (void *)&mreqn, sizeof(mreqn));
    }
    return ret;

    /* Example defines for another OS, boilerplate off other code in this
     function, AND handle optname as per other sections for consistency !! */
    /* #elif  defined(BOGON_NIX) && EXAMPLE_VERSION_CODE > -100000 */
    /* Add your favourite OS here! */

#elif defined(HAVE_BSD_STRUCT_IP_MREQ_HACK) /* #if OS_TYPE */
    /* standard BSD API */

    struct in_addr m;
    struct ip_mreq mreq;
    int ret;

    assert(optname == IP_ADD_MEMBERSHIP || optname == IP_DROP_MEMBERSHIP);

    m.s_addr = htonl(ifindex);

    memset (&mreq, 0, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = mcast_addr;
    mreq.imr_interface = m;

    ret = setsockopt (sock, IPPROTO_IP, optname, (void *)&mreq, sizeof(mreq));
    if ((ret < 0) && (optname == IP_ADD_MEMBERSHIP) && (errno == EADDRINUSE))
    {
        /* see above: handle possible problem when interface comes back up */
        char buf[1][INET_ADDRSTRLEN];
        printf("setsockopt_ipv4_multicast attempting to drop and "
               "re-add (fd %d, mcast %s, ifindex %u)",
               sock,
               inet_ntop(AF_INET, &mreq.imr_multiaddr,
               buf[0], sizeof(buf[0])), ifindex);
        setsockopt (sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                    (void *)&mreq, sizeof(mreq));
        ret = setsockopt (sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                          (void *)&mreq, sizeof(mreq));
    }
    return ret;

#elif defined(NSL_OS_VXWORKS) && defined(ROUTER_STACK)
    struct ip_mreq mreq;
    int ret;

    memset(&mreq, 0, sizeof(struct ip_mreq));
    mreq.imr_multiaddr.s_addr = mcast_addr;
    mreq.imr_index = (int)ifindex;

    if (optname == IP_ADD_MEMBERSHIP) {
        ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP_INDEX, (char *)&mreq, sizeof(mreq));
        if (OK != ret) {
            mreq.imr_interface.s_addr = ifaddr;
            ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
        }
    } else {
        ret = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP_INDEX, (char *)&mreq, sizeof(mreq));
        if (OK != ret) {
            mreq.imr_interface.s_addr = ifaddr;
            ret = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq));
        }
    }
    return ret;
#else
//	#error "Unsupported multicast API"
#endif /* #if OS_TYPE */

}

/*
 * Set IP_MULTICAST_IF socket option in an OS-dependent manner.
 */
int nsl_setsockopt_ipv4_multicast_if(int sock, unsigned int ifindex)
{

#ifdef NSL_HAVE_STRUCT_IP_MREQN_IMR_IFINDEX
    struct ip_mreqn mreqn;
    memset(&mreqn, 0, sizeof(mreqn));

    mreqn.imr_ifindex = (int)ifindex;
    return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (void *)&mreqn,
                      sizeof(mreqn));

    /* Example defines for another OS, boilerplate off other code in this
     function */
    /* #elif  defined(BOGON_NIX) && EXAMPLE_VERSION_CODE > -100000 */
    /* Add your favourite OS here! */
#elif defined(HAVE_BSD_STRUCT_IP_MREQ_HACK)
    struct in_addr m;

    m.s_addr = htonl(ifindex);

    return setsockopt (sock, IPPROTO_IP, IP_MULTICAST_IF, (void *)&m, sizeof(m));
#elif defined(NSL_OS_VXWORKS) && defined(ROUTER_STACK)
    short _ifindex = (short)ifindex;
    return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IFINDEX, (char *)&_ifindex, sizeof(short));
#else
//	#error "Unsupported multicast API"
#endif /* NSL_HAVE_STRUCT_IP_MREQN_IMR_IFINDEX */
}


int nsl_setsockopt_ipv4_multicast_loop(int sock, int on)
{
    int ret;
    char val = (char)on;
    ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (void *)&val,
                     sizeof(char));
    if (ret < 0) {
        printf("can't set sockopt IP_MULTICAST_LOOP to socket %d\n", sock);
    }
    return ret;
}

int nsl_setsockopt_ipv4_multicast_ttl(int sock, int ttl)
{
    int ret;
    uint8_t val = (uint8_t)ttl;
    ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&val,
                     sizeof(uint8_t));
    if (ret < 0) {
        printf("can't set sockopt IP_MULTICAST_TTL %d to socket %d\n", ttl, sock);
    }
    return ret;
}

/* Utility function to set boradcast option to the socket. */
int nsl_setsockopt_broadcast(int sock, int on)
{
    int ret;

    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(int));
    if (ret < 0) {
        printf("can't set sockopt SO_BROADCAST to socket %d\n", sock);
    }
    return ret;
}

int nsl_setsockopt_reuseaddr(int sock)
{
    int ret;
    int on = 1;

    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(int));
    if (ret < 0) {
        printf("can't set sockopt SO_REUSEADDR to socket %d\n", sock);
    }
    return ret;
}

#ifdef SO_REUSEPORT
int nsl_setsockopt_reuseport(int sock)
{
    int ret;
    int on = 1;

    ret = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void *)&on, sizeof(int));
    if (ret < 0) {
        printf("can't set sockopt SO_REUSEPORT to socket %d\n", sock);
    }
    return ret;
}
#else
int nsl_setsockopt_reuseport(int sock)
{
    if (SOCKET_ERROR == sock) {

    }
    return 0;
}
#endif /* SO_REUSEPORT */

int nsl_setsockopt_ipv4_tos(int sock, int tos)
{
    int ret = 0;
#ifndef NSL_OS_VXWORKS
    ret = setsockopt(sock, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    if (ret < 0) {
        printf("Can't set IP_TOS option for fd %d to %#x: %s\n", sock, tos,
               NSL_STR_ERROR(errno));
    }
#endif /* NSL_OS_VXWORKS */
    return ret;
}

int nsl_setsockopt_so_recvbuf(int sock, int size)
{
    int ret;

    ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(int));
    if (ret < 0) {
        printf("fd %d: can't setsockopt SO_RCVBUF to %d: %s\n", sock, size,
               NSL_STR_ERROR(errno));
    }

    return ret;
}

int nsl_setsockopt_so_sendbuf(int sock, int size)
{
    int ret;

    ret = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(int));
    if (ret < 0) {
        printf("fd %d: can't setsockopt SO_SNDBUF to %d: %s\n", sock, size,
               NSL_STR_ERROR(errno));
    }

    return ret;
}

int nsl_setsockopt_pktinfo(int sock)
{
    int ret = 0;
#ifndef NSL_OS_VXWORKS
    int val = 1;

    ret = setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &val, sizeof(val));
    if (ret < 0) {
        printf("Can't setsockopt IP_PKTINFO : %s\n", NSL_STR_ERROR(errno));
    }
#endif /* NSL_OS_VXWORKS */
    return ret;
}

int nsl_setsockopt_bindtodevice(int sock, const char *ifname)
{
    int ret = 0;
#ifdef SO_BINDTODEVICE
    struct ifreq ifr;

#ifndef IFNAMSIZ
    enum { IFNAMSIZ = 16 };
#endif /* IFNAMSIZ */

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    /* NB: passing (iface, strlen(iface) + 1) does not work!
     * (maybe it works on _some_ kernels, but not on 2.6.26)
     * Actually, ifr_name is at offset 0, and in practice
     * just giving char[IFNAMSIZ] instead of struct ifreq works too.
     * But just in case it's not true on some obscure arch... */
    ret = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr));
    if (ret < 0) {
        printf("Can't bind to interface %s%s", ifname, __LD__);
    }
#else
    if ((NULL == ifname) || (SOCKET_ERROR == sock)) {

    }
#endif /* SO_BINDTODEVICE */
    return ret;
}

int nsl_setsock_block(int fd, int block)
{
#ifdef NSL_OS_VXWORKS
    int flag = (block > 0) ? 0 : 1;
    return ioctl(fd, (int)FIONBIO, &flag);
#else
    int option = fcntl(fd, F_GETFL);
    if (block > 0) {
        option &= ~O_NONBLOCK;
    } else {
        option |= O_NONBLOCK;
    }
    return fcntl(fd, F_SETFL, option);
#endif /* NSL_OS_VXWORKS */
}

#ifndef NSL_OS_VXWORKS
int nsl_setfd_cloexec(int fd)
{
    int option = fcntl(fd, F_GETFL);
    option |= FD_CLOEXEC;
    return fcntl(fd, F_SETFD, option);
}

int nsl_setfd_noblock_cloexec(int fd)
{
    int option = fcntl(fd, F_GETFL);
    option |= (FD_CLOEXEC|O_NONBLOCK);
    return fcntl(fd, F_SETFD, option);
}

#endif /* NSL_OS_VXWORKS */

#ifdef NSL_OS_LINUX
#include <linux/if_packet.h>
int nsl_ethernet_send_raw_packet(uint8_t *raw, int raw_len,
                                 uint8_t *mac, uint16_t protocol_id,
                                 int ifindex)
{
    int ret;
    int fd;
    struct sockaddr_ll dest_sll;

    if ((fd = socket(PF_PACKET, SOCK_DGRAM, htons(protocol_id))) < 0) {
        return -1;
    }

    memset(&dest_sll, 0, sizeof(dest_sll));
    dest_sll.sll_family = AF_PACKET;
    dest_sll.sll_protocol = htons(protocol_id);
    dest_sll.sll_ifindex = ifindex;
    dest_sll.sll_halen = 6;
    memcpy(dest_sll.sll_addr, mac, 6);

    if ((ret =
        bind(fd, (struct sockaddr *)&dest_sll, sizeof(dest_sll))) == 0) {
        ret = (int)sendto(fd, raw, (size_t)raw_len, 0,
                          (struct sockaddr *)&dest_sll, sizeof(dest_sll));
    }

    close(fd);
    return ret;
}
#endif /* NSL_OS_LINUX */
