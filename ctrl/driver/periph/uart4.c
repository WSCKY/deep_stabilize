/*
 * uart4.c
 *
 *  Created on: Feb 20, 2020
 *      Author: kychu
 */

#include "uart4.h"

static uint8_t _uart4_init_flag = 0;

static uint32_t in_ptr = 0, out_ptr = 0;
extern uint8_t UART4_RX_CACHE[UART4_RX_CACHE_SIZE];

void uart4_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  if(_uart4_init_flag == 1) return; // already init.

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(UART4_GPIO_CLK, ENABLE);

  /* Connect pin to Periph */
  GPIO_PinAFConfig(UART4_GPIO, UART4_TxPin_AF, UART4_AF);
  GPIO_PinAFConfig(UART4_GPIO, UART4_RxPin_AF, UART4_AF);

  /* Configure UART4 pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = UART4_RxPin | UART4_TxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(UART4_GPIO, &GPIO_InitStructure);

  USART_DeInit(UART4);
  USART_Cmd(UART4, DISABLE);
  USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);

  /* Enable UART4 Clock */
  UART4_CLK_CMD(UART4_CLK, ENABLE);

  /* UART4 configuration -------------------------------------------*/
  /* UART4 configured as follow:
        - BaudRate = 115200
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive && Transmit enabled
  */

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* Configure UART4 */
  USART_Init(UART4, &USART_InitStructure);

  /* Enable the UART4 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = UART4_RX_INT_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the UART4 Receive Interrupt */
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

  /* Enable UART4 */
  USART_Cmd(UART4, ENABLE);

  _uart4_init_flag = 1;
}

void uart4_TxByte(uint8_t c)
{
  UART4->TDR = (c & (uint16_t)0x01FF);
  while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET) {}
}

void uart4_TxBytes(uint8_t *p, uint32_t l)
{
  while(l --) {
    UART4->TDR = ((*p) & (uint16_t)0x01FF);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET) {}
    p ++;
  }
}

uint8_t uart4_pullByte(uint8_t *p)
{
  if(out_ptr != in_ptr) {
    *p = UART4_RX_CACHE[out_ptr];
    if(++ out_ptr == UART4_RX_CACHE_SIZE) {
      out_ptr = 0;
    }
    return 1;
  }
  return 0;
}

uint32_t uart4_pullBytes(uint8_t *p, uint32_t l)
{
  uint32_t len = 0, cnt;
  if(out_ptr == in_ptr) return len;
  if(in_ptr > out_ptr) {
    len = in_ptr - out_ptr;
  } else {
    len = UART4_RX_CACHE_SIZE - out_ptr + in_ptr;
  }
  if(len > l) len = l;
  cnt = len;
  while(cnt --) {
    *p = UART4_RX_CACHE[out_ptr];
    p ++;
    if(++ out_ptr == UART4_RX_CACHE_SIZE) {
      out_ptr = 0;
    }
  }
  return len;
}

static void uart4_pushByte(uint8_t byte)
{
  UART4_RX_CACHE[in_ptr] = byte;
  /* Calculate current position in buffer */
  if(++ in_ptr == UART4_RX_CACHE_SIZE) {
    in_ptr = 0;
  }
}

void uart4_irq_handler(void)
{
  if(_uart4_init_flag == 0) return;

  USART_ClearFlag(UART4, USART_FLAG_ORE | USART_FLAG_PE);
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) {
    uart4_pushByte(UART4->RDR & 0xFF);
    USART_ClearFlag(UART4, USART_FLAG_RXNE);
    USART_ClearITPendingBit(UART4, USART_IT_RXNE);
  }
}
