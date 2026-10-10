#include "functiondeclaration.h"
#include "component_defines.h"
u8 Get2Digit(u8 max)
{
    u8 d1, d2, key;
    u8 value;

    while(1)
    {
        d1 = 0;
        d2 = 0;

        while(1)
        {
            key = KeyScan();

            if(key >= '0' && key <= '9')
            {
                d1 = key - '0';
                CharLCD(key);
                break;
            }
        }

        while(1)
        {
            key = KeyScan();

            if(key >= '0' && key <= '9')
            {
                d2 = key - '0';
                CharLCD(key);
                break;
            }
        }

        while(1)
        {
            key = KeyScan();


            if(key == '#')
                break;
        }

        value = (d1 * 10) + d2;

        if(value <= max)
            return value;

        CmdLCD(0xC0);
        StrLCD("INVALID VALUE");

        dly_ms(1000);

        CmdLCD(0xC0);
        StrLCD("                ");

        CmdLCD(0xC0);
    }
}

void SetVotingTime(void)
{
    u8 startHour;
    u8 startMin;
    u8 endHour;
    u8 endMin;
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("START HOUR:");
    CmdLCD(0xC0);

    startHour = Get2Digit(23);

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("START MIN:");
    CmdLCD(0xC0);

    startMin = Get2Digit(59);

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("END HOUR:");
    CmdLCD(0xC0);

    endHour = Get2Digit(23);


    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("END MIN:");
    CmdLCD(0xC0);

    endMin = Get2Digit(59);//1
    if((endHour < startHour) ||
       ((endHour == startHour) && (endMin <= startMin)))
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("INVALID TIME");

        CmdLCD(0xC0);
        StrLCD("END AFTER START");

        dly_ms(1500);

        return;
    }
    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_START_HOUR,
                     startHour);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_START_MIN,
                     startMin);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_END_HOUR,
                     endHour);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_END_MIN,
                     endMin);
    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("VOTING TIME SET");

    CmdLCD(0xC0);
    StrLCD("   SUCCESS");

    dly_ms(1500);
}
u8 OfficerPasswordCheck(void)
{
    u8 i, key;
    u8 pass[4];

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("OFFICER PASSWORD");
    CmdLCD(0xC0);

    for(i = 0; i < 4; i++)
    {
        while(1)
        {
            key = KeyScan();

            if(key >= '0' && key <= '9')
            {
                pass[i] = key;
                CharLCD('*');
                break;
            }
			/* 'C' is BACKSPACE */
        else if(key == 'C')
        {
            if(i > 0)
            {
                i--;
                /* Move cursor back, overwrite with a blank space, then move back again */
                CmdLCD(0x10); // Shift cursor position to the left
                CharLCD(' '); // Clear the character on screen
                CmdLCD(0x10); // Shift cursor back to the left again
            }
        }

        }
    }

    while(KeyScan() != '#');

    for(i = 0; i < 4; i++)
    {
        if(pass[i] != i2c_eeprom_randomread(I2C_EEPROM_SA0,
                                         EEPROM_OFFICER_PASS + i))
            return 0;
    }

    return 1;
}

void StartVoting(void)
{
    if(OfficerPasswordCheck())
    {
        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_VOTING_ENABLE,
                         1);

        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD(" VOTING STARTED");

        dly_ms(1500);
    }
    else
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("INVALID PASSWORD");

        dly_ms(1500);
    }
}
void StopVoting(void)
{
    if(OfficerPasswordCheck())
    {
        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_VOTING_ENABLE,
                         0);

        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD(" VOTING STOPPED");

        dly_ms(1500);
    }
    else
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("INVALID PASSWORD");

        dly_ms(1500);
    }
}
void ViewResult(void)
{
	u32 arr[4],max=-1,i,t;
    u8 Batters;
    u8 Bowlers;
    u8 Keepers;
    u8 Umpires;

    if(OfficerPasswordCheck())
    {
        Batters = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                                   EEPROM_PARTY1_COUNT);
		   arr[0]=Batters;
        Bowlers = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                      	         EEPROM_PARTY2_COUNT);
			arr[1]=Bowlers;

        Keepers = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                               EEPROM_PARTY3_COUNT);
			arr[2]=Keepers;
        Umpires = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                                EEPROM_PARTY4_COUNT);
			arr[3]=Umpires;
        CmdLCD(0x01);

        CmdLCD(0x80);
        StrLCD("Batters : ");
        U32LCD(Batters);

        CmdLCD(0xC0);
        StrLCD("Bowlers : ");
        U32LCD(Bowlers);

        CmdLCD(0x94);
        StrLCD("Keepers     : ");
        U32LCD(Keepers);

        CmdLCD(0xD4);
        StrLCD("Umpires    : ");
        U32LCD(Umpires);

        dly_ms(3000);
		for(i=0;i<4;i++)
		{
				 if(arr[i]>max){
				 	max=arr[i];
				   	t=i;
					}
		}
		CmdLCD(0x01);
		if(t==0)
		{ CmdLCD(0x80);
		  StrLCD("Batters Won");
		}
		else if(t==1)
		{
	  	  CmdLCD(0x80);
		  StrLCD("Bowlers Won");	
		}
		else if(t==2)
		{
		  CmdLCD(0x80);
		  StrLCD("Keepers Won");
		}
		else
		{
		  CmdLCD(0x80);
		  StrLCD("Umpires Won");
		}
		  CmdLCD(0xc0);
		  StrLCD("vote count: ");
		  U32LCD(max);
		  dly_ms(3000);
		  CmdLCD(0x01);


    }
    else
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("INVALID PASSWORD");

        dly_ms(1500);
    }
}
void ResetVoting(void)
{
    if(OfficerPasswordCheck())
    {
        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_PARTY1_COUNT, 0);

        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_PARTY2_COUNT, 0);

        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_PARTY3_COUNT, 0);

        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_PARTY4_COUNT, 0);

        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_VOTER1_STATUS, 0);

        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_VOTER2_STATUS, 0);

        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_VOTER3_STATUS, 0);

        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         EEPROM_VOTING_ENABLE, 0);

        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD(" VOTING RESET");

        CmdLCD(0xC0);
        StrLCD("   SUCCESS");

        dly_ms(1500);
    }
    else
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("INVALID PASSWORD");

        dly_ms(1500);
    }
}
void RTC_Edit(void)
{
    u8 hour;
    u8 min;
    u8 sec;
    u8 day;
    u8 month;
    u8 year2;
    u8 weekDay;
    u16 year;

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("SET HOUR:");
    CmdLCD(0xC0);
    hour = Get2Digit(23);

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("SET MIN:");
    CmdLCD(0xC0);
    min = Get2Digit(59);

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("SET SEC:");
    CmdLCD(0xC0);
    sec = Get2Digit(59);

    while(1)
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("SET DAY:");
        CmdLCD(0xC0);

        day = Get2Digit(31);

        if(day != 0)
            break;
    }

    while(1)
    {
        CmdLCD(0x01);
        CmdLCD(0x80);
        StrLCD("SET MONTH:");
        CmdLCD(0xC0);

        month = Get2Digit(12);

        if(month != 0)
            break;
    }
	CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("SET YEAR 20__");

    CmdLCD(0xC0);
    year2 = Get2Digit(99);

    year = 2000 + year2;

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("WEEK DAY:");

    CmdLCD(0xC0);
    weekDay = Get2Digit(6);

    SetRTCTimeInfo(hour, min, sec);
    SetRTCDateInfo(day, month, year);
    SetRTCDay(weekDay);

    CmdLCD(0x01);
    CmdLCD(0x80);
    StrLCD("RTC UPDATED");

    CmdLCD(0xC0);
    StrLCD("   SUCCESS");

    dly_ms(1500);
}

