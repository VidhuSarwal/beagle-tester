#ifndef TEST_RESULT_H
#define TEST_RESULT_H

#include <time.h>

#define MAX_RESULTS 256

struct test_result {
    char test[64];
    char status[64];
    time_t timestamp;
};

#endif
