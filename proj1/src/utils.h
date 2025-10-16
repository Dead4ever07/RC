#ifndef _UTILS_H_
#define _UTILS_H_


typedef enum{
    START,
    ADDRESS,
    CTRL,
    BCC1,
    DATA,
}RX_State;


/// @brief 
/// @param byte 
/// @param payload 
/// @param curr_packet 
/// @return the function uses a series of comands to tell what happened 
///  
/// 0 the byte was processed successfully.
///
/// -1 the frame was recived but there was an error, sould send a REJ 
///
/// Positive, the number of bytes stored in the packet
/// 
int processByte(unsigned char byte, char* payload,int curr_packet);

int processData(char byte, char* packet);

/// @brief Responsible to read a response and compare with the expected
/// @param bytesRef expected bytes on successfull response 
/// @return returns 0 if successfull
int readBytesAndCompare(unsigned char *bytesRef);


/// @brief Responsible to read a response without blocking, using an alarm
/// @param byte Responsible to read a response without 
/// @return the number of bytes read or -1 in case of error
int readByteWithAlarm(unsigned char *byte);



#endif