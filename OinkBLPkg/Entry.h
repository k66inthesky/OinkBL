/* Entry */
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>

#include <Library/BaseLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/SimpleFileSystem.h>
#include <IndustryStandard/Bmp.h>
#include <Guid/FileInfo.h>

/* Time */
#include<Library/TimerLib.h>
#include<Library/IoLib.h>
#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71
