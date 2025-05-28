/******************************************************************************
 *
 * @file red_client_types.h
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
 * Definition of common C types for the RED client
 *
 ******************************************************************************/

#ifndef RED_CLIENT_TYPES_H_
#define RED_CLIENT_TYPES_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* Version information */
#define RED_CLIENT_TYPES_VERSION_MAJOR 1
#define RED_CLIENT_TYPES_VERSION_MINOR 0
#define RED_CLIENT_TYPES_VERSION_PATCH 0

#define RED_CLIENT_TYPES_VERSION                                                         \
    ((RED_CLIENT_TYPES_VERSION_MAJOR << 16) | (RED_CLIENT_TYPES_VERSION_MINOR << 8) |    \
     (RED_CLIENT_TYPES_VERSION_PATCH))

/* Compatibility macros */
#define RED_CLIENT_TYPES_VERSION_CHECK(major, minor, patch)                              \
    (RED_CLIENT_TYPES_VERSION >= ((major << 16) | (minor << 8) | (patch)))

/* Feature availability macros */
#if RED_CLIENT_TYPES_VERSION_CHECK(1, 0, 0)
#define RED_CLIENT_TYPES_HAS_QUEUE_SUPPORT
#endif

/* Deprecation macro */
#if defined(__GNUC__) || defined(__clang__)
#define RED_CLIENT_TYPES_DEPRECATED          __attribute__((deprecated))
#define RED_CLIENT_TYPES_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define RED_CLIENT_TYPES_DEPRECATED          __declspec(deprecated)
#define RED_CLIENT_TYPES_DEPRECATED_MSG(msg) __declspec(deprecated(msg))
#else
#define RED_CLIENT_TYPES_DEPRECATED
#define RED_CLIENT_TYPES_DEPRECATED_MSG(msg)
#endif

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/limits.h> /* For PATH_MAX */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <uuid/uuid.h>

#include "red_status.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup RED_CLIENT_TYPES RED Client Types
 * @{
 */

/* Special marker for null version id */
#define RFS_NULL_VERSION_ID UINT64_MAX

#define RFS_CLUSTER_NAME_MAX    36
#define RFS_TENANT_NAME_MAX     36
#define RFS_SUB_TENANT_NAME_MAX 36
/*
 * Provide a limit to how much of an S3 key is traced
 */
#define RED_S3_KEY_TRACE_LEN 25

/* Use 2MB buffers for writes */
#define RED_WRITE_BUFFER_SIZE (2 * 1048576)
#define RFS_DS_NAME_MAX       256

#define RFS_MAX_XATTR_KEY_SIZE 256

#define RFS_MAX_S3_KEY_CHARS       1025 /* Include NULL ternimator */
#define RFS_MAX_S3_UPLOAD_ID_CHARS 35   /* Include NULL terminator */
#define RFS_MIN_S3_UPLOAD_ID_CHARS 10

/* Internal flag for linkat (*at(2)) for fuse-redfs.
   Start from the largest value of the *at(2) family of syscalls defined
   in /usr/include/linux/fcntl.h */
#define RFS_AT_LINK 0x10000 /* link op from redfs */

/*
 * @brief enum to describe the different type of deferred operations
 *        supported by the RED FS Library.
 */
#define RFS_NUM_DEF_BUFFERS 256

typedef enum
{
    RFS_XATTR_BUFFER          = 0,
    RFS_XATTR_GRP_BUFFER      = 1,
    RFS_TMPFILE_CREATE_BUFFER = 2,
    RFS_WRITE_BUFFER          = 3,
} rfs_buffer_alloc_e;

#define RED_S3_USER_XATTR_MAX_SIZE 256
#define RED_S3_USER_ETAG_SIZE      33 /* c-string of 32 hex digits */
/*
 * RED_S3_USER_MPART_ETAG_SIZE is the size of the etag for multipart objects.
 * This is needed for interoperability with reds3, as reds3 has below behavior -
 * For SPU, reds3 sets MD5 checksum as ETAG value
 * For MPU, reds3 sets MD5 checksum + delimiter (-) + no of parts as ETAG value
 *
 */
#define RED_S3_USER_MPART_ETAG_SIZE \
    39                            /* 32-byte hash + 1-byte delimiter + parts count(upto 10000)*/
#define RED_S3_CONT_TOKEN_SIZE 17 /* c-string of 16 hex digits */
#define RED_S3_MAX_KEY_SIZE    (1024 + 1) /* c-string of 1024 characters */
#define RED_MD5_SIZE           16

/* TODO: Needs to be removed once reds3 deprecates these from headers */
#define RED_S3_PART_CHKSUM_TYPE_KEY "user.checksum_algorithm"
#define RED_S3_PART_CHKSUM_KEY      "user.checksum"

/* XATTR keys for checksum information */
#define RED_CHKSUM_TYPE_KEY "user.checksum_algorithm"
#define RED_CHKSUM_KEY      "user.checksum"

/* XATTR keys for ETag support */
#define RED_S3_ETAG_KEY     "user.s3_etag"
#define RED_S3_ETAG_ALG     "system.etag_algorithm"

/* XATTR Keys for S3 bucket */
#define RED_S3_BUCKET_VERSION_XATTR_KEY "system.versioning"
#define RED_S3_USER_POLICY_XATTR_KEY    "user.s3_policy"
#define RED_S3_USER_ACL_XATTR_KEY       "user.s3_acl"
#define RED_S3_USER_META_KEY            "user.s3_meta"
#define RED_S3_BUCKET_CDATE_XATTR_KEY   "user.s3_cdate"

/* Allow object rename ops on S3 Bucket */
#define RED_S3_BUCKET_RENAME_XATTR_KEY "system.s3_allow_rename"

/* Use this flag in red_s3_create_version() so that a red_close() operation
 * will not delete the tmpfile that was created
 * These flags needs to co-exist with the flags
 * defined below.. RFS_SYNC_* etc
 * and rfs_s3_filetype_t in include/cat/rfs_phys.hpp
 */
#define RED_TMPFILE_KEEP (0x30000000)
#define RED_RETENTION_CREATE (0x80000)

/**
 * @brief RED client status structure.
 */
typedef struct red_client_status
{
    red_status_t status;    /**< Status code */
    const char  *error_msg; /**< Error message */
} red_client_status_t;

/* clang-format off */
#define RED_CLIENT_SUCCESS {RED_SUCCESS, ""}
/* clang-format on */

/**
 * @brief RED dataset handle.
 *
 * This data type represents a RED dataset. It is returned by the dataset open
 * APIs and then passed to APIs that operate within a dataset.
 *
 * It is opaque and has the same size and alignment as a pointer.
 */
typedef struct
{
    void *hndl; /* /private */
} __attribute__((__packed__, __aligned__(8))) red_ds_hndl_t;

static_assert(sizeof(red_ds_hndl_t) == 8, "sizeof(red_ds_hndl_t) != 8");

/**
 * @def RED_IS_VALID_HANDLE(h)
 * Check validity of a RED object or dataset handle
 * @param h Handle to check validity
 */
#define RED_IS_VALID_HANDLE(h) ((h).hndl != NULL)

#define RED_IS_VALID_OPEN_HANDLE(h) ((h).fd != 0)

/**
 * @def RED_INVALID_HANDLE
 * @brief RED object and dataset handle initializer
 *
 * This may be used to initialize a handle declaration or to invalidate a copy
 * of a handle.
 */
/* clang-format off */
#define RED_INVALID_HANDLE {NULL}
/* clang-format on */

/* clang-format off */
#define RED_INVALID_OPEN_HANDLE {0}
/* clang-format on */

/* clang-format off */
#define RED_NO_TRANSACTION {0}
/* clang-format on */

/** Maximum number of stripes */
#define RED_MAX_STRIPES 256

/** Minimum number of directory stripes */
#define RED_DIR_STRIPES_MIN 16U

/**
 * @brief Enumeration of deduplication modes.
 */
typedef enum
{
    RED_DEDUPE_NONE,    /**< No deduplication */
    RED_DEDUPE_OFF,     /**< Deduplication off */
    RED_DEDUPE_CINLINE, /**< Chunk inline deduplication */
    RED_DEDUPE_SINLINE, /**< Stream inline deduplication */
    RED_DEDUPE_POST,    /**< Post-process deduplication */
    RED_DEDUPE_MAX      /**< Maximum deduplication mode value */
} red_dedupe_mode_e;

/**
 * @brief Enumeration of encryption modes.
 */
typedef enum
{
    RED_ENCR_MODE_NONE, /**< No encryption */
    RED_ENCR_MODE_SW,   /**< Software encryption */
    RED_ENCR_MODE_SED,  /**< Self-encrypting drive */
    RED_ENCR_MODE_ON,   /**< Encryption on */
    RED_ENCR_MODE_MAX   /**< Maximum encryption mode value */
} red_encr_mode_e;

/**
 * @brief Enumeration of compression modes.
 */
typedef enum
{
    RED_COMPRESS_MODE_NONE, /**< No compression */
    RED_COMPRESS_MODE_DRY,  /**< Dry run compression */
    RED_COMPRESS_MODE_FAST, /**< Fast compression */
    RED_COMPRESS_MODE_HIGH, /**< High compression */
    RED_COMPRESS_MODE_MAX   /**< Maximum compression mode value */
} red_compress_mode_e;

/**
 * @brief Enumeration of dataset flavors.
 */
typedef enum
{
    RED_DS_FLAVOR_NONE       = 0,   /**< No specific flavor */
    RED_DS_FLAVOR_BLOCK      = 1,   /**< Block storage flavor */
    RED_DS_FLAVOR_S3         = 2,   /**< S3 storage flavor */
    RED_DS_FLAVOR_POSIX      = 3,   /**< POSIX storage flavor */
    RED_DS_FLAVOR_S3_PREF    = 4,   /**< S3 preferred flavor */
    RED_DS_FLAVOR_POSIX_PREF = 5,   /**< POSIX preferred flavor */
    RED_DS_FLAVOR_LAST       = 5,   /**< Last valid flavor value */
    RED_DS_FLAVOR_INVALID    = 0xFF /**< Invalid flavor */
} red_ds_flavor_e;

/**
 * @brief Dataset properties structure.
 *
 * @note A RED dataset exists within a flat ASCII string namespace private to each RED
 * tenant/subtenant.  It comprises a related set of storage entities sharing a
 * given prefix in the RED global keyspace.  It resides on its given storage
 * pool where it is sharded and replicated according to its given layout table.
 *
 * @note The striping parameter, nstripes, limits the number of different
 * layouts used by the dataset and the object parameters (bucket_size,
 * block_size) set defaults for object storage within the dataset.
 *
 * @note Dataset entities may store bulk data either locally within the dataset or
 * externally using erasure encoding.  Local bulk is replicated identically to
 * the metadata entities using it while external bulk is erasure coded as
 * determined by the dataset's erasure parameters (ec_poolid, ec_ndata,
 * ec_nparity).
 */
typedef struct red_ds_props
{
    uint32_t poolid;        /**< Pool ID */
    uint32_t ltid;          /**< Layout Table ID */
    uint32_t shard0;        /**< Shard 0 */
    uint32_t nstripes;      /**< Number of stripes */
    uint32_t bucket_size;   /**< Bucket size */
    uint32_t block_size;    /**< Block size */
    uint32_t dp_profile_id; /**< Data Placement profile ID */
    uint32_t ec_nparity;    /**< Number of parity blocks for Erasure Coding */
    uint8_t  compression;   /**< Compression mode - off, fast, high */
    uint8_t  encryption;    /**< Encryption - on / off */
    uint8_t  dedupe; /**< dedupe mode - off, client-inline, server-inline, post-process */
    uint32_t dir_nstripes;  /**< Directory nstripes for all sub-directories */
    red_ds_flavor_e flavor; /**< dataset flavor */
    bool create_root;       /**< Arg - Create the "/" directory as root on the dataset */
    uint32_t
        root_nstripes;  /**< Arg - Number of stripes to create the root directory with */
    uint64_t root_uid;  /**< Arg - root's uid */
    uint64_t root_gid;  /**< Arg - root's gid */
    uint32_t root_mode; /**< Arg - root's mode */
    bool
        versioning; /**< Arg - Set versioning on the root inode to indicate a s3 bucket */
    uint32_t skipio_flags; /**< Arg - Skip I/O flags - controls read/write skipping */
} red_ds_props_t;

/*
 * @brief This data structure is used to return data from the
 *        red_getdents64() API.
 */
struct red_dirent64
{
    uint64_t       d_ino;    /* Inode number */
    off64_t        d_off;    /* Offset to next red_dirent */
    struct stat    d_stat;   /* stat information for files */
    unsigned short d_reclen; /* Length of this red_dirent */
    char           d_type;   // File type offset is (d_reclen - 1)
    char           d_name[]; /* Filename (null-terminated) */
};

/*
 * @brief Used in preadv2 and pwritev2 APIs
 */
struct red_iovec
{
    void  *iov_base;   /* Starting address */
    size_t iov_len;    /* Number of bytes to transfer */
    size_t iov_offset; /* Used for sparse or skip writes */
};

/**
 * @brief RED object handle.
 *
 * This data type represents a RED object. It is returned by the object open
 * APIs and then passed to the object I/O APIs.
 *
 * It is opaque and has the same size and alignment as a pointer.
 */
typedef struct
{
    void *hndl; /* /private */
} __attribute__((__packed__, __aligned__(8))) red_object_hndl_t;

static_assert(sizeof(red_object_hndl_t) == 8, "sizeof(red_object_hndl_t) != 8");

/** RED Volume world-wide name */
typedef uint8_t red_volume_wwn_t[16];

/**
 * @brief Properties of a RED Volume object.
 *
 * @note RED Volume objects exist within a flat ASCII string namespace private to each
 * RED dataset.  They are fixed-size and their buckets are distributed over all
 * dataset stripes. The Volume ID is unique within a dataset and fixed for the
 * lifetime of Volumr. Block and bucket sizes are set from dataset defaults and
 * all I/O must be block aligned.
 */
typedef struct red_volume_props
{
    uint32_t volumeid; /**< Volume ID */
    uint64_t nblocks;  /**< Number of blocks */
    uuid_t   uuid;     /**< Volume UUID */
    char     wwn[64];  /**< Volume wwn */
} red_volume_props_t;

/**
 * @brief RED OST object ID.
 */
typedef struct red_ostoid
{
    uint8_t bytes[16];
} red_ostoid_t;

/**
 * @brief Properties of an OST object.
 *
 * @note RED OST objects exist within a flat 16-byte binary address space private to
 * each RED dataset.  They are variable-sized and single-stripe - i.e. all the
 * buckets and metadata of one OST object are on the stripe of their owning
 * dataset determined by hashing the OST object ID.
 *
 * @note Block and bucket sizes are set from dataset defaults, but block size may be
 * reduced for finer granularity I/O.  Object size is updated on extending
 * writes (i.e. write offset + size > current object size).
 *
 * @note The OST object truncation generation number is incremented every time
 * the OST object is truncated to 0 size.
 */
typedef struct
{
    red_ostoid_t    oid;         /**< Object ID */
    uint64_t        size;        /**< Object size */
    uint64_t        granularity; /**< Object granularity */
    struct timespec atime;       /**< Time of last access */
    struct timespec mtime;       /**< Time of last modification */
    uint64_t        truncgen;    /**< OST object truncation generation */
} red_ost_object_attr_t;

/**
 * @brief RED object type discriminator.
 */
typedef enum
{
    RED_OBJECT_OST    = 0,
    RED_OBJECT_VOLUME = 1,
} red_object_type_e;

typedef struct
{
    void *hndl; /* /private */
} __attribute__((__packed__, __aligned__(8))) red_fs_inode_t;

static_assert(sizeof(red_fs_inode_t) == 8, "sizeof(red_fs_inode_t) != 8");

typedef struct
{
    void *hndl; /* /private */
} __attribute__((__packed__, __aligned__(8))) red_fs_user_t;

static_assert(sizeof(red_fs_user_t) == 8, "sizeof(red_fs_user_t) != 8");

typedef struct
{
    dev_t    st_dev;    /* ID of device containing file */
    uint32_t st_layout; /* layout where inode lives */

    uint64_t st_ino; /* inode number */

    mode_t  st_mode;  /* permission bits and file type */
    nlink_t st_nlink; /* number of hardlinks in namespace */

    uint32_t st_uid;     /* owning user id */
    uint32_t st_gid;     /* owning group id */
    uint32_t st_project; /* owning project id */
    dev_t    st_rdev;    /* device id (if special file) */

    off_t st_size; /* total size in bytes */

    blksize_t st_blksize; /* block size for file system i/o */
    blkcnt_t  st_blocks;  /* number of 512 byte blocks allocated */

    struct timespec st_atim; /* time of last access */
    struct timespec st_mtim; /* time of last modification */
    struct timespec st_ctim; /* time of last status (metadata) change */

    uint32_t st_s3type; /* s3 file information */
} red_fs_stat_t;

typedef struct
{
    void *hndl; /* /private */
} __attribute__((__packed__, __aligned__(8))) red_fs_hndl_t;

static_assert(sizeof(red_fs_hndl_t) == 8, "sizeof(red_fs_hndl_t) != 8");

/* Used for dataset capacity query and response */
typedef struct
{
    uint32_t ds_id;
} ds_id_t;

typedef struct
{
    uint64_t ds_cap;
} ds_cap_t;

/**
 * This function pointer is used to register IO completion callback functions.
 * @param rc Status code of the completed operation
 * @param cb_arg user defined argument passed to completion callback
 */
/// Asynchronous object API completion callback
typedef void (*red_object_ccb_t)(red_status_t rc, void *cb_arg);

enum rfs_noop_conf_flags_e
{
    RED_NOOP_CONF_FLAGS_RDMA = 1 << 0
};

/* Config for a single rfs noop. See rfs_noop_group_conf_t. */
typedef struct rfs_noop_conf
{
    uint32_t n_stripe;
    uint32_t n_flags; /* Additional options. None is currently implemented. */
    uint32_t n_req_size;
    uint32_t n_rep_size;
    uint64_t n_flops; /* Flops to perform on the server. */
    uint64_t n_nob;   /* Bytes to allocate on the server. */
    /* If not NONE, take a local lock in this mode. Not currently implemented. */
    uint8_t n_local;
    /* If not NONE, take a lock in this mode on the server side. Not currently
     * implemented. */
    uint8_t  n_remote;
    uint16_t n_reserved0;
    uint32_t n_reserved1;
} rfs_noop_conf_t;

static_assert(sizeof(rfs_noop_conf_t) == 40, "sizeof(rfs_noop_conf_t) != 40");

/*
 * Parameters for red_noop_group().
 *
 * This call launches a number (ng_nstripes) of concurrent tasks, each of which sends a
 * dummy RPC to a particular server.
 *
 * The RPCs will be sent to the primaries of dataset stripes [ng_1.n_stripe, ng_1.n_stripe
 * + ng_stripes).
 *
 * Each RPC carries a payload with the size randomly selected between ng_1.n_req_size
 * and ng_1.n_req_size + ng_req_range (bytes). The reply has a payload with the size
 * randomly selected between ng_1.n_rep_size and ng_1.n_rep_size + ng_rep_range.
 */
typedef struct rfs_noop_group_conf
{
    rfs_noop_conf_t ng_1;
    uint32_t        ng_nstripes;
    uint32_t        ng_req_range;
    uint32_t        ng_rep_range;
    uint32_t        ng_max_inflight;
    uint32_t        ng_nob;
} rfs_noop_group_conf_t;

/**
 * @brief Red q entity structure that identifies a queue or group via its uuid
 */
typedef struct
{
    uuid_t uuid;
} __attribute__((__packed__, __aligned__(8))) red_q_entity_t;

static_assert(sizeof(red_q_entity_t) == 16, "sizeof(red_q_entity_t) != 16");

/**
 * @brief Red queue structure that represents an open queue
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_queue_hndl_t;

static_assert(sizeof(red_queue_hndl_t) == 8, "sizeof(red_queue_hndl_t) != 8");

/**
 * @brief Red queue params structure that represents queue parameters
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_queue_params_hndl_t;

static_assert(sizeof(red_queue_params_hndl_t) == 8,
              "sizeof(red_queue_params_hndl_t) != 8");

/**
 * @brief Red q group structure that represents an open group
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_q_group_hndl_t;

static_assert(sizeof(red_q_group_hndl_t) == 8, "sizeof(red_q_group_hndl_t) != 8");

/**
 * @brief Red q attach params structure that represents attach parameters
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_q_attach_params_hndl_t;

static_assert(sizeof(red_q_attach_params_hndl_t) == 8,
              "sizeof(red_q_attach_params_hndl_t) != 8");

/**
 * @brief Red q message with a max size of BEPT_NODE_MAX_UPSERT_VAL_SIZE (=1024)
 */
typedef struct
{
    char    *rqm_data;
    uint64_t rqm_size;
} __attribute__((__packed__, __aligned__(8))) red_q_msg_t;

static_assert(sizeof(red_q_msg_t) == 16, "sizeof(red_q_msg_t) != 16");

/**
 * @brief Red q gtx
 */
typedef struct
{
    uint64_t rqt_uniq;
    uint64_t rqt_tick;
} __attribute__((__packed__, __aligned__(8))) red_q_gtx_t;

static_assert(sizeof(red_q_gtx_t) == 16, "sizeof(red_q_gtx_t) != 16");

/**
 * @brief Red q consumable
 */
typedef struct
{
    red_q_msg_t rqo_msg;
    red_q_gtx_t rqo_gtx;
    uint32_t    rqo_partition;
} __attribute__((__packed__, __aligned__(8))) red_q_consumable_t;

static_assert(sizeof(red_q_consumable_t) == 40, "sizeof(red_q_consumable_t) != 40");

/**
 * @brief Red q consumer callback
 */
typedef int (*red_q_consumer_t)(const red_q_entity_t     *group,
                                const red_q_consumable_t *consumable,
                                void                     *arg);

/**
 * @brief Red q delivery type enum
 */
typedef enum
{
    RED_QUEUE_DELIVERY_NONE = 0, /**< No consumable delivery is taking place */
    RED_QUEUE_DELIVERY_SPARSE =
        1, /**< No message payload is delivered but all other consumable members */
    RED_QUEUE_DELIVERY_FULL = 2 /**< The full consumable is delivered */
} red_q_delivery_type_e;

/**
 * @brief Red dataset handle to dataset identity
 */
typedef struct
{
    uint32_t rds_tenant_id;
    uint32_t rds_subtenant_id;
    uint32_t rds_dataset_id;
} red_ds_ident_t;

/**
 * @brief Denotes delivery of new messages
 */
/* clang-format off */
#define RED_QUEUE_DELIVERY_GTX_NEW red_q_gtx_t { UINT64_MAX, UINT64_MAX }
/* clang-format on */

/**
 * @brief Denotes delivery to any core
 */
#define RED_QUEUE_DELIVERY_CORE_ANY UINT32_MAX

/**
 * @brief Denotes the number of fifteen buckets to keep for dataset usage stats
 */
#define RFS_DATASET_USAGE_FIFTEEN_MINUTES 4

/**
 * @brief Denotes the number of hour buckets to keep for dataset usage stats
 */
#define RFS_DATASET_USAGE_HOURS 4

/**
 * @brief Red dataset usage stats
 */
typedef struct
{
    uint64_t last_updated_at_cycles;
    uint64_t fifteen_minute[RFS_DATASET_USAGE_FIFTEEN_MINUTES];
    uint64_t one_hour[RFS_DATASET_USAGE_HOURS];
} red_dataset_usage_t;

/* Versioning states for s3 buckets */
typedef enum
{
    RED_BUCKET_UNVERSIONED = 0,
    RED_BUCKET_VERSIONED   = 1,
    RED_BUCKET_SUSPENDED   = 2,
    RED_BUCKET_INVALID,
} red_s3_bucket_versioning_e;

typedef enum
{
    RED_RETENTION_MODE_NONE        = 0,
    RED_RETENTION_MODE_GOVERNANACE = 1,
    RED_RETENTION_MODE_COMPLIANCE  = 2,
} red_retention_mode_e;

typedef enum
{
    RED_RETENTION_FLAG_NONE              = 0x0,
    RED_RETENTION_FLAG_BYPASS_GOVERNANCE = 0x1,
} red_retention_flags_e;

typedef enum
{
    RED_LEGAL_HOLD_TYPE_NONE  = 0,
    RED_LEGAL_HOLD_TYPE_S3    = 1, /* AWS S3 does not have legal hold names */
    RED_LEGAL_HOLD_TYPE_NAMED = 2,
} red_legal_hold_type_e;

/*
 * RED S3 ETag and Checksum Support
 */
typedef enum
{
    RED_S3CS_NONE = 0,          /* Do not compute an ETag or Checksum */

    RED_S3CS_CRC32,             /* Use a CRC32C for the checksum */
    RED_S3CS_CRC32C,            /* Use a CRC32C for the checksum or ETag */
    RED_S3CS_SHA1,              /* Use a SHA-1 for the checksum */
    RED_S3CS_SHA256,            /* Use a SHA-256 for the checksum */
    RED_S3CS_MD5,               /* Create a MD5 digest for the ETag, S3 default */
    RED_S3CS_CRC64NVME,         /* Use a CRC64/NVMe for the checksum, S3 default */
    RED_S3CS_UUID,              /* Use a Universally Unique Identifier (UUID) for the ETag */
    RED_S3CS_GTX,               /* Use a Global Transaction Clock (GTX) for the ETag */

    RED_S3CS_TOTAL
} red_s3_checksum_type_e;

static const char *const red_s3_validator_e_strs[] =
{
    "NONE",
    "CRC32",
    "CRC32C",
    "SHA1",
    "SHA256",
    "MD5",
    "CRC64NVME",
    "UUID",
    "GTX",
    "TOTAL"
};

static inline bool rfs_is_valid_etag_type(red_s3_checksum_type_e type)
{
    return (type == RED_S3CS_MD5 ||
            type == RED_S3CS_CRC32C ||
            type == RED_S3CS_UUID ||
            type == RED_S3CS_GTX);
}

static inline bool rfs_is_valid_checksum_type(red_s3_checksum_type_e type)
{
    return (type == RED_S3CS_NONE ||
            type == RED_S3CS_CRC32 ||
            type == RED_S3CS_CRC32C ||
            type == RED_S3CS_CRC64NVME ||
            type == RED_S3CS_SHA1 ||
            type == RED_S3CS_SHA256);
}

/* Content codings are described in RFC9110 */
typedef enum
{
    RED_S3CC_PRESERVE,
    RED_S3CC_NONE,
    RED_S23CC_COMPRESS, /* not implemented */
    RED_S3CC_DEFLATE,   /* not implemented */
    RED_S3CC_GZIP       /* not implemented */
} red_s3_content_coding_e;

typedef enum
{
    RED_S3GR_READ      = 1,
    RED_S3GR_READ_ACP  = 2,
    RED_S3GR_WRITE_ACP = 4,
    RED_S3GR_FULL      = 7
} red_s3_grants_e;

typedef enum
{
    RED_S3CN_MATCH               = 1,
    RED_S3CN_NO_MATCH            = 2,
    RED_S3CN_MODIFIED_SINCE      = 4,
    RED_S3CN_UNMODIFIED_SINCE    = 8,
    RED_S3CN_MATCH_MODIFIED      = RED_S3CN_MATCH | RED_S3CN_MODIFIED_SINCE,
    RED_S3CN_MATCH_UNMODIFIED    = RED_S3CN_MATCH | RED_S3CN_UNMODIFIED_SINCE,
    RED_S3CN_NO_MATCH_MODIFIED   = RED_S3CN_NO_MATCH | RED_S3CN_MODIFIED_SINCE,
    RED_S3CN_NO_MATCH_UNMODIFIED = RED_S3CN_NO_MATCH | RED_S3CN_UNMODIFIED_SINCE
} red_s3_conditions_e;

/* File or object attributes. */
typedef enum
{
    RED_FAF_NAME    = 1 << 0,
    RED_FAF_INO     = 1 << 1, /* stat::st_ino */
    RED_FAF_LAYOUT  = 1 << 2,
    RED_FAF_FTYPE   = 1 << 3,
    RED_FAF_S3TYPE  = 1 << 4,
    RED_FAF_DIHASH  = 1 << 5,  /* Data-integrity hash */
    RED_FAF_DEV     = 1 << 6,  /* stat::st_dev */
    RED_FAF_MODE    = 1 << 7,  /* stat::st_mode */
    RED_FAF_UID     = 1 << 8,  /* stat::st_uid */
    RED_FAF_GID     = 1 << 9,  /* stat::st_gid */
    RED_FAF_SIZE    = 1 << 10, /* stat::st_size */
    RED_FAF_BLKSIZE = 1 << 11, /* stat::st_blksize */
    RED_FAF_BLOCKS  = 1 << 12, /* stat::st_blocks */
    RED_FAF_ATIME   = 1 << 13,
    RED_FAF_MTIME   = 1 << 14,
    RED_FAF_CTIME   = 1 << 15,
    RED_FAF_VERSION = 1 << 16, /* S3 version identifier. */
    RED_FAF_REXPIRY = 1 << 17, /* S3 restore expiry date. */
    RED_FAF_SCLASS  = 1 << 18, /* S3 storage class. */
    RED_FAF_COOKIE  = 1 << 19, /* Cookie to restart readdir from this entry. */
    RED_FAF_ETAG    = 1 << 20  /* S3 etag */
} red_file_attr_fields_e;

/* RED open-stat and list-objects flags. */
enum
{
    RFS_KV_OBJ            = 0x00800000, /* KV Object identifier in rfs_openinst_t */
    RFS_IS_O_PART         = 0x01000000, /* Part object operation */
    RFS_SYNC_RM_ALL_XATTR = 0x02000000, /* remove all xatrrs in blocking call */
    RFS_GET_OATTR         = 0x04000000, /* Fetch additional object attributes. (Not implemented.) */
    RFS_SKIP_DMS          = 0x08000000, /* Skip delete markers in red_s3_list_objects() output. */
    RFS_USE_S3_KEY        = 0x10000000, /* Use flat path dentry namespace for readdir. */
    RFS_USE_S3_PATH       = 0x20000000, /* Use S3 altered path */
    RFS_LIST_MPU          = 0x40000000, /* For ListObjects this flag means ListMultipartUploads. */
    RFS_GET_IATTR         = 0x80000000, /* Fetch additional inode attributes. */
};

/**
 * @brief Length of the upload ID used in multipart uploads
 */
#define RFS_UPLOAD_ID_LEN 44

/*
 * Structure passed to fstat(), getattr(), readdir(), list_objects(), etc. to
 * specify which attributes the caller is interested in.
 *
 * The server must return attrs with attrs::a_flags, such that (a_flags & f_need) ==
 * f_need.
 */
typedef struct
{
    /* The client must have these attributes. */
    uint64_t f_need;
    /*
     * These attributes are nice to have if the server does not have to do
     * significant additional work to obtain them.
     */
    uint64_t f_nice;
} flags_t;

/* Object or file attributes. */
typedef struct
{
    uint8_t  a_version; /* Version of this struct. */
    uint8_t  a_dihash_len;
    uint16_t a_name_len; /* Length of name, without the trailing 0. */
    uint16_t a_cookie_len;
    uint8_t  a_ftype; /* Object type. One of DT_* constants. */
    uint8_t  a_padding1;
    uint64_t a_flags;  /* Specifies valid fields. */
    uint64_t a_ino;    /* Object inode number. */
    uint32_t a_layout; /* Object layout. */
    uint32_t a_s3type; /* A combination of rfs_s3_filetype_t bits. */
    uint64_t a_dev;
    uint32_t a_mode;
    uint32_t a_padding2;
    uint64_t a_uid;
    uint64_t a_gid;
    uint64_t a_size;
    uint64_t a_blksize;
    uint64_t a_blocks;
    uint64_t a_atime;      /* red_fstime_t */
    uint64_t a_mtime;      /* red_fstime_t */
    uint64_t a_ctime;      /* red_fstime_t */
    uint64_t a_version_id; /* Object version id */
    uint64_t a_rexpiry;    /* red_fstime_t */
    char     a_etag[RED_S3_USER_MPART_ETAG_SIZE];
    /* A buffer with \0 separated c-strings, of size a_name_len + a_dihash_len +
     * a_cookie_len + 3 */
    uint8_t a_strings[0];
} __attribute__((__packed__, __aligned__(8))) attrs_t;

typedef union
{
    uint32_t crc32;      /* Value of CRC32 or CRC32C */
    uint32_t sha1[5];    /* SHA1 digest */
    uint8_t  sha256[32]; /* SHA256 data */
} red_s3_checksum_u;

typedef struct
{
    red_s3_checksum_type_e type;
    red_s3_checksum_u      checksum;
} red_s3_checksum_t;

static const char *const red_s3_bucket_versioning_map[] = {
    [RED_BUCKET_UNVERSIONED] = "unversioned",
    [RED_BUCKET_VERSIONED]   = "versioned",
    [RED_BUCKET_SUSPENDED]   = "suspended",
    [RED_BUCKET_INVALID]     = "invalid",
};

static const size_t RED_MAX_VERSIONING_STR_LEN = sizeof("unversioned") + 1;

/*
 * struct used to hold know xattrs that are stored as a bitmap
 * in the inode_attr_t for a file or object
 */
struct rfs_xattr_index_t
{
    const char *xi_key;
    uint32_t    xi_index;
};

/**
 * @brief generic RED callback function that accepts return status
 *        and a void pointer argument.
 */
typedef void (*rfs_two_arg_cbp_t)(red_status_t rs, void *ucbarg);

/**
 * @brief Structure used in API calls to call back the given function
 *        with the given argument upon completion of the operation.
 */
typedef struct rfs_usercb
{
    rfs_two_arg_cbp_t ucb_fun;
    void             *ucb_arg;
    uint64_t          ucb_e2e; /* e2e request tag */
} rfs_usercb_t;

/**
 * @brief Structure used by red_client_lib_poll() to retrieve the callback
 *        function and the status of one completed operation.
 */
typedef struct rfs_usercomp
{
    rfs_two_arg_cbp_t ucp_fun; /* user callback */
    void             *ucp_arg; /* user callback arg */
    red_status_t      ucp_res; /* status of the completed operation */
} rfs_usercomp_t;

/**
 * @brief Data structure that represents an open session
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_session_hndl_t;

static_assert(sizeof(red_session_hndl_t) == 8, "sizeof(red_session_hndl_t) != 8");

/**
 * @brief Structure used to pass in the uid, gid, project, tenant,
 *        sub-tenant and cluster to the RED FS APIs.
 *
 * NOTE: This will always be copied by the APIs so it can be reused
 *       after the API call is made.
 */
typedef struct
{
    uint16_t rfs_version;
    uint16_t rfs_reserved;
    uint32_t rfs_project;
    uint64_t rfs_request_id; /* reds3 server fills this with unique id.
                              * This is used by redfs to track a request
                              * from top to bottom in the stack.
                              */
    uint64_t rfs_uid;        /* Cannot use red_principal_t in a C-facing interface. */
    uint64_t rfs_gid;
    uint32_t rfs_tenid;
    uint32_t rfs_subid;
    char    *rfs_tenname;
    char    *rfs_subname;
    char    *rfs_cluster; /* needed to open buckets for now, may go to rfs_session eventually
                        */
    red_session_hndl_t rfs_session;
} red_api_user_t;

/**
 * @brief Non-owning memory buffer.
 *
 * Use it instead of passing [pointer, size] pair of parameters around.
 */
typedef struct red_api_buf_t
{
    int32_t  b_size;
    uint8_t *b_data;
} red_api_buf_t;

/**
 * @brief Data structure that represents an open file
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    uint64_t fd;
} __attribute__((__packed__, __aligned__(8))) rfs_open_hndl_t;

static_assert(sizeof(rfs_open_hndl_t) == 8, "sizeof(rfs_open_hndl_t) != 8");

/**
 * @brief Data structure that represents an open dataset
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) rfs_dataset_hndl_t;

static_assert(sizeof(rfs_dataset_hndl_t) == 8, "sizeof(rfs_dataset_hndl_t) != 8");

/**
 * @brief Data structure that represents an directory stream (the RED FS API
 *        equivalent of the POSIX DIR)
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_dir_stream_t;

static_assert(sizeof(red_dir_stream_t) == 8, "sizeof(red_dir_stream_t) != 8");

/**
 * @brief Data structure that represents an xattr stream.
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_xattr_stream_t;

static_assert(sizeof(red_xattr_stream_t) == 8, "sizeof(red_xattr_stream_t) != 8");

/**
 * @brief Data structure that represents an search xattr stream.
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_srch_xattr_stream_t;

static_assert(sizeof(red_xattr_stream_t) == 8, "sizeof(red_srch_xattr_stream_t) != 8");

/**
 * @brief Data structure that represents an list entry returned by rfs_s3_ordered_readdir
 */
typedef struct
{
    char path[PATH_MAX];
} __attribute__((__packed__, __aligned__(8))) red_ordered_list_entry_t;

static_assert(sizeof(red_ordered_list_entry_t) == PATH_MAX,
              "sizeof(red_ordered_list_entry_t) != PATH_MAX");

/**
 * Data structure that represents an element returned from red_s3_list_objects().
 *
 * A common prefix has ::le_s3type of RFS_COMMON_PREFIX, and all other fields
 * except ::le_key[] zeroed.
 */
typedef struct
{
    uint32_t le_s3type;        /* A combination of rfs_s3_filetype_t bits. */
    uint32_t le_layout;        /* Object layout. */
    uint64_t le_inum;          /* Object inode number. */
    uint8_t  le_ftype;         /* Object type. One of DT_* constants. */
    char     le_key[PATH_MAX]; /* 0-terminated object key. */
} __attribute__((__packed__, __aligned__(8))) red_s3_list_objects_entry_t;

static_assert(sizeof(red_s3_list_objects_entry_t) == ((4 + 4 + 8 + 1 + PATH_MAX + 7) & ~7),
              "!sizeof(red_s3_list_objects_entry_t)");

/**
 * Data structure that represents an element returned from red_s3_list_objects().
 * adds more atrributes returned as part of list
 *
 * A common prefix has ::le_s3type of RFS_COMMON_PREFIX, and all other fields
 * except ::le_key[] zeroed.
 */
typedef struct
{
    uint16_t le_version; /* Version of this request */
    uint32_t le_s3type;  /* A combination of rfs_s3_filetype_t bits. */
    uint32_t le_layout;  /* Object layout. */
    uint64_t le_inum;    /* Object inode number. */
    uint64_t le_mtime;
    uint64_t le_size;
    uint8_t  le_ftype; /* Object type. One of DT_* constants. */
    size_t   le_this_size;
    char    *le_key;
    char    *le_owner_id;
    char    *le_display_name;
    char     le_etag[RED_S3_USER_MPART_ETAG_SIZE];
    char     le_info[1]; /* 8 byte aligned 0-terminated object key+owner_id+display name. */
} red_s3_list_objects_entry_v2_t;

/**
 * @brief Data structure that represents dataset name as an array element to capacity API
 */
typedef struct
{
    char ds_name[RFS_DS_NAME_MAX];
} __attribute__((__packed__, __aligned__(8))) red_dataset_name_entry_t;

static_assert(sizeof(red_dataset_name_entry_t) == RFS_DS_NAME_MAX,
              "sizeof(red_dataset_name_entry_t) != RFS_DS_NAME_MAX,");

/**
 * @brief Data structure that represents an red_bucket_stream_t stream
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_bucket_stream_t;

static_assert(sizeof(red_bucket_stream_t) == 8, "sizeof(red_bucket_stream_t) != 8");

/**
 * @brief Data structure that represents an SQL statement
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_sql_stmt_hndl_t;

static_assert(sizeof(red_sql_stmt_hndl_t) == 8, "sizeof(red_sql_stmt_hndl_t) != 8");

/**
 * @brief Data structure that represents a net_rdma_t class.
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    void *hndl;
} __attribute__((__packed__, __aligned__(8))) red_iomem_hndl_t;

static_assert(sizeof(red_iomem_hndl_t) == 8, "sizeof(red_iomem_hndl_t) != 8");

/*
 * @brief Definition of the entry returned for an S3 object for the
 *        red_s3_read_versions() API
 */
typedef struct
{
    uint64_t version;
    bool     is_versioned;
    bool     is_delete_marker;
} red_s3_ver_elem_t;

/*
 * @brief red_s3_bucket_t data structure
 */
typedef struct
{
    size_t      elem_size;
    uint32_t    tenid;
    uint32_t    subid;
    const char *cluster;
    const char *tenname;
    const char *subname;
    const char *bucket;
    char        strings[1];
} red_s3_bucket_t;

/*
 * @brief structure used to query quota limits/capacities
 */
struct red_ds_capacity
{
    uint64_t tenant_capacity;
    uint64_t subtenant_capacity;
    uint64_t dataset_capacity;
};

/*
 * @brief structure used to query inode attributes.
 * From v1_inode_attr_t, should be kept in sync.
 */
typedef struct
{
    mode_t red_mode; /* permissions+type */

    uint64_t red_uid; /* Cannot use red_principal_t in a C-facing interface. */
    uint64_t red_gid;
    uint32_t red_s3type;
    uint32_t red_nlink;  /* namespace references */
    uint64_t red_crtime; /* creation time */
    uint64_t red_atime;  /* Last access time */
    uint64_t red_mtime;  /* time, when data was modified */
    uint64_t red_ctime;  /* time, when attributes got modfication */
    uint64_t red_size;   /* file size */
    uint64_t red_xattrs;
    uint64_t red_s3_version; /* current version of the S3 object. */
    uint64_t red_vinum;
    uint32_t red_vlayout;
    uint32_t red_num_parts;
    uint32_t red_current_nstripes;
    /*
     *
     * For Object Namespace iattrs, this is set to 0.
     */
    uint64_t red_prev_inum;
    /*
     *
     * Back pointer to the Object Namespace inum used in the ekey.
     */
    uint64_t red_ninum;
} red_inode_attr_t;

typedef struct
{
    size_t key_len;
    size_t value_len;
    char   key[RFS_MAX_XATTR_KEY_SIZE];
    char   value[1];
} red_xattr_entry_t;

typedef struct
{
    bool     is_object;
    uint64_t obj_version;
    uint64_t obj_inum;
    size_t   name_len;
    char     obj_name[1];
} red_xattr_kinfo_t;

/* Maximum parts for s3 */
#define RED_S3_MAX_PARTS 10000
/* Maximum parts per complete_multipart_upload request */
#define RED_MAX_PARTS_PER_COMP_REQ 1000
/*
 * Multipart upload part entry
 */
typedef struct
{
    rfs_open_hndl_t part_hndl;
    size_t          size;
    off_t           offset;
    uint32_t        part_id;
} red_part_t;

/*
 * Multipart upload part entry v2
 */
typedef struct
{
    char     sp_etag[RED_S3_USER_ETAG_SIZE];
    uint32_t sp_part_id;
} red_s3_part_t;

/*
 * Data buffer, standalone or within the extent of iomem region
 * If buffer belongs to iomem region (iomem is not null) then it must fit within the
 * extent of the region
 */
typedef struct
{
    red_iomem_hndl_t iomem; /* optional region preallocated by red_iomem_alloc(), or NULL */
    void            *addr;  /* start address of the data buffer */
    size_t           size;  /* size of the buffer in bytes */
} red_buffer_t;

/*
 * Content fields
 */
typedef struct
{
    const char             *disposition; /* optional */
    red_s3_content_coding_e coding;
    const char             *language; /* optional */
    size_t                  length;   /* object length for red_s3_put(), size of first chunk from
                         red_s3_get() */
    const char *type;                 /* optional */
} red_s3_content_t;

/*
 * byte range
 */
typedef struct
{
    off_t  offset;
    size_t size;
} red_s3_range_t;

/*
 * Match conditions for the red_s3_get() API
 */
typedef struct
{
    unsigned   flags; /* one of red_s3_conditions_e */
    const char etag[RED_S3_USER_ETAG_SIZE];
    uint64_t   timestamp;
} red_s3_conditions_t;

/*
 * Used to return the etag for the s3 object (or part) and the checksum if
 * one is specified.
 */
typedef struct
{
    red_s3_checksum_t checksum;                    /* Checksum type and value */
    char              etag[RED_S3_USER_ETAG_SIZE]; /* ETag for the S3 object or part */
} red_data_integrity_t;

/**
 * red_s3_get_params_t is used to pass optional parameters and get results in red_s3_get()
 */
typedef struct
{
    bool retrieve_checksum;
    /**
     * in - part number or 0 if not provided
     * out - total number of parts if multipart
     */
    uint32_t       part_number;
    red_s3_range_t range; /* set size to 0 if not provided */
    /**
     * in - version id to retrieve or 0 for the current
     * out - version id of retrieved object
     */
    uint64_t            version_id;
    red_s3_conditions_t conditions;
    red_s3_content_t    response_content;
    uint64_t            response_expires; /* set to 0 if not provided */
    red_s3_checksum_t   response_checksum;
    uint64_t            response_last_modified;
    char                etag[RED_S3_USER_MPART_ETAG_SIZE];
    size_t              content_length;
    bool                delete_marker;
} red_s3_get_params_t;

/**
 * red_s3_put_params_t is used to pass parameters to red_s3_put()
 * red_s3_put() is only used for SPU writes
 * For MPU, there are calls as red_s3_multipart_upload()
 */
typedef struct
{
    const char          *acl;
    red_s3_content_t     content;
    uint64_t             expires; /* set to 0 if not provided */
    red_s3_checksum_t    checksum;
    unsigned             grants; /* combination of red_s3_grants_e values */
    red_data_integrity_t ret_data_integrity;
    uint64_t             ret_version_id;
} red_s3_put_params_t;

typedef struct
{
    red_s3_checksum_type_e checksum_type; /* NONE | CRC32 | CRC32C | SHA1 | SHA256 */
    red_s3_checksum_u      checksum;      /* S3 part checksum value */
    uint32_t               etag_size;     /* Number of chars in etag */
    char                   etag[0];       /* variable size ETag follows */
} red_part_xattr_info_t;

typedef struct
{
    uint32_t              part_num;      /* S3 part number */
    size_t                size;          /* part size */
    off_t                 offset;        /* part offset */
    uint64_t              last_modified; /* creation time when part is uploaded */
    red_part_xattr_info_t xattr_info;    /* S3 part xattr */
} red_part_info_t;

/*
 * Used to return the etag for the multipart object and the checksum if
 * one is specified.
 */
typedef struct
{
    red_s3_checksum_t checksum;                          /* Checksum type and value */
    char              etag[RED_S3_USER_MPART_ETAG_SIZE]; /* ETag for the mp object */
} red_mp_info_t;

/*
 * Used to return the part information for the red_list_parts_v2() API
 * that is used for the ListParts request.
 */
typedef struct
{
    uint32_t       pi_part_num;      /* S3 part number */
    red_s3_range_t pi_range;         /* offset and size */
    uint64_t       pi_last_modified; /* creation time when part is uploaded */
    red_mp_info_t  pi_xattr_info;    /* S3 part xattr */
} red_part_info_v2_t;

/*
 * Return data structures for the red_list_mpart_uploads()
 */
typedef struct
{
    uint16_t prefix_size;
    char     prefix[0];
} red_prefix_t;

typedef struct
{
    /*
     * the total size of all of the prefixes including the null termination
     * and the uint16_t for the prefix size
     */
    size_t   prefixes_nob;
    uint32_t num_prefixes;

    /* packed strings of prefixes */
    red_prefix_t prefix[0];
} red_common_prefixes_t;

typedef struct
{
    uint32_t               upl_s3type; /* Only RFS_COMMON_PREFIX is used at the moment. */
    red_s3_checksum_type_e upl_checksum_type;
    uint64_t               upl_timestamp;
    uint64_t               upl_owner;
    uint64_t               upl_initiator;
    char                   upl_upload_id[RFS_UPLOAD_ID_LEN];
    char                   upl_s3_key[PATH_MAX];
} red_mp_upload_t;

typedef struct
{
    /* The number of red_mp_upload_t structures returned */
    uint32_t mpu_num_uploads;

    /* Are there more red_mp_upload_t structures to return */
    bool mpu_is_truncated;

    red_common_prefixes_t *mpu_prefixes;
    red_mp_upload_t       *mpu_uploads;
    size_t                 mpu_key_marker_nob;
    char                  *mpu_key_marker;
    size_t                 mpu_next_key_marker_nob;
    char                  *mpu_next_key_marker;
    size_t                 mpu_upload_id_marker_nob;
    char                  *mpu_upload_id_marker;
} red_mp_uploads_t;

/* DEPRECATED: Used rfs_dataset_hndl_t instead */
typedef rfs_dataset_hndl_t red_dataset_hndl_t;

/* DEPRECATED: Used rfs_open_hndl_t instead */
typedef rfs_open_hndl_t red_open_hndl_t;

/* User arguments passed down to carry our upward traversal operation in a dataset */
typedef struct
{
    uint32_t layout;
    uint64_t inum;
    /* TODO use red_buffer_t here */
    void   *usr_addr;
    size_t  buf_size;
    size_t *ret_size;
} uplink_args_t;

typedef struct
{
    uint64_t mue_timestamp;
    uint64_t mue_owner;
    uint64_t mue_initiator;
    char     mue_key[RED_S3_MAX_KEY_SIZE];
    char     mue_upload_id[RFS_UPLOAD_ID_LEN];
} multipart_upload_entry_t;

/**
 * Common Request headers that provides the request params to red_s3_*() APIs
 * These are grouped as "search" and "data_protection" headers
 *  "search" headers contain the fields as search criteria for filling response,
 *  -  For eg. if_match[RED_S3_USER_ETAG_SIZE] determines the search based on the passed
 * in "etag" "data protection" headers determine the checksum, and customer algorithm,
 * keys etc.
 */
typedef struct
{
    uint16_t version;                                // version for this struct
    char     if_match[RED_S3_USER_MPART_ETAG_SIZE];  // Etag if-match, taking max value of
                                                     // multipart etag size
    char if_none_match[RED_S3_USER_MPART_ETAG_SIZE]; // Etag no match, taking max value of
                                                     // multipart etag size
    uint32_t       part_num;                         // part number
    uint64_t       version_id;                       // specific version id
    uint64_t       if_modified_since;                // modified since the specified time
    uint64_t       if_unmodified_since;              // not modified since the specified time
    red_s3_range_t range;                            // range
} red_s3_search_headers_t;

typedef struct
{
    uint16_t    version;               // version for this struct
    bool        checksum_mode;         // it should be true to retrieve the checksum
    const char *sse_cust_algorithm;    // x-amz-server-side-encryption-customer-algorithm
    const char *sse_cust_key;          // x-amz-server-side-encryption-customer-key
    const char *sse_cust_key_md5;      // x-amz-server-side-encryption-customer-key-MD5
    const char *expected_bucket_owner; // x-amz-expected-bucket-owner
} red_s3_data_protection_headers_t;

typedef enum
{
    RED_S3_ACL_PERMISSION_READ_ACCESS = 0,
    RED_S3_ACL_PERMISSION_WRITE_ACCESS,
    RED_S3_ACL_PERMISSION_READ_ACP,
    RED_S3_ACL_PERMISSION_WRITE_ACP,
    RED_S3_ACL_PERMISSION_FULL_CONTROL,
} red_s3_acl_permission_t;

typedef struct
{
    char *id;
    char *display_name;
} red_s3_acl_grantee_t;

typedef struct
{
    uint16_t                version; // version for this struct
    red_s3_acl_grantee_t    grantee;
    red_s3_acl_permission_t permission;
} red_s3_acl_headers_t;

/**
 * red_s3_list_multipart_params_t is used to pass and get parameters to/from
 * red_s3_list_multipart_uploads()
 */
typedef struct
{
    const char               *lmp_prefix;
    const char               *lmp_key_marker;
    const char               *lmp_upload_id_marker;
    const int                 lmp_max_uploads;
    multipart_upload_entry_t *lmp_uploads;
    unsigned                  lmp_num_uploads;
    bool                      lmp_is_truncated;
    char                     *lmp_next_key_marker;
    char                     *lmp_next_upload_id_marker;
} red_s3_list_multipart_params_t;

/**
 * red_s3_object_headers_t is to pass the headers request to red_s3_head_object() API
 */
typedef struct
{
    uint16_t                         version; // version for this struct
    red_s3_search_headers_t          ho_search_headers;
    red_s3_data_protection_headers_t ho_dp_headers;
} red_s3_object_headers_t;

/**
 * red_s3_object_info_t is used to get object info from red_s3_head_object()
 */
typedef struct
{
    char     oi_etag[RED_S3_USER_MPART_ETAG_SIZE]; // taking max value of multipart etag size
    uint64_t oi_version_id;
    size_t   oi_size;
    uint64_t oi_last_modified;
    unsigned oi_parts_count;
    bool     oi_delete_marker;
    red_s3_checksum_t oi_checksum;
} red_s3_object_info_t;

/**
 * red_s3_list_objects_params_t is used to pass parameters to red_s3_list_objects_v2
 */
typedef struct
{
    size_t      lop_max_keys;
    const char *lop_prefix;
    const char *lop_delimiter;
    const char *lop_cont_token;
    const char *lop_start_after;
    size_t      lop_contents_size;
    attrs_t    *lop_contents;
} red_s3_list_objects_params_t;

/**
 * red_s3_list_objects_result_t is used to get response from red_s3_list_objects_v2
 */
typedef struct
{
    bool   lor_is_truncated;
    size_t lor_key_count;
    char   lor_next_cont_token[RED_S3_CONT_TOKEN_SIZE];
} red_s3_list_objects_result_t;

typedef struct
{
    red_s3_bucket_versioning_e bi_versioning;
} red_bucket_info_t;

typedef struct
{
    uint16_t  version;
    uint64_t *dst_version;
} red_s3_copy_object_response_t;

typedef struct
{
    uint16_t    version;
    uint64_t    src_version;
    const char *tgt_bucket;
    const char *tgt_key;
    const char *src_bucket;
    const char *src_key;
} red_s3_copy_object_request_t;

typedef struct
{
    uint16_t                      s3co_version;
    red_s3_copy_object_request_t  s3co_req;
    red_s3_copy_object_response_t s3co_rsp;
} red_s3_copy_object_t;

/*
 * Structure to return information about a particular object
 */
typedef struct
{
    uint64_t version;
    uint64_t obj_inum;
    uint64_t ver_inum;
    uint32_t layout;
    bool     is_multipart;
    uint32_t num_parts;
    bool     is_delete_marker;
} red_obj_info_t;

/*
 * KV API related structures
 */

/**
 * @brief Data structure that represents an open transaction.
 *
 * An opaque 8 byte struct
 */
typedef struct
{
    uint64_t transaction_id;
} __attribute__((__packed__, __aligned__(8))) red_transaction_t;

static_assert(sizeof(red_transaction_t) == 8, "sizeof(red_transaction_t) != 8");

typedef struct
{
    uint64_t   id;
    time_t     start_time;
    size_t     name_len;
    const char name[];
} red_transaction_info_t;

typedef struct
{
    red_iomem_hndl_t  iomem;
    void             *addr;   /* start address of the data buffer */
    off_t             offset; /* Offset from the start of the buffer */
    size_t            size;   /* size of the buffer in bytes */
} red_sg_elem_t;

typedef struct
{
    size_t         val_size;
    size_t         num_elems; /* How many red_sg_elem_t in the array */
    red_sg_elem_t *sg_elem;
} red_sg_list_t;

/**
 * @brief Data structure used to send an "array" of keys for a batch GET
 *        operation.
 */
typedef struct
{
    char  *key;
    size_t key_len;
} red_key_t;

/**
 * @brief Data structure used to return groups of results for an array of
 *        keys.
 *
 *  - Each key response can have its own result.
 *  - The caller is reponsible for providing sufficient memory in the scattr/
 *    gather list to return the data read from the key.
 *  - The offset is where the data GET begins from within the object.
 *  - get_size is how many bytes to read from the object.
 */
typedef struct
{
    int                  result;
    char                *key;
    size_t               key_len;
    off_t                offset;
    size_t               get_size;
    red_sg_list_t        sg_list;
    red_data_integrity_t checksums_out; /* Optional array to store checksums */
} red_kv_batch_results_t;

/** @} */ /* end of RED_CLIENT_TYPES group */

#ifdef __cplusplus
}
#endif

#endif /* RED_CLIENT_TYPES_H_ */
