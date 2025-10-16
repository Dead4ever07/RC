#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "alarm.h"
#include "macros.h"


int alarmEnabled = FALSE;
int alarmCount = 0;



void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;

    printf("Alarm #%d received\n", alarmCount);
}


void alarmSetup()
{
    struct sigaction act = {0};
    act.sa_handler = &alarmHandler;
    if (sigaction(SIGALRM, &act, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
}