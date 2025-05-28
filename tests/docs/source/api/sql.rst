SQL API Reference
===============

The RED SDK SQL API provides asynchronous database operations within the distributed system environment. All operations are non-blocking and use the standard callback mechanism.

Key Concepts
-----------

* Asynchronous SQL operations
* Distributed query execution
* Transaction management
* Connection pooling

Database Operations
----------------

Connection Management
^^^^^^^^^^^^^^^^^

.. c:function:: int red_sql_connect(const char *conninfo, red_sql_conn_t *conn, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Establishes a connection to the SQL database.

   :param conninfo: Connection string
   :param conn: Output parameter for connection handle
   :param ucb: User callback structure
   :param api_user: User context for permissions
   :returns: 0 on success, error code on failure

Query Execution
^^^^^^^^^^^^^

.. c:function:: int red_sql_exec(red_sql_conn_t conn, const char *query, red_sql_result_t *result, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Executes a SQL query asynchronously.

   :param conn: Database connection handle
   :param query: SQL query string
   :param result: Output parameter for query results
   :param ucb: User callback structure
   :param api_user: User context for permissions
   :returns: 0 on success, error code on failure

Transaction Management
^^^^^^^^^^^^^^^^^^

.. c:function:: int red_sql_begin(red_sql_conn_t conn, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Starts a new transaction.

.. c:function:: int red_sql_commit(red_sql_conn_t conn, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Commits the current transaction.

.. c:function:: int red_sql_rollback(red_sql_conn_t conn, rfs_usercb_t *ucb, red_api_user_t *api_user)

   Rolls back the current transaction.

Result Processing
--------------

.. c:struct:: red_sql_result_t

   Structure containing query results.

   .. c:member:: uint32_t num_rows

      Number of rows in the result set.

   .. c:member:: uint32_t num_cols

      Number of columns in the result set.

   .. c:member:: red_sql_field_t *fields

      Array of field descriptors.

Error Handling
------------

Common error codes:

* RED_SQL_SUCCESS (0) - Operation completed successfully
* RED_SQL_CONN_FAILED - Connection failed
* RED_SQL_QUERY_ERROR - Query execution failed
* RED_SQL_TXN_ERROR - Transaction operation failed

Performance Considerations
-----------------------

* Use connection pooling for better performance
* Batch related queries in transactions
* Use prepared statements for repeated queries
* Consider query optimization for distributed execution

Header Files
----------

* ``red_sql_api.h`` - Core SQL API definitions
* ``sql_lib_ring.h`` - Ring buffer operations for SQL

See Also
--------

* :doc:`Client API Reference <client>`
* :doc:`Example: SQL Operations <../examples/sql_example>` 