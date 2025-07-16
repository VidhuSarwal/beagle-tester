

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

struct app_config {
    int enable_web;
    int enable_mqtt;
    char mqtt_host[64];
    int mqtt_port;
    char mqtt_topic[64];
    char mqtt_user[64];
    char mqtt_pass[64];
};

int load_config(struct app_config *cfg);

#endif
