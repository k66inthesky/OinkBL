#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>


static void Kernel(CHAR16 keyChar)
{
    EFI_HANDLE ImageHandle;
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FILE_PROTOCOL *bin;
    if (keyChar=='1')
    {
        Print(L"Enter 1 kernel...\n");
        Status = GetFileHandle(ImageHandle, L"\\Kernel1.bin",&bin);
    }
    if (keyChar=='2')
    {
        Print(L"Enter 2 kernel...\n");
        Status = GetFileHandle(ImageHandle, L"\\Kernel2.bin",&bin);
    }
    EFI_PHYSICAL_ADDRESS binAddr;
    Status = ReadFile(bin, &binAddr);
    asm("jmp %0"::"m"(binAddr)); // %0是第1個位址、%1是第2個位址，以此類推
}

static void BootMenu()
{
    // 設定字為red(0x04)，並且清除畫面
    gST -> ConOut -> SetAttribute(gST->ConOut,0x4);
    gST -> ConOut -> ClearScreen(gST->ConOut);
    Print(L"*******************************\n");
    Print(L"|          Boot Menu          |\n");
    Print(L"|-----------------------------|\n");
    Print(L"|                             |\n");
    Print(L"|                             |\n");
    Print(L"|                             |\n");
    Print(L"|                             |\n");
    Print(L"*******************************\n");
    
    while(1)
    {
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
        // 暫且設定僅兩種Kernel: Kernel1.bin及Kernel2.bin
        if(key.UnicodeChar == L'1' || key.UnicodeChar == L'2' )
        {
            Kernel(key.UnicodeChar);
            break;
        }
    }
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    // Initial
    EFI_STATUS Status = EFI_SUCCESS;
    ImageHandle=NULL;
    SystemTable=NULL;
    
    Print(L"*******************************\n");
    Print(L"|  Press B goto  Boot Menu    |\n");
    Print(L"|-----------------------------|\n");
    Print(L"|                             |\n");
    Print(L"|                             |\n");
    Print(L"|                             |\n");
    Print(L"|                             |\n");
    Print(L"*******************************\n");

    // 取key鍵值
    // UINTN Index;
    EFI_INPUT_KEY key;
    Print(L"start boot services...");
    // SystemTable->BootServices->WaitForEvent(1,&(SystemTable->ConIn->WaitForKey),&Index);
    // SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn,&key);
    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
    Print(L"read<> %c ", key.UnicodeChar);
    // 若輸入B，則進入Boot Menu
    while(1)
    {
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
        if(key.UnicodeChar == L'B')
        {
            BootMenu();
            break;
        }
    }    

    return Status;
}
