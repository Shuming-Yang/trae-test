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

# 文件規範
- 修改或新增 API/函式時，必須同步更新 Docstring，並確保符合 MkDocs/Sphinx 的文件生成格式。
