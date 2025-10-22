#ifndef __SERIAL_COMM__
#define __SERIAL_COMM__
#include "alarm.h"
#include "macros.h"
#include "serial_port.h"




/// @brief Responsible to send a package in N tries.
/// @param bytes The frame to be sent.
/// @param nBytes The number of bytes in the frame.
/// @param ackByte The Response byte expected.
/// @return  0 on success.
int sendFrame(unsigned char *bytes, int nBytes, unsigned char *ackByte, int nRetransmitions);

/// @brief Reads the bytes from the serial port and compares to an expected frame.
/// @param bytesRef The Command expected to be recived.
/// @return -1 on error, 1 if the byte recived was wrong and 0 on success.
int readBytesAndCompare(unsigned char *bytesRef);

/// @brief Read the bytes from the serial port while setting an alarm to avoid getting stuck.
/// @param byte pointer were the byte will be stored.
/// @return -1 on erro otherwise the number of bytes read.
int readByteWithAlarm(unsigned char *byte);




#endif