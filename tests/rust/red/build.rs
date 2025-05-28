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
 *   Name:       build.rs
 *   Project:    RED
 *
 *   Description: Automatic bindings generation for RED C API
 *
 ******************************************************************************/
use std::path::PathBuf;
use bindgen::EnumVariation;

fn main() {

    // Only include RED pkgbuild path if RED environment variable is set
    if let Ok(red_path) = std::env::var("RED") {
        println!("cargo:rustc-link-search={}/pkgbuild/red_inst/lib", red_path);
    }

    // Tell cargo to look for shared libraries in the specified directory
    println!("cargo:rustc-link-search=/opt/ddn/red/lib");

    // Link against red_client library
    println!("cargo:rustc-link-lib=red_client");

    // Watch all files in the include directory
    let include_dir = PathBuf::from("../../c/include");
    if let Ok(entries) = std::fs::read_dir(include_dir.clone()) {
        for entry in entries.flatten() {
            if let Ok(file_type) = entry.file_type() {
                if file_type.is_file() {
                    if let Ok(path) = entry.path().canonicalize() {
                        println!("cargo:rerun-if-changed={}", path.display());
                    }
                }
            }
        }
    }

    // Generate bindings
    let bindings = bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg(format!("-I{}", include_dir.display()))
        .allowlist_function("(red_|rfs_).*|bin_to_hex|red_next_s3_partinfo|red_nfsdentry_name|red_nfsdentry_next|red_nfsdentry_reclen")
        .allowlist_type("(red_|rfs_).*|file_attr_fields_e|qfu_results_e|attrs_t|client_sthread_t|dlm_t|ds_cap_t|red_ds_capacity|ds_id_t|file_handle|flags_t|gm_cache_copy_t|json_filler_t|multipart_upload_entry_t|open_how|reactor_hndlr_t|red_sql_lib_exec_sqe_t|statvfs|uplink_args_t")
        .allowlist_var("(RED_|RFS_).*")
        .default_enum_style(EnumVariation::Rust {
            non_exhaustive: false,
        })
        .generate()
        .expect("Unable to generate bindings");

    // Write bindings to src directory
    bindings
        .write_to_file("src/bindings.rs")
        .expect("Couldn't write bindings!");
}
