#include <string.h>
#include "functiondeclaration.h"
#include "component_defines.h"

/* RFID Card Numbers */
#define OFFICER_ID   "12548413"
#define VOTER1_ID    "12543380"
#define VOTER2_ID    "00354731"
#define VOTER3_ID    "00352530"

u8 Authenticate(u8 *cardID)
{
    if(strcmp((char *)cardID, OFFICER_ID) == 0)
        return OFFICER;

    else if(strcmp((char *)cardID, VOTER1_ID) == 0)
        return VOTER1;

    else if(strcmp((char *)cardID, VOTER2_ID) == 0)
        return VOTER2;

    else if(strcmp((char *)cardID, VOTER3_ID) == 0)
        return VOTER3;

    else
        return INVALID_CARD;
}
