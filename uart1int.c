#include <LPC21xx.h>
#include "functiondeclaration.h"
#include "component_defines.h"
volatile u8 rfidBuf[9]={'\0'};
volatile u8  ind = 0;
volatile u8   flag,i;
void UART1_isr(void) __irq
{
    u8 ch;

    if((U1IIR & 0x0E) == 0x04)
    {
        ch = U1RBR;
		if(ch==0x02){
			flag=1;
		}
		else if(ch==0x03){
			flag=0;
			i=0;
		}
		else{
			rfidBuf[i++]=ch;
		}
		
        //if((ch >= '0') && (ch <= '9'))
        //{
                 /* if(ch==0X02){
                                  flag=1;
                                  }
                                   if(flag){
                                        rfidBuf[ind++]=ch;
                                   }
                                   if(ch==0X03){
                                       rfidBuf[ind]='\0';
                                           flag=0;
                                           ind=0;
                                   } */

              /*           if(flag==0){
						  CmdLCD(0x01);
						  StrLCD("Hi working");
                            if(ind<8){
                                rfidBuf[ind]=ch;
                                ind++;
                                }
                                  if(ind==8)
								  {
                                  rfidBuf[ind]='\0';
								  CmdLCD(0xc0);
								  StrLCD(rfidBuf);
                                  ind=0;
								 
                                  flag=1;
                                  }
                          }

			*/
          //  }
        }					   


    VICVectAddr = 0;
}
  void UART1_Init(void)
{
    /* Configure P0.8 as TXD1 and P0.9 as RXD1 */
    PINSEL0 &= ~(0x0F << 16);
    PINSEL0 |= TXD1_PIN | RXD1_PIN;
    U1LCR = (1 << DLAB_BIT) | WORD_LEN;
    U1DLL = DIVISOR & 0xFF;
    U1DLM = (DIVISOR >> 8) & 0xFF;
    U1LCR &= ~(1 << DLAB_BIT);
    U1IER = 0x01;
    VICIntSelect &= ~(1 << 7);
    VICVectAddr0 = (u32)UART1_isr;
    VICVectCntl0 = (1 << 5)|7;
    VICIntEnable |= (1 << 7);
}

void U1_Tx(u8 sByte)
{
    U1THR = sByte;

    while(((U1LSR >> TEMT_BIT) & 1) == 0);
}

u8 U1_Rx(void)
{
    while(((U1LSR >> DR_BIT) & 1) == 0);

    return U1RBR;
}

void U1_TxStr(s8 *p)
{
    while(*p)
    {
        U1_Tx(*p);
        p++;
    }
}
void U1_TxU32(u32 num)
{
    u8 a[10];
    s32 i = 0;

    if(num == 0)
    {
        U1_Tx('0');
    }
    else
    {
        while(num > 0)
        {
            a[i++] = (num % 10) + '0';
            num /= 10;
        }

        while(--i >= 0)
        {
            U1_Tx(a[i]);
       }
    }
}
void U1_TxS32(s32 num)
{
    if(num < 0)
    {
        U1_Tx('-');
        num = -num;
    }

    U1_TxU32(num);
}

void U1_TxF32(f32 fNum, u32 nDP)
{
    u32 num;
    s32 i;

    if(fNum < 0)
    {
        U1_Tx('-');
        fNum = -fNum;
    }

    num = (u32)fNum;

    U1_TxU32(num);

    U1_Tx('.');
    for(i = 0; i < nDP; i++)
    {
        fNum = (fNum - num) * 10;

        num = (u32)fNum;

        U1_Tx(num + '0');
    }
}

s8 *U1_RxStr(void)
{
    static s8 rbuf[100];

    s32 i = 0;

    while(1)
    {
        rbuf[i] = U1_Rx();

        U1_Tx(rbuf[i]);

        if((rbuf[i] == '\r') || (rbuf[i] == '\n'))
        {
            rbuf[i] = '\0';
            break;
        }

        i++;
    }

    return rbuf;
}
               
