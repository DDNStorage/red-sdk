/******************************************************************************
 *
 * @file rfs_dhash_api.h
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
 *  Name: rfs_dhash_api.h
 *  Module:
 *  Project: RED
 *
 *  Description: This contains the data hash algorithm enumeration and related
 *               functions for the RED client library.
 *
 *  Created:
 *  Authors: Bryant Ly (bly@ddn.com)
 *
 ******************************************************************************/
#ifndef RFS_DHASH_API_H
#define RFS_DHASH_API_H

#include <stddef.h>
#include <stdint.h>

#include "red_status.h"

#include "red_client_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Enumeration of supported data hash algorithms.
 *
 * This enum defines the various hash algorithms that can be used
 * for data hashing operations in the RED SDK.
 */
typedef enum
{
    RED_DHASH_NONE,      /**< No hashing */
    RED_DHASH_OFF,       /**< Hashing is turned off */
    RED_DHASH_MH_SHA256, /**< Multi-Hash SHA256 */
    RED_DHASH_SHA256,    /**< SHA256 */
    RED_DHASH_SHA512,    /**< SHA512 */
    RED_DHASH_CRC,       /**< CRC */
    RED_DHASH_MD5,       /**< MD5 */
    RED_DHASH_MAX        /**< Upper bound of hash types */
} red_dhash_e;

/**
 * @brief Compute a hash of the provided data.
 *
 * This function calculates a hash of the input data using the specified
 * hash algorithm.
 *
 * @param d The hash algorithm to use.
 * @param buf Pointer to the input data buffer.
 * @param len Length of the input data in bytes.
 * @param hashp Pointer to a buffer where the computed hash will be stored.
 *              The buffer should be large enough to hold the hash value.
 * @return A status code indicating success or failure of the operation.
 */
red_rc_t red_dhash_data(red_dhash_e d, const void *buf, size_t len, void *hashp);

/**
 * @brief Compute a seeded hash of the provided data.
 *
 * This function calculates a hash of the input data using the specified
 * hash algorithm and an initial seed value.
 *
 * @param d The hash algorithm to use.
 * @param buf Pointer to the input data buffer.
 * @param len Length of the input data in bytes.
 * @param hashp Pointer to a buffer where the computed hash will be stored.
 *              The buffer should be large enough to hold the hash value.
 * @param seed An initial seed value for the hash computation.
 * @return A status code indicating success or failure of the operation.
 */
red_rc_t red_dhash_data_seed(red_dhash_e d,
                             const void *buf,
                             size_t      len,
                             void       *hashp,
                             uint64_t    seed);

/**
 * @brief Convert binary array to hex string
 *
 * This function converts input binary array to hex representation where each byte is
 * represented by 2 hex digits Length of the resulting hex string is {length}*2+1 and it
 * is null terminated
 *
 * @param bin Pointer to the input binary buffer.
 * @param length Length of the input data in bytes.
 * @param hexstr Pointer to the buffer for the output hex string, null terminated
 */
void red_bin_to_hex(const uint8_t *bin, unsigned length, char *hexstr);

/**
 * @brief Convert binary array to hex string
 *
 * This function converts input binary array to hex representation where each byte is
 * represented by 2 hex digits Length of the resulting hex string is {length}*2+1 and it
 * is null terminated
 *
 * @param bin Pointer to the input binary buffer.
 * @param length Length of the input data in bytes.
 * @param hexstr Pointer to the buffer for the output hex string, null terminated
 * @attention Deprecated, use red_bin_to_hex instead
 */
RED_CLIENT_TYPES_DEPRECATED_MSG("Use red_bin_to_hex instead")
void bin_to_hex(const uint8_t *bin, unsigned length, char *hexstr);

#ifdef __cplusplus
}
#endif

#endif /* RFS_DHASH_API_H */
