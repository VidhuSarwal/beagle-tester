#include "mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct mg_mgr mgr;
static char *cached_json = NULL;
static int running = 1;

char *load_json(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *data = malloc(size + 1);
    if (!data) {
        perror("malloc");
        fclose(fp);
        return NULL;
    }

    fread(data, 1, size, fp);
    data[size] = '\0';
    fclose(fp);
    return data;
}

static void handle_request(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        printf("[DEBUG] Request URI: %.*s\n", (int)hm->uri.len, hm->uri.buf);
        fflush(stdout);

        struct mg_http_serve_opts opts = {
            .root_dir = "web",
            .mime_types = "css=text/css,js=application/javascript,html=text/html",
            .extra_headers = "Cache-Control: no-cache\r\n"
        };

        if (mg_match(hm->uri, mg_str("/results.json"), NULL)) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", cached_json);
        } else if (mg_match(hm->uri, mg_str("/"), NULL)) {
            mg_http_serve_file(c, hm, "web/index.html", &opts);
        } else if (mg_match(hm->uri, mg_str("/style.css"), NULL)) {
            mg_http_serve_file(c, hm, "web/style.css", &opts);
        } else if (mg_match(hm->uri, mg_str("/app.js"), NULL)) {
            mg_http_serve_file(c, hm, "web/app.js", &opts);
        } else {
            mg_http_reply(c, 404, "", "Not found\n");
        }
    }
}

int main(int argc, char **argv) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("[DEBUG] Web server running in: %s\n", cwd);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <results.json>\n", argv[0]);
        return 1;
    }

    cached_json = load_json(argv[1]);
    if (!cached_json) {
        fprintf(stderr, "Failed to load test results from %s\n", argv[1]);
        return 1;
    }

    mg_mgr_init(&mgr);
    struct mg_connection *c = mg_http_listen(&mgr, "http://0.0.0.0:8000", handle_request, NULL);
    if (c == NULL) {
        fprintf(stderr, "Failed to start web server on port 8000\n");
        free(cached_json);
        return 1;
    }

    printf("Web server running at http://0.0.0.0:8000\n");

    while (running) {
        mg_mgr_poll(&mgr, 1000);
    }

    mg_mgr_free(&mgr);
    free(cached_json);
    return 0;
}
