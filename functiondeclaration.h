#ifndef __FUNCTION_DECLARATION_H__
#define __FUNCTION_DECLARATION_H__

/* =========================================================
   USER DEFINED DATA TYPES
   ========================================================= */

typedef unsigned char  u8;
typedef signed char    s8;

typedef unsigned short u16;
typedef signed short   s16;

typedef unsigned int   u32;
typedef signed int     s32;

typedef float          f32;

 
/* =========================================================
   LCD FUNCTION DECLARATIONS
   ========================================================= */

void WriteLCD(u8);
void CmdLCD(u8);
void InitLCD(void);
void CharLCD(u8);
void StrLCD(s8 *);
void U32LCD(u32);


/* =========================================================
   KEYPAD FUNCTION DECLARATIONS
   ========================================================= */

void Init_KPM(void);
u32 ColScan(void);
u32 RowCheck(void);
u32 ColCheck(void);
u32 KeyScan(void);
u32 ReadNum(int);


/* =========================================================
   RTC FUNCTION DECLARATIONS
   ========================================================= */

void RTC_Init(void);

void SetRTCTimeInfo(u8 hour,
                 u8 minute,
                 u8 second);
void SetRTCDateInfo(u8 day,
                 u8 month,
                 u16 year);

void SetRTCDay(u8 weekDay);

void GetRTCTimeInfo(u8 *hour,
                 u8 *minute,
                 u8 *second);

void GetRTCDateInfo(u8 *day,
                 u8 *month,
                 u16 *year);

void GetRTCDay(u8 *weekDay);


/* =========================================================
   UART0 FUNCTION DECLARATIONS
   UART0 is used for PC/audit communication
   ========================================================= */

void UART0_Init(void);

void U0_Tx(u8 data);
void U0_TxStr(s8 *str);
void U0_TxU32(u32 num);
void U0_TxS32(s32 num);
void U0_TxF32(f32 fNum, u32 nDP);

u8 U0_Rx(void);
s8 *U0_RxStr(void);


/* =========================================================
   UART1 / RFID FUNCTION DECLARATIONS
   ========================================================= */

void UART1_Init(void);

void UART1_isr(void) __irq;

void U1_Tx(u8 sByte);
void U1_TxStr(s8 *p);
void U1_TxU32(u32 num);
void U1_TxS32(s32 num);
void U1_TxF32(f32 fNum, u32 nDP);

u8 U1_Rx(void);
s8 *U1_RxStr(void);

/* =========================================================
   RFID BUFFER VARIABLES
   ========================================================= */

extern volatile u8 rfidBuf[9];
extern volatile u8 ind;
extern volatile u8 flag;


/* =========================================================
   I2C FUNCTION DECLARATIONS
   ========================================================= */

void init_i2c(void);

void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);

void i2c_write(u8 data);

u8 i2c_mack(void);
u8 i2c_nack(void);


/* =========================================================
   EEPROM FUNCTION DECLARATIONS
   ========================================================= */

void i2c_eeprom_bytewrite(u8 slaveAddr,
                      u16 memAddr,
                      u8 data);

u8 i2c_eeprom_randomread(u8 slaveAddr,
                     u16 memAddr);

void i2c_eeprom_pagewrite(u8 slaveAddr,
                      u16 memAddr,
                      u8 *buf,
                      u8 nBytes);

void i2c_eeprom_seqread(u8 slaveAddr,
                    u16 memAddr,
                    u8 *buf,
                    u8 nBytes);
  
/* =========================================================
   VOTER FUNCTION DECLARATIONS
   ========================================================= */

u8 CheckVotingStatus(void);

void DisplayVoterMenu(u8 voterNo);

void VoterMenu(u8 voterNo);

void Voting(u8 voterNo);

u8 PasswordCheck(u8 voterNo);

void ShowPartyMenu(void);

void CastVote(u8 voterNo);

u8 AlreadyVoted(u8 voterNo);

void EditPassword(u8 voterNo);


/* =========================================================
   OFFICER FUNCTION DECLARATIONS
   ========================================================= */

u8 Get2Digit(u8 max);

void SetVotingTime(void);

u8 OfficerPasswordCheck(void);

void StartVoting(void);

void StopVoting(void);

void ViewResult(void);

void ResetVoting(void);

void RTC_Edit(void);

/* =========================================================
   AUTHENTICATION FUNCTION DECLARATION
   ========================================================= */

u8 Authenticate(u8 *cardID);


/* =========================================================
   LOGGER FUNCTION DECLARATIONS
   ========================================================= */

void Logger_SetCardID(u8 *cardID);

u8 *Logger_GetCardID(void);

void LogEvent(s8 *event,
              s8 *cardID,
              s8 *status,
              s8 *description);


/* =========================================================
   DELAY FUNCTION DECLARATIONS
   ========================================================= */

void dly_us(u32 delay);
void dly_ms(u32 delay);
void dly_s(u32 delay);


#endif


