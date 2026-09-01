/*
 *
 *           ______ _______ _    _ ______ _____  _____  _____
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | |  \___ \
 *  / ____ \| |____   | |  | |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/
 *
 *                                                             v1.0
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
 * ae_ext_authenticator_t -- the external authenticator vtable
 *
 * Fully defines the type forward-declared as ae_ext_authenticator_t in
 * authentication_system.h. Provide a populated instance of this struct
 * to handle external auth flows (typically obtained or replaced via
 * ae_auth_system_t::get_external_authenticator). All function pointers
 * must be non-NULL.
 *
 * For implementors: each handler reads the request body and writes the
 * response body through request->context; see ae_http_context_t for
 * that API. None of these handlers take a separate output parameter.
 *
 * Concurrency: the server dispatches concurrent external auth requests
 * from multiple threads against a single shared instance. This interface
 * provides no locking of its own -- every implementation is responsible
 * for its own internal thread-safety across all of its function pointers
 * and whatever state 'impl' points to.
 * ====================================================================== */
struct ae_ext_authenticator {

    /**
     * @brief Handle an external login request.
     * @param self    The authenticator instance. Must not be NULL.
     * @param request Request to authenticate. Must not be NULL.
     */
    void (*handle_login)(
        struct ae_ext_authenticator *self,
        const ae_auth_request_t     *request);

    /**
     * @brief Handle an external account-creation request.
     * @param self    The authenticator instance. Must not be NULL.
     * @param request Request to authenticate. Must not be NULL.
     */
    void (*handle_account_creation)(
        struct ae_ext_authenticator *self,
        const ae_auth_request_t     *request);

    /**
     * @brief Handle an external password-reset request.
     * @param self    The authenticator instance. Must not be NULL.
     * @param request Request to authenticate. Must not be NULL.
     */
    void (*handle_password_reset)(
        struct ae_ext_authenticator *self,
        const ae_auth_request_t     *request);

    /** Implementation-specific data; opaque to callers. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macros
 *
 * Thin, direct forwards to the corresponding ae_ext_authenticator_t
 * member -- each macro performs no NULL-checking or other logic of its
 * own. auth is expanded twice in every macro below (once as the object
 * of ->, once as the first call argument), so pass a simple variable,
 * not an expression with side effects -- it would run twice.
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
