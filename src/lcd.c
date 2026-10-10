//lcd.c

#include <LPC21XX.h>

#include "functiondeclaration.h"
#include "component_defines.h"

void WriteLCD(u8 dat)

{

	IOCLR0=1<<RW;

	IOPIN0=(IOPIN0&~(255<<LCD_DATA))|(dat<<LCD_DATA);

	IOSET0=1<<EN;

	dly_us(1);

	IOCLR0=1<<EN;

	dly_ms(2);

}

void CmdLCD(u8 cmd)

{

	IOCLR0=1<<RS;

	WriteLCD(cmd);

}

void InitLCD(void)

{

	IODIR0|=255<<LCD_DATA;

	IODIR0|=7<<RS;

	

	/*delay_ms(15);

	Cmd_LCD(0x30);

	delay_ms(4);

	delay_us(100);

	Cmd_LCD(0x30);

	delay_us(100);

	Cmd_LCD(0x30);

	Cmd_LCD(0x38);

	Cmd_LCD(0x0C);

	Cmd_LCD(0x01);

	Cmd_LCD(0x06);

	*/

	dly_ms(15);

	CmdLCD(0x30);

	dly_ms(4);

	dly_us(100);

	CmdLCD(0x30);

	dly_us(100);

	CmdLCD(0x30);

	CmdLCD(MODE_8BIT_2LINE);//0x38

	CmdLCD(DSP_ON_CUR_OFF);//0x0c

	CmdLCD(CLEAR_LCD);//0x01

	CmdLCD(SHIFT_CUR_RIGHT);//0x06

}

void CharLCD(u8 asciiVal)

{

	//set rs pin for data register select

	IOSET0=1<<RS;

	//write to ddram via data reg via data pins

	WriteLCD(asciiVal);

}

void StrLCD(s8* str)

{

		while(*str)

		CharLCD(*str++);

}

void U32LCD(u32 num)

{

	u32 a[10];

  s32 i=0;

  if(num==0)

  {

		CharLCD('0');

  }		

	else

	{

		while(num>0)

		{

			a[i++]=(num%10);

			num/=10;

		}

		for(--i;i>=0;i--)

		 CharLCD(a[i]+48);

	}

	

}

void S32LCD(s32 num)

{

		if(num<0)

	{

		CharLCD('-');

		num=-num;

	}

	U32LCD(num);

	

}


void F32LCD(f32 fn,u32 ndp)

{

	u32 n,i;

	if(fn<0.0)

	{

		CharLCD('-');

		fn=-fn;

	}

	n=fn;

	U32LCD(n);

	CharLCD('.');

	for(i=0;i<ndp;i++)

	{

		fn=(fn-n)*10;

		n=fn;

		CharLCD(n+48);

	}

	

	

}


void BuildCGRAM(s8*p,u32 n)

{

	u32 i;

	CmdLCD(0x40); // CGRAM ADDRESS

	for(i=0;i<n;i++)

	{

		IOSET0=1<<RS;

		WriteLCD(p[i]);

	}

	CmdLCD(0x80); // DDRAM ADDRESS

}

void HEXLCD(u32 n)

{

	u8 a[10];

	s32 i=0,digit;

	while(n)

	{

		digit=n%16;

		(digit<10)?(digit+=48):(digit+=55);

		a[i++]=digit;

		n/=16;

	}

	for(--i;i>=0;i--)

		CharLCD(a[i]);

}


void OCTLCD(u32 n)

{

	u8 a[10];

	s32 i=0;

	while(n)

	{

		a[i++]=n%8+48;

		n/=8;

	}

	for(--i;i>=0;i--)

		CharLCD(a[i]);

}