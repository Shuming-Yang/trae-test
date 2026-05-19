---
name: python-qc
description: 專門用於 Python 程式碼編寫與品質控制。當需要編寫、重構 Python 函式、物件導向架構，或解決型別錯誤時自動觸發。
---

# Python Quality Control 指南

當此技能被觸發時，請在生成或審查 Python 程式碼時嚴格執行以下規範：

### 1. 嚴格的型別宣告 (Strict Type Hinting)
*   所有函式與方法的參數、返回值必須宣告明確的型別。
*   不允許使用模糊的 `Any`，除非在極端無法避免的情況下，且必須加上註解說明原因。
*   複雜結構優先使用 `typing` 模組中的 `List`, `Dict`, `Optional`, `Union` 或 `Callable`。
*   程式碼必須能完美通過 `mypy` 與 `pyright` 的最高等級靜態檢查。

### 2. 模組化與可測試性
*   程式碼設計必須遵循單一職責原則（Single Responsibility Principle）。
*   在設計桌面驗證工具（GUI）相關的後端邏輯時，界面（UI）與商業邏輯（Logic）必須徹底分離，確保邏輯層可進行獨立的單元測試（Unit Test）。

### 3. 文件與註解
*   所有類別與公開函式必須包含符合 Google Style 或 Sphinx 風格的 Docstring。
*   註解一律使用**繁體中文（台灣）**，且只說明「為什麼這樣寫（Why）」，而非「這行在做什麼（What）」。
