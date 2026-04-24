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
 * The type of client making the OAuth request.
 * Used to determine the appropriate redirection target.
 * ====================================================================== */
typedef enum {
    AE_OAUTH_CLIENT_DESKTOP,
    AE_OAUTH_CLIENT_MOBILE,
} ae_oauth_client_type_t;

/* =========================================================================
 * ae_oauth_authenticator_t , the OAuth authenticator vtable
 *
 * Provide a populated instance of this struct to handle OAuth flows.
 * All function pointers must be non-NULL.
 * ====================================================================== */
struct ae_oauth_authenticator {

    /*
     * Called when an OAuth login request is made.
     */
    void (*handle_login)(
        struct ae_oauth_authenticator *self,
        const ae_auth_request_t       *request);

    /*
     * Called when a client requests to redirect to the login page.
     * 'client_type' indicates whether the client is desktop or mobile.
     */
    void (*handle_redirection)(
        struct ae_oauth_authenticator *self,
        const ae_auth_request_t       *request,
        ae_oauth_client_type_t         client_type);

    /*
     * Called when an OAuth login requests a callback token process.
     */
    void (*handle_token_process)(
        struct ae_oauth_authenticator *self,
        const ae_auth_request_t       *request);

    /* Implementation-specific data; opaque to callers. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macros
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
