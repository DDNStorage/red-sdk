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
 *   Name:       eventfd.cpp
 *   Project:    Red SDK examples common library
 *
 *   Description: Event file descriptor wrapper class for asynchronous operations
 *
 ******************************************************************************/

#include <errno.h>
#include <cerrno>
#include <sys/eventfd.h>
#include <unistd.h>
#include <cassert>

#include "../include/eventfd.hpp"

namespace common
{

eventfd_t::eventfd_t()
: event_fd(-1)
{
    /* Create a pipe which can be used wake us up on local events */
    event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    assert(event_fd >= 0);
}

eventfd_t::~eventfd_t()
{
    if (event_fd >= 0)
    {
        close(event_fd);
        event_fd = -1;
    }
}

int eventfd_t::get_fd() const
{
    return event_fd;
}

void eventfd_t::kick()
{
    if (event_fd >= 0)
    {
        int rc = eventfd_write(event_fd, 1);
        assert(rc >= 0 || errno == EAGAIN);
    }
}

bool eventfd_t::read()
{
    if (event_fd < 0)
    {
        return false;
    }

    uint64_t u;
    int      rc = eventfd_read(event_fd, &u);
    if (rc < 0)
    {
        if (errno == EAGAIN || errno == EINTR)
        {
            return false;
        }
        assert(rc);
    }
    return true;
}

} // namespace common
