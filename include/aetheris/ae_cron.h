/*
 * 
 *           ______ _______ _    _ ______ _____  _____  _____ 
 *     /\   |  ____|__   __| |  | |  ____|  __ \|_   _|/ ____|
 *    /  \  | |__     | |  | |__| | |__  | |__) | | | | (___  
 *   / /\ \ |  __|    | |  |  __  |  __| |  _  /  | | |  \___ \ 
 *  / ____ \| |____   | |  |  |  | | |____| | \ \ _| |_ ____) |
 * /_/    \_\______|  |_|  |_|  |_|______|_|  \_\_____|_____/ 
 * 
 *                                                                                     v1.0
 * @name Aetheris
 *
 * @author dkitagawa
 *
 * @file ae_cron.h
 *
 * @brief Quartz-style cron expression parser and next-fire-time
 *        calculator.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#ifndef AE_CRON_H
#define AE_CRON_H

#include <aetheris/ae_error.h>
#include <aetheris/ae_types.h>

/* ============================================================ */

/*
 * Six-field Quartz cron format:
 *
 *   Second Minute Hour Day-of-Month Month Day-of-Week
 *   (0-59)  (0-59)  (0-23)    (1-31)    (1-12)   (?) 
 *
 * Day-of-week is 1-7 with 1 = Sunday ... 7 = Saturday, matching
 * the Quartz convention.  A "?" marks a field as unspecified and
 * is valid only in the day-of-month and day-of-week positions.
 *
 * Supported tokens:
 *   *        every value in the field
 *   ?        no specific value (day fields only)
 *   N        a single value
 *   a-b      an inclusive range
 *   star/step        every N-th value from the minimum
 *   a-b/step every N-th value within range
 *   a,b,c    a list (may combine the forms above)
 *
 * Examples:
 *   "0 0 0 * * ?"         every day at 00:00:00
 *   "0 * * * * ?"         every minute at second 0
 *   "0 0 12 1,15 * ?"     at 12:00:00 on the 1st and 15th of each month
 *   "30 0/5 9-17 * * "    Monday-Friday 09:00-17:05, every five minutes
 *   "0 0 0 * * 1-5"       every weekday at 00:00:00
 */

/* Bit-indexed field masks. */
#define AE_CRON_SEC_MAX 60
#define AE_CRON_MIN_MAX 60
#define AE_CRON_HOUR_MAX 24
#define AE_CRON_DOM_MAX 31
#define AE_CRON_MONTH_MAX 12
#define AE_CRON_DOW_MAX 7

typedef struct ae_cron_expr {
    ae_u64 seconds;     /* bit i set means value i fires.   */
    ae_u64 minutes;     /* bit i set means value i fires.   */
    ae_u32 hours;       /* bit i set means value i fires.   */
    ae_u32 dom;         /* bits 1..31, day of month.        */
    ae_u16 months;      /* bits 1..12.                      */
    ae_u8  dow;         /* bits 1..7 (1=Sunday..7=Saturday).*/
    ae_bool dom_any;    /* day-of-month was "?".            */
    ae_bool dow_any;    /* day-of-week was "?".             */
    ae_bool parsed;     /* valid parsed expression.         */
} ae_cron_expr_t;

/**
 * ae_cron_expr_parse - parse a six-field cron expression.
 * @expr: storage for the parsed expression.
 * @text: cron string.  May not be NULL.
 *
 * Returns AE_OK on success (and marks @expr as `parsed`), or a
 * negative error code for a malformed expression.  On failure the
 * expression is left in an unspecified state; callers must check
 * the return before using it.
 */
ae_error_t ae_cron_expr_parse(ae_cron_expr_t *expr, const char *text);

/**
 * ae_cron_expr_next - compute the next fire time after @from.
 * @expr: a successfully parsed expression.
 * @from: epoch seconds (UTC).  The result is strictly greater than
 *        this value.
 *
 * Returns the next fire time as epoch seconds (UTC), or -1 when
 * @expr is invalid or when no matching time exists within the
 * supported look-ahead window.  Fires at most ~9 years ahead to
 * guarantee termination.
 */
ae_s64 ae_cron_expr_next(const ae_cron_expr_t *expr, ae_s64 from);

/**
 * ae_cron_expr_valid - predicate for a usable expression.
 * @expr: expression to test.
 *
 * Returns true after a successful parse.
 */
ae_bool ae_cron_expr_valid(const ae_cron_expr_t *expr);

/* ============================================================ */

#endif /* AE_CRON_H */