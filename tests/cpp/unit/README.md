# RFS Basic Test

This test suite verifies the basic functionality of the RED S3 client implementation using mock objects. It ensures that the core S3 operations work correctly without requiring a real RED cluster.

## Prerequisites

- RED client library installed
- Google Test framework installed
- C++17 compatible compiler

## Building

```bash
cd <sdk_root>/tests/cpp/unit
make clean && make
```

## Running

```bash
./cpp-unit-test
```

## Test Cases

### CreateBucket
Tests the bucket creation functionality using mock objects. Verifies that:
1. The bucket creation request is properly formatted
2. The response is correctly handled
3. The bucket object is properly initialized

### PutObject
Tests the object upload functionality using mock objects. Verifies that:
1. The object upload request is properly formatted
2. The response is correctly handled
3. The upload operation completes successfully

## Test Output

The test program generates two output files:
- `test_results.xml` - Test results in XML format
- `test_results.json` - Test results in JSON format

## Mock Testing

This test suite uses mock objects to simulate the RED client library's behavior. This allows us to:
1. Test the S3 client implementation without a real cluster
2. Verify correct request formatting
3. Test error handling scenarios
4. Ensure consistent behavior

## Dependencies

- `simple_s3_client.hpp` - S3 client implementation
- `simple_s3_client.cpp` - S3 client implementation
- Google Test framework
- RED client library
