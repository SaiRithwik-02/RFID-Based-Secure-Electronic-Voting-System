#include "functiondeclaration.h"
void dly_us(u32 dlyUs)
{
dlyUs*=12;
while(dlyUs--);
}

void dly_ms(u32 dlyMs)
{
dlyMs*=12000;
while(dlyMs--);
} 

void dly_s(u32 dlyS)
{
dlyS*=12000000;
while(dlyS--);

}