#include "alarm.h"


int alarmEnabled = FALSE;



void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    timeOutIncrement();
    printError(__func__, "Alarm received\n");
}


void alarmSetup()
{
    struct sigaction act = {0};
    act.sa_handler = &alarmHandler;
    if (sigaction(SIGALRM, &act, NULL) == -1)
    {
        printError(__func__, "sigaction");
        exit(1);
    }
}