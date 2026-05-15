# IRQ Simulator - Integration Test Plan

## 1. Test Scope

Integration tests verify the interaction between multiple modules, including input parsing, end-to-end IRQ trigger and handling flow, and cross-module tick count consistency.

## 2. Test Environment

- Compiler: GCC (MinGW)
- Language Standard: C11
- Test Framework: Custom assert macros
- `irq_reset_all()` is called before each test case to reset state

```mermaid
flowchart TD
    START(["integrated_test main()"])
    RESET["irq_reset_all()"]
    SIM["simulate_input()<br/>Simulate user input"]
    ASSERT{"assert check"}
    PASS["PRINT: [PASS]"]
    FAIL["PRINT: [FAIL]"]
    NEXT{"More tests?"}
    REPORT["Output stats: Total / Passed / Failed"]
    END_(["return 0"])

    START --> RESET
    RESET --> SIM
    SIM --> ASSERT
    ASSERT -->|"Pass"| PASS
    ASSERT -->|"Fail"| FAIL
    PASS --> NEXT
    FAIL --> NEXT
    NEXT -->|"Yes"| RESET
    NEXT -->|"No"| REPORT
    REPORT --> END_
```

## 3. Test Cases

### IT_01: Numeric Mode Input Parsing

| ID | Test Item | Simulated Input | Expected Result |
|----|---------|---------|---------|
| IT_01_01 | Input 1 triggers IRQ0 | `"1"` | pending=0x01, IRQ0 handled, pending=0 |
| IT_01_02 | Input 32 triggers IRQ31 | `"32"` | pending=0x80000000, IRQ31 handled |
| IT_01_03 | Input 0 manual processing | trigger(3) → `"0"` | IRQ3 handled |
| IT_01_04 | Invalid number 33 | `"33"` | pending unchanged, error message output |
| IT_01_05 | Invalid number -5 | `"-5"` | pending unchanged, error message output |

### IT_02: b-mode Input Parsing

| ID | Test Item | Simulated Input | Expected Result |
|----|---------|---------|---------|
| IT_02_01 | b0 triggers IRQ0 | `"b0"` | pending=0x01, IRQ0 handled |
| IT_02_02 | b5 triggers IRQ5 | `"b5"` | pending=0x20, IRQ5 handled |
| IT_02_03 | b31 triggers IRQ31 | `"b31"` | pending=0x80000000, IRQ31 handled |
| IT_02_04 | B10 (uppercase) | `"B10"` | pending=0x400, IRQ10 handled |
| IT_02_05 | Invalid b32 | `"b32"` | pending unchanged, error output |
| IT_02_06 | Invalid b-1 | `"b-1"` | pending unchanged, error output |

### IT_03: h-mode Input Parsing

| ID | Test Item | Simulated Input | Expected Result |
|----|---------|---------|---------|
| IT_03_01 | h1 triggers IRQ0 | `"h1"` | pending=0x01, IRQ0 handled |
| IT_03_02 | h3 triggers IRQ0,1 | `"h3"` | IRQ0, IRQ1 handled in order |
| IT_03_03 | hFF triggers IRQ0~7 | `"hFF"` | IRQ0~7 all handled in order |
| IT_03_04 | h80000000 triggers IRQ31 | `"h80000000"` | IRQ31 handled |
| IT_03_05 | H0A (uppercase+hex) | `"H0A"` | pending=0x0A, IRQ1,3 handled |
| IT_03_06 | Invalid hGG | `"hGG"` | pending unchanged, error output |

### IT_04: Accumulated Trigger & Priority

| ID | Test Item | Steps | Expected Result |
|----|---------|------|---------|
| IT_04_01 | Trigger then h-mode append | trigger(0) → `"h6"` | IRQ0,1,2 handled in order |
| IT_04_02 | Multiple b-mode accumulation | `"b10"` → `"b5"` → `"0"` | IRQ5,10 handled in order |
| IT_04_03 | Priority order verification | `"h80000001"` | IRQ0 handled before IRQ31 |

### IT_05: Tick Count Consistency

| ID | Test Item | Steps | Expected Result |
|----|---------|------|---------|
| IT_05_01 | Initial tick is 0 | reset → get_tick | tick == 0 |
| IT_05_02 | IRQ0 increments tick by 1 | trigger(0) → process | tick incremented (IRQ0 handler +1) |
| IT_05_03 | Non-IRQ0 does not affect tick | trigger(5) → process | tick unchanged by IRQ5 |
| IT_05_04 | Multiple IRQ0 tick accumulation | trigger(0)→process, trigger(0)→process, trigger(0)→process | tick correctly accumulates to +3 |

### IT_06: Exit & Boundary Conditions

| ID | Test Item | Simulated Input | Expected Result |
|----|---------|---------|---------|
| IT_06_01 | exit normal termination | `"exit"` | Returns 0, outputs goodbye |
| IT_06_02 | Empty line input | `""` | Error prompt output, no crash |
| IT_06_03 | Garbage input | `"xyz"` | Error prompt output, no crash |

### IT_07: End-to-End Full Flow

| ID | Test Item | Steps | Expected Result |
|----|---------|------|---------|
| IT_07_01 | Complete operation sequence | `"1"` → `"b5"` → `"h3"` → `"exit"` | All IRQs correctly handled, normal exit |

## 4. Expected Results

- All IT_01 ~ IT_07 test cases must pass
- Pass rate: 100%

## 5. Integration Test Traceability

| ID | Chapter | Trace to SD | Trace to UT | Description |
|----|---------|-------------|-------------|-------------|
| IT_01 | 3 (IT_01) | SD_007<br>SD_010 | UT_03<br>UT_04<br>UT_05 | Numeric mode input parsing: IRQ0/IRQ31 trigger, manual processing (0), invalid range (33, -5) |
| IT_02 | 3 (IT_02) | SD_007<br>SD_010 | UT_03<br>UT_04<br>UT_05 | b-mode input parsing: b0/b5/b31 trigger, uppercase B10, invalid range (b32, b-1) |
| IT_03 | 3 (IT_03) | SD_007<br>SD_010<br>SD_014 | UT_05<br>UT_08 | h-mode input parsing: h1/h3/hFF/h80000000 trigger, uppercase H0A, invalid hGG |
| IT_04 | 3 (IT_04) | SD_004<br>SD_005<br>SD_007<br>SD_014 | UT_03<br>UT_05<br>UT_08<br>UT_10 | Accumulated trigger & priority: trigger+h-mode append, multi b-mode accumulation, IRQ0 before IRQ31 |
| IT_05 | 3 (IT_05) | SD_002<br>SD_006<br>SD_009 | UT_01<br>UT_04<br>UT_07 | Tick count consistency: initial 0, IRQ0 increments, non-IRQ0 no effect, multi accumulation |
| IT_06 | 3 (IT_06) | SD_007<br>SD_010 | — | Exit & boundary conditions: normal exit, empty input, garbage input (main loop parsing layer) |
| IT_07 | 3 (IT_07) | SD_004<br>SD_005<br>SD_006<br>SD_007<br>SD_014 | UT_03<br>UT_04<br>UT_05<br>UT_08 | End-to-end full flow: numeric→b-mode→h-mode→exit complete operation sequence |

### SD Coverage Mapping (Integration Test)

| SD Item | Covered by IT | Status |
|---------|---------------|--------|
| SD_001 | — | ⚠️ API declarations (verified in unit test) |
| SD_002 | IT_05 | ✅ Covered |
| SD_003 | IT_01 ~ IT_07 (all IT verify log output) | ✅ Covered |
| SD_004 | IT_04, IT_07 | ✅ Covered |
| SD_005 | IT_04, IT_07 | ✅ Covered |
| SD_006 | IT_05, IT_07 | ✅ Covered |
| SD_007 | IT_01, IT_02, IT_03, IT_04, IT_06, IT_07 | ✅ Covered |
| SD_008 | — | ⚠️ Data structure (verified in unit test) |
| SD_009 | IT_05 | ✅ Covered |
| SD_010 | IT_01, IT_02, IT_03, IT_06 | ✅ Covered |
| SD_011 | — | ⚠️ Design decision (verified in unit test) |
| SD_012 | IT_01 ~ IT_07 (all IT verify TICK_PRINTF macro) | ✅ Covered |
| SD_013 | — | ⚠️ Design decision (verified in unit test) |
| SD_014 | IT_03, IT_04, IT_07 | ✅ Covered |

> **Abbreviation Notes:**
>
> - **IT** = Integration Test (unified numbering for all integration test cases)
> - **UT** = Unit Test (traceability back to SWE.4 unit test items)
> - **SD** = Software Detailed Design (traceability back to SWE.3 detailed design items)