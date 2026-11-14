#include <stdint.h>
#include <stddef.h>

/* Temel UEFI veri tipleri */

typedef uint64_t EFI_STATUS;
typedef uint16_t CHAR16;
typedef void *EFI_HANDLE;

#define EFI_SUCCESS 0

/* GUID */

typedef struct {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
} EFI_GUID;


/* Text Output Protocol */

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {

    void *Reset;

    EFI_STATUS (*OutputString)(
        EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
        CHAR16 *String
    );
};


/* File Protocol */

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


/* Simple File System Protocol */

typedef struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

struct EFI_SIMPLE_FILE_SYSTEM_PROTOCOL {

    uint64_t Revision;

    EFI_STATUS (*OpenVolume)(
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This,
        EFI_FILE_PROTOCOL **Root
    );
};


/* Boot Services */

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


/* System Table */

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


/* GOP Mode Information */

typedef struct {

    uint32_t Version;

    uint32_t HorizontalResolution;
    uint32_t VerticalResolution;

    uint32_t PixelFormat;

    uint32_t PixelInformation[4];

    uint32_t PixelsPerScanLine;

} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;


/* GOP Mode */

typedef struct {

    uint32_t MaxMode;
    uint32_t Mode;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;

    uint64_t SizeOfInfo;

    uint64_t FrameBufferBase;
    uint64_t FrameBufferSize;

} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;


/* GOP Protocol */

typedef struct {

    void *QueryMode;
    void *SetMode;
    void *Blt;

    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;

} EFI_GRAPHICS_OUTPUT_PROTOCOL;


/* GUID Definitions */

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


/* Loaded Image Protocol */

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


/* Print Number */

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


/* Draw Rectangle */

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


/* Read BMP File */

EFI_STATUS ReadBMP(
    EFI_FILE_PROTOCOL *Root,
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut
) {

    EFI_FILE_PROTOCOL *File = 0;

    EFI_STATUS status;

    /* BMP dosyasını aç */

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


    /* BMP verisi için RAM alanı */

    static uint8_t bmpBuffer[1024 * 1024];

    uint64_t bufferSize = sizeof(bmpBuffer);


    /* Dosyayı oku */

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


    /* BMP başlık bilgileri */

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

    uint32_t imageWidth =
        *(uint32_t *)&bmpBuffer[18];

    int32_t imageHeight =
        *(int32_t *)&bmpBuffer[22];

    uint16_t bitsPerPixel =
        *(uint16_t *)&bmpBuffer[28];


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
        imageHeight < 0
            ? (uint32_t)(-imageHeight)
            : (uint32_t)imageHeight
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


    /* Dosyayı kapat */

    File->Close(File);

    return EFI_SUCCESS;
}


/* UEFI Entry Point */

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


    /* GOP protokolünü bul */

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


    /* Kırmızı dikdörtgen çiz */

    DrawRectangle(
        GOP,
        100,
        100,
        300,
        200,
        0x00FF0000
    );


    /* Loaded Image Protocol */

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


    /* Simple File System */

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


    /* ESP Root Directory */

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
        ConOut
    );


    /* Uygulamayı açık tut */

    while (1) {
        __asm__ volatile("hlt");
    }

    return EFI_SUCCESS;
}