Getting Started with RED SDK
============================

The RED SDK provides a file system and AWS-compatible S3 object store within Linux user space. This guide will help you understand the core concepts and get started with basic operations.

Prerequisites
-------------

Before you begin, ensure you have:

1. **System Requirements**

   * Linux-based operating system
   * C++ compiler with C++11 support
   * Rust >= 1.8 (*Only if you want to build Rust example*)
   * GNU Make >= 4.3
   * Git (for version control)

2. **Environment Setup**

   * Set ``RED_CLUSTER`` environment variable (e.g., "infinia")
   * Set ``RED_TENANT`` environment variable (e.g., "red/red")
   * Set ``RED_USER`` environment variable

Core Concepts
-------------

1. **Asynchronous Operations**

   * All APIs are asynchronous by design
   * Operations return immediately
   * Results delivered via callbacks
   * Non-blocking I/O model

2. **File Handles**

   * Uses ``rfs_open_hndl_t`` instead of file descriptors
   * Core-affined for thread safety
   * Two types: regular and dataset handles

3. **Callback Mechanism**

   * Uses ``rfs_usercb_t`` for completion notifications
   * Supports both blocking and non-blocking operations
   * Preserves callback context

Quick Start Example
-------------------

The SDK includes a complete "hello world" example that demonstrates creating a bucket and writing an object.
These examples are provided in 3 languages:

    * c -  ``examples/c/hello_world/``
    * cpp - ``examples/cpp/hello_world/``
    * Rust - ``examples/rust/hello_world/``

.. code-block:: cpp

    #include <red_client_ring.h>
    #include <rfs_client_types.h>
    #include <rfs_client_api.h>
    #include <rfs_dhash_api.h>

    int main(int argc, char **argv) {
        // Initialize client library
        int ret = red_client_init();
        if (ret != 0) {
            return ret;
        }

        // Parse command line arguments for bucket name, etc.
        // (See -h option for details)
        ret = parse_args(argc, argv);
        if (ret != 0) {
            red_client_fini();
            return ret;
        }

        // Setup user context
        red_api_user_t user = {};
        user.rfs_uid = geteuid();
        user.rfs_gid = getegid();
        user.rfs_tenname = getenv("RED_TENANT");

        // Create bucket and write object
        rfs_dataset_hndl_t bucket_hndl;
        rfs_open_hndl_t root_oh;
        ret = rfs_open_bucket(p_bucket_name, p_cluster, p_tenant,
                            p_subtenant, &bucket_hndl, &root_oh, &user);
        if (ret != 0) {
            red_client_fini();
            return ret;
        }

        // Write object
        ret = rfs_create_object(root_oh, &user);
        if (ret != 0) {
            red_close(root_oh, nullptr, &user);
            red_client_fini();
            return ret;
        }

        // Cleanup
        red_close(root_oh, nullptr, &user);
        red_client_fini();
        return 0;
    }

Building and Running cpp "hello world" example
----------------------------------------------

1. **Build the cpp "hello world" example**

.. code-block:: bash

    $ cd examples/cpp/hello_world
    $ make

   This will build the cpp "hello world" example.

2. **Set Environment Variables**

.. code-block:: bash

    $ export RED_CLUSTER=infinia
    $ export RED_TENANT=red/red
    $ export RED_USER=your_username

3. **Run the Hello World Example**

.. code-block:: bash

    $ cd examples/cpp/hello_world
    $ ./hello_world -B my-bucket -I your-user-id

   The program accepts several command line options:
   * ``-B, --bucket``: Bucket name (required)
   * ``-I, --id``: User ID for ACL setup (required)
   * ``-c, --cluster``: Cluster name (defaults to RED_CLUSTER env var)
   * ``-N, --tenant``: Tenant name (defaults to RED_TENANT env var)
   * ``-n, --subtenant``: Subtenant name
   * ``-h, --help``: Show help message

Building and Running rust "hello world" example
-----------------------------------------------
Make sure you have installed the rust toolchain.

0. **First, build the rust bindings for c**

.. code-block:: bash

    $ cd rust/red
    $ cargo build

1. **Build the rust "hello world" example**

.. code-block:: bash

    $ cd examples/rust/
    $ cargo build

   This will build the rust "hello world" example.

2. **Set Environment Variables**

.. code-block:: bash

    $ export RED_CLUSTER=infinia
    $ export RED_TENANT=red/red
    $ export RED_USER=your_username

3. **Run the Hello World Example**

.. code-block:: bash

    $ cd examples/rust
    $ cargo run -p hello-world --  --bucket my-bucket --id my-user-id


Key Features
------------

1. **File System Operations**

   * POSIX-like file and directory operations
   * Extended attributes support
   * Asynchronous I/O

2. **S3 Compatibility**

   * Bucket operations
   * Object storage
   * ACL support

3. **Performance Features**

   * Core-affined handles
   * Non-blocking operations
   * Efficient ring buffer management

Next Steps
----------

1. **Explore More Examples**

   * Check out the examples in ``sdk/examples/cpp/``
   * Try the S3 operations example
   * Experiment with asynchronous operations

2. **Learn Advanced Topics**

   * `Asynchronous Operations Guide <async_operations>`_
   * `Threading Model <threading>`_
   * `Error Handling <error_handling>`_

3. **API Reference**

   * `Client API <../api/client>`_
   * `Filesystem API <../api/filesystem>`_
   * `SQL API <../api/sql>`_
   * `Queue API <../api/queue>`_

Common Issues
-------------

1. **Handle Affinity**

   * Ensure handles are used only by the thread that created them
   * Root handle (from ``red_open_root()``) is the only exception

2. **Environment Setup**

   * Verify all required environment variables are set
   * Check cluster and tenant configurations

3. **Error Handling**

   * Always check return values
   * Implement proper cleanup in error paths
   * Use appropriate error handling patterns
