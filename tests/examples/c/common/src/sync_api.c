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
 *   Name:       sync_api.c
 *   Project:    Red SDK examples common library
 *
 *   Description: Synchronous API wrapper for asynchronous RED operations
 *
 ******************************************************************************/

#include "../include/sync_api.h"
#include <errno.h>
#include <poll.h>
#include <red/red_client_api.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <unistd.h>

static void common_sync_api_callback(red_status_t status, void *arg)
{
    common_sync_api_ctx_t *ctx = (common_sync_api_ctx_t *)arg;
    ctx->rs                    = status;
    uint64_t val               = 1;
    write(ctx->eventfd, &val, sizeof(val));
}

common_sync_api_ctx_t *common_sync_api_init(void)
{
    common_sync_api_ctx_t *ctx = calloc(1, sizeof(*ctx));
    if (!ctx)
    {
        return NULL;
    }

    ctx->eventfd = eventfd(0, EFD_CLOEXEC);
    if (ctx->eventfd < 0)
    {
        free(ctx);
        return NULL;
    }

    ctx->ucb.ucb_fun = common_sync_api_callback;
    ctx->ucb.ucb_arg = ctx;
    ctx->rs          = RED_SUCCESS;

    return ctx;
}

void common_sync_api_free(common_sync_api_ctx_t *ctx)
{
    if (ctx)
    {
        close(ctx->eventfd);
        free(ctx);
    }
}

red_status_t common_sync_wait(common_sync_api_ctx_t *ctx, int rc)
{
    if (rc != 0)
    {
        ctx->rs = (red_status_t)rc;
        return ctx->rs;
    }

    struct pollfd pfds[2] = {
        {.fd = ctx->eventfd, .events = POLLIN, .revents = 0},
        {.fd = red_client_lib_poll_fd(), .events = POLLIN, .revents = 0}};

    while (1)
    {
        rc = poll(pfds, 2, -1);
        if (rc < 0)
        {
            if (errno == EINTR)
                continue;
            return RED_EINVAL;
        }

        if (rc == 0)
            continue;

        if (pfds[0].revents & POLLIN)
        {
            uint64_t val;
            if (read(ctx->eventfd, &val, sizeof(val)) != sizeof(val))
                continue;
            break;
        }

        if (pfds[1].revents & POLLIN)
        {
            rfs_usercomp_t ucp = {0};
            rc                 = red_client_lib_poll(&ucp, 1);
            if (rc == 1 && ucp.ucp_fun)
            {
                ucp.ucp_fun(ucp.ucp_res, ucp.ucp_arg);
            }
        }
    }

    return ctx->rs;
}
