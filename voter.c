#include "functiondeclaration.h"
#include "component_defines.h"


/* ========================================================= */
/* CHECK WHETHER VOTING IS CURRENTLY OPEN                    */
/* ========================================================= */

u8 CheckVotingStatus(void)
{
    u8 votingEnable;

    u8 startHour;
    u8 startMin;

    u8 endHour;
    u8 endMin;

    u8 curHour;
    u8 curMin;
    u8 curSec;


    /* Read voting enable flag */

    votingEnable = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                                     EEPROM_VOTING_ENABLE);
    if(votingEnable == 0)
        return 0;

    /* Read voting start time */

    startHour = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                                  EEPROM_START_HOUR);

    startMin = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                                 EEPROM_START_MIN);
    /* Read voting end time */

    endHour = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                                EEPROM_END_HOUR);

    endMin = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                               EEPROM_END_MIN);
    /* Read current RTC time */

    GetRTCTimeInfo(&curHour, &curMin, &curSec);


    /* Check start time */

    if(curHour < startHour)
        return 0;

    if((curHour == startHour) && (curMin < startMin))
        return 0;


    /* Check end time */

    if(curHour > endHour)
        return 0;

    if((curHour == endHour) && (curMin > endMin))
        return 0;


    return 1;
}

 
/* ========================================================= */
/* DISPLAY VOTER MENU                                       */
/* ========================================================= */

void DisplayVoterMenu(u8 voterNo)
{
    CmdLCD(0x01);


    /* Row 1 */

    CmdLCD(0x80);

    switch(voterNo)
    {
        case 1:
            StrLCD("       VOTER 1");
            break;

        case 2:
            StrLCD("       VOTER 2");
            break;

        case 3:
            StrLCD("       VOTER 3");
            break;

        default:
            StrLCD("        VOTER");
            break;
    }


    /* Row 2 */

    CmdLCD(0xc0);
    StrLCD("1. VOTING");


    /* Row 3 */

    CmdLCD(0x94);
    StrLCD("2. EDIT PASSWORD");


    /* Row 4 */

    CmdLCD(0xD4);
    StrLCD("3. EXIT");
}
/*==========================================================*/
/*Voting Enabled */
/*==========================================================*/

/*u8 VotingEnabled(void)
{
	u8 status;
	status = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                               EEPROM_VOTING_ENABLE);


    if(status == 1)
        return 1;


    return 0;
} */

/* ========================================================= */
/* VOTER MENU                                                */
/* ========================================================= */

void VoterMenu(u8 voterNo)
{
    u8 option;

    while(1)
    {
        DisplayVoterMenu(voterNo);

        option = KeyScan();

        switch(option)
        {
            case '1':

                Voting(voterNo);

                break;


            case '2':

                EditPassword(voterNo);

                break;
			 case '3':

                CmdLCD(0x01);

                CmdLCD(0x80);
                StrLCD("     THANK YOU");

                dly_ms(1000);

                return;


            default:

                CmdLCD(0x01);

                CmdLCD(0x80);
                StrLCD("   INVALID OPTION");

                dly_ms(1000);

                break;
        }
    }
}


/* ========================================================= */
/* VOTING                                                     */
/* ========================================================= */

void Voting(u8 voterNo)
{	 if(CheckVotingStatus() == 0)
    {
        CmdLCD(0x01);

        CmdLCD(0x80);
        StrLCD(" VOTING NOT OPEN");

        CmdLCD(0xC0);
        StrLCD("  ACCESS DENIED");


        LogEvent("VOTING ACCESS BLOCKED",
                 (s8 *)Logger_GetCardID(),
                 "BLOCKED",
                 "Voting disabled or outside authorized time");


        dly_ms(2000);

        return;
    }

    /* Check whether voter has already voted */

    if(AlreadyVoted(voterNo))
    {
        CmdLCD(0x01);

        CmdLCD(0x80);
        StrLCD("   ALREADY VOTED");

        CmdLCD(0xC0);
        StrLCD("   ACCESS DENIED");

        dly_ms(2000);

        return;
    }


    /* Check password */

    if(PasswordCheck(voterNo))
    {
        CmdLCD(0x01);

        CmdLCD(0x80);
        StrLCD("    PASSWORD OK");

        dly_ms(1000);


        /* Display party menu */

        ShowPartyMenu();


        /* Cast vote */

        CastVote(voterNo);
    }
    else
    {
        CmdLCD(0x01);

        CmdLCD(0x80);
        StrLCD("  WRONG PASSWORD");

        CmdLCD(0xC0);
        StrLCD("   ACCESS DENIED");

        dly_ms(2000);
    }
}
/* ========================================================= */
/* PASSWORD CHECK                                            */
/* ========================================================= */

u8 PasswordCheck(u8 voterNo)
{
    u8 enteredPass[5];
    u8 storedPass[5];

    u16 addr;

    u8 i;
    u8 key;


    /* Select password address */

    switch(voterNo)
    {
        case 1:

            addr = EEPROM_VOTER1_PASS;

            break;


        case 2:

            addr = EEPROM_VOTER2_PASS;

            break;


        case 3:

            addr = EEPROM_VOTER3_PASS;

            break;


        default:

            return 0;
    }


    /* Enter password */

    CmdLCD(0x01);

    CmdLCD(0x80);
    StrLCD("   ENTER PASSWORD");

    CmdLCD(0xC0);


    i = 0;

    while(1)
    {
        key = KeyScan();


        if(key >= '0' && key <= '9')
        {
            if(i < 4)
            {
                enteredPass[i] = key;

                i++;

                CharLCD('*');
            }
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



        /* '=' is ENTER key */

        else if(key == '#')
        {
            if(i == 4)
                break;
        }
   }

enteredPass[4] = '\0';


    /* Read password from EEPROM */

    for(i = 0; i < 4; i++)
    {
        storedPass[i] =
            i2c_eeprom_randomread(I2C_EEPROM_SA0,
                              addr + i);
    }

    storedPass[4] = '\0';


    /* Compare password */

    for(i = 0; i < 4; i++)
    {
        if(enteredPass[i] != storedPass[i])
            return 0;
    }


    return 1;
}


/* ========================================================= */
/* DISPLAY PARTY MENU                                        */
/* ========================================================= */

void ShowPartyMenu(void)
{
    CmdLCD(0x01);


    /* Row 1 */

    CmdLCD(0x80);
    StrLCD("    SELECT PARTY");
	dly_ms(1500);
	CmdLCD(0x01);
    /* Row 1 */

    CmdLCD(0x80);
    StrLCD("1. Batters * ");


    /* Row 2 */

    CmdLCD(0xc0);
    StrLCD("2. Bowlers $ ");


    /* Row 3 */

    CmdLCD(0x94);
    StrLCD("3. Keepers  # ");

	 /* Row 4 */
	 CmdLCD(0xD4);
    StrLCD("4. Umpires % "); 

	
}

/* ========================================================= */
/* CAST VOTE                                                 */
/* ========================================================= */

void CastVote(u8 voterNo)
{
    u8 key;
    u8 count;

    u16 addr;


    while(1)
    {
        key = KeyScan();


        switch(key)
        {
            case '1':

                addr = EEPROM_PARTY1_COUNT;

                break;


            case '2':

                addr = EEPROM_PARTY2_COUNT;

                break;


            case '3':

                addr = EEPROM_PARTY3_COUNT;

                break;


            case '4':

                addr = EEPROM_PARTY4_COUNT;

                break;


            default:

                CmdLCD(0x01);

                CmdLCD(0x80);
                StrLCD("  INVALID CHOICE");

                dly_ms(1000);

                ShowPartyMenu();

                continue;
        }


        /* Read present vote count */

        count = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                                   addr);


        /* Increment vote count */

        count++;


        /* Store updated vote count */

        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         addr,
                         count);

        /* Mark voter as voted */

			   switch(voterNo)
        {
            case 1:

                i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                                 EEPROM_VOTER1_STATUS,
                                 1);

                break;


            case 2:

                i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                                 EEPROM_VOTER2_STATUS,
                                 1);

                break;


            case 3:

                i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                                 EEPROM_VOTER3_STATUS,
                                 1);

                break;
        }


        /* Vote success */

        CmdLCD(0x01);

        CmdLCD(0x80);
        StrLCD("     VOTE CAST");

        CmdLCD(0xC0);
        StrLCD("   SUCCESSFULLY");
		

        /* GREEN LED ON */
        //IOSET0 = (1 << GREEN_LED);

        dly_ms(2000);


        /* GREEN LED OFF */
        //IOCLR0 = (1 << GREEN_LED);


        return;
    }
}


/* ========================================================= */
/* CHECK WHETHER VOTER ALREADY VOTED                         */
/* ========================================================= */

u8 AlreadyVoted(u8 voterNo)
{
    u8 status;

    u16 addr;


    switch(voterNo)
    {
        case 1:

            addr = EEPROM_VOTER1_STATUS;

            break;


        case 2:

            addr = EEPROM_VOTER2_STATUS;

            break;
		 case 3:

            addr = EEPROM_VOTER3_STATUS;

            break;


        default:

            return 1;
    }


    status = i2c_eeprom_randomread(I2C_EEPROM_SA0,
                               addr);


    if(status == 1)
        return 1;


    return 0;
}


/* ========================================================= */
/* EDIT PASSWORD                                              */
/* ========================================================= */

void EditPassword(u8 voterNo)
{
    u16 addr;

    u8 oldPass[5];
    u8 newPass[5];
    u8 storedPass[5];

    u8 i;
    u8 key;


    /* Select password address */

    switch(voterNo)
    {
        case 1:

            addr = EEPROM_VOTER1_PASS;

            break;


        case 2:

            addr = EEPROM_VOTER2_PASS;

            break;


        case 3:

            addr = EEPROM_VOTER3_PASS;

            break;


        default:

            return;
    }
    /* ===================================================== */
    /* ENTER OLD PASSWORD                                    */
    /* ===================================================== */

    CmdLCD(0x01);

    CmdLCD(0x80);
    StrLCD("   OLD PASSWORD");

    CmdLCD(0xC0);
		

    i = 0;

    while(1)
    {
        key = KeyScan();


        if(key >= '0' && key <= '9')
        {
            if(i < 4)
            {
                oldPass[i] = key;

                i++;

                CharLCD('*');
            }
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

	   
        /* '#' is ENTER */

        else if(key == '#')
        {
            if(i == 4)
                break;
        }
    }


    oldPass[4] = '\0';
		

    /* ===================================================== */
    /* READ STORED PASSWORD                                  */
    /* ===================================================== */

    for(i = 0; i < 4; i++)
    {
        storedPass[i] =
            i2c_eeprom_randomread(I2C_EEPROM_SA0,
                              addr + i);
    }

    storedPass[4] = '\0';


    /* ===================================================== */
    /* CHECK OLD PASSWORD                                    */
    /* ===================================================== */

    for(i = 0; i < 4; i++)
    {
        if(oldPass[i] != storedPass[i])
        {
            CmdLCD(0x01);

            CmdLCD(0x80);
            StrLCD("  WRONG PASSWORD");

            CmdLCD(0xC0);
            StrLCD("   ACCESS DENIED");

            dly_ms(2000);

            return;
        }
    }

 /* ===================================================== */
    /* ENTER NEW PASSWORD                                    */
    /* ===================================================== */

    CmdLCD(0x01);

    CmdLCD(0x80);
    StrLCD("   NEW PASSWORD");

    CmdLCD(0xC0);


    i = 0;

    while(1)
    {
        key = KeyScan();


        if(key >= '0' && key <= '9')
        {
            if(i < 4)
            {
                newPass[i] = key;

                i++;

                CharLCD('*');
            }
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



        /* '#' is ENTER */

        else if(key == '#')
        {
            if(i == 4)
                break;
        }
    }


    newPass[4] = '\0';


    /* ===================================================== */
    /* STORE NEW PASSWORD                                    */
    /* ===================================================== */

    for(i = 0; i < 4; i++)
    {
        i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                         addr + i,
                         newPass[i]);
    }


    /* ===================================================== */
    /* SUCCESS MESSAGE                                       */
    /* ===================================================== */

    CmdLCD(0x01);

    CmdLCD(0x80);
    StrLCD("PASSWORD UPDATED");

    CmdLCD(0xC0);
    StrLCD("   SUCCESSFULLY");

    dly_ms(2000);
}


