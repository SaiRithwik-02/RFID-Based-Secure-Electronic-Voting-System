//KPM.c
//#include "types.h"
#include <lpc21xx.h>
#include "functiondeclaration.h"
#include "component_defines.h"

 u8 kpmLUT[4][4]={{'1','2','3','A'},
                  {'4','5','6','B'},
                  {'7','8','9','C'},
                  {'*','0','#','D'}};    

 void Init_KPM(void)
 {
         //cfg rows(p1.16-p1.19) as outputs
         IODIR1|=15<<ROW0;
 }
 u32 ColScan(void)
 {
         if(((IOPIN1>>COL0)&15)<15)
         {
                 return 0;
         }
         else
         {
                 return 1;
         }
 }
 u32 RowCheck(void)
 {
         u32 rno;
         for(rno=0; rno<4; rno++)
         {
        IOPIN1=(IOPIN1&~(15<<ROW0))|((~(1<<rno))<<ROW0);
                 if(ColScan()==0)
                 {
                         break;
                 }
         }
         //make row as defaults
         IOCLR1=15<<ROW0;
         return rno;
 }
 u32 ColCheck(void)
 {
         u32 cno;
         for(cno=0; cno<4; cno++)
         {
                 if(((IOPIN1>>(COL0+cno))&1)==0)
                         break;
         }

         return cno;

 }
 u32 KeyScan(void)
 {
         u32 rno,cno,key;
         //wait for switch press
         while(ColScan());
         //find rno
         rno=RowCheck();
         //find cno
         cno=ColCheck();
         //get key value from KPMLUT[][]
         key=kpmLUT[rno][cno];
         //wait for switch release
         while(!ColScan());
         //return keyvalue
         return key;
 }
 u32 ReadNum(int digits)
{
    u32 num = 0;
    u32 count = 0;
    u8 key;
    while(1)
    {
        key = KeyScan();
        /* ENTER */
        if(key == '#')
        {
            if(count != 0)
            {
                break;
            }
        }
        /* BACKSPACE */
        else if(key == 'C')
        {
            if(count > 0)
           {
                num = num / 10;
                count--;
                CmdLCD(0x10);
                CharLCD(' ');
                CmdLCD(0x10);
            }
        }
        /* Numeric key */
        else if((key >= '0')&&(key <= '9'))
        {
            if(count < digits)
           {
                num = num * 10 +(key - '0');
                    CharLCD(key);
                count++;
            }
        }
    }
    return num;
}

u32 ReadPassword(void)
{
    u32 password = 0;
    u32 count = 0;
    u8 key;
    while(1)
    {
        key = KeyScan();
        /* Number */
        if((key >= '0')&&(key <= '9'))
        {
            if(count < 3)
            {
                password =password*10+(key - '0');
                CharLCD('*');
                count++;
            }
        }
        /* Backspace */
        else if(key == 'C')
        {
            if(count > 0)
            {
                password = password / 10;
                count--;
                CmdLCD(0x10);
                CharLCD(' ');
                CmdLCD(0x10);
            }
        }
        /* Enter */
        else if(key == '#')
        {
            if(count == 3)
            {
                return password;
            }
        }
    }
}

