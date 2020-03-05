/*
 * printHardFault.c
 *
 *  Created on: Mar 8, 2019
 *      Author: kychu
 */

#include "uart2.h"

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
    uart2_TxByte((uint8_t)data4bit);
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
  uart2_TxString("HardFault Handler:\n");

  uart2_TxString(" - R0 = 0x");
  PutHex(hf_args[0]);
  uart2_TxByte('\n');

  uart2_TxString(" - R1 = 0x");
  PutHex(hf_args[1]);
  uart2_TxByte('\n');

  uart2_TxString(" - R2 = 0x");
  PutHex(hf_args[2]);
  uart2_TxByte('\n');

  uart2_TxString(" - R3 = 0x");
  PutHex(hf_args[3]);
  uart2_TxByte('\n');

  uart2_TxString(" - R12 = 0x");
  PutHex(hf_args[4]);
  uart2_TxByte('\n');

  uart2_TxString(" - LR = 0x");
  PutHex(hf_args[5]);
  uart2_TxByte('\n');

  uart2_TxString(" - PC = 0x");
  PutHex(hf_args[6]);
  uart2_TxByte('\n');

  uart2_TxString(" - xPSR = 0x");
  PutHex(hf_args[7]);
  uart2_TxByte('\n');

  for(;;);
}
