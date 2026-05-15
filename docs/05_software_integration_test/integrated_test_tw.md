# IRQ Simulator - Integration Test Plan

## 1. Test Scope

整合測試驗證多個模組之間的互動行為，包含輸入解析、IRQ 觸發與處理的端到端流程、tick 計數的跨模組一致性。

## 2. Test Environment

- 編譯器：GCC (MinGW)
- 語言標準：C11
- 測試框架：自訂 assert 巨集
- 每個測試案例前呼叫 `irq_reset_all()` 重置狀態

```mermaid
flowchart TD
    START(["integrated_test main()"])
    RESET["irq_reset_all()"]
    SIM["simulate_input()<br/>模擬使用者輸入"]
    ASSERT{"assert 檢查"}
    PASS["PRINT: [PASS]"]
    FAIL["PRINT: [FAIL]"]
    NEXT{"還有測試?"}
    REPORT["輸出統計: Total / Passed / Failed"]
    END_(["return 0"])

    START --> RESET
    RESET --> SIM
    SIM --> ASSERT
    ASSERT -->|"通過"| PASS
    ASSERT -->|"失敗"| FAIL
    PASS --> NEXT
    FAIL --> NEXT
    NEXT -->|"Yes"| RESET
    NEXT -->|"No"| REPORT
    REPORT --> END_
```

## 3. Test Cases

### IT_01: 數字模式輸入解析

| ID | 測試項目 | 模擬輸入 | 預期結果 |
|----|---------|---------|---------|
| IT_01_01 | 輸入 1 觸發 IRQ0 | `"1"` | pending=0x01, IRQ0 被處理, pending=0 |
| IT_01_02 | 輸入 32 觸發 IRQ31 | `"32"` | pending=0x80000000, IRQ31 被處理 |
| IT_01_03 | 輸入 0 手動處理 | trigger(3) → `"0"` | IRQ3 被處理 |
| IT_01_04 | 無效數字 33 | `"33"` | pending 不變，輸出錯誤訊息 |
| IT_01_05 | 無效數字 -5 | `"-5"` | pending 不變，輸出錯誤訊息 |

### IT_02: b-mode 輸入解析

| ID | 測試項目 | 模擬輸入 | 預期結果 |
|----|---------|---------|---------|
| IT_02_01 | b0 觸發 IRQ0 | `"b0"` | pending=0x01, IRQ0 被處理 |
| IT_02_02 | b5 觸發 IRQ5 | `"b5"` | pending=0x20, IRQ5 被處理 |
| IT_02_03 | b31 觸發 IRQ31 | `"b31"` | pending=0x80000000, IRQ31 被處理 |
| IT_02_04 | B10 (大寫) | `"B10"` | pending=0x400, IRQ10 被處理 |
| IT_02_05 | 無效 b32 | `"b32"` | pending 不變，輸出錯誤 |
| IT_02_06 | 無效 b-1 | `"b-1"` | pending 不變，輸出錯誤 |

### IT_03: h-mode 輸入解析

| ID | 測試項目 | 模擬輸入 | 預期結果 |
|----|---------|---------|---------|
| IT_03_01 | h1 觸發 IRQ0 | `"h1"` | pending=0x01, IRQ0 被處理 |
| IT_03_02 | h3 觸發 IRQ0,1 | `"h3"` | IRQ0, IRQ1 依序被處理 |
| IT_03_03 | hFF 觸發 IRQ0~7 | `"hFF"` | IRQ0~7 全部依序處理 |
| IT_03_04 | h80000000 觸發 IRQ31 | `"h80000000"` | IRQ31 被處理 |
| IT_03_05 | H0A (大寫+hex) | `"H0A"` | pending=0x0A, IRQ1,3 被處理 |
| IT_03_06 | 無效 hGG | `"hGG"` | pending 不變，輸出錯誤 |

### IT_04: 累積觸發與優先權

| ID | 測試項目 | 步驟 | 預期結果 |
|----|---------|------|---------|
| IT_04_01 | 先觸發再 h-mode 追加 | trigger(0) → `"h6"` | IRQ0,1,2 依序處理 |
| IT_04_02 | 多次 b-mode 累積 | `"b10"` → `"b5"` → `"0"` | IRQ5,10 依序處理 |
| IT_04_03 | 優先權順序驗證 | `"h80000001"` | IRQ0 先於 IRQ31 處理 |

### IT_05: Tick 計數一致性

| ID | 測試項目 | 步驟 | 預期結果 |
|----|---------|------|---------|
| IT_05_01 | 初始 tick 為 0 | reset → get_tick | tick == 0 |
| IT_05_02 | 觸發 IRQ0 後 tick+1 | trigger(0) → process | tick 增加 (IRQ0 handler +1) |
| IT_05_03 | 非 IRQ0 不影響 tick | trigger(5) → process | tick 不因 IRQ5 而增加 |
| IT_05_04 | 多次 IRQ0 tick 累計 | trigger(0)→process, trigger(0)→process, trigger(0)→process | tick 正確累加 3 |

### IT_06: exit 與邊界條件

| ID | 測試項目 | 模擬輸入 | 預期結果 |
|----|---------|---------|---------|
| IT_06_01 | exit 正常退出 | `"exit"` | 回傳 0，輸出 goodbye |
| IT_06_02 | 空行輸入 | `""` | 輸出錯誤提示，不崩潰 |
| IT_06_03 | 亂碼輸入 | `"xyz"` | 輸出錯誤提示，不崩潰 |

### IT_07: 端到端完整流程

| ID | 測試項目 | 步驟 | 預期結果 |
|----|---------|------|---------|
| IT_07_01 | 完整操作序列 | `"1"` → `"b5"` → `"h3"` → `"exit"` | 所有 IRQ 正確處理，正常退出 |

## 4. Expected Results

- 所有 IT_01 ~ IT_07 測試案例須全部通過
- 通過率：100%

## 5. 整合測試追溯表

| ID | 章節 | 追溯 SD | 追溯 UT | 描述 |
|----|------|---------|---------|------|
| IT_01 | 3 (IT_01) | SD_007<br>SD_010 | UT_03<br>UT_04<br>UT_05 | 數字模式輸入解析：IRQ0/IRQ31 觸發、手動處理 (0)、無效範圍 (33, -5) |
| IT_02 | 3 (IT_02) | SD_007<br>SD_010 | UT_03<br>UT_04<br>UT_05 | b-mode 輸入解析：b0/b5/b31 觸發、大寫 B10、無效範圍 (b32, b-1) |
| IT_03 | 3 (IT_03) | SD_007<br>SD_010<br>SD_014 | UT_05<br>UT_08 | h-mode 輸入解析：h1/h3/hFF/h80000000 觸發、大寫 H0A、無效 hGG |
| IT_04 | 3 (IT_04) | SD_004<br>SD_005<br>SD_007<br>SD_014 | UT_03<br>UT_05<br>UT_08<br>UT_10 | 累積觸發與優先權：trigger+h-mode 追加、多次 b-mode 累積、IRQ0 優先於 IRQ31 |
| IT_05 | 3 (IT_05) | SD_002<br>SD_006<br>SD_009 | UT_01<br>UT_04<br>UT_07 | Tick 計數一致性：初始為 0、IRQ0 遞增、非 IRQ0 不影響、多次累加 |
| IT_06 | 3 (IT_06) | SD_007<br>SD_010 | — | exit 與邊界條件：正常退出、空行輸入、亂碼輸入（主迴圈解析層驗證） |
| IT_07 | 3 (IT_07) | SD_004<br>SD_005<br>SD_006<br>SD_007<br>SD_014 | UT_03<br>UT_04<br>UT_05<br>UT_08 | 端到端完整流程：數字→b-mode→h-mode→exit 全操作序列 |

### SD 覆蓋對照表（整合測試）

| SD 項 | 覆蓋 IT | 狀態 |
|---------|---------------|--------|
| SD_001 | — | ⚠️ API 宣告（單元測試驗證） |
| SD_002 | IT_05 | ✅ 已覆蓋 |
| SD_003 | IT_01 ~ IT_07（全部 IT 驗證日誌輸出） | ✅ 已覆蓋 |
| SD_004 | IT_04, IT_07 | ✅ 已覆蓋 |
| SD_005 | IT_04, IT_07 | ✅ 已覆蓋 |
| SD_006 | IT_05, IT_07 | ✅ 已覆蓋 |
| SD_007 | IT_01, IT_02, IT_03, IT_04, IT_06, IT_07 | ✅ 已覆蓋 |
| SD_008 | — | ⚠️ 資料結構（單元測試驗證） |
| SD_009 | IT_05 | ✅ 已覆蓋 |
| SD_010 | IT_01, IT_02, IT_03, IT_06 | ✅ 已覆蓋 |
| SD_011 | — | ⚠️ 設計決策（單元測試驗證） |
| SD_012 | IT_01 ~ IT_07（全部 IT 驗證 TICK_PRINTF 巨集） | ✅ 已覆蓋 |
| SD_013 | — | ⚠️ 設計決策（單元測試驗證） |
| SD_014 | IT_03, IT_04, IT_07 | ✅ 已覆蓋 |

> **縮寫說明：**
>
> - **IT** = Integration Test（整合測試，為所有整合測試案例的統一編號）
> - **UT** = Unit Test（單元測試，追溯至 SWE.4 單元測試項）
> - **SD** = Software Detailed Design（軟體詳細設計，追溯至 SWE.3 詳細設計項）