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
 *   Name:       eventfd.rs
 *   Project:    Red SDK examples common library
 *
 *   Description: Event file descriptor wrapper class for asynchronous operations
 *
 ******************************************************************************/

use libc::{eventfd_read, eventfd_write};
use nix::sys::eventfd::{eventfd, EfdFlags};
use nix::unistd::close;
use std::io::{Error, Result};
use std::os::unix::io::{AsRawFd, RawFd};

pub struct EventFd {
    fd: RawFd,
}

impl EventFd {
    pub fn new() -> Result<Self> {
        let fd = eventfd(0, EfdFlags::EFD_CLOEXEC | EfdFlags::EFD_NONBLOCK)?;
        Ok(EventFd { fd })
    }

    pub fn get_fd(&self) -> RawFd {
        self.fd
    }

    pub fn kick(&self) -> Result<()> {
        if self.fd >= 0 {
            let rc = unsafe { eventfd_write(self.fd, 1) };
            if rc == 0 {
                Ok(())
            } else {
                match nix::errno::errno() {
                    libc::EAGAIN => Ok(()),
                    e => Err(Error::from_raw_os_error(e)),
                }
            }
        } else {
            Ok(())
        }
    }

    pub fn read(&self) -> Result<bool> {
        if self.fd < 0 {
            return Ok(false);
        }

        let rc = unsafe { eventfd_read(self.fd, &mut 0u64) };
        if rc == 0 {
            Ok(true)
        } else {
            match nix::errno::errno() {
                libc::EAGAIN | libc::EINTR => Ok(false),
                e => Err(Error::from_raw_os_error(e)),
            }
        }
    }
}

impl Drop for EventFd {
    fn drop(&mut self) {
        if self.fd >= 0 {
            let _ = close(self.fd);
        }
    }
}

impl AsRawFd for EventFd {
    fn as_raw_fd(&self) -> RawFd {
        self.fd
    }
}
