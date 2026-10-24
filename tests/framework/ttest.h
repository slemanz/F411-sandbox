#ifndef TTEST_H_
#define TTEST_H_

#include <stdint.h>
#include <stdbool.h>

#define TTEST_MAGIC         0x7E57C0DEU

typedef enum
{
    TTEST_STATE_RUNNING = 1,
    TTEST_STATE_DONE
}ttest_state_e;

typedef struct
{
    uint32_t    magic;
    uint32_t    state;
    uint32_t    total;
    uint32_t    failed;
    const char *fail_test;
    const char *fail_file;
    uint32_t    fail_line;
    uint32_t    fail_expected;
    uint32_t    fail_actual;
}ttest_results_t;

void ttest_begin(void);
void ttest_end(void);
void ttest_run(void (*test)(void), const char *name);
bool ttest_check(uint32_t expected, uint32_t actual, const char *file, uint32_t line);

#define RUN_TEST(test)  ttest_run(test, #test)

#define TEST_ASSERT_EQ(expected, actual) \
        do { if(!ttest_check((expected), (actual), __FILE__, __LINE__)) return; }while(0)

#endif /* TTEST_H_ */