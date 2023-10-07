#include<Entry.h>

void Paper()
{

Print(L"               _\n");
Print(L"           _  | |  _\n");
Print(L"          | | | | | |\n");
Print(L"    ___   | | | | | |   __\n");
Print(L"    \\  \\  | | | | | |  / / \n");
Print(L"     \\  \\_| |_| |_| |_/ / \n");
Print(L"      \\                 |\n");
Print(L"       \\                /   \n");
Print(L"        \\______________/\n");
}

void Scissors()
{
Print(L"      ___       ___ \n");
Print(L"     /    \\    /   \\\n");
Print(L"    /      \\__/     \\\n");
Print(L"    |                |\n");
Print(L"    |                |\n");
Print(L"     \\____      ____/\n");
Print(L"          |    |\n");
// Print(L"          |____|\n");
}


void Stone()
{

Print(L"        _________________\n");
Print(L"       /  |  |   |   |   \\\n");
Print(L"      /   |  |   |   |    \\\n");
Print(L"     |                     |\n");
Print(L"     |                     |\n");
Print(L"      \\                   /\n");
Print(L"       \\_________________/\n");
Print(L"\n");

}


CHAR16 RANDOM_CHOOSE(UINT8 dirty)
{
  IoWrite8(CMOS_ADDRESS,0x00);
  UINT8 randNo = ( dirty+IoRead8(CMOS_DATA) )%3;
  if(randNo==0)
    return 'P';
  else if(randNo==1)
    return 'S';
  else
    return 'O';
}


EFI_STATUS EFIAPI PaperScissorsStone()
{
  // Initialize the screen
  EFI_STATUS Status;
  Status = EFI_SUCCESS;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop=NULL;
  Status = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (VOID**) &Gop);
  if (EFI_ERROR(Status)) {
    Print(L"Unable to locate Graphics Output Protocol\n");
    Print(L"Press any key to exit...\n");
    UINTN EventIndex;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
    Exit(1);
  }
  
  Print(L"P(Paper), S(Scissors), O(Stones)\n");
  Print(L"++++++++++++++++++++++++++++++++\n");
  
  CHAR16 enemyKey = 'P';
  
  //Main loop
  BOOLEAN Stop = FALSE;
  UINT8 cnt = 0;
  UINT8 youWin = 0;
  UINT8 enemyWin = 0;
  EFI_INPUT_KEY playerKey;
  Print(L"Press ESC to leave the game...\n");
  while(!Stop){
    //Check for keys event
    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &playerKey);
    while(1)
    {
      Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &playerKey);
      if(playerKey.ScanCode==SCAN_ESC)
      {
        Stop=TRUE;
        break;
      }
      if( Status==EFI_SUCCESS )
      {

        /* enemy 隨機選一種:剪刀/石頭/布 */
        cnt+=1;
        enemyKey = RANDOM_CHOOSE(cnt);
        
        gST -> ConOut -> SetAttribute(gST->ConOut,0x0); // 白色 
        
        /* Player green */
        gST -> ConOut -> SetAttribute(gST->ConOut,0x2); 
        Print(L"(You) \n");
        switch(playerKey.UnicodeChar)
        {
            case 'P':
              Paper();
            case 'S':
              Scissors();
            case 'O':
              Stone();
        }

        /* Enemy Red */
        gST -> ConOut -> SetAttribute(gST->ConOut,0x4);
        Print(L"(Enemy) \n");
        switch(enemyKey)
        {
            case 'P':
              Paper();
            case 'S':
              Scissors();
            case 'O':
              Stone();
        }

        // 猜拳
        Print(L"The %d game result: \n", cnt);
        gST -> ConOut -> SetAttribute(gST->ConOut,0x0); // 白色 
        if( playerKey.UnicodeChar == enemyKey )
            Print(L"Tie\n");
        
        else if (  (playerKey.UnicodeChar=='S'&&enemyKey=='P')  ||
                   (playerKey.UnicodeChar=='O'&&enemyKey=='S') ||
                   (playerKey.UnicodeChar=='P'&&enemyKey=='O')
                ){
            Print(L"You Win\n");
            youWin+=1;
        }

        else{
            Print(L"You Lose\n");
            enemyWin+=1;
        }


        Print(L"Win %d times\n",youWin);
        Print(L"Lose %d times\n",enemyWin);
        Print(L"Tie %d times\n",cnt-youWin-enemyWin);
        Print("==========================\n");
        Print("==========================\n");


        if(cnt%3==0)
          gST -> ConOut -> SetAttribute(gST->ConOut,0x1);//每玩3局完畢，需清空螢幕
      } // if Status success
    }//while
    
  }

  Print(L"Game over !\n ctrl+C to exit...\n");
  
  return EFI_SUCCESS;
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status = EFI_SUCCESS;
    PaperScissorsStone();
    return Status;
}