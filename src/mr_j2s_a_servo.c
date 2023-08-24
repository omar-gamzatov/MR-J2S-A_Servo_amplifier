#include "mr_j2s_a_servo.h"

servo_func_mode servo_mode = nothing_mode;
servo_jog_functions jog_func = jog_on;
servo_pos_functions pos_func = pos_on;

uint8_t servo_jog_functions_cnt[5] = {0, 0, 0, 0, 0};
uint8_t servo_pos_functions_cnt[5] = {0, 0, 0, 0, 0};
uint32_t baudrate[4] = {9600, 19200, 38400, 57600};
uint32_t servo_curr_baudrate = 0;

uint16_t servo_rx_size = 50;
char servo_rxbuffer[50];

uint16_t servo_tx_size = 50;
char servo_txbuffer[50];

servo_state servo_status = ready;

uint16_t servo_freq = 100;
uint32_t servo_acceleration_time = 1000;
int32_t pos_mode_path_length = 1000;

uint8_t ascii[16] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46
};

void servo_init(servo_baudrate _baudrate)
{
	servo_curr_baudrate = _baudrate;
	dma_nvic_config();
	usart1_init(baudrate[_baudrate]);
	usart1_dma0_txinit(servo_txbuffer, servo_tx_size);
	usart1_dma0_rxinit(servo_rxbuffer, servo_rx_size);

	servo_timer_config();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
void servo_send_read_command(uint16_t command, uint16_t data, uint16_t response_size, uint8_t servo_number)
{
	usart1_dma0_rxdata(response_size);

	uint16_t sum = ascii[servo_number] + ascii[(command >> 4)] + ascii[(command & 0x0f)] + STX 
	             + ascii[(data >> 4)] + ascii[(data & 0x0f)] + ETX;
	
    sprintf(servo_txbuffer, "%c%c%X%02X%c%02X%c%02X", EOT, SOH, servo_number, command, STX, data, ETX, sum & 0xff);
	usart1_dma0_send(servo_txbuffer, COMMAND_SIZE + 1);
}

void servo_send_write_command4(uint16_t write_command, uint16_t data_number, uint16_t data_to_write, uint8_t servo_number)
{
	usart1_dma0_rxdata(WRITE_COMMAND_RESPONSE_SIZE);
	
	uint16_t sum = 0;
	for (uint16_t i = 0; i < 4; i++) {
		sum += ascii[(uint8_t)((data_to_write >> i * 4) & 0x0F)];
	}
	sum += ascii[servo_number] + ascii[(write_command >> 4)] + ascii[(write_command & 0x0F)]
				+ ascii[(data_number >> 4)] + ascii[(data_number & 0x0f)] + STX + ETX;
	
	sprintf(servo_txbuffer, "%c%c%X%02X%c%02X%04X%c%02X", EOT, SOH, servo_number, write_command, STX, data_number, data_to_write, ETX, sum & 0xff);
	
	usart1_dma0_send(servo_txbuffer, COMMAND_SIZE + 4 + 1);
}

void servo_send_write_command8(uint16_t write_command, uint16_t data_number, uint32_t data_to_write, uint8_t servo_number)
{
	usart1_dma0_rxdata(WRITE_COMMAND_RESPONSE_SIZE);
	
	volatile uint16_t sum = 0;
	for (uint16_t i = 0; i < 8; i++) {
		sum += ascii[(uint8_t)((data_to_write >> i * 4) & 0x0F)];
	}
	sum += ascii[servo_number] + ascii[(write_command >> 4)] + ascii[(write_command & 0x0F)]
				+ ascii[(data_number >> 4)] + ascii[(data_number & 0x0f)] + STX + ETX;
	
	sprintf(servo_txbuffer, "%c%c%X%02X%c%02X%08X%c%02X", EOT, SOH, servo_number, write_command, STX, data_number, data_to_write, ETX, sum & 0xff);
	
	usart1_dma0_send(servo_txbuffer, COMMAND_SIZE + 8 + 1);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------
void servo_emg_stop(void)
{
	switch (servo_status) {
		case jog:
			servo_timer_disable();
			servo_jog_mode_off();
			break;
		case positioning:
			servo_positioning_mode_off();
			break;
		case ready:
			break;
	}
}

void servo_set_freq(uint32_t freq)
{
	servo_freq = freq;
}

void servo_set_acceleration_time(uint64_t acceleration_time)
{
	servo_acceleration_time = acceleration_time;
}

void servo_set_path_length(int32_t path)
{
	pos_mode_path_length = path;
}

void servo_set_operating_mode(uint32_t op_mode)
{
	servo_send_write_command4(WRITE_PARAMS, OPERATING_MODE, op_mode, 0);
}

//-----------------------------------------------------------------Positioning mode----------------------------------------------------------------
void servo_positioning_mode_on(void)
{
	switch (servo_pos_functions_cnt[POS_ON]) {
		case 0:
			servo_status = positioning;
			servo_mode = pos_mode;
			pos_func = pos_on;
			servo_pos_functions_cnt[POS_ON] = 1;
			servo_send_write_command8(WRITE_PARAMS, 0x00, 0x30000002, 0);
			break;
		case 1:
			servo_pos_functions_cnt[POS_ON] = 2;
			servo_send_write_command4(EXTERN_OUTPUT_SIGNAL_BLOCK, OUTPUT_SIGNAL_BLOCK, TEST_MODE_BREAK_DATA, 0);
			break;
		case 2:
			servo_pos_functions_cnt[POS_ON] = 3;
			servo_send_write_command4(WRITE_TEST_OPERATING_MODE, SET_TEST_MODE, TEST_MODE_POSITIONING, 0);
		case 3:
			servo_pos_functions_cnt[POS_ON] = 0;
			servo_mode = nothing_mode;
			break;
		default:
			break;
	};
}

void servo_positioning_mode_off(void)
{
	switch (servo_pos_functions_cnt[POS_OFF]) {
		case 0:
			servo_mode = pos_mode;
			pos_func = pos_off;
			servo_pos_functions_cnt[POS_OFF] = 1;
			servo_send_write_command4(TEST_MODE, POS_MODE_STOP, TEST_MODE_BREAK_DATA, 0);
			break;
		case 1:
			servo_pos_functions_cnt[POS_OFF] = 2;
			servo_send_write_command4(TEST_MODE, POS_MODE_STOP, TEST_MODE_BREAK_DATA, 0);
			break;
		case 2:
			servo_pos_functions_cnt[POS_OFF] = 3;
			servo_send_write_command4(WRITE_TEST_OPERATING_MODE, SET_TEST_MODE, TEST_MODE_BREAK, 0);
			break;
		case 3:
			servo_pos_functions_cnt[POS_OFF] = 0;
			servo_send_write_command4(EXTERN_OUTPUT_SIGNAL_BLOCK, OUTPUT_SIGNAL_UNLOCK, TEST_MODE_BREAK_DATA, 0);
			servo_mode = nothing_mode;
			servo_status = ready;
			break;
		default:
			break;
	};
}

void servo_positioning_mode_config(void)
{
	switch (servo_pos_functions_cnt[POS_CONFIG]) {
		case 0:
			servo_mode = pos_mode;
			pos_func = pos_config;
			servo_pos_functions_cnt[POS_CONFIG] = 1;
			servo_send_write_command4(TEST_MODE, POS_MODE_FREQUENCY, servo_freq, 0);
			break;
		case 1:
			servo_pos_functions_cnt[POS_CONFIG] = 2;
			servo_send_write_command8(TEST_MODE, POS_MODE_ACCELERATION_TIME, servo_acceleration_time, 0);
			break;
		case 2:
			servo_pos_functions_cnt[POS_CONFIG] = 0;
			servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, POS_MODE_SON_LSP_LSN_ON_DATA, 0);
			servo_mode = nothing_mode;
			break;
		default:
			break;
	};
}

void servo_positioning_mode_path_length(void)
{
	if (servo_pos_functions_cnt[POS_PATH_LENGTH] == 0) {
		servo_mode = pos_mode;
		pos_func = pos_path_length;
		servo_pos_functions_cnt[POS_PATH_LENGTH] = 1;
		servo_send_write_command8(TEST_MODE, POS_MODE_SET_PATH_LENGTH, pos_mode_path_length, 0);
	} else if (servo_pos_functions_cnt[POS_PATH_LENGTH] == 1) {
		servo_pos_functions_cnt[POS_PATH_LENGTH] = 0;
		servo_mode = nothing_mode;
	}
}

void servo_positioning_mode_break(void)
{
	if (servo_pos_functions_cnt[POS_BREAK] == 0) {
		servo_mode = pos_mode;
		pos_func = pos_break;
		servo_pos_functions_cnt[POS_BREAK] = 1;
		servo_send_write_command4(TEST_MODE, POS_MODE_BREAK, TEST_MODE_BREAK_DATA, 0);
	} else if (servo_pos_functions_cnt[POS_BREAK] == 1) {
		servo_pos_functions_cnt[POS_BREAK] = 0;
		servo_mode = nothing_mode;
	}
}

//-----------------------------------------------------------------JOG mode----------------------------------------------------------------
void servo_jog_mode_on(void)	// ввод в режим JOG
{
	switch (servo_jog_functions_cnt[JOG_ON]) {
		case 0:
			servo_status = jog;
			servo_mode = jog_mode;
			jog_func = jog_on;
			servo_jog_functions_cnt[JOG_ON] = 1;
			servo_send_write_command8(WRITE_PARAMS, 0x00, 0x30000002, 0);
			break;
		case 1:
			servo_jog_functions_cnt[JOG_ON] = 2;
			servo_send_write_command4(EXTERN_OUTPUT_SIGNAL_BLOCK, OUTPUT_SIGNAL_BLOCK, TEST_MODE_BREAK_DATA, 0);
			break;
		case 2:
			servo_jog_functions_cnt[JOG_ON] = 3;
			servo_send_write_command4(WRITE_TEST_OPERATING_MODE, SET_TEST_MODE, TEST_MODE_JOG, 0);
			break;
		case 3:
			servo_jog_functions_cnt[JOG_ON] = 0;
			servo_mode = timer_mode;
			servo_timer_enable();
			break;	
		default:
			break;
	};
}

void servo_jog_mode_off(void)	// выход из режима JOG
{
	switch (servo_jog_functions_cnt[JOG_OFF]) {
		case 0:
			servo_timer_disable();
			servo_mode = jog_mode;
			jog_func = jog_off;
			servo_jog_functions_cnt[JOG_OFF] = 1;
			servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_STOP_ROTATION, 0);
			break;
		case 1:
			servo_jog_functions_cnt[JOG_OFF] = 2;
			servo_send_write_command4(TEST_MODE, JOG_MODE_STOP, TEST_MODE_BREAK_DATA, 0);
			break;
		case 2:
			servo_jog_functions_cnt[JOG_OFF] = 3;
			servo_send_write_command4(WRITE_TEST_OPERATING_MODE, SET_TEST_MODE, TEST_MODE_BREAK, 0);
			break;
		case 3:
			servo_jog_functions_cnt[JOG_OFF] = 0;
			servo_send_write_command4(EXTERN_OUTPUT_SIGNAL_BLOCK, OUTPUT_SIGNAL_UNLOCK, TEST_MODE_BREAK_DATA, 0);
			servo_mode = nothing_mode;
			servo_status = ready;
			break;
		default:
			break;
	};
}

void servo_jog_mode_direct_rotation(void)	// установка частоты вращения, времени разгона/торможения, включение прямого вращения
{
	switch (servo_jog_functions_cnt[JOG_DIRECT_ROT]) {
		case 0:
			servo_mode = jog_mode;
			jog_func = jog_direct_rotation;
			servo_jog_functions_cnt[JOG_DIRECT_ROT] = 1;
			servo_send_write_command4(TEST_MODE, POS_MODE_FREQUENCY, servo_freq, 0);
			break;
		case 1:
			servo_jog_functions_cnt[JOG_DIRECT_ROT] = 2;
			servo_send_write_command8(TEST_MODE, POS_MODE_ACCELERATION_TIME, servo_acceleration_time, 0);
			break;
		case 2:
			servo_jog_functions_cnt[JOG_DIRECT_ROT] = 0;
			servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_DIRECT_ROTATION, 0);
			servo_mode = timer_mode;
			break;
		default:
			break;
	};
}

void servo_jog_mode_revers_rotation(void)	// установка частоты вращения, времени разгона/торможения, включение обратного вращения
{
	switch (servo_jog_functions_cnt[JOG_REVERSE_ROT]) {
		case 0:
			servo_mode = jog_mode;
			jog_func = jog_reverse_rotation;
			servo_jog_functions_cnt[JOG_REVERSE_ROT] = 1;
			servo_send_write_command4(TEST_MODE, POS_MODE_FREQUENCY, servo_freq, 0);
			break;
		case 1:
			servo_jog_functions_cnt[JOG_REVERSE_ROT] = 2;
			servo_send_write_command8(TEST_MODE, POS_MODE_ACCELERATION_TIME, servo_acceleration_time, 0);
			break;
		case 2:
			servo_jog_functions_cnt[JOG_REVERSE_ROT] = 0;
			servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_REVERSE_ROTATION, 0);
			servo_mode = timer_mode;
			break;
		default:
			break;
	};
}

void servo_jog_mode_stop_rotation(void)		// остановка двигателя в режиме JOG
{
	if (servo_jog_functions_cnt[JOG_STOP] == 0) {
		servo_mode = jog_mode;
		jog_func = jog_stop;
		servo_jog_functions_cnt[JOG_STOP] = 1;
		servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_STOP_ROTATION, 0);
	} else if (servo_jog_functions_cnt[JOG_STOP] == 1) {
		servo_jog_functions_cnt[JOG_STOP] = 0;
		servo_mode = timer_mode;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------
uint8_t servo_handle_error(void)
{
	if (servo_rxbuffer[2] != 'A') {
		gpio_bit_set(GPIOA, GPIO_PIN_7);
		servo_emg_stop();
	}
	return servo_rxbuffer[2];
}

uint16_t get_servo_data_length(const char* data)
{
	uint16_t length = 0;
	while (data[length] != '\0') {
		++length;
	}
	return length;
}
