# IRQ Simulator - Unit Test Plan

## 1. Test Scope

Unit tests verify each independent function in `main.c`, ensuring correct behavior in an isolated environment.

## 2. Test Environment

- Compiler: GCC (MinGW)
- Language Standard: C11
- Test Framework: Custom assert macros (no external dependencies)
- `irq_reset_all()` is called before each test case to reset state

```mermaid
flowchart TD
    START(["unit_test main()"])
    RESET["irq_reset_all()"]
    RUN["Execute test function"]
    ASSERT{"assert check"}
    PASS["PRINT: [PASS]"]
    FAIL["PRINT: [FAIL]"]
    NEXT{"More tests?"}
    REPORT["Output stats: Total / Passed / Failed"]
    END_(["return 0"])

    START --> RESET
    RESET --> RUN
    RUN --> ASSERT
    ASSERT -->|"Pass"| PASS
    ASSERT -->|"Fail"| FAIL
    PASS --> NEXT
    FAIL --> NEXT
    NEXT -->|"Yes"| RESET
    NEXT -->|"No"| REPORT
    REPORT --> END_
```

## 3. Test Cases

### UT-01: tick_irq_handler

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-01-01 | Initial tick value | None | `irq_get_tick() == 0` |
| UT-01-02 | Single call | `tick_irq_handler()` | `irq_get_tick() == 1` |
| UT-01-03 | Multiple calls | Call 5 times | `irq_get_tick() == 5` |
| UT-01-04 | Call after reset | reset → call 3 times | `irq_get_tick() == 3` |

### UT-02: exception_irq_handler

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-02-01 | Function callable without crash | `exception_irq_handler()` | Returns normally |
| UT-02-02 | Multiple calls | Call 3 times | Returns normally, no side effects |

### UT-03: irq_trigger

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-03-01 | Trigger IRQ0 | `irq_trigger(0)` | `irq_get_pending() == 0x00000001` |
| UT-03-02 | Trigger IRQ5 | `irq_trigger(5)` | `irq_get_pending() == 0x00000020` |
| UT-03-03 | Trigger IRQ31 | `irq_trigger(31)` | `irq_get_pending() == 0x80000000` |
| UT-03-04 | Cumulative trigger | trigger(0), trigger(1) | `irq_get_pending() == 0x00000003` |
| UT-03-05 | Duplicate trigger | trigger(0), trigger(0) | `irq_get_pending() == 0x00000001` |
| UT-03-06 | Invalid IRQ (32) | `irq_trigger(32)` | pending unchanged |
| UT-03-07 | Invalid IRQ (99) | `irq_trigger(99)` | pending unchanged |

### UT-04: irq_handler

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-04-01 | Handle IRQ0 | trigger(0) → handler(0) | pending bit 0 cleared, tick+1 |
| UT-04-02 | Handle IRQ5 | trigger(5) → handler(5) | pending bit 5 cleared |
| UT-04-03 | Handle IRQ31 | trigger(31) → handler(31) | pending bit 31 cleared |
| UT-04-04 | Pending cleared after handling | trigger(0) → handler(0) | `irq_get_pending() == 0` |

### UT-05: irq_process_all

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-05-01 | No pending IRQs | `irq_process_all()` | Returns immediately, no action |
| UT-05-02 | Single IRQ | trigger(3) → process_all | IRQ3 handled, pending=0 |
| UT-05-03 | Multiple IRQs | trigger(0), trigger(5), trigger(10) | Processed in order 0→5→10, pending=0 |
| UT-05-04 | All IRQs | trigger all 0-31 | All processed, pending=0 |

### UT-06: irq_reset_all

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-06-01 | Reset pending | trigger(5) → reset | `irq_get_pending() == 0` |
| UT-06-02 | Reset tick | tick++ x3 → reset | `irq_get_tick() == 0` |
| UT-06-03 | Reset both | trigger + tick → reset | pending=0, tick=0 |

### UT-07: irq_get_pending / irq_get_tick

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-07-01 | Initial pending | reset → get_pending | Returns 0 |
| UT-07-02 | Initial tick | reset → get_tick | Returns 0 |
| UT-07-03 | Pending after trigger | trigger(7) → get_pending | Returns 0x00000080 |

## 4. Expected Results

- All UT-01 ~ UT-07 test cases must pass
- Pass rate: 100%