#include <LPC21xx.h>
#include "functiondeclaration.h"
#include "component_defines.h"

static u8 currentCardID[9] = "N/A";


void Logger_SetCardID(u8 *cardID)
{
    u8 i;

    if(cardID == 0)
    {
        currentCardID[0] = 'N';
        currentCardID[1] = '/';
        currentCardID[2] = 'A';
        currentCardID[3] = '\0';
        return;
    }

    for(i = 0; i < 8; i++)
    {
        if(cardID[i] == '\0')
            break;

        currentCardID[i] = cardID[i];
    }

    currentCardID[i] = '\0';
}
u8 *Logger_GetCardID(void)
{
    return currentCardID;
}


static void Tx2Digit(u8 value)
{
    U0_Tx((value / 10) + '0');
    U0_Tx((value % 10) + '0');
}


static void SendDateTime(void)
{
    u8 day;
    u8 month;
    u8 hour;
    u8 min;
    u8 sec;
    u16 year;

    GetRTCDateInfo(&day, &month, &year);
    GetRTCTimeInfo(&hour, &min, &sec);

    Tx2Digit(day);
    U0_Tx('-');

    Tx2Digit(month);
    U0_Tx('-');
 U0_TxU32(year);

    U0_Tx(',');
    U0_Tx(' ');

    Tx2Digit(hour);
    U0_Tx(':');

    Tx2Digit(min);
    U0_Tx(':');

    Tx2Digit(sec);
}


void LogEvent(s8 *event,
              s8 *cardID,
              s8 *status,
              s8 *description)
{
    SendDateTime();

    U0_Tx(',');
    U0_Tx(' ');
    U0_TxStr(event);

    U0_Tx(',');
    U0_Tx(' ');
    U0_TxStr(cardID);
	U0_Tx(',');
    U0_Tx(' ');
    U0_TxStr(status);

    U0_Tx(',');
    U0_Tx(' ');
    U0_TxStr(description);

    U0_TxStr("\r\n");
	}