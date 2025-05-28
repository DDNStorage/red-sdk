/******************************************************************************
 *
 * @copyright
 *                               --- WARNING ---
 *
 *     This work contains trade secrets of DataDirect Networks, Inc.  Any
 *     unauthorized use or disclosure of the work, or any part thereof, is
 *     strictly prohibited. Any use of this work without an express license
 *     or permission is in violation of applicable laws.
 *
 * @copyright DataDirect Networks, Inc. CONFIDENTIAL AND PROPRIETARY
 * @copyright DataDirect Networks Copyright, Inc. (c) 2021-2024. All rights reserved.
 *
 * @section DESCRIPTION
 *
 *  Project: RED
 *
 *  Description: This contains the methods for S3 operations
 *      in the RED client library.
 *
 ******************************************************************************/

#ifndef RED_S3_API_H_
#define RED_S3_API_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stddef.h>
#include "red_client_types.h"
#include "red_status.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup RED_S3_API RED S3 API
 * @{
 */

#define RFS_MAX_S3_KEY_CHARS       1025 /* Include NULL ternimator */
#define RFS_MAX_S3_UPLOAD_ID_CHARS 35   /* Include NULL terminator */
#define RFS_MIN_S3_UPLOAD_ID_CHARS 10

#define RED_S3_USER_XATTR_MAX_SIZE 256
#define RED_S3_USER_ETAG_SIZE      33 /* c-string of 32 hex digits */

/*
 * RED_S3_USER_MPART_ETAG_SIZE is the size of the etag for multipart objects.
 * This is needed for interoperability with reds3, as reds3 has below behavior -
 * For SPU, reds3 sets MD5 checksum as ETAG value
 * For MPU, reds3 sets MD5 checksum + delimiter (-) + no of parts as ETAG value
 *
 */
#define RED_S3_USER_MPART_ETAG_SIZE \
    39                            /* 32-byte hash + 1-byte delimiter + parts count(upto 10000)*/

#define RED_S3_CONT_TOKEN_SIZE 17 /* c-string of 16 hex digits */
#define RED_S3_MAX_KEY_SIZE    (1024 + 1) /* c-string of 1024 characters */
#define RED_MD5_SIZE           16

/* TODO: Needs to be removed once reds3 deprecates these from headers */
#define RED_S3_PART_CHKSUM_TYPE_KEY "user.checksum_algorithm"
#define RED_S3_PART_CHKSUM_KEY      "user.checksum"

/* XATTR keys for checksum information */
#define RED_CHKSUM_TYPE_KEY "user.checksum_algorithm"
#define RED_CHKSUM_KEY      "user.checksum"

/* XATTR keys for ETag support */
#define RED_S3_ETAG_KEY     "user.s3_etag"
#define RED_S3_ETAG_ALG     "system.etag_algorithm"

/* XATTR Keys for S3 bucket */
#define RED_S3_BUCKET_VERSION_XATTR_KEY "system.versioning"
#define RED_S3_USER_POLICY_XATTR_KEY    "user.s3_policy"
#define RED_S3_USER_ACL_XATTR_KEY       "user.s3_acl"
#define RED_S3_USER_META_KEY            "user.s3_meta"
#define RED_S3_BUCKET_CDATE_XATTR_KEY   "user.s3_cdate"

/* Allow object rename ops on S3 Bucket */
#define RED_S3_BUCKET_RENAME_XATTR_KEY "system.s3_allow_rename"

/*
 * Provide a limit to how much of an S3 key is traced
 */
#define RED_S3_KEY_TRACE_LEN 25

#define RFS_S3_NEXT_AVAIL(ent, buflen) ((ent->le_this_size) <= (buflen))
#define RFS_S3_NEXT_OBJ(ent, buflen)                                                     \
    ((buflen) -= (ent->le_this_size),                                                    \
     (red_s3_list_objects_entry_v2_t *)(((char *)ent) + ent->le_this_size))

/* TODO close bucket immediately if bucket_hndl = NULL */
/**
 * @brief Create an S3 bucket with specified configuration
 *
 * @note If bucket_hndl is NULL, the bucket will be closed immediately after creation
 *
 * @param[in] bucket_name Name of the bucket to create
 * @param[in] cluster Name of the cluster where the bucket will be created
 * @param[in] bucket_config Configuration properties for the bucket
 * @param[out] bucket_hndl Pointer to store the bucket handle (can be NULL)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_create_bucket_v3(const char         *bucket_name,
                            const char         *cluster,
                            red_ds_props_t     *bucket_config,
                            rfs_dataset_hndl_t *bucket_hndl,
                            rfs_usercb_t       *ucb,
                            red_api_user_t     *api_user);

/**
 * @brief Create an S3 bucket with data protection profile
 *
 * Extended version of red_s3_create_bucket_v3() that accepts a data protection profile.
 * This allows specifying additional protection characteristics for the bucket.
 * The bucket is created with versioning set to "unversioned".
 *
 * @param[in] bucket_name Name of the bucket to create
 * @param[in] cluster Name of the cluster where the bucket will be created
 * @param[in] dp_profile Data protection profile name
 * @param[in] bucket_props Configuration properties for the bucket
 * @param[out] bucket_hndl Pointer to store the bucket handle (can be NULL)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_create_bucket_v4(const char         *bucket_name,
                            const char         *cluster,
                            const char         *dp_profile,
                            red_ds_props_t     *bucket_props,
                            rfs_dataset_hndl_t *bucket_hndl,
                            rfs_usercb_t       *ucb,
                            red_api_user_t     *api_user);

int red_s3_create_bucket_v5(const char              *bucket_name,
                            const char              *cluster,
                            const char              *dp_profile,
                            red_ds_props_t          *bucket_props,
                            rfs_dataset_hndl_t      *bucket_hndl,
                            rfs_usercb_t            *ucb,
                            red_api_user_t          *api_user,
                            red_s3_checksum_type_e  etag_algorithm);

/**
 * @brief Set the owner of an S3 bucket
 *
 * @param[in] bucketname Name of the bucket to change ownership
 * @param[in] access_key Access key ID of the new owner
 * @param[in] access_secret Secret access key of the new owner
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_set_bucket_owner(const char     *bucketname,
                            const char     *access_key,
                            const char     *access_secret,
                            rfs_usercb_t   *ucb,
                            red_api_user_t *api_user);

/**
 * @brief List S3 buckets in the specified cluster
 *
 * @param[in] cluster Name of the cluster to list buckets from
 * @param[out] elems Array to store the bucket information
 * @param[in] size Size of the elems array (number of elements it can hold)
 * @param[out] ret_count Pointer to store the number of buckets returned
 * @param[out] stream Stream handle for pagination (NULL if not needed)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_list_buckets_v3(const char          *cluster,
                           red_s3_bucket_t     *elems,
                           size_t               size,
                           int                 *ret_count,
                           red_bucket_stream_t *stream,
                           rfs_usercb_t        *ucb,
                           red_api_user_t      *api_user);

/**
 * @brief Delete an S3 bucket from the specified cluster
 *
 * @param[in] bucket_name Name of the bucket to delete
 * @param[in] cluster Name of the cluster where the bucket resides
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_delete_bucket_v2(const char     *bucket_name,
                            const char     *cluster,
                            rfs_usercb_t   *ucb,
                            red_api_user_t *api_user);

/**
 * @brief Set S3 access credentials for the current session
 *
 * @param[in] access_key S3 access key ID
 * @param[in] access_secret S3 secret access key
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_set_access_key(const char     *access_key,
                          const char     *access_secret,
                          rfs_usercb_t   *ucb,
                          red_api_user_t *api_user);

/**
 * @brief Create an S3 bucket
 *
 * @param[in] bucket_name Name of the bucket to create
 * @param[in] cluster Name of the cluster where the bucket will be created
 * @param[in] tenname Tenant name
 * @param[in] subname Subtenant name
 * @param[in] bucket_config Configuration properties for the bucket
 * @param[out] bucket_hndl Pointer to store the handle to the created bucket
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_create_bucket(const char         *bucket_name,
                         const char         *cluster,
                         const char         *tenname,
                         const char         *subname,
                         red_ds_props_t     *bucket_config,
                         rfs_dataset_hndl_t *bucket_hndl,
                         rfs_usercb_t       *ucb,
                         red_api_user_t     *api_user);

/**
 * @brief Delete an S3 bucket
 *
 * @param[in] bucket_name Name of the bucket to delete
 * @param[in] cluster Name of the cluster where the bucket is located
 * @param[in] tenname Tenant name
 * @param[in] subname Subtenant name
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_delete_bucket(const char     *bucket_name,
                         const char     *cluster,
                         const char     *tenname,
                         const char     *subname,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user);

/**
 * @brief Set retention configuration for an S3 bucket
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] retention_mode Retention mode to set (GOVERNANCE, COMPLIANCE)
 * @param[in] retention_period Retention period in seconds
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int rfs_s3_set_bucket_retention(rfs_open_hndl_t      dir_oh,
                                red_retention_mode_e retention_mode,
                                uint64_t             retention_period,
                                rfs_usercb_t        *ucb,
                                red_api_user_t      *api_user);

/**
 * @brief Get retention configuration for an S3 bucket
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[out] retention_mode Pointer to store the current retention mode
 * @param[out] retention_period Pointer to store the current retention period in seconds
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int rfs_s3_get_bucket_retention(rfs_open_hndl_t       dir_oh,
                                red_retention_mode_e *retention_mode,
                                uint64_t             *retention_period,
                                rfs_usercb_t         *ucb,
                                red_api_user_t       *api_user);

/**
 * @brief Create a new version of an S3 object
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] flags Creation flags
 * @param[out] created_oh Pointer to store the open handle to the created object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_create_version(rfs_open_hndl_t  dir_oh,
                          const char      *s3_key,
                          int              flags,
                          rfs_open_hndl_t *created_oh,
                          rfs_usercb_t    *ucb,
                          red_api_user_t  *api_user) __nonnull((2));

/**
 * @brief Create a new version of an S3 object with upload ID
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] flags Creation flags
 * @param[out] upload_id_buffer Buffer to store the upload ID
 * @param[in] upload_id_buffer_nob Size of the upload ID buffer
 * @param[out] created_oh Pointer to store the open handle to the created object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_create_version_v2(rfs_open_hndl_t  dir_oh,
                             const char      *s3_key,
                             int              flags,
                             char            *upload_id_buffer,
                             size_t           upload_id_buffer_nob,
                             rfs_open_hndl_t *created_oh,
                             rfs_usercb_t    *ucb,
                             red_api_user_t  *api_user) __nonnull((2));

/**
 * @brief Publish an S3 object version
 *
 * @param[in] oh Open handle to the object
 * @param[out] version Pointer to store the version ID of the published object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_publish(rfs_open_hndl_t oh,
                   uint64_t       *version,
                   rfs_usercb_t   *ucb,
                   red_api_user_t *api_user);

/**
 * @brief Publish an S3 object version with data integrity verification
 *
 * @param[in] oh Open handle to the object
 * @param[out] version Pointer to store the version ID of the published object
 * @param[in,out] data_integrity Data integrity information for verification
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_publish_v2(rfs_open_hndl_t       oh,
                      uint64_t             *version,
                      red_data_integrity_t *data_integrity,
                      rfs_usercb_t         *ucb,
                      red_api_user_t       *api_user);

int red_s3_publish_v3(rfs_open_hndl_t       oh,
                      uint64_t             *version,
                      red_data_integrity_t *data_integrity,
                      red_retention_mode_e  retention_mode,
                      uint64_t              retain_until,
                      rfs_usercb_t         *ucb,
                      red_api_user_t       *api_user);

/**
 * @brief Open an S3 object
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] version Version ID of the object (0 for latest version)
 * @param[in] flags Open flags (e.g., O_RDONLY, O_RDWR)
 * @param[out] oh Pointer to store the open handle to the object
 * @param[out] out_version Pointer to store the actual version ID of the opened object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_open(rfs_open_hndl_t  dir_oh,
                const char      *s3_key,
                uint64_t         version,
                int              flags,
                rfs_open_hndl_t *oh,
                uint64_t        *out_version,
                rfs_usercb_t    *ucb,
                red_api_user_t  *api_user) __nonnull((2, 5, 6));

/**
 * @brief Get an S3 object
 *
 * @param[in] bucket_name Name of the bucket containing the object
 * @param[in] s3_key Object key
 * @param[in] params Parameters for the get operation (version, range, etc.)
 * @param[out] data Buffer to store the object data
 * @param[out] oh Optional pointer to store the open handle to the object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 * @retval RED_ETRUNC (-271) if the object is larger than the provided buffer and \p oh is nullptr
 */
int red_s3_get(const char          *bucket_name,
               const char          *s3_key,
               red_s3_get_params_t *params,
               red_buffer_t        *data,
               rfs_open_hndl_t     *oh,
               rfs_usercb_t        *ucb,
               red_api_user_t      *api_user) __nonnull((1, 2, 3, 6, 7));

/**
 * @brief Put an S3 object
 *
 * @param[in] bucket_name Name of the bucket to store the object
 * @param[in] s3_key Object key
 * @param[in] params Parameters for the put operation (metadata, ACL, etc.)
 * @param[in] data Buffer containing the object data
 * @param[out] oh Optional pointer to store the open handle to the created object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_put(const char          *bucket_name,
               const char          *s3_key,
               red_s3_put_params_t *params,
               const red_buffer_t  *data,
               rfs_open_hndl_t     *oh,
               rfs_usercb_t        *ucb,
               red_api_user_t      *api_user) __nonnull((1, 2, 3, 6, 7));

/**
 * @brief Delete an S3 object
 *
 * @param[in] bucket_name Name of the bucket containing the object
 * @param[in] s3_key Object key
 * @param[in] version Version ID of the object to delete (0 for latest version)
 * @param[in] flags Control flags for the delete operation
 * @param[out] retversion Pointer to store the version ID of the delete marker (if created)
 * @param[out] is_delete_marker Pointer to indicate if a delete marker was created
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_delete_object(const char     *bucket_name,
                         const char     *s3_key,
                         uint64_t        version,
                         int             flags,
                         uint64_t       *retversion,
                         bool           *is_delete_marker,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user) __nonnull((1, 2, 5, 6, 7, 8));

/**
 * @brief Create a delete marker for an S3 object
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[out] retversion Pointer to store the version ID of the created delete marker
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_delete_marker(rfs_open_hndl_t dir_oh,
                         const char     *s3_key,
                         uint64_t       *retversion,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user) __nonnull((2, 3));

/**
 * @brief Create a delete marker for an S3 object with additional options
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] flags Control flags for the delete marker creation
 * @param[out] retversion Pointer to store the version ID of the created delete marker
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_delete_marker_v2(rfs_open_hndl_t dir_oh,
                            const char     *s3_key,
                            int             flags,
                            uint64_t       *retversion,
                            rfs_usercb_t   *ucb,
                            red_api_user_t *api_user) __nonnull((2, 4));

/**
 * @brief Complete a multipart upload
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] num_parts Number of parts to combine
 * @param[in] part Array of part information (part numbers and ETags)
 * @param[in] flags Control flags for the completion operation
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_complete_multipart(rfs_open_hndl_t   dir_oh,
                           const char       *s3_key,
                           uint32_t          num_parts,
                           const red_part_t *part,
                           int               flags,
                           rfs_usercb_t     *ucb,
                           red_api_user_t   *api_user) __nonnull((2, 4));

/**
 * @brief List parts of a multipart upload
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] start_part_num Part number to start listing from
 * @param[out] info Buffer to store part information
 * @param[in] info_buff_size Size of the info buffer in bytes
 * @param[out] num_parts Pointer to store the number of parts returned
 * @param[out] next_part_num Pointer to store the next part number for pagination
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_list_parts(rfs_open_hndl_t  dir_oh,
                   const char      *s3_key,
                   uint32_t         start_part_num,
                   red_part_info_t *info,
                   uint32_t         info_buff_size,
                   uint32_t        *num_parts,
                   uint32_t        *next_part_num,
                   rfs_usercb_t    *ucb,
                   red_api_user_t  *api_user) __nonnull((2, 4, 6, 7));

/**
 * @brief Create a multipart upload
 *
 * @param[in] root_oh Open handle to the bucket's root directory
 * @param[in] s3_key Object key
 * @param[in] flags Creation flags
 * @param[out] upload_id Buffer to store the upload ID
 * @param[in] upload_id_nob Size of the upload ID buffer
 * @param[out] created_oh Pointer to store the open handle to the created object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_create_mpart_upload(rfs_open_hndl_t  root_oh,
                            const char      *s3_key,
                            int              flags,
                            char            *upload_id,
                            size_t           upload_id_nob,
                            rfs_open_hndl_t *created_oh,
                            rfs_usercb_t    *ucb,
                            red_api_user_t  *api_user) __nonnull((2, 4, 6, 7, 8));

/**
 * @brief List parts of a multipart upload (version 2)
 *
 * @param[in] root_oh Open handle to the bucket's root directory
 * @param[in] s3_key Object key
 * @param[in] upload_id Upload ID of the multipart upload
 * @param[in] part_num_marker Part number to start listing from
 * @param[out] info Buffer to store part information
 * @param[in] info_nob Size of the info buffer in bytes
 * @param[out] num_parts Pointer to store the number of parts returned
 * @param[out] next_part_num Pointer to store the next part number for pagination
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_list_parts_v2(rfs_open_hndl_t     root_oh,
                      const char         *s3_key,
                      const char         *upload_id,
                      uint32_t            part_num_marker,
                      red_part_info_v2_t *info,
                      uint32_t            info_nob,
                      uint32_t           *num_parts,
                      uint32_t           *next_part_num,
                      rfs_usercb_t       *ucb,
                      red_api_user_t     *api_user) __nonnull((2, 3, 5, 7, 8, 9, 10));

/**
 * @brief Close a part of a multipart upload
 *
 * @param[in] oh Open handle to the part
 * @param[in,out] part_integrity Data integrity information for verification
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_close_part(rfs_open_hndl_t       oh,
                   red_data_integrity_t *part_integrity,
                   rfs_usercb_t         *ucb,
                   red_api_user_t       *api_user) __nonnull((2, 3, 4));

/*
 * Methods to perform s3 operations
 */

/*
 * Struct holding most of ListMultipartUploads arguments.
 *
 * @param[in] prefix
 * @param[in] delimiter
 * @param[in] key_marker
 * @param[in] upload_id_marker
 * @param[in,out] uploads_buffer The block of memory to hold the returned data
 *                               for the multipart uploads in progress.
 * @param[in] uploads_buffer_nob The size in bytes of the uploads_buffer.
 * @param[in] max_uploads The maximum number of upload information to
 *                        return.
 *
 * Keep this outside of rfs_lib_list_mpart_uploads_sqe_t to avoid blowing the
 * sqe size up.
 */
typedef struct rfs_lib_list_mpart_uploads_args
{
    const char      *prefix;
    const char      *delimiter;
    const char      *key_marker;
    const char      *upload_id_marker;
    char            *next_key_marker;
    char            *next_upload_id_marker;
    red_mp_upload_t *uploads_buffer;
    size_t           uploads_nr;
    size_t          *uploads_ret_nr;
} rfs_lib_list_mpart_uploads_args_t;

/**
 * @brief List multipart uploads in progress
 *
 * @param[in] dirs Directory stream for the bucket
 * @param[in,out] args Arguments for the list operation including filters and output buffers
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_list_mpart_uploads(red_dir_stream_t                   dirs,
                           rfs_lib_list_mpart_uploads_args_t *args,
                           rfs_usercb_t                      *ucb,
                           red_api_user_t *api_user) __nonnull((2, 3, 4));

/**
 * @brief Upload a part for a multipart upload
 *
 * @param[in] root_oh Open handle to the bucket's root directory
 * @param[in] s3_key Object key
 * @param[in] upload_id Upload ID of the multipart upload
 * @param[in] part_num Part number (1-10000)
 * @param[in] flags Control flags for the upload operation
 * @param[out] part_oh Pointer to store the open handle to the part
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_upload_part(rfs_open_hndl_t  root_oh,
                    const char      *s3_key,
                    const char      *upload_id,
                    uint32_t         part_num,
                    int              flags,
                    rfs_open_hndl_t *part_oh,
                    rfs_usercb_t    *ucb,
                    red_api_user_t  *api_user) __nonnull((2, 3, 6, 7, 8));

/**
 * @brief Complete a multipart upload
 *
 * @param[in] root_oh Open handle to the bucket's root directory
 * @param[in] s3_key Object key
 * @param[in] upload_id Upload ID of the multipart upload
 * @param[in] num_parts Total number of parts in the multipart upload
 * @param[in] upload_parts Number of parts provided in the parts array
 * @param[in] parts Array of part information (part numbers and ETags)
 * @param[in] final_parts Flag indicating if these are the final parts
 * @param[in] flags Control flags for the completion operation
 * @param[out] mp_obj_info Pointer to store information about the completed object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_comp_mpart_upload(rfs_open_hndl_t     root_oh,
                          const char         *s3_key,
                          const char         *upload_id,
                          const uint32_t      num_parts,
                          const uint32_t      upload_parts,
                          red_part_info_v2_t *parts,
                          bool                final_parts,
                          uint32_t            flags,
                          red_mp_info_t      *mp_obj_info,
                          rfs_usercb_t       *ucb,
                          red_api_user_t     *api_user) __nonnull((2, 3, 6, 9, 10, 11));

/**
 * @brief Abort a multipart upload
 *
 * @param[in] root_oh Open handle to the bucket's root directory
 * @param[in] s3_key Object key
 * @param[in] upload_id Upload ID of the multipart upload to abort
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_abort_mpart(rfs_open_hndl_t root_oh,
                    const char     *s3_key,
                    const char     *upload_id,
                    rfs_usercb_t   *ucb,
                    red_api_user_t *api_user) __nonnull((2, 3, 4, 5));

/**
 * @brief Get the size of a part in a multipart object
 *
 * @param[in] obj_oh Open handle to the multipart object
 * @param[in] part_num Part number to query
 * @param[out] part_size Pointer to store the size of the part
 * @param[out] part_offset Pointer to store the offset of the part within the object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_get_part_size(rfs_open_hndl_t obj_oh,
                      const uint32_t  part_num,
                      size_t         *part_size,
                      uint64_t       *part_offset,
                      rfs_usercb_t   *ucb,
                      red_api_user_t *api_user) __nonnull((3, 4, 5, 6));

/**
 * @brief Get the size and ETag of a part in a multipart object
 *
 * @param[in] obj_oh Open handle to the multipart object
 * @param[in] part_num Part number to query
 * @param[out] part_size Pointer to store the size of the part
 * @param[out] part_offset Pointer to store the offset of the part within the object
 * @param[out] part_etag Buffer to store the ETag of the part
 * @param[in] part_etag_size Size of the part_etag buffer
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_get_part_size_v2(rfs_open_hndl_t obj_oh,
                         const uint32_t  part_num,
                         size_t         *part_size,
                         uint64_t       *part_offset,
                         char           *part_etag,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user) __nonnull((3, 4, 5, 6, 7));

/*
 * red_abort_multipart() is deprecated as it does not pass in the
 * upload_id
 */
int red_abort_multipart(rfs_open_hndl_t dir_oh,
                        const char     *s3_key,
                        rfs_usercb_t   *ucb,
                        red_api_user_t *api_user) __nonnull((2));

int red_abort_multipart_v2(rfs_open_hndl_t dir_oh,
                           const char     *s3_key,
                           const char     *upload_id,
                           rfs_usercb_t   *ucb,
                           red_api_user_t *api_user) __nonnull((2, 3, 4, 5));

/*
 * S3 part info structures reside in memory sequentially.
 * Use the following method to get a pointer to next structure
 */
/*
 * red_next_s3_partinfo() is an iterator of a flat buffer filled with
 * instances of red_s3_part_info_t. The in-memory format is as follow:
 *
 * <red_s3_part_info_t[0]><etag[0]>...<red_s3_part_info_t[n]><etag[n]>
 *
 * Size occupied by <etag[x]> is red_s3_part_info_t[x].etag_size
 */
static inline red_part_info_t *red_next_s3_partinfo(const red_part_info_t *p)
{
    size_t nob = sizeof(red_part_info_t) + p->xattr_info.etag_size;

    uintptr_t next = ((uintptr_t)p + nob + 7LU) & ~7LU; /* aligned to 8 bytes */
    return (red_part_info_t *)next;
}

int red_test_ring_interface(rfs_open_hndl_t  dir_oh,
                            const char      *tgt_name,
                            int              flags,
                            rfs_open_hndl_t *created_oh,
                            rfs_usercb_t    *ucb,
                            red_api_user_t  *api_user);

/**
 * @brief Read versions of an S3 object
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[out] elems Array to store version elements
 * @param[in] count Maximum number of versions to retrieve
 * @param[out] ret_count Pointer to store the actual number of versions retrieved
 * @param[in,out] dirp Directory stream for continuation
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_read_versions(rfs_open_hndl_t    dir_oh,
                         const char        *s3_key,
                         red_s3_ver_elem_t *elems,
                         int                count,
                         int               *ret_count,
                         red_dir_stream_t  *dirp,
                         rfs_usercb_t      *ucb,
                         red_api_user_t    *api_user) __nonnull((2, 3, 5, 6));

/**
 * @brief Read versions of an S3 object with additional flags
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] flags Control flags for the operation
 * @param[out] elems Array to store version elements
 * @param[in] count Maximum number of versions to retrieve
 * @param[out] ret_count Pointer to store the actual number of versions retrieved
 * @param[in,out] dirp Directory stream for continuation
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_read_versions_v2(rfs_open_hndl_t    dir_oh,
                            const char        *s3_key,
                            int                flags,
                            red_s3_ver_elem_t *elems,
                            int                count,
                            int               *ret_count,
                            red_dir_stream_t  *dirp,
                            rfs_usercb_t      *ucb,
                            red_api_user_t    *api_user) __nonnull((2, 4, 6, 7));

/**
 * @brief Close a directory stream used for S3 operations
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] dirp Directory stream to close
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_close_stream(rfs_open_hndl_t   dir_oh,
                        red_dir_stream_t *dirp,
                        rfs_usercb_t     *ucb,
                        red_api_user_t   *api_user) __nonnull((2));

/**
 * @brief Erase an S3 object or create a delete marker
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] version Version ID of the object to erase (0 for latest version)
 * @param[out] curr_version Pointer to store the version ID of the delete marker (if created)
 * @param[out] is_delete_marker Pointer to indicate if a delete marker was created
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_erase(rfs_open_hndl_t dir_oh,
                 const char     *s3_key,
                 uint64_t        version,
                 uint64_t       *curr_version,
                 bool           *is_delete_marker,
                 rfs_usercb_t   *ucb,
                 red_api_user_t *api_user) __nonnull((2, 4, 5));

/**
 * @brief Erase an S3 object with additional options
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] s3_key Object key
 * @param[in] version Version ID of the object to erase (0 for latest version)
 * @param[in] flags Control flags for the erase operation
 * @param[in] retention_flags Retention flags for the operation
 * @param[out] curr_version Pointer to store the version ID of the delete marker (if created)
 * @param[out] is_delete_marker Pointer to indicate if a delete marker was created
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_erase_v2(rfs_open_hndl_t       dir_oh,
                    const char           *s3_key,
                    uint64_t              version,
                    int                   flags,
                    red_retention_flags_e retention_flags,
                    uint64_t             *curr_version,
                    bool                 *is_delete_marker,
                    rfs_usercb_t         *ucb,
                    red_api_user_t       *api_user) __nonnull((2, 6, 7));

/* The test version of the API is used exclusively for internal testing ONLY! */
/**
 * @brief Test version of the erase API (for internal testing only)
 *
 * @param[in] dir_oh Open handle to the bucket's directory
 * @param[in] name Object key
 * @param[in] version Version ID of the object to erase
 * @param[in] lookup_flags Flags for the lookup operation
 * @param[in] erase_flags Flags for the erase operation
 * @param[out] curr_version Pointer to store the version ID of the delete marker
 * @param[out] is_delete_marker Pointer to indicate if a delete marker was created
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_erase_v2_test(rfs_open_hndl_t dir_oh,
                         const char     *name,
                         uint64_t        version,
                         int             lookup_flags,
                         int             erase_flags,
                         uint64_t       *curr_version,
                         bool           *is_delete_marker,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user);

/**
 * @brief Read directory entries in ordered fashion
 *
 * @param[in] dirs Directory stream
 * @param[out] list Buffer to store the ordered list entries
 * @param[in] size Size of the list buffer in bytes
 * @param[out] ret_size Pointer to store the actual size used in the buffer
 * @param[in] match_regex Regular expression to match entries (NULL for no filtering)
 * @param[in] ignore_regex Regular expression to ignore entries (NULL for no filtering)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_ordered_readdir(red_dir_stream_t          dirs,
                           red_ordered_list_entry_t *list,
                           size_t                    size,
                           size_t                   *ret_size,
                           const char               *match_regex,
                           const char               *ignore_regex,
                           rfs_usercb_t             *ucb,
                           red_api_user_t           *api_user);

/**
 * @brief Read directory entries in ordered fashion with additional flags
 *
 * @param[in] dirs Directory stream
 * @param[out] list Buffer to store the ordered list entries
 * @param[in] size Size of the list buffer in bytes
 * @param[out] ret_size Pointer to store the actual size used in the buffer
 * @param[in] flags Control flags for the operation
 * @param[in] match_regex Regular expression to match entries (NULL for no filtering)
 * @param[in] ignore_regex Regular expression to ignore entries (NULL for no filtering)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_ordered_readdir_v2(red_dir_stream_t          dirs,
                              red_ordered_list_entry_t *list,
                              size_t                    size,
                              size_t                   *ret_size,
                              int                       flags,
                              const char               *match_regex,
                              const char               *ignore_regex,
                              rfs_usercb_t             *ucb,
                              red_api_user_t           *api_user);

/**
 * @brief List S3 buckets
 *
 * @param[in] cluster Name of the cluster
 * @param[in] tenname Tenant name
 * @param[in] subname Subtenant name
 * @param[out] elems Array to store bucket information
 * @param[in] size Size of the elems array in bytes
 * @param[out] ret_count Pointer to store the number of buckets retrieved
 * @param[in,out] stream Bucket stream for continuation
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_list_buckets(const char          *cluster,
                        const char          *tenname,
                        const char          *subname,
                        red_s3_bucket_t     *elems,
                        size_t               size,
                        int                 *ret_count,
                        red_bucket_stream_t *stream,
                        rfs_usercb_t        *ucb,
                        red_api_user_t      *api_user) __nonnull((1, 2, 3, 4, 6, 7));

/**
 * @brief List objects in an S3 bucket
 *
 * @param[in] dirs Directory stream for the bucket
 * @param[out] list Buffer to store the object entries
 * @param[in] size Size of the list buffer in bytes
 * @param[out] ret_size Pointer to store the actual size used in the buffer
 * @param[in] flags Control flags for the operation
 * @param[in] prefix Prefix to filter objects (NULL for no filtering)
 * @param[in] delimiter Delimiter for hierarchical listing (NULL for flat listing)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_list_objects(red_dir_stream_t             dirs,
                        red_s3_list_objects_entry_t *list,
                        size_t                       size,
                        size_t                      *ret_size,
                        uint32_t                     flags,
                        const char                  *prefix,
                        const char                  *delimiter,
                        rfs_usercb_t                *ucb,
                        red_api_user_t *api_user) __nonnull((2, 4, 6, 7, 8, 9));

/**
 * @brief List objects in an S3 bucket (version 1)
 *
 * @param[in] dirs Directory stream for the bucket
 * @param[in] marker Key to start listing from (exclusive)
 * @param[out] list Buffer to store the object entries
 * @param[in] size Size of the list buffer in bytes
 * @param[out] ret_size Pointer to store the actual size used in the buffer
 * @param[in] flags Control flags for the operation
 * @param[in] prefix Prefix to filter objects (NULL for no filtering)
 * @param[in] delimiter Delimiter for hierarchical listing (NULL for flat listing)
 * @param[out] last_ret_marker Buffer to store the last key returned (for pagination)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_list_objects_v1(red_dir_stream_t             dirs,
                           const char                  *marker,
                           red_s3_list_objects_entry_t *list,
                           size_t                       size,
                           size_t                      *ret_size,
                           uint32_t                     flags,
                           const char                  *prefix,
                           const char                  *delimiter,
                           char                        *last_ret_marker,
                           rfs_usercb_t                *ucb,
                           red_api_user_t *api_user) __nonnull((3, 5, 7, 8, 10, 11));

/**
 * @brief List objects in an S3 bucket (version 2)
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] flags Control flags for the operation
 * @param[in] params Parameters for the list operation (prefix, delimiter, etc.)
 * @param[out] result Structure to store the listing results
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_list_objects_v2(const char                   *bucket_name,
                           flags_t                       flags,
                           red_s3_list_objects_params_t *params,
                           red_s3_list_objects_result_t *result,
                           rfs_usercb_t                 *ucb,
                           red_api_user_t *api_user) __nonnull((1, 3, 4, 5, 6));

/**
 * @brief List objects in an S3 bucket (version 3)
 *
 * @param[in] dirs Directory stream for the bucket
 * @param[in] marker Key to start listing from (exclusive)
 * @param[out] list Buffer to store the enhanced object entries
 * @param[in] size Size of the list buffer in bytes
 * @param[out] ret_size Pointer to store the actual size used in the buffer
 * @param[in] flags Control flags for the operation
 * @param[in] prefix Prefix to filter objects (NULL for no filtering)
 * @param[in] delimiter Delimiter for hierarchical listing (NULL for flat listing)
 * @param[out] last_ret_marker Buffer to store the last key returned (for pagination)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_list_objects_v3(red_dir_stream_t                dirs,
                           const char                     *marker,
                           red_s3_list_objects_entry_v2_t *list,
                           size_t                          size,
                           size_t                         *ret_size,
                           uint32_t                        flags,
                           const char                     *prefix,
                           const char                     *delimiter,
                           char                           *last_ret_marker,
                           rfs_usercb_t                   *ucb,
                           red_api_user_t *api_user) __nonnull((3, 5, 7, 8, 10, 11));

static inline attrs_t *red_s3_next_attrs(attrs_t *attrs)
{
    size_t len = sizeof(attrs_t) + attrs->a_name_len + attrs->a_dihash_len +
                 attrs->a_cookie_len + 3;
    len = (len + 7LU) & ~7LU; /* attrs_t is 8 byte aligned */
    return (attrs_t *)((char *)(attrs) + len);
}

/* Get the s3 config in a json format */
/**
 * @brief Get S3 configuration in JSON format
 *
 * @param[out] config_buf Buffer to store the configuration JSON
 * @param[in] config_buf_size Size of the configuration buffer in bytes
 * @return RED_SUCCESS on success, error code on failure
 */
red_status_t red_s3_get_config(char *config_buf, size_t config_buf_size);

/**
 * @brief Create a multipart upload for an S3 object
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] flags Control flags for the operation
 * @param[out] upload_id_buffer Buffer to store the upload ID
 * @param[in] upload_id_buffer_nob Size of the upload ID buffer
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_create_multipart_upload(const char     *bucket_name,
                                   const char     *key,
                                   int             flags,
                                   char           *upload_id_buffer,
                                   size_t          upload_id_buffer_nob,
                                   rfs_usercb_t   *ucb,
                                   red_api_user_t *api_user) __nonnull((1, 2, 4, 6, 7));

/**
 * @brief Create a multipart upload for an S3 object (version 3)
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] flags Control flags for the operation
 * @param[out] upload_id Buffer to store the upload ID
 * @param[in] upload_id_nob Size of the upload ID buffer
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_create_multipart_upload_v3(const char     *bucket_name,
                                      const char     *key,
                                      int             flags,
                                      char           *upload_id,
                                      size_t          upload_id_nob,
                                      rfs_usercb_t   *ucb,
                                      red_api_user_t *api_user)
    __nonnull((1, 2, 4, 6, 7));

/**
 * @brief Abort a multipart upload
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] upload_id Upload ID of the multipart upload to abort
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_abort_multipart_upload(const char     *bucket_name,
                                  const char     *key,
                                  const char     *upload_id,
                                  rfs_usercb_t   *ucb,
                                  red_api_user_t *api_user) __nonnull((1, 2, 3, 4, 5));

/**
 * @brief Abort a multipart upload (version 3)
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] upload_id Upload ID of the multipart upload to abort
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_abort_multipart_upload_v3(const char     *bucket_name,
                                     const char     *key,
                                     const char     *upload_id,
                                     rfs_usercb_t   *ucb,
                                     red_api_user_t *api_user) __nonnull((1, 2, 3, 4, 5));

/**
 * @brief Upload a part for a multipart upload
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] upload_id Upload ID of the multipart upload
 * @param[in] part_no Part number (1-10000)
 * @param[in] data Buffer containing the part data
 * @param[out] etag Buffer to store the ETag of the uploaded part
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_upload_part(const char         *bucket_name,
                       const char         *key,
                       const char         *upload_id,
                       unsigned            part_no,
                       const red_buffer_t *data,
                       char               *etag,
                       rfs_usercb_t       *ucb,
                       red_api_user_t     *api_user) __nonnull((1, 2, 3, 5, 6, 7, 8));

/**
 * @brief Upload a part for a multipart upload (version 3)
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] upload_id Upload ID of the multipart upload
 * @param[in] part_no Part number (1-10000)
 * @param[in] data Buffer containing the part data
 * @param[out] etag Buffer to store the ETag of the uploaded part
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_upload_part_v3(const char         *bucket_name,
                          const char         *key,
                          const char         *upload_id,
                          unsigned            part_no,
                          const red_buffer_t *data,
                          char               *etag,
                          rfs_usercb_t       *ucb,
                          red_api_user_t     *api_user) __nonnull((1, 2, 3, 5, 6, 7, 8));

/**
 * @brief Complete a multipart upload
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] upload_id Upload ID of the multipart upload
 * @param[in] flags Control flags for the operation
 * @param[in] parts Array of part information (part numbers and ETags)
 * @param[in] num_parts Number of parts in the parts array
 * @param[out] etag Buffer to store the ETag of the completed object
 * @param[out] version Pointer to store the version ID of the completed object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_complete_multipart_upload(const char          *bucket_name,
                                     const char          *key,
                                     const char          *upload_id,
                                     int                  flags,
                                     const red_s3_part_t *parts,
                                     unsigned             num_parts,
                                     char                *etag,
                                     uint64_t            *version,
                                     rfs_usercb_t        *ucb,
                                     red_api_user_t      *api_user)
    __nonnull((1, 2, 3, 5, 7, 8, 9, 10));

/**
 * @brief Complete a multipart upload (version 3)
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] upload_id Upload ID of the multipart upload
 * @param[in] flags Control flags for the operation
 * @param[in] parts Array of part information (part numbers and ETags)
 * @param[in] num_parts Number of parts in the parts array
 * @param[out] etag Buffer to store the ETag of the completed object
 * @param[out] version Pointer to store the version ID of the completed object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_complete_multipart_upload_v3(const char          *bucket_name,
                                        const char          *key,
                                        const char          *upload_id,
                                        int                  flags,
                                        const red_s3_part_t *parts,
                                        unsigned             num_parts,
                                        char                *etag,
                                        uint64_t            *version,
                                        rfs_usercb_t        *ucb,
                                        red_api_user_t      *api_user)
    __nonnull((1, 2, 3, 5, 7, 8, 9, 10));

/**
 * @brief List multipart uploads in a bucket
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] params Parameters for the list operation (prefix, delimiter, etc.)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_list_multipart_uploads(const char                     *bucket_name,
                                  red_s3_list_multipart_params_t *params,
                                  rfs_usercb_t                   *ucb,
                                  red_api_user_t                 *api_user);

/**
 * @brief Get bucket information
 *
 * @param[in] bucket_name Name of the bucket
 * @param[out] bucket_info Structure to store the bucket information
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_get_bucket_info(const char        *bucket_name,
                           red_bucket_info_t *bucket_info,
                           rfs_usercb_t      *ucb,
                           red_api_user_t    *api_user);

/**
 * @brief Get object metadata without retrieving the object data
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] key Object key
 * @param[in] headers Request headers for the operation
 * @param[out] info Structure to store the object information
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_head_object(const char              *bucket_name,
                       const char              *key,
                       red_s3_object_headers_t *headers,
                       red_s3_object_info_t    *info,
                       rfs_usercb_t            *ucb,
                       red_api_user_t          *api_user) __nonnull((1, 2, 3, 5, 6));

/**
 * @brief Copy an object from one location to another
 *
 * @param[in] bucket_name Destination bucket name
 * @param[in] key Destination object key
 * @param[in] src_bucket Source bucket name
 * @param[in] src_key Source object key
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_copy_object(const char     *bucket_name,
                       const char     *key,
                       const char     *src_bucket,
                       const char     *src_key,
                       rfs_usercb_t   *ucb,
                       red_api_user_t *api_user);

/**
 * @brief Rename an object within a bucket
 *
 * @param[in] bucket_name Name of the bucket
 * @param[in] old_key Current object key
 * @param[in] new_key New object key
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_rename_object(const char     *bucket_name,
                         const char     *old_key,
                         const char     *new_key,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user) __nonnull((1, 2, 3, 4, 5));

/**
 * @brief Close an S3 object after a PUT operation
 *
 * @param[in] oh Open handle to the S3 object
 * @param[in] publish Flag indicating whether to publish the object
 * @param[out] data_integrity Structure to store data integrity information
 * @param[out] version Pointer to store the version ID of the published object
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_s3_put_close(rfs_open_hndl_t       oh,
                     bool                  publish,
                     red_data_integrity_t *data_integrity,
                     uint64_t             *version,
                     rfs_usercb_t         *ucb,
                     red_api_user_t       *api_user) __nonnull((3, 4, 5));

/*
 * APIs that provide additional debug capabilities to the object interface.
 */
/**
 * @brief Open a specific part of a multipart object
 *
 * @param[in] obj_oh Open handle to the parent object
 * @param[in] part_num Part number to open
 * @param[out] part_oh Pointer to store the open handle to the part
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_open_part(rfs_open_hndl_t  obj_oh,
                  uint32_t         part_num,
                  rfs_open_hndl_t *part_oh,
                  rfs_usercb_t    *ucb,
                  red_api_user_t  *api_user) __nonnull((3));

/**
 * @brief Get object information
 *
 * @param[in] root_oh Open handle to the bucket's root directory
 * @param[in] s3_key Object key
 * @param[in] version Version ID of the object (0 for latest version)
 * @param[out] info Structure to store the object information
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_get_obj_info(rfs_open_hndl_t root_oh,
                     const char     *s3_key,
                     uint64_t        version,
                     red_obj_info_t *info,
                     rfs_usercb_t   *ucb,
                     red_api_user_t *api_user) __nonnull((2, 4, 5));

/** @}*/ /* end of RED_S3_API group */

#ifdef __cplusplus
}
#endif

#endif /* RED_S3_API_H_ */
