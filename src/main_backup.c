#include <stdint.h>

typedef uint64_t EFI_STATUS;
typedef uint16_t CHAR16;

#define EFI_SUCCESS 0

typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    void *Reset;

    EFI_STATUS (*OutputString)(
        EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
        CHAR16 *String
    );
};

typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;

    void *FirmwareVendor;
    uint32_t FirmwareRevision;
    uint32_t Padding;

    void *ConsoleInHandle;
    void *ConIn;

    void *ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;

} EFI_SYSTEM_TABLE;


/* Mesajı global alanda tutuyoruz. */
static CHAR16 message[] = {
    'H', 'e', 'l', 'l', 'o', ' ',
    'f', 'r', 'o', 'm', ' ',
    'B', 'a', 'r', 'e', ' ',
    'M', 'e', 't', 'a', 'l', '!',
    '\r', '\n',
    0
};


/* UEFI uygulamasının giriş noktası. */
EFI_STATUS efi_main(
    void *ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
)
{
    (void)ImageHandle;

    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        message
    );

    while (1) {
        __asm__ volatile("hlt");
    }

    return EFI_SUCCESS;
}