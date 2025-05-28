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
 *   Name:       eventfd.hpp
 *   Project:    Red SDK examples common library
 *
 *   Description: Event file descriptor wrapper class for asynchronous operations
 *
 ******************************************************************************/
#ifndef COMMON_EVENTFD_HPP_
#define COMMON_EVENTFD_HPP_

namespace common
{

/**
 * @brief Object that waits for an OP to complete
 */
class eventfd_t
{
public:
    eventfd_t();
    ~eventfd_t();

    int  get_fd() const;
    void kick();
    bool read();

private:
    int event_fd;
};

} // namespace common

#endif // COMMON_EVENTFD_HPP_
