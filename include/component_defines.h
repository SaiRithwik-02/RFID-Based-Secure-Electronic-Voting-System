#ifndef __COMPONENT_DEFINES_H__
#define __COMPONENT_DEFINES_H__


/* =========================================================
   LCD DEFINITIONS
   LCD controller: HD44780
   ========================================================= */


/* ---------- HD44780 LCD Commands ---------- */

#define CLEAR_LCD            0x01
// Clears the complete LCD display.

#define RET_CUR_HOME         0x02
// Returns the LCD cursor to the home position.

#define SHIFT_CUR_RIGHT      0x06
// Configures the LCD so the cursor moves to the right
// after writing a character.

#define SHIFT_CUR_LEFT       0x07
// Configures the LCD so the cursor moves to the left
// after writing a character.

#define DSP_OFF              0x08
// Turns the LCD display OFF.

#define DSP_ON_CUR_OFF       0x0C
// Turns the LCD display ON and cursor OFF.

#define DSP_ON_CUR_ON        0x0E
// Turns the LCD display ON and cursor ON.

#define DSP_ON_CUR_BLK       0x0F
// Turns the LCD display ON with blinking cursor.

#define SHIFT_DSP_LEFT       0x10
// Shifts the complete LCD display one position to the left.

#define SHIFT_DSP_RIGHT      0x14
// Shifts the complete LCD display one position to the right.


/* ---------- LCD Data Interface Modes ---------- */

#define MODE_8BIT_1LINE      0x30
// Configures LCD for 8-bit data mode and one display line.

#define MODE_4BIT_1LINE      0x20
// Configures LCD for 4-bit data mode and one display line.

#define MODE_8BIT_2LINE      0x38
// Configures LCD for 8-bit data mode and two display lines.

#define MODE_4BIT_2LINE      0x28
// Configures LCD for 4-bit data mode and two display lines.


/* ---------- LCD Cursor Positions ---------- */

#define GOTO_LINE1_POS0      0x80
// LCD DDRAM address for row 1, column 0.

#define GOTO_LINE2_POS0      0xC0
// LCD DDRAM address for row 2, column 0.

#define GOTO_LINE3_POS0      0x90
// LCD DDRAM address defined in the original project
// for row 3, column 0.

#define GOTO_LINE4_POS0      0xD0
// LCD DDRAM address defined in the original project
// for row 4, column 0.


/* ---------- LCD CGRAM ---------- */

#define GOTO_CGRAM_START     0x40
// Starting address of LCD CGRAM used for custom characters.


/* ---------- LCD Pin Configuration ---------- */

#define LCD_DATA 16
// Starting LPC2148 GPIO pin used for LCD data lines.

#define RS 13
// LCD Register Select control pin definition.

#define RW 15
// LCD Read/Write control pin definition.

#define EN               14
// LCD Enable control pin definition.

/* =========================================================
   KEYPAD DEFINITIONS
   Keypad connected to Port 1
   ========================================================= */


/* ---------- Keypad Row Pins ---------- */

#define ROW0 16
// Keypad row 0 is connected to P1.16.

#define ROW1 17
// Keypad row 1 is connected to P1.17.

#define ROW2 18
// Keypad row 2 is connected to P1.18.

#define ROW3 19
// Keypad row 3 is connected to P1.19.


/* ---------- Keypad Column Pins ---------- */

#define COL0 20
// Keypad column 0 is connected to P1.20.

#define COL1 21
// Keypad column 1 is connected to P1.21.

#define COL2 22
// Keypad column 2 is connected to P1.22.

#define COL3 23
// Keypad column 3 is connected to P1.23.



/* =========================================================
   COMMON CLOCK DEFINITIONS
   Used by UART0, UART1 and I2C
   ========================================================= */

#define FOSC                 12000000UL
// LPC2148 oscillator frequency = 12 MHz.

#define CCLK                 (FOSC * 5)
// CPU clock frequency.
// PLL multiplier is 5, therefore CCLK = 60 MHz.

#define PCLK                 (CCLK / 4)
// Peripheral clock frequency.
// PCLK = 60 MHz / 4 = 15 MHz.



/* =========================================================
   UART0 DEFINITIONS
   UART0 is used for PC/audit communication.
   ========================================================= */


/* ---------- UART0 Pin Configuration ---------- */

#define TXD0_PIN             0x00000001
// Selects P0.0 as TXD0.

#define RXD0_PIN             0x00000004
// Selects P0.1 as RXD0.


/* ---------- UART Baud Rate ---------- */

#define BAUD                 9600
// UART communication baud rate = 9600 bits per second.

#define DIVISOR              (PCLK / (16 * BAUD))
// UART baud-rate divisor calculated from PCLK and baud rate.


/* ---------- UART0 Line Control Register ---------- */

#define _8BIT                3
// UART data length selection for 8-bit data.

#define WORD_LEN             _8BIT
// Selects 8-bit UART word length.

#define DLAB_BIT             7
// Bit 7 of U0LCR/U1LCR.
// DLAB enables access to UART divisor registers.

/* ---------- UART0 Line Status Register ---------- */

#define DR_BIT               0
// Data Ready bit of UART LSR.

#define TEMT_BIT             6
// Transmitter Empty bit of UART LSR.



/* =========================================================
   UART1 DEFINITIONS
   UART1 is used for RFID communication.
   ========================================================= */


/* ---------- UART1 Pin Configuration ---------- */

#define TXD1_PIN             0x00010000
// Selects P0.8 as TXD1.

#define RXD1_PIN             0x00040000
// Selects P0.9 as RXD1.


/*
   UART1 uses the same clock and baud-rate configuration
   defined above:

   FOSC
   CCLK
   PCLK
   BAUD
   DIVISOR
   _8BIT
   WORD_LEN
   DLAB_BIT
   DR_BIT
   TEMT_BIT

   Therefore they are defined only once in this
   combined file.
   */


/* =========================================================
   I2C DEFINITIONS
   ========================================================= */


/* ---------- I2C Pin Function Selection ---------- */

/* ---------- I2C Pin Function Selection ---------- */

#define SCL_PIN_FUNC         0x00000010
// Selects the LPC2148 SCL pin function.

#define SDA_PIN_FUNC         0x00000040
// Selects the LPC2148 SDA pin function.


/* ---------- I2C Speed ---------- */

#define I2C_SPEED            100000
// I2C communication speed = 100 kHz.

#define LOADVAL              ((PCLK / I2C_SPEED) / 2)
// Value loaded into I2C clock high/low registers
// to generate the required I2C clock.



/* ---------- I2CONSET Register Bits ---------- */

#define AA_BIT               2
// Assert Acknowledge bit.

#define SI_BIT               3
// I2C Interrupt/Status bit.

#define STO_BIT              4
// STOP condition control bit.

#define STA_BIT              5
// START condition control bit.

#define I2EN_BIT             6
// I2C interface enable bit.



/* ---------- I2CONCLR Register Bits ---------- */

#define AAC_BIT              2
// Clears the Assert Acknowledge bit.

#define SIC_BIT              3
// Clears the I2C interrupt/status flag.

#define STAC_BIT             5
// Clears the START condition bit.

#define I2ENC_BIT            6
// I2C interface disable/clear bit.


/* =========================================================
   EEPROM DEFINITIONS
   AT24C256 EEPROM memory map
   ========================================================= */


/* ---------- EEPROM Slave Addresses ---------- */

#define I2C_EEPROM_SA0       0x50
// EEPROM slave address when A2,A1,A0 = 000.


#define I2C_EEPROM_SA1       0x51
// EEPROM slave address when A2,A1,A0 = 001.

#define I2C_EEPROM_SA2       0x52
// EEPROM slave address when A2,A1,A0 = 010.

#define I2C_EEPROM_SA3       0x53
// EEPROM slave address when A2,A1,A0 = 011.

#define I2C_EEPROM_SA4       0x54
// EEPROM slave address when A2,A1,A0 = 100.

#define I2C_EEPROM_SA5       0x55
// EEPROM slave address when A2,A1,A0 = 101.

#define I2C_EEPROM_SA6       0x56
// EEPROM slave address when A2,A1,A0 = 110.

#define I2C_EEPROM_SA7       0x57
// EEPROM slave address when A2,A1,A0 = 111.



/* =========================================================
   EEPROM MEMORY MAP
   ========================================================= */


/* ---------- Voting Control ---------- */

#define EEPROM_VOTING_ENABLE      0x0000
// Stores whether voting is enabled or disabled.

#define EEPROM_START_HOUR         0x0001
// Stores voting start hour.

#define EEPROM_START_MIN          0x0002
// Stores voting start minute.

#define EEPROM_END_HOUR           0x0003
// Stores voting closing hour.

#define EEPROM_END_MIN            0x0004
// Stores voting closing minute.



/* ---------- Voter RFID IDs ---------- */

#define EEPROM_VOTER1_ID          0x0010
// EEPROM location reserved for Voter 1 RFID ID.

#define EEPROM_VOTER2_ID          0x0020
// EEPROM location reserved for Voter 2 RFID ID.

#define EEPROM_VOTER3_ID          0x0030
// EEPROM location reserved for Voter 3 RFID ID.



/* ---------- Voter Passwords ---------- */

#define EEPROM_VOTER1_PASS        0x0040
// EEPROM starting address for Voter 1 password.

#define EEPROM_VOTER2_PASS        0x0044
// EEPROM starting address for Voter 2 password.

#define EEPROM_VOTER3_PASS        0x0048
// EEPROM starting address for Voter 3 password.



/* ---------- Voter Voting Status ---------- */

#define EEPROM_VOTER1_STATUS      0x0050
// Stores whether Voter 1 has already voted.

#define EEPROM_VOTER2_STATUS      0x0051
// Stores whether Voter 2 has already voted.

#define EEPROM_VOTER3_STATUS      0x0052
// Stores whether Voter 3 has already voted.



/* ---------- Party Vote Counts ---------- */

#define EEPROM_PARTY1_COUNT       0x0060
// Stores the vote count of Party 1.

#define EEPROM_PARTY2_COUNT       0x0061
// Stores the vote count of Party 2.

#define EEPROM_PARTY3_COUNT       0x0062
// Stores the vote count of Party 3.

#define EEPROM_PARTY4_COUNT       0x0063
// Stores the vote count of Party 4.



/* ---------- Officer Password ---------- */

#define EEPROM_OFFICER_PASS       0x0070
// EEPROM starting address for the officer password.



/* ---------- EEPROM First-Time Initialization ---------- */

#define EEPROM_INIT_FLAG          0x0080
// EEPROM location used to store the initialization marker.

#define EEPROM_INIT_VALUE         0xA5
// Marker value indicating that EEPROM has already
// received its initial default values.



/* =========================================================
   AUTHENTICATION / USER ROLE DEFINITIONS
   ========================================================= */

#define INVALID_CARD             0
// Returned when the RFID card does not match
// any registered card.

#define OFFICER                  1
// Identifies the RFID card as the officer card.

#define VOTER1                   2
// Identifies the RFID card as Voter 1.

#define VOTER2                   3
// Identifies the RFID card as Voter 2.

#define VOTER3                   4
// Identifies the RFID card as Voter 3.


#endif

