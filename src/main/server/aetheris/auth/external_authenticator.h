/*
 * 
 *           ______ _______ _    _ ______ _____  _____  _____ 
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___  
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | |  \___ \ 
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/ 
 * 
 *                                                                                     v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file external_authenticator.h
 *
 * @brief Defines the external authenticator interface.
 *        Handles authentication, account creation, and password resets
 *        via external routes.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef EXTERNAL_AUTHENTICATOR_H
#define EXTERNAL_AUTHENTICATOR_H

#include "authentication_system.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_ext_authenticator_t , the external authenticator vtable
 *
 * Provide a populated instance of this struct to handle external auth
 * flows. All function pointers must be non-NULL.
 *
 * For implementors: use ae_auth_request_t->context to access both the
 * request body and write the response body for each handler.
 * ====================================================================== */
struct ae_ext_authenticator {

    /*
     * Called when an external login request is made.
     */
    void (*handle_login)(
        struct ae_ext_authenticator *self,
        const ae_auth_request_t     *request);

    /*
     * Called when an external account creation request is made.
     * Use request->context to read the request body and write the
     * response body.
     */
    void (*handle_account_creation)(
        struct ae_ext_authenticator *self,
        const ae_auth_request_t     *request);

    /*
     * Called when an external password reset request is made.
     * Use request->context to read the request body and write the
     * response body.
     */
    void (*handle_password_reset)(
        struct ae_ext_authenticator *self,
        const ae_auth_request_t     *request);

    /* Implementation-specific data; opaque to callers. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macros
 * ====================================================================== */

#define ae_ext_handle_login(auth, req) \
    (auth)->handle_login((auth), (req))

#define ae_ext_handle_account_creation(auth, req) \
    (auth)->handle_account_creation((auth), (req))

#define ae_ext_handle_password_reset(auth, req) \
    (auth)->handle_password_reset((auth), (req))

#ifdef __cplusplus
}
#endif

#endif /* EXTERNAL_AUTHENTICATOR_H */
