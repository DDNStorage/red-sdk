Completion Polling
================

This guide covers the two mechanisms for handling operation completions in the RED SDK.

Polling Mechanisms
----------------

.. list-table::
   :header-rows: 1
   :widths: 20 40 40

   * - Feature
     - Automatic Polling (``poller_thread = true``)
     - Manual Polling (``poller_thread = false``)
   * - Implementation
     - Dedicated thread using ``epoll``
     - Application calls ``red_client_lib_poll()``
   * - Thread handling
     - Works for all threads automatically
     - Each thread must poll its own completions
   * - Callback context
     - Executed in poller thread context
     - Executed in calling thread context
   * - Critical requirement
     - None (automatic)
     - Must call on every thread that issues operations

.. important::
   **Thread Affinity Requirements:**

   * Operations are tied to the thread that initiated them
   * With manual polling, you must call ``red_client_lib_poll()`` on the same thread that initiated the operation
   * With automatic polling, callbacks execute on the poller thread, not on the thread that issued the command

.. warning::
   **Thread Synchronization with Automatic Polling**

   When using automatic polling (``poller_thread = true``):

   * Callbacks execute on the poller thread, NOT on the thread that issued the request
   * This creates a thread context switch between request and callback
   * You must implement proper thread synchronization for any data accessed by both threads
   * Be careful with shared resources, locks, and thread-local data
   * Consider thread-safety implications when designing callback functions

Example Usage
-----------

.. code-block:: c

    // Automatic polling setup
    opts.poller_thread = true;
    red_client_lib_init_v3(&opts);
    // No polling needed - callbacks execute automatically

    // Manual polling setup
    opts.poller_thread = false;
    red_client_lib_init_v3(&opts);

    // Must poll on each thread that issues operations
    rfs_usercomp_t ucps[MAX_COMPLETIONS];
    int num_completed = red_client_lib_poll(ucps, MAX_COMPLETIONS);

Using red_client_lib_poll()
------------------------

API for manual polling:

.. code-block:: c

    int red_client_lib_poll(rfs_usercomp_t *ucps, unsigned int num_ucps);

* ``ucps``: Output array for completion data
* ``num_ucps``: Maximum completions to retrieve
* Returns: Number of completions processed or negative error code

Processing completions:

.. code-block:: c

    #define MAX_COMPLETIONS 16
    rfs_usercomp_t ucps[MAX_COMPLETIONS];
    int num_completed = red_client_lib_poll(ucps, MAX_COMPLETIONS);

    if (num_completed > 0) {
        for (int i = 0; i < num_completed; i++) {
            ucps[i].ucp_fun(ucps[i].ucp_res, ucps[i].ucp_arg);
        }
    }

Key Considerations
--------------

* **Thread Safety**: Poll only on the thread that initiated operations
* **Resource Usage**: Poller thread consumes a CPU core
* **Monitoring**: Use ``red_client_lib_poll_fd()`` with select/poll/epoll for efficient waiting

See Also
-------

* :doc:`Client Initialization <client_init>`
* :doc:`Threading Guide <../guides/threading>`
* :doc:`Asynchronous Operations Guide <../guides/async_operations>`
