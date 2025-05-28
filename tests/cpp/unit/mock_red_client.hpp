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
 *   Name:       mock_red_client.hpp
 *   Project:    RED
 *
 *   Description: Mock implementation of IRedClient for testing
 *
 *   Created:    3/19/2025
 *   Author(s):  Bryant Ly (bly@ddn.com)
 *
 ******************************************************************************/
#ifndef MOCK_RED_CLIENT_HPP
#define MOCK_RED_CLIENT_HPP

#include "simple_s3_client.hpp"
#include <gmock/gmock.h>

/* Add comparison operators for handle types */
inline bool operator==(const rfs_dataset_hndl_t &a, const rfs_dataset_hndl_t &b)
{
    return a.hndl == b.hndl;
}

inline bool operator==(const rfs_open_hndl_t &a, const rfs_open_hndl_t &b)
{
    return a.fd == b.fd;
}

class MockRedClient : public IRedClient
{
public:
    MOCK_METHOD(red_status_t,
                obtain_dataset,
                (const char         *name,
                 const char         *cluster,
                 red_ds_props_t     *props,
                 rfs_dataset_hndl_t *hndl,
                 red_api_user_t     *user),
                (override));
    MOCK_METHOD(red_status_t,
                open_root,
                (rfs_dataset_hndl_t ds_hndl,
                 rfs_open_hndl_t   *root_oh,
                 red_api_user_t    *user),
                (override));
    MOCK_METHOD(red_status_t,
                openat,
                (rfs_open_hndl_t  dir_oh,
                 const char      *path,
                 int              flags,
                 mode_t           mode,
                 rfs_open_hndl_t *oh,
                 red_api_user_t  *user),
                (override));
    MOCK_METHOD(red_status_t,
                pwrite,
                (rfs_open_hndl_t oh,
                 void           *buf,
                 size_t          count,
                 off_t           offset,
                 ssize_t        *bytes_written,
                 red_api_user_t *user),
                (override));
    MOCK_METHOD(red_status_t,
                pread,
                (rfs_open_hndl_t oh,
                 void           *buf,
                 size_t          count,
                 off_t           offset,
                 ssize_t        *bytes_read,
                 red_api_user_t *user),
                (override));
    MOCK_METHOD(red_status_t,
                close,
                (rfs_open_hndl_t oh, red_api_user_t *user),
                (override));
    MOCK_METHOD(red_status_t,
                close_dataset,
                (rfs_dataset_hndl_t ds_hndl, red_api_user_t *user),
                (override));
};

#endif /* MOCK_RED_CLIENT_HPP */
