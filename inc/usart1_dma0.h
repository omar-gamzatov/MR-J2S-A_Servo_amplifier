#ifndef USART1_H
#define USART1_H

#include "gd32e50x.h"
#include <stdio.h>
#include <string.h>

void usart1_init(uint32_t);
void usart1_dma0_send(char*, uint16_t);
void usart1_dma0_txinit(char*, uint16_t);
void usart1_dma0_rxinit(char*, uint16_t);
void usart1_dma0_rxdata(uint16_t response_size);
void dma_nvic_config(void);	

#endif /* USART1_H */
