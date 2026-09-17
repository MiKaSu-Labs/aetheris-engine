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
 * @file test_ae_packet_opcodes_utils.c
 *
 * @brief Tests for opcode-to-name lookup and loop-packet detection.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <string.h>

#include <aetheris/ae_packet_opcodes.h>
#include <aetheris/ae_packet_opcodes_utils.h>

#include "ae_test.h"

/* ============================================================ */

static void test_known_names(void)
{
    const char *n;

    n = ae_get_opcode_name(PingReq);
    AE_TEST_ASSERT(n != NULL);
    AE_TEST_ASSERT(n[0] != '\0');

    AE_TEST_ASSERT(strcmp(n, ae_get_opcode_name(PingReq)) == 0);

    n = ae_get_opcode_name(PlayerLogoutReq);
    AE_TEST_ASSERT(n != NULL);
    AE_TEST_ASSERT(n[0] != '\0');

    n = ae_get_opcode_name(WorldPlayerReviveReq);
    AE_TEST_ASSERT(n != NULL);
    AE_TEST_ASSERT(n[0] != '\0');
}

static void test_unknown_and_boundary(void)
{
    AE_TEST_ASSERT(strcmp(ae_get_opcode_name(0), "UNKNOWN") == 0);
    AE_TEST_ASSERT(strcmp(ae_get_opcode_name(-1), "UNKNOWN") == 0);
    AE_TEST_ASSERT(strcmp(ae_get_opcode_name(999999), "UNKNOWN") == 0);
}

static void test_first_and_last(void)
{
    AE_TEST_ASSERT(strcmp(ae_get_opcode_name(0), "UNKNOWN") == 0);
    AE_TEST_ASSERT(strcmp(ae_get_opcode_name(205), "ACCKLIOPBHN") == 0);
}

static void test_loop_packets_true(void)
{
    AE_TEST_ASSERT(ae_is_loop_packet(PingReq) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(PingRsp) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(WorldPlayerRTTNotify) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(UnionCmdNotify) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(QueryPathReq) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(QueryPathRsp) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(PlayerTimeNotify) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(PlayerGameTimeNotify) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(AvatarPropNotify) == true);
    AE_TEST_ASSERT(ae_is_loop_packet(AvatarSatiationDataNotify) == true);
}

static void test_loop_packets_false(void)
{
    AE_TEST_ASSERT(ae_is_loop_packet(0) == false);
    AE_TEST_ASSERT(ae_is_loop_packet(-1) == false);
    AE_TEST_ASSERT(ae_is_loop_packet(PlayerLogoutReq) == false);
    AE_TEST_ASSERT(ae_is_loop_packet(WorldPlayerReviveReq) == false);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"Known opcode names", test_known_names},
    {"Unknown and boundary opcodes", test_unknown_and_boundary},
    {"First and last entries", test_first_and_last},
    {"Loop packets detected", test_loop_packets_true},
    {"Non-loop packets rejected", test_loop_packets_false},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("packet_utils: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */
