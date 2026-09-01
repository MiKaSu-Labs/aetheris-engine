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
 * @file authentication_system.h
 *
 * @brief Pluggable authentication system interface for Aetheris.
 *
 * Defines request structures, authenticator vtables, and the main
 * authentication system interface used for login, token validation,
 * external authentication, and plugin-based auth replacement at runtime.
 * The active system is replaced by swapping the global ae_auth_system
 * pointer (declared in aetheris.h) to point at a populated
 * ae_auth_system_t, typically from a plugin's init routine.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AUTHENTICATION_SYSTEM_H
#define AUTHENTICATION_SYSTEM_H

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
 * Data container holding the relevant data for authenticating a client.
 * Built by exactly one of the request builder helpers below, each of
 * which zeroes the struct first, so at most one of {password_request},
 * {token_request}, or {session_key_request + session_key_data} is ever
 * non-NULL at a time; a request built by ae_auth_request_from_external()
 * has all three NULL. This mutual exclusivity is a convention upheld by
 * the builders, not something the struct enforces on its own -- code
 * that reads these fields relies on callers going through a builder
 * rather than populating the struct by hand.
 *
 * Not meant to be shared across threads: each request-handling thread is
 * expected to build and use its own instance, typically stack-allocated
 * for the duration of handling one request.
 * ====================================================================== */
struct ae_auth_request {
    /** HTTP context for this request. Nullable; NULL if not applicable
     *  to how the request was built. */
    ae_http_context_t *context;

    /** Set only by ae_auth_request_from_password(). */
    ae_login_account_request_t *password_request;

    /** Set only by ae_auth_request_from_token(). */
    ae_login_token_request_t *token_request;

    /** Set only by ae_auth_request_from_combo_token(), alongside
     *  session_key_data. */
    ae_combo_token_req_t *session_key_request;

    /** Set only by ae_auth_request_from_combo_token(), alongside
     *  session_key_request. */
    ae_combo_token_login_data_t *session_key_data;
};

/* =========================================================================
 * Request builder helpers
 *
 * Each zeroes *out then populates only the fields relevant to that
 * login flow. None of these functions allocate, copy, or take ownership
 * of ctx/json_data/token_data: the pointers are stored as-is (borrowed),
 * so the caller must keep the objects they point to alive for as long
 * as the resulting ae_auth_request_t is used, and must not free them
 * through the request struct.
 *
 * out must not be NULL (the caller owns it and is responsible for its
 * lifetime, typically by stack-allocating it). ctx, json_data, and
 * token_data may be NULL; they are stored as given with no validation.
 * ====================================================================== */

/**
 * @brief Build a request from a password login payload.
 * @param out       Destination; must not be NULL. Zeroed, then populated.
 * @param ctx       HTTP context to store. May be NULL.
 * @param json_data Parsed password login payload to store. May be NULL.
 */
void ae_auth_request_from_password(
    ae_auth_request_t          *out,
    ae_http_context_t          *ctx,
    ae_login_account_request_t *json_data);

/**
 * @brief Build a request from a token login payload.
 * @param out       Destination; must not be NULL. Zeroed, then populated.
 * @param ctx       HTTP context to store. May be NULL.
 * @param json_data Parsed token login payload to store. May be NULL.
 */
void ae_auth_request_from_token(
    ae_auth_request_t        *out,
    ae_http_context_t        *ctx,
    ae_login_token_request_t *json_data);

/**
 * @brief Build a request from a combo-token (session key) payload.
 * @param out        Destination; must not be NULL. Zeroed, then populated.
 * @param ctx        HTTP context to store. May be NULL.
 * @param json_data  Parsed combo-token request payload to store. May be NULL.
 * @param token_data Decoded session-key login data to store. May be NULL.
 */
void ae_auth_request_from_combo_token(
    ae_auth_request_t           *out,
    ae_http_context_t           *ctx,
    ae_combo_token_req_t        *json_data,
    ae_combo_token_login_data_t *token_data);

/**
 * @brief Build a request from a raw HTTP context (external auth).
 *
 * Leaves password_request, token_request, session_key_request, and
 * session_key_data all NULL.
 *
 * @param out Destination; must not be NULL. Zeroed, then populated.
 * @param ctx HTTP context to store. May be NULL.
 */
void ae_auth_request_from_external(
    ae_auth_request_t *out,
    ae_http_context_t *ctx);

/* =========================================================================
 * Authenticator vtables
 *
 * Each role-specific authenticator is a struct with a single function
 * pointer. The output type varies per role and is cast by the caller --
 * see the comment on each function pointer for the concrete type it
 * expects. Passing the wrong concrete type, or a mismatched request kind,
 * is undefined behavior; this interface performs no runtime type check.
 *
 * The function pointer in every struct in this section must be non-NULL
 * once the struct is handed to the auth system; a populated struct with
 * a NULL callback is not a valid instance of that role.
 * ====================================================================== */

struct ae_authenticator {
    /**
     * @brief Perform authentication for one login flow.
     *
     * @param request Request to authenticate. Must not be NULL.
     * @param out     Role-specific result struct to populate. Must not
     *                be NULL; concrete type depends on which role this
     *                authenticator was obtained for:
     *                  password authenticator        -> ae_login_result_t *
     *                  token authenticator            -> ae_login_result_t *
     *                  session key authenticator      -> ae_combo_token_res_t *
     *                  session token validator        -> ae_account_t *
     * @return 0 on success, negative ae_error_t on failure.
     */
    int (*authenticate)(
        const ae_auth_request_t *request,
        void                    *out);
};

struct ae_ext_authenticator {
    /**
     * @brief Handle an external (third-party) authentication request.
     * @param request Request to authenticate. Must not be NULL.
     * @return 0 on success, negative ae_error_t on failure.
     */
    int (*handle_external)(const ae_auth_request_t *request);
};

struct ae_oauth_authenticator {
    /**
     * @brief Handle an OAuth authentication request.
     * @param request Request to authenticate. Must not be NULL.
     * @return 0 on success, negative ae_error_t on failure.
     */
    int (*handle_oauth)(const ae_auth_request_t *request);
};

struct ae_handbook_authenticator {
    /**
     * @brief Handle a handbook (GM tool) authentication request.
     * @param request Request to authenticate. Must not be NULL.
     * @return 0 on success, negative ae_error_t on failure.
     */
    int (*handle_handbook)(const ae_auth_request_t *request);
};

/* =========================================================================
 * ae_auth_system_t -- the authentication system vtable
 *
 * Provide a populated instance of this struct to replace the active
 * authentication system (e.g. from a plugin). All function pointers
 * must be non-NULL.
 *
 * Concurrency: the server dispatches concurrent login, token-validation,
 * and account-management requests from multiple threads against a single
 * shared ae_auth_system_t instance. This interface provides no locking
 * of its own -- every implementation of this vtable is responsible for
 * its own internal thread-safety across all of its function pointers
 * and whatever state 'impl' points to.
 * ====================================================================== */
struct ae_auth_system {

    /**
     * @brief Handle an account-creation request.
     * @param self     The auth system instance. Must not be NULL.
     * @param username Requested username. Must not be NULL.
     * @param password SHA-256 hash of the raw password, not the raw
     *                 password itself. Must not be NULL.
     */
    void (*create_account)(
        ae_auth_system_t *self,
        const char       *username,
        const char       *password);

    /**
     * @brief Handle a password-reset request.
     * @param self     The auth system instance. Must not be NULL.
     * @param username Account requesting the reset. Must not be NULL.
     */
    void (*reset_password)(
        ae_auth_system_t *self,
        const char       *username);

    /**
     * @brief Verify a user identity from an opaque token (e.g. a JWT).
     *
     * This is a separate verification path from get_session_token_validator()
     * below, not an alias for it: an implementation is free to give the
     * two different behavior. The default Aetheris implementation, for
     * example, always fails verify_user() while delegating actual
     * session-token checks entirely to get_session_token_validator().
     *
     * @param self    The auth system instance. Must not be NULL.
     * @param details Opaque token/credential to verify. Must not be NULL.
     * @return Borrowed pointer to the matching account in the account
     *         registry, or NULL if verification fails. Do not free the
     *         returned pointer.
     */
    ae_account_t *(*verify_user)(
        ae_auth_system_t *self,
        const char       *details);

    /**
     * @brief Return the authenticator for username/password login.
     * @param self The auth system instance. Must not be NULL.
     * @return Borrowed pointer, owned by this auth system instance; do
     *         not free. Its authenticate() expects an ae_login_result_t *
     *         out parameter.
     */
    ae_authenticator_t *(*get_password_authenticator)(
        ae_auth_system_t *self);

    /**
     * @brief Return the authenticator for token-based login.
     * @param self The auth system instance. Must not be NULL.
     * @return Borrowed pointer, owned by this auth system instance; do
     *         not free. Its authenticate() expects an ae_login_result_t *
     *         out parameter.
     */
    ae_authenticator_t *(*get_token_authenticator)(
        ae_auth_system_t *self);

    /**
     * @brief Return the authenticator for session-key (combo token) login.
     * @param self The auth system instance. Must not be NULL.
     * @return Borrowed pointer, owned by this auth system instance; do
     *         not free. Its authenticate() expects an
     *         ae_combo_token_res_t * out parameter.
     */
    ae_authenticator_t *(*get_session_key_authenticator)(
        ae_auth_system_t *self);

    /**
     * @brief Return the authenticator for validating existing session
     *        tokens. Used internally by dispatch_utils_authenticate().
     *        Plugins can override this to support alternate session
     *        auth methods. This is a separate verification path from
     *        verify_user() above, not an alias for it -- see that
     *        function's doc comment.
     * @param self The auth system instance. Must not be NULL.
     * @return Borrowed pointer, owned by this auth system instance; do
     *         not free. Its authenticate() expects an ae_account_t * out
     *         parameter; a non-NULL result on success means the session
     *         is valid.
     */
    ae_authenticator_t *(*get_session_token_validator)(
        ae_auth_system_t *self);

    /**
     * @brief Return the authenticator for external (third-party) auth.
     * @param self The auth system instance. Must not be NULL.
     * @return Borrowed pointer, owned by this auth system instance; do
     *         not free.
     */
    ae_ext_authenticator_t *(*get_external_authenticator)(
        ae_auth_system_t *self);

    /**
     * @brief Return the authenticator for OAuth auth requests.
     * @param self The auth system instance. Must not be NULL.
     * @return Borrowed pointer, owned by this auth system instance; do
     *         not free.
     */
    ae_oauth_authenticator_t *(*get_oauth_authenticator)(
        ae_auth_system_t *self);

    /**
     * @brief Return the authenticator for handbook (GM tool) auth
     *        requests.
     * @param self The auth system instance. Must not be NULL.
     * @return Borrowed pointer, owned by this auth system instance; do
     *         not free.
     */
    ae_handbook_authenticator_t *(*get_handbook_authenticator)(
        ae_auth_system_t *self);

    /** Implementation-specific data; opaque to callers. */
    void *impl;
};

/* =========================================================================
 * Convenience dispatch macros
 *
 * Thin, direct forwards to the corresponding ae_auth_system_t member --
 * each macro performs no NULL-checking or other logic of its own. See
 * the struct definition above for each member's parameter, ownership,
 * and thread-safety contract; the macros carry no additional contract
 * beyond what's documented there.
 *
 * sys is expanded twice in every macro below (once as the object of ->,
 * once as the first call argument), so pass a simple variable, not an
 * expression with side effects -- it would run twice.
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
