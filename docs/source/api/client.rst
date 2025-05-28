Client API Reference
===================

The RED SDK Client API provides the foundation for interacting with the RED Server. It handles initialization, communication, and resource management.

Initialization
------------

.. c:function:: int red_client_init(void)

   Initializes the RED SDK library. Must be called once per client process.

   :returns: 0 on success, error code on failure

.. c:function:: void red_client_fini(void)

   Cleans up resources acquired by the RED SDK library.

Callback Mechanism
---------------

The RED SDK uses callbacks for asynchronous operation completion:

.. c:struct:: rfs_usercb_t

   Structure for callback registration.

   .. c:member:: rfs_two_arg_cbp_t ucb_fun

      Callback function pointer.

   .. c:member:: const void *ucb_arg

      User argument passed to callback.

User Context
----------

.. c:struct:: red_api_user_t

   Structure for user authentication and permissions.

   .. c:member:: uint64_t rfs_uid

      User ID for Linux-style permissions.

   .. c:member:: uint64_t rfs_gid

      Group ID for Linux-style permissions.

   .. c:member:: uint32_t rfs_project

      Project ID for quota management.

   .. c:member:: char *rfs_tenname

      Tenant name for multi-tenancy support.

   .. c:member:: char *rfs_subname

      Sub-tenant name for hierarchical tenancy.


Header Files
----------

* ``red_client_ring.h`` - Core client functionality
* ``red_client_types.h`` - Data type definitions
* ``red_client_api.hpp`` - C++ API definitions

See Also
--------

* :doc:`Filesystem API Reference <filesystem>`
* :doc:`Getting Started Guide <../guides/getting_started>`
