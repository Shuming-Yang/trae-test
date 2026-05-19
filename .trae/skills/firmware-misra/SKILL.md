---
name: firmware-misra
description: 專門用於 C 語言嵌入式韌體開發與 MISRA 合規性審查。當修改 C 原始碼、標頭檔，或調整 autobuild.sh 編譯腳本時自動觸發。
---

# Embedded Firmware & MISRA Compliance 指南

當此技能被觸發時，請協助確保韌體專案的安全性、穩定性與自動化完整性：

### 1. MISRA C 規範嚴格執行
在生成、修改或 Review C 語言程式碼時，必須主動防範違反 MISRA C 的常見陷阱：
*   **記憶體安全**：嚴禁使用動態記憶體配置（如 `malloc`, `free`）。
*   **指標安全**：限制指標的層級（原則上不超過兩層），避免未定義行為。
*   **型別明確**：禁止使用基本 C 型別（如 `int`, `char`, `short`），一律使用 `<stdint.h>` 定義的標準型別（如 `int32_t`, `uint8_t`）。
*   **控制流**：所有 `if-else` 與 `switch` 語句必須包含大括號 `{}`，且 `switch` 必須有 `default` 分支。

### 2. 自動化建置整合 (Autobuild Compatibility)
*   編寫的 C 程式碼必須能順利通過 `autobuild.sh` 所整合的靜態掃描工具。
*   若修改了建置腳本，須確保編譯階段（Compile Stages）與 MISRA 掃描作業（Scanning Jobs）能流暢地在 GitLab CI/CD 環境中運行，並在出錯時正確回傳非零（Non-zero）結束碼。