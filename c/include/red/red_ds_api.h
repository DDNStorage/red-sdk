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
 *  Description: This contains the methods for dataset operations and extended attributes
 *      in the RED client library.
 *
 ******************************************************************************/

#ifndef RED_DATASET_API_H_
#define RED_DATASET_API_H_

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
 * @defgroup RED_DATASET_API RED Dataset API
 * @{
 */

/**
 * @brief Convert a string to a dataset flavor.
 *
 * @param[in] str String representation of the flavor.
 * @return red_ds_flavor_e The corresponding dataset flavor.
 */
red_ds_flavor_e red_tdsp_str2flavor(const char *str);

/**
 * @brief Convert a string to a dataset flavor.
 *
 * @param[in] str String representation of the flavor.
 * @return red_ds_flavor_e The corresponding dataset flavor.
 * @attention Deprecated, use red_tdsp_str2flavor instead
 */
RED_CLIENT_TYPES_DEPRECATED
red_ds_flavor_e tdsp_str2flavor(const char *str);

/**
 * @brief Convert a dataset flavor to a string.
 *
 * @param[in] flavor Dataset flavor to convert.
 * @return const char* String representation of the flavor.
 */
const char *red_tdsp_flavor2str(red_ds_flavor_e flavor);

/**
 * @brief Convert a dataset flavor to a string.
 *
 * @param[in] flavor Dataset flavor to convert.
 * @return const char* String representation of the flavor.
 * @attention Deprecated, use red_tdsp_flavor2str instead
 */
RED_CLIENT_TYPES_DEPRECATED
const char *tdsp_flavor2str(red_ds_flavor_e flavor);

/**
 * @brief Create a new dataset
 *
 * Create a new dataset as specified by dataset path (ds_path) using dataset properties
 * (ds_props). Dataset properties include bucket size, block size, pool ID, number of data
 * and parity (if Erasure coding is enabled)
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] ds_path path to dataset (in format "/tenant/subtenant/ds")
 * @param[in] ds_props dataset properties
 * @param[out] ds_hndl handle referring to the dataset
 *
 * @return RED status
 */
red_status_t red_ds_create(uint32_t              auth_index,
                           const char           *ds_path,
                           const red_ds_props_t *ds_props,
                           red_ds_hndl_t        *ds_hndl);

/**
 * @brief Open a dataset
 *
 * Open an existing dataset using the tenant and subtenant.  The caller provides a pointer
 * to the dataset handle which is populated by this call.
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] ds_name dataset name
 * @param[out] ds_hndl handle referring to the dataset
 *
 * @return RED status
 */
red_status_t red_ds_open(uint32_t auth_index, const char *ds_name, red_ds_hndl_t *ds_hndl);

/**
 * @brief Close previously opened data set
 * @param[in] ds_hndl handle referring to the dataset
 *
 * @return RED status
 */
red_status_t red_ds_close(red_ds_hndl_t *ds_hndl);

/**
 * Delete a dataset
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] ds_path path to dataset (in format "/tenant/subtenant/ds")
 *
 * @return RED status
 */
red_status_t red_ds_delete(uint32_t auth_index, const char *ds_path);

/**
 * @brief Get dataset properties
 *
 * @param[in] ds_hndl handle referring to the dataset
 * @param[out] ds_props dataset properties
 *
 * @return RED status
 */
red_status_t red_ds_get_props(red_ds_hndl_t ds_hndl, red_ds_props_t *ds_props);

#define RED_DS_DEFAULT_DP_PROFILE 1
red_status_t red_ds_get_default_props(red_ds_props_t *ds_props, uint32_t dp_profile_id);

/**
 * @brief Check dataset properties
 *
 * @param[in] ds_hndl handle referring to the dataset
 *
 * @return RED status
 */
red_status_t red_ds_props_check(red_ds_hndl_t *ds_hndl);

/*
 * @brief Query quota limits/capacity. 'capacities' array should be large enough
 *        to hold capacites for all datasets passed-in ds_name array
 * @param ds_name Array of dataset names.
 * @param cluster Cluster Name, not an array of strings.
 * @param capacities Array of struct red_ds_capacity
 * @param cnt Number of entries in ds_name array
 * @param ret_cnt Number of entres actually returned with capacity filled-in
 */
int red_obtain_capacity(red_dataset_name_entry_t *ds_name,
                        const char               *cluster,
                        struct red_ds_capacity   *capacities,
                        int                       cnt,
                        int                      *ret_cnt,
                        rfs_usercb_t             *ucb,
                        red_api_user_t           *api_user);

/**
 * @brief Open or create a dataset for use by the client library
 *
 * @param[in] ds_name Name of the dataset to open or create
 * @param[in] cluster Name of the cluster where the dataset resides
 * @param[in] ds_config Dataset properties for creation (NULL to open existing dataset)
 * @param[out] ds_hndl Pointer to store the dataset handle on success
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_obtain_dataset(const char         *ds_name,
                       const char         *cluster,
                       red_ds_props_t     *ds_config,
                       rfs_dataset_hndl_t *ds_hndl,
                       rfs_usercb_t       *ucb,
                       red_api_user_t     *api_user);

/* DEPRECATED: used red_obtain_dataset() instead */
/**
 * @brief Open or create a dataset for use by the client library (deprecated)
 *
 * @param[in] ds_name Name of the dataset to open or create
 * @param[in] cluster Name of the cluster where the dataset resides
 * @param[in] ds_config Dataset properties for creation (NULL to open existing dataset)
 * @param[out] ds_hndl Pointer to store the dataset handle on success
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_obtain_dataset_v3(const char         *ds_name,
                          const char         *cluster,
                          red_ds_props_t     *ds_config,
                          rfs_dataset_hndl_t *ds_hndl,
                          rfs_usercb_t       *ucb,
                          red_api_user_t     *api_user);

/**
 * @brief Open or create a dataset with data protection profile
 *
 * Extended version of red_obtain_dataset() that accepts a data protection profile.
 * This allows specifying additional protection characteristics for the dataset.
 *
 * @param[in] ds_name Name of the dataset to open or create
 * @param[in] cluster Name of the cluster where the dataset resides
 * @param[in] dp_profile Data protection profile name
 * @param[in] ds_props Dataset properties for creation (NULL to open existing dataset)
 * @param[out] ds_hndl Pointer to store the dataset handle on success
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_obtain_dataset_v4(const char         *ds_name,
                          const char         *cluster,
                          const char         *dp_profile,
                          red_ds_props_t     *ds_props,
                          rfs_dataset_hndl_t *ds_hndl,
                          rfs_usercb_t       *ucb,
                          red_api_user_t     *api_user);

/**
 * @brief List datasets available in the specified cluster
 *
 * @param[out] buffer Buffer to store the NULL-terminated dataset names
 * @param[in] size Size of the buffer in bytes
 * @param[in] cluster Name of the cluster to list datasets from
 * @param[out] count Pointer to store the number of datasets returned
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_list_datasets(char           *buffer,
                      size_t          size,
                      const char     *cluster,
                      size_t         *count,
                      rfs_usercb_t   *ucb,
                      red_api_user_t *api_user);

/* DEPRECATED: use red_list_datasets() instead */
/**
 * @brief List datasets available in the specified cluster (deprecated)
 *
 * @param[out] buffer Buffer to store the NULL-terminated dataset names
 * @param[in] size Size of the buffer in bytes
 * @param[in] cluster Name of the cluster to list datasets from
 * @param[out] count Pointer to store the number of datasets returned
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_list_datasets_v3(char           *buffer,
                         size_t          size,
                         const char     *cluster,
                         size_t         *count,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user);

/**
 * @brief Delete a dataset from the specified cluster
 *
 * @param[in] ds_name Name of the dataset to delete
 * @param[in] cluster Name of the cluster where the dataset resides
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_delete_dataset(const char     *ds_name,
                       const char     *cluster,
                       rfs_usercb_t   *ucb,
                       red_api_user_t *api_user);

/**
 * @brief Forcibly delete a dataset from the specified cluster
 *
 * @param[in] ds_name Name of the dataset to delete
 * @param[in] cluster Name of the cluster where the dataset resides
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_delete_dataset_v4(const char     *ds_name,
                          const char     *cluster,
                          rfs_usercb_t   *ucb,
                          red_api_user_t *api_user);

/* DEPRECATED: use red_delete_dataset() instead */
/**
 * @brief Delete a dataset from the specified cluster (deprecated)
 *
 * @param[in] ds_name Name of the dataset to delete
 * @param[in] cluster Name of the cluster where the dataset resides
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_delete_dataset_v3(const char     *ds_name,
                          const char     *cluster,
                          rfs_usercb_t   *ucb,
                          red_api_user_t *api_user);

/**
 * @brief Close a dataset handle with cluster specification (deprecated)
 *
 * @param[in] ds_hndl Dataset handle to close
 * @param[in] cluster Name of the cluster where the dataset resides
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_close_dataset_v1(rfs_dataset_hndl_t ds_hndl,
                         const char        *cluster,
                         rfs_usercb_t      *ucb,
                         red_api_user_t    *api_user);

/**
 * @brief Close a dataset handle
 *
 * @param[in] ds_hndl Dataset handle to close
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_close_dataset(rfs_dataset_hndl_t ds_hndl,
                      rfs_usercb_t      *ucb,
                      red_api_user_t    *api_user);

/**
 * @brief Get usage statistics for a dataset
 *
 * @param[in] ds_hndl Handle to the dataset
 * @param[out] usage Pointer to a red_dataset_usage_t structure to be filled with usage data
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_dataset_get_usage(rfs_dataset_hndl_t   ds_hndl,
                          red_dataset_usage_t *usage,
                          rfs_usercb_t        *ucb,
                          red_api_user_t      *api_user);

/**
 * @brief Obtain the root directory handle for the dataset.
 */
int red_open_root(rfs_dataset_hndl_t ds_hndl,
                  rfs_open_hndl_t   *root_dirfd,
                  rfs_usercb_t      *ucb,
                  red_api_user_t    *api_user);

/**
 * @brief Clean up temporary files in a dataset
 *
 * @param[in] ds_hndl Dataset handle to clean up
 * @param[in] tmpfile_oh Open handle to a temporary file directory (can be 0)
 * @param[in] ucb User callback for asynchronous completion
 * @param[out] found Pointer to store the number of temporary files removed
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_cleanup_tmpfiles(rfs_dataset_hndl_t ds_hndl,
                         rfs_open_hndl_t    tmpfile_oh,
                         rfs_usercb_t      *ucb,
                         size_t            *found,
                         red_api_user_t    *api_user);

/**
 * @brief Obtains dataset identity based on dataset handle
 *        Fills in ident parameter on success.
 *
 * @param ds_hndl          - dataset handle
 * @param ds_ident         - Pointer to dataset identity that would be used
 *                           to return dataset identity info upon success.
 */
int red_get_dsident(rfs_dataset_hndl_t ds_hndl, red_ds_ident_t *ident);

/**
 * @brief Obtains dataset flavor based on dataset handle
 *        Fills in flavor parameter on success.
 *
 * @param ds_hndl[in]      - dataset handle
 * @param flavor[out]      - Pointer to dataset flavor
 */
int red_get_ds_flavor(rfs_dataset_hndl_t ds_hndl, red_ds_flavor_e *flavor);

/**
 * @brief Perform open operation based on the open handle
 *        returns pointer to open handle upon success.
 *        flags supplied by the caller (refer to flags param).
 *
 * @param f_hndl           - pointer to file_handle structure that holds the
 *                           file_handle data.
 * @param ds_hndl          - dataset handle
 * @param flags            - supported open flags. Please refer to flags under
 *                           https://man7.org/linux/man-pages/man2/open.2.html.
 * @param oh               - Pointer to open handle that would be used to return
 *                           a reference to open handle upon success.
 * @param user_cb          - call back function after completion.
 * @param user             - user identity context, e.g. uid, gid... etc.
 */
int red_open_by_handle_at(const struct file_handle *f_hndl,
                          rfs_dataset_hndl_t        ds_hndl,
                          int                       flags,
                          rfs_open_hndl_t          *oh,
                          rfs_usercb_t             *ucb,
                          red_api_user_t           *api_user);

/*
 * Methods used to search extended attributes.
 */
/**
 * @brief Open a search extended attribute stream
 *
 * @param[in] ds_hndl Dataset handle
 * @param[out] xstrp Pointer to store the search extended attribute stream handle
 * @param[in] key Extended attribute key to search for
 * @param[in] pattern Pattern to match in the extended attribute values
 * @param[in] pattern_nob Size of the pattern in bytes
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_srch_open_xattr_stream(rfs_dataset_hndl_t       ds_hndl,
                               red_srch_xattr_stream_t *xstrp,
                               const char              *key,
                               const void              *pattern,
                               size_t                   pattern_nob,
                               rfs_usercb_t            *ucb,
                               red_api_user_t          *api_user) __nonnull((2, 3, 6, 7));

/**
 * @brief Close a search extended attribute stream
 *
 * @param[in] xstrp Search extended attribute stream to close
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_srch_close_xattr_stream(red_srch_xattr_stream_t xstrp,
                                rfs_usercb_t           *ucb,
                                red_api_user_t         *api_user) __nonnull((2, 3));

/**
 * @brief Read search results from a search extended attribute stream
 *
 * @param[in] xstrp Search extended attribute stream to read from
 * @param[out] buf Buffer to store the search results
 * @param[in] size Size of the buffer in bytes
 * @param[out] buf_nob Pointer to store the number of bytes written to the buffer
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_srch_read_xattrs(red_srch_xattr_stream_t xstrp,
                         char                   *buf,
                         size_t                  size,
                         size_t                 *buf_nob,
                         rfs_usercb_t           *ucb,
                         red_api_user_t         *api_user) __nonnull((2, 4, 5, 6));

/*
 * Get num of objects in a dataset
 */
/**
 * @brief Get the number of objects in a dataset
 *
 * @param[in] oh Dataset handle
 * @param[out] nfiles Pointer to store the number of files
 * @param[out] ndirs Pointer to store the number of directories
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_ds_nobjects(rfs_dataset_hndl_t oh,
                    int64_t           *nfiles,
                    int64_t           *ndirs,
                    rfs_usercb_t      *ucb,
                    red_api_user_t    *api_user);

/*
 * Check if the rfs_inode_t map (rd_inode_cache_t) is empty except for
 * the root inode.
 */
/**
 * @brief Check if the inode map is empty
 *
 * @param[in] ds_hndl Dataset handle
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_check_inode_map(rfs_dataset_hndl_t ds_hndl,
                        rfs_usercb_t      *ucb,
                        red_api_user_t    *api_user);

/* Cleanup the "pending delete" upserts */
/**
 * @brief Clean up pending delete upserts
 *
 * @param[in] ds_hndl Dataset handle
 * @param[in] ucb User callback for asynchronous completion
 * @param[out] found Pointer to store the number of pending delete upserts found
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_cleanup(rfs_dataset_hndl_t ds_hndl,
                rfs_usercb_t      *ucb,
                size_t            *found,
                red_api_user_t    *api_user);

/**
 * @brief Execute a group of no-operation commands
 *
 * @param[in] ds_hndl Dataset handle
 * @param[in] conf Configuration for the no-op group
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_noop_group(rfs_dataset_hndl_t           ds_hndl,
                   const rfs_noop_group_conf_t *conf,
                   rfs_usercb_t                *ucb,
                   red_api_user_t              *api_user);

/** @}*/ /* end of RED_DATASET_API group */
#ifdef __cplusplus
}
#endif

#endif /* RED_DATASET_API_H_ */
