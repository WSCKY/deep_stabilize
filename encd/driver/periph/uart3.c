/*
 * uart3.c
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#include "uart3.h"

#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

static uint8_t _uart3_init_flag = 0;
#if UART3_DMA_ENABLE
static uint32_t _tx_comp_flag = 1;

static uint32_t in_ptr = 0, out_ptr = 0;
#if FREERTOS_ENABLED
uint8_t *UART3_RX_CACHE;
#else
uint8_t UART3_RX_CACHE[UART3_RX_CACHE_SIZE];
#endif /* FREERTOS_ENABLED */

static void dma_config(void);
#else
static PortRecvByteCallback pCallback = 0;
#endif /* UART3_DMA_ENABLE */

status_t uart3_init(
#if UART3_DMA_ENABLE
		void
#else
		PortRecvByteCallback p
#endif
		)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
/*	NVIC_InitTypeDef NVIC_InitStructure; */

	if(_uart3_init_flag == 1) return status_ok; // already init.
#if !UART3_DMA_ENABLE
	if(p != 0) {
		pCallback = p;
	}
#endif

#if FREERTOS_ENABLED
  UART3_RX_CACHE = kmm_alloc(UART3_RX_CACHE_SIZE);
  if(UART3_RX_CACHE == NULL) return status_nomem;
#endif /* FREERTOS_ENABLED */

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(UART3_GPIO_CLK, ENABLE);

	/* Connect pin to Periph */
	GPIO_PinAFConfig(UART3_GPIO, UART3_TxPin_AF, UART3_AF);
	GPIO_PinAFConfig(UART3_GPIO, UART3_RxPin_AF, UART3_AF);

	/* Configure UART3 pins as AF pushpull */
	GPIO_InitStructure.GPIO_Pin = UART3_RxPin | UART3_TxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);

	USART_DeInit(UART3);
	USART_Cmd(UART3, DISABLE);
	USART_ITConfig(UART3, USART_IT_RXNE, DISABLE);

	/* Enable UART3 Clock */
	UART3_CLK_CMD(UART3_CLK, ENABLE);

#if UART3_DMA_ENABLE
	dma_config();
#endif /* UART3_DMA_ENABLE */
	/* UART3 configuration -------------------------------------------*/
  /* UART3 configured as follow:
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
	/* Configure UART3 */
	USART_Init(UART3, &USART_InitStructure);

	/* ##### ALREADY INITIALIZED IN irq.c ##### */
	/* Enable the UART3 Interrupt */
/*	NVIC_InitStructure.NVIC_IRQChannel = UART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = UART3_RX_INT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); */

#if UART3_DMA_ENABLE
	/* Enable the UART3 Idle line detection interrupt. */
	USART_ITConfig(UART3, USART_IT_IDLE, ENABLE);
	/* Enable the UART3 Tx DMA requests */
	USART_DMACmd(UART3, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
#else
	/* Enable the UART3 Receive Interrupt */
	USART_ITConfig(UART3, USART_IT_RXNE, ENABLE);
#endif /* UART3_DMA_ENABLE */
	/* Enable UART3 */
	USART_Cmd(UART3, ENABLE);

	_uart3_init_flag = 1;
	return status_ok;
}
#if UART3_DMA_ENABLE
static void dma_config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
/*	NVIC_InitTypeDef NVIC_InitStructure; */

	/* Enable UART3_DMA Clock */
	UART3_DMA_CLK_CMD(UART3_DMA_CLK, ENABLE);

	DMA_DeInit(UART3_TX_DMA);
	DMA_Cmd(UART3_TX_DMA, DISABLE);
	DMA_DeInit(UART3_RX_DMA);
	DMA_Cmd(UART3_RX_DMA, DISABLE);

	/* UART3_RX_DMA configuration */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_InitStructure.DMA_BufferSize = UART3_RX_CACHE_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART3_RX_CACHE;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART3->RDR);
	DMA_Init(UART3_RX_DMA, &DMA_InitStructure);

	/* UART3_TX_DMA configuration */
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_MemoryBaseAddr = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART3->TDR);
	DMA_Init(UART3_TX_DMA, &DMA_InitStructure);

	/* Enable the UART3_TX_DMA TC Interrupt */
	DMA_ITConfig(UART3_TX_DMA, DMA_IT_TC, ENABLE);
	/* Enable the UART3_RX_DMA HT/TC Interrupt */
	DMA_ITConfig(UART3_RX_DMA, DMA_IT_HT, ENABLE);
	DMA_ITConfig(UART3_RX_DMA, DMA_IT_TC, ENABLE);

/*	NVIC_InitStructure.NVIC_IRQChannel = UART3_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = UART3_DMA_INT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); */

	/* Enable the UART3_TX_DMA channels */
//	DMA_Cmd(UART3_TX_DMA, ENABLE);
	/* Enable the UART3_RX_DMA channels */
	DMA_Cmd(UART3_RX_DMA, ENABLE);
}
#endif /* UART3_DMA_ENABLE */
void uart3_TxByte(uint8_t c)
{
	UART3->TDR = (c & (uint16_t)0x01FF);
	while(USART_GetFlagStatus(UART3, USART_FLAG_TXE) == RESET) {}
}

void uart3_TxBytes(uint8_t *p, uint32_t l)
{
	while(l --) {
		uart3_TxByte(*p ++);
	}
}
#if UART3_DMA_ENABLE
status_t uart3_TxBytesDMA(uint8_t *p, uint32_t l)
{
  if(_tx_comp_flag == 1) {
    _tx_comp_flag = 0;
    UART3_TX_DMA->CNDTR = l;
    UART3_TX_DMA->CMAR = (uint32_t)p;

    /* Enable the UART3_DMA channels */
    UART3_TX_DMA->CCR |= DMA_CCR_EN;
    return status_ok;
  }
  return status_busy;
}

uint8_t uart3_pullByte(uint8_t *p)
{
	if(out_ptr != in_ptr) {
		*p = UART3_RX_CACHE[out_ptr];
		if(++ out_ptr == UART3_RX_CACHE_SIZE) {
			out_ptr = 0;
		}
		return 1;
	}
	return 0;
}

uint32_t uart3_pullBytes(uint8_t *p, uint32_t l)
{
	uint32_t len = 0, cnt;
	if(out_ptr == in_ptr) return len;
	if(in_ptr > out_ptr) {
		len = in_ptr - out_ptr;
	} else {
		len = UART3_RX_CACHE_SIZE - out_ptr + in_ptr;
	}
	if(len > l) len = l;
	cnt = len;
	while(cnt --) {
		*p = UART3_RX_CACHE[out_ptr];
		p ++;
		if(++ out_ptr == UART3_RX_CACHE_SIZE) {
			out_ptr = 0;
		}
	}
	return len;
}

static void uart3_pushBytes(void)
{
	/* Calculate current position in buffer */
	in_ptr = UART3_RX_CACHE_SIZE - UART3_RX_DMA->CNDTR;
}
#else
void uart3_set_callback(PortRecvByteCallback p)
{
	if(p != 0) {
		pCallback = p;
	}
}
#endif /* UART3_DMA_ENABLE */

#if UART3_DMA_ENABLE
void uart3_dma_irq_handler(void)
{
  if(_uart3_init_flag == 0) return;
  /* check if transfer complete flag is set. */
  if(DMA_GetITStatus(UART3_TX_DMA_IT_TC_FLAG)) {
    DMA_Cmd(UART3_TX_DMA, DISABLE);
    _tx_comp_flag = 1;
    DMA_ClearITPendingBit(UART3_TX_DMA_IT_TC_FLAG);
  }
  /* check if receive half complete flag is set. */
  if(DMA_GetITStatus(UART3_RX_DMA_IT_HT_FLAG)) {
    uart3_pushBytes();
    DMA_ClearITPendingBit(UART3_RX_DMA_IT_HT_FLAG);
  }
  /* check if receive complete flag is set. */
  if(DMA_GetITStatus(UART3_RX_DMA_IT_TC_FLAG)) {
    uart3_pushBytes();
    DMA_ClearITPendingBit(UART3_RX_DMA_IT_TC_FLAG);
  }
}
#endif /* UART3_DMA_ENABLE */

void uart3_irq_handler(void)
{
  if(_uart3_init_flag == 0) return;
#if UART3_DMA_ENABLE
  if(USART_GetITStatus(UART3, USART_IT_IDLE) != RESET) {
    uart3_pushBytes();
    USART_ClearFlag(UART3, USART_FLAG_IDLE);
    USART_ClearITPendingBit(UART3, USART_IT_IDLE);
  }
#else
  USART_ClearFlag(UART3, USART_FLAG_ORE | USART_FLAG_PE);
  if(USART_GetITStatus(UART3, USART_IT_RXNE) != RESET) {
    if(pCallback != 0) {
      pCallback(UART3->RDR & 0xFF);
    }
    USART_ClearFlag(UART3, USART_FLAG_RXNE);
    USART_ClearITPendingBit(UART3, USART_IT_RXNE);
  }
#endif /* UART3_DMA_ENABLE */
}
