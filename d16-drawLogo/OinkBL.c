/* BmpTransform: */
EFI_STATUS BmpTransform(
    
    EFI_STATUS Status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL * GraphicsProtocol = NULL;
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

	/* Show Logo.bmp */
	Status = GraphicsProtocol->Blt(GraphicsProtocol, GopBlt, EfiBltBufferToVideo, 0, 0, x, y, BmpWidth, BmpHeight, 0); // x,y是欲顯示之Logo.bmp左上角座標
    
    return Status;
}

EFI_STATUS GetFileHandle(
    IN EFI_HANDLE ImageHandle,
    IN CHAR16 *FileName,
    OUT EFI_FILE_PROTOCOL **FileHandle
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandleCount = 0;
    EFI_HANDLE *HandleBuffer;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
    );
    

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;
    Status = gBS->OpenProtocol(
        HandleBuffer[0],
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&SFSP,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    
    EFI_FILE_PROTOCOL *Root;
    Status = SFSP->OpenVolume(
        SFSP,
        &Root
    );

    Status = Root->Open(
        Root,
        FileHandle,
        FileName, 
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );

    return Status;
}

EFI_STATUS
EFIAPI
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;
    /* GetGopHandle 前一篇有提 */
    /* Video Init 前一篇有提 */
    /* Set Video 前一篇有提 */
    /* 前一篇有提找到1920x1080剛好是mode序號22，故將22設定為我們的螢幕解析度*/
    Status = Gop->SetMode(Gop, 22); /* 設定螢幕解析度為1920x1080(模式22)，其實不應寫死為22，此處為講解暫寫如此，請見諒*/
    
    /* Draw Logo */
    CHAR16 *FileName = L"\\Logo.bmp"; 
    /* 呼叫GetFileHandle()，執行三步: LocateHandleBuffer, OpenProtocol, OpenVolume */
    EFI_FILE_PROTOCOL *Logo;
    Status = GetFileHandle(ImageHandle, FileName, &Logo);
    /* 呼叫ReadFile()，執行三步: AllocatePool, GetInfo, AllocatePages, FreePool*/
    EFI_PHYSICAL_ADDRESS LogoAddress;
    Status = ReadFile(Logo, &LogoAddress);

    BMP_CONFIG BmpConfig;
    Status = BmpTransform(LogoAddress, &BmpConfig);
    Status = DrawBmp(Gop, BmpConfig, 0, 0);
    
    return Status;
}