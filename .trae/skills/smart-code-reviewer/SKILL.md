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

### Step 0: 回溯比對舊 Issue（每次 Code Review 前強制執行）
1. 讀取 `docs/code_review_record/code_review.md`。
2. 找出最近一筆 review entry 中所有狀態為 `pending` 或 `❌ ignored` 的 issue。
3. 對每一個 issue：
   - 讀取該 issue 引用的原始檔（`File` 欄位）與行號（`Line(s)` 欄位）。
   - 比對**當前原始碼**，判斷該 issue 是否已被修正。
   - **若已修正** → 使用 `python tools/gen_code_review.py --fix-issue N` 自動將狀態改為 `**fixed**` 並填入當天日期。
   - **若尚未修正** → 逐個詢問使用者：「Issue #N 仍未修復，是否維持 `pending` 或改為 `❌ ignored`？」
   - **若先前為 `❌ ignored` 的 issue 已被修正** → 使用 `python tools/gen_code_review.py --fix-issue N` 自動改為 `**fixed**`。
4. 回溯比對完成後，才進入 Step 1 的靜態分析。

### Step 1: 靜態分析
檢查程式碼是否存在語法錯誤、未處理的異常（Exception）或潛在的記憶體洩漏。

### Step 2: 資安漏洞掃描
評估是否有明文傳輸或權限控管不當的問題。

### Step 3: 效能評估
檢查是否有不必要的迴圈（例如 $O(N^2)$）、重複的 API 請求或資料庫 N+1 問題。

### Step 4: 提供重構建議
- 先用列表指出有問題的程式碼行數與原因。
- 提供優化後的程式碼對比（Before vs After）。
- 說明優化後帶來的具體好處（如：讀取時間減少、更易於維護）。

### Step 5: 寫入 Review 紀錄
- 將本次 review 的所有 issue 以 per-issue 表格格式寫入 `docs/code_review_record/code_review.md`。
- 每筆新 issue 初始狀態為 `pending`。
- 表格欄位：`# | Severity | File | Line(s) | Category | Issue | Status | Fix Date`。
- 若 `docs/code_review_record/code_review.md` 中尚無本次變動檔案的 review entry，先使用 `python tools/gen_code_review.py` 建立。