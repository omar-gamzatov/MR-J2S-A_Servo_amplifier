#include "timer.h"

// timer 2 (freq - 10 kHz, period - 200 ms)
void servo_timer_config(void)
{
		timer_oc_parameter_struct timer_ocinit_struct;
		timer_parameter_struct timer_init_struct;
	
		rcu_periph_clock_enable(RCU_TIMER2);
		
		timer_deinit(TIMER2);
		timer_struct_para_init(&timer_init_struct);
	
		timer_init_struct.prescaler					= 17999;
		timer_init_struct.alignedmode				= TIMER_COUNTER_EDGE;
		timer_init_struct.counterdirection	= TIMER_COUNTER_UP;
		timer_init_struct.period						= 1999;
		timer_init_struct.clockdivision			= TIMER_CKDIV_DIV1;
		timer_init(TIMER2, &timer_init_struct);
}

void servo_timer_enable(void)
{
		nvic_irq_enable(TIMER2_IRQn, 2U, 0U);
		timer_interrupt_enable(TIMER2, TIMER_INT_UP);
		timer_enable(TIMER2);
}

void servo_timer_disable(void)
{
		timer_disable(TIMER2);
		timer_interrupt_disable(TIMER2, TIMER_INT_UP);
		nvic_irq_disable(TIMER2_IRQn);
}
