BITS 64 ; 若不指定，預設是16bits
mov ecx, 0x001FA400 ; 1920x1080=0x001FA400
mov eax, 0xFF18679A ; 藍色十六進制是0x18679A，先轉B(0x9A)G(0x67)R(0x18)(0xFF)，再little alien
xor rdi, rdi ; 暫存器選rdi是因為64位元，buffer之間要存值，得透過站存器，賦值前先清零。
mov rdi, 0xC0000000 ; 寫入的目的地址

Write:
    mov [rdi], eax ; eax賦值給給rdi的address
    add rdi, 4 ; 每次rdi暫存器的值都加4
    loop Write ; 迴圈

jmp $ ; 為了避免程式執行完畢回到UEFI Shell，故保持當下畫面。