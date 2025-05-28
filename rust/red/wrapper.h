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
 *   Name:       wrapper.h
 *   Project:    RED
 *
 *   Description: Single file wrapper for red headers as bindgen has issues with multiple
 *                header files
 *
 ******************************************************************************/

#include <red/red_client_types.h>
#include <red/red_client_api.h>
#include <red/red_ds_api.h>
#include <red/red_s3_api.h>
#include <red/red_fs_api.h>
#include <red/red_dhash_api.h>
#include <red/red_queue_api.h>
#include <red/red_sql_api.h>
#include <red/red_status.h>
