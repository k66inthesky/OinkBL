; OinkBL.asm
org 0x7c00 ; 最前面的32KB-512B(MBR)-512B(data stack)=7C00

section .text
    global _start

_start:
    ; 開始前先清除螢幕畫面(清屏)
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ah, 0x06  ; AH=0x06 清屏
    mov al, 0x00  ; AL=0x00 清屏
    mov bh, 0x07  ; 顏色，黑底白字
    mov cx, 0x00  ; 清屏行數
    mov dx, 0x184f ; 清屏的結束位置
    int 0x10      ; 呼叫BIOS的int 0x10，執行清屏

    ; 在屏幕上顯示一條消息
    mov ah, 0x0e  ; 在TTY下寫字符
    mov al, 'N'
    int 0x10
    mov al, 'o'
    int 0x10
    ; 省略...請練習以int 0x10 mov al, '字母', 自己寫出No pig killing!

    ; 無窮迴圈
    cli           ; 禁用中斷
    hlt           ; 暫停CPU

times 510 - ($-$$) db 0
dw 0xaa55       ; 魔數，表示引導扇區的結束