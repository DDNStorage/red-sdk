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
 *  Description: This contains the methods for filesystem operations
 *      in the RED client library.
 *
 ******************************************************************************/

#ifndef RED_FS_API_H_
#define RED_FS_API_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stddef.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "red_client_types.h"
#include "red_status.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup RED_FS_API RED Filesystem API
 * @{
 */

/**
 * @brief Open/create a RED file system file
 *
 * @param dir_oh    Open handle of the parent directory
 * @param pathname  Pathname of the file from dir_oh
 * @param flags     Open flags
 * @param mode      mode bits to apply if the file is created
 * @param open_fd   Open handle for the file
 * @param ucb       User call back
 * @param api_user  This is the information about the caller and where the API
 *                  is directed to.
 *
 * @return Return the status of the operation being queued to the uring.
 *         If successfully queued, the provided callback function is
 *         called upon completion of the operation.
 */
int red_openat(rfs_open_hndl_t  dir_oh,
               const char      *pathname,
               int              flags,
               mode_t           mode,
               rfs_open_hndl_t *oh,
               rfs_usercb_t    *ucb,
               red_api_user_t  *api_user) __nonnull((2));

struct open_how;
/**
 * @brief Open/create a RED file system file
 *
 * Extension of openat(), provides additional functionality. flags,
 * mode, and others are provided in struct open_how.
 */
int red_openat2(rfs_open_hndl_t        dir_oh,
                const char            *pathname,
                const struct open_how *how,
                size_t                 size,
                rfs_open_hndl_t       *oh,
                rfs_usercb_t          *ucb,
                red_api_user_t        *api_user) __nonnull((2));

/**
 * @brief Get attributes of an opened file
 */
int red_fstatat(rfs_open_hndl_t dir_oh,
                const char     *pathname,
                struct stat    *statbuf,
                int             flags,
                rfs_usercb_t   *ucb,
                red_api_user_t *api_user);

/**
 * @brief Close an open file handle
 *
 * @param[in] oh Open handle to close
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_close(rfs_open_hndl_t oh, rfs_usercb_t *ucb, red_api_user_t *api_user);

/**
 * @brief Create a temporary file
 *
 * @param[in] dir_oh Open handle to the directory where the temporary file will be created
 * @param[in] flags File creation flags (same as for red_openat)
 * @param[in] mode File permission mode
 * @param[out] created_oh Pointer to store the open handle for the created temporary file
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_create_tmpfile(rfs_open_hndl_t  dir_oh,
                       int              flags,
                       mode_t           mode,
                       rfs_open_hndl_t *created_oh,
                       rfs_usercb_t    *ucb,
                       red_api_user_t  *api_user);

/**
 * @brief Create a directory.
 *
 * @return Return the status of the operation being queued to the uring.
 *         If successfully queued, the provided callback function is
 *         called upon completion of the operation.
 */
int red_mkdirat(rfs_open_hndl_t  dir_oh,
                const char      *pathname,
                mode_t           mode,
                rfs_open_hndl_t *new_oh,
                rfs_usercb_t    *ucb,
                red_api_user_t  *api_user);

/**
 * @brief Perform lookup on the name within REDFS directory and
 *        return a file handle if the lookup succeeds.
 *        flags supplied by the caller (refer to flags param).
 *
 * @param dir_oh           - open handle to the parent directory.
 *                           If dir_oh is provided and is valid,
 *                           the pathname is interpretted relative to
 *                           the path of the dir_oh
 * @param pathname         - pathname can be a relative or absolute path
 *                           ending with a directory or file.
 * @param file_handle      - pointer to file_handle structure to hold the
 *                           file_handle data.
 * @param flags            - supported flags. Return codes are based on
 *                           whether the inode referred to by the
 *                           path last component exist and the type of flag(s)
 *                           set.
 * @param user_cb          - call back function after completion.
 * @param user             - user identity context, e.g. uid, gid... etc.
 */
int red_name_to_handle_at(rfs_open_hndl_t     dir_oh,
                          const char         *pathname,
                          struct file_handle *handle,
                          int                 flags,
                          rfs_usercb_t       *ucb,
                          red_api_user_t     *api_user);
/*
 * @brief Create a special file.
 */
int red_mknodat(rfs_open_hndl_t  dir_oh,
                const char      *pathname,
                mode_t           mode,
                dev_t            dev,
                rfs_open_hndl_t *new_oh,
                rfs_usercb_t    *ucb,
                red_api_user_t  *api_user);

/*
 * Methods used to read information in from directories and to
 * obtain information about files (i.e. stat).
 */
/**
 * @brief Open a directory stream for reading directory entries
 *
 * @param[in] dir_oh Open handle to the directory to read entries from
 * @param[out] dirp Pointer to store the directory stream handle
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_fdopendir(rfs_open_hndl_t   dir_oh,
                  red_dir_stream_t *dirp,
                  rfs_usercb_t     *ucb,
                  red_api_user_t   *api_user);
/**
 * @brief Close a directory stream
 *
 * @param[in] dirp Directory stream handle to close
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_closedir(red_dir_stream_t dirp, rfs_usercb_t *ucb, red_api_user_t *api_user);
/**
 * @brief Read directory entries from a directory stream
 *
 * @param[in] dirp Directory stream handle to read from
 * @param[out] dent Buffer to store the directory entries
 * @param[in] size Maximum number of entries to read
 * @param[out] dirent_cnt Pointer to store the number of entries read
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_readdir(red_dir_stream_t dirp,
                struct dirent   *dent,
                int              size,
                int             *dirent_cnt,
                rfs_usercb_t    *ucb,
                red_api_user_t  *api_user);
/**
 * @brief Read directory entries with additional inode attributes
 *
 * @param[in] dirp Directory stream handle to read from
 * @param[out] dent Buffer to store the directory entries
 * @param[in] size Maximum number of entries to read
 * @param[out] dirent_cnt Pointer to store the number of entries read
 * @param[out] iattrs Buffer to store the inode attributes for each entry
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_readdirplus(red_dir_stream_t  dirp,
                    struct dirent    *dent,
                    int               size,
                    int              *dirent_cnt,
                    red_inode_attr_t *iattrs,
                    rfs_usercb_t     *ucb,
                    red_api_user_t   *api_user);

/**
 * @brief Get directory entries with extended information
 *
 * This API provides a way for clients to obtain a large number of
 * directory entries and their stat data (for files) in a single call.
 * It's more efficient than multiple readdir/stat calls for applications
 * that need to process many directory entries.
 *
 * @param[in] dir_oh Handle to the directory to read entries from
 * @param[out] dirp Buffer to store the red_dirent64 structures
 * @param[in] count Size of the dirp buffer in bytes
 * @param[in,out] cookie In the event that all entries cannot be returned
 *                in the provided buffer, the cookie can be used to continue
 *                retrieving entries from the last position. Set to NULL
 *                to start from the beginning.
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_getdents64(rfs_open_hndl_t dir_oh,
                   void           *dirp,
                   size_t          count,
                   void          **cookie,
                   rfs_usercb_t   *ucb,
                   red_api_user_t *api_user);

/*
 * @brief This data structure is used to return data from the
 *        red_nfsreaddir() API.
 *
 * CAVEAT: there are 2 variable length fields - NFS file handle and the
 * entry name. Use red_nfsdentry_name() to get to the starting address of
 * the name and it is always NUL terminated.
 */
typedef struct
{
    uint64_t           rd_cookie; /* cookie for the next dentry */
    struct stat        rd_stat;   /* stat information for files */
    uint16_t           rd_reclen; /* Length of this nfs dentry */
    struct file_handle rd_fh;     /*file_handle*/
} red_nfsdentry_t;

/* The returned pointer may not be valid. Caller should ensure that this
 * returned address is within the buflen returned.
 */
inline red_nfsdentry_t *red_nfsdentry_next(red_nfsdentry_t *dentry)
{
    return (red_nfsdentry_t *)((char *)dentry + dentry->rd_reclen);
}

inline char *red_nfsdentry_name(red_nfsdentry_t *dentry)
{
    return (char *)dentry + offsetof(red_nfsdentry_t, rd_fh.f_handle) +
           dentry->rd_fh.handle_bytes;
}

inline size_t red_nfsdentry_reclen(size_t fhlen, size_t name_nob)
{
    size_t len = offsetof(red_nfsdentry_t, rd_fh.f_handle) + fhlen + name_nob;
    return (len + 7) / 8 * 8; /* ROUND UP */
}

/**
 * @brief This API provides a way for clients to obtain a large number of
 *        directory entries and their stat data in a single call.
 *
 * @param dir_oh[in]   Handle for directory the operation is being performed in.
 * @param buf[in, out] Pointer to the buffer the red_dirent data will be
 *                     returned in.
 * @param bufcap[in]   Allocated size of the buf in bytes.
 * @param buflen[out]  Valid data length filled in with directory entries
 * @param cookie[in]   Next starting directory entry (0 is for the
 *                     first entry, otherwise this must be a valid rd_cookie
 *                     value from a struct red_dirent object)
 * @param ucb[in, out] This is used by the client to correlate the
 *                     response in the uring to the request.
 * @param api_user     API user details.
 */
int red_nfsreaddir(rfs_open_hndl_t dir_oh,
                   void           *buf,
                   size_t          bufcap,
                   size_t         *buflen,
                   uint64_t        cookie,
                   rfs_usercb_t   *ucb,
                   red_api_user_t *api_user);
/*
 * Rename and link operations
 *
 * AT_LINK_REPLACE is not yet in Linux and is a
 * proposal (see https://lwn.net/Articles/810848/) for use with
 * linkat(). The rest of the flags for linkat() are defined in
 * /usr/include/linux/fcntl.h.
 *
 * We create a duplicate of AT_LINK_REPLACE under the RED_ macro namespace
 * until it is officially included in Linux.
 */
#ifndef AT_LINK_REPLACE
#define AT_LINK_REPLACE 0x2000 /* Replace link or file at newdir_oh */
#endif

/**
 * @brief Rename a file or directory with extended options
 *
 * @param[in] olddir_oh Open handle to the directory containing oldpath
 * @param[in] oldpath Path of the file/directory to rename, relative to olddir_oh
 * @param[in] newdir_oh Open handle to the directory where newpath will be created
 * @param[in] newpath New path for the file/directory, relative to newdir_oh
 * @param[in] flags Control flags for the rename operation:
 *                  - RENAME_NOREPLACE: fail if newpath already exists
 *                  - RENAME_EXCHANGE: atomically exchange oldpath and newpath
 *                  - RENAME_WHITEOUT: create a whiteout object at oldpath
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_renameat2(rfs_open_hndl_t olddir_oh,
                  const char     *oldpath,
                  rfs_open_hndl_t newdir_oh,
                  const char     *newpath,
                  unsigned int    flags,
                  rfs_usercb_t   *ucb,
                  red_api_user_t *api_user);
/**
 * @brief Remove a file or directory
 *
 * @param[in] dir_oh Open handle to the directory containing pathname
 * @param[in] pathname Path of the file/directory to remove, relative to dir_oh
 * @param[in] flags Control flags for the unlink operation:
 *                  - AT_REMOVEDIR: remove a directory instead of a file
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_unlinkat(rfs_open_hndl_t dir_oh,
                 const char     *pathname,
                 int             flags,
                 rfs_usercb_t   *ucb,
                 red_api_user_t *api_user);
/**
 * @brief Create a hard link to a file
 *
 * @param[in] olddir_oh Open handle to the directory containing oldpath
 * @param[in] oldpath Path of the existing file, relative to olddir_oh
 * @param[in] newdir_oh Open handle to the directory where newpath will be created
 * @param[in] newpath Path for the new hard link, relative to newdir_oh
 * @param[in] flags Control flags for the link operation:
 *                  - AT_SYMLINK_FOLLOW: follow oldpath if it's a symbolic link
 *                  - AT_LINK_REPLACE: replace newpath if it already exists
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_linkat(rfs_open_hndl_t olddir_oh,
               const char     *oldpath,
               rfs_open_hndl_t newdir_oh,
               const char     *newpath,
               int             flags,
               rfs_usercb_t   *ucb,
               red_api_user_t *api_user);
/**
 * @brief Create a symbolic link
 *
 * @param[in] targetpath Path that the symbolic link will point to
 * @param[in] linkdir_oh Open handle to the directory where the link will be created
 * @param[in] linkpath Path for the new symbolic link, relative to linkdir_oh
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_symlinkat(const char     *targetpath,
                  rfs_open_hndl_t linkdir_oh,
                  const char     *linkpath,
                  rfs_usercb_t   *ucb,
                  red_api_user_t *api_user);
/**
 * @brief Read the target of a symbolic link
 *
 * @param[in] dir_oh Open handle to the directory containing pathname
 * @param[in] pathname Path of the symbolic link to read, relative to dir_oh
 * @param[out] buf Buffer to store the target path
 * @param[in] bufsiz Size of the buffer in bytes
 * @param[out] path_len Pointer to store the actual length of the target path
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_readlinkat(rfs_open_hndl_t dir_oh,
                   const char     *pathname,
                   char           *buf,
                   size_t          bufsiz,
                   ssize_t        *path_len,
                   rfs_usercb_t   *ucb,
                   red_api_user_t *api_user);

/*
 * File locks and fallocate
 */
/**
 * @brief Apply or remove an advisory lock on an open file
 *
 * @param[in] oh Open handle to the file to lock/unlock
 * @param[in] operation Lock operation to perform (e.g., LOCK_SH, LOCK_EX, LOCK_UN)
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_flock(rfs_open_hndl_t oh,
              int             operation,
              rfs_usercb_t   *ucb,
              red_api_user_t *api_user);
/**
 * @brief Manipulate file space
 *
 * @param[in] dir_oh Open handle to the file
 * @param[in] mode Operation mode:
 *                 - 0: Allocate space
 *                 - FALLOC_FL_KEEP_SIZE: Allocate without changing file size
 *                 - FALLOC_FL_PUNCH_HOLE: Create a hole in the file
 * @param[in] offset Starting offset for the operation
 * @param[in] len Length of the region to operate on
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_fallocate(rfs_open_hndl_t dir_oh,
                  int             mode,
                  off_t           offset,
                  off_t           len,
                  rfs_usercb_t   *ucb,
                  red_api_user_t *api_user);

/*
 * Methods to read and write from files
 */
/**
 * @brief Read from a file at a specified offset
 *
 * @param[in] oh Open handle to the file to read from
 * @param[out] buf Buffer to store the read data
 * @param[in] count Number of bytes to read
 * @param[in] offset Offset in the file to start reading from
 * @param[out] bytes_read Pointer to store the number of bytes actually read
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_pread(rfs_open_hndl_t oh,
              void           *buf,
              size_t          count,
              off_t           offset,
              ssize_t        *bytes_read,
              rfs_usercb_t   *ucb,
              red_api_user_t *api_user);
/**
 * @brief Read from a file at a specified offset using I/O memory
 *
 * @param[in] oh Open handle to the file to read from
 * @param[in] iomem I/O memory handle previously allocated with red_iomem_alloc()
 * @param[in] addr Address within the I/O memory region to store the data
 * @param[in] size Number of bytes to read
 * @param[in] offset Offset in the file to start reading from
 * @param[out] bytes_read Pointer to store the number of bytes actually read
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_pread_iomem(rfs_open_hndl_t  oh,
                    red_iomem_hndl_t iomem,
                    void            *addr,
                    size_t           size,
                    off_t            offset,
                    ssize_t         *bytes_read,
                    rfs_usercb_t    *ucb,
                    red_api_user_t  *api_user);

/*
 * red_pread_part:
 *
 * oh         - The rfs_open_hndl_t returned from red_s3_open().
 * iomem      - A handle to the already mapped memory region.
 * addr       - The address to use for reading data into.
 * part_num   - The part number to read from. If this is set to UINT32_MAX the
 *              offset will be used.
 * offset     - If the part_num is set, this must be 0, otherwise it is the
 *              offset from the start of the object.
 * size       - How much data to read. This is allowed to cross parts.
 * bytes_read - How much data was actually read.
 *
 * If this is reading using a part_num, the bytes_read will be the size of the
 * part (assuming the size passed in is large enough) or the size passed in.
 *
 * If the xattr_info is not NULL, then the values will be filled in using the
 * following if the part_num is set.
 * The checksum (user.checksum_algorithm and user.checksum) and etag
 *   (user.s3_etag) are pulled from xattrs associated with the part.
 * The user.checksum_algorithm can be NONE, CRC32, CRC32C, SHA1 or
 *   SHA256. If the xattr is not set, then the
 *   red_part_info_t.xattr_info.checksum_algorithm will be set to NONE.
 * If the part_num is set UINT32_MAX, then xattr_info must be set to NULL.
 */
int red_pread_part(rfs_open_hndl_t        oh,
                   red_iomem_hndl_t       iomem,
                   void                  *addr,
                   uint32_t               part_num,
                   off_t                  offset,
                   size_t                 size,
                   ssize_t               *byte_cnt,
                   red_part_xattr_info_t *xattr_info,
                   rfs_usercb_t          *ucb,
                   red_api_user_t        *api_user);

/**
 * @brief Write to a file at a specified offset
 *
 * @param[in] oh Open handle to the file to write to
 * @param[in] buf Buffer containing the data to write
 * @param[in] count Number of bytes to write
 * @param[in] offset Offset in the file to start writing at
 * @param[out] bytes_written Pointer to store the number of bytes actually written
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_pwrite(rfs_open_hndl_t oh,
               void           *buf,
               size_t          count,
               off_t           offset,
               ssize_t        *bytes_written,
               rfs_usercb_t   *ucb,
               red_api_user_t *api_user);
/**
 * @brief Read from a file at a specified offset using vectored I/O
 *
 * @param[in] oh Open handle to the file to read from
 * @param[in,out] iov Array of I/O vectors specifying the buffers to read into
 * @param[in] iovcnt Number of I/O vectors in the array
 * @param[in] offset Offset in the file to start reading from
 * @param[in] flags Control flags for the read operation:
 *                  - RWF_HIPRI: High priority request
 *                  - RWF_DSYNC: Data synchronization
 *                  - RWF_SYNC: File synchronization
 * @param[out] bytes_read Pointer to store the number of bytes actually read
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_preadv2(rfs_open_hndl_t   oh,
                struct red_iovec *iov,
                int               iovcnt,
                off_t             offset,
                int               flags,
                ssize_t          *bytes_read,
                rfs_usercb_t     *ucb,
                red_api_user_t   *api_user);
/**
 * @brief Write to a file at a specified offset using vectored I/O
 *
 * @param[in] oh Open handle to the file to write to
 * @param[in] iov Array of I/O vectors specifying the buffers to write from
 * @param[in] iovcnt Number of I/O vectors in the array
 * @param[in] offset Offset in the file to start writing at
 * @param[in] flags Control flags for the write operation:
 *                  - RWF_HIPRI: High priority request
 *                  - RWF_DSYNC: Data synchronization
 *                  - RWF_SYNC: File synchronization
 * @param[out] bytes_written Pointer to store the number of bytes actually written
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_pwritev2(rfs_open_hndl_t   oh,
                 struct red_iovec *iov,
                 int               iovcnt,
                 off_t             offset,
                 int               flags,
                 ssize_t          *bytes_written,
                 rfs_usercb_t     *ucb,
                 red_api_user_t   *api_user);

/**
 * @brief Write to a file at a specified offset using I/O memory
 *
 * @param[in] oh Open handle to the file to write to
 * @param[in] iomem I/O memory handle previously allocated with red_iomem_alloc()
 * @param[in] addr Address within the I/O memory region containing the data
 * @param[in] size Number of bytes to write
 * @param[in] offset Offset in the file to start writing at
 * @param[out] bytes_written Pointer to store the number of bytes actually written
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_pwrite_iomem(rfs_open_hndl_t  oh,
                     red_iomem_hndl_t iomem,
                     void            *addr,
                     size_t           size,
                     off_t            offset,
                     ssize_t         *bytes_written,
                     rfs_usercb_t    *ucb,
                     red_api_user_t  *api_user);
/*
 * Methods to manipulate extended attributes.
 */
/**
 * @brief Get an extended attribute value for an open file
 *
 * @param[in] oh Open handle to the file
 * @param[in] name Name of the extended attribute to retrieve
 * @param[out] value Buffer to store the attribute value
 * @param[in] size Size of the value buffer in bytes
 * @param[out] ret_size Pointer to store the actual size of the attribute value
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_fgetxattr(rfs_open_hndl_t oh,
                  const char     *name,
                  void           *value,
                  size_t          size,
                  size_t         *ret_size,
                  rfs_usercb_t   *ucb,
                  red_api_user_t *api_user);
/**
 * @brief Set an extended attribute for an open file
 *
 * @param[in] oh Open handle to the file
 * @param[in] name Name of the extended attribute to set
 * @param[in] value Buffer containing the attribute value
 * @param[in] size Size of the value in bytes
 * @param[in] flags Control flags for the operation:
 *                  - 0: Create or replace the attribute
 *                  - XATTR_CREATE: Fail if the attribute already exists
 *                  - XATTR_REPLACE: Fail if the attribute doesn't exist
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_fsetxattr(rfs_open_hndl_t oh,
                  const char     *name,
                  const void     *value,
                  size_t          size,
                  int             flags,
                  rfs_usercb_t   *ucb,
                  red_api_user_t *api_user);
/**
 * @brief List extended attributes for an open file
 *
 * @param[in] oh Open handle to the file
 * @param[out] list Buffer to store the list of attribute names (null-separated)
 * @param[in] size Size of the list buffer in bytes
 * @param[out] ret_size Pointer to store the actual size needed for the list
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_flistxattr(rfs_open_hndl_t oh,
                   char           *list,
                   size_t          size,
                   size_t         *ret_size,
                   rfs_usercb_t   *ucb,
                   red_api_user_t *api_user);
/**
 * @brief Remove an extended attribute from an open file
 *
 * @param[in] oh Open handle to the file
 * @param[in] name Name of the extended attribute to remove
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_fremovexattr(rfs_open_hndl_t oh,
                     const char     *name,
                     rfs_usercb_t   *ucb,
                     red_api_user_t *api_user);
/**
 * @brief Remove multiple extended attributes from an open file
 *
 * @param[in] oh Open handle to the file
 * @param[in] flags Control flags specifying which attributes to remove
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_fremove_xattrs(rfs_open_hndl_t oh,
                       uint32_t        flags,
                       rfs_usercb_t   *ucb,
                       red_api_user_t *api_user);

/*
 * Methods to manipulate stat attributes.
 */
/**
 * @brief Set file attributes for a file relative to a directory
 *
 * @param[in] oh Open handle to the directory
 * @param[in] path_name Path of the file relative to the directory
 * @param[in] path_flags Path resolution flags
 * @param[in] rstat Structure containing the attributes to set
 * @param[in] stat_flags Flags specifying which attributes to set
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_fsetattrat(rfs_open_hndl_t oh,
                   const char     *path_name,
                   uint32_t        path_flags,
                   red_fs_stat_t  *rstat,
                   uint32_t        stat_flags,
                   rfs_usercb_t   *ucb,
                   red_api_user_t *api_user);

struct statvfs;
/*
 * Get File System stat
 */
/**
 * @brief Get file system statistics
 *
 * @param[in] oh Open handle to a file or directory
 * @param[out] buf Structure to store the file system statistics
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_fstatvfs(rfs_open_hndl_t oh,
                 struct statvfs *buf,
                 rfs_usercb_t   *ucb,
                 red_api_user_t *api_user);

/**
 * @brief Create a directory path
 *
 * @param[in] oh Open handle to a directory
 * @param[in] path Path of the directory to create
 * @param[in] mode Permission mode for the created directories
 * @param[in] flags Control flags for the operation
 * @param[out] new_oh Pointer to store the open handle to the created directory
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_pathmkdir(rfs_open_hndl_t  oh,
                  const char      *path,
                  mode_t           mode,
                  int              flags,
                  rfs_open_hndl_t *new_oh,
                  rfs_usercb_t    *ucb,
                  red_api_user_t  *api_user);

/*
 * Methods used to read extended attributes.
 */
/**
 * @brief Open an extended attribute stream
 *
 * @param[in] oh Open handle to the file or directory
 * @param[out] xstrp Pointer to store the extended attribute stream handle
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_open_xattr_stream(rfs_open_hndl_t     oh,
                          red_xattr_stream_t *xstrp,
                          rfs_usercb_t       *ucb,
                          red_api_user_t     *api_user) __nonnull((2, 3, 4));
/**
 * @brief Close an extended attribute stream
 *
 * @param[in] xstrp Extended attribute stream to close
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_close_xattr_stream(red_xattr_stream_t xstrp,
                           rfs_usercb_t      *ucb,
                           red_api_user_t    *api_user) __nonnull((2, 3));
/**
 * @brief Read extended attributes from a stream
 *
 * @param[in] xstrp Extended attribute stream to read from
 * @param[out] buf Buffer to store the extended attributes
 * @param[in] size Size of the buffer in bytes
 * @param[out] buf_nob Pointer to store the number of bytes written to the buffer
 * @param[out] ret_iattr_sz Pointer to store the total size of all extended attributes
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_read_xattrs(red_xattr_stream_t xstrp,
                    char              *buf,
                    size_t             size,
                    size_t            *buf_nob,
                    size_t            *ret_iattr_sz,
                    rfs_usercb_t      *ucb,
                    red_api_user_t    *api_user) __nonnull((2, 4, 6, 7));

/*
 * Security APIs
 */
/**
 * @brief Check access permissions for a file or directory
 *
 * @param[in] obj_oh Open handle to the parent directory
 * @param[in] path Path to the file or directory to check
 * @param[in] mode Access mode to check (R_OK, W_OK, X_OK, F_OK)
 * @param[in] flags Control flags for the operation
 * @param[in] ucb User callback for asynchronous completion
 * @param[in] api_user User context information
 * @return RED_SUCCESS (0) on success, otherwise values from red_status_t cast to int
 */
int red_faccessat(rfs_open_hndl_t obj_oh,
                  const char     *path,
                  int             mode,
                  int             flags,
                  rfs_usercb_t   *ucb,
                  red_api_user_t *api_user) __nonnull((2, 5, 6));

/** @}*/ /* end of RED_FS_API group */
#ifdef __cplusplus
}
#endif

#endif /* RED_FS_API_H_ */
