# Simple S3 Example

This example demonstrates basic S3 operations using the RED client library. It shows how to:
1. Create a bucket
2. Upload a file
3. Download and read a file

## Prerequisites

- RED client library installed
- RED client setup completed with proper certificates
- Access to a RED cluster

## Building

```bash
cd <sdk_root>/examples/cpp/simple_s3
make clean && make
```

## Running

The example requires the following arguments:
1. `cluster` - The cluster name (e.g., "default")
2. `tenant` - The tenant name (e.g., "red")
3. `subtenant` - The subtenant name (e.g., "red")
4. `bucket` - The bucket name (e.g., "test-bucket")

Example command:
```bash
cd <sdk_root>/examples/cpp
sudo bash -c 'redcli client setup --server <server_ip> && source ~/.config/red/redrc && ASAN_OPTIONS=detect_leaks=0 ./simple_s3/simple-s3-example default red red test-bucket'
```

## What it Does

1. Initializes the RED client library
2. Establishes a session with the specified tenant/subtenant
3. Creates a bucket with the given name
4. Uploads a "Hello World" file to the bucket
5. Downloads and prints the file contents
6. Cleans up resources and shuts down

## Environment Variables

The example uses the following environment variables from the RED client setup:
- `LD_LIBRARY_PATH` - Library search path
- `RED_CERT_PATH` - Path to client certificate
- `RED_KEY_PATH` - Path to client key
- `RED_CA_PATH` - Path to server certificate

These are typically set by sourcing the redrc file after running `redcli client setup`. 