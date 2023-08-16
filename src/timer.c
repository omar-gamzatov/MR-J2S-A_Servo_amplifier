#include "timer.h"

// timer 2 (20 kHz)
void servo_timer_init()
{
		timer_oc_parameter_struct timer_ocinit_struct;
		timer_parameter_struct timer_init_struct;
	
		rcu_periph_clock_enable(RCU_TIMER2);
		
		timer_deinit(TIMER2);
		timer_struct_para_init(&timer_init_struct);
	
		timer_init_struct.prescaler		= 17999;
		timer_init_struct.alignedmode	= TIMER_COUNTER_EDGE;
		timer_init_struct.counterdirection	= TIMER_COUNTER_UP;
		timer_init_struct.period						= 8999;
		timer_init_struct.clockdivision			= TIMER_CKDIV_DIV1;
		timer_init(TIMER2, &timer_init_struct);
	
		timer_interrupt_enable(TIMER2, TIMER_INT_UP);
	
		timer_enable(TIMER2);
	
}

void timer_nvic_config(void)
{
		nvic_irq_enable(TIMER2_IRQn, 0, 0);
}