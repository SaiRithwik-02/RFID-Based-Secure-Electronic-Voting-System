//UART0
#include <LPC21xx.h>
#include "functiondeclaration.h"
#include "component_defines.h"


void UART0_Init(void)
{
    /* Configure P0.0 and P0.1 as TXD0 and RXD0 */
    PINSEL0 &= ~(0xF);
    PINSEL0 |= TXD0_PIN | RXD0_PIN;

    /* 8-bit, 1 Stop bit, No parity, Enable DLAB */
    U0LCR = (1<<DLAB_BIT) | WORD_LEN;

    /* Baud Rate */
    U0DLL = DIVISOR;
    U0DLM = DIVISOR >> 8;

    /* Disable DLAB */
    U0LCR &= ~(1<<DLAB_BIT);
}

void U0_Tx(u8 sByte)
{
    U0THR = sByte;
    while(((U0LSR >> TEMT_BIT) & 1) == 0);
}

u8 U0_Rx(void)
{
    while(((U0LSR >> DR_BIT) & 1) == 0);
    return U0RBR;
}

void U0_TxStr(s8 *p)
{
    while(*p)
        U0_Tx(*p++);
}

void U0_TxU32(u32 num)
{
    u8 a[10];
    s32 i = 0;

    if(num == 0)
    {
        U0_Tx('0');
    }
    else
    {
        while(num > 0)
        {
            a[i++] = (num % 10) + '0';
            num /= 10;
        }

        while(--i >= 0)
            U0_Tx(a[i]);
    }
}

void U0_TxS32(s32 num)
{
    if(num < 0)
    {
        U0_Tx('-');
        num = -num;
    }

    U0_TxU32(num);
}

void U0_TxF32(f32 fNum, u32 nDP)
{
    u32 num;
    s32 i;

    if(fNum < 0)
    {
        U0_Tx('-');
        fNum = -fNum;
    }

    num = (u32)fNum;

    U0_TxU32(num);
    U0_Tx('.');

    for(i = 0; i < nDP; i++)
    {
        fNum = (fNum - num) * 10;
        num = (u32)fNum;
        U0_Tx(num + '0');
    }
}
s8 *U0_RxStr(void)
{
    static s8 rbuf[100];
    s32 i = 0;

    while(1)
    {
        rbuf[i] = U0_Rx();
        U0_Tx(rbuf[i]);   // Echo

        if(rbuf[i] == '\r' || rbuf[i] == '\n')
        {
            rbuf[i] = '\0';
            break;
        }

        i++;
    }

    return rbuf;
}
