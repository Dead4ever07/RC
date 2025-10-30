#include "macros.h"
#include "alarm.h"


int alarmEnabled = FALSE;
int alarmCount = 0;



void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;

    perror("Alarm received\n");
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