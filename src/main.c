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
	
	uint8_t button0 = 0;
	uint8_t button1 = 0;
	uint8_t button2 = 0;
	
	servo_init(BAUDRATE_57600);
	
	//servo_set_freq(0x00ff);
	//servo_set_acceleration_time(0x00000500);
	//servo_set_path_length(500000);
		
	while(1) {
		
		if (gpio_input_bit_get(GPIOA, GPIO_PIN_0)) {
			delay_1ms(200);
			servo_check_rd_on();
			//if (button0 == 0) {
			//	button0 = 1;
			//	servo_positioning_mode_on();
			//	gpio_bit_set(GPIOA, GPIO_PIN_10);
			//	
			//	//servo_jog_mode_on();
			//} else {
			//	button0 = 0;
			//	servo_positioning_mode_off();
			//	gpio_bit_reset(GPIOA, GPIO_PIN_10);
			//	
			//	//servo_jog_mode_off();
			//}
			while (gpio_input_bit_get(GPIOA, GPIO_PIN_0));
		}
		
		//if (!gpio_input_bit_get(GPIOC, GPIO_PIN_0))	{
		//	delay_1ms(200);
		//	
		//	// pos mode
		//	if (button1 == 0) {
		//		button1 = 1;
		//		servo_set_freq(200);
		//		servo_set_acceleration_time(1000);
		//		servo_set_path_length(131072);
		//		servo_positioning_mode_config();
		//	} else {
		//		button1 = 0;
		//		servo_set_freq(100);
		//		servo_set_acceleration_time(1000);
		//		servo_set_path_length(-131072);
		//		servo_positioning_mode_config();
		//	}
		//	
		//	//// jog mode
		//	//if (button1 == 0) {
		//	//	button1 = 1;
		//	//	servo_set_freq(0x00ff);
		//	//	servo_set_acceleration_time(0x00000500);
		//	//	gpio_bit_set(GPIOA, GPIO_PIN_10);
		//	//	//servo_jog_mode_direct_rotation();
		//	//} else {
		//	//	button1 = 0;
		//	//	//servo_jog_mode_stop_rotation();
		//	//	gpio_bit_reset(GPIOA, GPIO_PIN_10);
		//	//}
		//	
		//	while (!gpio_input_bit_get(GPIOC, GPIO_PIN_0));
		//}
		//
		//if (!gpio_input_bit_get(GPIOC, GPIO_PIN_1))	{
		//	delay_1ms(200);
		//	
		//	// pos mode
		//	servo_positioning_mode_path_length();
		//	
		//	//// jog mode
		//	//if (button2 == 0) {
		//	//	button2 = 1;
		//	//	servo_set_freq(0x015E);
		//	//	servo_set_acceleration_time(0x00001000);
		//	//	gpio_bit_set(GPIOA, GPIO_PIN_9);
		//	//	servo_jog_mode_revers_rotation();
		//	//} else {
		//	//	button2 = 0;
		//	//	servo_jog_mode_stop_rotation();
		//	//	gpio_bit_reset(GPIOA, GPIO_PIN_9);
		//	//}
		//	
		//	while (!gpio_input_bit_get(GPIOC, GPIO_PIN_1));
		//}
    }
}
