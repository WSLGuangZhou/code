/*
 * File: /nsl/include/nsl/proto/topology/tplg.h
 *
 * topology module.
 *
 * Copyright (C) 2015 sl.cai(noirscsl@163.com). 
 *
 * This file is a part of the tplg.
 */

/*
 * modification history
 *--------------------
 * 01/17/2015,      1.0,  sl.cai   Created
 * 10/13/2015,      1.1,  sl.cai   Add tplg_ons_add_path and
 *                                 tplg_ons_remove_path2
 * 10/19/2015,      1.2,  sl.cai   Modify tplg_id and path_id to 16bit
 * 10/21/2015,      1.3,  sl.cai   Merge tplg and path ID to 32bit ID
 * 
 */

#ifndef NSL_INCLUDE_NSL_PROTO_TOPOLOGY_TPLG_H_
#define NSL_INCLUDE_NSL_PROTO_TOPOLOGY_TPLG_H_

/* tplg_inform_path_remove_handler_t */
typedef int (*tplg_inform_path_remove_handler_t)(uint32_t tplg_path_id,
                                                 void *usr_data);

/* tplg_ons_router_link_info_t */
typedef struct tplg_ons_router_link_info {
    uint32_t link_state_id;
    uint32_t local_ipaddr;
    uint32_t bandwidth;
    uint32_t delay;
    uint32_t ber;
} tplg_ons_router_link_info_t;

#define TSET_TPLG_PATH_ID(tplg_id_, path_id_) \
    (uint32_t)((((uint32_t)(tplg_id_)) << 16) | ((uint32_t)(path_id_)))
#define TGET_TPLG_ID(id_) (((id_) >> 16) & 0xFFFF)
#define TGET_PATH_ID(id_) ( (id_)        & 0xFFFF)

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/* TPLG module initialize or exit */
int tplg_module_init(uint16_t tplg_id
#ifdef __cplusplus
                     = 0
#endif /* __cplusplus */
                     );
int tplg_module_exit(void);

/******************************************************************************
 * Set/Get topology global ID.
 *
 * DESCRIPTION
 *      While Get, ID is zero If not set before.
 *
 * RETURN VALUE
 *      APP_RET_OK, Get Success.
 *      APP_RET_ERROR, Input is error.
 *
 */
int tplg_id_get(uint16_t *id);

/******************************************************************************
 * Update LSA information to datebase.
 *
 * DESCRIPTION
 *      router_info point to S_TopoInfo_Router buffer, defined in file
 *      "commonTypeDef.h".
 *      network_info point to S_TopoInfo_Network buffer, defined in file
 *      "commonTypeDef.h".
 *
 * RETURN VALUE
 *      APP_RET_XXX, defined in file "baseTypeDef.h"
 */
int tplg_ons_s_router_update(void *router_info);
int tplg_ons_s_network_update(void *network_info);

/******************************************************************************
 * Get RID of Router from ipv4 address.
 *
 * DESCRIPTION
 *      src_ipaddr, dst_ipaddr: (IN) ipv4 address in host order.
 *      src_rid, dst_rid: (OUT) match RID value.
 *
 * RETURN VALUE
 *      APP_RET_OK, find both match RIDs.
 *      APP_RET_ERROR, input param error.
 *      APP_RET_NOTFOUND, can not find src and dst Router.
 *      APP_RET_SOURCE_NO_EXIST, can not find src Router.
 *      APP_RET_DEST_NO_EXIST, can not find dst Router.
 *
 * NOTE
 *      If src_ipaddr and dst_ipaddr is found in same Router, src_rid and
 *      dst_rid will be set same. Otherwise, the best match router ID will
 *      be output.
 *      If the pointer of src_rid or dst_rid is null, return APP_RET_ERROR
 *      immediately.
 *      If no found, output rid is zero.
 */
int tplg_ons_match_rid_to_ipaddr(uint32_t src_ipaddr,
                                 uint32_t dst_ipaddr,
                                 uint32_t *src_rid,
                                 uint32_t *dst_rid);

/******************************************************************************
 * Check RID is local area.
 *
 * DESCRIPTION
 *      rid: (IN) RID value.
 *
 * RETURN VALUE
 *      APP_RET_OK, RID is in local area.
 *      APP_RET_ERROR, input param error.
 *      APP_RET_NOTFOUND, can not find RID in local area database.
 *
 */
int tplg_ons_check_rid_local(uint32_t rid);


/******************************************************************************
 * Get router link information.
 *
 * DESCRIPTION
 *      router_id, the Router ID of the router
 *      link_info_buf, buffer to get link information, alloc by caller
 *      link_info_cnt, link count of link_info_buf
 *      link_info_need_cnt, link count of return
 *
 * RETURN VALUE
 *      APP_RET_XXX, defined in file "baseTypeDef.h"
 */
int tplg_ons_get_router_info(uint32_t router_id,
                             tplg_ons_router_link_info_t *link_info_buf,
                             uint32_t link_info_cnt,
                             uint32_t *link_info_need_cnt);

/******************************************************************************
 * Get router link count.
 *
 * DESCRIPTION
 *      router_id, the Router ID of the router.
 *      link_cnt, output, link count.
 *
 * RETURN VALUE
 *      APP_RET_XXX, defined in file "baseTypeDef.h"
 */
int tplg_ons_get_link_cnt(uint32_t router_id, uint32_t *link_cnt);

/******************************************************************************
 * Get router network information.
 *
 * DESCRIPTION
 *      network_info point to S_TopoInfo_Network buffer, defined in file
 *      "commonTypeDef.h". RouterID and linkStateID must be set valid.
 *
 * RETURN VALUE
 *      APP_RET_XXX, defined in file "baseTypeDef.h"
 */
int tplg_ons_get_network_info(void *network_info);

/******************************************************************************
 * Query path from database.
 *
 * DESCRIPTION
 *      req point to S_FwPathReq buffer, defined in file "forwardDevEntDef.h"
 *      rsp point to S_FwPathRsp buffer, defined in file "forwardDevEntDef.h"
 *
 * RETURN VALUE
 *      APP_RET_XXX, defined in file "baseTypeDef.h"
 *
 * EXAMPLES
 *     void test1()
 *     {
 *         S_FwPathReq req;
 *         S_FwPathRsp rsp;
 *
 *         memset(&req, 0, sizeof(S_FwPathReq));
 *         memset(&rsp, 0, sizeof(S_FwPathRsp));
 *
 *         req.sourceId = 0x01010101;
 *         req.destId = 0x02020202;
 *         req.sQosInfo.bandwidth = 100;
 *
 *         if (APP_RET_OK == tplg_ons_fw_path((void *)&req, (void *)&rsp)) {
 *             ......
 *         }
 *      }
 */
int tplg_ons_fw_path(void *req, void *rsp);

/******************************************************************************
 * Remove path from database to release resource.
 *
 * DESCRIPTION
 *      tplg_path_id: 0-15bit is path ID, 16-31bit is tpld ID.
 *
 * RETURN VALUE
 *      APP_RET_XXX, defined in file "baseTypeDef.h"
 */
int tplg_ons_remove_path2(uint32_t tplg_path_id);

/******************************************************************************
 * add path to tplg database.
 *
 * DESCRIPTION
 *      path_info point to S_PathAddFromOtherDB buffer, defined in file
 *      "forwardDevEntDef.h".
 *      tplg_ons_add_error_t is output error cause, while add fail.
 *
 * RETURN VALUE
 *      APP_RET_XXX, defined in file "baseTypeDef.h"
 *
 * NOTE
 *      errcode                             rid            link_state_id
 *      -------                             ---            -------------
 *      TOA_ERRCODE_NO_INIT                 N              N
 *      TOA_ERRCODE_PARAM                   N              N
 *      TOA_ERRCODE_TPLG_ID                 N              N
 *      TOA_ERRCODE_TPLG_ID_LOCAL           N              N
 *      TOA_ERRCODE_PATH_ID                 N              N
 *      TOA_ERRCODE_PATH_ID_EXIST           N              N
 *      TOA_ERRCODE_RID_INVALID             Y              N
 *      TOA_ERRCODE_LINK_INVALID            Y              Y
 *      TOA_ERRCODE_LINK_BW_NOT_ENOUGH      Y              Y
 *      (N is invalid, Y is valid.)
 *
 */
/* TOA_ERRCODE_XXX */
#define TOA_ERRCODE_NO_INIT                 -1
#define TOA_ERRCODE_PARAM                   -2
#define TOA_ERRCODE_TPLG_ID                 -3
#define TOA_ERRCODE_TPLG_ID_LOCAL           -4
#define TOA_ERRCODE_PATH_ID                 -5
#define TOA_ERRCODE_PATH_ID_EXIST           -6
#define TOA_ERRCODE_RID_INVALID             -7
#define TOA_ERRCODE_LINK_INVALID            -8
#define TOA_ERRCODE_LINK_BW_NOT_ENOUGH      -9
typedef struct tplg_ons_add_err_s {
    int errcode;            /* TOA_ERRCODE_XXX */
    uint32_t rid;           /* order is same to S_FwPathRsp */
    uint32_t link_state_id; /* order is same to S_FwPathRsp */
} tplg_ons_add_err_t;

int tplg_ons_add_path(void *path_info,
                      tplg_ons_add_err_t *err);

/******************************************************************************
 * Remove all paths from database to release resource.
 *
 * DESCRIPTION
 *      none.
 *
 * RETURN VALUE
 *      the count of removed path
 *
 */
int tplg_ons_remove_all_path(void);

/******************************************************************************
 * Register callback while path is removed automatically.
 *
 * DESCRIPTION
 *      f is the callback function. If NULL, no callback will be executed.
 *      usr_data will be used in callback.
 *
 * RETURN VALUE
 *      APP_RET_OK.
 *
 */
int tplg_one_path_remove_inform_register(tplg_inform_path_remove_handler_t f,
                                         void *usr_data);

#ifdef __cplusplus

/******************************************************************************
 * Get neighbour Router(s) base designated Router.
 *
 * DESCRIPTION
 *      rid is designated Router ID.
 *      neighbour is output result.
 *
 * RETURN VALUE
 *      the size of neighbour
 *
 */
uint32_t tplg_one_get_neighbour(uint32_t rid,
                                std::set<uint32_t> &neighbour);

#endif /* __cplusplus */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* NSL_INCLUDE_NSL_PROTO_TOPOLOGY_TPLG_H_ */

