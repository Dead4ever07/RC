#ifndef _MACROS_H_
#define _MACROS_H_

#define FLAG_VALUE 0x7E
#define ADDRESS_UA 0x03
#define ADDRESS_SENDER 0x03
#define ADDRESS_RECEIVER_DISC 0x01
#define ADDRESS_SENDER_DISC 0x01
#define ADDRESS_SET 0x03
#define CTRL_UA 0x07
#define CTRL_SET 0x03
#define CTRL_DISC 0x0B
#define EXCAPE_CHAR 0x7D
#define XOR_BYTE 0x20

#define MAX_PAYLOAD_SIZE 1017
#define MAX_DATA_FIELD 1014
#define DATA_HEADER_SIZE 3


#define COMMAND_SIZE 5

#define CTRL_RR(x) (x?0xAA:0xAB)
#define CTRL_I(x) (x?0x80:0x00)

#define CTRL_REJ(x) 0x54+x
#define COMMAND(x,y)  {FLAG_VALUE, x, (y), (x^(y)), FLAG_VALUE}

#define FALSE 0
#define TRUE 1


#define FILE_SIZE_T 0
#define FILE_NAME_T 1
#define MAX_FILENAME_SIZE 200
#define CONTROL_HEADER_SIZE 9
#define START_CONTROL 1
#define DATA_CONTROL 2
#define END_CONTROL 3

#endif