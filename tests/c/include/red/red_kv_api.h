/******************************************************************************
 *
 * @file red_kv_api.h
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
 *  Name: red_kv_api.h
 *  Module:
 *  Project: RED
 *
 *  Description: This contains the methods that call the lower level rfs_*
 *      tasks and provide APIs that allow clients to build the SQE structures
 *      used to access the RED client library.
 *      These APIs are specific to the KV store.
 *
 *  Created: 4/4/2025
 *  Authors: Noel Otterness (notterness@ddn.com)
 *
 ******************************************************************************/

#ifndef RED_KV_API_HPP
#define RED_KV_API_HPP

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "red_client_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * API: red_kv_begin_transaction()
 *
 * @brief Create a transaction that can be used to commit multiple KV APIs
 *        in a single atomic operation.
 *
 * @param[in] root_oh The handle to the root of the bucket used for a KV store
 * @param[out] transaction The pointer to the transaction handle to return.
 * @param[in] transaction_name The name of the transaction. This can be set to
 *            nullptr to indicate there is no name.
 * @param[in] flags This defines the type of transaction. The two options are:
 *            RED_RO_TRANSACTION - Read Only transacton
 *            RED_RW_TRANSACTION - Read/Write transaction
 * @param[in] ucb The callback for when the APU completes
 * @param[in] api_user The user making the call.
 */
int red_kv_begin_transaction(rfs_open_hndl_t    root_oh,          /* Bucket root open handle */
                             red_transaction_t *transaction,      /* Output parameter for transaction handle */
                             const char        *transaction_name, /* optional name for the transaction */
                             uint32_t           flags,            /* Transaction flags */
                             rfs_usercb_t      *ucb,              /* User callback */
                             red_api_user_t    *api_user);        /* API user context */

/*
 * API: red_kv_commit_transaction()
 *
 * @brief Commit a transaction with all of the KV requests that are part of
 *        it and update the read GTX.
 *
 * @param[in] root_oh The handle to the root of the bucket used for a KV store
 * @param[out] transaction The transaction handle.
 * @param[in] ucb The callback for when the APU completes
 * @param[in] api_user The user making the call.
 */
int red_kv_commit_transaction(rfs_open_hndl_t    root_oh,         /* Bucket root open handle */
                              red_transaction_t  transaction,     /* Transaction handle */
                              rfs_usercb_t      *ucb,             /* User callback */
                              red_api_user_t    *api_user);       /* API user context */

/*
 * API: red_kv_cancel_transaction()
 *
 * @brief Cancel a transaction with all of the KV requests that are part of
 *        it. This will revert the keyspace back to prior to the transaction
 *        start.
 *
 * @param[in] root_oh The handle to the root of the bucket used for a KV store
 * @param[out] transaction The transaction handle to cancel.
 * @param[in] ucb The callback for when the APU completes
 * @param[in] api_user The user making the call.
 */
int red_kv_cancel_transaction(rfs_open_hndl_t    root_oh,        /* Bucket root open handle */
                              red_transaction_t  transaction,    /* Transaction handle */
                              rfs_usercb_t      *ucb,            /* User callback */
                              red_api_user_t    *api_user);      /* API user context */

int red_kv_list_transactions(rfs_open_hndl_t         root_oh,           /* Bucket root open handle */
                             size_t                  buffer_size,
                             red_transaction_info_t *transactions,      /* Buffer to return the transactions */
                             uint64_t                last_transaction,  /* Used to continue */
                             rfs_usercb_t           *ucb,               /* User callback */
                             red_api_user_t         *api_user);         /* API user context */

/*
 * API: red_kv_put()
 *
 * @brief Write data to the KV bucket at a specified key.
 *
 * @param[in] root_oh The handle to the root of the bucket used for a KV store
 * @param[in] transaction The transaction this PUT operation is for. The
 *            transaction is optional.
 * @param[in] key The key to attach the data to. The key is not NULL terminated.
 * @param[in] key_len The length of the key in bytes.
 * @param[in] offset The location to start writing to in the KV object or -1..
 * @param[in] data A scatter/gather list of the data to write to the object.
 * @param[in] flags This allows control of the PUT behavior. Optional
 * @param[in, out] checksum_out If this is not nullptr, then any computed
 *            checksums will be returned in the provided structure.
 * @param[in] ucb The callback for when the APU completes
 * @param[in] api_user The user making the call.
 */
int red_kv_put(rfs_open_hndl_t       root_oh,      /* Bucket root open handle */
               red_transaction_t     transaction,  /* Transaction handle or RED_NO_TRANSACTION */
               const char           *key,          /* Key to store, may include NULL bytes */
               size_t                key_len,      /* Length of the key (does not include a NULL */
                                                   /* terminator) */
               off_t                 offset,       /* Location in the KV object to begin writing */
               red_sg_list_t        *data,         /* Data buffer(s) containing the value */
               uint32_t              flags,        /* Optional flags */
               red_data_integrity_t *checksum_out, /* Optional output parameter for the calculated */
                                                   /* checksum */
               rfs_usercb_t         *ucb,          /* User callback */
               red_api_user_t       *api_user);    /* API user context */

/*
 * API: red_kv_get()
 *
 * @brief Read data from the KV bucket at a specified key.
 *
 * @param[in] root_oh The handle to the root of the bucket used for a KV store
 * @param[in] transaction The transaction this GET operation is for. The
 *            transaction is optional.
 * @param[in] key The key to read the data from. The key is not NULL terminated.
 * @param[in] key_len The length of the key in bytes.
 * @param[in] offset - Where to start reading the data from in the object.
 * @param[in] data A scatter/gather list of the data to read data from the
 *            object into. This will set the total number of bytes to read.
 * @param[in] flags This allows control of the GET behavior. Optional
 * @param[in, out] checksum_out If this is not nullptr, then any computed
 *            checksums will be returned in the provided structure.
 * @param[in] ucb The callback for when the APU completes
 * @param[in] api_user The user making the call.
 */
int red_kv_get(rfs_open_hndl_t        root_oh,       /* Bucket root open handle */
               red_transaction_t      transaction,   /* Transaction handle or RED_NO_TRANSACTION */
               const char            *key,           /* Key to retrieve */
               size_t                 key_len,
               off_t                  offset,        /* Where to start returning the data from. */
               red_sg_list_t         *data,          /* Buffer(s) to store the retrieved value */
               uint32_t               flags,         /* Optional flags */
               red_data_integrity_t *checksum_out,   /* Optional output parameter for the retrieved checksum */
               rfs_usercb_t         *ucb,            /* User callback */
               red_api_user_t       *api_user);      /* API user context */

/*
 * API: red_kv_erase()
 *
 * @brief Delete a specified key from the KV bucket.
 *
 * @param[in] root_oh The handle to the root of the bucket used for a KV store
 * @param[in] transaction The transaction this ERASE operation is for. The
 *            transaction is optional.
 * @param[in] key The key to erase from the bucket. The key is not NULL terminated.
 * @param[in] key_len The length of the key in bytes.
 * @param[in] flags This allows control of the ERASE behavior. Optional
 * @param[in] ucb The callback for when the APU completes
 * @param[in] api_user The user making the call.
 */
int red_kv_erase(rfs_open_hndl_t    root_oh,        /* Bucket root open handle */
                 red_transaction_t  transaction,    /* Transaction handle or RED_NO_TRANSACTION */
                 const char        *key,            /* Key to delete */
                 size_t             key_len,
                 uint32_t           flags,          /* Optional flags */
                 rfs_usercb_t      *ucb,            /* User callback */
                 red_api_user_t    *api_user);      /* API user context */

/*
 * API: red_kv_list()
 *
 * @brief List the keys and some of their attributes for a particular KV store.
 *
 * @param[in] marker The kye to start listing from
 * @param[in] marker_len How long the marker key is
 * @param[in,out] list A buffer used to hold the results of the listing operation
 *            This buffer must be provided by the caller.
 * @param[in] size The size of the buffer in bytes
 * @param[out] ret_size How many bytes of the buffer are used.
 * @param[in] flags Flags to control the listing of the keys.
 * @parampin] prefix Prefix to use in the listing operation.
 * @param[in] prefix_len Length of the prefix in bytes
 * @param[in] delimiter Delimiter used in the listing of the keys.
 * @param[in] last_ret_marker The place to pick the listing up at.
 * @param[in] ret_marker_len The length of the last_ret_marker in bytes.
 * @param[in] ucb The callback for when the APU completes
 * @param[in] api_user The user making the call.
 */
int red_kv_list(red_dir_stream_t                dirs,            /* Directory stream */
                const char                     *marker,          /* Pagination marker */
                const uint16_t                  marker_len,
                red_s3_list_objects_entry_v2_t *list,            /* Output buffer for results */
                uint32_t                        size,            /* Size of output buffer */
                uint32_t                       *ret_size,        /* Output parameter for actual */
                                                                 /* size used */
                uint32_t                        flags,           /* Optional flags */
                const char                     *prefix,          /* Optional prefix filter */
                const uint16_t                  prefix_len,
                const char                     *delimiter,       /* Optional delimiter */
                char                           *last_ret_marker, /* Output parameter for last marker */
                uint16_t                       *ret_marker_len,
                rfs_usercb_t                   *ucb,             /* User callback */
                red_api_user_t                 *api_user);       /* API user context */

/*
 * API: red_kv_batch_get()
 *
 * @brief Return a specific batch of keys in a single request.
 *
 * @param[in] oot_oh The handle to the root of the bucket used for a KV store
 * @param[in] transaction The transaction this ERASE operation is for. The
 *            transaction is optional.
 * @param[in] count The number of keys in the request.
 * @param[in] flags This allows control of the ERASE behavior. Optional
 * @param[in, out] results This is the list of keys and their scatter/gather
 *            arrays which the data is returned in.
 * @param[in] ucb The callback for when the APU completes
 * @param[in] api_user The user making the call.
 */
int red_kv_batch_get(rfs_open_hndl_t         root_oh,       /* Bucket root open handle */
                     red_transaction_t       transaction,   /* Transaction handle or */
                                                            /* RED_NO_TRANSACTION */
                     size_t                  count,         /* Number of keys */
                     uint32_t                flags,         /* Optional flags */
                     red_kv_batch_results_t *results,       /* Per-key results */
                     rfs_usercb_t           *ucb,           /* User callback */
                     red_api_user_t         *api_user);     /* API user context */

#ifdef __cplusplus
}
#endif

#endif /* RED_KV_API_H_ */
