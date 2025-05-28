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
 *   Name:       simple_s3_example.cpp
 *   Project:    RED
 *
 *   Description: A sample program that creates a bucket, uploads a hello.txt,
 *                and downloads it printing the content to stdout.
 *
 *   Created:    3/19/2025
 *   Author(s):  Dana Helwig (dhelwig@ddn.com)
 *               Bryant Ly (bly@ddn.com)
 *
 ******************************************************************************/
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>

#include <red/red_client_types.h>
#include <red/red_status.h>

#include "../common/include/log.hpp"

#include "simple_s3_client.hpp"

constexpr int      timeout         = 30; /* in seconds */
constexpr uint32_t p_dp_profile_id = RED_DS_DEFAULT_DP_PROFILE;

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        COMMON_LOG("Usage: %s <cluster> <tenant> <subtenant> <bucket>", argv[0]);
        COMMON_LOG("Example: %s default red red test-bucket", argv[0]);
        return 1;
    }

    const char *cluster_name   = argv[1];
    const char *tenant_name    = argv[2];
    const char *subtenant_name = argv[3];
    const char *bucket_name    = argv[4];

    struct red_client_lib_init_opts opts = {.num_sthreads     = 1,
                                            .coremask         = "0x1",
                                            .num_buffers      = 1024,
                                            .num_ring_entries = 1024,
                                            .poller_thread    = true};

    red_status_t rs = static_cast<red_status_t>(red_client_lib_init_v3(&opts));
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("ERROR: Failed to initialize client library: %s", red_strerror(rs));
        return -rs;
    }

    if (!red_client_is_ready(timeout))
    {
        COMMON_LOG("ERROR: red_client_is_ready failed");
        red_client_lib_fini();
        return 1;
    }

    red_api_user_t user = {};
    user.rfs_tenname    = strdup(tenant_name);
    user.rfs_subname    = strdup(subtenant_name);

    rs = static_cast<red_status_t>(
        red_establish_session(cluster_name, tenant_name, subtenant_name,
                              (uint64_t)geteuid(), (uint64_t)getegid(), &user));
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("ERROR: Could not establish session for ten/subten=%s/%s, err: %s",
                   tenant_name, subtenant_name, red_strerror(rs));
        free(user.rfs_tenname);
        free(user.rfs_subname);
        red_client_lib_fini();
        return 1;
    }

    // Scope to ensure client is destroyed before client library is shutdown
    {
        s3client client(&user);

        // Create a bucket
        auto bucket = client.create_bucket(cluster_name, bucket_name);
        if (bucket.expired())
        {
            COMMON_LOG("Failed to create bucket");
            free(user.rfs_tenname);
            free(user.rfs_subname);
            red_client_lib_fini();
            return 1;
        }

        // Upload a file
        const char *data = "Hello World";
        rs = client.put_object(bucket, "hello.txt", (void *)data, strlen(data));
        if (rs != RED_SUCCESS)
        {
            COMMON_LOG("Failed to upload file");
            free(user.rfs_tenname);
            free(user.rfs_subname);
            red_client_lib_fini();
            return 1;
        }

        // Download and print the file
        char    buffer[256];
        ssize_t bytes_read;
        rs = client.get_object(bucket, "hello.txt", buffer, sizeof(buffer), &bytes_read);
        if (rs != RED_SUCCESS)
        {
            COMMON_LOG("Failed to download file");
            free(user.rfs_tenname);
            free(user.rfs_subname);
            red_client_lib_fini();
            return 1;
        }

        buffer[bytes_read] = '\0';
        COMMON_LOG("%s", buffer);
    }

    free(user.rfs_tenname);
    free(user.rfs_subname);
    red_client_lib_fini();
    return rs != RED_SUCCESS;
}
