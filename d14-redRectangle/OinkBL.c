#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h> //include此Library可直接使用gBS，不然則須寫EFI_BOOT_SERVICES *gBS = SystemTable->BootServices;
#include <Library/BmpSupportLib.h>
#include <Library/MemoryAllocationLib.h>
#include <IndustryStandard/Bmp.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>

EFI_STATUS
EFIAPI
UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;
    /* LocateHandleBuffer() */
    UINTN NoHandles = 0;
    EFI_HANDLE *Buffer = NULL;
    Status = gBS->LocateHandleBuffer( // [註1]
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &NoHandles,
        &Buffer
    );

    Print(L"Status = %d.\n", Status);

    if(EFI_ERROR(Status)) { // 開發UEFI時隨時檢查並印出可能有錯誤處，方便日後查看報錯訊息。
        Print(L"Failed to LocateHandleBuffer.\n");
        return Status;
    }
    Print(L"Hello gEfiGraphicsOutputProtocolGuid Protocol.\n");
    Print(L"Number of Handles = %d.\n", NoHandles);
    

    // OpenProtocol
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    Status = gBS->OpenProtocol( // 把Drvier綁至Controller，OpenProtocol這個structure有四個變數: QueryMode, SetMode, Blt, *Mode。
        Buffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)&Gop,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );

    Print(L"Status = %d.\n", Status);
    if(EFI_ERROR(Status)) {
        Print(L"Failed to OpenProtocol\n");
        return Status;
    }

    /* 輸出螢幕分辨率：在UEFI BIOS時需手動調整螢幕解析度，進OS後就不用(Windows/Linux/MacOS會自動調整)。*/

    /* QueryMode查詢顯卡支援的顯示模式 */
    UINTN SizeofInfo = 0;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    for(UINT8 i = 0; i < Gop->Mode->MaxMode; i++)
    {
        Status = Gop->QueryMode(
            Gop,
            i,
            &SizeofInfo,
            &Info
        );
        if(EFI_ERROR(Status))
        {
            Print(L"Failed To QueryMode.\n");
            return Status;
        }
        // 印出所有支援的螢幕解析度的Mode,打開QEMU右上角view->serial就能看到
        Print(L"Mode = %d, H = %d, V = %d.\n",
            i,
            Info->HorizontalResolution,
            Info->VerticalResolution);
    }

    Status = Gop->SetMode(Gop, 22); // 我們選1920x1080，對應上述印出結果是是序號22，且螢幕會被清空
    if(EFI_ERROR(Status))
    {
        Print(L"Failed to SetMode.\n");
        return Status;
    }
    
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Red = {0,0,255,0}; //[自我挑戰6]
    Status = Gop->Blt (
                Gop,//This
                &Red, //圖(需是EFI_GRAPHICS_OUTPUT_BLT_PIXEL格式)
                EfiBltVideoFill, // 顯示於螢幕上有幾種方式：EfiBltVideoFill、EfiBltVideoToVideo
                0,0, // 起點座標 x,y
                0,0, // 終點座標 x,y
                100,100,//PixelWidth,PixelHeight
                0//Delta
            );
     return Status;
}
