#include "click_dispatch.h"
#include "rtc5click_test.h"

//add tests with respective function mapping here.
click_test_entry_t click_test_table[] = {
    {"MKBRCCLICK", run_rtc5click_test},
};


int click_test_table_size = sizeof(click_test_table) / sizeof(click_test_table[0]);
