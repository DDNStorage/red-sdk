Architecture Guide
================

The RED SDK architecture follows a POSIX-like design with enhancements for distributed systems and asynchronous operations.

Client Initialization
------------------

Before using the RED SDK, clients must initialize the library using ``red_client_lib_init_v3()``. This function is synchronous and must be called once per client process. When finished, clean up with:

.. code-block:: c

    void red_client_lib_fini(void);

This will close all open dataset handles and release resources.

File Handles
----------

The RED SDK uses two types of handles:

1. ``rfs_open_hndl_t``
   * Used for most RED SDK APIs
   * Contains dataset information
   * Core-affined for thread safety
   * Maintains current file offset

2. ``rfs_dataset_hndl_t``
   * Used for dataset management
   * Opaque type from client perspective

Important Thread Safety Notes:

* All ``rfs_open_hndl_t`` handles (except root) are core-affined
* Handles cannot be passed between threads
* Root handle (from ``red_open_root()``) can be shared

Client Callbacks
-------------

All asynchronous operations use the ``rfs_usercb_t`` structure:

.. code-block:: c

    typedef void (*rfs_two_arg_cbp_t)(red_status_t rs, void *ucbarg);

    typedef struct rfs_usercb {
        rfs_two_arg_cbp_t  ucb_fun;
        const void        *ucb_arg;
    } rfs_usercb_t;

Key points:
* Callback receives operation status and user argument
* Used to tie requests to responses
* Executed in completion thread context

Polling Mechanism
-------------------

The RED SDK supports two completion handling strategies:

* **Automatic Polling**: Dedicated thread handles all completions
* **Manual Polling**: Application code must poll for completions

See :doc:`Completion Polling <../api/completions>` for details.

Permission Model
-------------

Two permission models are supported:

1. **Simplified Model**
   * Uses ``geteuid()`` and ``getegid()``
   * Handled automatically by API

2. **Advanced Model**
   * Uses ``red_new_access_req()``
   * Custom permission types:
     * ``API_PROVIDED_ACCESS_INFO``
     * ``LINUX_ACCESS_INFO``
     * ``S3_ACCESS_INFO``
     * ``NFS_ACCESS_INFO``

See Also
--------

* :doc:`Threading Guide <threading>`
* :doc:`Client API Reference <../api/client>`
* :doc:`Example: Basic Usage <../examples/hello_world>`
