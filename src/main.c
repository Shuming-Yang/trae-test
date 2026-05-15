/**
 * @file   main.c
 * @author Steven.Yang
 * @brief: Main entry point for test-trae project
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#include "main.h"

/* Global function pointers for exception handlers (referenced in start.s) */
void (*_rom_except_tick)(void) = 0;
void (*_rom_except_int)(void) = 0;

static uint32_t irq_pending = 0;
static unsigned int g_tick_count = 0;
static unsigned int exception_count = 0;

#define TICK_PRINTF(fmt, ...) printf("[tick: %05u] " fmt, g_tick_count, ##__VA_ARGS__)

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
void irq_trigger(unsigned int irq_num) {
    if (irq_num < IRQ_COUNT) {
        irq_pending |= (1U << irq_num);
        TICK_PRINTF("[IRQ] IRQ%u triggered (pending: 0x%08X)\n", irq_num, irq_pending);
    } else {
        TICK_PRINTF("[IRQ] Invalid IRQ number: %u (valid range: 0-%u)\n", irq_num, IRQ_COUNT - 1);
    }
}

/**
 * @brief Trigger multiple IRQs by raw mask (for h-mode / test use)
 * @param [in] mask type=[uint32_t] R=[0-0xFFFFFFFF] P=[N/A] N=[N/A] D=[bitmask of IRQs to trigger]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_trigger_raw(uint32_t mask) {
    irq_pending |= mask;
    TICK_PRINTF("[IRQ] Raw trigger: pending = 0x%08X\n", irq_pending);
}

/**
 * @brief IRQ handler: process a specific IRQ and clear its pending bit
 * @remark Test Criteria: IRQ handled and pending bit cleared.
 * @param [in] irq_num type=[unsigned int] R=[0-31] P=[0-31] N=[N/A] D=[IRQ number to handle]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_handler(unsigned int irq_num) {
    TICK_PRINTF("  -> Handling IRQ%u: ", irq_num);

    switch (irq_num) {
    case 0:
        TICK_PRINTF("System Timer IRQ - tick count updated\n");
        tick_irq_handler();
        break;
    case 1:
        TICK_PRINTF("UART0 RX IRQ - data received\n");
        break;
    case 2:
        TICK_PRINTF("UART0 TX IRQ - data transmitted\n");
        break;
    case 3:
        TICK_PRINTF("GPIO Port A IRQ - pin state changed\n");
        break;
    case 4:
        TICK_PRINTF("GPIO Port B IRQ - pin state changed\n");
        break;
    case 5:
        TICK_PRINTF("SPI0 IRQ - transfer complete\n");
        break;
    case 6:
        TICK_PRINTF("I2C0 IRQ - transaction done\n");
        break;
    case 7:
        TICK_PRINTF("ADC IRQ - conversion complete\n");
        break;
    case 8:
        TICK_PRINTF("DMA Channel 0 IRQ - transfer finished\n");
        break;
    case 9:
        TICK_PRINTF("DMA Channel 1 IRQ - transfer finished\n");
        break;
    case 10:
        TICK_PRINTF("Watchdog IRQ - timer expired\n");
        break;
    case 11:
        TICK_PRINTF("RTC IRQ - alarm triggered\n");
        break;
    case 12:
        TICK_PRINTF("USB IRQ - device event\n");
        break;
    case 13:
        TICK_PRINTF("CAN0 IRQ - message received\n");
        break;
    case 14:
        TICK_PRINTF("PWM IRQ - period elapsed\n");
        break;
    case 15:
        TICK_PRINTF("Timer1 IRQ - compare match\n");
        break;
    case 16:
        TICK_PRINTF("Timer2 IRQ - overflow\n");
        break;
    case 17:
        TICK_PRINTF("UART1 RX IRQ - data received\n");
        break;
    case 18:
        TICK_PRINTF("UART1 TX IRQ - data transmitted\n");
        break;
    case 19:
        TICK_PRINTF("SPI1 IRQ - transfer complete\n");
        break;
    case 20:
        TICK_PRINTF("I2C1 IRQ - transaction done\n");
        break;
    case 21:
        TICK_PRINTF("External INT0 - rising edge detected\n");
        break;
    case 22:
        TICK_PRINTF("External INT1 - falling edge detected\n");
        break;
    case 23:
        TICK_PRINTF("External INT2 - level triggered\n");
        break;
    case 24:
        TICK_PRINTF("DMA Channel 2 IRQ - transfer finished\n");
        break;
    case 25:
        TICK_PRINTF("DMA Channel 3 IRQ - transfer finished\n");
        break;
    case 26:
        TICK_PRINTF("CRC IRQ - calculation done\n");
        break;
    case 27:
        TICK_PRINTF("AES IRQ - encryption complete\n");
        break;
    case 28:
        TICK_PRINTF("QSPI IRQ - command finished\n");
        break;
    case 29:
        TICK_PRINTF("SDIO IRQ - card event\n");
        break;
    case 30:
        TICK_PRINTF("Ethernet IRQ - packet received\n");
        break;
    case 31:
        TICK_PRINTF("Exception IRQ - external interrupt\n");
        exception_irq_handler();
        break;
    default:
        TICK_PRINTF("Unknown IRQ\n");
        break;
    }

    irq_pending &= ~(1U << irq_num);
}

/**
 * @brief Process all pending IRQs from lowest to highest priority (IRQ0 = highest)
 * @remark Test Criteria: All pending IRQs processed in priority order.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_process_all(void) {
    unsigned int i;

    if (irq_pending == 0) {
        return;
    }

    TICK_PRINTF("\n=== Processing pending IRQs (0x%08X) ===\n", irq_pending);

    for (i = 0; i < IRQ_COUNT; i++) {
        if (irq_pending & (1U << i)) {
            irq_handler(i);
        }
    }

    TICK_PRINTF("=== All IRQs processed ===\n\n");
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
unsigned int irq_get_tick(void) {
    return g_tick_count;
}

/**
 * @brief Reset all IRQ state to initial values (for test setup)
 */
void irq_reset_all(void) {
    irq_pending = 0;
    g_tick_count = 0;
    exception_count = 0;
}

/**
 * @brief Get current exception count value (for test access)
 * @retval type=[unsigned int] current exception_count value
 */
unsigned int exception_get_count(void) {
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
int main(int argc, char* argv[]) {
    char line[64];
    int input;
    unsigned int hex_val;

    _rom_except_tick = tick_irq_handler;
    _rom_except_int = exception_irq_handler;

    TICK_PRINTF("Hello, TRAE IDE!\n");
    TICK_PRINTF("This is a C project built with CMake.\n");
    TICK_PRINTF("Tick and exception handlers registered.\n\n");

    TICK_PRINTF("========== IRQ Simulator ==========\n");
    TICK_PRINTF("  <num>   : trigger single IRQ (1-32 -> IRQ0-IRQ31)\n");
    TICK_PRINTF("  b<num>  : bit mode, trigger single IRQ (b1 -> IRQ1)\n");
    TICK_PRINTF("  h<hex>  : hex mode, trigger multiple IRQs (h3 -> IRQ0,1)\n");
    TICK_PRINTF("  0       : process all pending IRQs manually\n");
    TICK_PRINTF("  exit    : quit simulator\n");
    TICK_PRINTF("===================================\n\n");

    while (1) {
        tick_irq_handler();

        TICK_PRINTF("IRQ> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        if (strcmp(line, "exit\n") == 0 || strcmp(line, "exit\r\n") == 0) {
            TICK_PRINTF("Exiting IRQ Simulator. Goodbye!\n");
            break;
        }

        if (line[0] == 'b' || line[0] == 'B') {
            if (sscanf(line + 1, "%d", &input) == 1 && input >= 0 && input < (int)IRQ_COUNT) {
                irq_trigger((unsigned int)input);
                irq_process_all();
            } else {
                TICK_PRINTF("Invalid bit mode. Usage: b<0-31> (e.g. b1)\n");
            }
        } else if (line[0] == 'h' || line[0] == 'H') {
            if (sscanf(line + 1, "%x", &hex_val) == 1) {
                irq_pending |= hex_val;
                TICK_PRINTF("[IRQ] Hex trigger: pending = 0x%08X\n", irq_pending);
                irq_process_all();
            } else {
                TICK_PRINTF("Invalid hex mode. Usage: h<hex> (e.g. h3, hFF)\n");
            }
        } else if (sscanf(line, "%d", &input) == 1) {
            if (input == 0) {
                irq_process_all();
            } else if (input >= 1 && input <= 32) {
                irq_trigger((unsigned int)(input - 1));
                irq_process_all();
            } else {
                TICK_PRINTF("Invalid IRQ number: %d (valid range: 1-32)\n", input);
            }
        } else {
            TICK_PRINTF("Invalid input. Use: <num>, b<num>, h<hex>, 0, or exit\n");
        }
    }

    return 0;
}
#endif /* TEST_BUILD */
