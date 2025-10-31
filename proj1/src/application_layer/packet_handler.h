#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdio.h>
#include <string.h>
#include "../link_layer/link_layer.h"
#include "../debug.h"


///@brief Builds and sends a control packet (START or END).
///@param control     The control field (START_CONTROL or END_CONTROL).
///@param fileSize    Size of the file being transferred, in bytes.
/// @param filename    Null-terminated string containing the file's name.
///
///@return 0 on success, -1 on failure.
int writeControlPacket(unsigned char control, unsigned long fileSize, const char *filename);


///@brief Parses a received control packet (START or END).
///@param control     Expected control value (START_CONTROL or END_CONTROL).
///@param buf         Pointer to the received packet buffer.
///@param fileSize    Output pointer that will store the parsed file size.
///@param filename    Output buffer to store the extracted filename. 
///                    Must be at least MAX_FILENAME_SIZE+1 bytes.
///@return 0 on success, -1 if packet format is invalid.
int readControlPacket(unsigned char control, const unsigned char *buf, unsigned long *fileSize, char *filename);

#endif