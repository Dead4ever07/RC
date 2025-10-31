#include "packet_handler.h"

int writeControlPacket(unsigned char control, unsigned long fileSize, const char *filename) {
    
    unsigned char nameSize = strlen(filename);
    if (nameSize > MAX_FILENAME_SIZE) {
        printError(__func__, "Error. The name of the file is too big (max 200 chars)\n");
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
        printError(__func__, "Internal error: incorrect size calculation \n");
        return -1;
    }

    if(llwrite(buf, totalSize) < 0){
        printError(__func__, "Error sending all the control packet.\n");
        return -1;
    }
    return 0;
}



int readControlPacket(unsigned char control, const unsigned char *buf, unsigned long *fileSize, char *filename) {
    int pos = 0;

    if(buf[pos++] != control){
        printError(__func__, "Error in the control value.\n");
        return -1;
    }

    // --- FILE SIZE ---
    if (buf[pos++] != FILE_SIZE_T) {
        printError(__func__, "Error: expected the file size field\n");
        return -1;
    }

    unsigned char sizeFieldLen = buf[pos++];
    if (sizeFieldLen != 4) {
        printError(__func__, "Error: invalid file size length (max 4GB)\n");
        return -1;
    }

    *fileSize = 0;
    for (int i = 0; i < 4; i++) {
        *fileSize = (*fileSize << 8) | buf[pos++];
    }

    // --- FILE NAME ---
    if (buf[pos++] != FILE_NAME_T) {
        printError(__func__, "Error: expected the file name field\n");
        return -1;
    }

    unsigned char nameSize = buf[pos++];
    if (nameSize > MAX_FILENAME_SIZE) {
        printError(__func__, "Error: received file name too long (max 200 chars)\n");
        return -1;
    }

    memcpy(filename, buf + pos, nameSize);
    filename[nameSize] = '\0';
    pos += nameSize;

    return 0;
}
