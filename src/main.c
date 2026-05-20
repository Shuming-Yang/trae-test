/**
 * @file   main.c
 * @author Steven.Yang
 * @brief: Main entry point for test-trae project
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#include "main.h"
#include <string.h>
/*
 * MISRA deviation: <stdarg.h> (Rule 17.1) and <stdio.h> (Rule 21.6)
 * are intentionally deviated for the tick_printf() debug output facility,
 * fflush, fgets, and sscanf used in the CLI loop.
 */
#include <stdarg.h>
// cppcheck-suppress-begin misra-c2012-21.6
#include <stdio.h>
// cppcheck-suppress-end misra-c2012-21.6

/* CLI input buffer size */
#define CLI_LINE_BUFFER_SIZE 64U

/* Global function pointers for exception handlers (referenced in start.s) */
void (*rom_except_tick)(void) = tick_irq_handler;
void (*rom_except_int)(void) = exception_irq_handler;

FW_STATIC uint32_t irq_pending = 0U;
FW_STATIC uint32_t g_tick_count = 0U;
FW_STATIC uint32_t exception_count = 0U;

/**
 * @brief Debug logging wrapper: prefix output with tick timestamp, then delegate to printf / vprintf
 * @remark R17.1 deviation: va_list / va_start / va_end are required for variadic debug output.
 *         This function serves CLI interactive development only and is excluded from production firmware.
 * @param [in] fmt type=[const char*] R=[N/A] P=[N/A] N=[N/A] D=[printf-compatible format string]
 * @param [in] ... type=[variable] R=[N/A] P=[N/A] N=[N/A] D=[variable arguments matching fmt]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
FW_STATIC void tick_printf(const char *fmt, ...) {
    // cppcheck-suppress-begin misra-c2012-17.1
    va_list args;
    (void)printf("[tick: %05u] ", g_tick_count);
    va_start(args, fmt);
    (void)vprintf(fmt, args);
    va_end(args);
    // cppcheck-suppress-end misra-c2012-17.1
}

/**
 * @brief Disable global interrupts (platform stub)
 * @remark Replace with arch-specific intrinsic (e.g. __disable_irq() on Cortex-M)
 *         on real hardware targets.
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void __disable_irq(void) {
    /* no-op stub — replace with CPSID / CLR FAULTMASK on real hardware */
}

/**
 * @brief Enable global interrupts (platform stub)
 * @remark Replace with arch-specific intrinsic (e.g. __enable_irq() on Cortex-M)
 *         on real hardware targets.
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void __enable_irq(void) {
    /* no-op stub — replace with CPSIE / SET FAULTMASK on real hardware */
}

/**
 * @brief Tick interrupt handler function
 * @remark Test Criteria: Function executed on tick interrupt.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void tick_irq_handler(void) {
    __disable_irq();
    g_tick_count++;
    __enable_irq();
}

/**
 * @brief Exception interrupt handler function
 * @remark Test Criteria: Function executed on external interrupt.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void exception_irq_handler(void) {
    __disable_irq();
    exception_count++;
    __enable_irq();
}

/**
 * @brief Trigger a specific IRQ by setting the corresponding bit in irq_pending
 * @remark Test Criteria: IRQ bit set correctly in pending register.
 * @param [in] irq_num type=[unsigned int] R=[0-31] P=[0-31] N=[N/A] D=[IRQ number to trigger]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_trigger(uint32_t irq_num) {
    if (irq_num < IRQ_COUNT) {
        irq_pending |= ((uint32_t)1U << irq_num);
        tick_printf("[IRQ] IRQ%u triggered (pending: 0x%08X)\n", irq_num, irq_pending);
    } else {
        tick_printf("[IRQ] Invalid IRQ number: %u (valid range: 0-%u)\n", irq_num, IRQ_COUNT - 1U);
    }
}

/**
 * @brief Trigger multiple IRQs by raw mask (for h-mode / test use)
 * @param [in] mask type=[uint32_t] R=[0-0xFFFFFFFF] P=[N/A] N=[N/A] D=[bitmask of IRQs to trigger]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
FW_STATIC void irq_trigger_raw(uint32_t mask) {
    irq_pending |= mask;
    tick_printf("[IRQ] Raw trigger: pending = 0x%08X\n", irq_pending);
}

/**
 * @brief IRQ human-readable name lookup table, indexed by IRQ number [0..IRQ_COUNT-1]
 */
static const char *const irq_names[IRQ_COUNT] = {
    [0]  = "System Timer IRQ - tick count updated",
    [1]  = "UART0 RX IRQ - data received",
    [2]  = "UART0 TX IRQ - data transmitted",
    [3]  = "GPIO Port A IRQ - pin state changed",
    [4]  = "GPIO Port B IRQ - pin state changed",
    [5]  = "SPI0 IRQ - transfer complete",
    [6]  = "I2C0 IRQ - transaction done",
    [7]  = "ADC IRQ - conversion complete",
    [8]  = "DMA Channel 0 IRQ - transfer finished",
    [9]  = "DMA Channel 1 IRQ - transfer finished",
    [10] = "Watchdog IRQ - timer expired",
    [11] = "RTC IRQ - alarm triggered",
    [12] = "USB IRQ - device event",
    [13] = "CAN0 IRQ - message received",
    [14] = "PWM IRQ - period elapsed",
    [15] = "Timer1 IRQ - compare match",
    [16] = "Timer2 IRQ - overflow",
    [17] = "UART1 RX IRQ - data received",
    [18] = "UART1 TX IRQ - data transmitted",
    [19] = "SPI1 IRQ - transfer complete",
    [20] = "I2C1 IRQ - transaction done",
    [21] = "External INT0 - rising edge detected",
    [22] = "External INT1 - falling edge detected",
    [23] = "External INT2 - level triggered",
    [24] = "DMA Channel 2 IRQ - transfer finished",
    [25] = "DMA Channel 3 IRQ - transfer finished",
    [26] = "CRC IRQ - calculation done",
    [27] = "AES IRQ - encryption complete",
    [28] = "QSPI IRQ - command finished",
    [29] = "SDIO IRQ - card event",
    [30] = "Ethernet IRQ - packet received",
    [31] = "Exception IRQ - external interrupt",
};

/**
 * @brief IRQ handler: process a specific IRQ and clear its pending bit
 * @remark Test Criteria: IRQ handled and pending bit cleared.
 * @param [in] irq_num type=[uint32_t] R=[0-31] P=[0-31] N=[N/A] D=[IRQ number to handle]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
FW_STATIC void irq_handler(uint32_t irq_num) {
    if (irq_num >= IRQ_COUNT) {
        tick_printf("  -> Unknown IRQ%u (out of range)\n", irq_num);
        return;
    }

    tick_printf("  -> Handling IRQ%u: %s\n", irq_num, irq_names[irq_num]);

    if (irq_num == 0U) {
        tick_irq_handler();
    }
    if (irq_num == 31U) {
        exception_irq_handler();
    }

    irq_pending &= ~((uint32_t)1U << irq_num);
}

/**
 * @brief Process all pending IRQs from lowest to highest priority (IRQ0 = highest)
 * @remark Test Criteria: All pending IRQs processed in priority order.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_process_all(void) {
    if (irq_pending != 0U) {
        tick_printf("\n=== Processing pending IRQs (0x%08X) ===\n", irq_pending);

        for (uint32_t i = 0U; i < IRQ_COUNT; i++) {
            if ((irq_pending & ((uint32_t)1U << i)) != 0U) {
                irq_handler(i);
            }
        }

        tick_printf("=== All IRQs processed ===\n\n");
    }
}

/**
 * @brief Get current IRQ pending register value (for test access)
 * @retval type=[uint32_t] current irq_pending value
 */
FW_STATIC uint32_t irq_get_pending(void) {
    return irq_pending;
}

/**
 * @brief Get current tick count value (for test access)
 * @retval type=[unsigned int] current g_tick_count value
 */
FW_STATIC uint32_t irq_get_tick(void) {
    return g_tick_count;
}

/**
 * @brief Reset all IRQ state to initial values (for test setup)
 */
FW_STATIC void irq_reset_all(void) {
    irq_pending = 0U;
    g_tick_count = 0U;
    exception_count = 0U;
}

/**
 * @brief Get current exception count value (for test access)
 * @retval type=[unsigned int] current exception_count value
 */
FW_STATIC uint32_t exception_get_count(void) {
    return exception_count;
}

/**
 * @brief Main entry point of the program
 * @remark Test Criteria: Program execution result.
 * @param [in] argc type=[int] R=[1-0x7fffffff] P=[Vail address] N=[0] D=[argument count]
 * @param [in] argv type=[char**] R=[Vail address] P=[Vail address] N=[NULL] D=[argument vector]
 * @retval type=[int] R=[0-0xff] P=[N/A] N=[N/A] D=[exit status]
 */
#ifndef TEST_BUILD
int32_t main(int32_t argc, char* argv[]) {
    char line[CLI_LINE_BUFFER_SIZE];
    int32_t input;
    uint32_t hex_val;

    (void)argc;
    (void)argv;

    tick_printf("Hello, TRAE IDE!\n");
    tick_printf("This is a C project built with CMake.\n");
    tick_printf("Tick and exception handlers registered.\n\n");

    tick_printf("========== IRQ Simulator ==========\n");
    tick_printf("  <num>   : trigger single IRQ (1-32 -> IRQ0-IRQ31)\n");
    tick_printf("  b<num>  : bit mode, trigger single IRQ (b1 -> IRQ1)\n");
    tick_printf("  h<hex>  : hex mode, trigger multiple IRQs (h3 -> IRQ0,1)\n");
    tick_printf("  0       : process all pending IRQs manually\n");
    tick_printf("  exit    : quit simulator\n");
    tick_printf("===================================\n\n");

    for (;;) {
        int input_raw;
        unsigned int hex_raw;
        int32_t done;

        tick_irq_handler();

        tick_printf("IRQ> ");
        (void)fflush(stdout);

        done = 0;

        if (fgets(line, sizeof(line), stdin) == NULL) {
            done = 1;
        } else if ((strcmp(line, "exit\n") == 0) || (strcmp(line, "exit\r\n") == 0)) {
            tick_printf("Exiting IRQ Simulator. Goodbye!\n");
            done = 1;
        } else if ((line[0] == 'b') || (line[0] == 'B')) {
            if ((sscanf(&line[1], "%d", &input_raw) == 1) && (input_raw >= 0) && ((uint32_t)input_raw < IRQ_COUNT)) {
                input = (int32_t)input_raw;
                irq_trigger((uint32_t)input);
                irq_process_all();
            } else {
                tick_printf("Invalid bit mode. Usage: b<0-31> (e.g. b1)\n");
            }
        } else if ((line[0] == 'h') || (line[0] == 'H')) {
            if (sscanf(&line[1], "%x", &hex_raw) == 1) {
                hex_val = (uint32_t)hex_raw;
                irq_pending |= hex_val;
                tick_printf("[IRQ] Hex trigger: pending = 0x%08X\n", irq_pending);
                irq_process_all();
            } else {
                tick_printf("Invalid hex mode. Usage: h<hex> (e.g. h3, hFF)\n");
            }
        } else if (sscanf(line, "%d", &input_raw) == 1) {
            input = (int32_t)input_raw;
            if (input == 0) {
                irq_process_all();
            } else if ((input >= 1) && (input <= 32)) {
                int32_t adjusted;
                adjusted = input - 1;
                irq_trigger((uint32_t)adjusted);
                irq_process_all();
            } else {
                tick_printf("Invalid IRQ number: %d (valid range: 1-32)\n", input);
            }
        } else {
            tick_printf("Invalid input. Use: <num>, b<num>, h<hex>, 0, or exit\n");
        }

        if (done != 0) {
            break;
        }
    }

    return 0;
}
#endif /* TEST_BUILD */