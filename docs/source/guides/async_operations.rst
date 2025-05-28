Asynchronous Operations Guide
========================

The RED SDK is designed with asynchronous operations at its core, enabling high-performance I/O and efficient resource utilization.

Core Concepts
-----------

1. **Callback-based Architecture**
   * Operations return immediately
   * Results delivered via callbacks
   * Non-blocking I/O model

2. **User Callbacks**
   * ``rfs_usercb_t`` structure for completion notifications
   * Callback context preservation
   * Thread safety considerations

Basic Usage
---------

1. **Callback Setup**

.. code-block:: c

    // Define callback structure
    rfs_usercb_t ucb = {};
    ucb.cb_func = completion_callback;  // Function pointer
    ucb.cb_data = user_context;         // Optional user data

    // Callback function definition
    void completion_callback(rfs_usercb_t *ucb) {
        // Access result via ucb->ret
        // Access user context via ucb->cb_data
        if (ucb->ret >= 0) {
            // Handle success
        } else {
            // Handle error
        }
    }

2. **Initiating Operations**

.. code-block:: c

    // Asynchronous open
    rfs_open_hndl_t oh;
    int ret = red_openat(dir_oh, path, flags, mode, &oh, &ucb, &api_user);
    if (ret < 0) {
        // Handle immediate error
        return ret;
    }
    // Operation in progress, result will be delivered via callback

3. **Vectored Operations**

.. code-block:: c

    // Setup I/O vectors
    struct iovec iov[2];
    iov[0].iov_base = buf1;
    iov[0].iov_len = len1;
    iov[1].iov_base = buf2;
    iov[1].iov_len = len2;

    // Submit vectored operation
    ret = red_writev(fh, iov, 2, &written, &ucb, &api_user);

Performance Considerations
-----------------------

1. **Callback Processing**
   * Keep callbacks lightweight
   * Avoid blocking operations
   * Consider thread pool for heavy processing

2. **Memory Management**
   * Ensure buffers remain valid
   * Use appropriate buffer sizes
   * Consider zero-copy options

3. **Queue Depth**
   * Monitor outstanding operations
   * Implement backpressure if needed
   * Balance parallelism and resources

Completion Handling
----------------

All asynchronous operations in the RED SDK require a completion mechanism to process callbacks.

.. warning::
   With automatic polling, callbacks execute on the poller thread, creating a thread context switch.
   This requires proper synchronization for shared data.

See :doc:`Completion Polling <../api/completions>` for implementation details and thread safety considerations.

Task Thread Model
----------------

.. note::
   For basic applications and demos, using the built-in poller thread (``poller_thread = true``) is the recommended approach.
   However, the task thread model is required for proper continuations and more complex applications since objects are
   affined to the thread that acquired them. This means operations on an object must be performed on the same thread
   that created or opened that object. The task thread model provides a structured way to ensure this thread affinity
   is maintained while still leveraging the asynchronous nature of the RED SDK.

When not using the automatic poller thread (``poller_thread = false``), applications must implement their own mechanism to poll for and process completions. One effective approach is the **task thread model**.

Core Concepts
~~~~~~~~~~~~

1. **Thread-Local Ring Buffers**
   * Each thread has its own client ring buffer
   * ``red_client_lib_poll()`` only polls the current thread's ring buffer
   * Operations must be polled on the same thread that initiated them

2. **Worker Thread Architecture**
   * Dedicated worker thread(s) for processing tasks and completions
   * Task queue for submitting operations
   * Event loop for polling and dispatching callbacks

.. code-block:: text
   :caption: Task Thread Model Diagram

┌──Client Thread──┐
│ Submit tasks    │
│ via queue       │
└────────┬────────┘
         │ Thread-safe
         │ submission
   ┌─────|──────────── Worker Thread ─────────────────────┐
   │     |            ┌─────────────── Event Loop ──────┐ │
   │     |            |                                 | │
   │     ▼            |   ┌─────────────────────────┐   | │
   │  ┌─────────────┐ |   │                         │   | │
   │  │ Task Queue  │────►│ Process Tasks           │   | │
   │  │ (owned by   │ |   │                         │   | │
   │  │  worker)    │ |   │ Execute SDK API calls   │   | │
   │  │ Task 1      │ |   │                         │   | │
   │  │ Task 2      │ |   └───────────┬─────────────┘   | │
   │  │ ...         │ |               │                 | │
   │  └─────────────┘ |               ▼                 | │
   │                  |   ┌─────────────────────────┐   | │
   │  ┌─────────────┐ |   │                         │   | │
   │  │ Thread's    │────►│ Poll for completions    │   | │
   │  │ Local       │ |   | red_client_lib_poll()   │   | │
   │  │ Ring Buffer │ |   │                         │   | │
   │  └─────────────┘ |   └───────────┬─────────────┘   | │
   │                  |               │                 | │
   │                  |               ▼                 | │
   │                  |  ┌─────────────────────────┐    | │
   │                  |  │                         │    | │
   │                  |  │   Callback Execution    │    | │
   │                  |  │                         │    | │
   │                  |  └─────────────────────────┘    | │
   |                  └─────────────────────────────────┘ |
   └──────────────────────────────────────────────────────┘

Conceptual Flow
~~~~~~~~~~~~~~

The task thread model operates as follows:

1. **Client Thread Interaction**
   * Client threads submit tasks to the worker thread's task queue
   * Tasks contain operations to be executed on the worker thread
   * Client threads don't directly call RED SDK functions

2. **Worker Thread Processing**
   * The worker thread runs a continuous event loop that:
     * Processes tasks from the queue
     * Executes RED SDK operations (which are thread-local)
     * Polls for completed operations using ``red_client_lib_poll()``
     * Dispatches callbacks for completed operations

3. **Completion Handling**
   * When RED SDK operations complete, they are placed in the thread's client ring buffer
   * The worker thread polls this buffer and processes completions
   * Callbacks are executed in the context of the worker thread

.. note::
   **Efficient Polling with** ``red_client_lib_poll_fd``

   Instead of continuously polling for completions, you can use ``red_client_lib_poll_fd()`` to get a file descriptor
   that becomes signaled when the thread's client ring buffer has completions ready. This file descriptor can be
   used with ``epoll``, ``select``, or ``poll`` to efficiently wait for completion events:

   .. code-block:: c

      // Get the file descriptor for the current thread's client ring buffer
      int poll_fd = red_client_lib_poll_fd();

      // Set up poll structures
      struct pollfd pfds[1] = {
          {.fd = poll_fd, .events = POLLIN, .revents = 0}
      };

      // Wait for completions (with optional timeout)
      int rc = poll(pfds, 1, timeout_ms);

      // Check if completions are available
      if (rc > 0 && (pfds[0].revents & POLLIN)) {
          // Poll for completions
          rfs_usercomp_t ucp = {0};
          rc = red_client_lib_poll(&ucp, 1);
          if (rc == 1 && ucp.ucp_fun) {
              // Execute the callback
              ucp.ucp_fun(ucp.ucp_res, ucp.ucp_arg);
          }
      }

   This approach is used in the synchronous API wrapper example, where it waits for either a specific
   completion event (via eventfd) or any completion in the client ring buffer.

.. code-block:: c

   while (running) {
       // Process any tasks in the queue
       if (taskQueue is not empty) {
           task = taskQueue.take()
           execute(task)
       }

       // Poll for completed operations
       completions = poll_for_completions()

       // Process any completions
       for each completion in completions:
           invoke_callback(completion)
   }

.. warning::
   This is a simplified conceptual example and is not thread-safe. A proper implementation
   must ensure thread-safe task submission and execution with appropriate synchronization
   mechanisms such as mutexes, atomic operations, or lock-free data structures.

Implementation Example
~~~~~~~~~~~~~~~~~~~~~

Here's a simplified example of how to implement a task thread:

.. code-block:: c

   // In the worker thread
   void worker_thread_main() {
       // Initialize thread-local resources

       while (!shutdown_requested) {
           // Process queued tasks
           process_task_queue();

           // Poll for completions
           rfs_usercomp_t ucps[MAX_COMPLETIONS];
           int num_completed = red_client_lib_poll(ucps, MAX_COMPLETIONS);

           // Process completions
           for (int i = 0; i < num_completed; i++) {
               if (ucps[i].ucp_fun) {
                   ucps[i].ucp_fun(ucps[i].ucp_res, ucps[i].ucp_arg);
               }
           }
       }
   }

   // In the client thread
   void submit_operation() {
       // Create a task that will execute on the worker thread
       Task* task = create_task([](void* context) {
           // This lambda runs on the worker thread
           // Call RED SDK function here
           red_open(path, flags, mode, &handle, &ucb, &api_user);
       }, context);

       // Submit task to worker's queue
       worker_thread.submit_task(task);
   }

.. warning::
   This example is simplified for clarity and omits necessary thread safety mechanisms.
   In a production implementation, you must ensure that task submission is thread-safe,
   context objects have proper lifetimes, and memory management is handled correctly
   across thread boundaries.

Synchronous API Wrapper Pattern
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. warning::
   This pattern is **not recommended** for production applications and should be avoided in most cases.
   It negates the performance benefits of the asynchronous design and can lead to poor scalability.
   Only use this pattern for extremely simple use cases, prototyping, or situations where sequential
   operations are acceptable and performance is not a concern.

For limited use cases, you can implement a synchronous wrapper around the asynchronous API using the same thread for both initiating operations and polling for completions:

.. code-block:: c

   // Example from SDK's sync_api.c
   red_status_t common_sync_wait(common_sync_api_ctx_t *ctx, int rc)
   {
       if (rc != 0)
       {
           ctx->rs = (red_status_t)rc;
           return ctx->rs;
       }

       struct pollfd pfds[2] = {
           {.fd = ctx->eventfd, .events = POLLIN, .revents = 0},
           {.fd = red_client_lib_poll_fd(), .events = POLLIN, .revents = 0}};

       while (1)
       {
           rc = poll(pfds, 2, -1);
           if (rc < 0)
           {
               if (errno == EINTR)
                   continue;
               return RED_EINVAL;
           }

           if (rc == 0)
               continue;

           if (pfds[0].revents & POLLIN)
           {
               uint64_t val;
               if (read(ctx->eventfd, &val, sizeof(val)) != sizeof(val))
                   continue;
               break;
           }

           if (pfds[1].revents & POLLIN)
           {
               rfs_usercomp_t ucp = {0};
               rc = red_client_lib_poll(&ucp, 1);
               if (rc == 1 && ucp.ucp_fun)
               {
                   ucp.ucp_fun(ucp.ucp_res, ucp.ucp_arg);
               }
           }
       }

       return ctx->rs;
   }

Usage example:

.. code-block:: c

   // Initialize context
   ctx = common_sync_api_init();
   if (!ctx)
   {
       return RED_ENOMEM;
   }

   // Start asynchronous operation
   rc = red_s3_create_bucket(bucket_name, cluster, tenant, subtenant,
                             &bucket_props, bucket_hndl, &ctx->ucb, user);

   // Wait for completion
   rs = common_sync_wait(ctx, rc);

   // Clean up
   common_sync_api_free(ctx);

This pattern should be limited to:

1. Testing and prototyping before implementing a proper task thread model
2. Extremely simple applications with minimal performance requirements
3. Educational purposes to understand the underlying callback mechanism

For any production application or performance-sensitive code, use either:
- The built-in poller thread (``poller_thread = true``)
- A properly implemented task thread model as described above

Current Limitations and Future Considerations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There are some important limitations to be aware of when implementing your own task thread model:

1. **Thread Locality**
   * The client ring buffers are thread-local
   * ``red_client_lib_poll()`` only polls the current thread's client ring buffer
   * Operations must be polled on the same thread that initiated them
   * Unlike the built-in poller thread, there is currently no API to poll all threads' ring buffers

2. **Multi-Thread Coordination**
   * When using multiple worker threads, each thread must handle its own operations
   * There is no built-in mechanism to distribute completions across threads
   * Applications must implement their own thread coordination mechanisms

3. **Future Enhancements**
   * A future API may provide a ``red_client_lib_poll_all()`` function to poll all threads' ring buffers
   * This would enable more flexible worker thread architectures
   * Until then, applications should follow the thread locality principle

.. note::
   For most applications, using the built-in poller thread (``poller_thread = true``) is recommended
   unless you have specific requirements that necessitate implementing your own task thread model.

Error Handling
------------

1. **Immediate Errors**
   * Check return value from submission
   * Handle resource allocation failures
   * Validate parameters

2. **Asynchronous Errors**
   * Check callback return value
   * Implement retry logic if needed
   * Clean up resources

3. **Cancellation**
   * Handle operation cancellation
   * Clean up pending operations
   * Reset state as needed

See Also
--------

* :doc:`Architecture Guide <architecture>`
* :doc:`Threading Guide <threading>`
* :doc:`Error Handling Guide <error_handling>`
