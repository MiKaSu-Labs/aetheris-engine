/*
 * authentication_system.c
 */

#include "authentication_system.h"

#include <string.h>

/* =========================================================================
 * Request builder helpers
 * ====================================================================== */

void ae_auth_request_from_password(
        ae_auth_request_t          *out,
        ae_http_context_t          *ctx,
        ae_login_account_request_t *json_data) {
    memset(out, 0, sizeof(*out));
    out->context          = ctx;
    out->password_request = json_data;
}

void ae_auth_request_from_token(
        ae_auth_request_t        *out,
        ae_http_context_t        *ctx,
        ae_login_token_request_t *json_data) {
    memset(out, 0, sizeof(*out));
    out->context       = ctx;
    out->token_request = json_data;
}

void ae_auth_request_from_combo_token(
        ae_auth_request_t           *out,
        ae_http_context_t           *ctx,
        ae_combo_token_req_t        *json_data,
        ae_combo_token_login_data_t *token_data) {
    memset(out, 0, sizeof(*out));
    out->context             = ctx;
    out->session_key_request = json_data;
    out->session_key_data    = token_data;
}

void ae_auth_request_from_external(
        ae_auth_request_t *out,
        ae_http_context_t *ctx) {
    memset(out, 0, sizeof(*out));
    out->context = ctx;
}
