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

char *searchFile(int byteOffset, int numOfBytesToRead, char *path)
{
    char *buffer = malloc(numOfBytesToRead);

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
            buffer = "";
        }
    }
    else
    {
        buffer = "";
    }
    return buffer;
}

int main()
{
    BootSector bootSector;
    BootSector *pointer = &bootSector;
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
    values = searchFile(13, 1, path);
    result = (unsigned int)values[0];
    toFillValues[counter] = result;
    counter++;
    values = searchFile(14, 2, path);
    result = 0;
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    values = searchFile(16, 1, path);
    result = (unsigned int)values[0];
    toFillValues[counter] = result;
    counter++;
    values = searchFile(17, 2, path);
    result = 0;
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    values = searchFile(19, 2, path);
    result = 0;
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    values = searchFile(22, 2, path);
    result = 0;
    for (int x = 0; x < 2; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    values = searchFile(32, 4, path);
    result = 0;
    for (int x = 0; x < 4; x++)
    {
        result |= (int)values[x] << (8 * x); // bit shifting
    }
    toFillValues[counter] = result;
    counter++;
    values = searchFile(42, 17, path);
    char *string = values;
    for (int x = 0; x < 8; x++)
    {
        printf("%d + "
               " ",
               toFillValues[x]);
    }
    printf("\n");
    pointer->BPB_BytsPerSec = toFillValues[0]; // Bytes per Sector
    pointer->BPB_SecPerClus = toFillValues[1]; // Sectors per Cluster
    pointer->BPB_RsvdSecCnt = toFillValues[2]; // Reserved Sector Count
    pointer->BPB_NumFATs = toFillValues[3];    // Number of copies of FAT
    pointer->BPB_RootEntCnt = toFillValues[4]; // FAT12/FAT16: size of root DIR
    pointer->BPB_TotSec16 = toFillValues[5];   // Sectors, may be 0, see below
    pointer->BPB_FATSz16 = toFillValues[6];    // Sectors in FAT (FAT12 or FAT16)
    pointer->BPB_TotSec32 = toFillValues[7];   // Sectors if BPB_TotSec16 == 0
    strcpy(pointer->BS_VolLab, string);        // Non zero terminated string
    // load the first fat
    uint16_t FAT_Offset = pointer->BPB_RsvdSecCnt * pointer->BPB_BytsPerSec;
    uint16_t FAT_Size = pointer->BPB_FATSz16 * pointer->BPB_BytsPerSec;
    uint16_t buffer[FAT_Size];
    int file = open("fat16.img", O_RDONLY | O_CREAT);
    off_t offset = lseek(file, FAT_Offset, SEEK_SET);
    size_t readFile = read(file, buffer, FAT_Size);
    uint16_t currentCluster = 5; // starting cluster
    int count = 0;
    while (currentCluster < 0xfff8)
    {
        printf("%d ->", currentCluster);
        currentCluster = buffer[currentCluster];
    }
    printf("%d\n", currentCluster);
}