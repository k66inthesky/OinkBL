[Defines]
    PLATFORM_NAME = NoPigKillPkg                          // 資料夾名稱
    PLATFORM_GUID = f2b2247d-b9ce-4ced-a58c-a534925ab4e3  // 需一GUID，需與inf的不同
    PLATFORM_VERSION = 0.1                                // 需給一版本號，自訂
    DSC_SPECIFICATION = 0x00010005                        // 建議此版本
    SUPPORTED_ARCHITECTURES = X64                         // X64表示x86的64位元
    BUILD_TARGETS = DEBUG|RELEASE                         // DEBUG或RELEASE


// [LibraryClasses]寫所有需要用到的inf，通常先填UefiLib和UefiApplicationEntryPoint，
// 及ｃ有用到的函式如Print()的BasePrintLib，其他就看報錯缺什麼填什麼。寫UEFI APPLICATION最痛苦之處~
[LibraryClasses]                                             
    UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
    UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
    PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
    PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
    MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
    DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
    BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
    BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
    UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
    DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
    UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
    RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
    DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
    
[Components]
    NoPigKillPkg/NoPigKill.inf  // NoPigKill.inf位置