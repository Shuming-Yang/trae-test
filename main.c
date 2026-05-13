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

/**
 * @brief Tick interrupt handler function
 * @remark Test Criteria: Function executed on tick interrupt.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void tick_irq_handler(void) {
    /* TODO: Implement tick interrupt handling logic */
    static unsigned int tick_count = 0;
    tick_count++;
}

/**
 * @brief Exception interrupt handler function
 * @remark Test Criteria: Function executed on external interrupt.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void exception_irq_handler(void) {
    /* TODO: Implement external interrupt handling logic */
    static unsigned int exception_count = 0;
    exception_count++;
}

/**
 * @brief Main entry point of the program
 * @remark Test Criteria: Program execution result.
 * @param [in] argc type=[int] R=[1-0x7fffffff] P=[Vail address] N=[0] D=[argument count]
 * @param [in] argv type=[char**] R=[Vail address] P=[Vail address] N=[NULL] D=[argument vector]
 * @retval type=[int] R=[0-0xff] P=[N/A] N=[N/A] D=[exit status]
 */
int main(int argc, char* argv[]) {
    /* Register the interrupt handlers */
    _rom_except_tick = tick_irq_handler;
    _rom_except_int = exception_irq_handler;

    printf("Hello, TRAE IDE!\n");
    printf("This is a C project built with CMake.\n");
    printf("Tick and exception handlers registered.\n");
    
    /* Main loop */
    while (1) {
        /* TODO: Add main application logic */
    }
    
    return 0;
}