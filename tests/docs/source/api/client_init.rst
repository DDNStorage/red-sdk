Client Initialization
==================

The RED SDK provides initialization functions with configuration options. This guide covers the initialization options and their usage.

Initialization
------------------

To initialize the RED client library, use ``red_client_lib_init_v3()``:

.. code-block:: c

    int red_client_lib_init_v3(const struct red_client_lib_init_opts *opts);

This function accepts a structure with detailed configuration options:

.. code-block:: c

    struct red_client_lib_init_opts {
        uint32_t    num_sthreads;     /* Number of client's service threads to initialize */
        const char *coremask;         /* Coremask where the service threads are running */
        uint32_t    num_buffers;      /* Number of buffers to allocate */
        uint32_t    num_ring_entries; /* Number of entries per ring */
        bool        poller_thread;    /* Switch on/off the RFS poller thread */
    };

Initialization Options
-------------------

.. list-table::
   :widths: 20 80
   :header-rows: 0

   * - **num_sthreads**
     - Number of service threads, each with its own ring buffer
   * - **coremask**
     - CPU core affinity mask in taskset format (e.g., "0-3")
   * - **num_buffers**
     - Buffer pool size for operations
   * - **num_ring_entries**
     - Maximum concurrent operations per thread
   * - **poller_thread**
     - Controls completion handling:

       * ``true``: Automatic polling via dedicated thread
       * ``false``: Manual polling required (see :doc:`Completion Polling <completions>`)

See Also
-------

* :doc:`Completion Polling <completions>`
* :doc:`Threading Guide <../guides/threading>`
* :doc:`Asynchronous Operations Guide <../guides/async_operations>`
* :doc:`Architecture Guide <../guides/architecture>`
