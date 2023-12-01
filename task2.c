#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
// #include <unistd.h> //for unix systems
#include <io.h> //for windows systems
#include <stdint.h>
#include <string.h>

/*http://www.maverick-os.dk/FileSystemFormats/FAT16_FileSystem.html#:~:text=The%20FAT16%20file%20system%20uses,volume%20(volume%20%3D%20partition).*/

typedef struct __attribute__((__packed__))
{
    uint8_t BS_jmpBoot[3];
    uint8_t BS_OEMName[8];
    uint16_t BPB_BytsPerSec; // Bytes per Sector
    uint8_t BPB_SecPerClus;  // Sectors per Cluster
    uint16_t BPB_RsvdSecCnt; // Reserved Sector Count
    uint8_t BPB_NumFATs;     // Number of copies of FAT
    uint16_t BPB_RootEntCnt; // FAT12/FAT16: size of root DIR
    uint16_t BPB_TotSec16;   // Sectors, may be 0, see below
    uint8_t BPB_Media;
    uint16_t BPB_FATSz16; // Sectors in FAT (FAT12 or FAT16)
    uint16_t BPB_SecPerTrk;
    uint16_t BPB_NumHeads;
    uint32_t BPB_HiddSec;
    uint32_t BPB_TotSec32; // Sectors if BPB_TotSec16 == 0
    uint8_t BS_DrvNum;
    uint8_t BS_Reserved1;
    uint8_t BS_BootSig;
    uint32_t BS_VolID;
    uint8_t BS_VolLab[11]; // Non zero terminated string
    uint8_t BS_FilSysType[8];
} BootSector;

unsigned char *searchFile(int byteOffset, int numOfBytesToRead, char *path)
{
    unsigned char *buffer = malloc(numOfBytesToRead);

    int file = open(path, O_RDONLY | O_CREAT);

    if (file != -1)
    {
        off_t offset = lseek(file, byteOffset, SEEK_CUR);

        if (offset != -1)
        {
            size_t readFile = read(file, buffer, numOfBytesToRead);
            return buffer;
        }
        else
        {
            buffer = "Theres's been an error moving the file bootSectorPointer.";
        }
    }
    else
    {
        buffer = "There's been an error opening the files";
    }
    return buffer;
}

int main()
{
    BootSector bootSector;
    BootSector *bootSectorPointer = &bootSector;
    int toFillValues[8];
    int counter = 0;
    int result = 0;
    char path[] = "fat16.img";
    unsigned char *values = searchFile(11, 2, path);
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    printf("Bytes Per Sector: %d\n", result);
    values = searchFile(13, 1, path);
    result = (unsigned int)values[0];
    toFillValues[counter] = result;
    counter++;
    printf("Sectors Per Cluster: %d\n", result);
    values = searchFile(14, 2, path);
    result = 0;
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    printf("Reserved Sector Count: %d\n", result);
    values = searchFile(16, 1, path);
    result = (unsigned int)values[0];
    toFillValues[counter] = result;
    counter++;
    printf("Copies Of Fat: %d\n", result);
    values = searchFile(17, 2, path);
    result = 0;
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    printf("FAT12/FAT16: size of root DIR: %d\n", result);
    values = searchFile(19, 2, path);
    result = 0;
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    printf("Sectors, may be 0, see below: %d\n", result);
    values = searchFile(22, 2, path);
    result = 0;
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    printf("Sectors in FAT (FAT12 or FAT16): %d\n", result);
    values = searchFile(32, 4, path);
    result = 0;
    for (int x = 0; x < 4; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    printf("Sectors if BPB_TotSec16 == 0: %d\n", result);
    toFillValues[counter] = result;
    counter++;
    values = searchFile(42, 17, path);
    char *string = values;
    printf("Non zero terminated string: %s\n", values);
    for (int x = 0; x < 8; x++)
    {
        printf("%d + "
               " ",
               toFillValues[x]);
    }
    printf("\n");
    bootSectorPointer->BPB_BytsPerSec = toFillValues[0];
    bootSectorPointer->BPB_SecPerClus = toFillValues[1];
    bootSectorPointer->BPB_RsvdSecCnt = toFillValues[2];
    bootSectorPointer->BPB_NumFATs = toFillValues[3];
    bootSectorPointer->BPB_RootEntCnt = toFillValues[4];
    bootSectorPointer->BPB_TotSec16 = toFillValues[5];
    bootSectorPointer->BPB_FATSz16 = toFillValues[6];
    bootSectorPointer->BPB_TotSec32 = toFillValues[7];
    strcpy(bootSectorPointer->BS_VolLab, string);
}