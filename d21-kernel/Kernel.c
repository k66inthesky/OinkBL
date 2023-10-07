EFI_STATUS GetFileHandle(
    IN EFI_HANDLE ImageHandle,
    IN CHAR16 *FileName,
    OUT EFI_FILE_PROTOCOL **FileHandle
);

EFI_STATUS ReadFile(
    IN EFI_FILE_PROTOCOL *File,
    OUT EFI_PHYSICAL_ADDRESS *FileBase
);

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_FILE_PROTOCOL *bin;
    Status = GetFileHandle(ImageHandle, L"Kernel.bin",&bin);
    EFI_PHYSICAL_ADDRESS binAddr;
    Status = ReadFile(bin, &binAddr);
    asm("jmp %0"::"m"(binAddr));

    return Status;
}