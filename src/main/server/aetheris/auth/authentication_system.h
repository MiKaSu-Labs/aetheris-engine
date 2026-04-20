#ifndef AUTHENTICATION_SYSTEM_H
#define AUTHENTICATION_SYSTEM_H

/*
 * authentication_system.h
 *
 * Defines the authentication system interface for the server.
 * Can be replaced by plugins at runtime by swapping ae_auth_system.
 */

#include <stdbool.h>

#include "account.h"
#include "http_context.h"
#include "login_account_request.h"
#include "login_token_request.h"
#include "combo_token_request.h"
#include "login_result.h"
#include "combo_token_response.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Forward declarations
 * ====================================================================== */

typedef struct ae_auth_system            ae_auth_system_t;
typedef struct ae_auth_request           ae_auth_request_t;
typedef struct ae_authenticator          ae_authenticator_t;
typedef struct ae_ext_authenticator      ae_ext_authenticator_t;
typedef struct ae_oauth_authenticator    ae_oauth_authenticator_t;
typedef struct ae_handbook_authenticator ae_handbook_authenticator_t;

/* =========================================================================
 * ae_auth_request_t
 *
 * Data container holding relevant data for authenticating a client.
 * All fields are nullable; unused fields are NULL.
 * ====================================================================== */
struct ae_auth_request {
    ae_http_context_t           *context;             /* nullable */
    ae_login_account_request_t  *password_request;    /* nullable */
    ae_login_token_request_t    *token_request;       /* nullable */
    ae_combo_token_req_t        *session_key_request; /* nullable */
    ae_combo_token_login_data_t *session_key_data;    /* nullable */
};

/* =========================================================================
 * Request builder helpers
 * Each zeroes the output struct then populates only the relevant fields.
 * The caller owns the stack-allocated ae_auth_request_t.
 * ====================================================================== */

/* Builds a request from a password login payload. */
void ae_auth_request_from_password(
    ae_auth_request_t          *out,
    ae_http_context_t          *ctx,
    ae_login_account_request_t *json_data);

/* Builds a request from a token login payload. */
void ae_auth_request_from_token(
    ae_auth_request_t        *out,
    ae_http_context_t        *ctx,
    ae_login_token_request_t *json_data);

/* Builds a request from a combo-token (session key) payload. */
void ae_auth_request_from_combo_token(
    ae_auth_request_t           *out,
    ae_http_context_t           *ctx,
    ae_combo_token_req_t        *json_data,
    ae_combo_token_login_data_t *token_data);

/* Builds a request from a raw HTTP context (external auth). */
void ae_auth_request_from_external(
    ae_auth_request_t *out,
    ae_http_context_t *ctx);

/* =========================================================================
 * Authenticator vtables
 *
 * Each role-specific authenticator is a struct with a single function
 * pointer. The output type varies per role and is cast by the caller.
 * ====================================================================== */

struct ae_authenticator {
    /*
     * Performs authentication. Returns 0 on success, negative ae_error_t
     * on failure. 'out' points to the role-specific result struct:
     *   password authenticator        , ae_login_result_t *
     *   token authenticator           , ae_login_result_t *
     *   session key authenticator     , ae_combo_token_res_t *
     *   session token validator       , ae_account_t *
     */
    int (*authenticate)(
        const ae_auth_request_t *request,
        void                    *out);
};

struct ae_ext_authenticator {
    int (*handle_external)(const ae_auth_request_t *request);
};

struct ae_oauth_authenticator {
    int (*handle_oauth)(const ae_auth_request_t *request);
};

struct ae_handbook_authenticator {
    int (*handle_handbook)(const ae_auth_request_t *request);
};

/* =========================================================================
 * ae_auth_system_t , the authentication system vtable
 *
 * Provide a populated instance of this struct to replace the active
 * authentication system (e.g. from a plugin). All function pointers
 * must be non-NULL.
 * ====================================================================== */
struct ae_auth_system {

    /*
     * Called when a user requests account creation.
     * 'password' is the SHA-256 hash of the raw password.
     */
    void (*create_account)(
        ae_auth_system_t *self,
        const char       *username,
        const char       *password);

    /*
     * Called when a user requests a password reset.
     */
    void (*reset_password)(
        ae_auth_system_t *self,
        const char       *username);

    /*
     * Verifies a user identity from an opaque token (e.g. a JWT).
     * Returns the matching account, or NULL if verification fails.
     */
    ae_account_t *(*verify_user)(
        ae_auth_system_t *self,
        const char       *details);

    /*
     * Returns the authenticator used for username/password login.
     * Output type: ae_login_result_t *
     */
    ae_authenticator_t *(*get_password_authenticator)(
        ae_auth_system_t *self);

    /*
     * Returns the authenticator used for token-based login.
     * Output type: ae_login_result_t *
     */
    ae_authenticator_t *(*get_token_authenticator)(
        ae_auth_system_t *self);

    /*
     * Returns the authenticator used for session-key (combo token) login.
     * Output type: ae_combo_token_res_t *
     */
    ae_authenticator_t *(*get_session_key_authenticator)(
        ae_auth_system_t *self);

    /*
     * Returns the authenticator used for validating existing session tokens.
     * Used internally by dispatch_utils_authenticate().
     * Plugins can override this to support alternate session auth methods.
     * Output type: ae_account_t * (non-NULL means valid)
     */
    ae_authenticator_t *(*get_session_token_validator)(
        ae_auth_system_t *self);

    /*
     * Returns the authenticator for external (third-party) auth requests.
     */
    ae_ext_authenticator_t *(*get_external_authenticator)(
        ae_auth_system_t *self);

    /*
     * Returns the authenticator for OAuth auth requests.
     */
    ae_oauth_authenticator_t *(*get_oauth_authenticator)(
        ae_auth_system_t *self);

    /*
     * Returns the authenticator for handbook auth requests.
     */
    ae_handbook_authenticator_t *(*get_handbook_authenticator)(
        ae_auth_system_t *self);

    /* Implementation-specific data; opaque to callers. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macros
 * ====================================================================== */

#define ae_auth_create_account(sys, user, pass) \
    (sys)->create_account((sys), (user), (pass))

#define ae_auth_reset_password(sys, user) \
    (sys)->reset_password((sys), (user))

#define ae_auth_verify_user(sys, details) \
    (sys)->verify_user((sys), (details))

#define ae_auth_get_password_authenticator(sys) \
    (sys)->get_password_authenticator((sys))

#define ae_auth_get_token_authenticator(sys) \
    (sys)->get_token_authenticator((sys))

#define ae_auth_get_session_key_authenticator(sys) \
    (sys)->get_session_key_authenticator((sys))

#define ae_auth_get_session_token_validator(sys) \
    (sys)->get_session_token_validator((sys))

#define ae_auth_get_external_authenticator(sys) \
    (sys)->get_external_authenticator((sys))

#define ae_auth_get_oauth_authenticator(sys) \
    (sys)->get_oauth_authenticator((sys))

#define ae_auth_get_handbook_authenticator(sys) \
    (sys)->get_handbook_authenticator((sys))

#ifdef __cplusplus
}
#endif

#endif /* AUTHENTICATION_SYSTEM_H */
