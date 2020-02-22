/*
 * uart1.c
 *
 *  Created on: Feb 20, 2020
 *      Author: kychu
 */

#include "uart1.h"

#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

static uint8_t _uart1_init_flag = 0;

static uint32_t subscribe_number = 0;
#if FREERTOS_ENABLED
static osSemaphoreId u1_semaphore;
#endif /* FREERTOS_ENABLED */

static uint32_t in_ptr = 0, out_ptr = 0;
#if FREERTOS_ENABLED
uint8_t *UART1_RX_CACHE;
#else
uint8_t UART1_RX_CACHE[UART1_RX_CACHE_SIZE];
#endif /* FREERTOS_ENABLED */

status_t uart1_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
/*  NVIC_InitTypeDef NVIC_InitStructure; */

  if(_uart1_init_flag == 1) return status_ok; // already initialized.

#if FREERTOS_ENABLED
  UART1_RX_CACHE = kmm_alloc(UART1_RX_CACHE_SIZE);
  if(UART1_RX_CACHE == NULL) return status_nomem;
  /* Define used semaphore */
  osSemaphoreDef(U1_SEM);
  /* Create the semaphore */
  u1_semaphore = osSemaphoreCreate(osSemaphore(U1_SEM) , 1);
  if(u1_semaphore == NULL) return status_error;
#endif /* FREERTOS_ENABLED */

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(UART1_GPIO_CLK, ENABLE);

  /* Connect pin to Periph */
  GPIO_PinAFConfig(UART1_GPIO, UART1_TxPin_AF, UART1_AF);
  GPIO_PinAFConfig(UART1_GPIO, UART1_RxPin_AF, UART1_AF);

  /* Configure UART1 pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = UART1_RxPin | UART1_TxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

  USART_DeInit(UART1);
  USART_Cmd(UART1, DISABLE);
  USART_ITConfig(UART1, USART_IT_RXNE, DISABLE);

  /* Enable UART1 Clock */
  UART1_CLK_CMD(UART1_CLK, ENABLE);

  /* UART1 configuration -------------------------------------------*/
  /* UART1 configured as follow:
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
  /* Configure UART1 */
  USART_Init(UART1, &USART_InitStructure);

  /* ##### ALREADY INITIALIZED IN irq.c ##### */
  /* Enable the UART1 Interrupt */
/*  NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = UART1_RX_INT_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); */

  /* Enable the UART1 Receive Interrupt */
  USART_ITConfig(UART1, USART_IT_RXNE, ENABLE);

  /* Enable UART1 */
  USART_Cmd(UART1, ENABLE);

  _uart1_init_flag = 1;
  return status_ok;
}

void uart1_TxByte(uint8_t c)
{
  UART1->TDR = (c & (uint16_t)0x01FF);
  while(USART_GetFlagStatus(UART1, USART_FLAG_TXE) == RESET) {}
}

void uart1_TxBytes(uint8_t *p, uint32_t l)
{
  while(l --) {
    UART1->TDR = ((*p) & (uint16_t)0x01FF);
    while(USART_GetFlagStatus(UART1, USART_FLAG_TXE) == RESET) {}
    p ++;
  }
}

status_t uart1_waitBytes(uint32_t size, uint32_t timeout)
{
  if(UART1_RX_CACHE_SIZE < size)
    subscribe_number = UART1_RX_CACHE_SIZE;
  else
    subscribe_number = size;
#if FREERTOS_ENABLED
  if(osSemaphoreWait(u1_semaphore , timeout) == osOK) {
    return status_ok;
  }
  return status_timeout;
#else
  // maybe we can do something here ...
  return status_ok;
#endif /* FREERTOS_ENABLED */
}

uint8_t uart1_pullByte(uint8_t *p)
{
  if(out_ptr != in_ptr) {
    *p = UART1_RX_CACHE[out_ptr];
    if(++ out_ptr == UART1_RX_CACHE_SIZE) {
      out_ptr = 0;
    }
    return 1;
  }
  return 0;
}

uint32_t uart1_pullBytes(uint8_t *p, uint32_t l)
{
  uint32_t len = 0, cnt;
  if(out_ptr == in_ptr) return len;
  if(in_ptr > out_ptr) {
    len = in_ptr - out_ptr;
  } else {
    len = UART1_RX_CACHE_SIZE - out_ptr + in_ptr;
  }
  if(len > l) len = l;
  cnt = len;
  while(cnt --) {
    *p = UART1_RX_CACHE[out_ptr];
    p ++;
    if(++ out_ptr == UART1_RX_CACHE_SIZE) {
      out_ptr = 0;
    }
  }
  return len;
}

static void uart1_pushByte(uint8_t byte)
{
  uint32_t len;
  UART1_RX_CACHE[in_ptr] = byte;
  /* Calculate current position in buffer */
  if(++ in_ptr == UART1_RX_CACHE_SIZE) {
    in_ptr = 0;
  }
  if(subscribe_number != 0) {
    if(in_ptr > out_ptr) {
      len = in_ptr - out_ptr;
    } else {
      len = UART1_RX_CACHE_SIZE - out_ptr + in_ptr;
    }
    if(len >= subscribe_number) {
#if FREERTOS_ENABLED
      osSemaphoreRelease(u1_semaphore);
#else
      // callback
#endif /* FREERTOS_ENABLED */
    }
  }
}

void uart1_irq_handler(void)
{
  if(_uart1_init_flag == 0) return;

  USART_ClearFlag(UART1, USART_FLAG_ORE | USART_FLAG_PE);
  if(USART_GetITStatus(UART1, USART_IT_RXNE) != RESET) {
    uart1_pushByte(UART1->RDR & 0xFF);
    USART_ClearFlag(UART1, USART_FLAG_RXNE);
    USART_ClearITPendingBit(UART1, USART_IT_RXNE);
  }
}
