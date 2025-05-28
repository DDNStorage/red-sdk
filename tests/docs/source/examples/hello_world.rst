Hello World Example
==================

This example demonstrates how to use the RED SDK to write "Hello World" to an S3 bucket using the S3 client container.

Prerequisites
------------

- RED SDK installed
- S3 client container running
- Basic understanding of C++

Code Example
-----------

The following example shows how to:

1. Initialize a RED SDK session
2. Connect to the S3 client container
3. Write "Hello World" to an S3 bucket
4. Clean up resources

.. code-block:: cpp

    // Include necessary headers
    #include <red/red_client_api.h>
    #include <iostream>
    #include <string>

    int main() {
        // Initialize RED SDK session
        red_init_params_t init_params = {};
        red_api_user_t api_user = {};
        rfs_usercb_t ucb = {};

        // Connect to S3 client container
        rfs_dataset_hndl_t ds_hndl;
        if (red_dataset_open("s3", &ds_hndl, &ucb, &api_user) != 0) {
            std::cerr << "Failed to open dataset" << std::endl;
            return 1;
        }

        // Write "Hello World" to S3 bucket
        const char* content = "Hello World!";
        const char* bucket = "my-bucket";
        const char* key = "hello.txt";

        red_s3_put_params_t params = {};
        red_buffer_t data = {
            .buf = (void*)content,
            .len = strlen(content)
        };

        if (red_s3_put(bucket, key, &params, &data, nullptr, &ucb, &api_user) != 0) {
            std::cerr << "Failed to write to S3" << std::endl;
            red_dataset_close(ds_hndl, &ucb, &api_user);
            return 1;
        }

        // Clean up
        red_dataset_close(ds_hndl, &ucb, &api_user);
        std::cout << "Successfully wrote Hello World to S3!" << std::endl;
        return 0;
    }

Building and Running
------------------

1. Navigate to the example directory:

   .. code-block:: bash

       cd examples/cpp/hello_world

2. Build the example:

   .. code-block:: bash

       make

3. Run the example:

   .. code-block:: bash

       ./hello_world

Expected Output
-------------

If successful, you should see:

.. code-block:: text

    Successfully wrote Hello World to S3!

You can verify the file was created by checking the S3 bucket "my-bucket" for a file named "hello.txt".
