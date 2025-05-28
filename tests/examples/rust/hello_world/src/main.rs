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
 *   Name:       main.rs
 *   Project:    RED
 *
 *   Description: A sample program that creates a bucket and an object named
 *                HelloWorld.
 *
 *   Created:    3/26/2025
 *   Author(s):  Noel Otterness (notterness@ddn.com)
 *               Dana Helwig (dhelwig@ddn.com)
 *
 ******************************************************************************/

use clap::Parser;
use libc::XATTR_CREATE;
use red::*;
use std::{env, ffi::CString, fmt, process};

const TIMEOUT: u32 = 30;
const RFS_NUM_DEF_BUFFERS: u32 = 64;
const RSMT_MAX_NAME: usize = 256;

// Environment variables
const RED_CLUSTER_ENV: &str = "RED_CLUSTER";
const RED_TENANT_ENV: &str = "RED_TENANT";
const RED_USER_ENV: &str = "RED_USER";

struct Config {
    cluster: String,
    ten_subten: String,
    bucket_name: Option<String>,
    user_id: Option<String>,
    dp_profile_id: u32,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            cluster: String::from("infinia"),
            ten_subten: String::from("red/red"),
            bucket_name: None,
            user_id: None,
            dp_profile_id: RED_DS_DEFAULT_DP_PROFILE,
        }
    }
}

#[derive(Parser, Debug)]
#[command(author, version, about = "Hello World example for RED SDK", long_about = None)]
struct Args {
    /// Cluster name
    #[arg(short = 'c', long)]
    cluster: Option<String>,

    /// Tenant name
    #[arg(short = 'N', long)]
    tenant: Option<String>,

    /// Subtenant name
    #[arg(short = 'n', long)]
    subtenant: Option<String>,

    /// Bucket name (required)
    #[arg(short = 'B', long)]
    bucket: String,

    /// User ID (required)
    #[arg(short = 'I', long)]
    id: String,
}

fn parse_args() -> Result<Config, String> {
    let args = Args::parse();

    let mut config = Config::default();

    if let Some(cluster) = args.cluster {
        config.cluster = cluster;
    } else {
        if let Ok(cluster) = env::var(RED_CLUSTER_ENV) {
            config.cluster = cluster;
            println!("LOADING {} as cluster", config.cluster);
        }
    }

    config.bucket_name = Some(args.bucket);
    config.user_id = Some(args.id);

    // Handle tenant/subtenant
    if let Some(tenant) = args.tenant {
        let parts: Vec<&str> = config.ten_subten.split('/').collect();
        config.ten_subten = format!("{}/{}", tenant, parts[1]);
    }
    if let Some(subtenant) = args.subtenant {
        let parts: Vec<&str> = config.ten_subten.split('/').collect();
        config.ten_subten = format!("{}/{}", parts[0], subtenant);
    }

    if config.ten_subten.is_empty() {
        if let Ok(tenant) = env::var(RED_TENANT_ENV) {
            config.ten_subten = tenant;
            println!("LOADING {} as ten_subten", config.ten_subten);
        }
    }

    // Validate RED_USER_ENV
    if env::var(RED_USER_ENV).is_err() {
        return Err(format!("Please specify correct {} env!", RED_USER_ENV));
    }

    // Validate name lengths
    if config.cluster.len() >= RSMT_MAX_NAME {
        return Err(format!("Cluster name too long (max {} chars)", RSMT_MAX_NAME - 1));
    }

    let ten_parts: Vec<&str> = config.ten_subten.split('/').collect();
    if ten_parts.len() != 2 {
        return Err("Tenant/subtenant must be in format 'tenant/subtenant'".to_string());
    }
    if ten_parts[0].len() >= RSMT_MAX_NAME || ten_parts[1].len() >= RSMT_MAX_NAME {
        return Err(format!("Tenant or subtenant name too long (max {} chars)", RSMT_MAX_NAME - 1));
    }

    // Set environment variables
    env::set_var(RED_CLUSTER_ENV, &config.cluster);
    env::set_var(RED_TENANT_ENV, &config.ten_subten);

    Ok(config)
}

impl From<red_status_t> for RedStatus {
    fn from(status: red_status_t) -> Self {
        RedStatus(status)
    }
}

impl From<i32> for RedStatus {
    fn from(status: i32) -> Self {
        // SAFETY: This is safe because we're converting from the underlying type
        unsafe { RedStatus(std::mem::transmute(status)) }
    }
}

#[derive(Copy, Clone)]
struct RedStatus(red_status_t);

impl fmt::Display for RedStatus {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let code = self.0 as i32;
        unsafe {
            let err_str = std::ffi::CStr::from_ptr(red_strerror(self.0)).to_string_lossy();
            write!(f, "{}({})", err_str, code)
        }
    }
}

impl PartialEq<red_status_t> for RedStatus {
    fn eq(&self, other: &red_status_t) -> bool {
        self.0 == *other
    }
}

#[repr(C, packed)]
union ObjHash {
    csum: u64,
    hash: [u8; 32], // SHA256_HASH_LEN
}

fn example_write(
    obj_oh: rfs_open_hndl_t,
    data: &[u8],
    chksum_type: red_s3_checksum_type_e,
    user: &mut red_api_user_t,
) -> (RedStatus, String) {
    let mut write_ret_size: isize = 0;
    let rs = RedStatus::from(common::pwrite(
        obj_oh,
        data.as_ptr() as *mut _,
        data.len(),
        0,
        &mut write_ret_size,
        user,
    ));

    if rs != red_status_t::RED_SUCCESS || write_ret_size as usize != data.len() {
        println!(
            "red_pwrite() failed! ret_size={} expected_size={} rc={}",
            write_ret_size,
            data.len(),
            rs
        );
        if rs == red_status_t::RED_SUCCESS {
            return (RedStatus::from(red_status_t::RED_EINVAL), String::new());
        }
        return (rs, String::new());
    }

    let mut hash = ObjHash { hash: [0; 32] };

    match chksum_type {
        red_s3_checksum_type_e::RED_S3CS_SHA256 => {
            assert_eq!(
                unsafe {
                    red_dhash_data(
                        red_dhash_e::RED_DHASH_SHA256,
                        data.as_ptr() as *const _,
                        data.len(),
                        &mut hash as *mut _ as *mut _,
                    )
                },
                0
            );
        }
        _ => {}
    }

    let mut md5_hash = [0u8; 16];
    let rc = unsafe {
        red_dhash_data(
            red_dhash_e::RED_DHASH_MD5,
            data.as_ptr() as *const _,
            data.len(),
            &mut md5_hash as *mut _ as *mut _,
        )
    };
    if rc != 0 {
        println!("red_dhash_data failed rc={}", rc);
        return (RedStatus::from(red_status_t::RED_EINVAL), String::new());
    }

    let mut etag = [0u8; 33]; // RED_S3_USER_ETAG_SIZE
    unsafe {
        red_bin_to_hex(
            md5_hash.as_ptr(),
            md5_hash.len().try_into().unwrap(),
            etag.as_mut_ptr() as *mut u8,
        );
    }
    etag[32] = 0;

    // Convert etag to a String for returning
    let etag_str = String::from_utf8_lossy(&etag[..32]).to_string();
    println!("Calculated MD5 etag={}", etag_str);

    // The RED Client code will internally set the user.s3_etag when common::s3_publish is called
    (RedStatus::from(red_status_t::RED_SUCCESS), etag_str)
}

fn example_set_acl(oh: rfs_open_hndl_t, user: &mut red_api_user_t) -> RedStatus {
    let user_id = match env::var(RED_USER_ENV) {
        Ok(id) => id,
        Err(_) => return RedStatus::from(red_status_t::RED_EINVAL),
    };

    let acl_str = format!(
        "{{\"Owner\":{{\"ID\":\"{}\",\"DisplayName\":\"admin\"}},\
        \"Grants\":[{{\"Grantee\":{{\"Type\":\"CanonicalUser\",\
        \"ID\":\"{}\",\"DisplayName\":\"admin\"}},\"Permission\":\
        \"FULL_CONTROL\"}}]}}",
        user_id, user_id
    );

    let acl_cstr = match CString::new(acl_str) {
        Ok(s) => s,
        Err(_) => return RedStatus::from(red_status_t::RED_EINVAL),
    };

    RedStatus::from(common::fsetxattr(
        oh,
        b"user.s3_acl\0".as_ptr() as *const u8,
        acl_cstr.as_ptr() as *const std::ffi::c_void,
        acl_cstr.as_bytes().len(),
        XATTR_CREATE as i32,
        user,
    ))
}

fn example_open_bucket(
    bucket_name: &str,
    cluster: &str,
    tenant: &str,
    subtenant: &str,
    dp_profile_id: u32,
    user: &mut red_api_user_t,
) -> Result<(rfs_dataset_hndl_t, rfs_open_hndl_t), RedStatus> {

    let mut bucket_hndl = RED_INVALID_HANDLE!(rfs_dataset_hndl_t);
    let mut root_oh = RED_INVALID_OPEN_HANDLE!(rfs_open_hndl_t);

    unsafe {
        let mut bucket_props = std::mem::zeroed::<red_ds_props_t>();
        let rs = RedStatus::from(red_ds_get_default_props(&mut bucket_props, dp_profile_id));
        if rs != red_status_t::RED_SUCCESS {
            println!(
                "red_ds_get_default_props() failed! dp_id={}, rs={}",
                dp_profile_id, rs
            );
            return Err(rs);
        }

        bucket_props.nstripes = RED_MAX_STRIPES;
        bucket_props.bucket_size = 256 * 1024;
        bucket_props.block_size = 4 * 1024;
        bucket_props.ec_nparity = 2;

        let bucket_name = CString::new(bucket_name).unwrap();
        let cluster = CString::new(cluster).unwrap();
        let tenant = CString::new(tenant).unwrap();
        let subtenant = CString::new(subtenant).unwrap();

        let rs = RedStatus::from(common::s3_create_bucket(
            bucket_name.as_ptr(),
            cluster.as_ptr() as *mut _,
            tenant.as_ptr() as *mut _,
            subtenant.as_ptr() as *mut _,
            &mut bucket_props,
            &mut bucket_hndl,
            user,
        ));
        if rs != red_status_t::RED_SUCCESS && rs != red_status_t::RED_EEXIST {
            println!("red_s3_create_bucket() failed! rs={}", rs);
            return Err(rs);
        }

        let rs = RedStatus::from(common::open_root(
            bucket_hndl,
            &mut root_oh,
            user,
        ));
        if rs != red_status_t::RED_SUCCESS {
            println!("red_open_root() failed! rs={}", rs);
            return Err(rs);
        }

        let rs = RedStatus::from(example_set_acl(root_oh, user));
        if rs != red_status_t::RED_SUCCESS {
            println!("example_set_acl(root) failed! rs={}", rs);
            return Err(rs);
        }

        Ok((bucket_hndl, root_oh))
    }
}

fn example_validate_etag(
    obj_oh: rfs_open_hndl_t,
    expected_etag: &str,
    user: &mut red_api_user_t,
) -> RedStatus {
    let mut actual_etag = [0u8; 33]; // RED_S3_USER_ETAG_SIZE
    let mut xattr_size: usize = 32; // RED_S3_USER_ETAG_SIZE - 1

    // Get the user.s3_etag extended attribute
    let rs = RedStatus::from(common::fgetxattr(
        obj_oh,
        b"user.s3_etag\0".as_ptr() as *const u8,
        actual_etag.as_mut_ptr() as *mut std::ffi::c_void,
        xattr_size,
        &mut xattr_size,
        user,
    ));

    // Log the result
    println!("red_fgetxattr result rs={}, xattr_size={}", rs, xattr_size);

    if rs != red_status_t::RED_SUCCESS {
        println!("Failed to get user.s3_etag, rs={}", rs);
        return rs;
    }

    // Ensure null termination for string comparison
    actual_etag[xattr_size] = 0;

    // Convert to string for comparison
    let actual_etag_str = String::from_utf8_lossy(&actual_etag[..xattr_size]).to_string();

    // Compare the expected and actual etags
    if expected_etag != actual_etag_str {
        println!(
            "Etag mismatch! Expected={}, Actual={}",
            expected_etag,
            actual_etag_str
        );
        return RedStatus::from(red_status_t::RED_FAILURE);
    }

    println!(
        "Etag validation successful. Expected etag={}, Actual etag={}",
        expected_etag,
        actual_etag_str
    );

    RedStatus::from(red_status_t::RED_SUCCESS)
}

fn example_create_object(root_oh: rfs_open_hndl_t, user: &mut red_api_user_t) -> RedStatus {
    let data = b"Hello World";
    let obj_name = CString::new("HelloWorld").unwrap();

    let mut obj_oh = RED_INVALID_OPEN_HANDLE!(rfs_open_hndl_t);
    let mut version: u64 = 0;

    // Create version
    let rs = RedStatus::from(common::s3_create_version(
        root_oh,
        obj_name.as_ptr(),
        0,
        &mut obj_oh,
        user,
    ));
    if rs != red_status_t::RED_SUCCESS {
        println!("red_s3_create_version() failed! rs={}", rs);
        return rs;
    }

    // Write data and get the calculated MD5 hash
    let (rs, md5_etag) = example_write(obj_oh, data, red_s3_checksum_type_e::RED_S3CS_SHA256, user);
    if rs != red_status_t::RED_SUCCESS {
        return rs;
    }

    // Set ACL
    let rs = RedStatus::from(example_set_acl(obj_oh, user));
    if rs != red_status_t::RED_SUCCESS {
        println!("red_set_acl(object) failed! rs={}", rs);
        common::close(obj_oh, user);
        return rs;
    }

    // Publish - this will internally calculate and set the user.s3_etag
    let rs = RedStatus::from(common::s3_publish(
        obj_oh,
        &mut version,
        user,
    ));
    if rs != red_status_t::RED_SUCCESS {
        println!("red_s3_publish() failed! rs={}", rs);
        return rs;
    }

    // Close handle to ensure the extended attributes are written
    let close_rs = RedStatus::from(common::close(obj_oh, user));
    if close_rs != red_status_t::RED_SUCCESS {
        println!("red_close() failed! rs={}", close_rs);
        if rs == red_status_t::RED_SUCCESS {
            return close_rs;
        }
    }
    obj_oh = RED_INVALID_OPEN_HANDLE!(rfs_open_hndl_t);

    // Reopen the object to validate the etag
    let obj_name = CString::new("HelloWorld").unwrap();
    let rs = RedStatus::from(common::s3_open(
        root_oh,
        obj_name.as_ptr(),
        0,
        0,
        &mut obj_oh,
        &mut version,
        user,
    ));
    if rs != red_status_t::RED_SUCCESS {
        println!("red_s3_open() failed! rs={}", rs);
        return rs;
    }

    // Validate that the etag matches our calculated MD5
    let rs = example_validate_etag(obj_oh, &md5_etag, user);
    if rs != red_status_t::RED_SUCCESS {
        println!("example_validate_etag() failed! rs={}", rs);
        return rs;
    }

    // Close handle again
    let close_rs = RedStatus::from(common::close(obj_oh, user));
    if close_rs != red_status_t::RED_SUCCESS {
        println!("red_close() failed! rs={}", close_rs);
        if rs == red_status_t::RED_SUCCESS {
            return close_rs;
        }
    }

    rs
}

fn main() {
    let config = match parse_args() {
        Ok(cfg) => cfg,
        Err(e) => {
            eprintln!("Error: {}", e);
            process::exit(1);
        }
    };

    let opts = red_client_lib_init_opts {
        num_sthreads: 1,
        coremask: std::ptr::null_mut(),
        num_buffers: RFS_NUM_DEF_BUFFERS,
        num_ring_entries: 64,
        poller_thread: true,
    };

    unsafe {
        let rc = red_client_lib_init_v3(&opts);
        if rc != 0 {
            eprintln!("Error: red_client_lib_init failed with rs={}", rc);
            process::exit(1);
        }
        println!("Client library initialized successfully");

        if !red_client_is_ready(TIMEOUT) {
            eprintln!("Error: red_client_is_ready failed");
            red_client_lib_fini();
            process::exit(1);
        }
        println!("Client is ready");

        let ten_parts: Vec<&str> = config.ten_subten.split('/').collect();
        let tenname = CString::new(ten_parts[0]).unwrap();
        let subname = CString::new(ten_parts[1]).unwrap();
        let cluster = CString::new(config.cluster.clone()).unwrap();

        let mut user = red_api_user_t {
            rfs_tenname: tenname.as_ptr() as *mut _,
            rfs_subname: subname.as_ptr() as *mut _,
            rfs_cluster: std::ptr::null_mut(),
            ..std::mem::zeroed()
        };

        println!("Using tenant={} subtenant={}", ten_parts[0], ten_parts[1]);

        let rc = RedStatus::from(red_establish_session(
            cluster.as_ptr(),
            tenname.as_ptr(),
            subname.as_ptr(),
            libc::geteuid().into(),
            libc::getegid().into(),
            &mut user,
        ));
        if rc != red_status_t::RED_SUCCESS {
            eprintln!(
                "ERROR: Could not establish session for ten/subten={}/{}",
                ten_parts[0], ten_parts[1]
            );
            red_client_lib_fini();
            process::exit(1);
        }
        println!(
            "Session established successfully with cluster={}",
            config.cluster
        );

        let (bucket_hndl, root_oh) = match example_open_bucket(
            &config.bucket_name.as_ref().unwrap(),
            &config.cluster,
            ten_parts[0],
            ten_parts[1],
            config.dp_profile_id,
            &mut user,
        ) {
            Ok((hndl, oh)) => (hndl, oh),
            Err(rs) => {
                eprintln!("Failed to open bucket: {}", rs);
                red_client_lib_fini();
                process::exit(1);
            }
        };

        println!("Bucket {} opened successfully", config.bucket_name.as_ref().unwrap());

        let rs = RedStatus::from(example_create_object(root_oh, &mut user));
        if rs != red_status_t::RED_SUCCESS {
            println!("Unable to create object");
        } else {
            println!("Object HelloWorld created and written successfully");
        }

        println!("Starting cleanup sequence");

        if RED_IS_VALID_OPEN_HANDLE!(root_oh) {
            println!("Closing root handle");
            common::close(root_oh, &mut user);
        }

        if RED_IS_VALID_HANDLE!(bucket_hndl) {
            println!("Closing bucket handle");
            common::close_dataset(bucket_hndl, &mut user);
        }

        println!("Cleaning up library resources");
        red_client_lib_fini();

        println!("Program finished with status {}", rs);
        process::exit(if rs != red_status_t::RED_SUCCESS {
            1
        } else {
            0
        });
    }
}
