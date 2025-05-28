Queue API Reference
=================

The RED SDK Queue API provides distributed message queue functionality with asynchronous operations. It supports reliable message delivery, multiple consumer patterns, and distributed queue management.

Key Concepts
-----------

* Asynchronous message operations
* Distributed queue management
* Message persistence
* Consumer group support

Queue Operations
-------------

Queue Management
^^^^^^^^^^^^^

.. c:function:: int red_queue_create(const char *name, red_queue_params_t *params, red_queue_t *queue, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Creates a new message queue.

   :param name: Queue name
   :param params: Queue configuration parameters
   :param queue: Output parameter for queue handle
   :param ucb: User callback structure
   :param api_user: User context for permissions
   :returns: 0 on success, error code on failure

Message Operations
^^^^^^^^^^^^^^^

.. c:function:: int red_queue_send(red_queue_t queue, const void *msg, size_t msg_size, red_msg_params_t *params, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Sends a message to the queue.

   :param queue: Queue handle
   :param msg: Message data
   :param msg_size: Size of message in bytes
   :param params: Message parameters (priority, TTL, etc.)
   :param ucb: User callback structure
   :param api_user: User context for permissions
   :returns: 0 on success, error code on failure

.. c:function:: int red_queue_receive(red_queue_t queue, void *buf, size_t buf_size, size_t *msg_size, red_msg_info_t *info, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Receives a message from the queue.

   :param queue: Queue handle
   :param buf: Buffer for received message
   :param buf_size: Size of buffer
   :param msg_size: Output parameter for actual message size
   :param info: Output parameter for message metadata
   :param ucb: User callback structure
   :param api_user: User context for permissions
   :returns: 0 on success, error code on failure

Consumer Groups
-------------

.. c:function:: int red_queue_consumer_group_create(red_queue_t queue, const char *group_name, red_consumer_group_t *group, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Creates a consumer group for the queue.

.. c:function:: int red_queue_consumer_join(red_consumer_group_t group, const char *consumer_id, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Joins a consumer group.

Message Patterns
-------------

The Queue API supports multiple messaging patterns:

* Point-to-Point
* Publish/Subscribe
* Request/Reply
* Fan-out

Error Handling
------------

Common error codes:

* RED_QUEUE_SUCCESS (0) - Operation completed successfully
* RED_QUEUE_FULL - Queue is at capacity
* RED_QUEUE_EMPTY - No messages available
* RED_QUEUE_TIMEOUT - Operation timed out

Performance Considerations
-----------------------

* Use appropriate message patterns for your use case
* Configure queue parameters for optimal performance
* Consider message batching for high-throughput scenarios
* Monitor queue depth and consumer lag

Header Files
----------

* ``red_queue_api.h`` - Core queue API definitions
* ``queue_types.h`` - Queue-related type definitions

See Also
--------

* :doc:`Client API Reference <client>`
* :doc:`Example: Queue Operations <../examples/queue_example>` 