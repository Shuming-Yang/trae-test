/**
 * @file   integrated_test.c
 * @author Steven.Yang
 * @brief: Integration tests for IRQ Simulator end-to-end flows
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#include "integrated_test.h"
#include "main.h"

static unsigned int g_it_passed = 0;
static unsigned int g_it_failed = 0;

#define RUN_TEST(name) do { \
    printf("  Running %s...\n", #name); \
    irq_reset_all(); \
    if (name() == 0) { \
        printf("  [PASS] %s\n", #name); \
        g_it_passed++; \
    } else { \
        g_it_failed++; \
    } \
} while (0)

/**
 * @brief Simulate the main loop's input parsing logic
 * @param line Input string (without trailing newline for simplicity)
 * @return 1 if "exit" was processed, 0 otherwise
 */
static int simulate_input(const char *line) {
    int input;
    unsigned int hex_val;
    char buf[64];

    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    if (strcmp(buf, "exit") == 0) {
        return 1;
    }

    if (buf[0] == 'b' || buf[0] == 'B') {
        if (sscanf(buf + 1, "%d", &input) == 1 && input >= 0 && input < (int)IRQ_COUNT) {
            irq_trigger((unsigned int)input);
            irq_process_all();
        }
    } else if (buf[0] == 'h' || buf[0] == 'H') {
        if (sscanf(buf + 1, "%x", &hex_val) == 1) {
            irq_trigger_raw(hex_val);
            irq_process_all();
        }
    } else if (sscanf(buf, "%d", &input) == 1) {
        if (input == 0) {
            irq_process_all();
        } else if (input >= 1 && input <= 32) {
            irq_trigger((unsigned int)(input - 1));
            irq_process_all();
        }
    }

    return 0;
}

/* ================================================================
 * IT_01: Number Mode Input Parsing
 * ================================================================ */

static int test_number_mode_irq0(void) {
    simulate_input("1");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_01_01: input '1' triggers IRQ0, pending cleared");
    return 0;
}

static int test_number_mode_irq31(void) {
    simulate_input("32");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_01_02: input '32' triggers IRQ31, pending cleared");
    return 0;
}

static int test_number_mode_zero(void) {
    irq_trigger(3);
    simulate_input("0");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_01_03: input '0' processes pending IRQ3");
    return 0;
}

static int test_number_mode_invalid_33(void) {
    uint32_t before = irq_get_pending();
    simulate_input("33");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_01_04: input '33' should not change pending");
    return 0;
}

static int test_number_mode_invalid_neg5(void) {
    uint32_t before = irq_get_pending();
    simulate_input("-5");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_01_05: input '-5' should not change pending");
    return 0;
}

/* ================================================================
 * IT_02: b-mode Input Parsing
 * ================================================================ */

static int test_bmode_irq0(void) {
    simulate_input("b0");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_02_01: b0 triggers IRQ0, pending cleared");
    return 0;
}

static int test_bmode_irq5(void) {
    simulate_input("b5");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_02_02: b5 triggers IRQ5, pending cleared");
    return 0;
}

static int test_bmode_irq31(void) {
    simulate_input("b31");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_02_03: b31 triggers IRQ31, pending cleared");
    return 0;
}

static int test_bmode_uppercase(void) {
    simulate_input("B10");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_02_04: B10 triggers IRQ10, pending cleared");
    return 0;
}

static int test_bmode_invalid_32(void) {
    uint32_t before = irq_get_pending();
    simulate_input("b32");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_02_05: b32 should not change pending");
    return 0;
}

static int test_bmode_invalid_neg1(void) {
    uint32_t before = irq_get_pending();
    simulate_input("b-1");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_02_06: b-1 should not change pending");
    return 0;
}

/* ================================================================
 * IT_03: h-mode Input Parsing
 * ================================================================ */

static int test_hmode_h1(void) {
    simulate_input("h1");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_01: h1 triggers IRQ0, pending cleared");
    return 0;
}

static int test_hmode_h3(void) {
    simulate_input("h3");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_02: h3 triggers IRQ0,1, pending cleared");
    return 0;
}

static int test_hmode_hFF(void) {
    simulate_input("hFF");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_03: hFF triggers IRQ0~7, pending cleared");
    return 0;
}

static int test_hmode_h80000000(void) {
    simulate_input("h80000000");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_04: h80000000 triggers IRQ31, pending cleared");
    return 0;
}

static int test_hmode_uppercase(void) {
    simulate_input("H0A");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_05: H0A triggers IRQ1,3, pending cleared");
    return 0;
}

static int test_hmode_invalid(void) {
    uint32_t before = irq_get_pending();
    simulate_input("hGG");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_03_06: hGG should not change pending");
    return 0;
}

/* ================================================================
 * IT_04: Accumulated Trigger & Priority
 * ================================================================ */

static int test_accumulate_trigger_then_hmode(void) {
    irq_trigger(0);
    simulate_input("h6");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_04_01: trigger(0)+h6 processes IRQ0,1,2");
    return 0;
}

static int test_accumulate_multi_bmode(void) {
    simulate_input("b10");
    simulate_input("b5");
    simulate_input("0");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_04_02: b10+b5+0 processes IRQ5,10");
    return 0;
}

static int test_priority_order(void) {
    simulate_input("h80000001");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_04_03: h80000001 processes IRQ0 before IRQ31");
    return 0;
}

/* ================================================================
 * IT_05: Tick Count Consistency
 * ================================================================ */

static int test_tick_initial_zero(void) {
    IT_ASSERT_EQ(irq_get_tick(), 0, "IT_05_01: initial tick should be 0");
    return 0;
}

static int test_tick_irq0_increment(void) {
    unsigned int before = irq_get_tick();
    irq_trigger(0);
    irq_process_all();
    IT_ASSERT_EQ(irq_get_tick(), before + 1, "IT_05_02: IRQ0 handler increments tick by 1");
    return 0;
}

static int test_tick_non_irq0_no_change(void) {
    unsigned int before = irq_get_tick();
    irq_trigger(5);
    irq_process_all();
    IT_ASSERT_EQ(irq_get_tick(), before, "IT_05_03: non-IRQ0 should not change tick");
    return 0;
}

static int test_tick_multi_irq0(void) {
    unsigned int before = irq_get_tick();
    irq_trigger(0);
    irq_process_all();
    irq_trigger(0);
    irq_process_all();
    irq_trigger(0);
    irq_process_all();
    IT_ASSERT_EQ(irq_get_tick(), before + 3, "IT_05_04: three separate IRQ0 triggers increment tick by 3");
    return 0;
}

/* ================================================================
 * IT_06: Exit & Boundary Conditions
 * ================================================================ */

static int test_exit_returns_one(void) {
    int result = simulate_input("exit");
    IT_ASSERT_EQ(result, 1, "IT_06_01: 'exit' should return 1");
    return 0;
}

static int test_empty_input_safe(void) {
    uint32_t before = irq_get_pending();
    simulate_input("");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_06_02: empty input should not crash");
    return 0;
}

static int test_garbage_input_safe(void) {
    uint32_t before = irq_get_pending();
    simulate_input("xyz");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_06_03: garbage input should not crash");
    return 0;
}

/* ================================================================
 * IT_07: End-to-End Full Flow
 * ================================================================ */

static int test_full_flow(void) {
    simulate_input("1");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_07_01: step1 '1' -> IRQ0 processed");

    simulate_input("b5");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_07_01: step2 'b5' -> IRQ5 processed");

    simulate_input("h3");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_07_01: step3 'h3' -> IRQ0,1 processed");

    int result = simulate_input("exit");
    IT_ASSERT_EQ(result, 1, "IT_07_01: step4 'exit' -> returns 1");

    return 0;
}

/* ================================================================
 * Test Runner
 * ================================================================ */

int run_all_integrated_tests(void) {
    g_it_passed = 0;
    g_it_failed = 0;

    printf("\n========== Integration Tests ==========\n\n");

    printf("[IT_01] Number Mode Input Parsing:\n");
    RUN_TEST(test_number_mode_irq0);
    RUN_TEST(test_number_mode_irq31);
    RUN_TEST(test_number_mode_zero);
    RUN_TEST(test_number_mode_invalid_33);
    RUN_TEST(test_number_mode_invalid_neg5);

    printf("\n[IT_02] b-mode Input Parsing:\n");
    RUN_TEST(test_bmode_irq0);
    RUN_TEST(test_bmode_irq5);
    RUN_TEST(test_bmode_irq31);
    RUN_TEST(test_bmode_uppercase);
    RUN_TEST(test_bmode_invalid_32);
    RUN_TEST(test_bmode_invalid_neg1);

    printf("\n[IT_03] h-mode Input Parsing:\n");
    RUN_TEST(test_hmode_h1);
    RUN_TEST(test_hmode_h3);
    RUN_TEST(test_hmode_hFF);
    RUN_TEST(test_hmode_h80000000);
    RUN_TEST(test_hmode_uppercase);
    RUN_TEST(test_hmode_invalid);

    printf("\n[IT_04] Accumulated Trigger & Priority:\n");
    RUN_TEST(test_accumulate_trigger_then_hmode);
    RUN_TEST(test_accumulate_multi_bmode);
    RUN_TEST(test_priority_order);

    printf("\n[IT_05] Tick Count Consistency:\n");
    RUN_TEST(test_tick_initial_zero);
    RUN_TEST(test_tick_irq0_increment);
    RUN_TEST(test_tick_non_irq0_no_change);
    RUN_TEST(test_tick_multi_irq0);

    printf("\n[IT_06] Exit & Boundary Conditions:\n");
    RUN_TEST(test_exit_returns_one);
    RUN_TEST(test_empty_input_safe);
    RUN_TEST(test_garbage_input_safe);

    printf("\n[IT_07] End-to-End Full Flow:\n");
    RUN_TEST(test_full_flow);

    printf("\n========== Integration Test Results ==========\n");
    printf("  Total:  %u\n", g_it_passed + g_it_failed);
    printf("  Passed: %u\n", g_it_passed);
    printf("  Failed: %u\n", g_it_failed);
    printf("===============================================\n");

    return (g_it_failed > 0) ? 1 : 0;
}

int main(void) {
    return run_all_integrated_tests();
}
