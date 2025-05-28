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
 *   Name:       simple_s3_client.hpp
 *   Project:    RED
 *
 *   Description: A simplified example of an S3 api built on top the red client library.
 *
 *   Created:    3/19/2025
 *   Author(s):  Dana Helwig (dhelwig@ddn.com)
 *               Bryant Ly (bly@ddn.com)
 *
 ******************************************************************************/
#pragma once

#include <memory>
#include <set>
#include <string>
#include <stdexcept>

#include <red/red_client_api.h>
#include <red/red_ds_api.h>
#include <red/red_fs_api.h>

#include "../common/include/sync_api.hpp"

class IRedClient
{
public:
    virtual ~IRedClient() = default;

    virtual red_status_t obtain_dataset(const char         *name,
                                        const char         *cluster,
                                        red_ds_props_t     *props,
                                        rfs_dataset_hndl_t *hndl,
                                        red_api_user_t     *user) = 0;

    virtual red_status_t open_root(rfs_dataset_hndl_t ds_hndl,
                                   rfs_open_hndl_t   *root_oh,
                                   red_api_user_t    *user) = 0;

    virtual red_status_t openat(rfs_open_hndl_t  dir_oh,
                                const char      *path,
                                int              flags,
                                mode_t           mode,
                                rfs_open_hndl_t *oh,
                                red_api_user_t  *user) = 0;

    virtual red_status_t pwrite(rfs_open_hndl_t oh,
                                void           *buf,
                                size_t          count,
                                off_t           offset,
                                ssize_t        *bytes_written,
                                red_api_user_t *user) = 0;

    virtual red_status_t pread(rfs_open_hndl_t oh,
                               void           *buf,
                               size_t          count,
                               off_t           offset,
                               ssize_t        *bytes_read,
                               red_api_user_t *user) = 0;

    virtual red_status_t close(rfs_open_hndl_t oh, red_api_user_t *user) = 0;

    virtual red_status_t close_dataset(rfs_dataset_hndl_t ds_hndl,
                                       red_api_user_t    *user) = 0;
};

class RedClientImpl : public IRedClient
{
public:
    red_status_t obtain_dataset(const char         *name,
                                const char         *cluster,
                                red_ds_props_t     *props,
                                rfs_dataset_hndl_t *hndl,
                                red_api_user_t     *user) override
    {
        return red::red_obtain_dataset(name, cluster, props, hndl, user);
    }

    red_status_t open_root(rfs_dataset_hndl_t ds_hndl,
                           rfs_open_hndl_t   *root_oh,
                           red_api_user_t    *user) override
    {
        return red::red_open_root(ds_hndl, root_oh, user);
    }

    red_status_t openat(rfs_open_hndl_t  dir_oh,
                        const char      *path,
                        int              flags,
                        mode_t           mode,
                        rfs_open_hndl_t *oh,
                        red_api_user_t  *user) override
    {
        return red::red_openat(dir_oh, path, flags, mode, oh, user);
    }

    red_status_t pwrite(rfs_open_hndl_t oh,
                        void           *buf,
                        size_t          count,
                        off_t           offset,
                        ssize_t        *bytes_written,
                        red_api_user_t *user) override
    {
        return red::red_pwrite(oh, buf, count, offset, bytes_written, user);
    }

    red_status_t pread(rfs_open_hndl_t oh,
                       void           *buf,
                       size_t          count,
                       off_t           offset,
                       ssize_t        *bytes_read,
                       red_api_user_t *user) override
    {
        return red::red_pread(oh, buf, count, offset, bytes_read, user);
    }

    red_status_t close(rfs_open_hndl_t oh, red_api_user_t *user) override
    {
        return red::red_close(oh, user);
    }

    red_status_t close_dataset(rfs_dataset_hndl_t ds_hndl, red_api_user_t *user) override
    {
        return red::red_close_dataset(ds_hndl, user);
    }
};

class s3bucket
{
private:
    rfs_dataset_hndl_t bucket_hndl;
    std::string        bucket_name;
    red_api_user_t    *api_user;
    IRedClient        *red_client;

public:
    s3bucket(const std::string &bucket_name,
             rfs_dataset_hndl_t hndl,
             red_api_user_t    *user,
             IRedClient        *client);
    ~s3bucket();

    rfs_dataset_hndl_t handle() const
    {
        return bucket_hndl;
    }
    const std::string &name() const
    {
        return bucket_name;
    }
};

class s3client
{
private:
    red_api_user_t                     *api_user;
    std::set<std::shared_ptr<s3bucket>> buckets;
    std::unique_ptr<IRedClient>         red_client;

public:
    explicit s3client(
        red_api_user_t             *user,
        std::unique_ptr<IRedClient> client = std::make_unique<RedClientImpl>());
    ~s3client();

    // Prevent copying
    s3client(const s3client &)            = delete;
    s3client &operator=(const s3client &) = delete;

    std::weak_ptr<s3bucket> create_bucket(const std::string &cluster,
                                          const std::string &bucket_name);

    red_status_t put_object(std::weak_ptr<s3bucket> bucket,
                            const std::string      &key,
                            void                   *data,
                            size_t                  size);

    red_status_t get_object(std::weak_ptr<s3bucket> bucket,
                            const std::string      &key,
                            void                   *buffer,
                            size_t                  size,
                            ssize_t                *bytes_read);
};
