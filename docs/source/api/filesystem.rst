Filesystem API Reference
=====================

The RED SDK filesystem API provides POSIX-like operations with enhanced functionality for distributed systems. This API operates entirely in user space and offers asynchronous operations for optimal performance.

Key Concepts
-----------

* All operations are asynchronous
* Uses file handles (``rfs_open_hndl_t``) instead of file descriptors
* Core-affined handles for thread safety
* Enhanced operations for distributed systems

File Operations
-------------

File Access and Manipulation
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. c:function:: int red_openat(rfs_open_hndl_t dir_oh, const char *pathname, int flags, mode_t mode, rfs_open_hndl_t *oh, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Opens a file and returns a handle for further operations.

   :param dir_oh: Directory handle for relative paths
   :param pathname: Path to the file
   :param flags: Open flags (O_RDONLY, O_WRONLY, etc.)
   :param mode: File mode for creation
   :param oh: Output parameter for the file handle
   :param ucb: User callback structure
   :param api_user: User context for permissions
   :returns: 0 on success, error code on failure

File I/O
^^^^^^^^

.. c:function:: int red_pwrite(rfs_open_hndl_t oh, const void *buf, size_t size, off_t offset, ssize_t *bytes_written, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Writes data to a file at a specified offset.

   :param oh: File handle
   :param buf: Data buffer to write
   :param size: Number of bytes to write
   :param offset: File offset for writing
   :param bytes_written: Output parameter for bytes written
   :param ucb: User callback structure
   :param api_user: User context for permissions
   :returns: 0 on success, error code on failure

   **Error codes:**

   * RED_SUCCESS (0) - Write operation succeeded
   * EBADF - Invalid file handle
   * ENOENT - File does not exist
   * ENOSPC - No space available on cluster
   * EDQUOT - User quota exceeded

S3 Operations
-----------

The filesystem API includes S3-compatible operations for object storage:

.. c:function:: int red_s3_put(const char *bucket_name, const char *key, red_s3_put_params_t *params, const red_buffer_t *data, rfs_open_hndl_t *oh, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Writes an object to S3 storage.

.. c:function:: int red_s3_get(const char *bucket_name, const char *key, red_s3_get_params_t *params, red_buffer_t *data, rfs_open_hndl_t *oh, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Retrieves an object from S3 storage.

Threading Considerations
---------------------

* File handles are core-affined
* Only the root handle can be shared between threads
* Operations must be performed on the same thread that opened the handle

Header Files
-----------

* :doc:`rfs_client_api.h <api/client>` - Core filesystem API definitions
* :doc:`rfs_client_types.h <api/client>` - Data type definitions
* :doc:`rfs_lib_ring.h <api/client>` - Ring buffer operations
* :doc:`rfs_defines.h <api/client>` - Constants and macros

See Also
--------

* :doc:`Client API Reference <client>`
* :doc:`Example: Hello World <../examples/hello_world>`
