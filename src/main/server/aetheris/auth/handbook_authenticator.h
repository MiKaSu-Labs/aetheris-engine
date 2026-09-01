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
 * Holds the response data sent back to the client after an
 * authentication attempt.
 * ====================================================================== */
typedef struct {
    /** HTTP status code. */
    int status;

    /**
     * Response body.
     *
     * Ownership is conditional on the specific authenticate()
     * implementation that populated it: the caller must free it if and
     * only if that implementation heap-allocated it (see the
     * authenticate() member below). This struct carries no flag
     * indicating which applies for a given implementation -- callers
     * must know the specific authenticator's convention.
     */
    const char *body;

    /** true if body is HTML, false if plain text. */
    bool html;
} ae_handbook_auth_response_t;

/* =========================================================================
 * ae_handbook_authenticator_t -- the handbook authenticator vtable
 *
 * Fully defines the type forward-declared as ae_handbook_authenticator_t
 * in authentication_system.h. Provide a populated instance of this
 * struct to handle GM handbook authentication (typically obtained or
 * replaced via ae_auth_system_t::get_handbook_authenticator). All
 * function pointers must be non-NULL.
 *
 * Concurrency: the server dispatches concurrent handbook auth requests
 * from multiple threads against a single shared instance. This interface
 * provides no locking of its own -- every implementation is responsible
 * for its own internal thread-safety across all of its function pointers
 * and whatever state 'impl' points to.
 * ====================================================================== */
struct ae_handbook_authenticator {

    /**
     * @brief Respond with a page that lets the user authenticate.
     *
     * Writes its response through request->context (both reading the
     * request body and writing the response body); does not use a
     * separate output parameter.
     *
     * @route GET /handbook/authenticate
     *
     * @param self    The authenticator instance. Must not be NULL.
     * @param request Request to authenticate. Must not be NULL.
     */
    void (*present_page)(
        struct ae_handbook_authenticator *self,
        const ae_auth_request_t          *request);

    /**
     * @brief Handle submission of the authentication form.
     *
     * Should populate *out with HTML that sends a message to the GM
     * Handbook; see the default handbook authentication page for an
     * example. See ae_handbook_auth_response_t::body for the ownership
     * caveat on the response body this writes.
     *
     * @param self    The authenticator instance. Must not be NULL.
     * @param request Request to authenticate. Must not be NULL.
     * @param out     Response to populate. Must not be NULL.
     */
    void (*authenticate)(
        struct ae_handbook_authenticator *self,
        const ae_auth_request_t          *request,
        ae_handbook_auth_response_t      *out);

    /** Implementation-specific data; opaque to callers. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macros
 *
 * Thin, direct forwards to the corresponding ae_handbook_authenticator_t
 * member -- each macro performs no NULL-checking or other logic of its
 * own. auth is expanded twice in every macro below (once as the object
 * of ->, once as the first call argument), so pass a simple variable,
 * not an expression with side effects -- it would run twice.
 * ====================================================================== */

#define ae_handbook_present_page(auth, req) \
    (auth)->present_page((auth), (req))

#define ae_handbook_authenticate(auth, req, out) \
    (auth)->authenticate((auth), (req), (out))

#ifdef __cplusplus
}
#endif

#endif /* HANDBOOK_AUTHENTICATOR_H */
