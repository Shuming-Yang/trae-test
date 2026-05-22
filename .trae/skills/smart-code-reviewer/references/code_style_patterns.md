# 程式碼風格與優化對照組 (Good vs Bad Patterns)

本文件提供專案常見問題的標準重構範例。在 Step 4 (重構建議) 輸出時，應參考此文件的排版與優化邏輯。

## 範例 1：資安漏洞 - 字串緩衝區溢位 (Buffer Overflow)

### [❌ Bad Pattern]
直接使用無邊界的 `strcpy`，若 `src` 長度大於 `dest` 空間，將導致記憶體覆寫或系統崩潰。
```c
void set_device_name(char *dest, const char *src) {
    // 嚴重漏洞：未檢查 src 長度，可能導致 Buffer Overflow
    strcpy(dest, src); 
}
```

### [✓ Good Pattern]
使用 `strncpy` 並保留最後一個 Byte 強制補 `\0`，確保絕對安全。
```c
void set_device_name(char *dest, const char *src, size_t dest_size) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return;
    }
    // 安全做法：限制寫入長度
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0'; // 強制截斷並封口
}
```

---

## 範例 2：併發控制 - Mutex 死鎖風險 (Deadlock / Resource Leak)

### [❌ Bad Pattern]
在多退出點的函式中，若在中途因為條件不滿足而提前 `return`，容易遺漏釋放 Mutex 鎖，導致系統死鎖。
```c
int process_data(void) {
    pthread_mutex_lock(&data_mutex);
    
    if (!is_system_ready()) {
        return -1; // 錯誤：未釋放 data_mutex 就直接退出！
    }
    
    // 正常業務邏輯...
    pthread_mutex_unlock(&data_mutex);
    return 0;
}
```

### [✓ Good Pattern]
統一退出路徑，或在每個 `return` 前嚴格執行解鎖動作。
```c
int process_data(void) {
    int status = 0;
    pthread_mutex_lock(&data_mutex);
    
    if (!is_system_ready()) {
        status = -1;
        goto EXIT; // 推薦做法：透過 goto 統一集中至退出點處理資源釋放
    }
    
    // 正常業務邏輯...

EXIT:
    pthread_mutex_unlock(&data_mutex);
    return status;
}
```

---

## 範例 3：效能優化 - 巢狀迴圈與複雜度優化 (Big-O Optimization)

### [❌ Bad Pattern]
在嵌入式系統或大數據處理中，使用 O(N^2) 的雙重迴圈逐一比對資料，會隨數據量增加而造成 CPU 負載飆高。
```c
// 在不重複的陣列中，找出相加等於 target 的兩個元素的索引
void find_two_sum(int* nums, int numsSize, int target) {
    for (int i = 0; i < numsSize; i++) {
        for (int j = i + 1; j < numsSize; j++) {
            if (nums[i] + nums[j] == target) {
                printf("Index: %d, %d\n", i, j);
                return;
            }
        }
    }
}
```

### [✓ Good Pattern]
透過排序（如快速排序 O(N log N)）搭配雙指標技術（Two Pointers），將整體的搜尋複雜度降至 O(N log N)，大幅減少 CPU 運算週期。
```c
// 註：此處簡化邏輯，假設 nums 已是排序好的陣列，複雜度降至 O(N)
void find_two_sum_optimized(int* nums, int numsSize, int target) {
    int left = 0;
    int right = numsSize - 1;
    
    while (left < right) {
        int sum = nums[left] + nums[right];
        if (sum == target) {
            printf("Index: %d, %d\n", left, right);
            return;
        } else if (sum < target) {
            left++; // 總和太小，左指標右移
        } else {
            right--; // 總和太大，右指標左移
        }
    }
}
```
