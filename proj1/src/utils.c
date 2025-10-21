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
    int pos = 0;
    int isWrong = FALSE;
    unsigned char buff[COMMAND_SIZE];
    while (pos < COMMAND_SIZE)
    {
        int res = readByteWithAlarm(buff + pos);
        if (res == 0)
        {
            //fazer um try
            continue;
        }
        else if (res == -1)
        {
            //dar o erro certo!
            return -1;
        }
        else
        {
            if (buff[pos] != bytesRef[pos])
            {
                //perguntar ao professor sobre o lixo!
                printf("Wrong pos =%d\n", pos);
                printf("%x != %x\n", buff[pos], bytesRef[pos]);
                printf("Erro while reading bytes, either number of bytes wrong, or wrong response\n");
                isWrong = TRUE;
            }
            pos++;
        }
    }
    return isWrong;
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

//perguntar ao professor se tem uma forma melhor de lidar com o lixo!
int processStart(unsigned char byte)
{
    if (byte == FLAG_VALUE)
    {
        state = ADDRESS;
    }else{
        printf("Error. The first value read was not a flag value!");
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
        printf("Receive the flag value again when should receive address.\n");
        state = ADDRESS;
    }
    else
    {
        printf("Didn't receive a valid address value.\n");
        state = START;
        return -1;
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
        printf("Receive the flag value again when should receive control.\n");
        state = ADDRESS;
    }
    else
    {
        printf("Receive the wrong control.\n");
        state = START;
        return -1;
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
        printf("Receive the wrong BCC1.\n");
        state = START;
        return -1;
    }
    return 0;
}

int processData(unsigned char byte, unsigned char *payload)
{
    //questão de receber a trama de controlo na data não damos erro e da return -1, rejeita a trama!
    //ou caso de so receber o bcc2!
    if (byte == FLAG_VALUE)
    {
        
        //printf("Flag recived\n");
        // printf("BCC2 = %x\n", payload[pos-1]);
        //printf("Real BCC2 = %x\n", BCC2);
        if (BCC2 == 0)
        {
            int ret = --pos;
            pos = 0;
            state = START;
            return ret;
        }
        else
        {
            printf("The BCC2 was not correct\n");
            state = START;
            pos = 0;
            return -1;
        }
    }
    if (pos >= MAX_PAYLOAD_SIZE + 1)
    {
        state = START;
        printf("The data size was bigger than the maximum.\n");
        return -1;
    }
    if (doDestuffing)
    {
        payload[pos] = byte ^ 0x20;
        BCC2 ^= payload[pos];
        pos++;
        doDestuffing = FALSE;
    }
    else if (byte == EXCAPE_CHAR)
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
