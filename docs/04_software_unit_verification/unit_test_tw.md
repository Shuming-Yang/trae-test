# IRQ Simulator - Unit Test Plan

## 1. Test Scope

單元測試針對 `main.c` 中的每個獨立函式進行驗證，確保各函式在隔離環境下行為正確。

## 2. Test Environment

- 編譯器：GCC (MinGW)
- 語言標準：C11
- 測試框架：自訂 assert 巨集（無外部依賴）
- 每個測試案例前呼叫 `irq_reset_all()` 重置狀態

```mermaid
flowchart TD
    START(["unit_test main()"])
    RESET["irq_reset_all()"]
    RUN["執行測試函式"]
    ASSERT{"assert 檢查"}
    PASS["PRINT: [PASS]"]
    FAIL["PRINT: [FAIL]"]
    NEXT{"還有測試?"}
    REPORT["輸出統計: Total / Passed / Failed"]
    END_(["return 0"])

    START --> RESET
    RESET --> RUN
    RUN --> ASSERT
    ASSERT -->|"通過"| PASS
    ASSERT -->|"失敗"| FAIL
    PASS --> NEXT
    FAIL --> NEXT
    NEXT -->|"Yes"| RESET
    NEXT -->|"No"| REPORT
    REPORT --> END_
```

## 3. Test Cases

### UT_01: tick_irq_handler

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_01_01 | tick 初始值 | 無 | `irq_get_tick() == 0` |
| UT_01_02 | 單次呼叫 | `tick_irq_handler()` | `irq_get_tick() == 1` |
| UT_01_03 | 多次呼叫 | 呼叫 5 次 | `irq_get_tick() == 5` |
| UT_01_04 | 重置後呼叫 | reset → 呼叫 3 次 | `irq_get_tick() == 3` |

### UT_02: exception_irq_handler

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_02_01 | 函式可被呼叫不崩潰 | `exception_irq_handler()` | 正常返回 |
| UT_02_02 | 多次呼叫 | 呼叫 3 次 | 正常返回，無副作用 |

### UT_03: irq_trigger

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_03_01 | 觸發 IRQ0 | `irq_trigger(0)` | `irq_get_pending() == 0x00000001` |
| UT_03_02 | 觸發 IRQ5 | `irq_trigger(5)` | `irq_get_pending() == 0x00000020` |
| UT_03_03 | 觸發 IRQ31 | `irq_trigger(31)` | `irq_get_pending() == 0x80000000` |
| UT_03_04 | 累積觸發 | trigger(0), trigger(1) | `irq_get_pending() == 0x00000003` |
| UT_03_05 | 重複觸發 | trigger(0), trigger(0) | `irq_get_pending() == 0x00000001` |
| UT_03_06 | 無效 IRQ (32) | `irq_trigger(32)` | pending 不變 |
| UT_03_07 | 無效 IRQ (99) | `irq_trigger(99)` | pending 不變 |

### UT_04: irq_handler

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_04_01 | 處理 IRQ0 | trigger(0) → handler(0) | pending bit 0 清除，tick+1 |
| UT_04_02 | 處理 IRQ5 | trigger(5) → handler(5) | pending bit 5 清除 |
| UT_04_03 | 處理 IRQ31 | trigger(31) → handler(31) | pending bit 31 清除 |
| UT_04_04 | 處理後 pending 歸零 | trigger(0) → handler(0) | `irq_get_pending() == 0` |

### UT_05: irq_process_all

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_05_01 | 無 pending 時 | `irq_process_all()` | 直接返回，無動作 |
| UT_05_02 | 單一 IRQ | trigger(3) → process_all | IRQ3 被處理，pending=0 |
| UT_05_03 | 多重 IRQ | trigger(0), trigger(5), trigger(10) | 依序處理 0→5→10，pending=0 |
| UT_05_04 | 全 IRQ | trigger all 0-31 | 全部處理完畢，pending=0 |

### UT_06: irq_reset_all

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_06_01 | 重置 pending | trigger(5) → reset | `irq_get_pending() == 0` |
| UT_06_02 | 重置 tick | tick++ x3 → reset | `irq_get_tick() == 0` |
| UT_06_03 | 重置兩者 | trigger + tick → reset | pending=0, tick=0 |

### UT_07: irq_get_pending / irq_get_tick

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_07_01 | 初始 pending | reset → get_pending | 回傳 0 |
| UT_07_02 | 初始 tick | reset → get_tick | 回傳 0 |
| UT_07_03 | 觸發後 pending | trigger(7) → get_pending | 回傳 0x00000080 |

### UT_08: irq_trigger_raw

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_08_01 | 單 bit raw mask | `irq_trigger_raw(0x00000001)` | `irq_get_pending() == 0x00000001` |
| UT_08_02 | 多 bit raw mask | `irq_trigger_raw(0x0000000F)` | `irq_get_pending() == 0x0000000F` |
| UT_08_03 | 累積 OR 行為 | trigger(0), trigger_raw(0x0006) | `irq_get_pending() == 0x00000007` |
| UT_08_04 | 零遮罩（無操作） | `irq_trigger_raw(0x00000000)` | pending 不變 |
| UT_08_05 | 全遮罩（全部 32 bit） | `irq_trigger_raw(0xFFFFFFFF)` | `irq_get_pending() == 0xFFFFFFFF` |
| UT_08_06 | 邊界：僅 MSB (IRQ31) | `irq_trigger_raw(0x80000000)` | `irq_get_pending() == 0x80000000` |

### UT_09: irq_handler（邊界案例）

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_09_01 | 無 pending bit 時呼叫 handler | `irq_handler(0)`（未觸發） | 不崩潰，pending 不變 |
| UT_09_02 | 處理中間 IRQ (IRQ15) | trigger(15) → handler(15) | pending bit 15 清除 |
| UT_09_03 | handler 僅清除目標 bit | trigger(0), trigger(1) → handler(0) | bit 0 清除，bit 1 保持置位 |

### UT_10: irq_process_all（邊界案例）

| ID | 測試項目 | 輸入 | 預期結果 |
|----|---------|------|---------|
| UT_10_01 | 僅最高優先權 (IRQ0) | trigger(0) → process_all | IRQ0 被處理，tick 遞增 |
| UT_10_02 | 僅最低優先權 (IRQ31) | trigger(31) → process_all | IRQ31 被處理，pending=0 |
| UT_10_03 | 優先權順序驗證 | trigger(31), trigger(0) → process_all | IRQ0 先於 IRQ31 被處理 |

## 4. Expected Results

- 所有 UT_01 ~ UT_10 測試案例須全部通過
- 通過率：100%

## 5. 單元測試追溯表

| ID | 章節 | 追溯 SD | 描述 |
|----|------|---------|------|
| UT_01 | 3 (UT_01) | SD_009 | `tick_irq_handler`：初始值、單次呼叫、多次呼叫、重置後行為 |
| UT_02 | 3 (UT_02) | SD_006 | `exception_irq_handler`：可呼叫不崩潰、多次呼叫無副作用 |
| UT_03 | 3 (UT_03) | SD_004<br>SD_008<br>SD_010 | `irq_trigger`：邊界 (IRQ0/IRQ5/IRQ31)、累積、重複、無效範圍 (32, 99) |
| UT_04 | 3 (UT_04) | SD_006<br>SD_013 | `irq_handler`：分發 (IRQ0/IRQ5/IRQ31)、處理後 pending bit 清除 |
| UT_05 | 3 (UT_05) | SD_005 | `irq_process_all`：空 pending、單一 IRQ、多重 IRQ、全部 32 個 IRQ |
| UT_06 | 3 (UT_06) | SD_002<br>SD_009<br>SD_011 | `irq_reset_all`：重置 pending、重置 tick、同時重置兩者 |
| UT_07 | 3 (UT_07) | SD_002<br>SD_011 | `irq_get_pending` / `irq_get_tick`：初始值、觸發後回讀 |
| UT_08 | 3 (UT_08) | SD_014<br>SD_010 | `irq_trigger_raw`：單/多/零/全遮罩、累積 OR、MSB 邊界 |
| UT_09 | 3 (UT_09) | SD_006<br>SD_013 | `irq_handler` 邊界：無 pending 呼叫、中間 IRQ (IRQ15)、僅清除目標 bit |
| UT_10 | 3 (UT_10) | SD_005 | `irq_process_all` 邊界：僅最高/最低優先權、優先權順序驗證 |

### SD 覆蓋對照表

| SD 項 | 覆蓋 UT | 狀態 |
|---------|---------------|--------|
| SD_001 | UT_01 ~ UT_10（全部 9 個 API 函式已測） | ✅ 已覆蓋 |
| SD_002 | UT_06, UT_07 | ✅ 已覆蓋 |
| SD_003 | — | ⚠️ 日誌輸出（整合測試驗證） |
| SD_004 | UT_03 | ✅ 已覆蓋 |
| SD_005 | UT_05, UT_10 | ✅ 已覆蓋 |
| SD_006 | UT_02, UT_04, UT_09 | ✅ 已覆蓋 |
| SD_007 | — | ⚠️ 主迴圈解析（整合測試驗證） |
| SD_008 | UT_03 | ✅ 已覆蓋 |
| SD_009 | UT_01, UT_06 | ✅ 已覆蓋 |
| SD_010 | UT_03, UT_08 | ✅ 已覆蓋 |
| SD_011 | UT_06, UT_07 | ✅ 已覆蓋 |
| SD_012 | — | ⚠️ 日誌輸出（整合測試驗證） |
| SD_013 | UT_04, UT_09 | ✅ 已覆蓋 |
| SD_014 | UT_08 | ✅ 已覆蓋 |

> **縮寫說明：**
>
> - **UT** = Unit Test（單元測試，為所有單元測試案例的統一編號）
> - **SD** = Software Detailed Design（軟體詳細設計，追溯至 SWE.3 詳細設計項）