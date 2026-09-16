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
 * @file ae_config.c
 *
 * @brief Config model defaults, cJSON round-tripping, and migration.
 *
 * @copyright (c) 2026 Douglas Kitagawa
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aetheris/ae_config.h"
#include "cJSON.h"

/* ============================================================ */

static void str_cpy(char *dst, ae_size dst_size, const char *src)
{
    if (dst_size == 0) {
        return;
    }
    (void)strncpy(dst, src ? src : "", dst_size);
    dst[dst_size - 1] = '\0';
}

/* ============================================================ */

void ae_config_init(ae_config_t *config)
{
    ae_s32 i;

    if (!config) {
        return;
    }
    memset(config, 0, sizeof(*config));

    /* folderStructure */
    str_cpy(config->folder_structure.resources, AE_CONFIG_STR_LEN,
            "./resources/");
    str_cpy(config->folder_structure.data, AE_CONFIG_STR_LEN, "./data/");
    str_cpy(config->folder_structure.packets, AE_CONFIG_STR_LEN,
            "./packets/");
    str_cpy(config->folder_structure.scripts, AE_CONFIG_STR_LEN,
            "resources:Scripts/");
    str_cpy(config->folder_structure.plugins, AE_CONFIG_STR_LEN,
            "./plugins/");
    str_cpy(config->folder_structure.cache, AE_CONFIG_STR_LEN,
            "./cache/");

    /* databaseInfo */
    str_cpy(config->database_info.server.connection_uri,
            AE_CONFIG_STR_LEN, "mongodb://localhost:27017");
    str_cpy(config->database_info.server.collection, AE_CONFIG_STR_LEN,
            "grasscutter");
    str_cpy(config->database_info.game.connection_uri,
            AE_CONFIG_STR_LEN, "mongodb://localhost:27017");
    str_cpy(config->database_info.game.collection, AE_CONFIG_STR_LEN,
            "grasscutter");

    /* language */
    str_cpy(config->language.language, AE_CONFIG_NAME_LEN, "en_US");
    str_cpy(config->language.fallback, AE_CONFIG_NAME_LEN, "en_US");
    str_cpy(config->language.document, AE_CONFIG_NAME_LEN, "EN");

    /* account */
    config->account.auto_create = false;
    config->account.experimental_real_password = false;
    config->account.default_permissions_count = 0;
    config->account.max_player = -1;

    /* server */
    config->server.debug_whitelist_count = 0;
    config->server.debug_blacklist_count = 0;
    config->server.run_mode = AE_CONFIG_RUN_HYBRID;
    config->server.log_commands = false;
    config->server.fast_require = true;

    /* server.http */
    config->server.http.start_immediately = false;
    str_cpy(config->server.http.bind_address, AE_CONFIG_STR_LEN,
            "0.0.0.0");
    config->server.http.bind_port = 443;
    str_cpy(config->server.http.access_address, AE_CONFIG_STR_LEN,
            "127.0.0.1");
    config->server.http.access_port = 0;

    config->server.http.encryption.use_encryption = true;
    config->server.http.encryption.use_in_routing = true;
    str_cpy(config->server.http.encryption.keystore, AE_CONFIG_STR_LEN,
            "./keystore.p12");
    str_cpy(config->server.http.encryption.keystore_password,
            AE_CONFIG_STR_LEN, "123456");

    config->server.http.policies.cors.enabled = true;
    str_cpy(config->server.http.policies.cors.allowed_origins[0],
            AE_CONFIG_STR_LEN, "*");
    config->server.http.policies.cors.allowed_origins_count = 1;

    str_cpy(config->server.http.files.index_file, AE_CONFIG_STR_LEN,
            "./index.html");
    str_cpy(config->server.http.files.error_file, AE_CONFIG_STR_LEN,
            "./404.html");

    /* server.game */
    str_cpy(config->server.game.bind_address, AE_CONFIG_STR_LEN,
            "0.0.0.0");
    config->server.game.bind_port = 22102;
    str_cpy(config->server.game.access_address, AE_CONFIG_STR_LEN,
            "127.0.0.1");
    config->server.game.access_port = 0;
    config->server.game.use_unique_packet_key = true;
    config->server.game.load_entities_for_player_range = 300;
    config->server.game.enable_script_in_big_world = true;
    config->server.game.enable_console = true;
    config->server.game.kcp_interval = 20;
    config->server.game.log_packets = AE_CONFIG_DEBUG_NONE;
    config->server.game.is_show_packet_payload = false;
    config->server.game.is_show_loop_packets = false;
    config->server.game.cache_scene_entities_every_run = false;

    /* game.gameOptions */
    config->server.game.game_options.inventory_limits.weapons = 2000;
    config->server.game.game_options.inventory_limits.relics = 2000;
    config->server.game.game_options.inventory_limits.materials = 2000;
    config->server.game.game_options.inventory_limits.furniture = 2000;
    config->server.game.game_options.inventory_limits.all = 30000;
    config->server.game.game_options.avatar_limits.single_player_team = 4;
    config->server.game.game_options.avatar_limits.multiplayer_team = 4;
    config->server.game.game_options.scene_entity_limit = 1000;
    config->server.game.game_options.watch_gacha_config = false;
    config->server.game.game_options.enable_shop_items = true;
    config->server.game.game_options.stamina_usage = true;
    config->server.game.game_options.energy_usage = true;
    config->server.game.game_options.fishhook_teleport = true;
    config->server.game.game_options.trial_costumes = false;
    config->server.game.game_options.force_legacy_drops = true;
    config->server.game.game_options.questing.enabled = true;
    config->server.game.game_options.resin_options.resin_usage = false;
    config->server.game.game_options.resin_options.cap = 160;
    config->server.game.game_options.resin_options.recharge_time = 480;
    config->server.game.game_options.rates.adventure_exp = 1.0F;
    config->server.game.game_options.rates.mora = 1.0F;
    config->server.game.game_options.rates.ley_lines = 1.0F;

    config->server.game.game_options.handbook.enable = false;
    config->server.game.game_options.handbook.allow_commands = true;
    config->server.game.game_options.handbook.limits.enabled = false;
    config->server.game.game_options.handbook.limits.interval = 3;
    config->server.game.game_options.handbook.limits.max_requests = 10;
    config->server.game.game_options.handbook.limits.max_entities = 25;
    config->server.game.game_options.handbook.server.enforced = false;
    str_cpy(config->server.game.game_options.handbook.server.address,
            AE_CONFIG_STR_LEN, "127.0.0.1");
    config->server.game.game_options.handbook.server.port = 443;
    config->server.game.game_options.handbook.server.can_change = true;

    /* game.joinOptions */
    {
        static const ae_s32 welcome[] = { 2007, 1002, 4010 };
        config->server.game.join_options.welcome_emotes_count = 3;
        for (i = 0; i < 3; ++i) {
            config->server.game.join_options.welcome_emotes[i] =
                welcome[i];
        }
    }
    str_cpy(config->server.game.join_options.welcome_message,
            AE_CONFIG_STR_LEN, "Welcome to a Grasscutter server.");
    str_cpy(config->server.game.join_options.welcome_mail.title,
            AE_CONFIG_STR_LEN, "Welcome to Grasscutter!");
    str_cpy(config->server.game.join_options.welcome_mail.sender,
            AE_CONFIG_STR_LEN, "Lawnmower");
    str_cpy(config->server.game.join_options.welcome_mail.content, 1024,
            "Hi there!\r\n"
            "First of all, welcome to Grasscutter. If you have any "
            "issues, please let us know so that Lawnmower can help "
            "you! \r\n"
            "\r\n"
            "Check out our:\r\n"
            "<type=\"browser\" text=\"Discord\" "
            "href=\"https://discord.gg/T5vZU6UyeG\"/>\r\n");
    config->server.game.join_options.welcome_mail.items_count = 2;
    config->server.game.join_options.welcome_mail.items[0].item_id = 13509;
    config->server.game.join_options.welcome_mail.items[0].item_count = 1;
    config->server.game.join_options.welcome_mail.items[0].item_level = 1;
    config->server.game.join_options.welcome_mail.items[1].item_id = 201;
    config->server.game.join_options.welcome_mail.items[1].item_count =
        99999;
    config->server.game.join_options.welcome_mail.items[1].item_level = 1;

    /* game.serverAccount */
    config->server.game.console_account.avatar_id = 10000007;
    config->server.game.console_account.name_card_id = 210001;
    config->server.game.console_account.adventure_rank = 1;
    config->server.game.console_account.world_level = 0;
    str_cpy(config->server.game.console_account.nick_name,
            AE_CONFIG_NAME_LEN, "Server");
    str_cpy(config->server.game.console_account.signature,
            AE_CONFIG_STR_LEN, "Welcome to Grasscutter!");

    /* game.visionOptions */
    {
        static const struct {
            const char *name;
            ae_s32 range;
            ae_s32 width;
        } visions[] = {
            { "VISION_LEVEL_NORMAL",        80,    20 },
            { "VISION_LEVEL_LITTLE_REMOTE", 16,    40 },
            { "VISION_LEVEL_REMOTE",        1000,  250 },
            { "VISION_LEVEL_SUPER",         4000,  1000 },
            { "VISION_LEVEL_NEARBY",        40,    20 },
            { "VISION_LEVEL_SUPER_NEARBY",  20,    20 }
        };
        ae_u32 n = (ae_u32)(sizeof(visions) / sizeof(visions[0]));
        config->server.game.vision_options_count = n;
        for (i = 0; (ae_s32)i < (ae_s32)n; ++i) {
            str_cpy(config->server.game.vision_options[i].name,
                    AE_CONFIG_NAME_LEN, visions[i].name);
            config->server.game.vision_options[i].vision_range =
                visions[i].range;
            config->server.game.vision_options[i].grid_width =
                visions[i].width;
        }
    }

    /* server.dispatch */
    config->server.dispatch.regions_count = 0;
    str_cpy(config->server.dispatch.dispatch_url, AE_CONFIG_STR_LEN,
            "ws://127.0.0.1:1111");
    str_cpy(config->server.dispatch.encryption_key, AE_CONFIG_STR_LEN, "");
    str_cpy(config->server.dispatch.dispatch_key, AE_CONFIG_STR_LEN, "");
    str_cpy(config->server.dispatch.default_name, AE_CONFIG_NAME_LEN,
            "Grasscutter");
    config->server.dispatch.log_requests = AE_CONFIG_DEBUG_NONE;

    /* server.debugMode */
    config->server.debug_mode.server_logger_level = AE_CONFIG_LOG_DEBUG;
    config->server.debug_mode.services_logger_level = AE_CONFIG_LOG_INFO;
    config->server.debug_mode.log_packets = AE_CONFIG_DEBUG_ALL;
    config->server.debug_mode.is_show_packet_payload = false;
    config->server.debug_mode.is_show_loop_packets = false;
    config->server.debug_mode.log_requests = AE_CONFIG_DEBUG_ALL;

    config->version = AE_CONFIG_VERSION;
}

/* ============================================================ */

static const char *const s_run_names[] = {
    "HYBRID", "DISPATCH_ONLY", "GAME_ONLY"
};
static const char *const s_debug_names[] = {
    "ALL", "MISSING", "WHITELIST", "BLACKLIST", "NONE"
};
static const char *const s_log_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "OFF"
};

#define ARRAY_NAME_COUNT(a) ((ae_s32)(sizeof(a) / sizeof(a[0])))

static ae_s32 find_name(const char *const *names, ae_s32 count,
                        const char *needle, ae_s32 dflt)
{
    ae_s32 i;

    for (i = 0; i < count; ++i) {
        if (strcmp(names[i], needle) == 0) {
            return i;
        }
    }
    return dflt;
}

static ae_config_run_mode_t run_from_name(const cJSON *obj,
                                          const char *key,
                                          ae_config_run_mode_t dflt)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsString(it) && it->valuestring) {
        return (ae_config_run_mode_t)find_name(
            s_run_names, ARRAY_NAME_COUNT(s_run_names),
            it->valuestring, (ae_s32)dflt);
    }
    return dflt;
}

static ae_config_debug_mode_t dbg_from_name(const cJSON *obj,
                                            const char *key,
                                            ae_config_debug_mode_t dflt)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsString(it) && it->valuestring) {
        return (ae_config_debug_mode_t)find_name(
            s_debug_names, ARRAY_NAME_COUNT(s_debug_names),
            it->valuestring, (ae_s32)dflt);
    }
    return dflt;
}

static ae_config_log_level_t log_from_name(const cJSON *obj,
                                           const char *key,
                                           ae_config_log_level_t dflt)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsString(it) && it->valuestring) {
        return (ae_config_log_level_t)find_name(
            s_log_names, ARRAY_NAME_COUNT(s_log_names),
            it->valuestring, (ae_s32)dflt);
    }
    return dflt;
}

/* ============================================================ */

static const cJSON *child(const cJSON *obj, const char *key)
{
    return cJSON_GetObjectItemCaseSensitive(obj, key);
}

static ae_bool js_bool(const cJSON *obj, const char *key, ae_bool dflt)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsBool(it)) {
        return cJSON_IsTrue(it);
    }
    return dflt;
}

static ae_s32 js_int(const cJSON *obj, const char *key, ae_s32 dflt)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsNumber(it)) {
        return it->valueint;
    }
    return dflt;
}

static float js_float(const cJSON *obj, const char *key, float dflt)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsNumber(it)) {
        return (float)it->valuedouble;
    }
    return dflt;
}

static void js_string(char *dst, ae_size size, const cJSON *obj,
                      const char *key)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);

    if (cJSON_IsString(it) && it->valuestring) {
        str_cpy(dst, size, it->valuestring);
    }
}

#define JS_BOOL_F(dst, obj, key) \
    ((dst) = js_bool((obj), (key), (dst)))
#define JS_INT_F(dst, obj, key) \
    ((dst) = js_int((obj), (key), (dst)))
#define JS_FLOAT_F(dst, obj, key) \
    ((dst) = js_float((obj), (key), (dst)))
#define JS_STR_F(dst, obj, key) \
    (js_string((dst), sizeof((dst)), (obj), (key)))
#define JS_RUN_F(dst, obj, key) \
    ((dst) = run_from_name((obj), (key), (dst)))
#define JS_DBG_F(dst, obj, key) \
    ((dst) = dbg_from_name((obj), (key), (dst)))
#define JS_LOG_F(dst, obj, key) \
    ((dst) = log_from_name((obj), (key), (dst)))

/* ============================================================ */

static void load_str_array(char *arr, ae_size elem_size, ae_u32 cap,
                           ae_u32 *count_out, const cJSON *obj,
                           const char *key)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);
    cJSON *elem;
    ae_u32 n = 0;

    if (!cJSON_IsArray(it)) {
        return;
    }
    cJSON_ArrayForEach(elem, it) {
        if (n >= cap) {
            break;
        }
        if (cJSON_IsString(elem) && elem->valuestring) {
            str_cpy(arr + (ae_size)n * elem_size, elem_size,
                    elem->valuestring);
            ++n;
        }
    }
    *count_out = n;
}

static void load_int_array(ae_s32 *arr, ae_u32 cap, ae_u32 *count_out,
                           const cJSON *obj, const char *key)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);
    cJSON *elem;
    ae_u32 n = 0;

    if (!cJSON_IsArray(it)) {
        return;
    }
    cJSON_ArrayForEach(elem, it) {
        if (n >= cap) {
            break;
        }
        if (cJSON_IsNumber(elem)) {
            arr[n] = elem->valueint;
            ++n;
        }
    }
    *count_out = n;
}

static void load_mail_items(ae_config_mail_item_t *items, ae_u32 cap,
                            ae_u32 *count_out, const cJSON *obj,
                            const char *key)
{
    const cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, key);
    cJSON *elem;
    ae_u32 n = 0;

    if (!cJSON_IsArray(it)) {
        return;
    }
    cJSON_ArrayForEach(elem, it) {
        if (n >= cap) {
            break;
        }
        if (cJSON_IsObject(elem)) {
            items[n].item_id = js_int(elem, "itemId",
                                      items[n].item_id);
            items[n].item_count = js_int(elem, "itemCount",
                                         items[n].item_count);
            items[n].item_level = js_int(elem, "itemLevel",
                                         items[n].item_level);
            ++n;
        }
    }
    *count_out = n;
}

static void load_vision(ae_config_vision_options_t *vis, ae_u32 cap,
                        ae_u32 *count_out, const cJSON *arr)
{
    cJSON *elem;
    ae_u32 n = 0;

    cJSON_ArrayForEach(elem, arr) {
        if (n >= cap) {
            break;
        }
        if (cJSON_IsObject(elem)) {
            js_string(vis[n].name, sizeof(vis[n].name), elem, "name");
            vis[n].vision_range = js_int(elem, "visionRange",
                                         vis[n].vision_range);
            vis[n].grid_width = js_int(elem, "gridWidth",
                                       vis[n].grid_width);
            ++n;
        }
    }
    *count_out = n;
}

/* ============================================================ */

static void load_structure(ae_config_structure_t *cfg, const cJSON *obj)
{
    JS_STR_F(cfg->resources, obj, "resources");
    JS_STR_F(cfg->data, obj, "data");
    JS_STR_F(cfg->packets, obj, "packets");
    JS_STR_F(cfg->scripts, obj, "scripts");
    JS_STR_F(cfg->plugins, obj, "plugins");
    JS_STR_F(cfg->cache, obj, "cache");
}

static void load_database(ae_config_database_t *cfg, const cJSON *obj)
{
    const cJSON *it;

    it = child(obj, "server");
    if (cJSON_IsObject(it)) {
        JS_STR_F(cfg->server.connection_uri, it, "connectionUri");
        JS_STR_F(cfg->server.collection, it, "collection");
    }
    it = child(obj, "game");
    if (cJSON_IsObject(it)) {
        JS_STR_F(cfg->game.connection_uri, it, "connectionUri");
        JS_STR_F(cfg->game.collection, it, "collection");
    }
}

static void load_language(ae_config_language_t *cfg, const cJSON *obj)
{
    JS_STR_F(cfg->language, obj, "language");
    JS_STR_F(cfg->fallback, obj, "fallback");
    JS_STR_F(cfg->document, obj, "document");
}

static void load_account(ae_config_account_t *cfg, const cJSON *obj)
{
    JS_BOOL_F(cfg->auto_create, obj, "autoCreate");
    JS_BOOL_F(cfg->experimental_real_password, obj,
              "EXPERIMENTAL_RealPassword");
    load_str_array((char *)cfg->default_permissions, AE_CONFIG_NAME_LEN,
                   AE_CONFIG_MAX_PERMISSIONS,
                   &cfg->default_permissions_count, obj,
                   "defaultPermissions");
    JS_INT_F(cfg->max_player, obj, "maxPlayer");
}

static void load_http(ae_config_http_t *cfg, const cJSON *obj)
{
    const cJSON *it;

    JS_BOOL_F(cfg->start_immediately, obj, "startImmediately");
    JS_STR_F(cfg->bind_address, obj, "bindAddress");
    JS_INT_F(cfg->bind_port, obj, "bindPort");
    JS_STR_F(cfg->access_address, obj, "accessAddress");
    JS_INT_F(cfg->access_port, obj, "accessPort");

    it = child(obj, "encryption");
    if (cJSON_IsObject(it)) {
        JS_BOOL_F(cfg->encryption.use_encryption, it, "useEncryption");
        JS_BOOL_F(cfg->encryption.use_in_routing, it, "useInRouting");
        JS_STR_F(cfg->encryption.keystore, it, "keystore");
        JS_STR_F(cfg->encryption.keystore_password, it,
                 "keystorePassword");
    }
    it = child(obj, "policies");
    if (cJSON_IsObject(it)) {
        const cJSON *cors = child(it, "cors");

        if (cJSON_IsObject(cors)) {
            JS_BOOL_F(cfg->policies.cors.enabled, cors, "enabled");
            load_str_array((char *)cfg->policies.cors.allowed_origins,
                           AE_CONFIG_STR_LEN, AE_CONFIG_MAX_ORIGINS,
                           &cfg->policies.cors.allowed_origins_count,
                           cors, "allowedOrigins");
        }
    }
    it = child(obj, "files");
    if (cJSON_IsObject(it)) {
        JS_STR_F(cfg->files.index_file, it, "indexFile");
        JS_STR_F(cfg->files.error_file, it, "errorFile");
    }
}

static void load_inventory_limits(ae_config_inventory_limits_t *cfg,
                                  const cJSON *obj)
{
    JS_INT_F(cfg->weapons, obj, "weapons");
    JS_INT_F(cfg->relics, obj, "relics");
    JS_INT_F(cfg->materials, obj, "materials");
    JS_INT_F(cfg->furniture, obj, "furniture");
    JS_INT_F(cfg->all, obj, "all");
}

static void load_avatar_limits(ae_config_avatar_limits_t *cfg,
                               const cJSON *obj)
{
    JS_INT_F(cfg->single_player_team, obj, "singlePlayerTeam");
    JS_INT_F(cfg->multiplayer_team, obj, "multiplayerTeam");
}

static void load_rates(ae_config_rates_t *cfg, const cJSON *obj)
{
    JS_FLOAT_F(cfg->adventure_exp, obj, "adventureExp");
    JS_FLOAT_F(cfg->mora, obj, "mora");
    JS_FLOAT_F(cfg->ley_lines, obj, "leyLines");
}

static void load_resin_options(ae_config_resin_options_t *cfg,
                               const cJSON *obj)
{
    JS_BOOL_F(cfg->resin_usage, obj, "resinUsage");
    JS_INT_F(cfg->cap, obj, "cap");
    JS_INT_F(cfg->recharge_time, obj, "rechargeTime");
}

static void load_game_options(ae_config_game_options_t *cfg,
                              const cJSON *obj)
{
    const cJSON *it;

    it = child(obj, "inventoryLimits");
    if (cJSON_IsObject(it)) {
        load_inventory_limits(&cfg->inventory_limits, it);
    }
    it = child(obj, "avatarLimits");
    if (cJSON_IsObject(it)) {
        load_avatar_limits(&cfg->avatar_limits, it);
    }
    JS_INT_F(cfg->scene_entity_limit, obj, "sceneEntityLimit");
    JS_BOOL_F(cfg->watch_gacha_config, obj, "watchGachaConfig");
    JS_BOOL_F(cfg->enable_shop_items, obj, "enableShopItems");
    JS_BOOL_F(cfg->stamina_usage, obj, "staminaUsage");
    JS_BOOL_F(cfg->energy_usage, obj, "energyUsage");
    JS_BOOL_F(cfg->fishhook_teleport, obj, "fishhookTeleport");
    JS_BOOL_F(cfg->trial_costumes, obj, "trialCostumes");
    JS_BOOL_F(cfg->force_legacy_drops, obj, "forceLegacyDrops");

    it = child(obj, "questing");
    if (!cJSON_IsObject(it)) {
        it = child(obj, "questOptions");
    }
    if (cJSON_IsObject(it)) {
        JS_BOOL_F(cfg->questing.enabled, it, "enabled");
    }
    it = child(obj, "resinOptions");
    if (cJSON_IsObject(it)) {
        load_resin_options(&cfg->resin_options, it);
    }
    it = child(obj, "rates");
    if (cJSON_IsObject(it)) {
        load_rates(&cfg->rates, it);
    }
    it = child(obj, "handbook");
    if (cJSON_IsObject(it)) {
        const cJSON *lim = child(it, "limits");
        const cJSON *srv = child(it, "server");

        JS_BOOL_F(cfg->handbook.enable, it, "enable");
        JS_BOOL_F(cfg->handbook.allow_commands, it, "allowCommands");
        if (cJSON_IsObject(lim)) {
            JS_BOOL_F(cfg->handbook.limits.enabled, lim, "enabled");
            JS_INT_F(cfg->handbook.limits.interval, lim, "interval");
            JS_INT_F(cfg->handbook.limits.max_requests, lim,
                     "maxRequests");
            JS_INT_F(cfg->handbook.limits.max_entities, lim,
                     "maxEntities");
        }
        if (cJSON_IsObject(srv)) {
            JS_BOOL_F(cfg->handbook.server.enforced, srv, "enforced");
            JS_STR_F(cfg->handbook.server.address, srv, "address");
            JS_INT_F(cfg->handbook.server.port, srv, "port");
            JS_BOOL_F(cfg->handbook.server.can_change, srv,
                      "canChange");
        }
    }
}

static void load_console_account(ae_config_console_account_t *cfg,
                                 const cJSON *obj)
{
    JS_INT_F(cfg->avatar_id, obj, "avatarId");
    JS_INT_F(cfg->name_card_id, obj, "nameCardId");
    JS_INT_F(cfg->adventure_rank, obj, "adventureRank");
    JS_INT_F(cfg->world_level, obj, "worldLevel");
    JS_STR_F(cfg->nick_name, obj, "nickName");
    JS_STR_F(cfg->signature, obj, "signature");
}

static void load_join_options(ae_config_join_options_t *cfg,
                              const cJSON *obj)
{
    const cJSON *it;

    load_int_array(cfg->welcome_emotes, 4,
                   &cfg->welcome_emotes_count, obj, "welcomeEmotes");
    JS_STR_F(cfg->welcome_message, obj, "welcomeMessage");
    it = child(obj, "welcomeMail");
    if (cJSON_IsObject(it)) {
        JS_STR_F(cfg->welcome_mail.title, it, "title");
        JS_STR_F(cfg->welcome_mail.content, it, "content");
        JS_STR_F(cfg->welcome_mail.sender, it, "sender");
        load_mail_items(cfg->welcome_mail.items, AE_CONFIG_MAX_MAIL_ITEMS,
                        &cfg->welcome_mail.items_count, it, "items");
    }
}

static void load_game(ae_config_game_t *cfg, const cJSON *obj)
{
    const cJSON *it;

    JS_STR_F(cfg->bind_address, obj, "bindAddress");
    JS_INT_F(cfg->bind_port, obj, "bindPort");
    JS_STR_F(cfg->access_address, obj, "accessAddress");
    JS_INT_F(cfg->access_port, obj, "accessPort");
    JS_BOOL_F(cfg->use_unique_packet_key, obj, "useUniquePacketKey");
    JS_INT_F(cfg->load_entities_for_player_range, obj,
             "loadEntitiesForPlayerRange");
    JS_BOOL_F(cfg->enable_script_in_big_world, obj,
              "enableScriptInBigWorld");
    JS_BOOL_F(cfg->enable_console, obj, "enableConsole");
    JS_INT_F(cfg->kcp_interval, obj, "kcpInterval");
    JS_DBG_F(cfg->log_packets, obj, "logPackets");
    JS_BOOL_F(cfg->is_show_packet_payload, obj, "isShowPacketPayload");
    JS_BOOL_F(cfg->is_show_loop_packets, obj, "isShowLoopPackets");
    JS_BOOL_F(cfg->cache_scene_entities_every_run, obj,
              "cacheSceneEntitiesEveryRun");

    it = child(obj, "gameOptions");
    if (cJSON_IsObject(it)) {
        load_game_options(&cfg->game_options, it);
    }
    it = child(obj, "joinOptions");
    if (cJSON_IsObject(it)) {
        load_join_options(&cfg->join_options, it);
    }
    it = child(obj, "serverAccount");
    if (cJSON_IsObject(it)) {
        load_console_account(&cfg->console_account, it);
    }
    it = child(obj, "visionOptions");
    if (cJSON_IsArray(it)) {
        load_vision(cfg->vision_options, AE_CONFIG_MAX_VISION,
                    &cfg->vision_options_count, it);
    }
}

static void load_dispatch(ae_config_dispatch_t *cfg, const cJSON *obj)
{
    const cJSON *it;
    cJSON *elem;
    ae_u32 n = 0;

    JS_STR_F(cfg->dispatch_url, obj, "dispatchUrl");
    JS_STR_F(cfg->encryption_key, obj, "encryptionKey");
    JS_STR_F(cfg->dispatch_key, obj, "dispatchKey");
    JS_STR_F(cfg->default_name, obj, "defaultName");
    JS_DBG_F(cfg->log_requests, obj, "logRequests");

    it = child(obj, "regions");
    if (cJSON_IsArray(it)) {
        cJSON_ArrayForEach(elem, it) {
            if (n >= AE_CONFIG_MAX_REGIONS) {
                break;
            }
            if (cJSON_IsObject(elem)) {
                js_string(cfg->regions[n].name,
                          sizeof(cfg->regions[n].name), elem, "Name");
                js_string(cfg->regions[n].title,
                          sizeof(cfg->regions[n].title), elem, "Title");
                js_string(cfg->regions[n].ip,
                          sizeof(cfg->regions[n].ip), elem, "Ip");
                cfg->regions[n].port = js_int(elem, "Port",
                                              cfg->regions[n].port);
                ++n;
            }
        }
        cfg->regions_count = n;
    }
}

static void load_debug_mode(ae_config_debug_options_t *cfg,
                            const cJSON *obj)
{
    JS_LOG_F(cfg->server_logger_level, obj, "serverLoggerLevel");
    JS_LOG_F(cfg->services_logger_level, obj, "servicesLoggersLevel");
    JS_DBG_F(cfg->log_packets, obj, "logPackets");
    JS_BOOL_F(cfg->is_show_packet_payload, obj, "isShowPacketPayload");
    JS_BOOL_F(cfg->is_show_loop_packets, obj, "isShowLoopPackets");
    JS_DBG_F(cfg->log_requests, obj, "logRequests");
}

static void load_server(ae_config_server_t *cfg, const cJSON *obj)
{
    const cJSON *it;

    JS_RUN_F(cfg->run_mode, obj, "runMode");
    JS_BOOL_F(cfg->log_commands, obj, "logCommands");
    JS_BOOL_F(cfg->fast_require, obj, "fastRequire");
    load_int_array(cfg->debug_whitelist, 64,
                   &cfg->debug_whitelist_count, obj, "debugWhitelist");
    load_int_array(cfg->debug_blacklist, 64,
                   &cfg->debug_blacklist_count, obj, "debugBlacklist");

    it = child(obj, "http");
    if (cJSON_IsObject(it)) {
        load_http(&cfg->http, it);
    }
    it = child(obj, "game");
    if (cJSON_IsObject(it)) {
        load_game(&cfg->game, it);
    }
    it = child(obj, "dispatch");
    if (cJSON_IsObject(it)) {
        load_dispatch(&cfg->dispatch, it);
    }
    it = child(obj, "debugMode");
    if (cJSON_IsObject(it)) {
        load_debug_mode(&cfg->debug_mode, it);
    }
}

/* ============================================================ */

ae_error_t ae_config_load_string(ae_config_t *config, const char *json)
{
    cJSON *root;
    const cJSON *it;

    if (!config || !json) {
        return AE_ERR_INVALID_ARG;
    }
    root = cJSON_Parse(json);
    if (!root) {
        return AE_ERR_BAD_DATA;
    }

    it = child(root, "folderStructure");
    if (cJSON_IsObject(it)) {
        load_structure(&config->folder_structure, it);
    }
    it = child(root, "databaseInfo");
    if (cJSON_IsObject(it)) {
        load_database(&config->database_info, it);
    }
    it = child(root, "language");
    if (cJSON_IsObject(it)) {
        load_language(&config->language, it);
    }
    it = child(root, "account");
    if (cJSON_IsObject(it)) {
        load_account(&config->account, it);
    }
    it = child(root, "server");
    if (cJSON_IsObject(it)) {
        load_server(&config->server, it);
    }
    it = child(root, "version");
    if (cJSON_IsNumber(it)) {
        config->version = (ae_u32)it->valueint;
    }

    cJSON_Delete(root);
    (void)ae_config_update(config);
    return AE_OK;
}

/* ============================================================ */

static void save_structure(cJSON *obj, const ae_config_structure_t *cfg)
{
    cJSON_AddStringToObject(obj, "resources", cfg->resources);
    cJSON_AddStringToObject(obj, "data", cfg->data);
    cJSON_AddStringToObject(obj, "packets", cfg->packets);
    cJSON_AddStringToObject(obj, "scripts", cfg->scripts);
    cJSON_AddStringToObject(obj, "plugins", cfg->plugins);
    cJSON_AddStringToObject(obj, "cache", cfg->cache);
}

static void save_database(cJSON *obj, const ae_config_database_t *cfg)
{
    cJSON *server = cJSON_AddObjectToObject(obj, "server");
    cJSON *game = cJSON_AddObjectToObject(obj, "game");

    cJSON_AddStringToObject(server, "connectionUri",
                                         cfg->server.connection_uri);
    cJSON_AddStringToObject(server, "collection",
                                         cfg->server.collection);
    cJSON_AddStringToObject(game, "connectionUri",
                                         cfg->game.connection_uri);
    cJSON_AddStringToObject(game, "collection",
                                         cfg->game.collection);
}

static void save_language(cJSON *obj, const ae_config_language_t *cfg)
{
    cJSON_AddStringToObject(obj, "language", cfg->language);
    cJSON_AddStringToObject(obj, "fallback", cfg->fallback);
    cJSON_AddStringToObject(obj, "document", cfg->document);
}

static void save_account(cJSON *obj, const ae_config_account_t *cfg)
{
    cJSON *perms;
    ae_u32 i;

    cJSON_AddBoolToObject(obj, "autoCreate",
                                       cfg->auto_create);
    cJSON_AddBoolToObject(obj, "EXPERIMENTAL_RealPassword",
                                       cfg->experimental_real_password);
    perms = cJSON_AddArrayToObject(obj, "defaultPermissions");
    for (i = 0; i < cfg->default_permissions_count; ++i) {
        cJSON_AddItemToArray(perms,
                             cJSON_CreateString(
                                 cfg->default_permissions[i]));
    }
    cJSON_AddNumberToObject(obj, "maxPlayer",
                                         (double)cfg->max_player);
}

static void save_encryption(cJSON *obj, const ae_config_encryption_t *cfg)
{
    cJSON_AddBoolToObject(obj, "useEncryption",
                                       cfg->use_encryption);
    cJSON_AddBoolToObject(obj, "useInRouting",
                                       cfg->use_in_routing);
    cJSON_AddStringToObject(obj, "keystore", cfg->keystore);
    cJSON_AddStringToObject(obj, "keystorePassword",
                                         cfg->keystore_password);
}

static void save_cors(cJSON *obj, const ae_config_cors_t *cfg)
{
    cJSON *origins;
    ae_u32 i;

    cJSON_AddBoolToObject(obj, "enabled", cfg->enabled);
    origins = cJSON_AddArrayToObject(obj, "allowedOrigins");
    for (i = 0; i < cfg->allowed_origins_count; ++i) {
        cJSON_AddItemToArray(origins,
                             cJSON_CreateString(cfg->allowed_origins[i]));
    }
}

static void save_http(cJSON *obj, const ae_config_http_t *cfg)
{
    cJSON *encryption;
    cJSON *policies;
    cJSON *cors;
    cJSON *files;

    cJSON_AddBoolToObject(obj, "startImmediately",
                                       cfg->start_immediately);
    cJSON_AddStringToObject(obj, "bindAddress",
                                         cfg->bind_address);
    cJSON_AddNumberToObject(obj, "bindPort",
                                         (double)cfg->bind_port);
    cJSON_AddStringToObject(obj, "accessAddress",
                                         cfg->access_address);
    cJSON_AddNumberToObject(obj, "accessPort",
                                         (double)cfg->access_port);

    encryption = cJSON_AddObjectToObject(obj, "encryption");
    save_encryption(encryption, &cfg->encryption);

    policies = cJSON_AddObjectToObject(obj, "policies");
    cors = cJSON_AddObjectToObject(policies, "cors");
    save_cors(cors, &cfg->policies.cors);

    files = cJSON_AddObjectToObject(obj, "files");
    cJSON_AddStringToObject(files, "indexFile",
                                         cfg->files.index_file);
    cJSON_AddStringToObject(files, "errorFile",
                                         cfg->files.error_file);
}

static void save_game_options(cJSON *obj, const ae_config_game_options_t *cfg)
{
    cJSON *inventory;
    cJSON *avatar;
    cJSON *questing;
    cJSON *resin;
    cJSON *rates;
    cJSON *handbook;
    cJSON *limits;
    cJSON *srv;

    inventory = cJSON_AddObjectToObject(obj,
                                                     "inventoryLimits");
    cJSON_AddNumberToObject(
        inventory, "weapons", (double)cfg->inventory_limits.weapons);
    cJSON_AddNumberToObject(
        inventory, "relics", (double)cfg->inventory_limits.relics);
    cJSON_AddNumberToObject(
        inventory, "materials", (double)cfg->inventory_limits.materials);
    cJSON_AddNumberToObject(
        inventory, "furniture", (double)cfg->inventory_limits.furniture);
    cJSON_AddNumberToObject(
        inventory, "all", (double)cfg->inventory_limits.all);

    avatar = cJSON_AddObjectToObject(obj, "avatarLimits");
    cJSON_AddNumberToObject(
        avatar, "singlePlayerTeam",
        (double)cfg->avatar_limits.single_player_team);
    cJSON_AddNumberToObject(
        avatar, "multiplayerTeam",
        (double)cfg->avatar_limits.multiplayer_team);

    cJSON_AddNumberToObject(
        obj, "sceneEntityLimit", (double)cfg->scene_entity_limit);
    cJSON_AddBoolToObject(obj, "watchGachaConfig",
                                       cfg->watch_gacha_config);
    cJSON_AddBoolToObject(obj, "enableShopItems",
                                       cfg->enable_shop_items);
    cJSON_AddBoolToObject(obj, "staminaUsage",
                                       cfg->stamina_usage);
    cJSON_AddBoolToObject(obj, "energyUsage",
                                       cfg->energy_usage);
    cJSON_AddBoolToObject(obj, "fishhookTeleport",
                                       cfg->fishhook_teleport);
    cJSON_AddBoolToObject(obj, "trialCostumes",
                                       cfg->trial_costumes);
    cJSON_AddBoolToObject(obj, "forceLegacyDrops",
                                       cfg->force_legacy_drops);

    questing = cJSON_AddObjectToObject(obj, "questing");
    cJSON_AddBoolToObject(questing, "enabled",
                                       cfg->questing.enabled);

    resin = cJSON_AddObjectToObject(obj, "resinOptions");
    cJSON_AddBoolToObject(resin, "resinUsage",
                                       cfg->resin_options.resin_usage);
    cJSON_AddNumberToObject(resin, "cap",
                                         (double)cfg->resin_options.cap);
    cJSON_AddNumberToObject(
        resin, "rechargeTime", (double)cfg->resin_options.recharge_time);

    rates = cJSON_AddObjectToObject(obj, "rates");
    cJSON_AddNumberToObject(rates, "adventureExp",
                                         (double)cfg->rates.adventure_exp);
    cJSON_AddNumberToObject(rates, "mora",
                                         (double)cfg->rates.mora);
    cJSON_AddNumberToObject(rates, "leyLines",
                                         (double)cfg->rates.ley_lines);

    handbook = cJSON_AddObjectToObject(obj, "handbook");
    cJSON_AddBoolToObject(handbook, "enable",
                                       cfg->handbook.enable);
    cJSON_AddBoolToObject(handbook, "allowCommands",
                                       cfg->handbook.allow_commands);
    limits = cJSON_AddObjectToObject(handbook, "limits");
    cJSON_AddBoolToObject(limits, "enabled",
                                       cfg->handbook.limits.enabled);
    cJSON_AddNumberToObject(
        limits, "interval", (double)cfg->handbook.limits.interval);
    cJSON_AddNumberToObject(
        limits, "maxRequests",
        (double)cfg->handbook.limits.max_requests);
    cJSON_AddNumberToObject(
        limits, "maxEntities",
        (double)cfg->handbook.limits.max_entities);
    srv = cJSON_AddObjectToObject(handbook, "server");
    cJSON_AddBoolToObject(srv, "enforced",
                                       cfg->handbook.server.enforced);
    cJSON_AddStringToObject(srv, "address",
                                         cfg->handbook.server.address);
    cJSON_AddNumberToObject(srv, "port",
                                         (double)cfg->handbook.server.port);
    cJSON_AddBoolToObject(srv, "canChange",
                                       cfg->handbook.server.can_change);
}

static void save_mail_item(cJSON *elem, const ae_config_mail_item_t *item)
{
    cJSON_AddNumberToObject(elem, "itemId",
                                         (double)item->item_id);
    cJSON_AddNumberToObject(elem, "itemCount",
                                         (double)item->item_count);
    cJSON_AddNumberToObject(elem, "itemLevel",
                                         (double)item->item_level);
}

static void save_join_options(cJSON *obj, const ae_config_join_options_t *cfg)
{
    cJSON *emotes;
    cJSON *mail;
    cJSON *items;
    ae_u32 i;

    emotes = cJSON_AddArrayToObject(obj, "welcomeEmotes");
    for (i = 0; i < cfg->welcome_emotes_count; ++i) {
        cJSON_AddItemToArray(emotes,
                             cJSON_CreateNumber(cfg->welcome_emotes[i]));
    }
    cJSON_AddStringToObject(obj, "welcomeMessage",
                                         cfg->welcome_message);
    mail = cJSON_AddObjectToObject(obj, "welcomeMail");
    cJSON_AddStringToObject(mail, "title",
                                         cfg->welcome_mail.title);
    cJSON_AddStringToObject(mail, "content",
                                         cfg->welcome_mail.content);
    cJSON_AddStringToObject(mail, "sender",
                                         cfg->welcome_mail.sender);
    items = cJSON_AddArrayToObject(mail, "items");
    for (i = 0; i < cfg->welcome_mail.items_count; ++i) {
        cJSON *elem = cJSON_CreateObject();

        save_mail_item(elem, &cfg->welcome_mail.items[i]);
        cJSON_AddItemToArray(items, elem);
    }
}

static void save_console_account(cJSON *obj,
                                 const ae_config_console_account_t *cfg)
{
    cJSON_AddNumberToObject(obj, "avatarId",
                                         (double)cfg->avatar_id);
    cJSON_AddNumberToObject(obj, "nameCardId",
                                         (double)cfg->name_card_id);
    cJSON_AddNumberToObject(obj, "adventureRank",
                                         (double)cfg->adventure_rank);
    cJSON_AddNumberToObject(obj, "worldLevel",
                                         (double)cfg->world_level);
    cJSON_AddStringToObject(obj, "nickName",
                                         cfg->nick_name);
    cJSON_AddStringToObject(obj, "signature",
                                         cfg->signature);
}

static void save_vision(cJSON *obj, const ae_config_vision_options_t *cfg)
{
    cJSON_AddStringToObject(obj, "name", cfg->name);
    cJSON_AddNumberToObject(obj, "visionRange",
                                         (double)cfg->vision_range);
    cJSON_AddNumberToObject(obj, "gridWidth",
                                         (double)cfg->grid_width);
}

static void save_game(cJSON *obj, const ae_config_game_t *cfg)
{
    cJSON *opts;
    cJSON *join;
    cJSON *account;
    cJSON *vision;
    ae_u32 i;

    cJSON_AddStringToObject(obj, "bindAddress",
                                         cfg->bind_address);
    cJSON_AddNumberToObject(obj, "bindPort",
                                         (double)cfg->bind_port);
    cJSON_AddStringToObject(obj, "accessAddress",
                                         cfg->access_address);
    cJSON_AddNumberToObject(obj, "accessPort",
                                         (double)cfg->access_port);
    cJSON_AddBoolToObject(obj, "useUniquePacketKey",
                                       cfg->use_unique_packet_key);
    cJSON_AddNumberToObject(
        obj, "loadEntitiesForPlayerRange",
        (double)cfg->load_entities_for_player_range);
    cJSON_AddBoolToObject(obj, "enableScriptInBigWorld",
                                       cfg->enable_script_in_big_world);
    cJSON_AddBoolToObject(obj, "enableConsole",
                                       cfg->enable_console);
    cJSON_AddNumberToObject(obj, "kcpInterval",
                                         (double)cfg->kcp_interval);
    cJSON_AddStringToObject(
        obj, "logPackets",
        s_debug_names[(ae_s32)cfg->log_packets]);
    cJSON_AddBoolToObject(obj, "isShowPacketPayload",
                                       cfg->is_show_packet_payload);
    cJSON_AddBoolToObject(obj, "isShowLoopPackets",
                                       cfg->is_show_loop_packets);
    cJSON_AddBoolToObject(obj, "cacheSceneEntitiesEveryRun",
                                       cfg->cache_scene_entities_every_run);

    opts = cJSON_AddObjectToObject(obj, "gameOptions");
    save_game_options(opts, &cfg->game_options);
    join = cJSON_AddObjectToObject(obj, "joinOptions");
    save_join_options(join, &cfg->join_options);
    account = cJSON_AddObjectToObject(obj, "serverAccount");
    save_console_account(account, &cfg->console_account);
    vision = cJSON_AddArrayToObject(obj, "visionOptions");
    for (i = 0; i < cfg->vision_options_count; ++i) {
        cJSON *elem = cJSON_CreateObject();

        save_vision(elem, &cfg->vision_options[i]);
        cJSON_AddItemToArray(vision, elem);
    }
}

static void save_dispatch(cJSON *obj, const ae_config_dispatch_t *cfg)
{
    cJSON *regions;
    ae_u32 i;

    regions = cJSON_AddArrayToObject(obj, "regions");
    for (i = 0; i < cfg->regions_count; ++i) {
        cJSON *elem = cJSON_CreateObject();
        const ae_config_region_t *region = &cfg->regions[i];

        cJSON_AddStringToObject(elem, "Name",
                                             region->name);
        cJSON_AddStringToObject(elem, "Title",
                                             region->title);
        cJSON_AddStringToObject(elem, "Ip", region->ip);
        cJSON_AddNumberToObject(elem, "Port",
                                             (double)region->port);
        cJSON_AddItemToArray(regions, elem);
    }
    cJSON_AddStringToObject(obj, "dispatchUrl",
                                         cfg->dispatch_url);
    cJSON_AddStringToObject(obj, "encryptionKey",
                                         cfg->encryption_key);
    cJSON_AddStringToObject(obj, "dispatchKey",
                                         cfg->dispatch_key);
    cJSON_AddStringToObject(obj, "defaultName",
                                         cfg->default_name);
    cJSON_AddStringToObject(
        obj, "logRequests", s_debug_names[(ae_s32)cfg->log_requests]);
}

static void save_debug_mode(cJSON *obj, const ae_config_debug_options_t *cfg)
{
    cJSON_AddStringToObject(
        obj, "serverLoggerLevel",
        s_log_names[(ae_s32)cfg->server_logger_level]);
    cJSON_AddStringToObject(
        obj, "servicesLoggersLevel",
        s_log_names[(ae_s32)cfg->services_logger_level]);
    cJSON_AddStringToObject(
        obj, "logPackets", s_debug_names[(ae_s32)cfg->log_packets]);
    cJSON_AddBoolToObject(obj, "isShowPacketPayload",
                                       cfg->is_show_packet_payload);
    cJSON_AddBoolToObject(obj, "isShowLoopPackets",
                                       cfg->is_show_loop_packets);
    cJSON_AddStringToObject(
        obj, "logRequests", s_debug_names[(ae_s32)cfg->log_requests]);
}

static void save_server(cJSON *obj, const ae_config_server_t *cfg)
{
    cJSON *whitelist;
    cJSON *blacklist;
    cJSON *http;
    cJSON *game;
    cJSON *dispatch;
    cJSON *debug;
    ae_u32 i;

    whitelist = cJSON_AddArrayToObject(obj,
                                                    "debugWhitelist");
    for (i = 0; i < cfg->debug_whitelist_count; ++i) {
        cJSON_AddItemToArray(whitelist,
                             cJSON_CreateNumber(cfg->debug_whitelist[i]));
    }
    blacklist = cJSON_AddArrayToObject(obj,
                                                    "debugBlacklist");
    for (i = 0; i < cfg->debug_blacklist_count; ++i) {
        cJSON_AddItemToArray(blacklist,
                             cJSON_CreateNumber(cfg->debug_blacklist[i]));
    }
    cJSON_AddStringToObject(
        obj, "runMode", s_run_names[(ae_s32)cfg->run_mode]);
    cJSON_AddBoolToObject(obj, "logCommands",
                                       cfg->log_commands);
    cJSON_AddBoolToObject(obj, "fastRequire",
                                       cfg->fast_require);

    http = cJSON_AddObjectToObject(obj, "http");
    save_http(http, &cfg->http);
    game = cJSON_AddObjectToObject(obj, "game");
    save_game(game, &cfg->game);
    dispatch = cJSON_AddObjectToObject(obj, "dispatch");
    save_dispatch(dispatch, &cfg->dispatch);
    debug = cJSON_AddObjectToObject(obj, "debugMode");
    save_debug_mode(debug, &cfg->debug_mode);
}

/* ============================================================ */

ae_error_t ae_config_save_string(const ae_config_t *config, char **out,
                                 ae_bool pretty)
{
    cJSON *root;
    char *text;

    if (!config || !out) {
        return AE_ERR_INVALID_ARG;
    }
    root = cJSON_CreateObject();
    if (!root) {
        return AE_ERR_OUT_OF_MEMORY;
    }

    {
        cJSON *section;

        section = cJSON_AddObjectToObject(root, "folderStructure");
        save_structure(section, &config->folder_structure);
        section = cJSON_AddObjectToObject(root, "databaseInfo");
        save_database(section, &config->database_info);
        section = cJSON_AddObjectToObject(root, "language");
        save_language(section, &config->language);
        section = cJSON_AddObjectToObject(root, "account");
        save_account(section, &config->account);
        section = cJSON_AddObjectToObject(root, "server");
        save_server(section, &config->server);
    }
    cJSON_AddNumberToObject(root, "version",
                            (double)config->version);

    text = pretty ? cJSON_Print(root) : cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (!text) {
        return AE_ERR_OUT_OF_MEMORY;
    }
    *out = text;
    return AE_OK;
}

/* ============================================================ */

ae_error_t ae_config_load_file(ae_config_t *config, const char *path)
{
    FILE *fp;
    long len;
    size_t got;
    char *text;
    ae_error_t err;

    if (!config || !path) {
        return AE_ERR_INVALID_ARG;
    }
    fp = fopen(path, "rb");
    if (!fp) {
        return AE_ERR_IO;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return AE_ERR_IO;
    }
    len = ftell(fp);
    if (len < 0) {
        fclose(fp);
        return AE_ERR_IO;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return AE_ERR_IO;
    }
    if (len > 1024 * 1024 * 32) {
        fclose(fp);
        return AE_ERR_OVERFLOW;
    }

    text = (char *)malloc((size_t)len + 1u);
    if (!text) {
        fclose(fp);
        return AE_ERR_OUT_OF_MEMORY;
    }
    got = fread(text, 1u, (size_t)len, fp);
    fclose(fp);
    if (got != (size_t)len) {
        free(text);
        return AE_ERR_IO;
    }
    text[got] = '\0';

    err = ae_config_load_string(config, text);
    free(text);
    return err;
}

ae_error_t ae_config_save_file(const ae_config_t *config,
                               const char *path)
{
    char *text;
    FILE *fp;
    ae_error_t err;
    size_t len;

    if (!config || !path) {
        return AE_ERR_INVALID_ARG;
    }
    err = ae_config_save_string(config, &text, true);
    if (err != AE_OK) {
        return err;
    }
    fp = fopen(path, "wb");
    if (!fp) {
        free(text);
        return AE_ERR_IO;
    }
    len = strlen(text);
    if (fwrite(text, 1u, len, fp) != len) {
        fclose(fp);
        free(text);
        return AE_ERR_IO;
    }
    fclose(fp);
    free(text);
    return AE_OK;
}

/* ============================================================ */

ae_bool ae_config_update(ae_config_t *config)
{
    if (!config) {
        return false;
    }
    if (config->version == AE_CONFIG_VERSION) {
        return false;
    }
    config->version = AE_CONFIG_VERSION;
    return true;
}