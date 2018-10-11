/********************************************************************
	created:	2018/04/20	13:48
	filename: 	DTSOpenFlowCommon.h
	author:		sl.wu
	
	purpose:	OpenFlow通用定义
*********************************************************************/

#ifndef DTSOpenFlowCommon_h__
#define DTSOpenFlowCommon_h__

/* Version number:
 * OpenFlow versions released: 0x01 = 1.0 ; 0x02 = 1.1 ; 0x03 = 1.2
 *     0x04 = 1.3
 */
/* The most significant bit in the version field is reserved and must
 * be set to zero.
 */

enum ofp_version {
	OFP10_VERSION = 0x01,
	OFP11_VERSION = 0x02,
	OFP12_VERSION = 0x03,
	OFP13_VERSION = 0x04,
	OFP14_VERSION = 0x05,
	OFP15_VERSION = 0x06,
	OFP16_VERSION = 0x07
};

#define PIPELINE_TABLES 64
#define OFP_MAX_TABLE_NAME_LEN 32
#define OFP_MAX_PORT_NAME_LEN  16
/* Official IANA registered port for OpenFlow. */
#define OFP_TCP_PORT  6653
#define OFP_SSL_PORT  6653

#define OFP_ETH_ALEN 6          /* Bytes in an Ethernet address. */

/* Port numbering. Ports are numbered starting from 1. */
enum ofp_port_no {
    /* Maximum number of physical and logical switch ports. */
    OFPP_MAX        = 0xffffff00,

    /* Reserved OpenFlow Port (fake output "ports"). */
    OFPP_IN_PORT    = 0xfffffff8,  /* Send the packet out the input port.  This
                                      reserved port must be explicitly used
                                      in order to send back out of the input
                                      port. */
    OFPP_TABLE      = 0xfffffff9,  /* Submit the packet to the first flow table
                                      NB: This destination port can only be
                                      used in packet-out messages. */
    OFPP_NORMAL     = 0xfffffffa,  /* Forward using non-OpenFlow pipeline. */
    OFPP_FLOOD      = 0xfffffffb,  /* Flood using non-OpenFlow pipeline. */
    OFPP_ALL        = 0xfffffffc,  /* All standard ports except input port. */
    OFPP_CONTROLLER = 0xfffffffd,  /* Send to controller. */
    OFPP_LOCAL      = 0xfffffffe,  /* Local openflow "port". */
    OFPP_ANY        = 0xffffffff   /* Special value used in some requests when
                                      no port is specified (i.e. wildcarded). */
};


/* Header on all OpenFlow packets. */
struct ofp_header {
    BYTE bVersion;    /* OFP_VERSION. */
    BYTE bType;       /* One of the OFPT_ constants. */
    WORD wLength;    /* Length including this ofp_header. */
    DWORD dwXid;       /* Transaction id associated with this packet.
                           Replies use the same id as was in the request
                           to facilitate pairing. */
}PACK_STRUCT;

enum ofp_type {
	/* Immutable messages. */
	OFPT_HELLO              = 0,  /* Symmetric message */
	OFPT_ERROR              = 1,  /* Symmetric message */
	OFPT_ECHO_REQUEST       = 2,  /* Symmetric message */
	OFPT_ECHO_REPLY         = 3,  /* Symmetric message */
	OFPT_EXPERIMENTER       = 4,  /* Symmetric message */

	/* Switch configuration messages. */
	OFPT_FEATURES_REQUEST   = 5,  /* Controller/switch message */
	OFPT_FEATURES_REPLY     = 6,  /* Controller/switch message */
	OFPT_GET_CONFIG_REQUEST = 7,  /* Controller/switch message */
	OFPT_GET_CONFIG_REPLY   = 8,  /* Controller/switch message */
	OFPT_SET_CONFIG         = 9,  /* Controller/switch message */

	/* Asynchronous messages. */
	OFPT_PACKET_IN          = 10, /* Async message */
	OFPT_FLOW_REMOVED       = 11, /* Async message */
	OFPT_PORT_STATUS        = 12, /* Async message */

	/* Controller command messages. */
	OFPT_PACKET_OUT         = 13, /* Controller/switch message */
	OFPT_FLOW_MOD           = 14, /* Controller/switch message */
	OFPT_GROUP_MOD          = 15, /* Controller/switch message */
	OFPT_PORT_MOD           = 16, /* Controller/switch message */
	OFPT_TABLE_MOD          = 17, /* Controller/switch message */

	/* Multipart messages. */
	OFPT_MULTIPART_REQUEST      = 18, /* Controller/switch message */
	OFPT_MULTIPART_REPLY        = 19, /* Controller/switch message */

	/* Barrier messages. */
	OFPT_BARRIER_REQUEST    = 20, /* Controller/switch message */
	OFPT_BARRIER_REPLY      = 21, /* Controller/switch message */

	/* Queue Configuration messages. */
	OFPT_QUEUE_GET_CONFIG_REQUEST = 22,  /* Controller/switch message */
	OFPT_QUEUE_GET_CONFIG_REPLY   = 23,  /* Controller/switch message */

	/* Controller role change request messages. */
	OFPT_ROLE_REQUEST       = 24, /* Controller/switch message */
	OFPT_ROLE_REPLY         = 25, /* Controller/switch message */

	/* Asynchronous message configuration. */
	OFPT_GET_ASYNC_REQUEST  = 26, /* Controller/switch message */
	OFPT_GET_ASYNC_REPLY    = 27, /* Controller/switch message */
	OFPT_SET_ASYNC          = 28, /* Controller/switch message */

	/* Meters and rate limiters configuration messages. */
	OFPT_METER_MOD          = 29, /* Controller/switch message */
};



//边缘交换机的出口使用的消息
//#define OFPT_SET_PRIVATE            ((uint8_t)100)


/* Special buffer-id to indicate 'no buffer' */
#define OFP_NO_BUFFER 0xffffffff

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//Match
/* Fields to match against flows */
struct ofp_match {
    uint16_t wType;             /* One of OFPMT_* */
    uint16_t wLength;           /* Length of ofp_match (excluding padding) */
    /* Followed by:
     *   - Exactly (length - 4) (possibly 0) bytes containing OXM TLVs, then
     *   - Exactly ((length + 7)/8*8 - length) (between 0 and 7) bytes of
     *     all-zero bytes
     * In summary, ofp_match is padded as needed, to make its overall size
     * a multiple of 8, to preserve alignment in structures using it.
     */
//  uint8_t oxm_fields[0];     /* 0 or more OXM match fields */
    uint8_t pad[4];            /* Zero bytes - see above for sizing */
}PACK_STRUCT;


enum ofp_match_type {
	OFPMT_STANDARD = 0,       /* Deprecated. */
	OFPMT_OXM      = 1       /* OpenFlow Extensible Match */
};


/* Components of a OXM TLV header.
 * Those macros are not valid for the experimenter class, macros for the
 * experimenter class will depend on the experimenter header used. */
#define OXM_HEADER__(CLASS, FIELD, HASMASK, LENGTH) \
	(((CLASS) << 16) | ((FIELD) << 9) | ((HASMASK) << 8) | (LENGTH))
#define OXM_HEADER(CLASS, FIELD, LENGTH) \
	OXM_HEADER__(CLASS, FIELD, 0, LENGTH)
#define OXM_HEADER_W(CLASS, FIELD, LENGTH) \
	OXM_HEADER__(CLASS, FIELD, 1, (LENGTH) * 2)
#define OXM_CLASS(HEADER) ((HEADER) >> 16)
#define OXM_FIELD(HEADER) (((HEADER) >> 9) & 0x7f)
#define OXM_TYPE(HEADER) (((HEADER) >> 9) & 0x7fffff)
#define OXM_HASMASK(HEADER) (((HEADER) >> 8) & 1)
#define OXM_LENGTH(HEADER) ((HEADER) & 0xff)

#define OXM_MAKE_WILD_HEADER(HEADER) \
	OXM_HEADER_W(OXM_CLASS(HEADER), OXM_FIELD(HEADER), OXM_LENGTH(HEADER))

/* OXM Class IDs.
 * The high order bit differentiate reserved classes from member classes.
 * Classes 0x0000 to 0x7FFF are member classes, allocated by ONF.
 * Classes 0x8000 to 0xFFFE are reserved classes, reserved for standardisation.
 */
enum ofp_oxm_class {
    OFPXMC_NXM_0          = 0x0000,    /* Backward compatibility with NXM */
    OFPXMC_NXM_1          = 0x0001,    /* Backward compatibility with NXM */
    OFPXMC_OPENFLOW_BASIC = 0x8000,    /* Basic class for OpenFlow */
    OFPXMC_EXPERIMENTER   = 0xFFFF,    /* Experimenter class */
};

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_ofb_match_fields {
    OFPXMT_OFB_IN_PORT        = 0,  /* Switch input port. */
    OFPXMT_OFB_IN_PHY_PORT    = 1,  /* Switch physical input port. */
    OFPXMT_OFB_METADATA       = 2,  /* Metadata passed between tables. */
    OFPXMT_OFB_ETH_DST        = 3,  /* Ethernet destination address. */
    OFPXMT_OFB_ETH_SRC        = 4,  /* Ethernet source address. */
    OFPXMT_OFB_ETH_TYPE       = 5,  /* Ethernet frame type. */
    OFPXMT_OFB_VLAN_VID       = 6,  /* VLAN id. */
    OFPXMT_OFB_VLAN_PCP       = 7,  /* VLAN priority. */
    OFPXMT_OFB_IP_DSCP        = 8,  /* IP DSCP (6 bits in ToS field). */
    OFPXMT_OFB_IP_ECN         = 9,  /* IP ECN (2 bits in ToS field). */
    OFPXMT_OFB_IP_PROTO       = 10, /* IP protocol. */
    OFPXMT_OFB_IPV4_SRC       = 11, /* IPv4 source address. */
    OFPXMT_OFB_IPV4_DST       = 12, /* IPv4 destination address. */
    OFPXMT_OFB_TCP_SRC        = 13, /* TCP source port. */
    OFPXMT_OFB_TCP_DST        = 14, /* TCP destination port. */
    OFPXMT_OFB_UDP_SRC        = 15, /* UDP source port. */
    OFPXMT_OFB_UDP_DST        = 16, /* UDP destination port. */
    OFPXMT_OFB_SCTP_SRC       = 17, /* SCTP source port. */
    OFPXMT_OFB_SCTP_DST       = 18, /* SCTP destination port. */
    OFPXMT_OFB_ICMPV4_TYPE    = 19, /* ICMP type. */
    OFPXMT_OFB_ICMPV4_CODE    = 20, /* ICMP code. */
    OFPXMT_OFB_ARP_OP         = 21, /* ARP opcode. */
    OFPXMT_OFB_ARP_SPA        = 22, /* ARP source IPv4 address. */
    OFPXMT_OFB_ARP_TPA        = 23, /* ARP target IPv4 address. */
    OFPXMT_OFB_ARP_SHA        = 24, /* ARP source hardware address. */
    OFPXMT_OFB_ARP_THA        = 25, /* ARP target hardware address. */
    OFPXMT_OFB_IPV6_SRC       = 26, /* IPv6 source address. */
    OFPXMT_OFB_IPV6_DST       = 27, /* IPv6 destination address. */
    OFPXMT_OFB_IPV6_FLABEL    = 28, /* IPv6 Flow Label */
    OFPXMT_OFB_ICMPV6_TYPE    = 29, /* ICMPv6 type. */
    OFPXMT_OFB_ICMPV6_CODE    = 30, /* ICMPv6 code. */
    OFPXMT_OFB_IPV6_ND_TARGET = 31, /* Target address for ND. */
    OFPXMT_OFB_IPV6_ND_SLL    = 32, /* Source link-layer for ND. */
    OFPXMT_OFB_IPV6_ND_TLL    = 33, /* Target link-layer for ND. */
    OFPXMT_OFB_MPLS_LABEL     = 34, /* MPLS label. */
    OFPXMT_OFB_MPLS_TC        = 35, /* MPLS TC. */
    OFPXMT_OFB_MPLS_BOS       = 36, /* MPLS BoS bit. */
    OFPXMT_OFB_PBB_ISID       = 37, /* PBB I-SID. */
    OFPXMT_OFB_TUNNEL_ID      = 38, /* Logical Port Metadata. */
    OFPXMT_OFB_IPV6_EXTHDR    = 39, /* IPv6 Extension Header pseudo-field */
};

#define OFPXMT_OFB_ALL    ((UINT64_C(1) << 40) - 1)

/* OpenFlow port on which the packet was received.
 * May be a physical port, a logical port, or the reserved port OFPP_LOCAL
 *
 * Prereqs: None.
 *
 * Format: 32-bit integer in network byte order.
 *
 * Masking: Not maskable. */
#define OXM_OF_IN_PORT    OXM_HEADER  (0x8000, OFPXMT_OFB_IN_PORT, 4)

/* Physical port on which the packet was received.
 *
 * Consider a packet received on a tunnel interface defined over a link
 * aggregation group (LAG) with two physical port members.  If the tunnel
 * interface is the logical port bound to OpenFlow.  In this case,
 * OFPXMT_OF_IN_PORT is the tunnel's port number and OFPXMT_OF_IN_PHY_PORT is
 * the physical port number of the LAG on which the tunnel is configured.
 *
 * When a packet is received directly on a physical port and not processed by a
 * logical port, OFPXMT_OF_IN_PORT and OFPXMT_OF_IN_PHY_PORT have the same
 * value.
 *
 * This field is usually not available in a regular match and only available
 * in ofp_packet_in messages when it's different from OXM_OF_IN_PORT.
 *
 * Prereqs: OXM_OF_IN_PORT must be present.
 *
 * Format: 32-bit integer in network byte order.
 *
 * Masking: Not maskable. */
#define OXM_OF_IN_PHY_PORT OXM_HEADER  (0x8000, OFPXMT_OFB_IN_PHY_PORT, 4)

/* Table metadata.
 *
 * Prereqs: None.
 *
 * Format: 64-bit integer in network byte order.
 *
 * Masking: Arbitrary masks.
 */
#define OXM_OF_METADATA   OXM_HEADER  (0x8000, OFPXMT_OFB_METADATA, 8)
#define OXM_OF_METADATA_W OXM_HEADER_W(0x8000, OFPXMT_OFB_METADATA, 8)

/* Source or destination address in Ethernet header.
 *
 * Prereqs: None.
 *
 * Format: 48-bit Ethernet MAC address.
 *
 * Masking: Arbitrary masks. */
#define OXM_OF_ETH_DST    OXM_HEADER  (0x8000, OFPXMT_OFB_ETH_DST, 6)
#define OXM_OF_ETH_DST_W  OXM_HEADER_W(0x8000, OFPXMT_OFB_ETH_DST, 6)
#define OXM_OF_ETH_SRC    OXM_HEADER  (0x8000, OFPXMT_OFB_ETH_SRC, 6)
#define OXM_OF_ETH_SRC_W  OXM_HEADER_W(0x8000, OFPXMT_OFB_ETH_SRC, 6)

/* Packet's Ethernet type.
 *
 * Prereqs: None.
 *
 * Format: 16-bit integer in network byte order.
 *
 * Masking: Not maskable. */
#define OXM_OF_ETH_TYPE   OXM_HEADER  (0x8000, OFPXMT_OFB_ETH_TYPE, 2)

/* The VLAN id is 12-bits, so we can use the entire 16 bits to indicate
 * special conditions.
 */
enum ofp_vlan_id {
    OFPVID_PRESENT = 0x1000, /* Bit that indicate that a VLAN id is set */
    OFPVID_NONE    = 0x0000, /* No VLAN id was set. */
};
/* Define for compatibility */
#define OFP_VLAN_NONE      OFPVID_NONE

/* 802.1Q VID.
 *
 * For a packet with an 802.1Q header, this is the VLAN-ID (VID) from the
 * outermost tag, with the CFI bit forced to 1. For a packet with no 802.1Q
 * header, this has value OFPVID_NONE.
 *
 * Prereqs: None.
 *
 * Format: 16-bit integer in network byte order with bit 13 indicating
 * presence of VLAN header and 3 most-significant bits forced to 0.
 * Only the lower 13 bits have meaning.
 *
 * Masking: Arbitrary masks.
 *
 * This field can be used in various ways:
 *
 *   - If it is not constrained at all, the nx_match matches packets without
 *     an 802.1Q header or with an 802.1Q header that has any VID value.
 *
 *   - Testing for an exact match with 0x0 matches only packets without
 *     an 802.1Q header.
 *
 *   - Testing for an exact match with a VID value with CFI=1 matches packets
 *     that have an 802.1Q header with a specified VID.
 *
 *   - Testing for an exact match with a nonzero VID value with CFI=0 does
 *     not make sense.  The switch may reject this combination.
 *
 *   - Testing with nxm_value=0, nxm_mask=0x0fff matches packets with no 802.1Q
 *     header or with an 802.1Q header with a VID of 0.
 *
 *   - Testing with nxm_value=0x1000, nxm_mask=0x1000 matches packets with
 *     an 802.1Q header that has any VID value.
 */
#define OXM_OF_VLAN_VID   OXM_HEADER  (0x8000, OFPXMT_OFB_VLAN_VID, 2)
#define OXM_OF_VLAN_VID_W OXM_HEADER_W(0x8000, OFPXMT_OFB_VLAN_VID, 2)

/* 802.1Q PCP.
 *
 * For a packet with an 802.1Q header, this is the VLAN-PCP from the
 * outermost tag.  For a packet with no 802.1Q header, this has value
 * 0.
 *
 * Prereqs: OXM_OF_VLAN_VID must be different from OFPVID_NONE.
 *
 * Format: 8-bit integer with 5 most-significant bits forced to 0.
 * Only the lower 3 bits have meaning.
 *
 * Masking: Not maskable.
 */
#define OXM_OF_VLAN_PCP   OXM_HEADER  (0x8000, OFPXMT_OFB_VLAN_PCP, 1)

/* The Diff Serv Code Point (DSCP) bits of the IP header.
 * Part of the IPv4 ToS field or the IPv6 Traffic Class field.
 *
 * Prereqs: OXM_OF_ETH_TYPE must be either 0x0800 or 0x86dd.
 *
 * Format: 8-bit integer with 2 most-significant bits forced to 0.
 * Only the lower 6 bits have meaning.
 *
 * Masking: Not maskable. */
#define OXM_OF_IP_DSCP     OXM_HEADER  (0x8000, OFPXMT_OFB_IP_DSCP, 1)

/* The ECN bits of the IP header.
 * Part of the IPv4 ToS field or the IPv6 Traffic Class field.
 *
 * Prereqs: OXM_OF_ETH_TYPE must be either 0x0800 or 0x86dd.
 *
 * Format: 8-bit integer with 6 most-significant bits forced to 0.
 * Only the lower 2 bits have meaning.
 *
 * Masking: Not maskable. */
#define OXM_OF_IP_ECN     OXM_HEADER  (0x8000, OFPXMT_OFB_IP_ECN, 1)

/* The "protocol" byte in the IP header.
 *
 * Prereqs: OXM_OF_ETH_TYPE must be either 0x0800 or 0x86dd.
 *
 * Format: 8-bit integer.
 *
 * Masking: Not maskable. */
#define OXM_OF_IP_PROTO   OXM_HEADER  (0x8000, OFPXMT_OFB_IP_PROTO, 1)

/* The source or destination address in the IP header.
 *
 * Prereqs: OXM_OF_ETH_TYPE must match 0x0800 exactly.
 *
 * Format: 32-bit integer in network byte order.
 *
 * Masking: Arbitrary masks.
 */
#define OXM_OF_IPV4_SRC     OXM_HEADER  (0x8000, OFPXMT_OFB_IPV4_SRC, 4)
#define OXM_OF_IPV4_SRC_W   OXM_HEADER_W(0x8000, OFPXMT_OFB_IPV4_SRC, 4)
#define OXM_OF_IPV4_DST     OXM_HEADER  (0x8000, OFPXMT_OFB_IPV4_DST, 4)
#define OXM_OF_IPV4_DST_W   OXM_HEADER_W(0x8000, OFPXMT_OFB_IPV4_DST, 4)

/* The source or destination port in the TCP header.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must be either 0x0800 or 0x86dd.
 *   OXM_OF_IP_PROTO must match 6 exactly.
 *
 * Format: 16-bit integer in network byte order.
 *
 * Masking: Not maskable. */
#define OXM_OF_TCP_SRC    OXM_HEADER  (0x8000, OFPXMT_OFB_TCP_SRC, 2)
#define OXM_OF_TCP_DST    OXM_HEADER  (0x8000, OFPXMT_OFB_TCP_DST, 2)

/* The source or destination port in the UDP header.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match either 0x0800 or 0x86dd.
 *   OXM_OF_IP_PROTO must match 17 exactly.
 *
 * Format: 16-bit integer in network byte order.
 *
 * Masking: Not maskable. */
#define OXM_OF_UDP_SRC    OXM_HEADER  (0x8000, OFPXMT_OFB_UDP_SRC, 2)
#define OXM_OF_UDP_DST    OXM_HEADER  (0x8000, OFPXMT_OFB_UDP_DST, 2)

/* The source or destination port in the SCTP header.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match either 0x0800 or 0x86dd.
 *   OXM_OF_IP_PROTO must match 132 exactly.
 *
 * Format: 16-bit integer in network byte order.
 *
 * Masking: Not maskable. */
#define OXM_OF_SCTP_SRC   OXM_HEADER  (0x8000, OFPXMT_OFB_SCTP_SRC, 2)
#define OXM_OF_SCTP_DST   OXM_HEADER  (0x8000, OFPXMT_OFB_SCTP_DST, 2)

/* The type or code in the ICMP header.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x0800 exactly.
 *   OXM_OF_IP_PROTO must match 1 exactly.
 *
 * Format: 8-bit integer.
 *
 * Masking: Not maskable. */
#define OXM_OF_ICMPV4_TYPE  OXM_HEADER  (0x8000, OFPXMT_OFB_ICMPV4_TYPE, 1)
#define OXM_OF_ICMPV4_CODE  OXM_HEADER  (0x8000, OFPXMT_OFB_ICMPV4_CODE, 1)

/* ARP opcode.
 *
 * For an Ethernet+IP ARP packet, the opcode in the ARP header.  Always 0
 * otherwise.
 *
 * Prereqs: OXM_OF_ETH_TYPE must match 0x0806 exactly.
 *
 * Format: 16-bit integer in network byte order.
 *
 * Masking: Not maskable. */
#define OXM_OF_ARP_OP     OXM_HEADER  (0x8000, OFPXMT_OFB_ARP_OP, 2)

/* For an Ethernet+IP ARP packet, the source or target protocol address
 * in the ARP header.  Always 0 otherwise.
 *
 * Prereqs: OXM_OF_ETH_TYPE must match 0x0806 exactly.
 *
 * Format: 32-bit integer in network byte order.
 *
 * Masking: Arbitrary masks.
 */
#define OXM_OF_ARP_SPA    OXM_HEADER  (0x8000, OFPXMT_OFB_ARP_SPA, 4)
#define OXM_OF_ARP_SPA_W  OXM_HEADER_W(0x8000, OFPXMT_OFB_ARP_SPA, 4)
#define OXM_OF_ARP_TPA    OXM_HEADER  (0x8000, OFPXMT_OFB_ARP_TPA, 4)
#define OXM_OF_ARP_TPA_W  OXM_HEADER_W(0x8000, OFPXMT_OFB_ARP_TPA, 4)

/* For an Ethernet+IP ARP packet, the source or target hardware address
 * in the ARP header.  Always 0 otherwise.
 *
 * Prereqs: OXM_OF_ETH_TYPE must match 0x0806 exactly.
 *
 * Format: 48-bit Ethernet MAC address.
 *
 * Masking: Not maskable. */
#define OXM_OF_ARP_SHA    OXM_HEADER   (0x8000, OFPXMT_OFB_ARP_SHA, 6)
#define OXM_OF_ARP_SHA_W  OXM_HEADER_W (0x8000, OFPXMT_OFB_ARP_SHA, 6)
#define OXM_OF_ARP_THA    OXM_HEADER   (0x8000, OFPXMT_OFB_ARP_THA, 6)
#define OXM_OF_ARP_THA_W  OXM_HEADER_W (0x8000, OFPXMT_OFB_ARP_THA, 6)


/* The source or destination address in the IPv6 header.
 *
 * Prereqs: OXM_OF_ETH_TYPE must match 0x86dd exactly.
 *
 * Format: 128-bit IPv6 address.
 *
 * Masking: Arbitrary masks.
 */
#define OXM_OF_IPV6_SRC    OXM_HEADER  (0x8000, OFPXMT_OFB_IPV6_SRC, 16)
#define OXM_OF_IPV6_SRC_W  OXM_HEADER_W(0x8000, OFPXMT_OFB_IPV6_SRC, 16)
#define OXM_OF_IPV6_DST    OXM_HEADER  (0x8000, OFPXMT_OFB_IPV6_DST, 16)
#define OXM_OF_IPV6_DST_W  OXM_HEADER_W(0x8000, OFPXMT_OFB_IPV6_DST, 16)

/* The IPv6 Flow Label
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x86dd exactly
 *
 * Format: 32-bit integer with 12 most-significant bits forced to 0.
 * Only the lower 20 bits have meaning.
 *
 * Masking: Arbitrary masks.
 */
#define OXM_OF_IPV6_FLABEL   OXM_HEADER  (0x8000, OFPXMT_OFB_IPV6_FLABEL, 4)
#define OXM_OF_IPV6_FLABEL_W OXM_HEADER_W(0x8000, OFPXMT_OFB_IPV6_FLABEL, 4)

/* The type or code in the ICMPv6 header.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x86dd exactly.
 *   OXM_OF_IP_PROTO must match 58 exactly.
 *
 * Format: 8-bit integer.
 *
 * Masking: Not maskable. */
#define OXM_OF_ICMPV6_TYPE OXM_HEADER  (0x8000, OFPXMT_OFB_ICMPV6_TYPE, 1)
#define OXM_OF_ICMPV6_CODE OXM_HEADER  (0x8000, OFPXMT_OFB_ICMPV6_CODE, 1)

/* The target address in an IPv6 Neighbor Discovery message.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x86dd exactly.
 *   OXM_OF_IP_PROTO must match 58 exactly.
 *   OXM_OF_ICMPV6_TYPE must be either 135 or 136.
 *
 * Format: 128-bit IPv6 address.
 *
 * Masking: Not maskable. */
#define OXM_OF_IPV6_ND_TARGET OXM_HEADER (0x8000, OFPXMT_OFB_IPV6_ND_TARGET, 16)

/* The source link-layer address option in an IPv6 Neighbor Discovery
 * message.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x86dd exactly.
 *   OXM_OF_IP_PROTO must match 58 exactly.
 *   OXM_OF_ICMPV6_TYPE must be exactly 135.
 *
 * Format: 48-bit Ethernet MAC address.
 *
 * Masking: Not maskable. */
#define OXM_OF_IPV6_ND_SLL  OXM_HEADER  (0x8000, OFPXMT_OFB_IPV6_ND_SLL, 6)

/* The target link-layer address option in an IPv6 Neighbor Discovery
 * message.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x86dd exactly.
 *   OXM_OF_IP_PROTO must match 58 exactly.
 *   OXM_OF_ICMPV6_TYPE must be exactly 136.
 *
 * Format: 48-bit Ethernet MAC address.
 *
 * Masking: Not maskable. */
#define OXM_OF_IPV6_ND_TLL  OXM_HEADER  (0x8000, OFPXMT_OFB_IPV6_ND_TLL, 6)

/* The LABEL in the first MPLS shim header.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x8847 or 0x8848 exactly.
 *
 * Format: 32-bit integer in network byte order with 12 most-significant
 * bits forced to 0. Only the lower 20 bits have meaning.
 *
 * Masking: Not maskable. */
#define OXM_OF_MPLS_LABEL  OXM_HEADER  (0x8000, OFPXMT_OFB_MPLS_LABEL, 4)

/* The TC in the first MPLS shim header.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x8847 or 0x8848 exactly.
 *
 * Format: 8-bit integer with 5 most-significant bits forced to 0.
 * Only the lower 3 bits have meaning.
 *
 * Masking: Not maskable. */
#define OXM_OF_MPLS_TC     OXM_HEADER  (0x8000, OFPXMT_OFB_MPLS_TC, 1)

/* The BoS bit in the first MPLS shim header.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x8847 or 0x8848 exactly.
 *
 * Format: 8-bit integer with 7 most-significant bits forced to 0.
 * Only the lowest bit have a meaning.
 *
 * Masking: Not maskable. */
#define OXM_OF_MPLS_BOS     OXM_HEADER  (0x8000, OFPXMT_OFB_MPLS_BOS, 1)

/* IEEE 802.1ah I-SID.
 *
 * For a packet with a PBB header, this is the I-SID from the
 * outermost service tag.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x88E7 exactly.
 *
 * Format: 24-bit integer in network byte order.
 *
 * Masking: Arbitrary masks. */
#define OXM_OF_PBB_ISID   OXM_HEADER  (0x8000, OFPXMT_OFB_PBB_ISID, 3)
#define OXM_OF_PBB_ISID_W OXM_HEADER_W(0x8000, OFPXMT_OFB_PBB_ISID, 3)

/* Logical Port Metadata.
 *
 * Metadata associated with a logical port.
 * If the logical port performs encapsulation and decapsulation, this
 * is the demultiplexing field from the encapsulation header.
 * For example, for a packet received via GRE tunnel including a (32-bit) key,
 * the key is stored in the low 32-bits and the high bits are zeroed.
 * For a MPLS logical port, the low 20 bits represent the MPLS Label.
 * For a VxLAN logical port, the low 24 bits represent the VNI.
 * If the packet is not received through a logical port, the value is 0.
 *
 * Prereqs: None.
 *
 * Format: 64-bit integer in network byte order.
 *
 * Masking: Arbitrary masks. */
#define OXM_OF_TUNNEL_ID    OXM_HEADER  (0x8000, OFPXMT_OFB_TUNNEL_ID, 8)
#define OXM_OF_TUNNEL_ID_W  OXM_HEADER_W(0x8000, OFPXMT_OFB_TUNNEL_ID, 8)

/* The IPv6 Extension Header pseudo-field.
 *
 * Prereqs:
 *   OXM_OF_ETH_TYPE must match 0x86dd exactly
 *
 * Format: 16-bit integer with 7 most-significant bits forced to 0.
 * Only the lower 9 bits have meaning.
 *
 * Masking: Maskable. */
#define OXM_OF_IPV6_EXTHDR   OXM_HEADER  (0x8000, OFPXMT_OFB_IPV6_EXTHDR, 2)
#define OXM_OF_IPV6_EXTHDR_W OXM_HEADER_W(0x8000, OFPXMT_OFB_IPV6_EXTHDR, 2)

/* Bit definitions for IPv6 Extension Header pseudo-field. */
enum ofp_ipv6exthdr_flags {      
    OFPIEH_NONEXT = 1 << 0,     /* "No next header" encountered. */
    OFPIEH_ESP    = 1 << 1,     /* Encrypted Sec Payload header present. */
    OFPIEH_AUTH   = 1 << 2,     /* Authentication header present. */
    OFPIEH_DEST   = 1 << 3,     /* 1 or 2 dest headers present. */
    OFPIEH_FRAG   = 1 << 4,     /* Fragment header present. */
    OFPIEH_ROUTER = 1 << 5,     /* Router header present. */
    OFPIEH_HOP    = 1 << 6,     /* Hop-by-hop header present. */
    OFPIEH_UNREP  = 1 << 7,     /* Unexpected repeats encountered. */
    OFPIEH_UNSEQ  = 1 << 8,     /* Unexpected sequencing encountered. */
};

struct ofp_instruction {
	uint16_t type;                /* Instruction type */
	uint16_t len;                 /* Length of this struct in bytes. */
};

struct ofp_instruction_actions {
    uint16_t wType;              /* One of OFPIT_*_ACTIONS */
    uint16_t wLen;               /* Length of this struct in bytes. */
    uint8_t pad[4];             /* Align to 64-bits */
//  struct ofp_action_header actions[0];  
								/* 0 or more actions associated with
                                            OFPIT_WRITE_ACTIONS and
                                             OFPIT_APPLY_ACTIONS */
};

enum ofp_instruction_type {
    OFPIT_GOTO_TABLE = 1,       /* Setup the next table in the lookup
                                   pipeline */
    OFPIT_WRITE_METADATA = 2,   /* Setup the metadata field for use later in
                                   pipeline */
    OFPIT_WRITE_ACTIONS = 3,    /* Write the action(s) onto the datapath action
                                   set */
    OFPIT_APPLY_ACTIONS = 4,    /* Applies the action(s) immediately */
    OFPIT_CLEAR_ACTIONS = 5,    /* Clears all actions from the datapath
                                   action set */
    OFPIT_METER = 6,            /* Apply meter (rate limiter) */

    OFPIT_EXPERIMENTER = 0xFFFF  /* Experimenter instruction */
};

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//ofp_action
struct ofp_action_header {
    uint16_t type;                  /* One of OFPAT_*. */
    uint16_t len;                   /* Length of action, including this
                                       header.  This is the length of action,
                                       including any padding to make it
                                       64-bit aligned. */
    uint8_t pad[4];
};


enum ofp_controller_max_len {
    OFPCML_MAX       = 0xffe5, /* maximum max_len value which can be used
                                  to request a specific byte length. */
    OFPCML_NO_BUFFER = 0xffff  /* indicates that no buffering should be
                                  applied and the whole packet is to be
                                  sent to the controller. */
};

/* Action structure for OFPAT_OUTPUT, which sends packets out 'port'.
 * When the 'port' is the OFPP_CONTROLLER, 'max_len' indicates the max
 * number of bytes to send.  A 'max_len' of zero means no bytes of the
 * packet should be sent. A 'max_len' of OFPCML_NO_BUFFER means that
 * the packet is not buffered and the complete packet is to be sent to
 * the controller. */
struct ofp_action_output {
    uint16_t wType;                  /* OFPAT_OUTPUT. */
    uint16_t wLen;                   /* Length is 16. */
    uint32_t dwPort;                  /* Output port. */
    uint16_t wMax_len;               /* Max length to send to controller. */
    uint8_t pad[6];                 /* Pad to 64 bits. */
};

struct ofp_action_set_field {
    uint16_t wType;                  /* OFPAT_SET_FIELD. */
    uint16_t wLen;                   /* Length is padded to 64 bits. */
    /* Followed by:
     *   - Exactly (4 + oxm_length) bytes containing a single OXM TLV, then
     *   - Exactly ((8 + oxm_length) + 7)/8*8 - (8 + oxm_length)
     *     (between 0 and 7) bytes of all-zero bytes
     */
    uint8_t field[4];               /* OXM TLV - Make compiler happy */
};

enum ofp_action_type {
    OFPAT_OUTPUT       = 0,  /* Output to switch port. */
    OFPAT_COPY_TTL_OUT = 11, /* Copy TTL "outwards" -- from next-to-outermost
                                to outermost */
    OFPAT_COPY_TTL_IN  = 12, /* Copy TTL "inwards" -- from outermost to
                               next-to-outermost */
    OFPAT_SET_MPLS_TTL = 15, /* MPLS TTL */
    OFPAT_DEC_MPLS_TTL = 16, /* Decrement MPLS TTL */

    OFPAT_PUSH_VLAN    = 17, /* Push a new VLAN tag */
    OFPAT_POP_VLAN     = 18, /* Pop the outer VLAN tag */
    OFPAT_PUSH_MPLS    = 19, /* Push a new MPLS tag */
    OFPAT_POP_MPLS     = 20, /* Pop the outer MPLS tag */
    OFPAT_SET_QUEUE    = 21, /* Set queue id when outputting to a port */
    OFPAT_GROUP        = 22, /* Apply group. */
    OFPAT_SET_NW_TTL   = 23, /* IP TTL. */
    OFPAT_DEC_NW_TTL   = 24, /* Decrement IP TTL. */
    OFPAT_SET_FIELD    = 25, /* Set a header field using OXM TLV format. */
    OFPAT_PUSH_PBB     = 26, /* Push a new PBB service tag (I-TAG) */
    OFPAT_POP_PBB      = 27, /* Pop the outer PBB service tag (I-TAG) */
    OFPAT_EXPERIMENTER = 0xffff
};


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//ofp_flow_mod
struct ofp_flow_mod {
    struct ofp_header header;
    uint64_t n64Cookie;              /* Opaque controller-issued identifier. */
    uint64_t n64Cookie_mask;         /* Mask used to restrict the cookie bits
                                     that must match when the command is
                                     OFPFC_MODIFY* or OFPFC_DELETE*. A value
                                     of 0 indicates no restriction. */
    uint8_t bTable_id;             /* ID of the table to put the flow in.
                                     For OFPFC_DELETE_* commands, OFPTT_ALL
                                     can also be used to delete matching
                                     flows from all tables. */
    uint8_t bCommand;              /* One of OFPFC_*. */
    uint16_t wIdle_timeout;        /* Idle time before discarding (seconds). */
    uint16_t wHard_timeout;        /* Max time before discarding (seconds). */
    uint16_t wPriority;            /* Priority level of flow entry. */
    uint32_t dwBuffer_id;           /* Buffered packet to apply to, or
                                     OFP_NO_BUFFER.
                                     Not meaningful for OFPFC_DELETE*. */
    uint32_t dwOut_port;            /* For OFPFC_DELETE* commands, require
                                     matching entries to include this as an
                                     output port.  A value of OFPP_ANY
                                     indicates no restriction. */
    uint32_t dwOut_group;           /* For OFPFC_DELETE* commands, require
                                     matching entries to include this as an
                                     output group.  A value of OFPG_ANY
                                     indicates no restriction. */
    uint16_t wFlags;               /* Bitmap of OFPFF_* flags. */
    uint8_t pad[2];
    struct ofp_match match;       /* Fields to match. Variable size. */
    /* The variable size and padded match is always followed by instructions. */
    /*struct ofp_instruction instructions[0];*/ /* Instruction set - 0 or more.
                                                 The length of the instruction
                                                 set is inferred from the
                                                 length field in the header. */
}PACK_STRUCT;



enum ofp_flow_mod_command {
    OFPFC_ADD           = 0, /* New flow. */
    OFPFC_MODIFY        = 1, /* Modify all matching flows. */
    OFPFC_MODIFY_STRICT = 2, /* Modify entry strictly matching wildcards and
                                priority. */
    OFPFC_DELETE        = 3, /* Delete all matching flows. */
    OFPFC_DELETE_STRICT = 4, /* Delete entry strictly matching wildcards and
                                priority. */
};

/* Value used in "idle_timeout" and "hard_timeout" to indicate that the entry
 * is permanent. */
#define OFP_FLOW_PERMANENT 0

/* By default, choose a priority in the middle. */
#define OFP_DEFAULT_PRIORITY 0x8000

enum ofp_flow_mod_flags {
    OFPFF_SEND_FLOW_REM = 1 << 0,  /* Send flow removed message when flow
                                    * expires or is deleted. */
    OFPFF_CHECK_OVERLAP = 1 << 1,  /* Check for overlapping entries first. */
    OFPFF_RESET_COUNTS  = 1 << 2,  /* Reset flow packet and byte counts. */
    OFPFF_NO_PKT_COUNTS = 1 << 3,  /* Don't keep track of packet count. */
    OFPFF_NO_BYT_COUNTS = 1 << 4,  /* Don't keep track of byte count. */
};


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//ofp_group_mod
/* Group numbering. Groups can use any number up to OFPG_MAX. */
enum ofp_group {
    /* Last usable group number. */
    OFPG_MAX        = 0xffffff00,

    /* Fake groups. */
    OFPG_ALL        = 0xfffffffc,  /* Represents all groups for group delete
                                      commands. */
    OFPG_ANY        = 0xffffffff   /* Special wildcard: no group specified. */
};

/* Group commands */
enum ofp_group_mod_command {
    OFPGC_ADD    = 0,       /* New group. */
    OFPGC_MODIFY = 1,       /* Modify all matching groups. */
    OFPGC_DELETE = 2,       /* Delete all matching groups. */
};


#endif // DTSOpenFlowCommon_h__



