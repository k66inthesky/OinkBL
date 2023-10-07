#include<Entry.h>
#include<PaperScissorsStone.c>

EFI_STATUS ConvertBmpToBlt(IN VOID *BmpImage, IN UINTN BmpImageSize, IN OUT VOID **GopBlt, IN OUT UINTN *GopBltSize, OUT UINTN *PixelHeight, OUT UINTN *PixelWidth)
{

	UINT8                         *ImageData;
	UINT8                         *ImageBegin;
	BMP_IMAGE_HEADER              *BmpHeader;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
	UINT64                        BltBufferSize;
	UINTN                         Height;
	UINTN                         Width;
	UINTN                         ImageIndex;

	BmpHeader = (BMP_IMAGE_HEADER *)BmpImage;
	ImageBegin = ((UINT8 *)BmpImage) + BmpHeader->ImageOffset;

	if (BmpHeader->BitPerPixel != 24 && BmpHeader->BitPerPixel != 32)
		return EFI_UNSUPPORTED;

	BltBufferSize = BmpHeader->PixelWidth * BmpHeader->PixelHeight * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
	*GopBltSize = (UINTN)BltBufferSize;
	*GopBlt = AllocatePool(*GopBltSize);   
	if (*GopBlt == NULL)
	{
		return EFI_OUT_OF_RESOURCES;
	}

	*PixelWidth = BmpHeader->PixelWidth;
	*PixelHeight = BmpHeader->PixelHeight;

	ImageData = ImageBegin;
	BltBuffer = *GopBlt;
	for (Height = 0; Height < BmpHeader->PixelHeight; Height++)
	{
		Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
		for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Blt++)
		{
			switch (BmpHeader->BitPerPixel)
			{
			case 24:
				Blt->Blue = *ImageData++;
				Blt->Green = *ImageData++;
				Blt->Red = *ImageData++;
				break;
			case 32:
				ImageData++;
				Blt->Blue = *ImageData++;
				Blt->Green = *ImageData++;
				Blt->Red = *ImageData++;
				break;
			default:
				break;
			}
		}

		ImageIndex = (UINTN)(ImageData - ImageBegin);
		if ((ImageIndex % 4) != 0)
			ImageData = ImageData + (4 - (ImageIndex % 4));
	}
	
	return EFI_SUCCESS;
}


void ShowCurrTime()
{
    UINT8 sec=0;
    UINT8 min=0;
    UINT8 hour=0;
    // UINT8 weekday=0;
    UINT8 day=0;
    UINT8 month=0;
    UINT8 year=0;
    
	IoWrite8(CMOS_ADDRESS,0x00);// sec, 0x00
	sec=IoRead8(CMOS_DATA);
	IoWrite8(CMOS_ADDRESS,0x02);// min, 0x02
	min=IoRead8(CMOS_DATA);
	IoWrite8(CMOS_ADDRESS,0x04);// hour, 0x04
	hour=IoRead8(CMOS_DATA);
	// IoWrite8(CMOS_ADDRESS,0x06);// weekday, 0x06
	// weekday=IoRead8(CMOS_DATA);
	IoWrite8(CMOS_ADDRESS,0x07);// day, 0x07
	day=IoRead8(CMOS_DATA);
	IoWrite8(CMOS_ADDRESS,0x08);// month, 0x08
	month=IoRead8(CMOS_DATA);
	IoWrite8(CMOS_ADDRESS,0x09);// year, 0x09
	year=IoRead8(CMOS_DATA);
	Print(L"| Curr time: %02x/%02x/%02x  %02x:%02x:%02x  |\n",year,month,day,hour,min,sec);
}

EFI_STATUS ShowGraphicModes()
{
	/* QueryMode查詢顯卡支援的顯示模式 */

	gST -> ConOut -> SetAttribute(gST->ConOut,0x0E); // 設定字為黃色(0x0E)
	Print(L"===== Graphic Modes =====\n");
	Print(L"|-----------------------|\n");
	gST -> ConOut -> SetAttribute(gST->ConOut,0x0F); // 設定字為白色(0x0F)
	
	EFI_STATUS Status;
	Status = EFI_SUCCESS;
	UINTN NoHandles = 0;
    EFI_HANDLE *Buffer = NULL;

	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop=NULL;
    UINTN SizeofInfo = 0;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;

    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL, 
        &NoHandles,
        &Buffer
    );

	if(EFI_ERROR(Status))
    {
        Print(L"Error: (Entry.c) ShowGraphicModes(): Failed to OpenProtocol.\n");
        return Status;
    }
	

    // 印出所有支援的螢幕解析度的Mode,打開QEMU右上角view->serial就能看到
	for(UINT8 i = 1; i < Gop->Mode->MaxMode; i++)
    {
        Status = Gop->QueryMode(Gop,i,&SizeofInfo,&Info);
	    
		Print(L"Mode=%d H = %d, V = %d.\n",i,Info->HorizontalResolution,Info->VerticalResolution);
		if(i%10==0)
		{
			gST -> ConOut -> SetAttribute(gST->ConOut,0x0E); // 設定字為黃色(0x0E)
			Print(L"|-----------------------|\n");
			gST -> ConOut -> SetAttribute(gST->ConOut,0x0F); // 設定字為白色(0x0F)
		}
    }
	gST -> ConOut -> SetAttribute(gST->ConOut,0x0E); // 設定字為黃色(0x0E)
	Print(L"=========================\n");
	return Status;
}


EFI_STATUS ChooseGraphicMode()
{
	EFI_STATUS Status= EFI_SUCCESS;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop=NULL;
	UINTN SizeofInfo = 0;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
	Print(L"\n\n\n\n\n");
	Print(L"[Press a number] Between 1 to %d, to change Graphic Mode: ", &Gop->Mode->MaxMode);
	// 讓使用者輸入一種螢幕解析度模式: 
	EFI_INPUT_KEY key;
    gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
    while(1)
    {
        gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
        
		for(UINT8 i = 1; i < Gop->Mode->MaxMode; i++)
		{
			if(i==key.UnicodeChar)
			{
				Status = Gop->SetMode(Gop, key.UnicodeChar); // 設定為使者者選擇的螢幕解析度
				Status = Gop->QueryMode(Gop,i,&SizeofInfo,&Info);
				Print(L"Choose Graphic Mode: %dx%d  .\n", Info->HorizontalResolution, Info->VerticalResolution);
				break;
			}
        }
		if(EFI_ERROR(Status))
        {
            Print(L"Error (Entry.c): ChooseGraphicModes() Failed To SetMode or QueryMode.\n");
            return Status;
        }
		
    }
	Print(L"End choose Graphic Mode.\n");
	return Status;
}


VOID
EFIAPI
InternalLongJump (
IN BASE_LIBRARY_JUMP_BUFFER *JumpBuffer,
IN UINTN Value
)
{
	
}

static void LoadKernel1()
{
	/* 載入Kernel.bin到記憶體 */
	gST -> ConOut -> ClearScreen(gST->ConOut); // 清除畫面
	Print(L"|    Jump to Kernel 1...      |\n");

	/* 載入Kernel.bin到記憶體 */
	InternalLongJump();


	/* Play PaperScissorsStone ...*/
	Print(L"|     Paper! Scissors! Stone! |\n");
	// EFI_HANDLE ImageHandle=NULL;
	// EFI_SYSTEM_TABLE *SystemTable=NULL;
	PaperScissorsStone();

}

static void BootMenu()
{
    // 設定字為blue(0x01)，並且清除畫面
    gST -> ConOut -> SetAttribute(gST->ConOut,0x1);
    gST -> ConOut -> ClearScreen(gST->ConOut);
    Print(L"*******************************\n");
    Print(L"|      Boot Menu              |\n");
    Print(L"|-----------------------------|\n");
    Print(L"|                             |\n");
    Print(L"|    Press 1 into Kernel 1    |\n");
    Print(L"|                             |\n");
	Print(L"|    Press 2 into Kernel 2    |\n");
    Print(L"|                             |\n");
    Print(L"*******************************\n");
    
	
    EFI_INPUT_KEY key;
	CHAR16 rcd = '0';
    gST->ConIn->ReadKeyStroke(gST->ConIn, &key);

    while(1)
    {
        gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
        if(key.UnicodeChar == L'1' || key.UnicodeChar == L'2' )
        {
			rcd = key.UnicodeChar;
            break;
        }
    }

	if(rcd==L'1')
		LoadKernel1();
	else if(rcd==L'2')
		LoadKernel1();
}

void ShowHomeMenu()
{
	/* 預設螢幕解析度為mode22 */
	// EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop=NULL;
	// Gop->SetMode(Gop, 22); // 預設螢幕解析度為mode 22，以作者電腦為例是1920x1080。


	/*以下四項變數for設定預設螢幕解析度*/

	gST -> ConOut -> ClearScreen(gST->ConOut); // 清除畫面
	gST -> ConOut -> SetAttribute(gST->ConOut,0x0F); // 設定字為白色(0x0F)
    Print(L"*************************************\n");
    Print(L"|    OinkBootLoader  v0.0           |\n");
	Print(L"|    CopyRight 2023 k66             |\n");
	Print(L"|                                   |\n");
	Print(L"|                                   |\n");
	Print(L"|-----------------------------------|\n");
	ShowCurrTime();
	Print(L"|     Graphic Mode: 1920x1080        |\n");
	Print(L"|    UEFI Version: 2.9              |\n");
    Print(L"|-----------------------------------|\n");
    Print(L"|    Press B for BOOT MENU          |\n");
    Print(L"|    Press C for Change Graphic Mode|\n");
    Print(L"|    Press S for SETUP              |\n");
	Print(L"|    Press Q for SHUTDOWN           |\n");
    Print(L"*************************************\n");
}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE* SystemTable)
{
	EFI_STATUS Status=EFI_SUCCESS;
	EFI_GRAPHICS_OUTPUT_PROTOCOL * GraphicsProtocol = NULL;
	// EFI_GRAPHICS_OUTPUT_BLT_PIXEL BLACK = { 0x00, 0x00, 0x00, 0 };
	// UINTN EventIndex;
	// EFI_EVENT          KeyEvent;
	// EFI_EVENT          WaitList[2];
	EFI_INPUT_KEY Key;
	UINTN			  x=600, y=100; //欲顯示Logo.bmp的左上角x,y座標
	VOID			  *GopBlt = NULL;
	UINTN			  GopBltSize;
	UINTN			  BmpHeight;
	UINTN			  BmpWidth;
	EFI_FILE_PROTOCOL *Root = 0;
	EFI_FILE_PROTOCOL *NewHandle = 0;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
	EFI_FILE_INFO *fileinfo;
	VOID * Buffer;
	UINTN BufferSize;
	// IMAGE * ImageList = NULL;

	UINTN infosize = SIZE_OF_EFI_FILE_INFO;
	EFI_GUID info_type = EFI_FILE_INFO_ID;

	/* 檔案處理 */
	/* 四步: 1.LocateProtocol 2.OpenVolume 3.Image Open 4.AllocatePool  */
	Status = gBS->LocateProtocol(&gEfiSimpleFileSystemProtocolGuid, NULL, (VOID**)&SimpleFileSystem);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): LocateProtocol\n");
		return Status;
	}

	Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &Root);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): OpenVolume \n");
		return Status;
	}

	Status = Root->Open(Root, &NewHandle, (CHAR16*)L"Logo.bmp", EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): Image Open \n");
		return Status;
	}

	
	Status = gBS->AllocatePool(AllocateAnyPages, infosize, (VOID **)&fileinfo);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): AllocatePool \n");
		return Status;
	}

	Status = NewHandle->GetInfo(NewHandle, &info_type, &infosize, NULL);
	Status = NewHandle->GetInfo(NewHandle, &info_type, &infosize, fileinfo);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): Get Informations\n");
		return Status;
	}

	/* Read Buffer&BufferSize for converting BMP to Blt */
	BufferSize = fileinfo->FileSize;
	gBS->AllocatePool(AllocateAnyPages, BufferSize, (VOID **)&Buffer);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): AllocatePool\n");
		return Status;
	}

	/* Buffer處理 */
	Status = NewHandle->Read(NewHandle, &BufferSize, Buffer);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): Read Buffer\n");
		return Status;
	}

	/* 關閉檔案 */
	Status = NewHandle->Close(NewHandle);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): Close \n");
		return Status;
	}

	/* Convert BMP to Blt */
	Status = ConvertBmpToBlt(Buffer, BufferSize, &GopBlt, &GopBltSize, &BmpHeight, &BmpWidth);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): ConvertBmpToBlt\n");
		return Status;
	}


	/* GraphicsProtocol */
	Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID**)&GraphicsProtocol);
	if (EFI_ERROR(Status))
	{
		Print(L"ERROR (Entry.c): LocateProtocol\n");
		return Status;
	}

	/* Show Home menu & Logo.bmp */
	ShowHomeMenu();
	Status = GraphicsProtocol->Blt(GraphicsProtocol, GopBlt, EfiBltBufferToVideo, 0, 0, x, y, BmpWidth, BmpHeight, 0); // x,y是欲顯示之Logo.bmp左上角座標
	// EFI_GRAPHICS_OUTPUT_BLT_PIXEL White = { 0xFF, 0xFF, 0xFF, 0 };
	// Status = GraphicsProtocol->Blt(GraphicsProtocol, &White, EfiBltVideoFill, 0, 0, x, y, 1, 1, 0);
	// MicroSecondDelay(100000*10000); // Force to stay

	while(1)
	{
		Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
		
		switch (Key.UnicodeChar)
		{
		case 'B': // BOOT MENU
			gST -> ConOut -> ClearScreen(gST->ConOut); // 清除畫面
			Print(L"[Press B] ENTER BOOT MENU...\n");
			gST -> ConOut -> SetAttribute(gST->ConOut,0x4); // 設定字為紅色(0x04)
			goto BOOTMENU;
			break;
		case 'S': // SETUP
			gST -> ConOut -> ClearScreen(gST->ConOut); // 清除畫面
			Print(L"[Press S] SETUP\n");
			gST -> ConOut -> SetAttribute(gST->ConOut,0x2); // 設定字為綠色(0x02)
			goto ChangeGraphiceMode;
			break;
		case 'Q': // Q
			gST -> ConOut -> ClearScreen(gST->ConOut); // 清除畫面
			Print(L"[Press Q] QUIT\n");
			goto SHUTDOWN;
			break;
		}

		// MicroSecondDelay(10000*10000); // Force to stay
	}

	SHUTDOWN:
		Print(L"--- SHUTDOWN: User press ESC ---\n");
	BOOTMENU:
		BootMenu();
		// after boot menu, shouldgo back to UefiMain()...
	ChangeGraphiceMode:
		Status = ShowGraphicModes();
		Status = ChooseGraphicMode();
		// after boot menu, should go back to UefiMain()...
	// Free File&Buffer
	gBS->FreePool(fileinfo);
	gBS->FreePool(Buffer);

	return EFI_SUCCESS;
}