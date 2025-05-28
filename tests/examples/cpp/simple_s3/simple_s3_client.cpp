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
 *   Name:       simple_s3_client.cpp
 *   Project:    RED
 *
 *   Description: A simplified example of an S3 api built on top the red client library.
 *
 *   Created:    3/19/2025
 *   Author(s):  Dana Helwig (dhelwig@ddn.com)
 *
 ******************************************************************************/
#include "simple_s3_client.hpp"
#include <cassert>
#include <fcntl.h>
#include "../common/include/log.hpp"

#define RED_CLUSTER_ENV "RED_CLUSTER"
#define RED_TENANT_ENV  "RED_TENANT"
#define RED_USER_ENV    "RED_USER"

s3bucket::s3bucket(const std::string &bucket_name,
                   rfs_dataset_hndl_t hndl,
                   red_api_user_t    *user,
                   IRedClient        *client)
: bucket_hndl(hndl),
  bucket_name(bucket_name),
  api_user(user),
  red_client(client)
{
}

s3bucket::~s3bucket()
{
    if (RED_IS_VALID_HANDLE(bucket_hndl))
    {
        red_client->close_dataset(bucket_hndl, api_user);
    }
}

s3client::s3client(red_api_user_t *user, std::unique_ptr<IRedClient> client)
: api_user(user),
  red_client(std::move(client))
{
}

s3client::~s3client()
{
    buckets.clear();
}

std::weak_ptr<s3bucket> s3client::create_bucket(const std::string &cluster,
                                                const std::string &bucket_name)
{
    red_ds_props_t bucket_props = {};
    /* Update the default ds prop with user specified values */
    bucket_props.nstripes    = RED_MAX_STRIPES;
    bucket_props.bucket_size = 256 * 1024;
    bucket_props.block_size  = 4 * 1024;
    bucket_props.ec_nparity  = 2;
    bucket_props.poolid      = 1;
    bucket_props.ltid        = 1;

    rfs_dataset_hndl_t bucket_hndl;

    red_status_t rs = red_client->obtain_dataset(bucket_name.c_str(), cluster.c_str(),
                                                 &bucket_props, &bucket_hndl, api_user);

    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("ERROR: Failed to obtain dataset: %s", red_strerror(rs));
        return std::weak_ptr<s3bucket>();
    }

    auto bucket = std::make_shared<s3bucket>(bucket_name, bucket_hndl, api_user,
                                             red_client.get());
    buckets.insert(bucket);
    return bucket;
}

red_status_t s3client::put_object(std::weak_ptr<s3bucket> bucket_weak,
                                  const std::string      &key,
                                  void                   *data,
                                  size_t                  size)
{
    auto bucket = bucket_weak.lock();
    if (!bucket)
    {
        COMMON_LOG("ERROR: Invalid bucket handle");
        return RED_EINVAL;
    }

    rfs_open_hndl_t root_oh;
    rfs_open_hndl_t oh;

    red_status_t rs = red_client->open_root(bucket->handle(), &root_oh, api_user);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("ERROR: Failed to open root: %s", red_strerror(rs));
        return rs;
    }

    rs = red_client->openat(root_oh, key.c_str(), O_CREAT | O_WRONLY, 0644, &oh,
                            api_user);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("ERROR: Failed to open file %s: %s", key.c_str(), red_strerror(rs));
        red_client->close(root_oh, api_user);
        return rs;
    }

    ssize_t bytes_written;
    rs = red_client->pwrite(oh, data, size, 0, &bytes_written, api_user);
    if (rs != RED_SUCCESS || bytes_written != static_cast<ssize_t>(size))
    {
        COMMON_LOG("ERROR: Failed to write data: %s", red_strerror(rs));
    }
    red_client->close(oh, api_user);
    red_client->close(root_oh, api_user);
    return rs;
}

red_status_t s3client::get_object(std::weak_ptr<s3bucket> bucket_weak,
                                  const std::string      &key,
                                  void                   *buffer,
                                  size_t                  size,
                                  ssize_t                *bytes_read)
{
    auto bucket = bucket_weak.lock();
    if (!bucket)
    {
        COMMON_LOG("ERROR: Invalid bucket handle");
        return RED_EINVAL;
    }

    rfs_open_hndl_t root_oh;
    rfs_open_hndl_t oh;

    red_status_t rs = red_client->open_root(bucket->handle(), &root_oh, api_user);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("ERROR: Failed to open root: %s", red_strerror(rs));
        return rs;
    }

    rs = red_client->openat(root_oh, key.c_str(), O_RDONLY, 0, &oh, api_user);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("ERROR: Failed to open file %s: %s", key.c_str(), red_strerror(rs));
        red_client->close(root_oh, api_user);
        return rs;
    }

    rs = red_client->pread(oh, buffer, size, 0, bytes_read, api_user);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("ERROR: Failed to read data: %s", red_strerror(rs));
    }
    red_client->close(oh, api_user);
    red_client->close(root_oh, api_user);
    return rs;
}
