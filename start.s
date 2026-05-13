/**
 * @file   start.s
 * @author Steven.Yang
 * @brief: Assembly startup and interrupt handler routines
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

	.global _rom_except_tick          # 宣告 _rom_except_tick 為全域符號，可被外部引用
	.global _rom_except_int           # 宣告 _rom_except_int 為全域符號，可被外部引用

/*
	tick handler - Tick 中斷處理常式
*/
_tick_isr:
	/* restart tick */
	l.mfspr r2, r0, 20480             # 從特殊暫存器 (SPR) 20480 (Tick 計數器) 讀取值到 r2
	l.ori   r4, r0, 268435456         # 將立即數 0x10000000 (268435456) 載入 r4 (設定 Tick 致能位元)
	l.and   r3, r2, r4                # r2 與 r4 做邏輯 AND，結果存到 r3 (檢查 Tick 中斷旗標)
	l.sfeq  r3, r4                    # 設定旗標：如果 r3 等於 r4，則設定條件碼
	l.nop                              # 無操作 (延遲槽)

	/* clear tick interrupt */
	l.andi	r3, r2, 4026531839        # r2 與 0xEFFFFFFF (4026531839) 做 AND，清除 Tick 中斷致能位元
	l.mtspr r0,r3, 20480              # 將 r3 的值寫回特殊暫存器 20480，清除 Tick 中斷

	/*
		if(rom_except_tick){
			rom_except_tick();
		}
	*/
	l.ori	r3,r0,_rom_except_tick     # 載入 _rom_except_tick 函數指標的地址到 r3
	l.lwz	r4,0(r3)                   # 從 r3 指向的記憶體位置載入一個字組 (32位元) 到 r4
	l.sfeqi	r4,0                       # 設定旗標：如果 r4 等於 0，則設定條件碼
	l.nop                              # 無操作 (延遲槽，若條件成立則跳過呼叫)

/*
	interrupt handler - 外部中斷處理常式
*/
_int_isr:
	/* clear interrupt */
	l.mtspr r0,r0,18434               # 寫入特殊暫存器 18434，清除中斷狀態

	/*
		if(rom_except_int){
			rom_except_int();
		}
	*/
	l.ori	r3,r0,_rom_except_int      # 載入 _rom_except_int 函數指標的地址到 r3
	l.lwz	r4,0(r3)                   # 從 r3 指向的記憶體位置載入一個字組 (32位元) 到 r4
	l.sfeqi	r4,0                       # 設定旗標：如果 r4 等於 0，則設定條件碼
	l.nop                              # 無操作 (延遲槽，若條件成立則跳過呼叫)

/*
   	entry - 重置異常入口點
*/
_except_reset:
    /* startup */
	l.movhi  r2,hi(_main)             # 將 _main 函數地址的高16位載入到寄存器 r2 中
	l.ori    r2,r2,lo(_main)          # 將 _main 函數地址的低16位與 r2 當前值結合，完成地址載入
	l.jr     r2                       # 跳轉到 _main 函數執行
	l.nop                             # 無操作指令，用於跳轉延遲槽