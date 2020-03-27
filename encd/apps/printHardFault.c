/*
 * printHardFault.c
 *
 *  Created on: Mar 8, 2019
 *      Author: kychu
 */

#include "ifconfig.h"

/* Private functions ---------------------------------------------------------*/

void PutHex(unsigned int din)
{
  unsigned int nmask = 0xF0000000U;
  unsigned int nshift = 28;
  unsigned short int data4bit;
  do {
    data4bit = (din & nmask) >> nshift;
    data4bit = data4bit + 48;
    if(data4bit > 57) data4bit = data4bit + 7;
    LOG_PORT_TXCHAR((uint8_t)data4bit);
    nshift = nshift - 4;
    nmask = nmask >> 4;
  } while(nmask != 0);
}

void hard_fault_handler_c(unsigned int * hf_args)
{
  /* R0   = hf_args[0]
     R1   = hf_args[1]
     R2   = hf_args[2]
     R3   = hf_args[3]
     R12  = hf_args[4]
     LR   = hf_args[5]
     PC   = hf_args[6]
     xPSR = hf_args[7]
  */
  LOG_PORT_OUTPUT("HardFault Handler:\n");

  LOG_PORT_OUTPUT(" - R0 = 0x");
  PutHex(hf_args[0]);
  LOG_PORT_TXCHAR('\n');

  LOG_PORT_OUTPUT(" - R1 = 0x");
  PutHex(hf_args[1]);
  LOG_PORT_TXCHAR('\n');

  LOG_PORT_OUTPUT(" - R2 = 0x");
  PutHex(hf_args[2]);
  LOG_PORT_TXCHAR('\n');

  LOG_PORT_OUTPUT(" - R3 = 0x");
  PutHex(hf_args[3]);
  LOG_PORT_TXCHAR('\n');

  LOG_PORT_OUTPUT(" - R12 = 0x");
  PutHex(hf_args[4]);
  LOG_PORT_TXCHAR('\n');

  LOG_PORT_OUTPUT(" - LR = 0x");
  PutHex(hf_args[5]);
  LOG_PORT_TXCHAR('\n');

  LOG_PORT_OUTPUT(" - PC = 0x");
  PutHex(hf_args[6]);
  LOG_PORT_TXCHAR('\n');

  LOG_PORT_OUTPUT(" - xPSR = 0x");
  PutHex(hf_args[7]);
  LOG_PORT_TXCHAR('\n');

  for(;;);
}
