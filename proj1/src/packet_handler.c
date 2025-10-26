#include "packet_handler.h"

int writeControlPacket(unsigned char control, unsigned long fileSize, const char *filename) {
    
    unsigned char nameSize = strlen(filename);

    if (nameSize > MAX_FILENAME_SIZE) {
        printf("Error. The name of the file is too big (max %d chars)\n", MAX_FILENAME_SIZE);
        return -1;
    }

    unsigned char totalSize = CONTROL_HEADER_SIZE + nameSize;

    unsigned char buf[totalSize];
    int pos = 0;

    buf[pos++] = control;

    // --- FILE SIZE ---
    buf[pos++] = FILE_SIZE_T;
    buf[pos++] = 4; 
    //big endian
    buf[pos++] = (fileSize >> (8 * 3)) & 0xFF;
    buf[pos++] = (fileSize >> (8 * 2)) & 0xFF;
    buf[pos++] = (fileSize >> 8) & 0xFF;
    buf[pos++] = fileSize & 0xFF;

    // --- FILE NAME ---
    buf[pos++] = FILE_NAME_T;
    buf[pos++] = nameSize;

    memcpy(buf + pos, filename, nameSize);
    pos += nameSize;

    if (pos != totalSize) {
        printf("Internal error: incorrect size calculation (%d != %d)\n", pos, totalSize);
        return -1;
    }

    return llwrite(buf, totalSize);
}



int readControlPacket(unsigned char control, const unsigned char *buf, unsigned long *fileSize, char *filename) {
    int pos = 0;

    if(buf[pos++] != control){
        printf("Error in the control value.\n");
        return -1;
    }

    // --- FILE SIZE ---
    if (buf[pos++] != FILE_SIZE_T) {
        printf("Error: expected the file size field\n");
        return -1;
    }

    unsigned char sizeFieldLen = buf[pos++];
    if (sizeFieldLen != 4) {
        printf("Error: invalid file size length (%d)\n", sizeFieldLen);
        return -1;
    }

    *fileSize = 0;
    for (int i = 0; i < 4; i++) {
        *fileSize = (*fileSize << 8) | buf[pos++];
    }

    // --- FILE NAME ---
    if (buf[pos++] != FILE_NAME_T) {
        printf("Error: expected the file name field\n");
        return -1;
    }

    unsigned char nameSize = buf[pos++];
    if (nameSize > MAX_FILENAME_SIZE) {
        printf("Error: received file name too long (%d > %d)\n", nameSize, MAX_FILENAME_SIZE);
        return -1;
    }

    memcpy(filename, buf + pos, nameSize);
    filename[nameSize] = '\0';
    pos += nameSize;

    return 0;
}
