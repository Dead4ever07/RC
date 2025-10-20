#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "macros.h"
#include "alarm.h"
#include "serial_port.h"

extern int alarmEnabled;
extern int timeout;

RX_State state = START;
int pos = 0;
int doDestuffing = FALSE;
int BCC2 = 0;

int readBytesAndCompare(unsigned char *bytesRef)
{
    printf("Reading bytes and comparing\n");
    int pos = 0;
    unsigned char buff[COMMAND_SIZE];
    while (pos < COMMAND_SIZE)
    {
        int res = readByteWithAlarm(buff + pos);
        if (res != 1)
            continue;
        printf("res = %x\n", res);
        printf("recived = %x, expected = %x\n", buff[pos], bytesRef[pos]);
        if (buff[pos] != bytesRef[pos])
        {
            printf("Erro while reading bytes, either number of bytes wrong, or wrong response\n");
            return -1;
        }
        pos++;
    }
    return 0;
}

int readByteWithAlarm(unsigned char *byte)
{
    if (alarmEnabled == FALSE)
    {
        alarm(timeout);
        alarmEnabled = TRUE;
    }
    int nbytes = readByteSerialPort(byte);
    alarm(0);
    alarmEnabled = FALSE;
    return nbytes;
}

int processStart(unsigned char byte)
{
    if (byte == FLAG_VALUE)
    {
        state = ADDRESS;
    }
    return 0;
}

int processAddress(unsigned char byte)
{
    if (byte == ADDRESS_SET)
    {
        state = CTRL;
    }
    else if (byte == FLAG_VALUE)
    {
        state = ADDRESS;
    }
    else
    {
        state = START;
    }
    return 0;
}

int processControl(unsigned char byte, int curr_frame)
{
    if (byte == CTRL_I(curr_frame))
    {
        state = BCC1;
    }
    else if (byte == FLAG_VALUE)
    {
        state = ADDRESS;
    }
    else
    {
        state = START;
    }
    return 0;
}

int processBCC1(unsigned char byte, int curr_frame)
{
    if (byte == (ADDRESS_SET ^ CTRL_I(curr_frame)))
    {
        state = DATA;
    }
    else
    {
        state = START;
    }
    return 0;
}

int processData(unsigned char byte, unsigned char *payload)
{

    if (pos >= MAX_PAYLOAD_SIZE)
    {
        state = START;
        return 0;
    }
    if (byte == FLAG_VALUE)
    {
        if (BCC2 == 0)
        {
            int ret = --pos;
            pos = 0;
            state = START;
            return ret;
        }
        else
        {
            state = START;
            pos = 0;
            return -1;
        }
    }
    else if (doDestuffing)
    {
        payload[pos] = byte ^ 0x20;
        BCC2 ^= payload[pos];
        pos++;
        doDestuffing = FALSE;
    }
    else if (EXCAPE_CHAR)
    {
        doDestuffing = TRUE;
    }
    else
    {
        payload[pos] = byte;
        BCC2 ^= payload[pos];
        pos++;
    }
    return 0;
}

int processByte(unsigned char byte, unsigned char *payload, int curr_frame)
{

    switch (state)
    {
    case START:
        return processStart(byte);
        break;
    case ADDRESS:
        return processAddress(byte);
        break;
    case CTRL:
        return processControl(byte, curr_frame);
        break;
    case BCC1:
        return processBCC1(byte, curr_frame);
        break;
    case DATA:
        return processData(byte, payload);
        break;
    default:
        state = START;
        return -1;
        break;
    }
}
