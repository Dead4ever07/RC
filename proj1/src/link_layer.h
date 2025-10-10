// Link layer header.
// DO NOT CHANGE THIS FILE

#ifndef _LINK_LAYER_H_
#define _LINK_LAYER_H_

#define FLAG_VALUE 0x7E
#define ADDRESS_UA 0x03
#define ADDRESS_SET 0x03
#define CTRL_UA 0x07
#define CTRL_SET 0x03
#define CTRL_RR 0x05
#define CTRL_REJ 0x01
#define CTRL_DISC 0x0B
#define COMMAND_SIZE 4

typedef enum
{
    LlTx,
    LlRx,
} LinkLayerRole;

typedef struct
{
    char serialPort[50];
    LinkLayerRole role;
    int baudRate;
    int nRetransmissions;
    int timeout;
} LinkLayer;

// Size of maximum acceptable payload.
// Maximum number of bytes that application layer should send to link layer.
#define MAX_PAYLOAD_SIZE 1000

// MISC
#define FALSE 0
#define TRUE 1

// Open a connection using the "port" parameters defined in struct linkLayer.
// Return 0 on success or -1 on error.
int llopen(LinkLayer connectionParameters);

// Send data in buf with size bufSize.
// Return number of chars written, or -1 on error.
int llwrite(const unsigned char *buf, int bufSize);

// Receive data in packet.
// Return number of chars read, or -1 on error.
int llread(unsigned char *packet);

// Close previously opened connection and print transmission statistics in the console.
// Return 0 on success or -1 on error.
int llclose();

#endif // _LINK_LAYER_H_
