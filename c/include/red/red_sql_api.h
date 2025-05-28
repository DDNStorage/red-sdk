/******************************************************************************
 *
 * @file
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
 * RED SQL client API.
 *
 ******************************************************************************/

#ifndef RED_SQL_API_H
#define RED_SQL_API_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* Version information */
#define RED_SQL_API_VERSION_MAJOR 1
#define RED_SQL_API_VERSION_MINOR 0
#define RED_SQL_API_VERSION_PATCH 0

#define RED_SQL_API_VERSION                                                              \
    ((RED_SQL_API_VERSION_MAJOR << 16) | (RED_SQL_API_VERSION_MINOR << 8) |              \
     (RED_SQL_API_VERSION_PATCH))

/* Compatibility macros */
#define RED_SQL_API_VERSION_CHECK(major, minor, patch)                                   \
    (RED_SQL_API_VERSION >= ((major << 16) | (minor << 8) | (patch)))

/* Feature availability macros */
#if RED_SQL_API_VERSION_CHECK(1, 0, 0)
#define RED_SQL_API_HAS_EXEC_DIRECT
#endif

#include "red_client_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup RED_SQL_API RED SQL API
 * @{
 */

/**
 * @brief Allocate a new SQL statement handle. All interactions with the red
 * sql requires this statement handle. Dataset and user need to be initialized
 * before calling this.
 *
 * @param[in]  ds_hndl   Dataset handle.
 * @param[out] stmt_hndl Pointer to store the allocated statement handle.
 * @param[in]  api_userp API user information.

 * @return int Status code indicating success or failure.
 */
int red_sql_alloc_stmt(red_ds_hndl_t        ds_hndl,
                       red_sql_stmt_hndl_t *stmt_hndl,
                       red_api_user_t      *api_userp);

/**
 * @brief Release the statement handle to terminate the interaction with red
 * sql.
 *
 * @param[in,out] stmt_hndl Pointer to the statement handle to be freed.
 * @return int Status code indicating success or failure.
 */
int red_sql_free_stmt(red_sql_stmt_hndl_t *stmt_hndl);

/**
 * @brief Reset the statement handle for reuse, e.g., to executue a new sql
 * query.
 *
 * @param[in] stmt_hndl Statement handle to be reset.

 * @return int Status code indicating success or failure.
 */
int red_sql_reset_stmt(red_sql_stmt_hndl_t stmt_hndl);

/**
 * @brief Check if the current execution has completed. The execution could be
 * incomplete because the given buffer was not enough to accommodate the query
 * result. The caller is expected to consume the buffer and resume the query
 * execution by calling red_sql_exec_direct().
 *
 * @param[in] stmt_hndl Statement handle to check.

 * @return true if the current execution has completed, false otherwise.
 */
bool red_sql_is_done_stmt(red_sql_stmt_hndl_t stmt_hndl);

/**
 * @brief Execute a query. The caller needs to check with
 * red_sql_is_done_stmt() to check if the given buffer was sufficient or not.
 * This is an asynchronous interface. If the buffer @bufp is too small to make
 * any progress, RED_ENOBUFS is returned.
 *
 * NOTE: the buffer is NOT null-terminated!
 *
 * Example:
 * ---
 * int rc;
 * rfs_test_sync_api_t sync;
 *
 * rc = red_sql_exec_direct(hndl, str, buf, size, used, sync.tsa_get_ucb());
 * red_status_t rs = sync.tsa_wait(rc);
 * ---
 *
 * @param[in]  stmt_hndl Statement handle.
 * @param[in]  sqlstr    SQL statement string to execute.
 * @param[out] bufp      Buffer to store the execution results.
 * @param[in]  buf_size  Size of the buffer.
 * @param[out] buf_usedp Pointer to store the amount of buffer used.
 * @param[in]  ucb       User callback information.

 * @return int Status code indicating success or failure.
 */
int red_sql_exec_direct(red_sql_stmt_hndl_t stmt_hndl,
                        const char         *sqlstr,
                        char               *bufp,
                        size_t              buf_size,
                        size_t             *buf_usedp,
                        rfs_usercb_t       *ucb);

/**
 * @brief Execute a query, synchrnous wrapper of red_sql_exec_direct().
 *
 * @param[in]  stmt_hndl Statement handle.
 * @param[in]  sqlstr    SQL statement to execute.
 * @param[out] bufp      Buffer to be filled with the result.
 * @param[in]  buf_size  Buffer size.
 * @param[out] buf_usedp Size of result that fills the buffer.

 * @return int Status code indicating success or failure.
 */
red_status_t red_sql_exec_direct_sync(red_sql_stmt_hndl_t stmt_hndl,
                                      const char         *sqlstrp,
                                      char               *bufp,
                                      size_t              bufsize,
                                      size_t             *bufused);

/**
 * @brief Get the sql message from the last sql execution.
 *
 * @param[in] stmt_hndl Statement handle.
 *
 * @return the sql message from the last sql execution. The message is returned
 * in a null terminated string format. In case of success, NULL is returned.
 * The returned string must NOT be freed by the caller.
 */
const char *red_sql_msg(red_sql_stmt_hndl_t stmt_hndl);

/**
 * @brief Reset the sql message.
 *
 * @param[in] stmt_hndl Statement handle.
 */
void red_sql_reset_msg(red_sql_stmt_hndl_t stmt_hndl);

/** @} */ /* end of RED_SQL_API group */

#ifdef __cplusplus
}
#endif

#endif /* RED_SQL_API_H */
