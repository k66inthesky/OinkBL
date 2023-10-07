#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h> //gBS用
#include <Library/MemoryAllocationLib.h>

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    // Initial
    EFI_STATUS Status = EFI_SUCCESS;
    ImageHandle=NULL;
    SystemTable=NULL;
    if (EFI_ERROR(Status))
        return Status;

    // 創建一個按鍵事件
    EFI_EVENT KeyEvent;
    Status = gBS->CreateEvent(
        EVT_TIMER | EVT_NOTIFY_WAIT,
        TPL_CALLBACK,
        (EFI_EVENT_NOTIFY)NULL,
        NULL,
        &KeyEvent
    );
    if (EFI_ERROR(Status)) {
        Print(L"Cannot create a key Event: %r\n", Status);
        return Status;
    }

    // 設置定時器，以便在按下ESC鍵後定期檢查
    Status = gBS->SetTimer(
        KeyEvent,
        TimerRelative,
        10000000  // 1000萬微秒（1秒）
    );
    if (EFI_ERROR(Status)) {
        Print(L"Cannot set timer: %r\n", Status);
        return Status;
    }

    // 迴圈，等待ESC鍵按下
    Print(L"按下ESC鍵以停止程式...\n");
    while(1){
        UINTN Index;
        EFI_INPUT_KEY Key;
        Status = gBS->WaitForEvent(1, &KeyEvent, &Index);
        if (!EFI_ERROR(Status)) {
            Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
            if (!EFI_ERROR(Status) && Key.ScanCode == SCAN_ESC)
                break;  // 等ESC鍵被按下，退出迴圈
        }
    }

    // 釋放並退出
    gBS->CloseEvent(KeyEvent);
    return EFI_SUCCESS;
}