#ifndef __SERIAL_COMM__
#define __SERIAL_COMM__

#include <stdio.h>
#include <signal.h>
#include <unistd.h>  
#include "macros.h"
#include "alarm.h"
#include "serial_port.h"

typedef enum{
    START_READ,
    ADDRESS_READ,
    CONTROL_READ,
    BCC1_READ, 
    FLAGE_READ,
    END_READ
}Read_State;


/// @brief Responsible to send a package in N tries.
/// @param bytes The frame to be sent.
/// @param nBytes The number of bytes in the frame.
/// @param ackByte The Response byte expected.
/// @return  0 on success.
int sendFrame(unsigned char *bytes, int nBytes, unsigned char *ackByte, int nRetransmitions);

/// @brief Reads the bytes from the serial port and compares to an expected frame.
/// @param bytesRef An list of expected frames.
/// @return -1 on error, the position of the command recived on success.
int readBytesAndCompare(unsigned char bytesRef1[5], unsigned char bytesRef2[5]);


/// @brief Read the bytes from the serial port while setting an alarm to avoid getting stuck.
/// @param byte pointer were the byte will be stored.
/// @return -1 on erro otherwise the number of bytes read.
int readByteWithAlarm(unsigned char *byte);




#endif