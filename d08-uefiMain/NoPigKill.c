#include<Uefi.h> 　　　　　　　　// Uefi所需最基本庫
#include<Library/UefiLib.h> 　　// Print庫
EFI_STATUS 　　　　　　　　　　　 // 宣告函數的型別，實作UEFI APPICATION使用EFI_STATUS是硬性規定。 
UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) // 宣告UefiMain()函式，名稱要與INF檔的ENTRY_POINT名稱一致。　　　　　　　
{
    Print(L"No pig killing!\n");　 // 輸出字串，L: Long
    return 0; 　　　　　　　　　　　// C語言函數一定得有返回值
}