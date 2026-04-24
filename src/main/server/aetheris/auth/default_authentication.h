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
 * ====================================================================== */
typedef struct {
    ae_authenticator_t          *password_authenticator;
    ae_authenticator_t          *token_authenticator;
    ae_authenticator_t          *session_key_authenticator;
    ae_authenticator_t          *session_token_validator;
    ae_ext_authenticator_t      *external_authenticator;
    ae_oauth_authenticator_t    *oauth_authenticator;
    ae_handbook_authenticator_t *handbook_authenticator;
} ae_default_auth_t;

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

/*
 * Allocates and returns a fully populated ae_auth_system_t using the
 * default authentication implementation.
 *
 * If ae_config->account.experimental_real_password is true, the
 * experimental password authenticator is selected; otherwise the
 * standard password authenticator is used.
 *
 * The caller is responsible for freeing with
 * default_authentication_free().
 */
ae_auth_system_t *default_authentication_create(void);

/*
 * Releases all resources owned by a default authentication system
 * instance created with default_authentication_create().
 */
void default_authentication_free(ae_auth_system_t *system);

#ifdef __cplusplus
}
#endif

#endif /* DEFAULT_AUTHENTICATION_H */
