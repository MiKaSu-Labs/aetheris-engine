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
 * @file default_authentication.c
 *
 * @brief Default authentication system implementation.
 *        Wires together all sub-authenticators and exposes the
 *        ae_auth_system_t vtable for use by the server core.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include "default_authentication.h"

#include <stdlib.h>

#include "aetheris.h"
#include "configuration.h"
#include "default_authenticators.h"
#include "lang.h"

/* =========================================================================
 * vtable implementations
 * ====================================================================== */

static void _create_account(
        ae_auth_system_t *self,
        const char       *username,
        const char       *password) {
    /* Unhandled. The default authenticator does not store passwords. */
    (void)self;
    (void)username;
    (void)password;
}

static void _reset_password(
        ae_auth_system_t *self,
        const char       *username) {
    /* Unhandled. The default authenticator does not store passwords. */
    (void)self;
    (void)username;
}

static ae_account_t *_verify_user(
        ae_auth_system_t *self,
        const char       *details) {
    (void)self;
    (void)details;
    ae_logger_info(ae_logger,
        translate("messages.dispatch.authentication.default_unable_to_verify"));
    return NULL;
}

static ae_authenticator_t *_get_password_authenticator(ae_auth_system_t *self) {
    return ((ae_default_auth_t *)self->impl)->password_authenticator;
}

static ae_authenticator_t *_get_token_authenticator(ae_auth_system_t *self) {
    return ((ae_default_auth_t *)self->impl)->token_authenticator;
}

static ae_authenticator_t *_get_session_key_authenticator(ae_auth_system_t *self) {
    return ((ae_default_auth_t *)self->impl)->session_key_authenticator;
}

static ae_authenticator_t *_get_session_token_validator(ae_auth_system_t *self) {
    return ((ae_default_auth_t *)self->impl)->session_token_validator;
}

static ae_ext_authenticator_t *_get_external_authenticator(ae_auth_system_t *self) {
    return ((ae_default_auth_t *)self->impl)->external_authenticator;
}

static ae_oauth_authenticator_t *_get_oauth_authenticator(ae_auth_system_t *self) {
    return ((ae_default_auth_t *)self->impl)->oauth_authenticator;
}

static ae_handbook_authenticator_t *_get_handbook_authenticator(ae_auth_system_t *self) {
    return ((ae_default_auth_t *)self->impl)->handbook_authenticator;
}

/* =========================================================================
 * Lifecycle
 * ====================================================================== */

ae_auth_system_t *default_authentication_create(void) {
    ae_default_auth_t *impl = calloc(1, sizeof(ae_default_auth_t));
    if (!impl) return NULL;

    /*
     * Select the password authenticator based on the server configuration.
     * If experimental_real_password is enabled, use the experimental
     * authenticator; otherwise use the standard password authenticator.
     */
    if (AE_ACCOUNT->experimental_real_password) {
        impl->password_authenticator = experimental_password_authenticator_create();
    } else {
        impl->password_authenticator = password_authenticator_create();
    }

    impl->token_authenticator       = token_authenticator_create();
    impl->session_key_authenticator = session_key_authenticator_create();
    impl->session_token_validator   = session_token_validator_create();
    impl->external_authenticator    = external_authentication_create();
    impl->oauth_authenticator       = oauth_authentication_create();
    impl->handbook_authenticator    = handbook_authentication_create();

    ae_auth_system_t *system = calloc(1, sizeof(ae_auth_system_t));
    if (!system) {
        free(impl);
        return NULL;
    }

    system->impl                        = impl;
    system->create_account              = _create_account;
    system->reset_password              = _reset_password;
    system->verify_user                 = _verify_user;
    system->get_password_authenticator  = _get_password_authenticator;
    system->get_token_authenticator     = _get_token_authenticator;
    system->get_session_key_authenticator = _get_session_key_authenticator;
    system->get_session_token_validator = _get_session_token_validator;
    system->get_external_authenticator  = _get_external_authenticator;
    system->get_oauth_authenticator     = _get_oauth_authenticator;
    system->get_handbook_authenticator  = _get_handbook_authenticator;

    return system;
}

void default_authentication_free(ae_auth_system_t *system) {
    if (!system) return;

    ae_default_auth_t *impl = (ae_default_auth_t *)system->impl;
    if (impl) {
        password_authenticator_free(impl->password_authenticator);
        token_authenticator_free(impl->token_authenticator);
        session_key_authenticator_free(impl->session_key_authenticator);
        session_token_validator_free(impl->session_token_validator);
        external_authentication_free(impl->external_authenticator);
        oauth_authentication_free(impl->oauth_authenticator);
        handbook_authentication_free(impl->handbook_authenticator);
        free(impl);
    }

    free(system);
}
