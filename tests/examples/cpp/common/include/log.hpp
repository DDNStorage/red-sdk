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
 *   Name:       log.hpp
 *   Project:    Red SDK examples common library
 *
 *   Description: Common log functionality
 *
 ******************************************************************************/
#ifndef COMMON_LOG_HPP_
#define COMMON_LOG_HPP_

namespace common
{
void common_log(char const *file, const int line, const char *func, const char *fmt, ...);
} // namespace common

#define COMMON_LOG(fmt, ...)                                                             \
    do                                                                                   \
    {                                                                                    \
        common::common_log(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__);            \
    } while (0)

#endif // COMMON_LOG_HPP_
