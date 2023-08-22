/*!
    \file    gd32e50x_it.c
    \brief   interrupt service routines

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32e50x_it.h"
#include "systick.h"
#include "mr_j2s_a_servo.h"

//extern servo_ready_status servo_ready;
extern uint16_t rx_size;
extern servo_func_mode servo_mode;
extern servo_jog_functions jog_func;
extern servo_pos_functions pos_func;
extern uint8_t servo_jog_functions_cnt[];
extern uint8_t servo_pos_functions_cnt[];

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
}

/*!
    \brief      this function handles DMA_Channel6_IRQHandler interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA0_Channel6_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH6, DMA_INT_FLAG_FTF)) {
		//gpio_bit_set(GPIOA, GPIO_PIN_9);
		
        dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_INT_FLAG_G);
    }
}


/*!
    \brief      this function handles DMA_Channel5_IRQHandler interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DMA0_Channel5_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH5, DMA_INT_FLAG_FTF)) {
		//gpio_bit_reset(GPIOA, GPIO_PIN_9);
		
		dma_channel_disable(DMA0, DMA_CH5);
		dma_flag_clear(DMA0, DMA_CH5, DMA_FLAG_G);				
		dma_transfer_number_config(DMA0, DMA_CH5, rx_size);
		dma_channel_enable(DMA0, DMA_CH5);
		dma_interrupt_flag_clear(DMA0, DMA_CH5, DMA_INT_FLAG_G); 
		
		servo_handle_error();
		
		switch (servo_mode) {
			case jog_mode:
				switch (jog_func) {
					case jog_on:
						if (servo_jog_functions_cnt[JOG_ON] != 0) {
							servo_jog_mode_on();
						}
						break;
					case jog_off:
						if (servo_jog_functions_cnt[JOG_OFF] != 0) {
							servo_jog_mode_off();
						}
						break;
					case jog_freq_set:
						if (servo_jog_functions_cnt[JOG_FREQ_SET] != 0) {
							servo_jog_mode_set_freq();
						}
						break;
					case jog_acceleration_time_set:
						if (servo_jog_functions_cnt[JOG_ACCELERATION_TIME_SET] != 0) {
							servo_jog_mode_set_acceleration_time();
						}
						break;
					case jog_direct_rotation:
						if (servo_jog_functions_cnt[JOG_DIRECT_ROT] != 0) {
							servo_jog_mode_direct_rotation();
						}
						break;
					case jog_reverse_rotation:
						if (servo_jog_functions_cnt[JOG_REVERSE_ROT] != 0) {
							servo_jog_mode_revers_rotation();
						}
						break;
					case jog_stop:
						if (servo_jog_functions_cnt[JOG_STOP] != 0) {
							servo_jog_mode_stop_rotation();
						}
						break;
						break;
				};
				break;
			case pos_mode:
				switch (pos_func) {
					case pos_on:
						if (servo_pos_functions_cnt[POS_ON] != 0) {
							servo_jog_mode_on();
						}
						break;
					case pos_off:
						if (servo_pos_functions_cnt[POS_OFF] != 0) {
							servo_jog_mode_on();
						}
						break;
					case pos_freq_set:
						if (servo_pos_functions_cnt[POS_FREQ_SET] != 0) {
							servo_jog_mode_on();
						}
						break;
					case pos_acceleration_time_set:
						if (servo_pos_functions_cnt[POS_ACCELERATION_TIME_SET] != 0) {
							servo_jog_mode_on();
						}
						break;
					case pos_break:
						if (servo_pos_functions_cnt[POS_PATH_LENGTH] != 0) {
							servo_jog_mode_on();
						}
						break;
					case pos_path_length:
						if (servo_pos_functions_cnt[POS_BREAK] != 0) {
							servo_jog_mode_on();
						}
						break;
				}
				break;
			case timer_mode:
				servo_send_read_command(READ_STATE, DATA_FEEDBACK_IMPULSES, RESPONSE_SIZE_STATE, 0);
				gpio_bit_reset(GPIOA, GPIO_PIN_8);
				break;
			case nothing_mode:
				break;
			default:
				break;
		};
    }
}

/*!
    \brief      this function handles TIMER2 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_UP)) {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);
		if (servo_mode == timer_mode) {
			gpio_bit_set(GPIOA, GPIO_PIN_8);
		}
		else if (servo_mode == nothing_mode) {
			servo_mode = timer_mode;
			gpio_bit_set(GPIOA, GPIO_PIN_8);
			servo_send_read_command(READ_STATE, DATA_FEEDBACK_IMPULSES, RESPONSE_SIZE_STATE, 0);
			gpio_bit_reset(GPIOA, GPIO_PIN_8);
		}
    }
}
