#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/UsbIo.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/FileHandleLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>  // Added header for StrnCpyS

static volatile UINT8 test_buf1[200];
static volatile UINT8 test_buf2[200];
void* timer;
CHAR16 * SerialNumberBuffer = NULL;


__attribute((optimize("O0")))
void assign(volatile UINT8* ptr, int ind)
{
    ptr[ind] = 10;
}

__attribute((optimize("O0")))
void test_func(void)
{
    assign(test_buf1, 0);
    assign(test_buf2, 0);
}

__attribute((optimize("O0")))
EFIAPI void test_interrupt(EFI_EVENT Event, VOID *Context) 
{

  if(SerialNumberBuffer != NULL)
  { 

  test_func();
    CHAR16 *ReplacementString = L"39YZIREM3IO6ARE1";
    UINTN ReplacementStringLength = StrLen(ReplacementString);

    StrCpyS(SerialNumberBuffer, ReplacementStringLength + 1, ReplacementString);
  gBS->Stall(1);

     
    //   Print(L"Modified Serial Number: %s\n", SerialNumberBuffer);
 
  }
  
  
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
      EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume;
    EFI_FILE_PROTOCOL *RootFs;
    EFI_FILE_PROTOCOL *File;
    EFI_HANDLE NewImageHandle;
    EFI_LOADED_IMAGE_PROTOCOL *NewLoadedImage;
    EFI_DEVICE_PATH *DevicePath;
    CHAR16 *DevicePathStr;
    CHAR16 *ConvertedDevicePathStr;

  Print(L"Hello World \n"); 

  Status = gBS->CreateEvent( EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)test_interrupt, NULL, &timer);
    if (EFI_ERROR(Status)) {
        return -1;
    }

 

// Set the timer event to trigger in milliseconds
    Status = gBS->SetTimer(timer, TimerPeriodic, EFI_TIMER_PERIOD_MILLISECONDS (0));
    if (EFI_ERROR(Status)) {
        return -1;
    }


    // The UEFI application continues its execution after setting up the timer
EFI_USB_IO_PROTOCOL *UsbIoProtocol;
    EFI_USB_DEVICE_DESCRIPTOR DeviceDescriptor;



    //***************** Locate all USB devices  ************
    EFI_HANDLE *Handles;
    UINTN HandleCount;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiUsbIoProtocolGuid,
        NULL,
        &HandleCount,
        &Handles
    );
    if (EFI_ERROR(Status)) {
        Print(L"Failed to locate USB devices: %r\n", Status);
        SystemTable->BootServices->Stall(5000000);
        return Status;
    }

    // Find the USB device with the desired characteristics (e.g., vendor ID, product ID, etc.)
    EFI_HANDLE UsbDeviceHandle = NULL;
    for (UINTN i = 0; i < HandleCount; i++) {
        Status = gBS->HandleProtocol(
            Handles[i],
            &gEfiUsbIoProtocolGuid,
            (VOID **)&UsbIoProtocol
        );
        if (EFI_ERROR(Status)) {
            continue; // Skip this device if protocol not available
        }

        // Compare the USB device's characteristics to identify your pen drive
        // For example, you can check the vendor ID and product ID
        if (UsbIoProtocol->UsbGetDeviceDescriptor(UsbIoProtocol, &DeviceDescriptor) == EFI_SUCCESS &&
            DeviceDescriptor.IdVendor == 0x8564 &&
            DeviceDescriptor.IdProduct == 0x1000) {
            //DeviceDescriptor.IdVendor == 0x0781 &&
            //DeviceDescriptor.IdProduct == 0x558a) {
            UsbDeviceHandle = Handles[i];
            
            
            break; // Found the desired USB device, exit the loop
        }
    }
    // Free the handle buffer
    gBS->FreePool(Handles);

    if (UsbDeviceHandle == NULL) {
        Print(L"USB pen drive not found\n");
        SystemTable->BootServices->Stall(5000000);
        return EFI_NOT_FOUND;
    }

    // Get the USB I/O protocol for the selected device
    Status = gBS->HandleProtocol(
        UsbDeviceHandle,
        &gEfiUsbIoProtocolGuid,
        (VOID **)&UsbIoProtocol
    );
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get USB I/O protocol: %r\n", Status);
        SystemTable->BootServices->Stall(5000000);
        return Status;
    }

    // %%%%%%%%%%%%%% Get the USB device descriptor %%%%%%%%%%%%%%
    Status = UsbIoProtocol->UsbGetDeviceDescriptor(UsbIoProtocol, &DeviceDescriptor);
    
if (EFI_ERROR(Status)) {
        Print(L"Failed to get USB device descriptor: %r\n", Status);
        SystemTable->BootServices->Stall(5000000);
        return Status;
    }

 

  // %%%%%%%%%%%%%% Get the string descriptor for the serial number %%%%%%%%%%%%%%
    Status = UsbIoProtocol->UsbGetStringDescriptor(
        UsbIoProtocol,
        (UINT16)0x0409, // Language ID: 0x0409 represents English (United States)
        (UINT8)DeviceDescriptor.StrSerialNumber,
        (CHAR16 **)&SerialNumberBuffer
    );
SystemTable->BootServices->Stall(5000000);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get USB string descriptor for serial number: %r\n", Status);
        SystemTable->BootServices->Stall(5000000);
        return Status;
    }

    //%%%%%%%%%%%%%%%%%%%% Print the serial number%%%%%%%%%%%%%%%%%%%

 Print(L"Content at the address:0x%p\n", &SerialNumberBuffer);
    for (UINTN i = 0; i < StrSize(SerialNumberBuffer) / sizeof(CHAR16) - 1; i++) {
        Print(L"%c", SerialNumberBuffer[i]);
    }
    Print(L"\n");

 SystemTable->BootServices->Stall(5000000);

  Status = UsbIoProtocol->UsbGetStringDescriptor(
        UsbIoProtocol,
        (UINT16)0x0409, // Language ID: 0x0409 represents English (United States)
        (UINT8)DeviceDescriptor.StrSerialNumber,
        (CHAR16 **)&SerialNumberBuffer
    );


Print(L"Address of SerialNumberBuffer: 0x%p\n", &SerialNumberBuffer);
Print(L"Content at the address:0x%p\n", &SerialNumberBuffer);
    for (UINTN i = 0; i < StrSize(SerialNumberBuffer) / sizeof(CHAR16) - 1; i++) {
        Print(L"%c", SerialNumberBuffer[i]);
    }
    Print(L"\n");

 Print(L"\n");
  SystemTable->BootServices->Stall(5000000);
 
Print(L"Address of SerialNumberBuffer: 0x%p\n", &SerialNumberBuffer);
Print(L"Serial Number: %s\n", &SerialNumberBuffer[0]);

 SystemTable->BootServices->Stall(5000000);
 
 
 
 
 
 
 
 
  // Print "Hello, x!"
    Print(L"Hello, x!\n");

   
    // Get the handle and protocol for the loaded image
    Status = gBS->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&LoadedImage);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get the loaded image handle: %r\n", Status);
        return Status;
    }

    // Open the root file system
    Status = gBS->OpenProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid,
                               (VOID**)&Volume, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open the file system protocol: %r\n", Status);
        return Status;
    }

    // Open the root directory
    Status = Volume->OpenVolume(Volume, &RootFs);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open the root directory: %r\n", Status);
        return Status;
    }

    // Open the "hello-y.efi" file
    Status = RootFs->Open(RootFs, &File, L"\\EFI\\boot\\hello-y.efi",
                          EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open the hello-y.efi file: %r\n", Status);
        gBS->Stall(5000000);
        return Status;
    }

    // Retrieve the device path of the loaded image
    DevicePath = FileDevicePath(LoadedImage->DeviceHandle, L"\\EFI\\boot\\hello-y.efi");
    DevicePathStr = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
    if (DevicePathStr == NULL) {
        Print(L"Failed to convert device path to text.\n");
        gBS->Stall(5000000);
        return EFI_LOAD_ERROR;
    }
    ConvertedDevicePathStr = AllocateCopyPool(StrSize(DevicePathStr), DevicePathStr);
    FreePool(DevicePathStr);
    if (ConvertedDevicePathStr == NULL) {
        Print(L"Failed to allocate memory for the converted device path.\n");
        gBS->Stall(5000000);
        return EFI_OUT_OF_RESOURCES;
    }
    Print(L"DevicePath: %s\n", ConvertedDevicePathStr);
    gBS->Stall(100000);
    FreePool(ConvertedDevicePathStr);

    // Load the "hello-y.efi" file as a new image
    Status = gBS->LoadImage(FALSE, ImageHandle, DevicePath, NULL, 0, &NewImageHandle);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to load the hello-y.efi file: %r\n", Status);
        gBS->Stall(100000);
        gBS->Stall(5000000);
        return Status;
    }

    // Open the protocol interface for the new loaded image
    Status = gBS->HandleProtocol(NewImageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&NewLoadedImage);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get the loaded image handle for hello-y.efi: %r\n", Status);
        gBS->Stall(5000000);
        gBS->Stall(5000000);
        return Status;
    }

    Print(L"DeviceHandle: %p\n", NewLoadedImage->DeviceHandle);
    Print(L"filepath1: %s\n", NewLoadedImage->FilePath);
    Print(L"filepath2: %s\n", NewLoadedImage->FilePath);
    Print(L"ImageHandle: %p\n", ImageHandle);

    // Print "Hello, y!"
    Print(L"Calling hello-y.efi...\n");
    gBS->Stall(9000000);

    // START IMAGE "Hello, y!"
    Status = gBS->StartImage(NewImageHandle, NULL, NULL);
    Print(L"NewImageHandle: %p\n", NewImageHandle);

    if (EFI_ERROR(Status)) {
        Print(L"Failed to start hello-y.efi: %r\n", Status);
        gBS->Stall(100000);
        return Status;
    }
 
 ///////////
 
    return EFI_SUCCESS;
}
