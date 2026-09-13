/*
 * 
 *           ______ _______ _    _ ______ _____  _____  _____ 
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (____ 
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | | |\___  \ 
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/ 
 * 
 *                                                                                     v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file ae_error.c
 *
 * @brief Implementation of the ae_error_t code table and predicates.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <aetheris/ae_error.h>

/* ============================================================ */

ae_bool ae_error_ok(ae_error_t err)
{
    return err == AE_OK;
}

ae_bool ae_error_failed(ae_error_t err)
{
    return err != AE_OK;
}

const char *ae_error_str(ae_error_t err)
{
    switch (err) {
    case AE_OK:
        return "AE_OK";
    case AE_ERR_INVALID_ARG:
        return "AE_ERR_INVALID_ARG";
    case AE_ERR_INVALID_STATE:
        return "AE_ERR_INVALID_STATE";
    case AE_ERR_OUT_OF_MEMORY:
        return "AE_ERR_OUT_OF_MEMORY";
    case AE_ERR_NOT_FOUND:
        return "AE_ERR_NOT_FOUND";
    case AE_ERR_ALREADY_EXISTS:
        return "AE_ERR_ALREADY_EXISTS";
    case AE_ERR_BUFFER_TOO_SMALL:
        return "AE_ERR_BUFFER_TOO_SMALL";
    case AE_ERR_BAD_DATA:
        return "AE_ERR_BAD_DATA";
    case AE_ERR_BAD_LENGTH:
        return "AE_ERR_BAD_LENGTH";
    case AE_ERR_OVERFLOW:
        return "AE_ERR_OVERFLOW";
    case AE_ERR_IO:
        return "AE_ERR_IO";
    case AE_ERR_TIMEOUT:
        return "AE_ERR_TIMEOUT";
    case AE_ERR_UNSUPPORTED:
        return "AE_ERR_UNSUPPORTED";
    case AE_ERR_PERMISSION:
        return "AE_ERR_PERMISSION";
    case AE_ERR_ABORTED:
        return "AE_ERR_ABORTED";
    default:
        return "AE_ERR_UNKNOWN";
    }
}

/* ============================================================ */