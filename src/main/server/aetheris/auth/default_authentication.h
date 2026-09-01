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
 * @file default_authentication.h
 *
 * @brief The default Aetheris authentication implementation.
 *        Allows all users to access any account. Selects between the
 *        standard password authenticator and the experimental real-password
 *        authenticator based on the active server configuration.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef DEFAULT_AUTHENTICATION_H
#define DEFAULT_AUTHENTICATION_H

#include "authentication_system.h"
#include "external_authenticator.h"
#include "oauth_authenticator.h"
#include "handbook_authenticator.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * ae_default_auth_t
 *
 * Concrete implementation data for the default authentication system.
 * Holds the selected sub-authenticator instances. Embedded as the
 * 'impl' pointer of ae_auth_system_t.
 *
 * Ownership: every field is created and owned by
 * default_authentication_create(); do not free any field directly, and
 * do not construct this struct by hand. Released as a whole by
 * default_authentication_free().
 * ====================================================================== */
typedef struct {
    /** Handles password-based login. Either the standard or the
     *  experimental implementation, chosen in
     *  default_authentication_create(). */
    ae_authenticator_t *password_authenticator;

    /** Handles token-based login. */
    ae_authenticator_t *token_authenticator;

    /** Handles session-key (combo token) login. */
    ae_authenticator_t *session_key_authenticator;

    /** Authenticator used to validate existing session tokens -- a
     *  separate code path from the ae_auth_system_t vtable's own
     *  verify_user, which always fails in this default implementation
     *  (see _verify_user() in default_authentication.c). */
    ae_authenticator_t *session_token_validator;

    /** Handles external (third-party) authentication requests. */
    ae_ext_authenticator_t *external_authenticator;

    /** Handles OAuth authentication requests. */
    ae_oauth_authenticator_t *oauth_authenticator;

    /** Handles handbook (GM tool) authentication requests. */
    ae_handbook_authenticator_t *handbook_authenticator;
} ae_default_auth_t;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/**
 * @brief Allocate and populate an ae_auth_system_t using the default
 *        authentication implementation.
 *
 * @warning As shipped, the vtable's create_account() and reset_password()
 * are unimplemented no-ops, and verify_user() (session token validation)
 * always returns NULL. This default is documented as allowing any user
 * to access any account and MUST NOT be used in a production or
 * publicly-reachable deployment.
 *
 * If AE_ACCOUNT->experimental_real_password is true, the experimental
 * password authenticator is selected; otherwise the standard password
 * authenticator is used. The remaining sub-authenticators are always
 * created via their respective *_create() functions.
 *
 * Not thread-safe. Call from the main thread only, during startup.
 *
 * @return A fully populated ae_auth_system_t. The caller owns it and
 *         must release it with default_authentication_free().
 * @return NULL if any allocation or any sub-authenticator creation
 *         fails. Whatever was already created is freed before
 *         returning, so no partial or leaked state remains on failure.
 */
ae_auth_system_t *default_authentication_create(void);

/**
 * @brief Release all resources owned by a default authentication system
 *        instance created with default_authentication_create().
 *
 * @param system Instance to release. May be NULL, in which case this is
 *               a no-op. After this call, system and every pointer it
 *               previously exposed (via its get_*_authenticator members)
 *               are invalid and must not be used.
 */
void default_authentication_free(ae_auth_system_t *system);

#ifdef __cplusplus
}
#endif

#endif /* DEFAULT_AUTHENTICATION_H */
