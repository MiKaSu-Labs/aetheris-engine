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
 * @file test_ae_base_packet.c
 *
 * @brief Tests for the outgoing big-endian wire frame builder.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aetheris/ae_base_packet.h>

#include "ae_test.h"

/* ============================================================ */

static void test_empty_frame(void)
{
    ae_base_packet_t pkt = {0};
    ae_u8 *frame = NULL;
    ae_size len = 0;

    pkt.opcode = 26672;

    AE_TEST_ASSERT(AE_OK == ae_base_packet_build(&pkt, &frame, &len));
    AE_TEST_ASSERT(frame != NULL);
    AE_TEST_ASSERT(len == 2 + 2 + 2 + 4 + 2);

    /* const1 = 0x4567 */
    AE_TEST_ASSERT(frame[0] == 0x45);
    AE_TEST_ASSERT(frame[1] == 0x67);
    /* opcode = 26672 = 0x6830 */
    AE_TEST_ASSERT(frame[2] == 0x68);
    AE_TEST_ASSERT(frame[3] == 0x30);
    /* header_len = 0 */
    AE_TEST_ASSERT(frame[4] == 0x00);
    AE_TEST_ASSERT(frame[5] == 0x00);
    /* data_len = 0 */
    AE_TEST_ASSERT(frame[6] == 0x00);
    AE_TEST_ASSERT(frame[7] == 0x00);
    AE_TEST_ASSERT(frame[8] == 0x00);
    AE_TEST_ASSERT(frame[9] == 0x00);
    /* const2 = 0x89ab */
    AE_TEST_ASSERT(frame[10] == 0x89);
    AE_TEST_ASSERT(frame[11] == 0xab);

    free(frame);
}

static void test_with_header_and_data(void)
{
    ae_base_packet_t pkt = {0};
    ae_u8 *frame = NULL;
    ae_size len = 0;
    ae_u8 header[4] = { 0xAA, 0xBB, 0xCC, 0xDD };
    ae_u8 data[3] = { 0x11, 0x22, 0x33 };

    pkt.opcode = 123;
    pkt.header = header;
    pkt.header_len = sizeof(header);
    pkt.data = data;
    pkt.data_len = sizeof(data);

    AE_TEST_ASSERT(AE_OK == ae_base_packet_build(&pkt, &frame, &len));
    /* 2+2+2+4 + 4 header + 3 data + 2 = 19 */
    AE_TEST_ASSERT(len == 19);

    /* opcode = 123 = 0x007B */
    AE_TEST_ASSERT(frame[2] == 0x00);
    AE_TEST_ASSERT(frame[3] == 0x7B);
    /* header_len = 4 */
    AE_TEST_ASSERT(frame[4] == 0x00);
    AE_TEST_ASSERT(frame[5] == 0x04);
    /* data_len = 3 */
    AE_TEST_ASSERT(frame[6] == 0x00);
    AE_TEST_ASSERT(frame[7] == 0x00);
    AE_TEST_ASSERT(frame[8] == 0x00);
    AE_TEST_ASSERT(frame[9] == 0x03);
    /* header bytes */
    AE_TEST_ASSERT(frame[10] == 0xAA);
    AE_TEST_ASSERT(frame[11] == 0xBB);
    AE_TEST_ASSERT(frame[12] == 0xCC);
    AE_TEST_ASSERT(frame[13] == 0xDD);
    /* data bytes */
    AE_TEST_ASSERT(frame[14] == 0x11);
    AE_TEST_ASSERT(frame[15] == 0x22);
    AE_TEST_ASSERT(frame[16] == 0x33);
    /* const2 = 0x89ab */
    AE_TEST_ASSERT(frame[17] == 0x89);
    AE_TEST_ASSERT(frame[18] == 0xab);

    free(frame);
}

static void test_null_handling(void)
{
    ae_u8 *frame = NULL;
    ae_size len = 0;

    AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_base_packet_build(NULL, &frame, &len));

    {
        ae_base_packet_t pkt = {0};
        pkt.opcode = 1;
        AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_base_packet_build(&pkt, NULL, &len));
        AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_base_packet_build(&pkt, &frame, NULL));
    }

    {
        ae_base_packet_t pkt = {0};
        pkt.opcode = 1;
        pkt.header = (const ae_u8 *) 1;
        pkt.header_len = 1;
        pkt.data = NULL;
        pkt.data_len = 1;
        AE_TEST_ASSERT(AE_ERR_INVALID_ARG == ae_base_packet_build(&pkt, &frame, &len));
    }
}

static void test_empty_with_flags(void)
{
    ae_base_packet_t pkt = {0};
    ae_u8 *frame = NULL;
    ae_size len = 0;

    pkt.opcode = 100;
    pkt.should_encrypt = true;
    pkt.use_dispatch_key = false;
    pkt.should_build_header = true;

    AE_TEST_ASSERT(AE_OK == ae_base_packet_build(&pkt, &frame, &len));
    AE_TEST_ASSERT(frame != NULL);
    AE_TEST_ASSERT(len == 2 + 2 + 2 + 4 + 2);

    free(frame);
}

/* ============================================================ */

static const ae_test_case_t test_cases[] = {
    {"Empty frame (no header, no data)", test_empty_frame},
    {"Frame with header and data", test_with_header_and_data},
    {"NULL handling", test_null_handling},
    {"Flags are preserved in build", test_empty_with_flags},
};

int main(void)
{
    ae_u32 failed = AE_TEST_SUITE(test_cases);

    printf("base_packet: %u assertions, %u failed\n",
           ae_test_run_count(), ae_test_fail_count());

    return failed != 0 ? 1 : 0;
}

/* ============================================================ */