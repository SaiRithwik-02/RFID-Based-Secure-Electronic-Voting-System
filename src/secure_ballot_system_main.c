#include "functiondeclaration.h"
#include "component_defines.h"

extern volatile u8 flag;
//extern volatile u8 rfidBuf[9];
int main(void)
{
    u8 role;
    u8 key;


    /* ------------------------------------------------ */
    /* Initialize peripherals                           */
    /* ------------------------------------------------ */

    InitLCD();
    Init_KPM();
    init_i2c();
    RTC_Init();

    UART0_Init();
    UART1_Init();


    /* ------------------------------------------------ */
    /* RTC test time                                    */
    /* ------------------------------------------------ */

    SetRTCTimeInfo(15, 30, 0);
    SetRTCDateInfo(22, 9, 2026);
    SetRTCDay(2);


    /* ------------------------------------------------ */
    /* Initial EEPROM configuration for testing         */
    /* ------------------------------------------------ */

    /*
     * Officer password = 0000
     */

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_OFFICER_PASS + 0, '0');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_OFFICER_PASS + 1, '0');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_OFFICER_PASS + 2, '0');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_OFFICER_PASS + 3, '0');


    /*
     * Voter 1 password = 1111
     */

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER1_PASS + 0, '1');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER1_PASS + 1, '1');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER1_PASS + 2, '1');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER1_PASS + 3, '1');


    /*
     * Voter 2 password = 2222
     */

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER2_PASS + 0, '2');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER2_PASS + 1, '2');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER2_PASS + 2, '2');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER2_PASS + 3, '2');


    /*
     * Voter 3 password = 3333
     */

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER3_PASS + 0, '3');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER3_PASS + 1, '3');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER3_PASS + 2, '3');

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER3_PASS + 3, '3');


    /* ------------------------------------------------ */
    /* Initial voting status                            */
    /* ------------------------------------------------ */

    /*
     * IMPORTANT:
     * Voting must be CLOSED when system starts.
     *
     * Officer must explicitly select START VOTING.
     */
	 if(i2c_eeprom_randomread(I2C_EEPROM_SA0,EEPROM_INIT_FLAG)!=EEPROM_INIT_VALUE)
	 {
    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTING_ENABLE, 0);
	   

    /*
     * Voting time = 00:00 to 23:59
     */

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_START_HOUR, 0);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_START_MIN, 0);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_END_HOUR, 23);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_END_MIN, 59);


    /* ------------------------------------------------ */
    /* Reset voter status                               */
    /* ------------------------------------------------ */

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER1_STATUS, 0);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER2_STATUS, 0);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_VOTER3_STATUS, 0);


    /* ------------------------------------------------ */
    /* Reset vote counts                                */
    /* ------------------------------------------------ */

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_PARTY1_COUNT, 0);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_PARTY2_COUNT, 0);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_PARTY3_COUNT, 0);

    i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
                     EEPROM_PARTY4_COUNT, 0);
	i2c_eeprom_bytewrite(I2C_EEPROM_SA0,EEPROM_INIT_FLAG,EEPROM_INIT_VALUE);
	}

	/*-------------------------------------------------*/
	/*Voting Option*/
	/*-------------------------------------------------*/
//	i2c_eeprom_bytewrite(I2C_EEPROM_SA0,
  //                   EEPROM_VOTING_ENABLE, 0);
 
    /* ------------------------------------------------ */
    /* SYSTEM START LOG                                 */
    /* ------------------------------------------------ */

    LogEvent("SYSTEM_START",
             "N/A",
             "SUCCESS",
             "System initialized");


    /* ------------------------------------------------ */
    /* Main RFID screen                                 */
    /* ------------------------------------------------ */

    CmdLCD(0x01);

    CmdLCD(0x80);
    StrLCD(" SECURE BALLOT");

    CmdLCD(0xC0);
    StrLCD("SCAN YOUR CARD");

    dly_ms(2000);


    /* ------------------------------------------------ */
    /* Main loop                                        */
    /* ------------------------------------------------ */

    while(1)
    {			   
        /*
         * UART1 interrupt receives 8-digit RFID.
         * When complete:
         * flag = 1
         */

        if(flag == 1)
        {
            /*
             * Save scanned card ID for logger.
             */

            Logger_SetCardID((u8 *)rfidBuf);


            /* ------------------------------------------------ */
            /* CARD SCAN LOG                                    */
            /* ------------------------------------------------ */

            LogEvent("CARD_SCAN",
                     (s8 *)Logger_GetCardID(),
                     "RECEIVED",
                     "RFID detected");


            /* ------------------------------------------------ */
            /* Authenticate scanned RFID                        */
            /* ------------------------------------------------ */

            role = Authenticate((u8 *)rfidBuf);


            CmdLCD(0x01);

            CmdLCD(0x80);
            StrLCD("RFID VERIFIED");

            CmdLCD(0xC0);


            switch(role)
            {
                /* -------------------------------------------- */
                /* OFFICER                                      */
                /* -------------------------------------------- */

                case OFFICER:

                    StrLCD("OFFICER");

                    dly_ms(1500);

                    flag = 0;


                    /* ---------------------------------------- */
                    /* AUTHENTICATION LOG                       */
                    /* ---------------------------------------- */

                    LogEvent("AUTHENTICATION",
                             (s8 *)Logger_GetCardID(),
                             "SUCCESS",
                             "Officer authenticated");


                    /* ---------------------------------------- */
                    /* OFFICER LOGIN LOG                        */
                    /* ---------------------------------------- */

                    LogEvent("OFFICER_LOGIN",
                             (s8 *)Logger_GetCardID(),
                             "SUCCESS",
                             "Officer authenticated");


                    /* ---------------------------------------- */
                    /* Officer menu                              */
                    /* ---------------------------------------- */

                    while(1)
                    {
                        CmdLCD(0x01);

                        CmdLCD(0x80);
                        StrLCD("1.SET TIME 2.START");

                        CmdLCD(0xC0);
                        StrLCD("3.STOP     4.RESULT");

                        CmdLCD(0x94);
                        StrLCD("5.RESET    6.RTC");

                        CmdLCD(0xD4);
                        StrLCD("7.EXIT");
                        key = KeyScan();
                        switch(key)
                        {
                            case '1':
                                SetVotingTime();
                                break;

                            case '2':
                                StartVoting();
                                break;

                            case '3':
                                StopVoting();
                                break;

                            case '4':
                                ViewResult();
                                break;

                            case '5':
                                ResetVoting();
                                break;

                            case '6':
                                RTC_Edit();
                                break;

                            case '7':
                                break;

                            default:

                                CmdLCD(0x01);
                                CmdLCD(0x80);
                                StrLCD("INVALID OPTION");

                                dly_ms(1000);

                                break;
                        }


                        if(key == '7')
                            break;
                    }

                    break;


                /* -------------------------------------------- */
                /* VOTER 1                                      */
                /* -------------------------------------------- */

                case VOTER1:

                    StrLCD("VOTER 1");

                    dly_ms(1500);

                    flag = 0;


                    LogEvent("AUTHENTICATION",
                             (s8 *)Logger_GetCardID(),
                             "SUCCESS",
                             "Voter authenticated");


                    VoterMenu(1);

                    break;


                /* -------------------------------------------- */
                /* VOTER 2                                      */
                /* -------------------------------------------- */

                case VOTER2:

                    StrLCD("VOTER 2");

                    dly_ms(1500);

                    flag = 0;


                    LogEvent("AUTHENTICATION",
                             (s8 *)Logger_GetCardID(),
                             "SUCCESS",
                             "Voter authenticated");


                    VoterMenu(2);

                    break;


                /* -------------------------------------------- */
                /* VOTER 3                                      */
                /* -------------------------------------------- */

                case VOTER3:

                    StrLCD("VOTER 3");

                    dly_ms(1500);

                    flag = 0;


                    LogEvent("AUTHENTICATION",
                             (s8 *)Logger_GetCardID(),
                             "SUCCESS",
                             "Voter authenticated");


                    VoterMenu(3);

                    break;


                /* -------------------------------------------- */
                /* INVALID CARD                                 */
                /* -------------------------------------------- */

                default:

                    LogEvent("AUTHENTICATION",
                             (s8 *)Logger_GetCardID(),
                             "FAILED",
                             "Invalid card");


                    StrLCD("INVALID CARD");

                    dly_ms(1500);

                    flag = 0;

                    break;
            }


            /* ---------------------------------------- */
            /* Return to RFID scan screen               */
            /* ---------------------------------------- */

            CmdLCD(0x01);

            CmdLCD(0x80);
            StrLCD(" SECURE BALLOT");

            CmdLCD(0xC0);
            StrLCD("SCAN YOUR CARD");
        }
    }
}
