#pragma once

typedef int (*click_test_func_t)(void);

typedef struct {
    const char *click_id;
    click_test_func_t run_test;
} click_test_entry_t;

extern click_test_entry_t click_test_table[];
extern int click_test_table_size;
