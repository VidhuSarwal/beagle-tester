#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <time.h>

#define MAX_RESULTS 256

struct test_result {
    char test[64];
    char status[64];
    time_t timestamp;
};

// starting function
void start_web_server(struct test_result *results, int *count);

void stop_web_server(void);

#endif
