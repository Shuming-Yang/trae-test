---
alwaysApply: true
---

# 項目基本資訊與技術棧
- 當前項目主要技術：C, Python 3.10+, JavaScript (Node.js), Docker。
- Python程式碼檢查工具：項目強制使用 Mypy/Pyright 進行靜態型別檢查，程式碼必須符合 MISRA 規範或嚴格的 Linter 標準。
- C程式碼檢查工具：項目強制使用 CppCheck 加上其MISRA插件 進行靜態型別檢查，程式碼必須符合 MISRA 規範或嚴格的 Linter 標準。
- 測試框架：Unit Test 與 Integrata Test為最基本的測試需求規範並且需要使用gcovr來產生HTML覆蓋測試報告。

# 專案架構與禁忌（Do's and Don'ts）
- 禁止行為：
  - 嚴禁在任何程式碼中寫死（Hardcode）密碼、API Key、Token 或個人隱私資訊。所有敏感資訊必須自環境變數（Environment Variables）讀取。
  - 嚴禁直接在 Controller/UI 層編寫資料庫或底層硬體的控制邏輯，必須透過 Service 層或 Driver 層進行解耦。
- 強制規範：
  - Python: 所有的非同步操作（Async/Await）必須包裹 `try...catch` 或異常處理機制，並記錄錯誤日誌（Log）。
  - C: 所有的實作必須在需要時添加錯誤異常處理機制，尤其是指針的操作運用之前並記錄錯誤日誌（Log）。
  - 新增任何核心功能，必須同時在 `unit_test/` 目錄下產生對應的單元測試（Unit Test）。
  - 新增任何核心功能，必須同時在 `integration_test/` 目錄下產生對應的整合與介面接口測試（Integration Test）。

# Code Review 流程規範
- 每次 `src/` 目錄內的 C 原始碼或標頭檔發生變動時，使用者會觸發 `smart-code-reviewer` 技能進行審查。
- AI 必須在執行新的 Code Review 之前，先回溯比對 `docs/code_review_record/code_review.md` 中前次 review 遺留的 issue：
  - 若 issue 已於當前程式碼中修復 → 自動標記為 `**fixed**` + 當天日期（使用 `python tools/gen_code_review.py --fix-issue N`）
  - 若 issue 尚未修復 → 逐個詢問使用者是否維持 `pending` 或改為 `❌ ignored`（使用 `python tools/gen_code_review.py --ignore-issue N`）
  - 若先前標記為 `❌ ignored` 的 issue 已被修復 → 自動改為 `**fixed**`
- 新的 review 結果必須以 per-issue 表格格式寫入同一個檔案，每筆新 issue 初始狀態為 `pending`
- review 紀錄檔位置：`docs/code_review_record/code_review.md`
- CI pipeline 中的 `gen_code_review.py` 僅負責偵測 `src/` 變動並建立新的 review entry 模板，**不執行 auto-fix**（auto-fix 由 AI 在 Code Review 階段處理）

# 文件規範
- 修改或新增 API/函式時，必須同步更新 Docstring，並確保符合 MkDocs/Sphinx 的文件生成格式。