EFI_STATUS ConvertBmpToBlt(VOID *BmpImage, UINTN BmpImageSize, VOID **GopBlt, UINTN *GopBltSize, UINTN *PixelHeight, UINTN *PixelWidth)
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
    
    /* Bmp僅支援24位元或32位元，非者即非Bmp */
	if (BmpHeader->BitPerPixel != 24 && BmpHeader->BitPerPixel != 32)
		return EFI_UNSUPPORTED;

	BltBufferSize = BmpHeader->PixelWidth * BmpHeader->PixelHeight * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
	*GopBltSize = (UINTN)BltBufferSize;
	*GopBlt = AllocatePool(*GopBltSize);   
	
	*PixelWidth = BmpHeader->PixelWidth;
	*PixelHeight = BmpHeader->PixelHeight;

	ImageData = ImageBegin;
	BltBuffer = *GopBlt;
	for (Height = 0; Height < BmpHeader->PixelHeight; Height++)
	{
        // Buffer大小=長x寬
		Blt = &BltBuffer[(BmpHeader->PixelHeight-Height-1) * BmpHeader->PixelWidth];
		for (Width = 0; Width < BmpHeader->PixelWidth; Width++, Blt++)
		{
            Blt->Blue = *ImageData++;
			Blt->Green = *ImageData++;
			Blt->Red = *ImageData++;
			/* 僅會有24位元或32位元，
            因為非者已在前面被return，若32位元則ImageData++。
            */
			switch (BmpHeader->BitPerPixel)
			{
			case 24:
				break;
			case 32:
				ImageData++;
				break;
			default:
				break;
			}
		}

		ImageIndex = (UINTN)(ImageData - ImageBegin);
        /* 除以4後，將商+餘數 */
		if ((ImageIndex % 4) != 0)
			ImageData = ImageData + (4 - (ImageIndex % 4));
	}
	
	return EFI_SUCCESS;
}    return Status;

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

    return Status;
}