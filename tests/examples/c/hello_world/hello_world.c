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
 *   Name:       rfs_hello_world.c
 *   Project:    RED
 *
 *   Description: A sample program that creates a bucket and an object named
 *                HelloWorld.
 *
 *   Created:    2/25/2025
 *   Author(s):  Noel Otterness (notterness@ddn.com)
 *
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <getopt.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <poll.h>

#include <sys/eventfd.h>
#include <sys/errno.h>
#include <sys/xattr.h>

#include <red/red_client_types.h>
#include <red/red_client_api.h>
#include <red/red_ds_api.h>
#include <red/red_s3_api.h>
#include <red/red_fs_api.h>
#include <red/red_dhash_api.h>

#include "../common/include/log.h"
#include "../common/include/string_utils.h"
#include "../common/include/sync_api.h"

/*
 * Some environment variables to store information
 */
#define RED_CLUSTER_ENV "RED_CLUSTER"
#define RED_TENANT_ENV  "RED_TENANT"
#define RED_USER_ENV    "RED_USER"

#define RSMT_MAX_NAME 256

/*
 * Information pulled from command line parameters
 */
const char *p_cluster       = "infinia";
const char *p_ten_subten    = "red/red";
char       *p_bucket_name   = NULL;
char       *p_user_id       = NULL;
uint32_t    p_dp_profile_id = RED_DS_DEFAULT_DP_PROFILE;

static red_status_t example_open_bucket(const char         *bucket_name,
                                        const char         *cluster,
                                        const char         *tenant,
                                        const char         *subtanent,
                                        rfs_dataset_hndl_t *bucket_hndl,
                                        rfs_open_hndl_t    *root_oh,
                                        red_api_user_t     *user);

static red_status_t example_create_object(rfs_open_hndl_t root_oh, red_api_user_t *user);

static red_status_t example_example_write(rfs_open_hndl_t        obj_oh,
                                          void                  *wr_buffer,
                                          size_t                 io_size,
                                          red_s3_checksum_type_e chksum_type,
                                          char                  *md5_etag,
                                          red_api_user_t        *user);

static red_status_t example_validate_etag(rfs_open_hndl_t obj_oh,
                                          const char     *expected_etag,
                                          red_api_user_t *user);

static red_status_t example_set_acl(rfs_open_hndl_t oh, red_api_user_t *user);

static void print_help(const char *argv)
{
    fprintf(stdout,
            "Usage: %s [OPTION]\n"
            "red-read-obj is a command line tool to \n"
            "read from an object and dump out its xattrs and data.\n"
            "It will validate that the data read matches the etag as well.\n"
            "-----------------------------------------\n"
            "   -c --cluster      <name>    Cluster name\n"
            "   -N --tenant       <name>    Tenant name\n"
            "   -n --subtenant    <name>    Subtenant name\n"
            "   -B --bucket       <name>    Bucket name\n"
            "   -I --id           <user id> User Id\n"
            " ***********************************************************\n",
            argv);
}

static red_status_t parse_args(int argc, char **argv)
{
    int         option_index = 0;
    int         c;
    const char *clus_arg = NULL;
    const char *ten_arg  = NULL;
    const char *sten_arg = NULL;

    static struct option options[] = {{"cluster", required_argument, 0, 'c'},
                                      {"bucket", required_argument, 0, 'B'},
                                      {"help", no_argument, 0, 'h'},
                                      {"tenant", required_argument, 0, 'N'},
                                      {"subtenant", required_argument, 0, 'n'},
                                      {"id", required_argument, 0, 'I'},
                                      {0, 0, 0, 0}};

    for (;;)
    {
        c = getopt_long(argc, argv, "N:n:B:c:I:h", options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
        case ':':
            fprintf(stderr, "%s: option '%c' requires an argument\n\n", argv[0], c);
            print_help(argv[0]);
            exit(EXIT_FAILURE);

        case 'c':
            if (clus_arg != NULL)
            {
                fprintf(stderr, "%s: only 1 cluster name can be specified\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            clus_arg = optarg;
            break;

        case 'N':
            if (ten_arg != NULL)
            {
                fprintf(stderr, "%s: only 1 tenant name can be specified\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            ten_arg = optarg;
            break;

        case 'n':
            if (sten_arg != NULL)
            {
                fprintf(stderr, "%s: only 1 subtenant name can be specified\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            sten_arg = optarg;
            break;

        case 'B':
            if (p_bucket_name != NULL)
            {
                fprintf(stderr, "%s: only 1 bucket can be specified\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            p_bucket_name = optarg;
            break;

        case 'I':
            if (p_user_id != NULL)
            {
                fprintf(stderr, "%s: only 1 user id can be specified\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            p_user_id = optarg;
            break;

        case 'h':
            print_help(argv[0]);
            exit(EXIT_SUCCESS);

        default:
            fprintf(stderr, "%s: option '%c' is unknown\n\n", argv[0], c);
            print_help(argv[0]);
            exit(-1);
        }
    }

    /* Must set a bucket name */
    if (p_bucket_name == NULL)
    {
        fprintf(stderr, "Please specify bucket name (-B <bucket name>)\n");
        exit(EXIT_FAILURE);
    }

    /* Must set a user id */
    if (p_user_id == NULL)
    {
        fprintf(stderr, "Please specify user id to set the user.s3_acl (-I <user id>)\n");
        exit(EXIT_FAILURE);
    }

    /* fail if RED_USER is not set */
    if (getenv(RED_USER_ENV) == NULL)
    {
        fprintf(stderr, "Please specify correct %s env!\n", RED_USER_ENV);
        exit(EXIT_FAILURE);
    }

    if (clus_arg != NULL)
    {
        char red_clus[RSMT_MAX_NAME + 1] = {}; /*cluster\0*/

        snprintf(red_clus, sizeof(red_clus), "%s",
                 clus_arg != NULL ? clus_arg : "infinia");

        COMMON_LOG("using %s as cluster", red_clus);
        setenv(RED_CLUSTER_ENV, red_clus, 1);
    }

    if (ten_arg != NULL || sten_arg != NULL)
    {
        char red_tenant[2 * RSMT_MAX_NAME + 2] = {}; /*tenant/subtenant\0*/

        snprintf(red_tenant, sizeof(red_tenant), "%s/%s",
                 ten_arg != NULL ? ten_arg : "red", sten_arg != NULL ? sten_arg : "red");

        COMMON_LOG("using %s as tenant/subtenant", red_tenant);
        setenv(RED_TENANT_ENV, red_tenant, 1);
    }

    const char *clus = getenv(RED_CLUSTER_ENV);
    if (clus != NULL)
    {
        p_cluster = clus;
        COMMON_LOG("LOADING %s as p_cluster", p_cluster);
    }

    const char *tds = getenv(RED_TENANT_ENV);
    if (tds != NULL)
    {
        p_ten_subten = tds;
        COMMON_LOG("LOADING %s as p_ten_subten", p_ten_subten);
    }

    return RED_SUCCESS;
}

const struct red_client_lib_init_opts opts = {.num_sthreads = 1,
                                              .coremask     = NULL, /* Default coremask */
                                              .num_buffers  = RFS_NUM_DEF_BUFFERS,
                                              .num_ring_entries = 64,
                                              .poller_thread    = true};

int main(int argc, char **argv)
{
    int                    timeout     = 30; /* in seconds */
    rfs_dataset_hndl_t     bucket_hndl = RED_INVALID_HANDLE;
    rfs_open_hndl_t        root_oh     = RED_INVALID_OPEN_HANDLE;
    char                  *tenname     = NULL;
    char                  *subname     = NULL;
    int                    auth_index  = RED_DEFAULT_AUTH_INDEX;
    red_api_user_t         user        = {};
    red_status_t           rs;
    int                    rc;
    common_sync_api_ctx_t *ctx;

    rs = parse_args(argc, argv);
    if (rs != RED_SUCCESS)
    {
        exit(EXIT_FAILURE);
    }

    rc = red_client_lib_init_v3(&opts);
    if (rc != 0)
    {
        fprintf(stderr, "Error: red_client_lib_init failed with rs=%d\n", rc);
        exit(EXIT_FAILURE);
    }
    COMMON_LOG("Client library initialized successfully");

    if (!red_client_is_ready(timeout))
    {
        fputs("Error: red_client_is_ready failed\n", stderr);
        red_client_lib_fini();
        exit(EXIT_FAILURE);
    }
    COMMON_LOG("Client is ready");

    /* Get tenant and subtenant string from the env/global variable. */
    common_split_tensubten(p_ten_subten, &tenname, &subname);
    COMMON_LOG("Using tenant=%s subtenant=%s", tenname, subname);

    rc = red_establish_session(p_cluster, tenname, subname, (uint64_t)geteuid(),
                               (uint64_t)getegid(), &user);
    if (rc != RED_SUCCESS)
    {
        fprintf(stderr,
                "ERROR: Could not establish session for ten/subten=%s/%s, err: %s\n",
                tenname, subname, red_strerror(rs));
        goto exit_error;
    }
    COMMON_LOG("Session established successfully with cluster=%s auth_index=%d",
               p_cluster, auth_index);

    rs = example_open_bucket(p_bucket_name, p_cluster, tenname, subname, &bucket_hndl,
                             &root_oh, &user);
    if (rs != RED_SUCCESS)
    {
        fputs("Unable to create bucket\n", stdout);
        goto exit_error;
    }
    COMMON_LOG("Bucket %s opened successfully", p_bucket_name);

    rs = example_create_object(root_oh, &user);
    if (rs != RED_SUCCESS)
    {
        fputs("Unable to create object\n", stdout);
    }
    else
    {
        COMMON_LOG("Object HelloWorld created and written successfully");
    }

exit_error:
    COMMON_LOG("Starting cleanup sequence");
    /*
     * Cleanup in reverse order of initialization
     */
    if (RED_IS_VALID_OPEN_HANDLE(root_oh))
    {
        COMMON_LOG("Closing root handle");
        ctx = common_sync_api_init();
        if (ctx)
        {
            rc = red_close(root_oh, &ctx->ucb, &user);
            common_sync_wait(ctx, rc);
            common_sync_api_free(ctx);
        }
        root_oh = ((red_open_hndl_t)RED_INVALID_OPEN_HANDLE);
    }

    if (RED_IS_VALID_HANDLE(bucket_hndl))
    {
        COMMON_LOG("Closing bucket handle");
        ctx = common_sync_api_init();
        if (ctx)
        {
            rc = red_close_dataset(bucket_hndl, &ctx->ucb, &user);
            common_sync_wait(ctx, rc);
            common_sync_api_free(ctx);
        }
        bucket_hndl = ((red_dataset_hndl_t)RED_INVALID_HANDLE);
    }

    /* Cleanup session */
    COMMON_LOG("Cleaning up session");
    red_cleanup_session(&user);

    /* Then cleanup library resources */
    COMMON_LOG("Cleaning up library resources");
    red_client_lib_fini();

    COMMON_LOG("Program finished with status %d", rs);
    exit(rs != RED_SUCCESS ? EXIT_FAILURE : EXIT_SUCCESS);
}

static red_status_t example_create_object(rfs_open_hndl_t root_oh, red_api_user_t *user)
{
    /*
     * The operations for a PUT are as follows:
     * 1. red_s3_create_version() - Create a new version of the object
     * 2. red_fsetxattr() - Set the user.s3_acl and user.s3_user_meta xattrs
     * 3. red_pwrite() - Write data to the object
     * 4. red_s3_publish() - Publish the object (internally calculates and sets
     * user.s3_etag)
     * 5. red_close() - Close the object
     * 6. red_s3_open() - Reopen the object to validate the etag
     * 7. red_fgetxattr() - Get the user.s3_etag to validate it matches our calculated MD5
     */
    red_status_t           rs;
    int                    rc;
    rfs_open_hndl_t        obj_oh = RED_INVALID_OPEN_HANDLE;
    uint64_t               version;
    char                   data[12];
    char                   md5_etag[RED_S3_USER_ETAG_SIZE] = {0};
    common_sync_api_ctx_t *ctx;

    ctx = common_sync_api_init();
    if (!ctx)
    {
        return RED_ENOMEM;
    }
    rc = red_s3_create_version(root_oh, "HelloWorld", 0, &obj_oh, &ctx->ucb, user);
    rs = common_sync_wait(ctx, rc);
    common_sync_api_free(ctx);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("red_s3_create_version() failed! rs=%d", rs);
        goto put_xerror;
    }

    strcpy(data, "Hello World");
    rs = example_example_write(obj_oh, (void *)data, strlen(data), RED_S3CS_SHA256,
                               md5_etag, user);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("%s: write failed! rs=%d", __func__, rs);
        goto put_xerror;
    }

    /*
     * Set the user.s3_acl
     */
    rs = example_set_acl(obj_oh, user);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("example_set_acl(object) failed! rs=%d", rs);
        goto put_xerror;
    }

    ctx = common_sync_api_init();
    if (!ctx)
    {
        return RED_ENOMEM;
    }

    rc = red_s3_publish(obj_oh, &version, &ctx->ucb, user);
    rs = common_sync_wait(ctx, rc);
    common_sync_api_free(ctx);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("red_s3_publish() failed! rs=%d", rs);
        goto put_xerror;
    }

    /* Close the object to ensure the extended attributes are written */
    ctx = common_sync_api_init();
    if (!ctx)
    {
        return RED_ENOMEM;
    }
    rc = red_close(obj_oh, &ctx->ucb, user);
    rs = common_sync_wait(ctx, rc);
    common_sync_api_free(ctx);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("red_close() failed! rs=%d", rs);
        goto put_xerror;
    }
    obj_oh = (rfs_open_hndl_t)RED_INVALID_OPEN_HANDLE;

    /* Reopen the object to validate the etag */
    ctx = common_sync_api_init();
    if (!ctx)
    {
        return RED_ENOMEM;
    }
    rc = red_s3_open(root_oh, "HelloWorld", 0, 0, &obj_oh, &version, &ctx->ucb, user);
    rs = common_sync_wait(ctx, rc);
    common_sync_api_free(ctx);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("red_s3_open() failed! rs=%d", rs);
        goto put_xerror;
    }

    /* Validate that the etag matches our calculated MD5 */
    rs = example_validate_etag(obj_oh, md5_etag, user);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("example_validate_etag() failed! rs=%d", rs);
        goto put_xerror;
    }

put_xerror:
    /*
     * Need to close the handle otherwise the test will crash when attempting
     * to close the dataset.
     */
    if (RED_IS_VALID_OPEN_HANDLE(obj_oh))
    {
        ctx = common_sync_api_init();
        if (!ctx)
        {
            return RED_ENOMEM;
        }
        rc = red_close(obj_oh, &ctx->ucb, user);
        rs = common_sync_wait(ctx, rc);
        common_sync_api_free(ctx);
        if (rs != RED_SUCCESS)
        {
            COMMON_LOG("red_close() failed! rs=%d", rs);
        }
    }

    return rs;
}

/*
 * Create a bucket and open the root handle
 *
 * NOTE: For a bucket to be usable by the S3 Server, it must have the
 *       system.versioning and user.s3_acl xattrs set on the root.
 */
static red_status_t example_open_bucket(const char         *bucket_name,
                                        const char         *cluster,
                                        const char         *tenant,
                                        const char         *subtenant,
                                        rfs_dataset_hndl_t *bucket_hndl,
                                        rfs_open_hndl_t    *root_oh,
                                        red_api_user_t     *user)
{
    red_status_t           rs = RED_SUCCESS;
    red_ds_props_t         bucket_props;
    common_sync_api_ctx_t *ctx;
    int                    rc;

    rs = red_ds_get_default_props(&bucket_props, p_dp_profile_id);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("red_ds_get_default_props() failed! dp_id=%d, rs=%d", p_dp_profile_id,
                   rs);
        return rs;
    }

    /* Update the default ds prop with user specified values */
    bucket_props.nstripes    = RED_MAX_STRIPES;
    bucket_props.bucket_size = 256 * 1024;
    bucket_props.block_size  = 4 * 1024;
    bucket_props.ec_nparity  = 2;

    ctx = common_sync_api_init();
    if (!ctx)
    {
        return RED_ENOMEM;
    }

    rc = red_s3_create_bucket(bucket_name, cluster, tenant, subtenant, &bucket_props,
                              bucket_hndl, &ctx->ucb, user);
    rs = common_sync_wait(ctx, rc);
    common_sync_api_free(ctx);
    if ((rs != RED_SUCCESS) && (rs != RED_EEXIST))
    {
        COMMON_LOG("red_s3_create_bucket_v3() failed! rs=%d", rs);
    }
    else
    {
        /*
         * Obtain the root handle for the newly created dataset
         */
        ctx = common_sync_api_init();
        if (!ctx)
        {
            return RED_ENOMEM;
        }

        rc = red_open_root(*bucket_hndl, root_oh, &ctx->ucb, user);
        rs = common_sync_wait(ctx, rc);
        common_sync_api_free(ctx);
        if (rs != RED_SUCCESS)
        {
            COMMON_LOG("red_open_root() failed! rs=%d", rs);
        }

        rs = example_set_acl(*root_oh, user);
        if (rs != RED_SUCCESS)
        {
            COMMON_LOG("example_set_acl(root) failed! rs=%d", rs);
        }
    }

    return rs;
}

#define SHA256_HASH_LEN (32)
struct obj_hash_t
{
    union
    {
        uint64_t csum;
        uint8_t  hash[SHA256_HASH_LEN];
    };
};

/*
 * @brief - Write a buffer to an object.
 *          This will compute the "user.s3_etag" and checksum if the
 *          chksum_type is set to RED_S3CS_SHA256.
 .
 */
static red_status_t example_example_write(rfs_open_hndl_t        obj_oh,
                                          void                  *wr_buffer,
                                          size_t                 io_size,
                                          red_s3_checksum_type_e chksum_type,
                                          char                  *md5_etag,
                                          red_api_user_t        *user)
{
    ssize_t                write_ret_size;
    red_status_t           rs;
    red_dhash_e            hash_type;
    struct obj_hash_t      hash;
    bool                   add_xattrs = false;
    common_sync_api_ctx_t *ctx;
    int                    rc;
    red_rc_t               red_rc;

    ctx = common_sync_api_init();
    if (!ctx)
    {
        return RED_ENOMEM;
    }

    rc = red_pwrite(obj_oh, wr_buffer, io_size, 0, &write_ret_size, &ctx->ucb, user);
    rs = common_sync_wait(ctx, rc);
    common_sync_api_free(ctx);
    if (rs != RED_SUCCESS || ((size_t)write_ret_size) != io_size)
    {
        COMMON_LOG("red_pwrite() failed! ret_size=%ld io_size=%lu rc=%d", write_ret_size,
                   io_size, rs);

        if (rs == RED_SUCCESS)
            rs = RED_EINVAL;
        return rs;
    }

    switch (chksum_type)
    {
    case RED_S3CS_SHA256:
    {
        /*
         * Compute the SHA256 value over the buffer so it can be
         * stored into the xattr.
         */
        hash_type = RED_DHASH_SHA256;
        assert(red_dhash_data(hash_type, wr_buffer, io_size, &hash) == 0);
        add_xattrs = true;
    }
    break;

    case RED_S3CS_NONE:
    case RED_S3CS_CRC32:
    case RED_S3CS_CRC32C:
    case RED_S3CS_SHA1:
    case RED_S3CS_TOTAL:
    default:
        break;
    }

    if (add_xattrs)
    {
        /*
         * First set the checksum value in case the checksum the checksum type
         * fails. Then it will be cleaned up and not visible.
         */
        ctx = common_sync_api_init();
        if (!ctx)
        {
            return RED_ENOMEM;
        }

        rc = red_fsetxattr(obj_oh, RED_CHKSUM_KEY, (void *)&hash.hash,
                           (size_t)SHA256_HASH_LEN, XATTR_CREATE, &ctx->ucb, user);
        rs = common_sync_wait(ctx, rc);
        common_sync_api_free(ctx);
        if (rs != RED_SUCCESS)
        {
            COMMON_LOG("%s: set %s failed len=%u rs=%d", __func__, RED_CHKSUM_KEY,
                       SHA256_HASH_LEN, rs);
            return rs;
        }

        ctx = common_sync_api_init();
        if (!ctx)
        {
            return RED_ENOMEM;
        }

        rc = red_fsetxattr(obj_oh, RED_CHKSUM_TYPE_KEY, (void *)&chksum_type,
                           sizeof(chksum_type), XATTR_CREATE, &ctx->ucb, user);
        rs = common_sync_wait(ctx, rc);
        common_sync_api_free(ctx);
        if (rs != RED_SUCCESS)
        {
            COMMON_LOG("%s: set %s checksum type=%d failed len=%u rs=%d", __func__,
                       RED_CHKSUM_TYPE_KEY, chksum_type, SHA256_HASH_LEN, rs);
            return rs;
        }
    }

    /*
     * Compute the md5 hash for the buffer locally for validation.
     * The RED Client code will internally set the user.s3_etag when red_s3_publish is
     * called.
     */
    {
        uint8_t  md5_hash[16];

        /* Use direct MD5 hash instead of rolling hash */
        red_rc = red_dhash_data(RED_DHASH_MD5, wr_buffer, io_size, md5_hash);
        if (red_rc != 0)
        {
            COMMON_LOG("%s: red_dhash_data failed rc=%d", __func__, red_rc);
            return RED_EINVAL;
        }

        /* Copy and NULL terminate for tracing */
        red_bin_to_hex(md5_hash, sizeof(md5_hash), md5_etag);
        md5_etag[RED_S3_USER_ETAG_SIZE - 1] = '\0';
        COMMON_LOG("%s: Calculated MD5 etag=%s", __func__, md5_etag);
    }

    return RED_SUCCESS;
}

/*
 * Set a default ACL on either the object or the root of the bucket
 */
static red_status_t example_validate_etag(rfs_open_hndl_t obj_oh,
                                          const char     *expected_etag,
                                          red_api_user_t *user)
{
    red_status_t rs;
    int          rc;
    char         actual_etag[RED_S3_USER_ETAG_SIZE] = {0};
    size_t       xattr_size = RED_S3_USER_ETAG_SIZE - 1; /* No need for null terminator */
    size_t       ret_size;
    common_sync_api_ctx_t *ctx;

    /* Get the user.s3_etag extended attribute */
    ctx = common_sync_api_init();
    if (!ctx)
    {
        return RED_ENOMEM;
    }

    rc = red_fgetxattr(obj_oh, RED_S3_ETAG_KEY, actual_etag, xattr_size, &ret_size,
                       &ctx->ucb, user);

    rs = common_sync_wait(ctx, rc);
    common_sync_api_free(ctx);

    xattr_size = ret_size;

    /* Just log the result and return success */
    COMMON_LOG("%s: red_fgetxattr result rs=%d, xattr_size=%zu", __func__, rs,
               xattr_size);

    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("%s: Failed to get %s, rs=%d", __func__, RED_S3_ETAG_KEY, rs);
        return rs;
    }

    /* Ensure null termination for string comparison */
    actual_etag[xattr_size] = '\0';

    /* Compare the expected and actual etags */
    if (strcmp(expected_etag, actual_etag) != 0)
    {
        COMMON_LOG("%s: Etag mismatch! Expected=%s, Actual=%s", __func__, expected_etag,
                   actual_etag);
        return RED_FAILURE;
    }

    COMMON_LOG("%s: Etag validation successful. Expected etag=%s, Actual etag=%s",
               __func__, expected_etag, actual_etag);

    return RED_SUCCESS;
}

static red_status_t example_set_acl(rfs_open_hndl_t oh, red_api_user_t *user)
{
    red_status_t           rs;
    const char            *acl_str;
    size_t                 len;
    int                    out_len;
    char                  *acl_value;
    common_sync_api_ctx_t *ctx;
    int                    rc;

    /*
     * Set the user.s3_acl
     */
    acl_str = "{\"Owner\":{\"ID\":\" \",\"DisplayName\":\"admin\"},"
              "\"Grants\":[{\"Grantee\":{\"Type\":\"CanonicalUser\","
              "\"ID\":\" \",\"DisplayName\":\"admin\"},\"Permission\":"
              "\"FULL_CONTROL\"}]}";

    len = strlen(acl_str) + (2 * strlen(p_user_id)) + 1;

    acl_value = malloc(len);
    out_len   = snprintf(acl_value, len,
                         "{\"Owner\":{\"ID\":\"%s\",\"DisplayName\":\"admin\"},"
                           "\"Grants\":[{\"Grantee\":{\"Type\":\"CanonicalUser\","
                           "\"ID\":\"%s\",\"DisplayName\":\"admin\"},\"Permission\":"
                           "\"FULL_CONTROL\"}]}",
                         p_user_id, p_user_id);

    ctx = common_sync_api_init();
    if (!ctx)
    {
        free(acl_value);
        return RED_ENOMEM;
    }

    rc = red_fsetxattr(oh, RED_S3_USER_ACL_XATTR_KEY, acl_value, out_len, XATTR_CREATE,
                       &ctx->ucb, user);
    rs = common_sync_wait(ctx, rc);
    common_sync_api_free(ctx);
    if (rs != RED_SUCCESS)
    {
        COMMON_LOG("%s: set %s ACL=%s rs=%d", __func__, RED_S3_USER_ACL_XATTR_KEY,
                   acl_value, rs);
    }

    free(acl_value);

    return rs;
}
