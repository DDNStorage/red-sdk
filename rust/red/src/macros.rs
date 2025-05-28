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
 *   Name:       macros.rs
 *   Project:    RED
 *
 *   Description: Macros ported from C to Rust
 *
 ******************************************************************************/

#[macro_export]
macro_rules! RED_IS_VALID_OPEN_HANDLE {
    ($handle:expr) => {
        $handle.fd != 0
    };
}

#[macro_export]
macro_rules! RED_IS_VALID_HANDLE {
    ($handle:expr) => {
        $handle.hndl != std::ptr::null_mut()
    };
}

#[macro_export]
macro_rules! RED_INVALID_HANDLE {
    ($t:ty) => {{
        let mut handle: $t = unsafe { std::mem::zeroed() };
        handle.hndl = std::ptr::null_mut();
        handle
    }};
}

#[macro_export]
macro_rules! RED_INVALID_OPEN_HANDLE {
    ($t:ty) => {{
        let mut handle: $t = unsafe { std::mem::zeroed() };
        handle.fd = 0;
        handle
    }};
}
