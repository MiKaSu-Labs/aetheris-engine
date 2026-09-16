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
 * @file ae_cron.c
 *
 * @brief Six-field cron parser and next-fire computation.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "aetheris/ae_cron.h"

/* ============================================================ */

/* Field value that also treats a leading "?" as "whole field". */
#define AE_CRON_ANY ((ae_s32)-1)

/* Compose a bit mask from start..end advancing by step. */
static ae_u64 mask_make(ae_s32 start, ae_s32 end, ae_s32 step)
{
    ae_u64 mask = 0;
    ae_s32 v;

    for (v = start; v <= end; v += step) {
        mask |= (ae_u64)1 << (ae_u32)v;
    }

    return mask;
}

/* Extract a numeric token from [p, end]. Returns AE_ERR_BAD_DATA on
 * a non-digit sequence and stores the parsed value.  *p is advanced
 * past the token. */
static ae_error_t parse_number(const char **p, const char *end,
                               ae_s32 *out)
{
    ae_s64 val = 0;
    ae_bool any = false;

    while (*p < end && isdigit((unsigned char)**p)) {
        val = val * 10 + (ae_s64)(**p - '0');
        ++(*p);
        any = true;
    }

    if (!any) {
        return AE_ERR_BAD_DATA;
    }

    *out = (ae_s32)val;
    return AE_OK;
}

/* Parse one list member which is one of "*", "star/step", "N", "N-M",
 * "N-M/step" where step defaults to 1.  Bounds are validated against
 * min..max. */
static ae_error_t parse_list_member(const char *tok, ae_size len,
                                    ae_s32 min, ae_s32 max,
                                    ae_s32 *start, ae_s32 *end,
                                    ae_s32 *step)
{
    const char *p = tok;
    const char *tend = tok + len;

    *step = 1;

    if (len == 1 && *tok == '*') {
        *start = min;
        *end = max;
        return AE_OK;
    }

    if (len >= 2 && *tok == '*' && tok[1] == '/') {
        ae_error_t err;
        p = tok + 2;
        err = parse_number(&p, tend, step);
        if (err != AE_OK || *step == 0 || p != tend) {
            return AE_ERR_BAD_DATA;
        }
        *start = min;
        *end = max;
        return AE_OK;
    }

    /* N or N-M or N-M/step. */
    {
        ae_error_t err;
        err = parse_number(&p, tend, start);
        if (err != AE_OK) {
            return AE_ERR_BAD_DATA;
        }
        if (p == tend) {
            *end = *start;
            if (*start < min || *start > max) {
                return AE_ERR_BAD_DATA;
            }
            return AE_OK;
        }
        if (*p != '-') {
            return AE_ERR_BAD_DATA;
        }
        ++p;
        err = parse_number(&p, tend, end);
        if (err != AE_OK) {
            return AE_ERR_BAD_DATA;
        }
        if (p == tend) {
            if (*start < min || *end > max || *start > *end) {
                return AE_ERR_BAD_DATA;
            }
            return AE_OK;
        }
        if (*p != '/') {
            return AE_ERR_BAD_DATA;
        }
        ++p;
        err = parse_number(&p, tend, step);
        if (err != AE_OK || *step == 0 || p != tend) {
            return AE_ERR_BAD_DATA;
        }
    }

    if (*start < min || *end > max || *start > *end) {
        return AE_ERR_BAD_DATA;
    }

    return AE_OK;
}

/* Parse a comma-separated list field into a mask. */
static ae_error_t parse_field(const char *field, ae_size len,
                              ae_s32 min, ae_s32 max, ae_u64 *mask)
{
    const char *p = field;
    const char *tend = field + len;
    ae_u64 acc = 0;

    while (p < tend) {
        const char *start = p;
        const char *comma = memchr(p, ',', (ae_size)(tend - p));
        const char *member_end = comma ? comma : tend;
        ae_s32 s, e, st;
        ae_error_t err;

        if (member_end == start) {
            return AE_ERR_BAD_DATA;
        }

        err = parse_list_member(start, (ae_size)(member_end - start),
                                min, max, &s, &e, &st);
        if (err != AE_OK) {
            return err;
        }

        acc |= mask_make(s, e, st);

        if (!comma) {
            break;
        }
        p = comma + 1;
    }

    if (acc == 0) {
        return AE_ERR_BAD_DATA;
    }

    *mask = acc;
    return AE_OK;
}

/* Day-of-month/day-of-week fields accept "?" in addition to the
 * standard tokens. */
static ae_error_t parse_day_field(const char *field, ae_size len,
                                  ae_s32 min, ae_s32 max, ae_u64 *mask,
                                  ae_bool *any)
{
    if (len == 1 && *field == '?') {
        *any = true;
        *mask = mask_make(min, max, 1);
        return AE_OK;
    }
    *any = false;
    return parse_field(field, len, min, max, mask);
}

/* ============================================================ */

ae_error_t ae_cron_expr_parse(ae_cron_expr_t *expr, const char *text)
{
    const char *p;
    const char *tend;
    ae_size field_count = 0;
    const char *fields[7];
    ae_size field_lens[7];

    if (!expr || !text) {
        return AE_ERR_INVALID_ARG;
    }

    memset(expr, 0, sizeof(*expr));
    expr->parsed = false;

    /* Normalise consecutive whitespace into single tokens. */
    p = text;
    tend = text + strlen(text);

    while (p < tend && field_count < 7) {
        const char *tok;

        while (p < tend && isspace((unsigned char)*p)) {
            ++p;
        }
        if (p >= tend) {
            break;
        }
        tok = p;
        while (p < tend && !isspace((unsigned char)*p)) {
            ++p;
        }
        fields[field_count] = tok;
        field_lens[field_count] = (ae_size)(p - tok);
        ++field_count;
    }

    if (field_count != 6) {
        return AE_ERR_BAD_DATA;
    }

    {
        ae_u64 mask;
        ae_error_t err;

        err = parse_field(fields[0], field_lens[0], 0, 59, &expr->seconds);
        if (err != AE_OK) {
            return err;
        }

        err = parse_field(fields[1], field_lens[1], 0, 59, &expr->minutes);
        if (err != AE_OK) {
            return err;
        }

        err = parse_field(fields[2], field_lens[2], 0, 23, &mask);
        if (err != AE_OK) {
            return err;
        }
        expr->hours = (ae_u32)mask;

        err = parse_day_field(fields[3], field_lens[3], 1, 31, &mask,
                              &expr->dom_any);
        if (err != AE_OK) {
            return err;
        }
        expr->dom = (ae_u32)mask;

        err = parse_field(fields[4], field_lens[4], 1, 12, &mask);
        if (err != AE_OK) {
            return err;
        }
        expr->months = (ae_u16)mask;

        err = parse_day_field(fields[5], field_lens[5], 1, 7, &mask,
                              &expr->dow_any);
        if (err != AE_OK) {
            return err;
        }
        expr->dow = (ae_u8)mask;
    }

    expr->parsed = true;
    return AE_OK;
}

/* ============================================================ */

/* Next bit index >= start within max inclusive, or -1. */
static ae_s32 bit_find_next(ae_u64 mask, ae_s32 start, ae_s32 max)
{
    ae_u64 shifted;
    ae_s32 bits;

    if (start < 0) {
        start = 0;
    }
    shifted = mask >> (ae_u64)start;
    if (shifted == 0) {
        return -1;
    }
    bits = 0;
    while ((shifted & 1) == 0) {
        shifted >>= 1;
        ++bits;
    }
    bits += start;
    return bits <= max ? bits : -1;
}

/* ============================================================ */

/* Weekday from days since epoch: 0 = Sunday. */
static ae_s32 weekday_from_days(ae_s64 z)
{
    return (ae_s32)((z + 4) % 7);
}

/* Civil year/month/day from days since epoch. */
static void civil_from_days(ae_s64 z, ae_s64 *y, ae_s32 *m, ae_s32 *d)
{
    z += 719468;
    const ae_s64 era = (z >= 0 ? z : z - 146096) / 146097;
    const ae_s32 doe = (ae_s32)(z - era * 146097);
    const ae_s32 yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    ae_s64 yy = (ae_s64)yoe + era * 400;
    const ae_s32 doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const ae_s32 mp = (5 * doy + 2) / 153;
    const ae_s32 dd = doy - (153 * mp + 2) / 5 + 1;
    ae_s32 mm = mp + (mp < 10 ? 3 : -9);
    if (mm <= 2) {
        yy += 1;
    }
    *y = yy;
    *m = mm;
    *d = dd;
}

/* True when the day matches, given the Quartz dom/dow rules. */
static ae_bool day_matches(const ae_cron_expr_t *e, ae_s64 days,
                           ae_s32 month, ae_s32 day)
{
    ae_s32 dow_quartz;
    ae_bool dom_ok;
    ae_bool dow_ok;

    if ((e->months & ((ae_u16)1 << (ae_u32)month)) == 0) {
        return false;
    }

    dow_quartz = weekday_from_days(days) + 1; /* 1=Sunday..7=Sat */
    dom_ok = (e->dom & ((ae_u32)1 << (ae_u32)day)) != 0;
    dow_ok = (e->dow & ((ae_u8)1 << (ae_u32)dow_quartz)) != 0;

    /* "?" fields defer to the other; when both are explicit a day
     * fires if either field matches. */
    if (e->dom_any && e->dow_any) {
        return true;
    }
    if (e->dom_any) {
        return dow_ok;
    }
    if (e->dow_any) {
        return dom_ok;
    }
    return dom_ok || dow_ok;
}

ae_s64 ae_cron_expr_next(const ae_cron_expr_t *expr, ae_s64 from)
{
    ae_s64 t;
    ae_s32 sec, minute, hour, day, month;
    ae_s64 year;
    ae_s64 days;
    ae_s32 next;
    long iterations = 0;

    if (!expr || !expr->parsed) {
        return -1;
    }

    t = from + 1;

    /*
     * Walk time forward until the expression fires. Every pass either
     * slides to the next matching second/minute/hour or the next day,
     * so the loop terminates quickly for real expressions. A generous
     * cap catches impossible combinations (e.g. 31 Feb).
     */
    for (;;) {
        ae_s64 total_min = t / 60;
        ae_s64 total_hour = t / 3600;
        ae_s64 total_day = t / 86400;

        sec = (ae_s32)(t % 60);
        minute = (ae_s32)(total_min % 60);
        hour = (ae_s32)(total_hour % 24);
        days = total_day;
        civil_from_days(days, &year, &month, &day);

        if (!(expr->seconds & ((ae_u64)1 << (ae_u32)sec))) {
            next = bit_find_next(expr->seconds, sec + 1, 59);
            if (next >= 0) {
                t = total_min * 60 + (ae_s64)next;
            } else {
                t = (total_min + 1) * 60;
            }
            continue;
        }

        if (!(expr->minutes & ((ae_u64)1 << (ae_u32)minute))) {
            next = bit_find_next(expr->minutes, minute + 1, 59);
            if (next >= 0) {
                t = total_hour * 3600 + (ae_s64)next * 60;
            } else {
                t = (total_hour + 1) * 3600;
            }
            continue;
        }

        if (!(expr->hours & ((ae_u32)1 << (ae_u32)hour))) {
            next = bit_find_next(expr->hours, hour + 1, 23);
            if (next >= 0) {
                t = total_day * 86400 + (ae_s64)next * 3600;
            } else {
                t = (total_day + 1) * 86400;
            }
            continue;
        }

        if (day_matches(expr, days, month, day)) {
            return t;
        }

        t = (total_day + 1) * 86400;

        if (++iterations > 30000) {
            return -1;
        }
    }
}

ae_bool ae_cron_expr_valid(const ae_cron_expr_t *expr)
{
    return expr && expr->parsed;
}