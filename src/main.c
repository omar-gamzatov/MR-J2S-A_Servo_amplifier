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
				/* configure LED GPIO port */ 
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_0);
    gpio_bit_reset(GPIOA, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_0);
		
		servo_init(9600);
	
		//servo_jog_mode_on("00FF", "000003E8");
	
		while(1){
				if (gpio_input_bit_get(GPIOA, GPIO_PIN_0))
				{
						gpio_bit_set(GPIOA, GPIO_PIN_10);
						while (gpio_input_bit_get(GPIOA, GPIO_PIN_0));
						gpio_bit_reset(GPIOA, GPIO_PIN_10);
						//servo_jog_mode_off();
					
		servo_send_read_command(READ_STATE, DATA_FEEDBACK_IMPULSES, RESPONSE_SIZE_STATE, 0);
						//servo_send_read_command(READ_STATE, DATA_FEEDBACK_IMPULSES, RESPONSE_SIZE_STATE, 0);
						//servo_send_write_command(WRITE_PARAMS, 0x00, "00000002", 0);
						//servo_send_write_command(WRITE_PARAMS, 0x00, "00000003", 0);

						
				}
    }
}
