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
 *   Name:       sync_api.rs
 *   Project:    RED
 *
 *   Description: Synchronous API wrapper class for asynchronous RED operation
 *
 ******************************************************************************/

use crate::eventfd::EventFd;
use nix::poll::{poll, PollFd, PollFlags};
use red::*;

pub struct SyncApi {
    rs: red_status_t,
    ucb: rfs_usercb_t,
    eventfd: EventFd
}

impl SyncApi {
    pub fn new() -> std::io::Result<Box<Self>> {
        let eventfd = EventFd::new()?;
        let ucb = rfs_usercb_t {
            ucb_fun: Some(
                Self::callback as unsafe extern "C" fn(red_status_t, *mut std::ffi::c_void),
            ),
            ucb_arg: std::ptr::null_mut(),
            ucb_e2e: 0,
        };

        let mut sync_api = Box::new(SyncApi {
            rs: red_status_t::RED_SUCCESS,
            ucb,
            eventfd,
        });

        // Set ucb_arg to point to self
        sync_api.ucb.ucb_arg = &*sync_api as *const _ as *mut std::ffi::c_void;

        Ok(sync_api)
    }

    pub fn get_ucb(&mut self) -> &mut rfs_usercb_t {
        &mut self.ucb
    }

    extern "C" fn callback(status: red_status_t, arg: *mut std::ffi::c_void) {
        let sync_api = unsafe { &mut *(arg as *mut SyncApi) };
        sync_api.done(status);
    }

    fn done(&mut self, status: red_status_t) {
        self.rs = status;
        let _ = self.eventfd.kick();
    }

    pub fn wait(&mut self, rc: red_status_t) -> red_status_t {
        if rc != red_status_t::RED_SUCCESS {
            self.done(rc);
            return self.rs;
        }

        let mut fds = [
            PollFd::new(self.eventfd.get_fd(), PollFlags::POLLIN),
            PollFd::new(unsafe { red_client_lib_poll_fd() }, PollFlags::POLLIN),
        ];

        loop {
            match poll(&mut fds, -1) {
                Ok(rc) => {
                    if rc < 0 {
                        if nix::errno::errno() == libc::EINTR {
                            continue;
                        }
                        return red_status_t::RED_EINVAL;
                    }

                    if rc == 0 {
                        continue;
                    }

                    if fds[0].revents().unwrap().contains(PollFlags::POLLIN) {
                        // eventfd kicked in done()
                        if !self.eventfd.read().unwrap_or(false) {
                            continue;
                        }
                        break;
                    }

                    if fds[1].revents().unwrap().contains(PollFlags::POLLIN) {
                        let mut ucp = rfs_usercomp_t {
                            ucp_fun: None,
                            ucp_arg: std::ptr::null_mut(),
                            ucp_res: red_status_t::RED_SUCCESS,
                        };

                        let rc = unsafe { red_client_lib_poll(&mut ucp, 1) };
                        if rc == 1 && ucp.ucp_fun.is_some() {
                            unsafe {
                                (ucp.ucp_fun.unwrap())(ucp.ucp_res, ucp.ucp_arg);
                            }
                        }
                    }
                }
                Err(_) => return red_status_t::RED_EINVAL,
            }
        }

        self.rs
    }
}

// Safety: SyncApi can be sent between threads
unsafe impl Send for SyncApi {}

pub fn s3_create_version(
    dir_oh: rfs_open_hndl_t,
    tgt_name: *const std::os::raw::c_char,
    flags: i32,
    created_oh: *mut rfs_open_hndl_t,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_s3_create_version(
            dir_oh,
            tgt_name,
            flags,
            created_oh,
            sync.get_ucb(),
            user,
        ))
    };
    sync.wait(rc)
}

pub fn s3_publish(
    oh: rfs_open_hndl_t,
    version: *mut u64,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe { std::mem::transmute(red::red_s3_publish(oh, version, sync.get_ucb(), user)) };
    sync.wait(rc)
}

pub fn s3_open(
    dir_oh: rfs_open_hndl_t,
    s3_key: *const std::os::raw::c_char,
    version: u64,
    flags: i32,
    oh: *mut rfs_open_hndl_t,
    out_version: *mut u64,
    api_user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_s3_open(
            dir_oh,
            s3_key,
            version,
            flags,
            oh,
            out_version,
            sync.get_ucb(),
            api_user,
        ))
    };
    sync.wait(rc)
}

pub fn pwrite(
    oh: rfs_open_hndl_t,
    buff: *mut std::ffi::c_void,
    size: usize,
    off: off_t,
    ret_size: *mut isize,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_pwrite(
            oh,
            buff,
            size,
            off,
            ret_size,
            sync.get_ucb(),
            user,
        ))
    };
    sync.wait(rc)
}

pub fn close(oh: rfs_open_hndl_t, user: *mut red_api_user_t) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe { std::mem::transmute(red::red_close(oh, sync.get_ucb(), user)) };
    sync.wait(rc)
}

pub fn close_dataset(ds_hndl: rfs_dataset_hndl_t, user: *mut red_api_user_t) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe { std::mem::transmute(red::red_close_dataset(ds_hndl, sync.get_ucb(), user)) };
    sync.wait(rc)
}

pub fn s3_create_bucket(
    bucket_name: *const std::os::raw::c_char,
    cluster: *const std::os::raw::c_char,
    tenant: *const std::os::raw::c_char,
    subtenant: *const std::os::raw::c_char,
    bucket_props: *mut red_ds_props_t,
    bucket_hndl: *mut rfs_dataset_hndl_t,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_s3_create_bucket(
            bucket_name,
            cluster,
            tenant,
            subtenant,
            bucket_props,
            bucket_hndl,
            sync.get_ucb(),
            user,
        ))
    };
    sync.wait(rc)
}

pub fn open_root(
    ds_hndl: rfs_dataset_hndl_t,
    root_oh: *mut rfs_open_hndl_t,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe { std::mem::transmute(red::red_open_root(ds_hndl, root_oh, sync.get_ucb(), user)) };
    sync.wait(rc)
}

pub fn fsetxattr(
    oh: rfs_open_hndl_t,
    name: *const std::os::raw::c_char,
    value: *const std::ffi::c_void,
    size: usize,
    flags: i32,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_fsetxattr(
            oh,
            name,
            value,
            size,
            flags,
            sync.get_ucb(),
            user,
        ))
    };
    sync.wait(rc)
}

pub fn fgetxattr(
    oh: rfs_open_hndl_t,
    name: *const std::os::raw::c_char,
    value: *mut std::ffi::c_void,
    size: usize,
    ret_size: *mut usize,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_fgetxattr(
            oh,
            name,
            value,
            size,
            ret_size,
            sync.get_ucb(),
            user,
        ))
    };
    sync.wait(rc)
}

pub fn obtain_dataset(
    ds_name: *const std::os::raw::c_char,
    cluster: *const std::os::raw::c_char,
    ds_props: *mut red_ds_props_t,
    ds_hndl: *mut rfs_dataset_hndl_t,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_obtain_dataset(
            ds_name,
            cluster,
            ds_props,
            ds_hndl,
            sync.get_ucb(),
            user,
        ))
    };
    sync.wait(rc)
}

pub fn openat(
    dir_oh: rfs_open_hndl_t,
    pathname: *const std::os::raw::c_char,
    flags: i32,
    mode: mode_t,
    oh: *mut rfs_open_hndl_t,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_openat(
            dir_oh,
            pathname,
            flags,
            mode,
            oh,
            sync.get_ucb(),
            user,
        ))
    };
    sync.wait(rc)
}

pub fn pread(
    oh: rfs_open_hndl_t,
    buff: *mut std::ffi::c_void,
    size: usize,
    off: off_t,
    ret_size: *mut isize,
    user: *mut red_api_user_t,
) -> red_status_t {
    let mut sync = SyncApi::new().unwrap();
    let rc = unsafe {
        std::mem::transmute(red::red_pread(
            oh,
            buff,
            size,
            off,
            ret_size,
            sync.get_ucb(),
            user,
        ))
    };
    sync.wait(rc)
}
