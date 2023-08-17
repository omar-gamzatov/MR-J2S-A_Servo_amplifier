#include "gd32e50x.h"
#include "systick.h"
#include "gd32e50x_gpio.h"
#include "gd32e50x_dma.h"
#include "stdio.h"
#include "mr_j2s_a_servo.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
		rcu_periph_clock_enable(RCU_GPIOA);
		rcu_periph_clock_enable(RCU_GPIOC);
				/* configure LED GPIO port */ 
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_0);
    gpio_bit_reset(GPIOA, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_0);
	
	  gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
    gpio_bit_set(GPIOC, GPIO_PIN_0 | GPIO_PIN_1);
	
		servo_init(9600);
	
		servo_jog_mode_on("00FF", "00000005");

	
		while(1){
				
				if (gpio_input_bit_get(GPIOA, GPIO_PIN_0))
				{
						gpio_bit_set(GPIOA, GPIO_PIN_7);
						while (gpio_input_bit_get(GPIOA, GPIO_PIN_0));
						gpio_bit_reset(GPIOA, GPIO_PIN_7);
						servo_jog_mode_off();
				}
				
				if (!gpio_input_bit_get(GPIOC, GPIO_PIN_0))
				{
						gpio_bit_set(GPIOA, GPIO_PIN_10);
						servo_send_write_command(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_DIRECT_ROTATION, 0);
						while (!gpio_input_bit_get(GPIOC, GPIO_PIN_0));
						servo_send_write_command(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_STOP_ROTATION, 0);
						gpio_bit_reset(GPIOA, GPIO_PIN_10);
				}
				
				if (!gpio_input_bit_get(GPIOC, GPIO_PIN_1))
				{
						gpio_bit_set(GPIOA, GPIO_PIN_9);
						servo_send_write_command(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_REVERSE_ROTATION, 0);
						while (!gpio_input_bit_get(GPIOC, GPIO_PIN_1));
						servo_send_write_command(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_STOP_ROTATION, 0);
						gpio_bit_reset(GPIOA, GPIO_PIN_9);
				}
    }
}
