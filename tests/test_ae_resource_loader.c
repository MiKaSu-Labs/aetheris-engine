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
 * @file test_ae_resource_loader.c
 *
 * @brief Unit tests for the resource loader module.
 *
 * Fixture files are written into a `test_resources` tree under the
 * working directory at runtime; nothing external is required.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "cJSON.h"

#include <aetheris/ae_resource_loader.h>

#include "ae_test.h"

/* ============================================================ */

static void make_dir(const char *path)
{
#if defined(_WIN32)
    (void)_mkdir(path);
#else
    (void)mkdir(path, 0755);
#endif
}

static void write_file(const char *path, const char *text)
{
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        AE_TEST_ASSERT(false);
        return;
    }
    fputs(text, fp);
    fclose(fp);
}

static void make_fixture_tree(void)
{
    make_dir("test_resources");
    make_dir("test_resources/Server");
    make_dir("test_resources/ExcelBinOutput");
}

/* ============================================================ */

static void test_priority_lookup(void)
{
    AE_TEST_ASSERT(ae_resource_load_priority_from_value(4) ==
                   AE_RESOURCE_LOAD_PRIORITY_HIGHEST);
    AE_TEST_ASSERT(ae_resource_load_priority_from_value(3) ==
                   AE_RESOURCE_LOAD_PRIORITY_HIGH);
    AE_TEST_ASSERT(ae_resource_load_priority_from_value(2) ==
                   AE_RESOURCE_LOAD_PRIORITY_NORMAL);
    AE_TEST_ASSERT(ae_resource_load_priority_from_value(1) ==
                   AE_RESOURCE_LOAD_PRIORITY_LOW);
    AE_TEST_ASSERT(ae_resource_load_priority_from_value(0) ==
                   AE_RESOURCE_LOAD_PRIORITY_LOWEST);
    AE_TEST_ASSERT(ae_resource_load_priority_from_value(99) ==
                   AE_RESOURCE_LOAD_PRIORITY_LOWEST);
    AE_TEST_ASSERT(ae_resource_load_priority_from_value(-1) ==
                   AE_RESOURCE_LOAD_PRIORITY_LOWEST);

    AE_TEST_ASSERT(strcmp(ae_resource_load_priority_name(
                              AE_RESOURCE_LOAD_PRIORITY_HIGHEST),
                          "HIGHEST") == 0);
    AE_TEST_ASSERT(strcmp(ae_resource_load_priority_name(
                              AE_RESOURCE_LOAD_PRIORITY_NORMAL),
                          "NORMAL") == 0);
    AE_TEST_ASSERT(strcmp(ae_resource_load_priority_name(
                              AE_RESOURCE_LOAD_PRIORITY_LOWEST),
                          "LOWEST") == 0);
    AE_TEST_ASSERT(ae_resource_load_priority_name(
                       (ae_resource_load_priority_t)99) == NULL);
}

static void test_resolve_server_over_excel(void)
{
    char path[512];

    /* Server root wins over ExcelBinOutput even for a different ext. */
    write_file("test_resources/Server/Item.json", "[]");
    write_file("test_resources/ExcelBinOutput/Item.tsj", "[]");
    AE_TEST_ASSERT(ae_resource_loader_resolve(
                       "test_resources", "Item.json", path, sizeof(path)) ==
                   AE_OK);
    AE_TEST_ASSERT(strcmp(path, "test_resources/Server/Item.json") == 0);

    /* Within one root, extension priority is tsj > json > tsv. */
    write_file("test_resources/ExcelBinOutput/Gadget.json", "[]");
    write_file("test_resources/ExcelBinOutput/Gadget.tsv", "[]");
    AE_TEST_ASSERT(ae_resource_loader_resolve(
                       "test_resources", "Gadget.json", path, sizeof(path)) ==
                   AE_OK);
    AE_TEST_ASSERT(strcmp(path, "test_resources/ExcelBinOutput/Gadget.json") ==
                   0);

    write_file("test_resources/ExcelBinOutput/Scene.tsj", "[]");
    write_file("test_resources/ExcelBinOutput/Scene.json", "[]");
    AE_TEST_ASSERT(ae_resource_loader_resolve(
                       "test_resources", "Scene.json", path, sizeof(path)) ==
                   AE_OK);
    AE_TEST_ASSERT(strcmp(path, "test_resources/ExcelBinOutput/Scene.tsj") ==
                   0);
}

static void test_resolve_missing(void)
{
    char path[512];
    char small[4];

    AE_TEST_ASSERT(ae_resource_loader_resolve(
                       "test_resources", "DoesNotExist.json",
                       path, sizeof(path)) == AE_ERR_NOT_FOUND);
    AE_TEST_ASSERT(strcmp(path,
                          "test_resources/Server/DoesNotExist.tsj") == 0);

    /* Out buffer too small must not overflow. */
    AE_TEST_ASSERT(ae_resource_loader_resolve(
                       "test_resources", "DoesNotExist.json",
                       small, sizeof(small)) == AE_ERR_BUFFER_TOO_SMALL);
}

/* ============================================================ */

typedef struct collect_ctx {
    ae_s32 values[16];
    ae_size count;
} collect_ctx_t;

static ae_error_t collect_number(const cJSON *object, void *user)
{
    collect_ctx_t *ctx = (collect_ctx_t *)user;
    if (cJSON_IsNumber(object) && ctx->count < AE_ARRAY_SIZE(ctx->values)) {
        ctx->values[ctx->count++] = (ae_s32)object->valuedouble;
    }
    return AE_OK;
}

static void test_load_json_array_elements(void)
{
    collect_ctx_t ctx;
    ae_error_t err;

    memset(&ctx, 0, sizeof(ctx));
    write_file("test_resources/ExcelBinOutput/Counts.json", "[1,2,3]");
    err = ae_resource_loader_load_json_array(
        "test_resources", "Counts.json", collect_number, &ctx);
    AE_TEST_ASSERT(err == AE_OK);
    AE_TEST_ASSERT(ctx.count == 3);
    AE_TEST_ASSERT(ctx.values[0] == 1);
    AE_TEST_ASSERT(ctx.values[1] == 2);
    AE_TEST_ASSERT(ctx.values[2] == 3);

    /* A missing file is not an error. */
    memset(&ctx, 0, sizeof(ctx));
    err = ae_resource_loader_load_json_array(
        "test_resources", "Missing.json", collect_number, &ctx);
    AE_TEST_ASSERT(err == AE_OK);
    AE_TEST_ASSERT(ctx.count == 0);
}

static void test_load_json_array_bad_input(void)
{
    collect_ctx_t ctx;
    ae_error_t err;

    memset(&ctx, 0, sizeof(ctx));
    write_file("test_resources/ExcelBinOutput/Broken.json", "{not json");
    err = ae_resource_loader_load_json_array(
        "test_resources", "Broken.json", collect_number, &ctx);
    AE_TEST_ASSERT(err == AE_ERR_BAD_DATA);

    memset(&ctx, 0, sizeof(ctx));
    write_file("test_resources/ExcelBinOutput/ObjectRoot.json",
               "{\"root\": []}");
    err = ae_resource_loader_load_json_array(
        "test_resources", "ObjectRoot.json", collect_number, &ctx);
    AE_TEST_ASSERT(err == AE_ERR_BAD_DATA);

    err = ae_resource_loader_load_json_array(
        "test_resources", "Counts.json", NULL, &ctx);
    AE_TEST_ASSERT(err == AE_ERR_INVALID_ARG);
}

/* ============================================================ */

static ae_error_t stub_load(const char *resources_dir,
                            const struct ae_resource_def *def)
{
    (void)resources_dir;
    (void)def;
    return AE_OK;
}

static const char *const k_stub_files[] = { "StubExcelConfigData.json" };

static const ae_resource_def_t k_stub_def = {
    .type_name = "StubType",
    .priority = AE_RESOURCE_LOAD_PRIORITY_NORMAL,
    .filenames = k_stub_files,
    .filename_count = AE_ARRAY_SIZE(k_stub_files),
    .load = stub_load,
};

static void test_register_duplicate(void)
{
    ae_resource_loader_reset();
    AE_TEST_ASSERT(ae_resource_loader_register(&k_stub_def) == AE_OK);
    AE_TEST_ASSERT(ae_resource_loader_register(&k_stub_def) ==
                   AE_ERR_ALREADY_EXISTS);

    {
        static const ae_resource_def_t null_name = {
            .type_name = NULL,
            .priority = AE_RESOURCE_LOAD_PRIORITY_NORMAL,
            .filenames = k_stub_files,
            .filename_count = 1,
            .load = stub_load,
        };
        AE_TEST_ASSERT(ae_resource_loader_register(&null_name) ==
                       AE_ERR_INVALID_ARG);
    }
    {
        static const ae_resource_def_t null_load = {
            .type_name = "NoLoad",
            .priority = AE_RESOURCE_LOAD_PRIORITY_LOW,
            .filenames = k_stub_files,
            .filename_count = 1,
            .load = NULL,
        };
        AE_TEST_ASSERT(ae_resource_loader_register(&null_load) ==
                       AE_ERR_INVALID_ARG);
    }
    ae_resource_loader_reset();
}

/* ============================================================ */

typedef struct record_ctx {
    char names[8][32];
    ae_size count;
} record_ctx_t;

static record_ctx_t g_record;

static ae_error_t record_load(const char *resources_dir,
                              const struct ae_resource_def *def)
{
    (void)resources_dir;
    if (g_record.count < AE_ARRAY_SIZE(g_record.names)) {
        size_t len = strlen(def->type_name);
        if (len >= sizeof(g_record.names[0])) {
            len = sizeof(g_record.names[0]) - 1u;
        }
        memcpy(g_record.names[g_record.count], def->type_name, len);
        g_record.names[g_record.count][len] = '\0';
        g_record.count++;
    }
    return AE_OK;
}

static ae_error_t failing_load(const char *resources_dir,
                               const struct ae_resource_def *def)
{
    (void)resources_dir;
    (void)def;
    return AE_ERR_IO;
}

static const char *const k_files_a[] = { "A.json" };
static const char *const k_files_b[] = { "B.json" };
static const char *const k_files_c[] = { "C.json" };
static const char *const k_files_f[] = { "F.json" };

static const ae_resource_def_t k_def_a = {
    .type_name = "TypeA",
    .priority = AE_RESOURCE_LOAD_PRIORITY_HIGH,
    .filenames = k_files_a,
    .filename_count = AE_ARRAY_SIZE(k_files_a),
    .load = record_load,
};

static const ae_resource_def_t k_def_b = {
    .type_name = "TypeB",
    .priority = AE_RESOURCE_LOAD_PRIORITY_LOW,
    .filenames = k_files_b,
    .filename_count = AE_ARRAY_SIZE(k_files_b),
    .load = record_load,
};

static const ae_resource_def_t k_def_c = {
    .type_name = "TypeC",
    .priority = AE_RESOURCE_LOAD_PRIORITY_HIGHEST,
    .filenames = k_files_c,
    .filename_count = AE_ARRAY_SIZE(k_files_c),
    .load = record_load,
};

static const ae_resource_def_t k_def_fail = {
    .type_name = "TypeFail",
    .priority = AE_RESOURCE_LOAD_PRIORITY_NORMAL,
    .filenames = k_files_f,
    .filename_count = AE_ARRAY_SIZE(k_files_f),
    .load = failing_load,
};

static void test_load_all_priority_order_and_reload(void)
{
    ae_error_t err;

    ae_resource_loader_reset();
    memset(&g_record, 0, sizeof(g_record));

    AE_TEST_ASSERT(ae_resource_loader_register(&k_def_a) == AE_OK);
    AE_TEST_ASSERT(ae_resource_loader_register(&k_def_b) == AE_OK);
    AE_TEST_ASSERT(ae_resource_loader_register(&k_def_c) == AE_OK);

    err = ae_resource_loader_load_all("test_resources", false);
    AE_TEST_ASSERT(err == AE_OK);
    AE_TEST_ASSERT(g_record.count == 3);
    AE_TEST_ASSERT(strcmp(g_record.names[0], "TypeC") == 0);
    AE_TEST_ASSERT(strcmp(g_record.names[1], "TypeA") == 0);
    AE_TEST_ASSERT(strcmp(g_record.names[2], "TypeB") == 0);

    /* Without reload, already-loaded definitions are skipped. */
    err = ae_resource_loader_load_all("test_resources", false);
    AE_TEST_ASSERT(err == AE_OK);
    AE_TEST_ASSERT(g_record.count == 3);

    /* A reload drives everything again. */
    err = ae_resource_loader_load_all("test_resources", true);
    AE_TEST_ASSERT(err == AE_OK);
    AE_TEST_ASSERT(g_record.count == 6);
}

static void test_load_all_collects_first_error(void)
{
    ae_error_t err;

    ae_resource_loader_reset();
    memset(&g_record, 0, sizeof(g_record));

    AE_TEST_ASSERT(ae_resource_loader_register(&k_def_fail) == AE_OK);
    AE_TEST_ASSERT(ae_resource_loader_register(&k_def_b) == AE_OK);

    err = ae_resource_loader_load_all("test_resources", false);
    AE_TEST_ASSERT(err == AE_ERR_IO);
    /* Loading continues past the failure. */
    AE_TEST_ASSERT(g_record.count == 1);
    AE_TEST_ASSERT(strcmp(g_record.names[0], "TypeB") == 0);

    ae_resource_loader_reset();
}

/* ============================================================ */

int main(void)
{
    make_fixture_tree();

    static const ae_test_case_t suite[] = {
        {"priority_lookup", test_priority_lookup},
        {"resolve_server_over_excel", test_resolve_server_over_excel},
        {"resolve_missing", test_resolve_missing},
        {"load_json_array_elements", test_load_json_array_elements},
        {"load_json_array_bad_input", test_load_json_array_bad_input},
        {"register_duplicate", test_register_duplicate},
        {"load_all_priority_order_and_reload",
         test_load_all_priority_order_and_reload},
        {"load_all_collects_first_error", test_load_all_collects_first_error},
    };

    ae_u32 failed = AE_TEST_SUITE(suite);

    return (int)failed;
}