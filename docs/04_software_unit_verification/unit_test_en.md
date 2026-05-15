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

### UT-08: irq_trigger_raw

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-08-01 | Single bit via raw mask | `irq_trigger_raw(0x00000001)` | `irq_get_pending() == 0x00000001` |
| UT-08-02 | Multiple bits via raw mask | `irq_trigger_raw(0x0000000F)` | `irq_get_pending() == 0x0000000F` |
| UT-08-03 | Cumulative OR behavior | trigger(0), trigger_raw(0x0006) | `irq_get_pending() == 0x00000007` |
| UT-08-04 | Zero mask (no-op) | `irq_trigger_raw(0x00000000)` | pending unchanged |
| UT-08-05 | Full mask (all 32 bits) | `irq_trigger_raw(0xFFFFFFFF)` | `irq_get_pending() == 0xFFFFFFFF` |
| UT-08-06 | Boundary: MSB only (IRQ31) | `irq_trigger_raw(0x80000000)` | `irq_get_pending() == 0x80000000` |

### UT-09: irq_handler (Boundary Cases)

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-09-01 | Handler without pending bit | `irq_handler(0)` (no trigger) | No crash, pending unchanged |
| UT-09-02 | Handler for middle IRQ (IRQ15) | trigger(15) → handler(15) | pending bit 15 cleared |
| UT-09-03 | Handler clears only target bit | trigger(0), trigger(1) → handler(0) | bit 0 cleared, bit 1 remains set |

### UT-10: irq_process_all (Boundary Cases)

| ID | Test Item | Input | Expected Result |
|----|---------|------|---------|
| UT-10-01 | Highest priority only (IRQ0) | trigger(0) → process_all | IRQ0 handled, tick incremented |
| UT-10-02 | Lowest priority only (IRQ31) | trigger(31) → process_all | IRQ31 handled, pending=0 |
| UT-10-03 | Priority order verification | trigger(31), trigger(0) → process_all | IRQ0 processed before IRQ31 |

## 4. Expected Results

- All UT-01 ~ UT-10 test cases must pass
- Pass rate: 100%

## 5. Unit Test Traceability

| ID | Chapter | Trace to SD | Description |
|----|---------|-------------|-------------|
| UT-01 | 3 (UT-01) | SD_009 | `tick_irq_handler`: initial value, single call, multiple calls, post-reset behavior |
| UT-02 | 3 (UT-02) | SD_006 | `exception_irq_handler`: callable without crash, multiple calls with no side effects |
| UT-03 | 3 (UT-03) | SD_004<br>SD_008<br>SD_010 | `irq_trigger`: boundary (IRQ0/IRQ5/IRQ31), cumulative, duplicate, invalid range (32, 99) |
| UT-04 | 3 (UT-04) | SD_006<br>SD_013 | `irq_handler`: dispatch (IRQ0/IRQ5/IRQ31), pending bit cleared after handling |
| UT-05 | 3 (UT-05) | SD_005 | `irq_process_all`: empty pending, single IRQ, multiple IRQs, all 32 IRQs |
| UT-06 | 3 (UT-06) | SD_002<br>SD_009<br>SD_011 | `irq_reset_all`: reset pending, reset tick, reset both simultaneously |
| UT-07 | 3 (UT-07) | SD_002<br>SD_011 | `irq_get_pending` / `irq_get_tick`: initial values, post-trigger readback |
| UT-08 | 3 (UT-08) | SD_014<br>SD_010 | `irq_trigger_raw`: single/multiple/zero/full mask, cumulative OR, MSB boundary |
| UT-09 | 3 (UT-09) | SD_006<br>SD_013 | `irq_handler` boundary: no-pending call, middle IRQ (IRQ15), only target bit cleared |
| UT-10 | 3 (UT-10) | SD_005 | `irq_process_all` boundary: highest/lowest priority only, priority order verification |

### SD Coverage Mapping

| SD Item | Covered by UT | Status |
|---------|---------------|--------|
| SD_001 | UT-01 ~ UT-10 (all 9 API functions tested) | ✅ Covered |
| SD_002 | UT-06, UT-07 | ✅ Covered |
| SD_003 | — | ⚠️ Log output (verified in integration test) |
| SD_004 | UT-03 | ✅ Covered |
| SD_005 | UT-05, UT-10 | ✅ Covered |
| SD_006 | UT-02, UT-04, UT-09 | ✅ Covered |
| SD_007 | — | ⚠️ Main loop parsing (verified in integration test) |
| SD_008 | UT-03 | ✅ Covered |
| SD_009 | UT-01, UT-06 | ✅ Covered |
| SD_010 | UT-03, UT-08 | ✅ Covered |
| SD_011 | UT-06, UT-07 | ✅ Covered |
| SD_012 | — | ⚠️ Log output (verified in integration test) |
| SD_013 | UT-04, UT-09 | ✅ Covered |
| SD_014 | UT-08 | ✅ Covered |

> **Abbreviation Notes:**
>
> - **UT** = Unit Test (unified numbering for all unit test cases)
> - **SD** = Software Detailed Design (traceability back to SWE.3 detailed design items)