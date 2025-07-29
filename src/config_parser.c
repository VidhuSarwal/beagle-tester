#include <string.h>
#include <stdlib.h>
#include "config_parser.h"
#include "ini.h"

#define CONFIG_PATH "/tmp/beagle_tester.conf"

static int handler(void* user, const char* section, const char* name, const char* value) {
    struct app_config* cfg = (struct app_config*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("services", "enable_web")) {
        cfg->enable_web = strcmp(value, "true") == 0 || strcmp(value, "1") == 0;
    } else if (MATCH("services", "enable_mqtt")) {
        cfg->enable_mqtt = strcmp(value, "true") == 0 || strcmp(value, "1") == 0;
    } else if (MATCH("broker", "host")) {
        strncpy(cfg->mqtt_host, value, sizeof(cfg->mqtt_host));
    } else if (MATCH("broker", "port")) {
        cfg->mqtt_port = atoi(value);
    } else if (MATCH("broker", "topic")) {
        strncpy(cfg->mqtt_topic, value, sizeof(cfg->mqtt_topic));
    } else if (MATCH("auth", "username")) {
        strncpy(cfg->mqtt_user, value, sizeof(cfg->mqtt_user));
    } else if (MATCH("auth", "password")) {
        strncpy(cfg->mqtt_pass, value, sizeof(cfg->mqtt_pass));
    }
    return 1;
}

int load_config(struct app_config* cfg) {
    memset(cfg, 0, sizeof(*cfg));
    strncpy(cfg->mqtt_host, "localhost", sizeof(cfg->mqtt_host));
    cfg->mqtt_port = 1883;
    strncpy(cfg->mqtt_topic, "beagle/test/results", sizeof(cfg->mqtt_topic));
    return ini_parse(CONFIG_PATH, handler, cfg);
}

int load_config_path(struct app_config *cfg, const char *path) {
    return ini_parse(path, handler, cfg);
}
