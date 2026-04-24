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
 * @file handbook_authenticator.h
 *
 * @brief Defines the handbook authenticator interface.
 *        Handles player authentication for the web GM handbook,
 *        including page presentation and form submission handling.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef HANDBOOK_AUTHENTICATOR_H
#define HANDBOOK_AUTHENTICATOR_H

#include <stdbool.h>

#include "authentication_system.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_handbook_auth_response_t
 *
 * Holds the response data sent back to the client after an authentication
 * attempt. Mirrors the inner Response class (@Getter @Builder).
 * ====================================================================== */
typedef struct {
    int         status;  /* HTTP status code                              */
    const char *body;    /* Response body; caller owns the string         */
    bool        html;    /* true if body is HTML, false if plain text     */
} ae_handbook_auth_response_t;

/* =========================================================================
 * ae_handbook_authenticator_t , the handbook authenticator vtable
 *
 * Provide a populated instance of this struct to handle GM handbook
 * authentication. All function pointers must be non-NULL.
 * ====================================================================== */
struct ae_handbook_authenticator {

    /*
     * Invoked when the user requests to authenticate.
     * Should respond with a page that allows the user to authenticate.
     *
     * @route GET /handbook/authenticate
     */
    void (*present_page)(
        struct ae_handbook_authenticator *self,
        const ae_auth_request_t          *request);

    /*
     * Invoked when the user submits the authentication form.
     * Should respond with HTML that sends a message to the GM Handbook.
     * See the default handbook authentication page for an example.
     *
     * Writes the response into 'out'; caller is responsible for
     * freeing 'out->body' if the implementation heap-allocates it.
     */
    void (*authenticate)(
        struct ae_handbook_authenticator *self,
        const ae_auth_request_t          *request,
        ae_handbook_auth_response_t      *out);

    /* Implementation-specific data; opaque to callers. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macros
 * ====================================================================== */

#define ae_handbook_present_page(auth, req) \
    (auth)->present_page((auth), (req))

#define ae_handbook_authenticate(auth, req, out) \
    (auth)->authenticate((auth), (req), (out))

#ifdef __cplusplus
}
#endif

#endif /* HANDBOOK_AUTHENTICATOR_H */
