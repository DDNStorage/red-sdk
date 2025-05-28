Threading Guide
=============

The RED SDK uses a sophisticated threading model to provide high-performance asynchronous operations.

Thread Safety
-----------

Key Thread Safety Considerations:

1. **Handle Affinity**
   * File handles are core-affined
   * Only root handle can be shared
   * Operations must use originating thread

Polling and Thread Affinity
----------------------

The RED SDK uses thread-local ring buffers for operation completions. This has important implications:

.. important::
   * Operations are tied to the thread that initiated them
   * With manual polling, you must poll on the same thread that initiated the operation
   * With automatic polling, a dedicated thread handles completions for all threads

.. warning::
   **Thread Synchronization with Automatic Polling**

   When using automatic polling (``poller_thread = true``):

   * Callbacks execute on the poller thread, NOT on the thread that issued the request
   * This creates a thread context switch between request and callback
   * You must implement proper thread synchronization for any data accessed by both threads
   * Be careful with shared resources, locks, and thread-local data
   * Consider thread-safety implications when designing callback functions

For complete details on polling mechanisms and callback execution, see :doc:`Completion Polling <../api/completions>`.

See Also
--------

* :doc:`Architecture Guide <architecture>`
* :doc:`Asynchronous Operations Guide <async_operations>`
* :doc:`Example: Multi-threaded Operations <../examples/threading>`
