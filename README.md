# RED Software Development Kit (SDK)

The RED SDK provides a comprehensive set of header files, libraries, and utilities for developing external client applications that interact with RED storage systems. This SDK enables third-party developers to build applications that leverage RED's high-performance storage capabilities.

## Key Features

- Public C/C++ API for RED client applications
- Cross-platform compatibility for various client environments
- Simplified access to RED storage features including object storage, file systems, and queues
- Performance-optimized client libraries

## Usage

Include the SDK headers in your application:

```c
#include <red/red_status.h>
#include <red/red_client_types.h>
// Additional headers as needed
```

The SDK is installed to `/opt/ddn/red/include/red/` when the red-client-dev package is installed.

## Examples

Example code demonstrating how to use the SDK can be found in the `examples` directory. These examples cover common use cases and provide a starting point for your own applications.

## Documentation

For more detailed information about the SDK, please refer to the documentation in `sdk/docs/README.md`.
