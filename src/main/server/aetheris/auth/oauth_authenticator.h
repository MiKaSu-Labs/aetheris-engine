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
 * @file oauth_authenticator.h
 *
 * @brief Defines the OAuth authenticator interface.
 *        Handles authentication via OAuth routes including login,
 *        redirection, and token processing.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef OAUTH_AUTHENTICATOR_H
#define OAUTH_AUTHENTICATOR_H

#include "authentication_system.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_oauth_client_type_t
 *
 * The type of client making the OAuth request. Used to determine the
 * appropriate redirection target.
 * ====================================================================== */
typedef enum {
    AE_OAUTH_CLIENT_DESKTOP, /**< Desktop client. */
    AE_OAUTH_CLIENT_MOBILE,  /**< Mobile client. */
} ae_oauth_client_type_t;

/* =========================================================================
 * ae_oauth_authenticator_t -- the OAuth authenticator vtable
 *
 * Fully defines the type forward-declared as ae_oauth_authenticator_t
 * in authentication_system.h. Provide a populated instance of this
 * struct to handle OAuth flows (typically obtained or replaced via
 * ae_auth_system_t::get_oauth_authenticator). All function pointers
 * must be non-NULL.
 *
 * Concurrency: the server dispatches concurrent OAuth requests from
 * multiple threads against a single shared instance. This interface
 * provides no locking of its own -- every implementation is responsible
 * for its own internal thread-safety across all of its function pointers
 * and whatever state 'impl' points to.
 * ====================================================================== */
struct ae_oauth_authenticator {

    /**
     * @brief Handle an OAuth login request.
     * @param self    The authenticator instance. Must not be NULL.
     * @param request Request to authenticate. Must not be NULL.
     */
    void (*handle_login)(
        struct ae_oauth_authenticator *self,
        const ae_auth_request_t       *request);

    /**
     * @brief Handle a client's request to redirect to the OAuth login
     *        page.
     * @param self        The authenticator instance. Must not be NULL.
     * @param request     Request to authenticate. Must not be NULL.
     * @param client_type Whether the requesting client is desktop or
     *                    mobile; determines the redirection target.
     */
    void (*handle_redirection)(
        struct ae_oauth_authenticator *self,
        const ae_auth_request_t       *request,
        ae_oauth_client_type_t         client_type);

    /**
     * @brief Process an OAuth callback: exchange the code/token the
     *        provider redirected back with for a completed login.
     * @param self    The authenticator instance. Must not be NULL.
     * @param request Request to authenticate. Must not be NULL.
     */
    void (*handle_token_process)(
        struct ae_oauth_authenticator *self,
        const ae_auth_request_t       *request);

    /** Implementation-specific data; opaque to callers. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macros
 *
 * Thin, direct forwards to the corresponding ae_oauth_authenticator_t
 * member -- each macro performs no NULL-checking or other logic of its
 * own. auth is expanded twice in every macro below (once as the object
 * of ->, once as the first call argument), so pass a simple variable,
 * not an expression with side effects -- it would run twice.
 * ====================================================================== */

#define ae_oauth_handle_login(auth, req) \
    (auth)->handle_login((auth), (req))

#define ae_oauth_handle_redirection(auth, req, ctype) \
    (auth)->handle_redirection((auth), (req), (ctype))

#define ae_oauth_handle_token_process(auth, req) \
    (auth)->handle_token_process((auth), (req))

#ifdef __cplusplus
}
#endif

#endif /* OAUTH_AUTHENTICATOR_H */
