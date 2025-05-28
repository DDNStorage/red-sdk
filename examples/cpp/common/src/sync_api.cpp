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
 *   Name:       red_sync_api.cpp
 *   Project:    Red SDK examples common library
 *
 *   Description: Synchronous API wrapper class for asynchronous RED operations
 *
 ******************************************************************************/

#include "../include/sync_api.hpp"
#include <cerrno>
#include <poll.h>
#include <red/red_client_api.h>
#include <red/red_ds_api.h>
#include <red/red_s3_api.h>
#include <red/red_fs_api.h>

namespace common
{

sync_api_t::sync_api_t()
: rs(RED_SUCCESS)
{
    ucb.ucb_fun = sync_api_t::callback;
    ucb.ucb_arg = this;
}

void sync_api_t::done(red_status_t status)
{
    rs = status;
    eventfd.kick();
}

void sync_api_t::callback(red_status_t status, void *arg)
{
    auto *me = static_cast<sync_api_t *>(arg);
    me->done(status);
}

rfs_usercb_t *sync_api_t::get_ucb()
{
    return &ucb;
}

red_status_t sync_api_t::wait(int rc)
{
    if (rc != 0)
    {
        done((red_status_t)rc);
        return rs;
    }

    struct pollfd pfds[2] = {
        {.fd = eventfd.get_fd(), .events = POLLIN, .revents = 0},
        {.fd = red_client_lib_poll_fd(), .events = POLLIN, .revents = 0}};

    while (true)
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
            /* evenfd kicked in done() */
            if (!eventfd.read())
            {
                continue;
            }
            break;
        }

        if (pfds[1].revents & POLLIN)
        {
            rfs_usercomp_t ucp = {};
            rc                 = red_client_lib_poll(&ucp, 1);
            if (rc == 1 && ucp.ucp_fun)
            {
                ucp.ucp_fun(ucp.ucp_res, ucp.ucp_arg);
            }
        }
    }

    return rs;
}
} // namespace common

namespace red
{
red_status_t red_s3_create_version(rfs_open_hndl_t  dir_oh,
                                   const char      *tgt_name,
                                   int              flags,
                                   rfs_open_hndl_t *created_oh,
                                   red_api_user_t  *user)
{
    common::sync_api_t sync;
    int rc = ::red_s3_create_version(dir_oh, tgt_name, flags, created_oh, sync.get_ucb(),
                                     user);
    return sync.wait(rc);
}

red_status_t red_s3_open(rfs_open_hndl_t  dir_oh,
                         const char      *s3_key,
                         uint64_t         version,
                         int              flags,
                         rfs_open_hndl_t *oh,
                         uint64_t        *out_version,
                         red_api_user_t  *api_user)
{
    common::sync_api_t sync;

    int rc = ::red_s3_open(dir_oh, s3_key, version, flags, oh, out_version,
                           sync.get_ucb(), api_user);
    return sync.wait(rc);
}

red_status_t red_s3_publish(rfs_open_hndl_t oh, uint64_t *version, red_api_user_t *user)
{
    common::sync_api_t sync;
    int                rc = ::red_s3_publish(oh, version, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_pwrite(rfs_open_hndl_t oh,
                        void           *buff,
                        size_t          size,
                        off_t           off,
                        ssize_t        *ret_size,
                        red_api_user_t *user)
{
    common::sync_api_t sync;
    int rc = red_pwrite(oh, buff, size, off, ret_size, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_close(rfs_open_hndl_t oh, red_api_user_t *user)
{
    common::sync_api_t sync;
    int                rc = ::red_close(oh, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_close_dataset(rfs_dataset_hndl_t ds_hndl, red_api_user_t *user)
{
    common::sync_api_t sync;
    int                rc = ::red_close_dataset(ds_hndl, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_s3_create_bucket(const char         *bucket_name,
                                  const char         *cluster,
                                  const char         *tenant,
                                  const char         *subtenant,
                                  red_ds_props_t     *bucket_props,
                                  rfs_dataset_hndl_t *bucket_hndl,
                                  red_api_user_t     *user)
{
    common::sync_api_t sync;
    int rc = ::red_s3_create_bucket(bucket_name, cluster, tenant, subtenant, bucket_props,
                                    bucket_hndl, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_open_root(rfs_dataset_hndl_t ds_hndl,
                           rfs_open_hndl_t   *root_oh,
                           red_api_user_t    *user)
{
    common::sync_api_t sync;
    int                rc = ::red_open_root(ds_hndl, root_oh, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_fsetxattr(rfs_open_hndl_t oh,
                           const char     *name,
                           const void     *value,
                           size_t          size,
                           int             flags,
                           red_api_user_t *user)
{
    common::sync_api_t sync;
    int rc = ::red_fsetxattr(oh, name, value, size, flags, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_fgetxattr(rfs_open_hndl_t oh,
                           const char     *name,
                           void           *value,
                           size_t          size,
                           size_t         *ret_size,
                           red_api_user_t *user)
{
    common::sync_api_t sync;
    int rc = ::red_fgetxattr(oh, name, value, size, ret_size, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_obtain_dataset(const char         *ds_name,
                                const char         *cluster,
                                red_ds_props_t     *ds_props,
                                rfs_dataset_hndl_t *ds_hndl,
                                red_api_user_t     *user)
{
    common::sync_api_t sync;
    int rc = ::red_obtain_dataset(ds_name, cluster, ds_props, ds_hndl, sync.get_ucb(),
                                  user);
    return sync.wait(rc);
}

red_status_t red_openat(rfs_open_hndl_t  dir_oh,
                        const char      *pathname,
                        int              flags,
                        mode_t           mode,
                        rfs_open_hndl_t *oh,
                        red_api_user_t  *user)
{
    common::sync_api_t sync;
    int rc = ::red_openat(dir_oh, pathname, flags, mode, oh, sync.get_ucb(), user);
    return sync.wait(rc);
}

red_status_t red_pread(rfs_open_hndl_t oh,
                       void           *buff,
                       size_t          size,
                       off_t           off,
                       ssize_t        *ret_size,
                       red_api_user_t *user)
{
    common::sync_api_t sync;
    int rc = ::red_pread(oh, buff, size, off, ret_size, sync.get_ucb(), user);
    return sync.wait(rc);
}

} // namespace red
