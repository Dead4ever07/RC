#ifndef __PACKET__
#define __PACKET__


#define FLAG_VALUE 0x7E
#define ADDRESS_UA 0x03
#define ADDRESS_SET 0x03
#define CTRL_UA 0x07
#define CTRL_SET 0x03


enum STATE{
    START,
    FLAG,
    ADDRESS,
    CONTROL,
    CHECKED,
    DATA,
    FINISHED
};


typedef struct
{
    enum STATE state;
    char flag_b;
    char address;
    char control;
    char bcc;
    char data;
    char flag_e;
    char curr_byte;
}Packet;

/*
    Creates a new handshake initialize at 0
    @returns A pointer to the Handshake
*/
Packet* create_packet();

/*
    @param byte - Byte to be processed
    @param Handshake - Handshake where the byte will be incerted into
    @returns 0 on success, 1 if the message was fully sent and -1 on error
*/
int process_byte_sender(char byte, Packet* handshake);

int process_byte_receiver(char byte, Packet* handshake);

/*
    Returns number of bytes already filled in
    the Handshake on success, -1 otherwise
*/
int number_bytes_received(Packet* handshake);


#endif