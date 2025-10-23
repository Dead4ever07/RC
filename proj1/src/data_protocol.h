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
/// -2 the frame recived is repeated, send a RR 
///
/// Positive, the number of bytes stored in the packet
/// 
int processByte(unsigned char byte, unsigned char* payload,int curr_packet);

/// @brief 
/// @param data 
/// @param byte 
/// @return number of bytes added to the data array 
int  byteStuffing(unsigned char* data, unsigned char byte);


#endif