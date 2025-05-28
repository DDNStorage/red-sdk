Error Handling Guide
=================

The RED SDK provides comprehensive error handling mechanisms to help you identify and resolve issues in your applications.

Common Error Types
---------------

1. **System Errors**
   * Standard POSIX error codes (errno values)
   * SDK-specific error codes
   * Network and I/O related errors

2. **Permission Errors**
   * ``EACCES`` - Permission denied
   * ``EPERM`` - Operation not permitted
   * ``EDQUOT`` - Quota exceeded

3. **Resource Errors**
   * ``ENOMEM`` - Out of memory
   * ``ENOSPC`` - No space left
   * ``EMFILE`` - Too many open files

4. **Operation Errors**
   * ``EINVAL`` - Invalid argument
   * ``EBUSY`` - Device or resource busy
   * ``EAGAIN`` - Resource temporarily unavailable

Error Handling Patterns
--------------------

1. **Synchronous Operations**

.. code-block:: c

    rfs_open_hndl_t oh;
    rfs_usercb_t ucb = {};
    int ret;

    ret = red_openat(dir_oh, path, flags, mode, &oh, &ucb, &api_user);
    if (ret < 0) {
        // Handle error
        switch (errno) {
            case EACCES:
                // Handle permission denied
                break;
            case ENOENT:
                // Handle file not found
                break;
            default:
                // Handle other errors
                break;
        }
        return ret;
    }

2. **Asynchronous Operations**

.. code-block:: c

    void completion_callback(rfs_usercb_t *ucb) {
        if (ucb->ret < 0) {
            // Handle error from async operation
            switch (ucb->ret) {
                case -EACCES:
                    // Handle permission denied
                    break;
                case -ENOENT:
                    // Handle file not found
                    break;
                default:
                    // Handle other errors
                    break;
            }
            return;
        }
        // Process successful completion
    }

See Also
--------

* :doc:`Architecture Guide <architecture>`
* :doc:`Threading Guide <threading>`
* :doc:`Permissions Guide <permissions>`
