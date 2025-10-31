#ifndef __SERIAL_COMM__
#define __SERIAL_COMM__

#include <stdio.h>
#include <signal.h>
#include <unistd.h>  
#include "../macros.h"
#include "../serial_port.h"
#include "../debug.h"
#include "alarm.h"
#include "statistics.h"

typedef enum{
    START_READ,
    ADDRESS_READ,
    CONTROL_READ,
    BCC1_READ, 
    FLAG_END_READ,
    END_READ
}Read_State;


/// @brief Responsible to send a package in N tries.
/// @param bytes The frame to be sent.
/// @param nBytes The number of bytes in the frame.
/// @param ackByte The Response byte expected.
/// @param nRetransmitions number of retransmissions allowed
/// @return  0 on success.
int sendFrame(unsigned char *bytes, int nBytes, unsigned char *ackByte, int nRetransmitions);

/// @brief Reads the bytes from the serial port and compares to an expected frame.
/// @param bytesRef1 first expected command to be recived
/// @param bytesRef2 second expected command to be recived, can be null
/// @return returns -1 on error, 0 if the response was bytesRef1 and 1 if the response was bytesRef2
int readBytesAndCompare(unsigned char bytesRef1[5], unsigned char bytesRef2[5]);


/// @brief Read the bytes from the serial port while setting an alarm to avoid getting stuck.
/// @param byte pointer were the byte will be stored.
/// @return -1 on error otherwise the number of bytes read.
int readByteWithAlarm(unsigned char *byte);

/// @brief Write up to n bytes from the "byte" array to the serial port.
/// @param byte array of bytes to read from
/// @param n number of bytes to sent
/// @return The number of bytes actually sent
int writeBytesToSerialPort(unsigned char *byte, int n);



#endif