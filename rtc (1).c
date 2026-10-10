#include <lpc214x.h>
#include "functiondeclaration.h"
#include "component_defines.h"
#define LPC2129 0
#define LPC2148 1
#define CPU LPC2148

#define FOSC 12000000
#define CCLK (FOSC*5)    
#define PCLK (CCLK/4)

#define PREINT_VAL  ((int) ((PCLK / 32768) - 1))
#define PREFRAC_VAL (PCLK -((PREINT_VAL + 1) * 32768))
//CCR
#define RTC_ENABLE (1<<0)
#define RTC_RESET (1<<1)
#define RTC_CLKSRC  (1<<4)
char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
s32 hour,min,sec,date,month,year,day;

void RTC_Init(void)
{
  // Disable and reset the RTC
        CCR = RTC_RESET;

        #if (CPU == LPC2129)
  // Set prescaler integer and fractional parts
        PREINT = PREINT_VAL;
        PREFRAC = PREFRAC_VAL;
		// Enable the RTC
        CCR = RTC_ENABLE;

        #elif (CPU == LPC2148)
  // Enable the RTC with external clock source
        CCR = RTC_ENABLE | RTC_CLKSRC;
        #endif

}

void SetRTCTimeInfo(u8 hour, u8 minute, u8 second)
{
        HOUR = hour;
        MIN = minute;
        SEC = second;
}
void SetRTCDateInfo(u8 date, u8 month, u16 year)
{
        DOM = date;
        MONTH = month;
        YEAR = year;
}

void SetRTCDay(u8 day)
{
        DOW = day;
}
void GetRTCTimeInfo(u8 *hour, u8 *minute, u8 *second)
{
        *hour = HOUR;
        *minute = MIN;
        *second = SEC;
}
void GetRTCDateInfo(u8 *date, u8 *month, u16 *year)
{
        *date = DOM;
        *month = MONTH;
        *year = YEAR;
}
void GetRTCDay(u8 *day)
{
        *day = DOW;
}

void DisplayRTCTime(u32 hour, u32 minute, u32 second)
{
                CmdLCD(GOTO_LINE1_POS0);
                CharLCD((hour/10)+48);
                CharLCD((hour%10)+48);
                CharLCD(':');
                CharLCD((minute/10)+0x30);
                CharLCD((minute%10)+0x30);
                CharLCD(':');
                CharLCD((second/10)+'0');
                CharLCD((second%10)+'0');
}

void DisplayRTCDate(u32 date, u32 month, u32 year)
{
        CmdLCD(GOTO_LINE2_POS0);
        CharLCD((date/10)+48);
        CharLCD((date%10)+48);
        CharLCD('/');
        CharLCD((month/10)+0x30);
        CharLCD((month%10)+0x30);
        CharLCD('/');
        U32LCD(year);
}
void DisplayRTCDay(u32 day)
{
        CmdLCD(GOTO_LINE1_POS0+11);
        StrLCD(week[day]);
}








       