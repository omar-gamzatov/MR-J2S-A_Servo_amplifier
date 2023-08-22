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
	
	servo_init(57600);
	servo_set_freq(0x00ff);
	servo_set_acceleration_time(0x00000200);
	servo_jog_mode_on();

	//servo_timer_enable();
		
	while(1) {
		
		if (gpio_input_bit_get(GPIOA, GPIO_PIN_0)) {
			gpio_bit_set(GPIOA, GPIO_PIN_7);
			while (gpio_input_bit_get(GPIOA, GPIO_PIN_0));
			gpio_bit_reset(GPIOA, GPIO_PIN_7);
			servo_jog_mode_off();
		}
		
		if (!gpio_input_bit_get(GPIOC, GPIO_PIN_0))	{
			gpio_bit_set(GPIOA, GPIO_PIN_10);
			servo_set_freq(120);
			servo_set_acceleration_time(0x500);
			servo_jog_mode_set_freq();
			servo_jog_mode_set_acceleration_time();
			servo_jog_mode_direct_rotation();
			while (!gpio_input_bit_get(GPIOC, GPIO_PIN_0));
			servo_jog_mode_stop_rotation();
			gpio_bit_reset(GPIOA, GPIO_PIN_10);
		}
		
		if (!gpio_input_bit_get(GPIOC, GPIO_PIN_1))	{
			gpio_bit_set(GPIOA, GPIO_PIN_9);
			servo_set_freq(60);
			servo_set_acceleration_time(0x500);
			servo_jog_mode_set_freq();
			servo_jog_mode_set_acceleration_time();
			servo_jog_mode_revers_rotation();
			while (!gpio_input_bit_get(GPIOC, GPIO_PIN_1));
			servo_jog_mode_stop_rotation();
			gpio_bit_reset(GPIOA, GPIO_PIN_9);
		}
    }
}
