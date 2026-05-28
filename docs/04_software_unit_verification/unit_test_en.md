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

### UT_01: tick_irq_handler

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_001 | Initial tick value | None | `irq_get_tick() == 0` | FR-05 |
| UT_002 | Single call | `tick_irq_handler()` | `irq_get_tick() == 1` | FR-05 |
| UT_003 | Multiple calls | Call 5 times | `irq_get_tick() == 5` | FR-05 |
| UT_004 | Call after reset | reset → call 3 times | `irq_get_tick() == 3` | FR-05 |

### UT_02: exception_irq_handler

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_005 | Function callable without crash | `exception_irq_handler()` | Returns normally | FR-04 |
| UT_006 | Multiple calls | Call 3 times | Returns normally, no side effects | FR-04 |
| UT_007 | Internal counter verification | Call 3 times | `exception_get_count() == 3` | FR-04 |

### UT_03: irq_trigger

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_008 | Trigger IRQ0 | `irq_trigger(0)` | `irq_get_pending() == 0x00000001` | FR-01, FR-04 |
| UT_009 | Trigger IRQ5 | `irq_trigger(5)` | `irq_get_pending() == 0x00000020` | FR-01, FR-04 |
| UT_010 | Trigger IRQ31 | `irq_trigger(31)` | `irq_get_pending() == 0x80000000` | FR-01, FR-04 |
| UT_011 | Cumulative trigger | trigger(0), trigger(1) | `irq_get_pending() == 0x00000003` | FR-01, FR-04 |
| UT_012 | Duplicate trigger | trigger(0), trigger(0) | `irq_get_pending() == 0x00000001` | FR-01, FR-04 |
| UT_013 | Invalid IRQ (32) | `irq_trigger(32)` | pending unchanged | FR-01 |
| UT_014 | Invalid IRQ (99) | `irq_trigger(99)` | pending unchanged | FR-01 |

### UT_04: irq_handler

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_015 | Handle IRQ0 | trigger(0) → handler(0) | pending bit 0 cleared, tick+1 | FR-04, FR-05 |
| UT_016 | Handle IRQ5 | trigger(5) → handler(5) | pending bit 5 cleared | FR-04 |
| UT_017 | Handle IRQ31 | trigger(31) → handler(31) | pending bit 31 cleared | FR-04 |
| UT_018 | Pending cleared after handling | trigger(0) → handler(0) | `irq_get_pending() == 0` | FR-04 |
| UT_019 | Invalid IRQ number (default branch) | `irq_handler(99)` | No crash, prints "Unknown IRQ" | FR-04 |

### UT_05: irq_process_all

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_020 | No pending IRQs | `irq_process_all()` | Returns immediately, no action | FR-00 |
| UT_021 | Single IRQ | trigger(3) → process_all | IRQ3 handled, pending=0 | FR-04 |
| UT_022 | Multiple IRQs | trigger(0), trigger(5), trigger(10) | Processed in order 0→5→10, pending=0 | FR-03, FR-04 |
| UT_023 | All IRQs | trigger all 0-31 | All processed, pending=0 | FR-03, FR-04 |

### UT_06: irq_reset_all

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_024 | Reset pending | trigger(5) → reset | `irq_get_pending() == 0` | FR-06 |
| UT_025 | Reset tick | tick++ x3 → reset | `irq_get_tick() == 0` | FR-05, FR-06 |
| UT_026 | Reset both | trigger + tick → reset | pending=0, tick=0 | FR-05, FR-06 |

### UT_07: irq_get_pending / irq_get_tick

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_027 | Initial pending | reset → get_pending | Returns 0 | FR-06 |
| UT_028 | Initial tick | reset → get_tick | Returns 0 | FR-05, FR-06 |
| UT_029 | Pending after trigger | trigger(7) → get_pending | Returns 0x00000080 | FR-01 |
| UT_030 | Non-zero tick value | tick×3 → get_tick | Returns 3 | FR-05 |

### UT_08: irq_trigger_raw

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_031 | Single bit via raw mask | `irq_trigger_raw(0x00000001)` | `irq_get_pending() == 0x00000001` | FR-01 |
| UT_032 | Multiple bits via raw mask | `irq_trigger_raw(0x0000000F)` | `irq_get_pending() == 0x0000000F` | FR-01 |
| UT_033 | Cumulative OR behavior | trigger(0), trigger_raw(0x0006) | `irq_get_pending() == 0x00000007` | FR-01 |
| UT_034 | Zero mask (no-op) | `irq_trigger_raw(0x00000000)` | pending unchanged | FR-01 |
| UT_035 | Full mask (all 32 bits) | `irq_trigger_raw(0xFFFFFFFF)` | `irq_get_pending() == 0xFFFFFFFF` | FR-01 |
| UT_036 | Boundary: MSB only (IRQ31) | `irq_trigger_raw(0x80000000)` | `irq_get_pending() == 0x80000000` | FR-01 |

### UT_09: irq_handler (Boundary Cases)

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_037 | Handler without pending bit | `irq_handler(0)` (no trigger) | No crash, pending unchanged | FR-04 |
| UT_038 | Handler for middle IRQ (IRQ15) | trigger(15) → handler(15) | pending bit 15 cleared | FR-04 |
| UT_039 | Handler clears only target bit | trigger(0), trigger(1) → handler(0) | bit 0 cleared, bit 1 remains set | FR-04 |

### UT_10: irq_process_all (Boundary Cases)

| ID | Test Item | Input | Expected Result | SRS ID |
|----|---------|------|---------|--------|
| UT_040 | Highest priority only (IRQ0) | trigger(0) → process_all | IRQ0 handled, tick incremented | FR-03, FR-04, FR-05 |
| UT_041 | Lowest priority only (IRQ31) | trigger(31) → process_all | IRQ31 handled, pending=0 | FR-03, FR-04 |
| UT_042 | Priority order verification | trigger(31), trigger(0) → process_all | IRQ0 processed before IRQ31 | FR-03, FR-04, FR-05 |

## 4. Expected Results

- All UT_01 ~ UT_10 test cases must pass
- Pass rate: 100%

## 5. Unit Test Traceability

| ID | Chapter | Trace to SD | Description |
|----|---------|-------------|-------------|
| UT_001-UT_004 | 3 (UT_01) | SD_009 | `tick_irq_handler`: initial value, single call, multiple calls, post-reset behavior |
| UT_005-UT_007 | 3 (UT_02) | SD_006 | `exception_irq_handler`: callable without crash, multiple calls with no side effects, internal counter verification |
| UT_008-UT_014 | 3 (UT_03) | SD_004<br>SD_008<br>SD_010 | `irq_trigger`: boundary (IRQ0/IRQ5/IRQ31), cumulative, duplicate, invalid range (32, 99) |
| UT_015-UT_019 | 3 (UT_04) | SD_006<br>SD_013 | `irq_handler`: dispatch (IRQ0/IRQ5/IRQ31), pending bit cleared after handling, invalid IRQ number (default branch) |
| UT_020-UT_023 | 3 (UT_05) | SD_005 | `irq_process_all`: empty pending, single IRQ, multiple IRQs, all 32 IRQs |
| UT_024-UT_026 | 3 (UT_06) | SD_002<br>SD_009<br>SD_011 | `irq_reset_all`: reset pending, reset tick, reset both simultaneously |
| UT_027-UT_030 | 3 (UT_07) | SD_002<br>SD_011 | `irq_get_pending` / `irq_get_tick`: initial values, post-trigger readback, non-zero tick value |
| UT_031-UT_036 | 3 (UT_08) | SD_014<br>SD_010 | `irq_trigger_raw`: single/multiple/zero/full mask, cumulative OR, MSB boundary |
| UT_037-UT_039 | 3 (UT_09) | SD_006<br>SD_013 | `irq_handler` boundary: no-pending call, middle IRQ (IRQ15), only target bit cleared |
| UT_040-UT_042 | 3 (UT_10) | SD_005 | `irq_process_all` boundary: highest/lowest priority only, priority order verification |

### SD Coverage Mapping

| SD Item | Covered by UT | Status |
|---------|---------------|--------|
| SD_001 | UT_01 ~ UT_10 (all 9 API functions tested) | ✅ Covered |
| SD_002 | UT_06, UT_07 | ✅ Covered |
| SD_003 | — | ⚠️ Log output (verified in integration test) |
| SD_004 | UT_03 | ✅ Covered |
| SD_005 | UT_05, UT_10 | ✅ Covered |
| SD_006 | UT_02, UT_04, UT_09 | ✅ Covered |
| SD_007 | — | ⚠️ Main loop parsing (verified in integration test) |
| SD_008 | UT_03 | ✅ Covered |
| SD_009 | UT_01, UT_06 | ✅ Covered |
| SD_010 | UT_03, UT_08 | ✅ Covered |
| SD_011 | UT_06, UT_07 | ✅ Covered |
| SD_012 | — | ⚠️ Log output (verified in integration test) |
| SD_013 | UT_04, UT_09 | ✅ Covered |
| SD_014 | UT_08 | ✅ Covered |

> **Abbreviation Notes:**
>
> - **UT** = Unit Test (unified numbering for all unit test cases)
> - **SD** = Software Detailed Design (traceability back to SWE.3 detailed design items)