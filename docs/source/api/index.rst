API Reference
============

The RED SDK API provides a comprehensive set of tools for creating POSIX-compliant file systems and AWS-compatible S3 object stores within Linux user space. All APIs are asynchronous and require a ``rfs_usercb_t *ucb`` parameter.

Core Concepts
------------

* **Asynchronous Operations**: All RED SDK APIs are asynchronous by design, see :doc:`Asynchronous Operations Guide <../guides/async_operations>`
* **File Handles**: Operations use file handles (``rfs_open_hndl_t``) instead of traditional file descriptors
* **Thread Affinity**: File handles are core-affined for optimal performance
* **Callback Mechanism**: Uses ``rfs_usercb_t`` for asynchronous completion notification
* **Polling Mechanism**: Supports both automatic (poller thread) and manual polling for completions

Client Initialization
-------------------

* :doc:`Client Initialization <client_init>`
* :doc:`Completion Polling <completions>`

File System Operations
--------------------

Core file system operations include:

* File and Directory Management
* Access Control and Permissions
* Extended Attributes
* Asynchronous I/O Operations

.. toctree::
   :maxdepth: 2
   :caption: API Documentation:

   client
   filesystem
   sql
   queue

Error Handling
------------

The API uses standard error codes with additional RED-specific codes:

* Standard POSIX error codes (EBADF, ENOENT, etc.)
* RED-specific error codes for specialized operations

Security Model
------------

The security model supports:

* Linux-style UID/GID permissions
* S3-style ACLs
* Custom access control through ``red_api_user_t``

See Also
--------

* :doc:`Getting Started Guide <../guides/getting_started>`
* :doc:`Asynchronous Operations Guide <../guides/async_operations>`
* :doc:`Example Code <../examples/hello_world>`

.. _client-api:

Client APIs
----------

The client APIs provide core functionality for interacting with the RED system:

* :doc:`Red Client Ring <client>`
* :doc:`Red Client Types <client_types>`
* :doc:`Red Client Ring Buffer <client_ringbuf>`

.. _fs-api:

File System Operations
--------------------

File system operations for distributed storage:

* :doc:`RFS Client API <filesystem>`
* :doc:`RFS Types <fs_types>`
* :doc:`RFS Defines <fs_defines>`

.. _sql-api:

SQL Operations
------------

SQL database interaction APIs:

* :doc:`SQL API <sql>`
* :doc:`SQL Library Ring <sql_lib>`

.. _queue-api:

Queue Operations
--------------

Message queue system APIs:

* :doc:`Queue API <queue>`
* :doc:`Queue Types <queue_types>`
