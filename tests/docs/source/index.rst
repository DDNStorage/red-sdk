RED SDK Documentation
===================

Welcome to the RED SDK documentation. The RED SDK provides a POSIX-compliant file system and AWS-compatible S3 object store within Linux user space.

Core Concepts
------------

* All APIs are asynchronous with callback-based completion
* File operations use handles (``rfs_open_hndl_t``) instead of file descriptors
* Core-affined handles for thread safety
* Enhanced POSIX-like operations with S3 compatibility
* Flexible polling mechanisms for operation completions

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   guides/getting_started
   guides/architecture
   guides/threading
   guides/permissions
   guides/async_operations
   guides/error_handling
   api/index
   api/client_init
   api/completions
   examples/index

Architecture Guide
----------------

* :doc:`Client Initialization <guides/architecture>`
* :doc:`File Handles <guides/architecture>`
* :doc:`Client Callbacks <guides/architecture>`
* :doc:`Completion Notification <guides/architecture>`

Threading Model
-------------

* :doc:`Thread Types <guides/threading>`
* :doc:`Blocking Notification <guides/threading>`
* :doc:`File Descriptor Wait <guides/threading>`
* :doc:`Thread Safety <guides/threading>`

API Reference
------------

Core APIs:

* :doc:`Client API <api/client>`
* :doc:`Filesystem API <api/filesystem>`
* :doc:`SQL API <api/sql>`
* :doc:`Queue API <api/queue>`

Advanced Topics
-------------

* :doc:`Permission Models <guides/permissions>`
* :doc:`Asynchronous Operations <guides/async_operations>`
* :doc:`Error Handling <guides/error_handling>`
* :doc:`Performance Tuning <guides/performance>`

Examples
--------

* :doc:`Basic Usage <examples/hello_world>`
* :doc:`File Operations <examples/file_ops>`
* :doc:`S3 Operations <examples/s3_ops>`
* :doc:`Distributed Training <examples/distributed_training>`

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
