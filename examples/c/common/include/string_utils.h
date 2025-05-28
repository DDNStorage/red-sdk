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
 *   Name:       string_utils.h
 *   Project:    Red SDK examples common library
 *
 *   Description: Common string utilities
 *
 ******************************************************************************/
#ifndef COMMON_STRING_UTILS_H_
#define COMMON_STRING_UTILS_H_

/**
 * @brief Split apart tenant/subtenant combined string into two strings
 *
 * @param ten_subten Input string in format "tenant/subtenant"
 * @param[out] tenant Output tenant string
 * @param[out] subtenant Output subtenant string
 */
void common_split_tensubten(char const *ten_subten, char **tenant, char **subtenant);

#endif // COMMON_STRING_UTILS_H_
