/******************************************************************************
 *
 * @file
 * @copyright
 *                               --- WARNING ---
 *
 *     This work contains trade secrets of DataDirect Networks, Inc.  Any
 *     unauthorized use or disclosure of the work, or any part thereof, is
 *     strictly prohibited. Any use of this work without an express license
 *     or permission is in violation of applicable laws.
 *
 * @copyright DataDirect Networks, Inc. CONFIDENTIAL AND PROPRIETARY
 * @copyright DataDirect Networks Copyright, Inc. (c) 2021-2025. All rights reserved.
 *
 * @section DESCRIPTION
 *
 *   Name:       sync_api.h
 *   Project:    Red SDK examples common library
 *
 *   Description: Synchronous API wrapper for asynchronous RED operations
 *
 ******************************************************************************/

#ifndef COMMON_SYNC_API_H_
#define COMMON_SYNC_API_H_

#include <red/red_client_api.h>

/* Sync API context */
typedef struct
{
    rfs_usercb_t ucb;
    red_status_t rs;
    int          eventfd;
} common_sync_api_ctx_t;

/* Initialize sync API context */
common_sync_api_ctx_t *common_sync_api_init(void);

/* Free sync API context */
void common_sync_api_free(common_sync_api_ctx_t *ctx);

/* Wait for asynchronous operation to complete */
red_status_t common_sync_wait(common_sync_api_ctx_t *ctx, int rc);

#endif /* COMMON_SYNC_API_H_ */
