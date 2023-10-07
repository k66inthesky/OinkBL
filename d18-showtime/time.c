#include<Time.h>
#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

EFI_STATUS
EFIAPI
UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 sec=0;
    UINT8 min=0;
    UINT8 hour=0;
    UINT8 weekday=0;
    UINT8 day=0;
    UINT8 month=0;
    UINT8 year=0;
    
    // sec, 0x00
    IoWrite8(CMOS_ADDRESS,0x00);
    sec=IoRead8(CMOS_DATA);

    // min, 0x02
    IoWrite8(CMOS_ADDRESS,0x02);
    min=IoRead8(CMOS_DATA);

    // hour, 0x04
    IoWrite8(CMOS_ADDRESS,0x04);
    hour=IoRead8(CMOS_DATA);

    // weekday, 0x06
    IoWrite8(CMOS_ADDRESS,0x06);
    weekday=IoRead8(CMOS_DATA);

    // day, 0x07
    IoWrite8(CMOS_ADDRESS,0x07);
    day=IoRead8(CMOS_DATA);

    // month, 0x08
    IoWrite8(CMOS_ADDRESS,0x08);
    month=IoRead8(CMOS_DATA);

    // year, 0x09
    IoWrite8(CMOS_ADDRESS,0x09);
    year=IoRead8(CMOS_DATA);

    Print(L"Oink! Current time is: \n");
    Print(L"%02x/%02x/%02x  %02x  %02x:%02x:%02x\n",year,month,day,weekday,hour,min,sec);

    return Status;
}