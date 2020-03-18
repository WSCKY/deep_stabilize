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

#if UART1_DMA_ENABLE
static uint32_t _tx_comp_flag = 1;
#if FREERTOS_ENABLED
/* Define used semaphore */
osSemaphoreDef(U1_TX_SEM);
static osSemaphoreId u1_tx_semaphore;
#endif /* FREERTOS_ENABLED */
#endif /* UART1_DMA_ENABLE */

static uint32_t subscribe_number = 0;
#if FREERTOS_ENABLED
/* Define used semaphore */
osSemaphoreDef(U1_RX_SEM);
static osSemaphoreId u1_rx_semaphore;
#endif /* FREERTOS_ENABLED */

static uint32_t in_ptr = 0, out_ptr = 0;
#if FREERTOS_ENABLED
uint8_t *UART1_RX_CACHE;
#else
uint8_t UART1_RX_CACHE[UART1_RX_CACHE_SIZE];
#endif /* FREERTOS_ENABLED */

#if UART1_DMA_ENABLE
static void uart1_dma_config(void);
#endif /* UART1_DMA_ENABLE */

status_t uart1_init(uint32_t baudrate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
/*  NVIC_InitTypeDef NVIC_InitStructure; */

  if(_uart1_init_flag == 1) return status_ok; // already initialized.

#if FREERTOS_ENABLED
  UART1_RX_CACHE = kmm_alloc(UART1_RX_CACHE_SIZE);
  if(UART1_RX_CACHE == NULL) return status_nomem;
#if UART1_DMA_ENABLE
  /* Create the semaphore */
  u1_tx_semaphore = osSemaphoreCreate(osSemaphore(U1_TX_SEM) , 1);
  if(u1_tx_semaphore == NULL) return status_error;
  osSemaphoreWait(u1_tx_semaphore, 0);
#endif /* UART1_DMA_ENABLE */
  /* Create the semaphore */
  u1_rx_semaphore = osSemaphoreCreate(osSemaphore(U1_RX_SEM) , 1);
  if(u1_rx_semaphore == NULL) return status_error;
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

#if UART1_DMA_ENABLE
  uart1_dma_config();
#endif /* UART1_DMA_ENABLE */

  /* UART1 configuration -------------------------------------------*/
  /* UART1 configured as follow:
        - BaudRate = baudrate
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive && Transmit enabled
  */

  USART_InitStructure.USART_BaudRate = baudrate;
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

#if UART1_DMA_ENABLE
  /* Enable the UART1 Idle line detection interrupt. */
  USART_ITConfig(UART1, USART_IT_IDLE, ENABLE);
  /* Enable the UART1 Tx DMA requests */
  USART_DMACmd(UART1, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
#else
  /* Enable the UART1 Receive Interrupt */
  USART_ITConfig(UART1, USART_IT_RXNE, ENABLE);
#endif /* UART1_DMA_ENABLE */

  /* Enable UART1 */
  USART_Cmd(UART1, ENABLE);

  _uart1_init_flag = 1;
  return status_ok;
}

#if UART1_DMA_ENABLE
static void uart1_dma_config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
/*  NVIC_InitTypeDef NVIC_InitStructure; */

#if CONFIG_USE_BOARD_IMU
  SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART1Rx, ENABLE);
  SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_USART1Tx, ENABLE);
#endif /* CONFIG_USE_BOARD_IMU */

  /* Enable UART1_DMA Clock */
  UART1_DMA_CLK_CMD(UART1_DMA_CLK, ENABLE);

  DMA_DeInit(UART1_TX_DMA);
  DMA_Cmd(UART1_TX_DMA, DISABLE);
  DMA_DeInit(UART1_RX_DMA);
  DMA_Cmd(UART1_RX_DMA, DISABLE);

  /* UART1_RX_DMA configuration */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_InitStructure.DMA_BufferSize = UART1_RX_CACHE_SIZE;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART1_RX_CACHE;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_PeripheralBaseAddr = UART1_RDR_ADDRESS;
  DMA_Init(UART1_RX_DMA, &DMA_InitStructure);

  /* UART1_TX_DMA configuration */
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_MemoryBaseAddr = 0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_PeripheralBaseAddr = UART1_TDR_ADDRESS;
  DMA_Init(UART1_TX_DMA, &DMA_InitStructure);

  /* Enable the UART1_TX_DMA TC Interrupt */
  DMA_ITConfig(UART1_TX_DMA, DMA_IT_TC, ENABLE);
  /* Enable the UART1_RX_DMA HT/TC Interrupt */
  DMA_ITConfig(UART1_RX_DMA, DMA_IT_HT, ENABLE);
  DMA_ITConfig(UART1_RX_DMA, DMA_IT_TC, ENABLE);

/*  NVIC_InitStructure.NVIC_IRQChannel = UART1_DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = UART1_DMA_INT_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); */

  /* Enable the UART1_TX_DMA channels */
//  DMA_Cmd(UART1_TX_DMA, ENABLE);
  /* Enable the UART1_RX_DMA channels */
  DMA_Cmd(UART1_RX_DMA, ENABLE);
}
#endif /* UART1_DMA_ENABLE */

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

#if UART1_DMA_ENABLE
status_t uart1_TxBytesDMA(uint8_t *p, uint32_t l)
{
  if(_tx_comp_flag == 1) {
    _tx_comp_flag = 0;
    UART1_TX_DMA->CNDTR = l;
    UART1_TX_DMA->CMAR = (uint32_t)p;

    /* Enable the UART1_DMA channels */
    UART1_TX_DMA->CCR |= DMA_CCR_EN;
    return status_ok;
  }
  return status_busy;
}

#if FREERTOS_ENABLED
status_t uart1_TxBytesDMA_Block(uint8_t *p, uint32_t l, uint32_t timeout)
{
  if(_tx_comp_flag == 1) {
    _tx_comp_flag = 0;
    UART1_TX_DMA->CNDTR = l;
    UART1_TX_DMA->CMAR = (uint32_t)p;

    /* Enable the UART1_DMA channels */
    UART1_TX_DMA->CCR |= DMA_CCR_EN;
//    osSemaphoreWait(u1_tx_semaphore, 0);
    if(osSemaphoreWait(u1_tx_semaphore, timeout) == osOK)
      return status_ok;
    else
      return status_error;
  }
  return status_busy;
}
#endif /* FREERTOS_ENABLED */
#endif /* UART1_DMA_ENABLE */

status_t uart1_waitBytes(uint32_t size, uint32_t timeout)
{
  if(UART1_RX_CACHE_SIZE < size)
    subscribe_number = UART1_RX_CACHE_SIZE;
  else
    subscribe_number = size;
#if FREERTOS_ENABLED
  if(osSemaphoreWait(u1_rx_semaphore , timeout) == osOK) {
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

#if FREERTOS_ENABLED
status_t uart1_read_block(uint8_t *p, uint32_t l, uint32_t timeout)
{
  uint32_t num_rd;
  uint32_t ts_now, ts_start;
  ts_start = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);

read:
  num_rd = uart1_pullBytes(p, l);
  p += num_rd;
  if(num_rd < l) {
    l -= num_rd;
    if(UART1_RX_CACHE_SIZE < l) subscribe_number = UART1_RX_CACHE_SIZE;
    else subscribe_number = l;
    ts_now = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
    if((ts_now - ts_start) < timeout) {
      if(osSemaphoreWait(u1_rx_semaphore , timeout + ts_start - ts_now) == osOK) {
        goto read;
      }
    }
    return status_timeout;
  }
  return status_ok;
}
#endif /* FREERTOS_ENABLED */

#if UART1_DMA_ENABLE
static void uart1_pushBytes(void)
{
  uint32_t len;
  /* Calculate current position in buffer */
  in_ptr = UART1_RX_CACHE_SIZE - UART1_RX_DMA->CNDTR;
  if(subscribe_number != 0) {
    if(in_ptr > out_ptr) {
      len = in_ptr - out_ptr;
    } else {
      len = UART1_RX_CACHE_SIZE - out_ptr + in_ptr;
    }
    if(len >= subscribe_number) {
      subscribe_number = 0; // clear flag
#if FREERTOS_ENABLED
      osSemaphoreRelease(u1_rx_semaphore);
#else
      // callback
#endif /* FREERTOS_ENABLED */
    }
  }
}
#else
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
      osSemaphoreRelease(u1_rx_semaphore);
#else
      // callback
#endif /* FREERTOS_ENABLED */
    }
  }
}
#endif /* UART1_DMA_ENABLE */

#if UART1_DMA_ENABLE
void uart1_dma_irq_handler(void)
{
  if(_uart1_init_flag == 0) return;
  /* check if transfer complete flag is set. */
  if(DMA_GetITStatus(UART1_TX_DMA_IT_TC_FLAG)) {
    DMA_Cmd(UART1_TX_DMA, DISABLE);
    _tx_comp_flag = 1;
#if FREERTOS_ENABLED
    osSemaphoreRelease(u1_tx_semaphore);
#endif /* FREERTOS_ENABLED */
    DMA_ClearITPendingBit(UART1_TX_DMA_IT_TC_FLAG);
  }
  /* check if receive half complete flag is set. */
  if(DMA_GetITStatus(UART1_RX_DMA_IT_HT_FLAG)) {
    uart1_pushBytes();
    DMA_ClearITPendingBit(UART1_RX_DMA_IT_HT_FLAG);
  }
  /* check if receive complete flag is set. */
  if(DMA_GetITStatus(UART1_RX_DMA_IT_TC_FLAG)) {
    uart1_pushBytes();
    DMA_ClearITPendingBit(UART1_RX_DMA_IT_TC_FLAG);
  }
}
#endif /* UART1_DMA_ENABLE */

void uart1_irq_handler(void)
{
  if(_uart1_init_flag == 0) return;
#if UART1_DMA_ENABLE
  if(USART_GetITStatus(UART1, USART_IT_IDLE) != RESET) {
    uart1_pushBytes();
    USART_ClearFlag(UART1, USART_FLAG_IDLE);
    USART_ClearITPendingBit(UART1, USART_IT_IDLE);
  }
#else
  USART_ClearFlag(UART1, USART_FLAG_ORE | USART_FLAG_PE);
  if(USART_GetITStatus(UART1, USART_IT_RXNE) != RESET) {
    uart1_pushByte(UART1->RDR & 0xFF);
    USART_ClearFlag(UART1, USART_FLAG_RXNE);
    USART_ClearITPendingBit(UART1, USART_IT_RXNE);
  }
#endif /* UART1_DMA_ENABLE */
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
