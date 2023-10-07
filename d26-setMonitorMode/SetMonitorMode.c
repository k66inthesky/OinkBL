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


EFI_STATUS EFIAPI UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status = EFI_SUCCESS;
    while(1)
	{
		Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
		
		switch (Key.UnicodeChar)
		{
		case 'B': // BOOT MENU
			省略;
		case 'S': // SETUP
			gST -> ConOut -> ClearScreen(gST->ConOut); // 清除畫面
			Print(L"[Press S] SETUP\n");
			gST -> ConOut -> SetAttribute(gST->ConOut,0x2); // 設定字為綠色(0x02)
			goto ChangeGraphiceMode;
			break;
		case 'Q': // Q
			省略;
		}

		// MicroSecondDelay(10000*10000); // Force to stay
	}

	
	ChangeGraphiceMode:
		Status = ShowGraphicModes();
		Status = ChooseGraphicMode();

    return Status;
}