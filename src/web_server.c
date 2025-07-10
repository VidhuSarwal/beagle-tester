#include "web_server.h"
#include "mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct server_context {
    struct test_result *results;
    int *count;
};

static struct mg_mgr mgr;
static int running = 1;

static void handle_request(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        struct server_context *ctx = (struct server_context *) c->fn_data;

        // Debug: print incoming URI
        printf("[DEBUG] URI: %.*s\n", (int)hm->uri.len, hm->uri.buf);
        fflush(stdout);

        struct mg_http_serve_opts opts = {
            .mime_types = "css=text/css,js=application/javascript,html=text/html",
            .extra_headers = "Cache-Control: no-cache\r\n"
        };

        if (mg_match(hm->uri, mg_str("/results.json"), NULL)) {
            printf("[DEBUG] Matched /results.json\n");

            // Assemble JSON in a local buffer
            char buffer[4096] = {0};
            char *p = buffer;
            p += sprintf(p, "[");

            for (int i = 0; i < *(ctx->count); ++i) {
                p += sprintf(p, "{\"test\":\"%s\",\"status\":\"%s\"}%s",
                             ctx->results[i].test,
                             ctx->results[i].status,
                             i < (*(ctx->count) - 1) ? "," : "");
            }

            p += sprintf(p, "]");

            // Send HTTP 200 response with Content-Type
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", buffer);

        } else if (mg_match(hm->uri, mg_str("/"), NULL)) {
            mg_http_serve_file(c, hm, "web/index.html", &opts);
        } else if (mg_match(hm->uri, mg_str("/style.css"), NULL)) {
            mg_http_serve_file(c, hm, "web/style.css", &opts);
        } else if (mg_match(hm->uri, mg_str("/app.js"), NULL)) {
            mg_http_serve_file(c, hm, "web/app.js", &opts);
        } else {
            mg_http_reply(c, 404, "", "Not found");
        }
    }
}


static void *web_server_loop(void *arg) {
    while (running) mg_mgr_poll(&mgr, 1000);
    mg_mgr_free(&mgr);
    return NULL;
}

void start_web_server(struct test_result *results, int *count) {
    static struct server_context ctx;  // Static to persist across threads
    ctx.results = results;
    ctx.count = count;

    mg_mgr_init(&mgr);
    struct mg_connection *c = mg_http_listen(&mgr, "http://0.0.0.0:8000", handle_request, &ctx);
    if (c == NULL) {
        fprintf(stderr, "Error starting web server\n");
        return;
    }
    c->fn_data = &ctx;

    pthread_t tid;
    pthread_create(&tid, NULL, web_server_loop, NULL);
}

void stop_web_server() {
    running = 0;
}
