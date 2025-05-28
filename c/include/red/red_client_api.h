/******************************************************************************
 *
 * @copyright
 *                               --- WARNING ---
 *
 *     This work contains trade secrets of DataDirect Networks, Inc.  Any
 *     unauthorized use or disclosure of the work, or any part thereof, is
 *     strictly prohibited. Any use of this work without an express license
 *     or permission is in violation of applicable laws.
 *
 * @copyright DataDirect Networks, Inc. CONFIDENTIAL AND PROPRIETARY
 * @copyright DataDirect Networks Copyright, Inc. (c) 2021-2024. All rights reserved.
 *
 * @section DESCRIPTION
 *
 *  Project: RED
 *
 *  Description: Provides interfaces to access the RED client library.
 *
 *  Authors: Noel Otterness (notterness@ddn.com)
 *           Dmitry Lapik (dlapik@ddn.com)
 *
 ******************************************************************************/

#ifndef RED_CLIENT_API_H_
#define RED_CLIENT_API_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* Version information */
#define RED_CLIENT_API_VERSION_MAJOR 1
#define RED_CLIENT_API_VERSION_MINOR 0
#define RED_CLIENT_API_VERSION_PATCH 0

#define RED_CLIENT_API_VERSION                                                           \
    ((RED_CLIENT_API_VERSION_MAJOR << 16) | (RED_CLIENT_API_VERSION_MINOR << 8) |        \
     (RED_CLIENT_API_VERSION_PATCH))

/* Compatibility macros */
#define RED_CLIENT_API_VERSION_CHECK(major, minor, patch)                                \
    (RED_CLIENT_API_VERSION >= ((major << 16) | (minor << 8) | (patch)))

#include <stdint.h>
#include <stddef.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h> /* For PATH_MAX */

#include "red_client_types.h"
#include "red_dhash_api.h"
#include "red_status.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup RED_CLIENT_API RED Client API
 * @{
 */

/**
 * @brief Check if the RED client is started
 *
 * @return true if the RED client is started, false otherwise
 */
bool red_client_is_started();

/**
 * @brief Check if all client dependencies and services
 *        are ready. This API will block for a given
 *        amount of time and wait for the client to
 *        become ready if it is not already ready
 *
 * @param[in] timeout_sec amount of time to wait for client
 *                        to become ready
 *
 * @return true if the client is ready, else false
 *
 * Should be called only after red_client_init() and red_auth_add_*() by
 * stateful clients to ensure that the red client is ready.
 */
bool red_client_is_ready(unsigned int timeout_sec);

/**
 * @brief Allocate and register a buffer for IO operations
 *
 * @param[in] size size of the IO memory region to allocate
 * @return Pointer to the allocated memory. Must be freed with
 *         red_client_free_buffer()
 */
void *red_client_alloc_buffer(size_t size);

/**
 * @brief Free previously allocated buffer
 *
 * @param[in] ptr Pointer previously allocated with red_client_alloc_buffer()
 */
void red_client_free_buffer(void *ptr);

/**
 * @brief Return the number of running service threads
 */
unsigned red_client_get_num_service_threads(void);

/**
 * @brief Return the service threads id running on given lcore
 *
 * @param[in] unsigned int logical core number
 * @return  RED_ERANGE - if lcore is greater than max cpu set
 *          RED_EINVAL - if no sthread runs on the lcore
 *          service thread id - if the lcore is valid and service threads runs
 *                              on the lcore.
 */
red_rc_t red_client_get_lcore_2_service_thread_id(uint32_t lcore);

/**
 * @brief Register a buffer for IO operations
 *
 * @param[in] ptr Pointer to the buffer register
 * @param[in] nob Size of the buffer pointed by ptr
 *
 * @return RED status
 */
red_status_t red_client_register_buffer(void *ptr, size_t nob);

/**
 * @brief De-register a buffer for IO operations
 *
 * @param[in] ptr Pointer to the buffer previously allocated by
 *                red_client_register_buffer()
 *
 * @return RED status
 */
red_status_t red_client_unregister_buffer(void *ptr);

/********************* JRPC server specific APIs **************************/

enum red_client_jrpc_server_type
{
    RED_JRPC_SERVER_S3,
    RED_JRPC_SERVER_FUSE,
    RED_JRPC_SERVER_LAST
};

/* Note: These two routines are much different (no parameters).  Cannot
 *       change them until reds3 is also updated.
 */

/**
 * @brief Start the JRPC server (this call being deprecated)
 *
 * @param[in] type Type of the JRPC server to start
 * @param[in] starting_port Starting port number
 * @param[in] port_range Number of ports to test in the range
 *            [starting_port:starting_port+port_range]
 * @param[in] num_workers Number of worker threads to create
 * @param[out] selected_port Selected port number. Relevant only for
 *             successful operations.
 *
 * @note: Because of a limitation in the JRPC code, the JRPC server *MUST* be
 *        started before the initialization of the RED client with
 *        call red_client_init().
 *
 * @return RED status
 */
RED_CLIENT_TYPES_DEPRECATED
red_status_t red_client_jrpc_server_start(enum red_client_jrpc_server_type type,
                                          unsigned short                   starting_port,
                                          unsigned short                   port_range,
                                          unsigned int                     num_workers,
                                          unsigned short                  *selected_port);
/**
 * @brief Stop the JRPC server.
 */
void red_client_jrpc_server_stop();

/**
 * @brief Start of the JRPC server.
 */
void red_client_jrpc_server_start2();

/********************* Authorizations specific APIs **************************/

/**
 * @enum red_auth_type
 * @brief Types of supported authorization credentials
 */
enum red_auth_creds_type
{
    RED_AUTH_INVALID, /**< Invalid authorization credentials */
    RED_AUTH_USERID,  /**< Basic user/password authorization credentials */
    RED_AUTH_OPENID,  /**< OpenIB authorization credentials */
    RED_AUTH_CERT,    /**< Certificate-based authorization credentials */
    RED_AUTH_TOKEN,   /**< Token-based authorization credentials */
    RED_AUTH_LAST
};

/**
 * @brief Add a basic user/password authorization for a given tenant
 *
 * @param[in] tenant tenant to authorize
 * @param[in] username username
 * @param[in] password password associated to the given username
 *
 * @return The authorization index if >= 0 (operation successful), or a RED
 * status if < 0 (operation failed)
 */
red_rc_t red_auth_add_userid(const char *tenant, const char *username, const char *password);

red_rc_t red_auth_add_userid_by_ids(const char *tenant_str,
                                    uint32_t    tenant_id,
                                    uint32_t    subtenant_id,
                                    const char *username,
                                    const char *password);

/**
 * @brief Add an OpenID authorization for a given tenant
 *
 * @param[in] tenant tenant to authorize
 * @param[in] code OpenID code
 * @param[in] identity OpenID identity
 *
 * @return The authorization index if >= 0 (operation successful), or a RED
 * status if < 0 (operation failed)
 */
red_rc_t red_auth_add_openid(const char *tenant, const char *code, const char *identity);

red_rc_t red_auth_add_openid_by_ids(const char *tenant_str,
                                    uint32_t    tenant_id,
                                    uint32_t    subtenant_id,
                                    const char *code,
                                    const char *identity);

/**
 * @brief Add a certificate-based authorization for a given tenant (text version)
 *
 * @param[in] tenant tenant to authorize
 * @param[in] certificate (text format)
 *
 * @return The authorization index if >= 0 (operation successful), or a RED
 * status if < 0 (operation failed)
 */
red_rc_t red_auth_add_cert(const char *tenant, const char *cert);

red_rc_t red_auth_add_cert_by_ids(const char *tenant_str,
                                  uint32_t    tenant_id,
                                  uint32_t    subtenant_id,
                                  const char *cert);

/**
 * @brief Add a certificate-based authorization for a given tenant (file version)
 *
 * @param[in] tenant tenant to authorize
 * @param[in] cert_file path to certificate
 *
 * @return The authorization index if >= 0 (operation successful), or a RED
 * status if < 0 (operation failed)
 */
red_rc_t red_auth_add_cert_file(const char *tenant, const char *cert_file);

red_rc_t red_auth_add_cert_file_by_ids(const char *tenant_str,
                                       uint32_t    tenant_id,
                                       uint32_t    subtenant_id,
                                       const char *cert_file);

/**
 * @brief Add a token-based authorization for a given tenant
 *
 * @param[in] tenant tenant to authorize
 * @param[in] token token
 *
 * @return The authorization index if >= 0 (operation successful), or a RED
 * status if < 0 (operation failed)
 */
red_rc_t red_auth_add_token(const char *tenant, const char *token);

red_rc_t red_auth_add_token_by_ids(const char *tenant_str,
                                   uint32_t    tenant_id,
                                   uint32_t    subtenant_id,
                                   const char *token);

/**
 * @brief Remove an authorization for a given tenant
 *
 * @param[in] tenant tenant of the authorization to remove
 *
 * @return RED status
 */
red_status_t red_auth_remove(const char *tenant);
red_status_t red_auth_remove_by_ids(const char *tenant, uint32_t tenant_id, uint32_t subtenant_id);

/**
 * @brief Remove an authorization for a given auth index
 *
 * @param[in] auth_index index of the authorization to remove
 *
 * @return RED status
 */
red_status_t red_auth_index_remove(uint32_t auth_index);

/**
 * @brief Return the auth index of the given tenant
 *
 * @param[in] tenant tenant to target
 *
 * @return The authorization index if >= 0 (operation successful), or a RED
 * status if < 0 (operation failed)
 */
red_rc_t red_auth_get_index(const char *tenant);

red_rc_t red_auth_get_index_by_ids(const char *tenant_str,
                                   uint32_t    tenant_id,
                                   uint32_t    subtenant_id);

/**
 * @brief Check if the authorization is active for a given tenant
 *
 * @param[in] tenant tenant to target
 *
 * @return true if the authorization for the given tenant is active, false otherwise
 */
bool red_auth_is_active(const char *tenant);

bool red_auth_is_active_by_ids(const char *tenant_str, uint32_t tenant_id, uint32_t subtenant_id);

/**
 * @brief Check if the given authorization index is active
 *
 * @param[in] auth_index authorization index to target
 *
 * @return true if the given authorization index is active, false otherwise
 */
bool red_auth_index_is_active(uint32_t auth_index);

/**
 * @brief Find the tenant and subtenant IDs associated with
 *  an auth_index
 *
 * @param[in] auth_index authorization index to target
 * @param[out] ids array of tenant and subtenant IDs
 *
 * @return true if the given authorization index is active, false otherwise
 */
bool red_auth_index_to_ids(uint32_t auth_index, uint32_t ids[2]);

const char *red_auth_index_to_str(uint32_t auth_index);

/**
 * @brief Find the auth_index by the tenant and subtenant IDs. The tenant
 *        name string is not required by this API
 *
 * @param[in] ids array of tenant and subtenant IDs
 * @param[out] auth_index authorization index to target
 *
 * @return true if the given authorization index is active, false otherwise
 */
bool red_auth_ids_to_index(uint32_t ids[2], uint32_t *auth_index);

/**
 * @brief Retrieve the type of authorization credentials for a given tenant
 *
 * @param[in] tenant tenant to target
 *
 * @return the type of authorization credentials for the given tenant
 */
enum red_auth_creds_type red_auth_get_creds_type(const char *tenant);

enum red_auth_creds_type red_auth_get_creds_type_by_ids(const char *tenant_str,
                                                        uint32_t    tenant_id,
                                                        uint32_t    subtenant_id);

/**
 * @brief Retrieve the type of authorization credentials for a given authorization index
 *
 * @param[in] auth_index authorization index to target
 *
 * @return the type of authorization credentials for the given authorization index
 */
enum red_auth_creds_type red_auth_index_get_creds_type(uint32_t auth_index);

/********************* Tenant specific APIs **************************/

/**
 * @brief Create a new tenant
 *
 * Create the given tenant.
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] tenant Tenant to create
 *
 * @return RED status
 */
red_status_t red_tenant_create(uint32_t auth_index, const char *tenant);

/**
 * @brief Delete a tenant
 *
 * Delete the given tenant.
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] tenant Tenant to delete
 *
 * @return RED status
 */
red_status_t red_tenant_delete(uint32_t auth_index, const char *tenant);

/**
 * @brief Lookup a tenant
 *
 * Lookup the given tenant.
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] tenant Tenant to lookup
 * @param[out] id ID of the tenant if lookup succeeded
 *
 * @return RED status
 */
red_status_t red_tenant_lookup(uint32_t auth_index, const char *tenant, uint32_t *id);

/********************* Subtenant specific APIs **************************/

/**
 * @brief Create a new subtenant
 *
 * Create the given subtenant.
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] subtenant Subtenant to create
 *
 * @return RED status
 */
red_status_t red_subtenant_create(uint32_t auth_index, const char *subtenant);

/**
 * @brief Delete a subtenant
 *
 * Delete the given subtenant.
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] subtenant Subenant to delete
 *
 * @return RED status
 */
red_status_t red_subtenant_delete(uint32_t auth_index, const char *subtenant);

/**
 * @brief Lookup a subtenant
 *
 * Lookup the given subtenant.
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] subtenant Subtenant to lookup
 * @param[out] id ID of the subtenant if lookup succeeded
 *
 * @return RED status
 */
red_status_t red_subtenant_lookup(uint32_t auth_index, const char *subtenant, uint32_t *id);

/********************* User specific APIs **************************/

/**
 * @brief Create a principal
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] type of principal (user/group...)
 * @param[in] uuid UUID of the principal to create
 * @param[in] tenant Tenant for principal creation (optional)
 * @param[in] subtenant Subtenant for principal creation (optional)
 *
 * @return RED status
 */
red_status_t red_prncpl_create(uint32_t      auth_index,
                               const char   *type,
                               const uuid_t *uuid,
                               const char   *tenant,
                               const char   *subtenant);
/**
 * @brief Lookup a principal
 *
 * @param[in] auth_index Authorization index (returned by red_auth_add_*())
 * @param[in] type of principal (user/group... NULL -> any)
 * @param[in] uuid UUID of the principal to lookup
 * @param[out] id ID of the principal if lookup succeeded. Principal ID can be
 *             used in red_client_sqe_set_userid() when preparing the SQE.
 *
 * @return RED status
 */
red_status_t red_prncpl_lookup(uint32_t      auth_index,
                               const char   *type,
                               const uuid_t *uuid,
                               uint32_t     *id);

/**
 * @brief Get error message
 *
 * @param[in] rc red status
 * @param[in] error error message
 *
 * @return client status
 */
red_client_status_t red_client_status_error(red_status_t rc, const char *error);
/** @}*/

/************* IO memory buffers handline *************/

/**
 * @brief Allocate an IO buffer suitable for object IO operations.
 *
 * Allocate a new memory segment and register it to be ready
 * for RDMA operations to perform IOs.
 *
 * @param[in] size Define the size of the desired memory segment.
 * @return Address of the allocated segment or NULL on error.
 */
void *red_client_iomem_alloc(size_t size);

/**
 * @brief Free an existing IO buffer after usage for IO operations.
 *
 * Unregister and free the given memory segment.
 *
 * @param addr Define the address of the segment to free.
 */
void red_client_iomem_free(void *addr);

/**
 * @brief Register a pre-allocated segment for IO operations.
 *
 * Register a memory segment which is pre-allocated by the caller
 * in order to perform RDMA operations for the IOs.
 *
 * @param addr Base address of the user segment.
 * @param nob Size of the user segment.
 * @return Return success or error.
 */
red_status_t red_client_iomem_register(void *addr, size_t nob);

/**
 * @brief Unregister a pre-allocated segment used for IO operations.
 *
 * Unregister a user segment previously registered via
 * red_client_iomem_register().
 *
 * @param addr Base address of the segment to unregister.
 * @return Return success or error.
 */
red_status_t red_client_iomem_unregister(void *addr);

/**
 * @brief Export SIGUSR1 handler.
 *
 * @param sig Signal number.
 */
void red_sig_usr1_handler(int sig);

/**
 * @brief Export SIGUSR1 handler.
 *
 * @param sig Signal number.
 * @attention Deprecated, use red_sig_usr1_handler instead
 */
RED_CLIENT_TYPES_DEPRECATED
void sig_usr1_handler(int sig);

/************* Timer functions *************/

/**
 * @brief Return the current number of CPU cycles (TSC)
 *
 * @return The current number of CPU cycles
 */
uint64_t red_client_get_timer_cycles(void);

/**
 * @brief Return the frequency in Hertz
 *
 * @return the CPU frequency in Hertz
 */
uint64_t red_client_get_timer_hz(void);

/**
 * @brief Return the default auth_index
 *
 * @return the default auth_index
 */
uint32_t red_client_jrpc_get_def_auth_idx();

/* Index of the default authorization index */
#define RED_DEFAULT_AUTH_INDEX (red_client_jrpc_get_def_auth_idx())

/* Index of the administration authorization index */
#define RED_ADMIN_AUTH_INDEX 0U

/**
 * @brief Initialization options for red_client_lib_init_v3()
 */
struct red_client_lib_init_opts
{
    uint32_t    num_sthreads;     /* Number of client's service threads to initialize */
    const char *coremask;         /* Coremask where the service threads are running */
    uint32_t    num_buffers;      /* Number of buffers to allocate */
    uint32_t    num_ring_entries; /* Number of entries per ring */
    bool        poller_thread;    /* Switch on/off the RFS poller thread. Applications
                                   * that disable the poller thread need to drive completions
                                   * with red_client_lib_poll() on each thread that issues
                                   * operations. */
};

/**
 * @brief Initialize the RFS client API version 3
 *
 * Must be called once per process before any other RED SDK functions.
 *
 * @param opts Initialization options
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_client_lib_init_v3(const struct red_client_lib_init_opts *opts);

/**
 * @brief Check if the RED client library is ready for operations
 *
 * Call after initialization before issuing commands. Blocks until ready or timeout.
 *
 * @param timeout_sec Maximum wait time in seconds (0 for immediate return)
 * @return true if ready, false if not ready or timeout occurred
 */
bool red_client_lib_is_ready(unsigned int timeout_sec);

/**
 * @brief Release all RED client library resources
 *
 * Must be called when finished.
 * Do not call any RED SDK functions after this.
 */
void red_client_lib_fini(void);

/**
 * @brief Retrieve the file descriptor associated with the current thread.
 *
 * The file descriptor returned by the function stays signaled as long as the
 * current thread has completed operations to poll (with red_client_lib_pool()).
 *
 * Function Specific to the API v3
 *
 * @return file descriptor if >= 0, or -1 if the file descriptor is not
 *         available (e.g., red_client_lib_init_v3() was called with
 *         'poller_thread' = true).
 *
 */
int red_client_lib_poll_fd();

/**
 * @brief Poll the current thread for completed operations (non-blocking).
 *
 * Must be called on every thread that issues operations.
 * Each thread can only poll its own completions. Callbacks won't execute otherwise.
 *
 * Function specific to the API v3
 *
 * @param ucps Array of rfs_usercb_t pointers that completed. Must contain at
 *             least num_ucps entries.
 * @param num_ucps Number of entries in ucps.
 *
 * @return Negative values indicate an error (red_status_t cast to int). Non-negative values
 * indicate the number of completed operations processed, which is always less than or equal to
 * num_ucps. A return value of 0 means no completions were available for the thread.
 *
 * @note The caller must execute the callbacks returned by the function in ucps.
 */
int red_client_lib_poll(rfs_usercomp_t *ucps, unsigned int num_ucps);

/**
 * @brief Establish a session to a RED cluster.
 *
 * @param cluster cluster name to connect to
 * @param tenant name of the tenant to connect to
 * @param subtenant name of the subtenant to connect to
 * @param uid current posix user id @see geteuid from unistd.h
 * @param gid group posix group id @see getegid from unistd.h
 * @param api_user object to be filled with the session information
 * @return RED_SUCCESS on success, error code otherwise
 */
int red_establish_session(const char     *cluster,
                          const char     *tenant,
                          const char     *subtenant,
                          uint64_t        uid,
                          uint64_t        gid,
                          red_api_user_t *api_user);

/**
 * @brief Cleanup a session
 *
 * @param user Pointer to the red_api_user_t structure to be cleaned up.
 */
void red_cleanup_session(red_api_user_t *user);

/**
 * @brief Create a new tenant
 *
 * @param[in] tenname Name of the tenant to create
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_create_tenant(const char *tenname, rfs_usercb_t *ucb, red_api_user_t *api_user);

/**
 * @brief Delete an existing tenant
 *
 * @param[in] tenname Name of the tenant to delete
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_delete_tenant(const char *tenname, rfs_usercb_t *ucb, red_api_user_t *api_user);

/**
 * @brief Create a new subtenant within a tenant
 *
 * @param[in] tenname Name of the parent tenant
 * @param[in] subtenname Name of the subtenant to create
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_create_subtenant(const char     *tenname,
                         const char     *subtenname,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user);

/**
 * @brief Delete an existing subtenant
 *
 * @param[in] tenname Name of the parent tenant
 * @param[in] subtenname Name of the subtenant to delete
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_delete_subtenant(const char     *tenname,
                         const char     *subtenname,
                         rfs_usercb_t   *ucb,
                         red_api_user_t *api_user);

/*
 * Test APIs
 */
int red_test_alloc_deferred(rfs_buffer_alloc_e alloc_type,
                            void             **alloc_buffer,
                            rfs_usercb_t      *ucb,
                            red_api_user_t    *api_user);

int red_test_free_deferred(rfs_buffer_alloc_e alloc_type,
                           void              *alloc_buffer,
                           rfs_usercb_t      *ucb,
                           red_api_user_t    *api_user);

/*
 * Mapped memory allocation methods
 */
/**
 * @brief Allocate I/O memory
 *
 * @param[in] size Size of memory to allocate in bytes
 * @param[out] iomem Pointer to store the I/O memory handle
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS on success, error code on failure
 */
red_status_t red_iomem_alloc(size_t            size,
                             red_iomem_hndl_t *iomem,
                             rfs_usercb_t     *ucb,
                             red_api_user_t   *api_user);

/**
 * @brief Free I/O memory
 *
 * @param[in] iomem I/O memory handle to free
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS on success, error code on failure
 */
red_status_t red_iomem_free(red_iomem_hndl_t iomem,
                            rfs_usercb_t    *ucb,
                            red_api_user_t  *api_user);

/**
 * @brief Convert I/O memory handle to memory address
 *
 * @param[in] iomem I/O memory handle
 * @param[in] offset Offset into the memory region
 * @return Pointer to the memory address, or NULL on failure
 */
void *red_iomem_to_addr(red_iomem_hndl_t iomem, off_t offset);

/**
 * @brief Get the size of I/O memory
 *
 * @param[in] iomem I/O memory handle
 * @return Size of the memory region in bytes
 */
size_t red_iomem_size(red_iomem_hndl_t iomem);

/** @}*/ /* end of RED_CLIENT_API group */

#ifdef __cplusplus
}
#endif

#endif // RED_CLIENT_API_H_
