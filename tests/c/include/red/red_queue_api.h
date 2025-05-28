/******************************************************************************
 *
 *  @file red_queue_api.h
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
 *   Name:       red_queue_api.h
 *   Project:    RED
 *
 *   Description:
 *      The public client API for REDQUEUE (RQ).
 *
 *   Created:    2024-05-14
 *   Author(s):  Christian Blume (cblume@ddn.com)
 *
 ******************************************************************************/
#ifndef RED_QUEUE_API_H
#define RED_QUEUE_API_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* Version information */
#define RED_QUEUE_API_VERSION_MAJOR 1
#define RED_QUEUE_API_VERSION_MINOR 0
#define RED_QUEUE_API_VERSION_PATCH 0

#define RED_QUEUE_API_VERSION                                                            \
    ((RED_QUEUE_API_VERSION_MAJOR << 16) | (RED_QUEUE_API_VERSION_MINOR << 8) |          \
     (RED_QUEUE_API_VERSION_PATCH))

/* Compatibility macros */
#define RED_QUEUE_API_VERSION_CHECK(major, minor, patch)                                 \
    (RED_QUEUE_API_VERSION >= ((major << 16) | (minor << 8) | (patch)))

/* Feature availability macros */
#if RED_QUEUE_API_VERSION_CHECK(1, 0, 0)
#define RED_QUEUE_API_HAS_GROUP_OPERATIONS
#endif

#include "red_client_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup RED_QUEUE_API RED Queue API
 * @{
 */

/**
 * @brief Allocates the default queue parameters
 *
 * @return red_queue_params_hndl_t Handle representing the queue parameters.
 */
red_queue_params_hndl_t red_q_alloc_queue_params();

/**
 * @brief Deallocates the queue parameters
 *
 * @param[in] params The queue parameters
 */
void red_q_dealloc_queue_params(red_queue_params_hndl_t params);

/**
 * @brief Returns the queue parameter for retention_ms:
 *        Max time in milliseconds to keep messages for (0 = disabled)
 *        [default = 86400000]
 *
 * @param[in] params The queue parameter hndl
 * @return uint64_t retention_ms
 */
uint64_t red_q_get_queue_param_retention_ms(red_queue_params_hndl_t params);

/**
 * @brief Sets the queue parameter for retention_ms:
 *        Max time in milliseconds to keep messages for (0 = disabled)
 *        [default = 86400000]
 *
 * @param[in] params The queue parameter hndl
 * @param[in] retention_ms The parameter
 */
void red_q_set_queue_param_retention_ms(red_queue_params_hndl_t params,
                                        uint64_t                retention_ms);

/**
 * @brief Returns the queue parameter for msg_max_size_bytes:
 *        The max size of a single message in bytes (0 = disabled) [default = 0]
 *
 * @param[in] params The queue parameter hndl
 * @return uint64_t msg_max_size_bytes
 */
uint64_t red_q_get_queue_param_msg_max_size_bytes(red_queue_params_hndl_t params);

/**
 * @brief Sets the queue parameter for msg_max_size_bytes:
 *        The max size of a single message in bytes (0 = disabled) [default = 0]
 *
 * @param[in] params The queue parameter hndl
 * @param[in] msg_max_size_bytes The parameter
 */
void red_q_set_queue_param_msg_max_size_bytes(red_queue_params_hndl_t params,
                                              uint64_t                msg_max_size_bytes);

/**
 * @brief Returns the queue parameter for flags:
 *        Can be: 0, O_EXCL; with O_EXCL meaning that an error is
 *        returned if the queue exists already [default = 0]
 *
 * @param[in] params The queue parameter hndl
 * @return uint32_t flags
 */
uint32_t red_q_get_queue_param_flags(red_queue_params_hndl_t params);

/**
 * @brief Sets the queue parameter for flags:
 *        Can be: 0, O_EXCL; with O_EXCL meaning that an error is
 *        returned if the queue exists already [default = 0]
 *
 * @param[in] params The queue parameter hndl
 * @param[in] flags The parameter
 */
void red_q_set_queue_param_flags(red_queue_params_hndl_t params, uint32_t flags);

/**
 * @brief Returns the queue parameter for mode (permission bits):
 *        Can be: S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP, S_IROTH, S_IWOTH
 *        [default = all enabled]
 *
 * @param params The queue parameter hndl
 * @return uint32_t mode
 */
uint32_t red_q_get_queue_param_mode(red_queue_params_hndl_t params);

/**
 * @brief Sets the queue parameter for mode (permission bits):
 *        Can be: S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP, S_IROTH, S_IWOTH
 *        [default = all enabled]
 *
 * @param[in] params The queue parameter hndl
 * @param[in] mode The parameter
 */
void red_q_set_queue_param_mode(red_queue_params_hndl_t params, uint32_t mode);

/**
 * @brief Creates a new queue.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue_name Name of the queue to create.
 * @param[in] params Queue parameters.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_create(rfs_dataset_hndl_t      ds_hndl,
                 const char             *queue_name,
                 red_queue_params_hndl_t params,
                 rfs_usercb_t           *ucb,
                 const red_api_user_t   *api_user);

/**
 * @brief Deletes an existing queue.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue_name Name of the queue to delete.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_delete(rfs_dataset_hndl_t    ds_hndl,
                 const char           *queue_name,
                 rfs_usercb_t         *ucb,
                 const red_api_user_t *api_user);

/**
 * @brief Opens a queue.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue_name Name of the queue to open.
 * @param[in] flags Open flags (O_RDONLY, O_WRONLY, O_RDWR).
 * @param[out] queue Pointer to store the opened queue handle.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_open(rfs_dataset_hndl_t    ds_hndl,
               const char           *queue_name,
               uint32_t              flags,
               red_queue_hndl_t     *queue,
               rfs_usercb_t         *ucb,
               const red_api_user_t *api_user);

/**
 * @brief Closes a queue.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue Queue handle to close.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_close(rfs_dataset_hndl_t    ds_hndl,
                red_queue_hndl_t      queue,
                rfs_usercb_t         *ucb,
                const red_api_user_t *api_user);

/**
 * @brief Returns the queue's entity
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue Queue handle.
 * @param[out] entity The queue's entity
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_queue_entity(const rfs_dataset_hndl_t ds_hndl,
                     const red_queue_hndl_t   queue,
                     red_q_entity_t          *entity,
                     rfs_usercb_t            *ucb,
                     const red_api_user_t    *api_user);

/**
 * @brief Allocates the default attach parameters
 *
 * @return red_q_attach_params_hndl_t Handle representing the attach parameters.
 */
red_q_attach_params_hndl_t red_q_alloc_attach_params();

/**
 * @brief Deallocates the attach parameters
 *
 * @param[in] params The attach parameters
 */
void red_q_dealloc_attach_params(red_q_attach_params_hndl_t params);

/**
 * @brief Returns the attach parameter for flags:
 *        0 = group must exist already,
 *        O_CREAT = group is created if not exist yet [default],
 *        O_EXCL = error is returned if group exists already and O_CREAT is passed
 *
 * @param params The attach parameter hndl
 * @return uint32_t flags
 */
uint32_t red_q_get_attach_param_flags(red_q_attach_params_hndl_t params);

/**
 * @brief Sets the attach parameter for flags:
 *        0 = group must exist already,
 *        O_CREAT = group is created if not exist yet [default],
 *        O_EXCL = error is returned if group exists already and O_CREAT is passed
 *
 * @param[in] params The attach parameter hndl
 * @param[in] flags The parameter
 */
void red_q_set_attach_param_flags(red_q_attach_params_hndl_t params, uint32_t flags);

/**
 * @brief Returns the attach parameter for delivery_type:
 *        RED_QUEUE_DELIVERY_NONE = No consumable delivery is taking place [default],
 *        RED_QUEUE_DELIVERY_SPARSE = No message payload is delivered but all other
 *                                    consumable members,
 *        RED_QUEUE_DELIVERY_FULL = The full consumable is delivered
 *
 * @param params The attach parameter hndl
 * @return red_q_delivery_type_e delivery_type
 */
red_q_delivery_type_e red_q_get_attach_param_delivery_type(
    red_q_attach_params_hndl_t params);

/**
 * @brief Sets the attach parameter for delivery_type:
 *        RED_QUEUE_DELIVERY_NONE = No consumable delivery is taking place [default],
 *        RED_QUEUE_DELIVERY_SPARSE = No message payload is delivered but all other
 *                                    consumable members,
 *        RED_QUEUE_DELIVERY_FULL = The full consumable is delivered
 *
 * @param[in] params The attach parameter hndl
 * @param[in] delivery_type The parameter
 */
void red_q_set_attach_param_delivery_type(red_q_attach_params_hndl_t params,
                                          red_q_delivery_type_e      delivery_type);

/**
 * @brief Returns the attach parameter for delivery_gtx:
 *        delivery_gtx=RED_QUEUE_DELIVERY_GTX_NEW -> Deliver only new messages [default],
 *        delivery_gtx=other -> Deliver messages newer or equal to gtx
 *
 * @param params The attach parameter hndl
 * @return red_q_gtx_t delivery_gtx
 */
red_q_gtx_t red_q_get_attach_param_delivery_gtx(red_q_attach_params_hndl_t params);

/**
 * @brief Sets the attach parameter for delivery_gtx:
 *        delivery_gtx=RED_QUEUE_DELIVERY_GTX_NEW -> Deliver only new messages [default],
 *        delivery_gtx=other -> Deliver messages newer or equal to gtx
 *
 * @param[in] params The attach parameter hndl
 * @param[in] delivery_gtx The parameter
 */
void red_q_set_attach_param_delivery_gtx(red_q_attach_params_hndl_t params,
                                         red_q_gtx_t                delivery_gtx);

/**
 * @brief Returns the attach parameter for delivery_core:
 *        delivery_core=RED_QUEUE_DELIVERY_CORE_ANY -> Any client core [default]
 *        delivery_core=other -> Deliver on this particular client core
 *
 * @param params The attach parameter hndl
 * @return uint64_t delivery_core
 */
uint32_t red_q_get_attach_param_delivery_core(red_q_attach_params_hndl_t params);

/**
 * @brief Sets the attach parameter for delivery_core:
 *        delivery_core=RED_QUEUE_DELIVERY_CORE_ANY -> Any client core [default]
 *        delivery_core=other -> Deliver on this particular client core
 *
 * @param[in] params The attach parameter hndl
 * @param[in] delivery_core The parameter
 */
void red_q_set_attach_param_delivery_core(red_q_attach_params_hndl_t params,
                                          uint32_t                   delivery_core);

/**
 * @brief Sets the consumer callback to receive consumables on [default = null]
 *
 * @param[in] params The attach parameter hndl
 * @param[in] consumer The call-back function
 * @param[in] consumer The additional argument to the call-back function
 */
void red_q_set_attach_param_consumer(red_q_attach_params_hndl_t params,
                                     red_q_consumer_t           consumer,
                                     void                      *arg);

/**
 * @brief Attaches a group to the given `queue`.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue Queue handle.
 * @param[in] group_name Name of the group to attach.
 * @param[in] params The attach params
 * @param[out] group Pointer to store the attached group handle.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_attach(rfs_dataset_hndl_t         ds_hndl,
                 red_queue_hndl_t           queue,
                 const char                *group_name,
                 red_q_attach_params_hndl_t params,
                 red_q_group_hndl_t        *group,
                 rfs_usercb_t              *ucb,
                 const red_api_user_t      *api_user);

/**
 * @brief Detaches a group from a queue.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue Queue handle.
 * @param[in] group_name Name of the group to detach.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_detach(rfs_dataset_hndl_t    ds_hndl,
                 red_queue_hndl_t      queue,
                 const char           *group_name,
                 rfs_usercb_t         *ucb,
                 const red_api_user_t *api_user);

/**
 * @brief Leaves a group.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] group Group handle to leave.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_leave(rfs_dataset_hndl_t    ds_hndl,
                red_q_group_hndl_t    group,
                rfs_usercb_t         *ucb,
                const red_api_user_t *api_user);

/**
 * @brief List the queues in the dataset.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[inout] The buffer to return NUL-terminated queue names.
 * @param[inout] Continuation cookie. Zero on initial call.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_list(rfs_dataset_hndl_t    ds_hndl,
               red_api_buf_t         buf,
               red_api_buf_t         cookie,
               rfs_usercb_t         *ucb,
               const red_api_user_t *api_user);

/**
 * @brief Returns the group's entity
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] group Group handle.
 * @param[out] entity The group's entity
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_group_entity(const rfs_dataset_hndl_t ds_hndl,
                       const red_q_group_hndl_t group,
                       red_q_entity_t          *entity,
                       rfs_usercb_t            *ucb,
                       const red_api_user_t    *api_user);

/**
 * @brief Publishes new messages to the given `queue` and `partition`.
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue Queue handle.
 * @param[in] partition The partition.
 * @param[in] size The number of messages.
 * @param[in] msg The array of messages to publish, must be at least of size `size`.
 * @param[out] gtx The array of message gtx, can be NULL, otherwise must be at least of
 * size `size`.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_put(rfs_dataset_hndl_t    ds_hndl,
              red_queue_hndl_t      queue,
              uint32_t              partition,
              uint64_t              size,
              const red_q_msg_t    *msg,
              red_q_gtx_t          *gtx,
              rfs_usercb_t         *ucb,
              const red_api_user_t *api_user);

/**
 * @brief Acknowledges message receipt for `group` and `partition` at the given `gtx`
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] group Group handle to acknowledge for
 * @param[in] partition The partition.
 * @param[in] gtx The gtx tick
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_ack(rfs_dataset_hndl_t    ds_hndl,
              red_q_group_hndl_t    group,
              uint32_t              partition,
              red_q_gtx_t           gtx,
              rfs_usercb_t         *ucb,
              const red_api_user_t *api_user);

/**
 * @brief Retrieves the acknowledgment `gtx` for `group` and `partition`
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] group Group handle
 * @param[in] partition The partition.
 * @param[out] gtx The acknowledgment gtx tick
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_retrieve_ack(rfs_dataset_hndl_t    ds_hndl,
                       red_q_group_hndl_t    group,
                       uint32_t              partition,
                       red_q_gtx_t          *gtx,
                       rfs_usercb_t         *ucb,
                       const red_api_user_t *api_user);

/**
 * @brief Fetches consumables for `queue` and `partition`, newer or equal to `gtx`
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[in] queue Queue handle.
 * @param[in] partition The partition.
 * @param[in] gtx The gtx.
 * @param[in] size The max number of consumables to fetch.
 * @param[out] consumables Array of consumables (must be fully pre-allocated)
 * @param[out] count The actual number of consumables received
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_get(rfs_dataset_hndl_t    ds_hndl,
              red_queue_hndl_t      queue,
              uint32_t              partition,
              red_q_gtx_t           gtx,
              uint64_t              size,
              red_q_consumable_t   *consumables,
              uint64_t             *count,
              rfs_usercb_t         *ucb,
              const red_api_user_t *api_user);

/**
 * @brief Allocates an array of consumables including msg memory
 *
 * @param[in] size The number of consumables to allocate
 * @return Array of consumables
 */
red_q_consumable_t *red_q_alloc_consumables(uint64_t size);

/**
 * @brief Deallocates an array of `consumables`
 *
 * @param[in] consumables A pointer to an array of consumables
 * @param[in] size The array size
 */
void red_q_dealloc_consumables(red_q_consumable_t *consumables, uint64_t size);

/**
 * @brief Returns the number of partitions of the given dataset
 *
 * @param[in] ds_hndl Dataset handle.
 * @param[out] count The partition count.
 * @param[in] ucb User callback information.
 * @param[in] api_user API user information.
 * @return int Status code indicating success or failure.
 */
int red_q_partition_count(rfs_dataset_hndl_t    ds_hndl,
                          uint32_t             *count,
                          rfs_usercb_t         *ucb,
                          const red_api_user_t *api_user);

/**
 * @brief Advances the given GTX such that the resulting GTX
 *        is minimally newer than the input.
 *
 * @param[in,out] gtx The GTX
 */
void red_q_advance_gtx(red_q_gtx_t *gtx);

#ifdef __cplusplus
}
#endif

#endif /* RED_QUEUE_API_H */
