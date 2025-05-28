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
 *   Name:       sync_api.hpp
 *   Project:    Red SDK examples common library
 *
 *   Description: Synchronous API wrapper class for asynchronous RED operations
 *
 ******************************************************************************/
#ifndef COMMON_SYNC_API_HPP_
#define COMMON_SYNC_API_HPP_

#include "../include/eventfd.hpp"
#include <red/red_client_api.h>

namespace common
{

class sync_api_t
{
public:
    sync_api_t();
    rfs_usercb_t *get_ucb();
    red_status_t  wait(int rc);

private:
    void        done(red_status_t status);
    static void callback(red_status_t status, void *arg);

    red_status_t rs;
    rfs_usercb_t ucb;
    eventfd_t    eventfd;
};
} // namespace common

namespace red
{
red_status_t red_s3_create_version(rfs_open_hndl_t  dir_oh,
                                   const char      *tgt_name,
                                   int              flags,
                                   rfs_open_hndl_t *created_oh,
                                   red_api_user_t  *user);

red_status_t red_s3_open(rfs_open_hndl_t  dir_oh,
                         const char      *s3_key,
                         uint64_t         version,
                         int              flags,
                         rfs_open_hndl_t *oh,
                         uint64_t        *out_version,
                         red_api_user_t  *api_user);

red_status_t red_s3_publish(rfs_open_hndl_t oh, uint64_t *version, red_api_user_t *user);

red_status_t red_pwrite(rfs_open_hndl_t oh,
                        void           *buff,
                        size_t          size,
                        off_t           off,
                        ssize_t        *ret_size,
                        red_api_user_t *user);

red_status_t red_close(rfs_open_hndl_t oh, red_api_user_t *user);

red_status_t red_close_dataset(rfs_dataset_hndl_t ds_hndl, red_api_user_t *user);

red_status_t red_s3_create_bucket(const char         *bucket_name,
                                  const char         *cluster,
                                  const char         *tenant,
                                  const char         *subtenant,
                                  red_ds_props_t     *bucket_props,
                                  rfs_dataset_hndl_t *bucket_hndl,
                                  red_api_user_t     *user);

red_status_t red_open_root(rfs_dataset_hndl_t ds_hndl,
                           rfs_open_hndl_t   *root_oh,
                           red_api_user_t    *user);

red_status_t red_fsetxattr(rfs_open_hndl_t oh,
                           const char     *name,
                           const void     *value,
                           size_t          size,
                           int             flags,
                           red_api_user_t *user);

red_status_t red_fgetxattr(rfs_open_hndl_t oh,
                           const char     *name,
                           void           *value,
                           size_t          size,
                           size_t         *ret_size,
                           red_api_user_t *user);

red_status_t red_obtain_dataset(const char         *ds_name,
                                const char         *cluster,
                                red_ds_props_t     *ds_props,
                                rfs_dataset_hndl_t *ds_hndl,
                                red_api_user_t     *user);

red_status_t red_openat(rfs_open_hndl_t  dir_oh,
                        const char      *pathname,
                        int              flags,
                        mode_t           mode,
                        rfs_open_hndl_t *oh,
                        red_api_user_t  *user);

red_status_t red_pread(rfs_open_hndl_t oh,
                       void           *buff,
                       size_t          size,
                       off_t           off,
                       ssize_t        *ret_size,
                       red_api_user_t *user);

} // namespace red

#endif // COMMON_SYNC_API_HPP
