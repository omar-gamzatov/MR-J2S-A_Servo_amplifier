#include "usart1_dma0.h"

uint32_t index = 0;

void usart1_init(uint32_t baudrate)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    
     /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* configure USART Tx as alternate function push-pull */
		gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
  
    /* configure USART Rx as alternate function push-pull */
		gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    /* USART configure */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, baudrate);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
		usart_parity_config(USART1, USART_PM_EVEN);
		usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_enable(USART1);
}

void usart1_dma0_send(char* buffer, uint16_t len)
{
    dma_channel_disable(DMA0, DMA_CH6);
    
    dma_memory_address_config(DMA0, DMA_CH6, (uint32_t)buffer);
    dma_transfer_number_config(DMA0, DMA_CH6, len);
    
    /* enable DMA channel1 */
    dma_channel_enable(DMA0, DMA_CH6);
    /* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_ENABLE);
    /* wait DMA Channel transfer complete */
    //while(RESET == dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF));
}

void usart1_dma0_txinit(char* txbuffer, uint16_t len)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    
    /* deinitialize DMA channel1 */
    dma_deinit(DMA0, DMA_CH6);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)txbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = len;
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART1);
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH6, &dma_init_struct);
    
    /* configure DMA mode */
		dma_interrupt_enable(DMA0, DMA_CH6, DMA_INT_FTF);
    dma_circulation_disable(DMA0, DMA_CH6);
    dma_memory_to_memory_disable(DMA0, DMA_CH6);
}

void usart1_dma0_rxinit(char* rxbuffer, uint16_t len)
{
    dma_parameter_struct dma_init_struct;
    /* deinitialize DMA channel2 */
    dma_deinit(DMA0, DMA_CH5);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)rxbuffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = len;
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART1);;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH5, &dma_init_struct);
    
    dma_circulation_disable(DMA0, DMA_CH5);
    dma_memory_to_memory_disable(DMA0, DMA_CH5);

    /* enable DMA channel2 transfer complete interrupt */
    dma_interrupt_enable(DMA0, DMA_CH5, DMA_INT_FTF);
    usart_dma_receive_config(USART1, USART_RECEIVE_DMA_ENABLE);
    dma_channel_enable(DMA0, DMA_CH5);
}

void usart1_dma0_rxdata(uint16_t response_size)
{
    dma_channel_disable(DMA0, DMA_CH5);
    dma_flag_clear(DMA0, DMA_CH5, DMA_FLAG_G);
    //index = buffer_len - dma_transfer_number_get(DMA0, DMA_CH5);
    
    dma_transfer_number_config(DMA0, DMA_CH5, response_size);
    dma_channel_enable(DMA0, DMA_CH5);
    
    //if( index == 0 )
    //    return 0;
    //else
    //    return 1;
}

void dma_nvic_config(void)
{
    /*configure DMA0_CH3 interrupt*/
    nvic_irq_enable(DMA0_Channel6_IRQn, 0U, 0U);
    /*configure DMA0_CH4 interrupt*/
    nvic_irq_enable(DMA0_Channel5_IRQn, 2U, 0U);
}
