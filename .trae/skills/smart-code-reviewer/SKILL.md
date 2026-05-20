---
name: smart-code-reviewer
description: 對指定的程式碼片段進行深度 Code Review，找出潛在 Bug、效能瓶頸與資安漏洞。
---

# smart-code-reviewer

## Description
對指定的程式碼片段進行深度 Code Review，找出潛在 Bug、效能瓶頸與資安漏洞。

## Triggers
- 當用戶要求審查程式碼、優化效能，或輸入「請幫我 Review 這段 Code」時。

## Steps
1. 靜態分析：檢查程式碼是否存在語法錯誤、未處理的異常（Exception）或潛在的記憶體洩漏。
2. 資安漏洞掃描：評估是否有明文傳輸或權限控管不當的問題。
3. 效能評估：檢查是否有不必要的迴圈（例如 $O(N^2)$）、重複的 API 請求或資料庫 N+1 問題。
4. 提供重構建議：
   - 先用列表指出有問題的程式碼行數與原因。
   - 提供優化後的程式碼對比（Before vs After）。
   - 說明優化後帶來的具體好處（如：讀取時間減少、更易於維護）。