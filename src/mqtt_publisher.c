#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mosquitto.h"
#include "config_parser.h"

#define MAX_PAYLOAD 8192

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <json_file>\n", argv[0]);
        return 1;
    }

    const char *json_file = argv[1];
    FILE *fp = fopen(json_file, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open JSON file: %s\n", json_file);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    if (size >= MAX_PAYLOAD) {
        fprintf(stderr, "Payload too large\n");
        fclose(fp);
        return 1;
    }

    char payload[MAX_PAYLOAD];
    size_t read = fread(payload, 1, size, fp);
    payload[read] = '\0';
    fclose(fp);

    struct app_config cfg;
    if (load_config(&cfg) != 0) {
        fprintf(stderr, "Failed to load config\n");
        return 1;
    }

    if (strlen(cfg.mqtt_host) == 0 || strlen(cfg.mqtt_topic) == 0) {
        fprintf(stderr, "MQTT host or topic not configured.\n");
        return 1;
    }

    fprintf(stderr, "Using config:\n  Host: %s\n  Port: %d\n  Topic: %s\n  User: %s\n",
            cfg.mqtt_host, cfg.mqtt_port, cfg.mqtt_topic,
            strlen(cfg.mqtt_user) > 0 ? cfg.mqtt_user : "(none)");

    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Failed to create MQTT client\n");
        mosquitto_lib_cleanup();
        return 1;
    }

    if (strlen(cfg.mqtt_user) > 0) {
        mosquitto_username_pw_set(mosq, cfg.mqtt_user, cfg.mqtt_pass);
    }

    // Add TLS if using secure port (commonly 8883 or 8884)
    if (cfg.mqtt_port == 8883 || cfg.mqtt_port == 8884) {
        mosquitto_tls_set(mosq, NULL, NULL, NULL, NULL, NULL);
        mosquitto_tls_opts_set(mosq, 1, "tlsv1.2", NULL);
    }

    if (mosquitto_connect(mosq, cfg.mqtt_host, cfg.mqtt_port, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to connect to MQTT broker at %s:%d\n",
                cfg.mqtt_host, cfg.mqtt_port);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    int rc = mosquitto_publish(mosq, NULL, cfg.mqtt_topic, strlen(payload), payload, 0, false);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to publish message: %s\n", mosquitto_strerror(rc));
    } else {
        fprintf(stdout, "MQTT publish successful to topic '%s'\n", cfg.mqtt_topic);
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
