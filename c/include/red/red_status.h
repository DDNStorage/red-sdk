/******************************************************************************
 *
 * @file red_status.h
 * @copyright
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
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 *
 * @section DESCRIPTION
 *
 * RED status codes
 *
 ******************************************************************************/

#ifndef RED_STATUS_H_
#define RED_STATUS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Status codes returned by Red
 */
typedef enum
{
    RED_SUCCESS              = 0,
    RED_EPERM                = -1,
    RED_ENOENT               = -2,
    RED_ESRCH                = -3,
    RED_EINTR                = -4,
    RED_EIO                  = -5,
    RED_ENXIO                = -6,
    RED_E2BIG                = -7,
    RED_ENOEXEC              = -8,
    RED_EBADF                = -9,
    RED_ECHILD               = -10,
    RED_EAGAIN               = -11,
    RED_EWOULDBLOCK          = RED_EAGAIN,
    RED_ENOMEM               = -12,
    RED_EACCES               = -13,
    RED_EFAULT               = -14,
    RED_ENOTBLK              = -15,
    RED_EBUSY                = -16,
    RED_EEXIST               = -17,
    RED_EXDEV                = -18,
    RED_ENODEV               = -19,
    RED_ENOTDIR              = -20,
    RED_EISDIR               = -21,
    RED_EINVAL               = -22,
    RED_ENFILE               = -23,
    RED_EMFILE               = -24,
    RED_ENOTTY               = -25,
    RED_ETXTBSY              = -26,
    RED_EFBIG                = -27,
    RED_ENOSPC               = -28,
    RED_ESPIPE               = -29,
    RED_EROFS                = -30,
    RED_EMLINK               = -31,
    RED_EPIPE                = -32,
    RED_EDOM                 = -33,
    RED_ERANGE               = -34,
    RED_EDEADLK              = -35,
    RED_EDEADLOCK            = RED_EDEADLK,
    RED_ENAMETOOLONG         = -36,
    RED_ENOLCK               = -37,
    RED_ENOSYS               = -38,
    RED_ENOTEMPTY            = -39,
    RED_ELOOP                = -40,
    RED_ENOMSG               = -42,
    RED_EIDRM                = -43,
    RED_ECHRNG               = -44,
    RED_EL2NSYNC             = -45,
    RED_EL3HLT               = -46,
    RED_EL3RST               = -47,
    RED_ELNRNG               = -48,
    RED_EUNATCH              = -49,
    RED_ENOCSI               = -50,
    RED_EL2HLT               = -51,
    RED_EBADE                = -52,
    RED_EBADR                = -53,
    RED_EXFULL               = -54,
    RED_ENOANO               = -55,
    RED_EBADRQC              = -56,
    RED_EBADSLT              = -57,
    RED_EBFONT               = -59,
    RED_ENOSTR               = -60,
    RED_ENODATA              = -61,
    RED_ETIME                = -62,
    RED_ENOSR                = -63,
    RED_ENONET               = -64,
    RED_ENOPKG               = -65,
    RED_EREMOTE              = -66,
    RED_ENOLINK              = -67,
    RED_EADV                 = -68,
    RED_ESRMNT               = -69,
    RED_ECOMM                = -70,
    RED_EPROTO               = -71,
    RED_EMULTIHOP            = -72,
    RED_EDOTDOT              = -73,
    RED_EBADMSG              = -74,
    RED_EOVERFLOW            = -75,
    RED_ENOTUNIQ             = -76,
    RED_EBADFD               = -77,
    RED_EREMCHG              = -78,
    RED_ELIBACC              = -79,
    RED_ELIBBAD              = -80,
    RED_ELIBSCN              = -81,
    RED_ELIBMAX              = -82,
    RED_ELIBEXEC             = -83,
    RED_EILSEQ               = -84,
    RED_ERESTART             = -85,
    RED_ESTRPIPE             = -86,
    RED_EUSERS               = -87,
    RED_ENOTSOCK             = -88,
    RED_EDESTADDRREQ         = -89,
    RED_EMSGSIZE             = -90,
    RED_EPROTOTYPE           = -91,
    RED_ENOPROTOOPT          = -92,
    RED_EPROTONOSUPPORT      = -93,
    RED_ESOCKTNOSUPPORT      = -94,
    RED_ENOTSUP              = -95,
    RED_EPFNOSUPPORT         = -96,
    RED_EAFNOSUPPORT         = -97,
    RED_EADDRINUSE           = -98,
    RED_EADDRNOTAVAIL        = -99,
    RED_ENETDOWN             = -100,
    RED_ENETUNREACH          = -101,
    RED_ENETRESET            = -102,
    RED_ECONNABORTED         = -103,
    RED_ECONNRESET           = -104,
    RED_ENOBUFS              = -105,
    RED_EISCONN              = -106,
    RED_ENOTCONN             = -107,
    RED_ESHUTDOWN            = -108,
    RED_ETOOMANYREFS         = -109,
    RED_ETIMEDOUT            = -110,
    RED_ECONNREFUSED         = -111,
    RED_EHOSTDOWN            = -112,
    RED_EHOSTUNREACH         = -113,
    RED_EALREADY             = -114,
    RED_EINPROGRESS          = -115,
    RED_ESTALE               = -116,
    RED_EUCLEAN              = -117,
    RED_ENOTNAM              = -118,
    RED_ENAVAIL              = -119,
    RED_EISNAM               = -120,
    RED_EREMOTEIO            = -121,
    RED_EDQUOT               = -122,
    RED_ENOMEDIUM            = -123,
    RED_EMEDIUMTYPE          = -124,
    RED_ECANCELED            = -125,
    RED_ENOKEY               = -126,
    RED_EKEYEXPIRED          = -127,
    RED_EKEYREVOKED          = -128,
    RED_EKEYREJECTED         = -129,
    RED_EOWNERDEAD           = -130,
    RED_ENOTRECOVERABLE      = -131,
    RED_ERFKILL              = -132,
    RED_EHWPOISON            = -133,
    RED_FAILURE              = -255,
    RED_UNKNOWN              = -256,
    RED_ECRC                 = -257,
    RED_EREPLINCONSIST       = -258,
    RED_EKVINVALID           = -259,
    RED_ERSMINVALID          = -260,
    RED_ELACKCAPCAT          = -262,
    RED_ENOAGENT             = -263,
    RED_ETOOSMALL            = -264,
    RED_EOPBADSTATE          = -265,
    RED_EAVAIL               = -266,
    RED_EBADFLAGS            = -267,
    RED_ENOEQ                = -268,
    RED_EDOMAIN              = -269,
    RED_ENOCQ                = -270,
    RED_ETRUNC               = -271,
    RED_ENOAV                = -272,
    RED_EOVERRUN             = -273,
    RED_ENORECIPIENT         = -274,
    RED_ETOOMANYCOFAILS      = -275,
    RED_ERSMVERSION          = -276,
    RED_ECATEVICTED          = -277,
    RED_ECATNOTOWNED         = -278,
    RED_EALLPORTSBUSY        = -279,
    RED_EREVISION            = -280,
    RED_EBADVERSION          = -281,
    RED_ECKSUM               = -282,
    RED_EDX                  = -283,
    RED_HNDLR_EINSTNC        = -284,
    RED_HNDLR_ECATNOTMOUNTED = -285,
    RED_NET_ETIMEDOUT        = -287,
    RED_NET_EUSRCANCEL       = -288,
    RED_NET_EOPFAIL          = -289,
    RED_NET_ENOTCONN         = -290,
    RED_NET_EINPROGRESS      = -291,
    RED_HNDLR_ECATNOCAP      = -294,
    RED_HNDLR_NOTREADY       = -295,
    RED_TRG_ENOCOUNT         = -296,
    RED_TRG_ENOSET           = -297,
    RED_TRG_ENONAME          = -298,
    RED_HNDLR_EOFFLINE       = -299,
    RED_DEV_PARTITION        = -300,
    RED_EPOOLINVAL           = -301,
    RED_ELTABLEINVAL         = -302,
    RED_ENOTALEADER          = -303,
    RED_DLM_ENOTPRIME        = -304,
    RED_DLM_ERETRY           = -305,
    RED_DLM_EDUP             = -306,
    RED_DLM_ENOENT           = -307,
    RED_DLM_EBADVALUE        = -308,
    RED_DLM_UPFAIL           = -309,
    RED_DLM_EPROTO           = -310,
    RED_ENOTALIGNED          = -311,
    RED_ENOECLAYOUT          = -312,
    RED_EEBNOMETA            = -313,
    RED_EEBNMISSING          = -314,
    RED_JRPC_EPARSE          = -315,
    RED_JRPC_EREQ            = -316,
    RED_JRPC_NOMETHOD        = -317,
    RED_JRPC_EPARAMS         = -318,
    RED_JRPC_EINTERNAL       = -319,
    RED_JRPC_FAILURE         = -320,
    RED_JRPC_EAGAIN          = -321,
    RED_JRPC_NOTAUTH         = -322,
    RED_JRPC_NORESOURCE      = -323,
    RED_JRPC_NOTALLOWED      = -324,
    RED_JRPC_NOTREADY        = -325,
    RED_JRPC_ESHUTDOWN       = -326,
    RED_JRPC_EEXIST          = -327,
    RED_EBREB_NFAILS         = -328,
    RED_EBREB_NTGTS          = -329,
    RED_EBREB_IOFAIL         = -330,
    RED_EBC_BROKEN           = -331,
    RED_NET_EENCRYPT         = -332,
    RED_HNDLR_AUTHRETRY      = -333,
    RED_HNDLR_EQVEXP         = -334,
    RED_ELACK_AVAIL          = -335,
    RED_NET_ERDMALLOC        = -336,
    RED_SCR_ESTALE           = -337,
    RED_SCR_EEVICT           = -338,
    RED_SCR_ENOTREG          = -339,
    RED_NET_ENOCRED          = -340,
    RED_RFS_EPROTO           = -341,
    RED_SCR_EAGAIN           = -342,
    RED_IM_NOTREADY          = -343,
    RED_IM_ESHUTDOWN         = -344,
    RED_HNDLR_EQVPROTO       = -345,
    RED_HNDLR_EQVSTALE       = -346,
    RED_HNDLR_EQVREVOKED     = -347,
    RED_NET_EEVICTED         = -348,
    RED_HNDLR_ECANCELED      = -349,
    RED_RETRY_FORMAT         = -350,
    RED_JRPC_EBUSY           = -351,
    RED_JRPC_EDQUOT          = -352,
    RED_HNDLR_ETASKCR        = -353,
    RED_HNDLR_ETCRAGAIN      = -354,
    RED_CAT_EOFFLINE         = -355,
    RED_CAT_EDRIVER          = -356,
    RED_CAT_ECONFIG          = -357,
    RED_CAT_EMISMATCH        = -358,
    RED_CAT_EDEVEOL          = -359,
    RED_CAT_EIOMMU           = -360,
    RED_CLNT_EINSTNC         = -361,
    RED_TASK_ERTRYLMT        = -362,
    RED_HNDLR_ENOTSUP        = -363,
} red_status_t;

/**
 * @brief Return code for procedures that return >= 0 for success and -ve for
 *        failure.
 **/
typedef int red_rc_t;

/**
 * @brief Convert a red_rc_t to a red_status_t
 *        Use to determine success/failure of functions returning red_rc_t
 *
 * @param rc red_rc_t to convert
 * @return the equivalent red_status_t.
 **/
static inline red_status_t red_rc2status(red_rc_t rc)
{
    return (rc < 0) ? (red_status_t)rc : RED_SUCCESS;
}

/**
 * @brief evaluate the first error expression
 * Return it if it's not RED_SUCCESS - otherwise evaluate and return the 2nd
 * error expression
 * CAVEAT EMPTOR this relies on RED_SUCCESS == 0!!!
 */
#define RED_1STERR(rc1, rc2)                                                             \
    ({                                                                                   \
        auto _rc1 = (rc1);                                                               \
        _rc1 ? (_rc1) : (rc2);                                                           \
    })

/**
 *  @brief Convert a C errno to the equivalent red_status_t
 *         Unknown values are reported as RED_UNKNOWN
 *
 *  @param errnum the C errno
 *  @return the red_status_t code
 **/
red_status_t red_errno(int errnum);

/**
 *  @brief Convert a red_status_t to the equivalent C errno
 *         Codes that cannot be translated return 255
 *
 *  @param red_code the RED error code
 *  @return the Linux error code
 **/
int red_ret_code(red_status_t rc);

/**
 * @brief Return a C-string describing the given red_status_t
 *
 * @param rc the red_status_t
 * @return the C-string description
 **/
const char *red_strerror(red_status_t rc);

#ifdef __cplusplus
}
#endif

#endif /* RED_STATUS_H_ */
