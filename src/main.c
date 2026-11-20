#include <stdint.h>
#include <stddef.h>


typedef uint64_t EFI_STATUS;
typedef uint16_t CHAR16;
typedef void *EFI_HANDLE;

#define EFI_SUCCESS 0


typedef struct {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
} EFI_GUID;



typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {

    void *Reset;

    EFI_STATUS (*OutputString)(
        EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
        CHAR16 *String
    );
};



typedef struct EFI_FILE_PROTOCOL EFI_FILE_PROTOCOL;

struct EFI_FILE_PROTOCOL {

    uint64_t Revision;

    EFI_STATUS (*Open)(
        EFI_FILE_PROTOCOL *This,
        EFI_FILE_PROTOCOL **NewHandle,
        CHAR16 *FileName,
        uint64_t OpenMode,
        uint64_t Attributes
    );

    EFI_STATUS (*Close)(
        EFI_FILE_PROTOCOL *This
    );

    void *Delete;

    EFI_STATUS (*Read)(
        EFI_FILE_PROTOCOL *This,
        uint64_t *BufferSize,
        void *Buffer
    );

    void *Write;
    void *GetPosition;
    void *SetPosition;
    void *GetInfo;
    void *SetInfo;
    void *Flush;
};



typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {

    uint64_t Revision;

    EFI_STATUS (*OpenVolume)(
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This,
        EFI_FILE_PROTOCOL **Root
    );
};



typedef EFI_STATUS (*EFI_LOCATE_PROTOCOL)(
    EFI_GUID *Protocol,
    void *Registration,
    void **Interface
);

typedef EFI_STATUS (*EFI_HANDLE_PROTOCOL)(
    EFI_HANDLE Handle,
    EFI_GUID *Protocol,
    void **Interface
);

typedef struct {

    uint8_t Header[24];

    void *FunctionsBeforeHandle[16];

    EFI_HANDLE_PROTOCOL HandleProtocol;

    void *FunctionsBetween[20];

    EFI_LOCATE_PROTOCOL LocateProtocol;

} EFI_BOOT_SERVICES;



typedef struct {

    uint8_t Header[24];

    void *FirmwareVendor;
    uint32_t FirmwareRevision;
    uint32_t Padding;

    void *ConsoleInHandle;
    void *ConIn;

    void *ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;

    void *StandardErrorHandle;
    void *StdErr;

    void *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;

    uint64_t NumberOfTableEntries;
    void *ConfigurationTable;

} EFI_SYSTEM_TABLE;



typedef struct {

    uint32_t Version;

    uint32_t HorizontalResolution;
    uint32_t VerticalResolution;

    uint32_t PixelFormat;

    uint32_t PixelInformation[4];

    uint32_t PixelsPerScanLine;

} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;



typedef struct {

    uint32_t MaxMode;
    uint32_t Mode;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;

    uint64_t SizeOfInfo;

    uint64_t FrameBufferBase;
    uint64_t FrameBufferSize;

} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;



typedef struct {

    void *QueryMode;
    void *SetMode;
    void *Blt;

    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;

} EFI_GRAPHICS_OUTPUT_PROTOCOL;



static EFI_GUID GOP_GUID = {
    0x9042a9de,
    0x23dc,
    0x4a38,
    {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}
};

static EFI_GUID SIMPLE_FILE_SYSTEM_GUID = {
    0x964e5b22,
    0x6459,
    0x11d2,
    {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}
};

static EFI_GUID LOADED_IMAGE_GUID = {
    0x5b1b31a1,
    0x9562,
    0x11d2,
    {0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}
};



typedef struct {

    uint32_t Revision;

    EFI_HANDLE ParentHandle;
    void *SystemTable;

    EFI_HANDLE DeviceHandle;

    void *FilePath;
    void *Reserved;

    uint32_t LoadOptionsSize;
    void *LoadOptions;

    void *ImageBase;
    uint64_t ImageSize;

    uint32_t ImageCodeType;
    uint32_t ImageDataType;

} EFI_LOADED_IMAGE_PROTOCOL;



void PrintNumber(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut,
    uint32_t number
) {

    CHAR16 buffer[12];

    int i = 0;

    do {

        buffer[i++] = '0' + (number % 10);

        number /= 10;

    } while (number > 0);

    buffer[i] = 0;

    CHAR16 reversed[12];

    for (int j = 0; j < i; j++) {

        reversed[j] = buffer[i - j - 1];
    }

    reversed[i] = 0;

    ConOut->OutputString(ConOut, reversed);
}



void DrawRectangle(
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP,
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
    uint32_t color
) {

    volatile uint32_t *FrameBuffer =
        (volatile uint32_t *)(uintptr_t)
        GOP->Mode->FrameBufferBase;

    uint32_t screenWidth =
        GOP->Mode->Info->HorizontalResolution;

    uint32_t screenHeight =
        GOP->Mode->Info->VerticalResolution;

    uint32_t pitch =
        GOP->Mode->Info->PixelsPerScanLine;

    for (uint32_t j = 0; j < height; j++) {

        for (uint32_t i = 0; i < width; i++) {

            uint32_t pixelX = x + i;
            uint32_t pixelY = y + j;

            if (pixelX < screenWidth &&
                pixelY < screenHeight) {

                uint32_t offset =
                    pixelY * pitch + pixelX;

                FrameBuffer[offset] = color;
            }
        }
    }
}


static uint16_t ReadLE16(const uint8_t *p)
{
    return (uint16_t)p[0] |
           ((uint16_t)p[1] << 8);
}

static uint32_t ReadLE32(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void DrawBMP(
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP,
    const uint8_t *bmp,
    uint32_t pixelOffset,
    uint32_t imageWidth,
    uint32_t imageHeight,
    uint32_t rowStride,
    int topDown
)
{
    uint32_t screenWidth = GOP->Mode->Info->HorizontalResolution;
    uint32_t screenHeight = GOP->Mode->Info->VerticalResolution;
    uint32_t pitch = GOP->Mode->Info->PixelsPerScanLine;
    uint32_t pixelFormat = GOP->Mode->Info->PixelFormat;

    if (pixelFormat != 0 && pixelFormat != 1) {
        return;
    }

    volatile uint32_t *frameBuffer =
        (volatile uint32_t *)(uintptr_t)GOP->Mode->FrameBufferBase;

    for (uint32_t y = 0; y < imageHeight; y++) {
        uint32_t sourceY = topDown ? y : imageHeight - 1 - y;

        for (uint32_t x = 0; x < imageWidth; x++) {
            if (x >= screenWidth || y >= screenHeight) {
                continue;
            }

            uint64_t sourceIndex = (uint64_t)pixelOffset +
                                   (uint64_t)sourceY * rowStride +
                                   (uint64_t)x * 3;

            uint8_t blue = bmp[sourceIndex];
            uint8_t green = bmp[sourceIndex + 1];
            uint8_t red = bmp[sourceIndex + 2];

            uint32_t color;
            if (pixelFormat == 1) {
                color = ((uint32_t)red << 16) |
                        ((uint32_t)green << 8) | blue;
            } else {
                color = ((uint32_t)blue << 16) |
                        ((uint32_t)green << 8) | red;
            }

            frameBuffer[(uint64_t)y * pitch + x] = color;
        }
    }
}


EFI_STATUS ReadBMP(
    EFI_FILE_PROTOCOL *Root,
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut,
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP
) {

    EFI_FILE_PROTOCOL *File = 0;

    EFI_STATUS status;


    status = Root->Open(
        Root,
        &File,
        (CHAR16 *)u"images\\test.bmp",
        1,
        0
    );

    if (status != EFI_SUCCESS) {

        ConOut->OutputString(
            ConOut,
            u"\r\nBMP file could not be opened!\r\n"
        );

        return status;
    }

    ConOut->OutputString(
        ConOut,
        u"\r\nBMP file opened successfully!\r\n"
    );



    static uint8_t bmpBuffer[1024 * 1024];

    uint64_t bufferSize = sizeof(bmpBuffer);



    status = File->Read(
        File,
        &bufferSize,
        bmpBuffer
    );

    if (status != EFI_SUCCESS) {

        ConOut->OutputString(
            ConOut,
            u"BMP read failed!\r\n"
        );

        File->Close(File);

        return status;
    }


    ConOut->OutputString(
        ConOut,
        u"BMP read successfully!\r\nFile size: "
    );

    PrintNumber(
        ConOut,
        (uint32_t)bufferSize
    );



    if (bufferSize < 54 ||
        bmpBuffer[0] != 'B' ||
        bmpBuffer[1] != 'M') {

        ConOut->OutputString(
            ConOut,
            u"\r\nInvalid BMP file!\r\n"
        );

        File->Close(File);

        return 1;
    }

    uint32_t pixelOffset = ReadLE32(&bmpBuffer[10]);
    int32_t signedWidth = (int32_t)ReadLE32(&bmpBuffer[18]);
    int32_t signedHeight = (int32_t)ReadLE32(&bmpBuffer[22]);
    uint16_t planes = ReadLE16(&bmpBuffer[26]);
    uint16_t bitsPerPixel = ReadLE16(&bmpBuffer[28]);
    uint32_t compression = ReadLE32(&bmpBuffer[30]);

    if (signedWidth <= 0 || signedHeight == 0 ||
        signedHeight == INT32_MIN || planes != 1 ||
        bitsPerPixel != 24 || compression != 0) {
        ConOut->OutputString(ConOut, u"Unsupported BMP format!\r\n");
        File->Close(File);
        return 1;
    }

    uint32_t imageWidth = (uint32_t)signedWidth;
    uint32_t imageHeight = (uint32_t)(
        signedHeight < 0 ? -signedHeight : signedHeight
    );

    uint64_t rowStride64 = (((uint64_t)imageWidth * 3) + 3) & ~((uint64_t)3);
    uint64_t requiredSize = (uint64_t)pixelOffset + rowStride64 * imageHeight;

    if (rowStride64 > UINT32_MAX || pixelOffset > bufferSize ||
        requiredSize > bufferSize) {
        ConOut->OutputString(ConOut, u"BMP pixel data is incomplete!\r\n");
        File->Close(File);
        return 1;
    }
    uint32_t rowStride = (uint32_t)rowStride64;


    ConOut->OutputString(
        ConOut,
        u"\r\nImage Width: "
    );

    PrintNumber(
        ConOut,
        imageWidth
    );


    ConOut->OutputString(
        ConOut,
        u"\r\nImage Height: "
    );

    PrintNumber(
        ConOut,
        imageHeight
    );


    ConOut->OutputString(
        ConOut,
        u"\r\nBits Per Pixel: "
    );

    PrintNumber(
        ConOut,
        bitsPerPixel
    );

    ConOut->OutputString(
        ConOut,
        u"\r\n"
    );


    DrawBMP(
        GOP,
        bmpBuffer,
        pixelOffset,
        imageWidth,
        imageHeight,
        rowStride,
        signedHeight < 0
    );


    File->Close(File);

    return EFI_SUCCESS;
}



EFI_STATUS efi_main(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
) {

    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut =
        SystemTable->ConOut;

    ConOut->OutputString(
        ConOut,
        u"Hello from Bare Metal!\r\n"
    );



    EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP = 0;

    EFI_STATUS status =
        SystemTable->BootServices->LocateProtocol(
            &GOP_GUID,
            0,
            (void **)&GOP
        );

    if (status != EFI_SUCCESS || GOP == 0) {

        ConOut->OutputString(
            ConOut,
            u"GOP not found!\r\n"
        );

        while (1) {
            __asm__ volatile("hlt");
        }
    }

    ConOut->OutputString(
        ConOut,
        u"GOP successfully found!\r\n"
    );



    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage = 0;

    status =
        SystemTable->BootServices->HandleProtocol(
            ImageHandle,
            &LOADED_IMAGE_GUID,
            (void **)&LoadedImage
        );

    if (status != EFI_SUCCESS || LoadedImage == 0) {

        ConOut->OutputString(
            ConOut,
            u"Loaded Image not found!\r\n"
        );

        while (1) {
            __asm__ volatile("hlt");
        }
    }



    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = 0;

    status =
        SystemTable->BootServices->HandleProtocol(
            LoadedImage->DeviceHandle,
            &SIMPLE_FILE_SYSTEM_GUID,
            (void **)&FileSystem
        );

    if (status != EFI_SUCCESS || FileSystem == 0) {

        ConOut->OutputString(
            ConOut,
            u"File System not found!\r\n"
        );

        while (1) {
            __asm__ volatile("hlt");
        }
    }



    EFI_FILE_PROTOCOL *Root = 0;

    status = FileSystem->OpenVolume(
        FileSystem,
        &Root
    );

    if (status != EFI_SUCCESS || Root == 0) {

        ConOut->OutputString(
            ConOut,
            u"ESP could not be opened!\r\n"
        );

        while (1) {
            __asm__ volatile("hlt");
        }
    }

    ConOut->OutputString(
        ConOut,
        u"ESP opened successfully!\r\n"
    );


    /* BMP dosyasını oku */

    ReadBMP(
        Root,
        ConOut,
        GOP
    );



    while (1) {
        __asm__ volatile("hlt");
    }

    return EFI_SUCCESS;
}
