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
#include <stdio.h>

/* Global function pointers for exception handlers (referenced in start.s) */
void (*rom_except_tick)(void) = NULL;
void (*rom_except_int)(void) = NULL;

static uint32_t irq_pending = 0U;
static uint32_t g_tick_count = 0U;
static uint32_t exception_count = 0U;

static void tick_printf(const char *fmt, ...) {
    va_list args;
    (void)printf("[tick: %05u] ", g_tick_count);
    va_start(args, fmt);
    (void)vprintf(fmt, args);
    va_end(args);
}

/**
 * @brief Tick interrupt handler function
 * @remark Test Criteria: Function executed on tick interrupt.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void tick_irq_handler(void) {
    g_tick_count++;
}

/**
 * @brief Exception interrupt handler function
 * @remark Test Criteria: Function executed on external interrupt.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void exception_irq_handler(void) {
    exception_count++;
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
void irq_trigger_raw(uint32_t mask) {
    irq_pending |= mask;
    tick_printf("[IRQ] Raw trigger: pending = 0x%08X\n", irq_pending);
}

/**
 * @brief IRQ handler: process a specific IRQ and clear its pending bit
 * @remark Test Criteria: IRQ handled and pending bit cleared.
 * @param [in] irq_num type=[unsigned int] R=[0-31] P=[0-31] N=[N/A] D=[IRQ number to handle]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_handler(uint32_t irq_num) {
    tick_printf("  -> Handling IRQ%u: ", irq_num);

    switch (irq_num) {
    case 0:
        tick_printf("System Timer IRQ - tick count updated\n");
        tick_irq_handler();
        break;
    case 1:
        tick_printf("UART0 RX IRQ - data received\n");
        break;
    case 2:
        tick_printf("UART0 TX IRQ - data transmitted\n");
        break;
    case 3:
        tick_printf("GPIO Port A IRQ - pin state changed\n");
        break;
    case 4:
        tick_printf("GPIO Port B IRQ - pin state changed\n");
        break;
    case 5:
        tick_printf("SPI0 IRQ - transfer complete\n");
        break;
    case 6:
        tick_printf("I2C0 IRQ - transaction done\n");
        break;
    case 7:
        tick_printf("ADC IRQ - conversion complete\n");
        break;
    case 8:
        tick_printf("DMA Channel 0 IRQ - transfer finished\n");
        break;
    case 9:
        tick_printf("DMA Channel 1 IRQ - transfer finished\n");
        break;
    case 10:
        tick_printf("Watchdog IRQ - timer expired\n");
        break;
    case 11:
        tick_printf("RTC IRQ - alarm triggered\n");
        break;
    case 12:
        tick_printf("USB IRQ - device event\n");
        break;
    case 13:
        tick_printf("CAN0 IRQ - message received\n");
        break;
    case 14:
        tick_printf("PWM IRQ - period elapsed\n");
        break;
    case 15:
        tick_printf("Timer1 IRQ - compare match\n");
        break;
    case 16:
        tick_printf("Timer2 IRQ - overflow\n");
        break;
    case 17:
        tick_printf("UART1 RX IRQ - data received\n");
        break;
    case 18:
        tick_printf("UART1 TX IRQ - data transmitted\n");
        break;
    case 19:
        tick_printf("SPI1 IRQ - transfer complete\n");
        break;
    case 20:
        tick_printf("I2C1 IRQ - transaction done\n");
        break;
    case 21:
        tick_printf("External INT0 - rising edge detected\n");
        break;
    case 22:
        tick_printf("External INT1 - falling edge detected\n");
        break;
    case 23:
        tick_printf("External INT2 - level triggered\n");
        break;
    case 24:
        tick_printf("DMA Channel 2 IRQ - transfer finished\n");
        break;
    case 25:
        tick_printf("DMA Channel 3 IRQ - transfer finished\n");
        break;
    case 26:
        tick_printf("CRC IRQ - calculation done\n");
        break;
    case 27:
        tick_printf("AES IRQ - encryption complete\n");
        break;
    case 28:
        tick_printf("QSPI IRQ - command finished\n");
        break;
    case 29:
        tick_printf("SDIO IRQ - card event\n");
        break;
    case 30:
        tick_printf("Ethernet IRQ - packet received\n");
        break;
    case 31:
        tick_printf("Exception IRQ - external interrupt\n");
        exception_irq_handler();
        break;
    default:
        tick_printf("Unknown IRQ\n");
        break;
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
uint32_t irq_get_pending(void) {
    return irq_pending;
}

/**
 * @brief Get current tick count value (for test access)
 * @retval type=[unsigned int] current g_tick_count value
 */
uint32_t irq_get_tick(void) {
    return g_tick_count;
}

/**
 * @brief Reset all IRQ state to initial values (for test setup)
 */
void irq_reset_all(void) {
    irq_pending = 0U;
    g_tick_count = 0U;
    exception_count = 0U;
}

/**
 * @brief Get current exception count value (for test access)
 * @retval type=[unsigned int] current exception_count value
 */
uint32_t exception_get_count(void) {
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
    char line[64];
    int32_t input;
    uint32_t hex_val;

    (void)argc;
    (void)argv;

    rom_except_tick = tick_irq_handler;
    rom_except_int = exception_irq_handler;

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
            if ((sscanf(&line[1], "%d", &input) == 1) && (input >= 0) && ((uint32_t)input < IRQ_COUNT)) {
                irq_trigger((uint32_t)input);
                irq_process_all();
            } else {
                tick_printf("Invalid bit mode. Usage: b<0-31> (e.g. b1)\n");
            }
        } else if ((line[0] == 'h') || (line[0] == 'H')) {
            if (sscanf(&line[1], "%x", &hex_val) == 1) {
                irq_pending |= hex_val;
                tick_printf("[IRQ] Hex trigger: pending = 0x%08X\n", irq_pending);
                irq_process_all();
            } else {
                tick_printf("Invalid hex mode. Usage: h<hex> (e.g. h3, hFF)\n");
            }
        } else if (sscanf(line, "%d", &input) == 1) {
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