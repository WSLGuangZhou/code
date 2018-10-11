/*
 * File: /nsl/include/nsl/proto/openflow/openflow_private.h
 *
 * Extend define of OpenFlow.
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * The name and trademarks of copyright holder(s) may NOT be used in
 * advertising or publicity pertaining to the Software or any
 * derivatives without specific, written prior permission.
 */

/*
 * modification history
 * --------------------
 * 04/02/2018,      1.0,  sl.cai   Created
 * 04/23/2018,      1.1,  sl.cai   Enable asymmetrical label of VPLS PW IN/OUT.
 * 
 */

#ifndef NSL_INCLUDE_NSL_PROTO_OPENFLOW_OPENFLOW_PRIVATE_H_
#define NSL_INCLUDE_NSL_PROTO_OPENFLOW_OPENFLOW_PRIVATE_H_

#include "DTSOpenFlowCommon.h"

/* enum ofp_type */
//边缘交换机的出口使用的消息
#define OFPT_SET_PRIVATE            ((BYTE)100)

/* OFP_PRI_TYPE_XXX */
#define OFP_PRI_TYPE_VPLS           ((BYTE)0x01)
#define OFP_PRI_TYPE_QOS            ((BYTE)0x02)

/* OFP_PRI_OPT_XXX */
#define OFP_PRI_OPT_INSERT          ((BYTE)0x01)
#define OFP_PRI_OPT_REMOVE          ((BYTE)0x02)
#define OFP_PRI_OPT_ACK             ((BYTE)0x03)

struct ofp_pri_header {
    struct ofp_header ofp_h;
    BYTE bType;                   /* OFP_PRI_TYPE_XXX */
    BYTE bSubtype;                /* OFP_PRI_SUBTYPE_XXX */
    BYTE bOpt;                    /* OFP_PRI_OPT_XXX */
    BYTE bResverd;
} PACK_STRUCT;
//BISPASSERT(sizeof(struct ofp_pri_header) == 12);


/* OFP_PRI_TYPE_VPLS */
#define OFP_PRI_SUBTYPE_VPLS        ((BYTE)0x01)
#define OFP_PRI_SUBTYPE_VPLS_VC     ((BYTE)0x02)
#define OFP_PRI_SUBTYPE_VPLS_PW     ((BYTE)0x03)

#define OFP_PRI_VPLS_ALIAS_MAXSIZE  16

/* OFP_PRI_TYPE_VPLS, OFP_PRI_SUBTYPE_VPLS */
struct ofp_pri_vpls {
    struct ofp_pri_header h;
    char alias[OFP_PRI_VPLS_ALIAS_MAXSIZE];
} PACK_STRUCT;
//BISPASSERT(sizeof(struct ofp_pri_vpls) == 28);

/* OFP_PRI_VPLS_F_XXX */
#define OFP_PRI_VPLS_F_POLICER_ID_VALID     ((WORD)0x0001)
#define OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC    ((WORD)0x0002)

/* OFP_PRI_TYPE_VPLS, OFP_PRI_SUBTYPE_VPLS_VC */
struct ofp_pri_vpls_vc {
    struct ofp_pri_header h;
    char alias[OFP_PRI_VPLS_ALIAS_MAXSIZE];
    DWORD dwPort;
} PACK_STRUCT;
//OFP_ASSERT(sizeof(struct ofp_pri_vpls_vc) == 32);

/* OFP_PRI_TYPE_VPLS, OFP_PRI_SUBTYPE_VPLS_PW */
struct ofp_pri_vpls_pw {
    struct ofp_pri_header h;
    char alias[OFP_PRI_VPLS_ALIAS_MAXSIZE];
    WORD wflag; /* OFP_PRI_VPLS_F_XXX */
    WORD wPolicer_id; /* Valid, while OFP_PRI_VPLS_F_POLICER_ID_VALID is set */
    WORD wResverd;
    BYTE mac[6];
    DWORD dwOut_pw_label;
    DWORD dwOut_vc_label;
    DWORD dwPort;
    DWORD dwIn_pw_label; /* Valid, while OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC is set */
    DWORD dwIn_vc_label; /* Valid, while OFP_PRI_VPLS_F_PW_USE_ASYMMETRIC is set */
} PACK_STRUCT;
//OFP_ASSERT(sizeof(struct ofp_pri_vpls_pw) == 60);


/* OFP_PRI_TYPE_QOS */
#define OFP_PRI_SUBTYPE_POLICER        ((BYTE)0x01)

enum ofp_pri_qos_policer_type_e
{
    OFP_PRI_QOS_POLICER_TYPE_PORT    = 0, /* Port policer (support ing and egs) */
    OFP_PRI_QOS_POLICER_TYPE_FLOW    = 1, /* Flow policer (support ing and egs)*/
    OFP_PRI_QOS_POLICER_TYPE_SERVICE = 2  /* Service policer (only support ingress) */
};

enum ofp_pri_qos_policer_mode_e
{
    OFP_PRI_QOS_POLICER_MODE_RFC2697 = 0, /* RFC2697 srTCM */
    OFP_PRI_QOS_POLICER_MODE_RFC2698 = 1, /* RFC2698 trTCM */
    OFP_PRI_QOS_POLICER_MODE_RFC4115 = 2  /* RFC4115 mode enhanced trTCM*/
};

enum ofp_pri_qos_color_e
{
    OFP_PRI_QOS_COLOR_NONE   = 0, /* None color */
    OFP_PRI_QOS_COLOR_RED    = 1, /* Red color: the lowest drop precedence */
    OFP_PRI_QOS_COLOR_YELLOW = 2, /* Yellow color: the mild drop precedence */
    OFP_PRI_QOS_COLOR_GREEN  = 3  /* Green color: the highest drop precedence */
};

/* OFP_PRI_QOS_POLICER_F_XXX */ /* Param is valid, while set flag */
#define OFP_PRI_QOS_POLICER_F_DROP_COLOR   ((BYTE)0x01)
#define OFP_PRI_QOS_POLICER_F_ID           ((BYTE)0x02)
#define OFP_PRI_QOS_POLICER_F_CIR          ((BYTE)0x04)
#define OFP_PRI_QOS_POLICER_F_CBS          ((BYTE)0x08)
#define OFP_PRI_QOS_POLICER_F_PIR          ((BYTE)0x10)
#define OFP_PRI_QOS_POLICER_F_PBS          ((BYTE)0x20)

/* OFP_PRI_TYPE_QOS, OFP_PRI_SUBTYPE_POLICER */
struct ofp_pri_qos_policer {
    struct ofp_pri_header h;
    BYTE bType; /* enum ofp_pri_qos_policer_type_e */
    BYTE bMode; /* enum ofp_pri_qos_policer_mode_e */
    BYTE bDrop_color; /* enum ofp_pri_qos_color_e */
    BYTE bFlag; /* OFP_PRI_QOS_POLICER_INFO_F_XXX */
    DWORD dwId; /* Flow policer: Flow policer and HBWP id <1-0xFFFF> */
                 /* Service policer: Service ID <1-0xFFFF> */
                 /* Port policer: Global port */
    DWORD dwCir; /* Committed Information Rate (unit:kbps) */
    DWORD dwCbs; /* Committed Burst Size, equivalent to the C-bucket size (unit:kb) */
    DWORD dwPir; /* PIR(RFC2698) or EIR(RFC4115, BWP) (unit:kbps) */
    DWORD dwPbs; /* PBS(RFC2698) or EBS(RFC2697, RFC4115, BWP) (unit:kb) */
}PACK_STRUCT;



/*
 * ERROR CODE, expand ofp_error_type.
 */

#define OFPET_SET_PRIVATE_FAILED 100

enum ofp_set_private_failed_code {
    OFPSPFC_FAIL     = 0,
    OFPSPFC_UNAVAIL  = 1,
    OFPSPFC_BAD_SIZE = 2,
    OFPSPFC_MEMORY   = 3,
    OFPSPFC_NO_FOUND = 4
};


#endif /* NSL_INCLUDE_NSL_PROTO_OPENFLOW_OPENFLOW_PRIVATE_H_ */

