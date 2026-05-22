# 嵌入式 C 安全與穩定性審查規範 (MISRA-C 核心摘要)

當執行 Step 1 (靜態分析) 與 Step 2 (資安掃描) 時，必須嚴格比對程式碼是否符合以下嵌入式核心安全條款。

## 1. 記憶體與指標安全 (Memory & Pointer Safety)
*   **不允許動態記憶體分配 (避免記憶體碎片化)**：
    - 嚴禁在初始化之後使用 malloc(), calloc(), realloc(), 或 free()。
    - 應優先使用編譯期即決定大小的靜態陣列 (Static) 或配置好的記憶體池 (Memory Pool)。
*   **指標防空檢查 (Null Pointer Verification)**：
    - 任何從外部傳入、全域取得、或作為函式參數的指標，在解引用 (Dereference) 前必須進行 if (ptr != NULL) 的防空檢查。

## 2. 標準函式庫限制 (Standard Library Restrictions)
*   **嚴禁使用無邊界限制的字串操作函式 (防範緩衝區溢位)**：
    - 絕對禁止使用 strcpy(), strcat(), sprintf(), gets()。
    - 必須強制替換為具備長度限制的安全版本，如 strncpy(), strncat(), snprintf()，且必須手動確保字串以 \0 結尾。

## 3. 資源與併發控制 (Resource & Concurrency Management)
*   **函式返回路徑的資源釋放 (Resource Leakage)**：
    - 函式內若有複數個退出點 (return 語句)，必須確保每一個退出點都已正確釋放該函式內取得的資源。
    - 包含但不限於：動態鎖 (Mutex Unlock)、關閉檔案描述符 (File Descriptor)、動態記憶體釋放。
*   **忽略回傳值 (Ignored Return Values)**：
    - 所有具備非 void 回傳值的關鍵函式（如系統 API、硬體讀取、佇列操作），其回傳值必須被檢查或處理。若刻意不處理，必須顯式轉型為 (void)。
