Permission Models
================

The RED SDK provides flexible permission models to support both traditional POSIX-style permissions and cloud-native access controls.

Access Control Types
-----------------

The SDK supports multiple access control types through ``rfs_access_info_e``:

.. code-block:: c

    typedef enum {
        API_PROVIDED_ACCESS_INFO,
        LINUX_ACCESS_INFO,
        S3_ACCESS_INFO,
        NFS_ACCESS_INFO
    } rfs_access_info_e;

User Context
----------

All operations require a user context (``red_api_user_t``):

.. code-block:: c

    typedef struct {
        uint64_t rfs_uid;      // Linux user ID
        uint64_t rfs_gid;      // Linux group ID
        uint32_t rfs_project;  // Project ID
        uint32_t rfs_reserved;
        char *rfs_tenname;     // Tenant name
        char *rfs_subname;     // Sub-tenant name
    } red_api_user_t;

Linux-style Permissions
--------------------

For traditional POSIX permissions:

.. code-block:: c

    // Get current user/group IDs
    red_api_user_t api_user = {};
    api_user.rfs_uid = (2UL << 32) | geteuid();  // POSIX principal format
    api_user.rfs_gid = (2UL << 32) | getegid();  // POSIX principal format

    // Use in API calls
    red_openat(dir_oh, path, flags, mode, &oh, &ucb, &api_user);

S3-style ACLs
-----------

For S3 bucket and object access:

.. code-block:: c

    // Create S3 access context
    red_api_user_t api_user = {};
    api_user.rfs_uid = s3_canonical_user_id;  // S3 user ID
    api_user.rfs_tenname = "my-tenant";       // Optional tenant
    
    // Use with S3 operations
    red_s3_put(bucket, key, &params, &data, &oh, &ucb, &api_user);

Multi-tenancy
-----------

Support for multi-tenant environments:

1. **Tenant Isolation**
   * Each tenant gets isolated namespace
   * Sub-tenants for hierarchical organization
   * Resource quotas per tenant

2. **Cross-tenant Access**
   * Explicit sharing between tenants
   * ACLs for fine-grained control
   * Audit logging of cross-tenant operations

Project Quotas
------------

Resource management by project:

.. code-block:: c

    // Set project context
    red_api_user_t api_user = {};
    api_user.rfs_project = project_id;
    
    // Project quotas apply to operations
    red_write(fh, buf, size, &written, &ucb, &api_user);

Advanced Access Control
--------------------

Custom access control using ``red_fs_user_t``:

.. code-block:: c

    // Create custom access context
    red_fs_user_t user;
    red_new_access_req(&user, API_PROVIDED_ACCESS_INFO);
    
    // Use custom access control
    red_openat(dir_oh, path, flags, mode, &oh, &ucb, &user);
    
    // Cleanup
    red_delete_access_req(&user);

Best Practices
------------

1. **Permission Handling**
   * Always provide appropriate user context
   * Check operation results for permission errors
   * Handle permission denied gracefully

2. **Security**
   * Don't share user contexts between operations
   * Validate user input for paths and permissions
   * Use least privilege principle

3. **Multi-tenant Setup**
   * Plan tenant hierarchy carefully
   * Set appropriate quotas
   * Monitor resource usage

Error Handling
------------

Common permission-related errors:

* ``EACCES`` - Permission denied
* ``EPERM`` - Operation not permitted
* ``EDQUOT`` - Quota exceeded
* ``ENOENT`` - No such file/directory

Example: Permission Check
----------------------

.. code-block:: c

    int check_access(const char *path, red_api_user_t *user) {
        rfs_open_hndl_t oh;
        rfs_usercb_t ucb = {};
        
        int ret = red_openat(dir_oh, path, O_RDONLY, 0, &oh, &ucb, user);
        if (ret == EACCES) {
            // Handle permission denied
            return -1;
        }
        
        // Cleanup
        red_close(oh, &ucb, user);
        return 0;
    }

See Also
--------

* :doc:`Architecture Guide <architecture>`
* :doc:`S3 Operations <../examples/s3_ops>`
* :doc:`Multi-tenant Setup <../examples/multi_tenant>`
