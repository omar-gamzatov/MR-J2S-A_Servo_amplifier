#include "mr_j2s_a_servo.h"

servo_func_mode servo_mode = jog_mode;
servo_jog_functions jog_func = jog_on;
servo_pos_functions pos_func = pos_on;

uint8_t servo_jog_functions_cnt[7] = {0, 0, 0, 0, 0, 0, 0};
uint8_t servo_pos_functions_cnt[6] = {0, 0, 0, 0, 0, 0};

uint16_t rx_size = 50;
char rxbuffer[50];

char tx_write_buffer[50];
uint16_t tx_write_size = 50;

char tx_read_buffer[50];
uint16_t tx_read_size = 50;

servo_ready_status servo_ready = READY;

uint32_t servo_freq = 100;
uint64_t servo_acceleration_time = 1000;
int64_t pos_mode_path_length = 0;

uint8_t ascii[16] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46
};

void servo_init(uint32_t baudrate)
{
	dma_nvic_config();
	usart1_init(baudrate);
	usart1_dma0_txinit(tx_read_buffer, tx_read_size);
	usart1_dma0_rxinit(rxbuffer, rx_size);
	
	servo_timer_config();
}

//----------------------------------------------------------------------------------------------------------------------
void servo_send_read_command(uint16_t command, uint16_t data, uint16_t response_size, uint8_t servo_number)
{
	usart1_dma0_rxdata(response_size);

	uint16_t sum = ascii[servo_number] + ascii[(command >> 4)] + ascii[(command & 0x0f)] + STX 
	             + ascii[(data >> 4)] + ascii[(data & 0x0f)] + ETX;
	
    sprintf(tx_read_buffer, "%c%c%X%02X%c%02X%c%02X", EOT, SOH, servo_number, command, STX, data, ETX, sum & 0xff);
	usart1_dma0_send(tx_read_buffer, COMMAND_SIZE);
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
	
	sprintf(tx_write_buffer, "%c%c%X%02X%c%02X%04X%c%02X", EOT, SOH, servo_number, write_command, STX, data_number, data_to_write, ETX, sum & 0xff);
	
	usart1_dma0_send(tx_write_buffer, COMMAND_SIZE + 4 + 1);
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
	
	sprintf(tx_write_buffer, "%c%c%X%02X%c%02X%08X%c%02X", EOT, SOH, servo_number, write_command, STX, data_number, data_to_write, ETX, sum & 0xff);
	
	usart1_dma0_send(tx_write_buffer, COMMAND_SIZE + 8 + 1);
}
//----------------------------------------------------------------------------------------------------------------------
void servo_emg_stop(void)
{
		
}

void servo_set_freq(uint32_t freq)
{
	servo_freq = freq;
}

void servo_set_acceleration_time(uint64_t acceleration_time)
{
	servo_acceleration_time = acceleration_time;
}

void servo_set_path_length(int64_t path)
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
	switch (servo_jog_functions_cnt[POS_ON]) {
		case 0:
			servo_mode = pos_mode;
			pos_func = pos_on;
			servo_jog_functions_cnt[POS_ON] = 1;
			servo_send_write_command8(WRITE_PARAMS, 0x00, 0x30000002, 0);
			break;
		case 1:
			servo_jog_functions_cnt[POS_ON] = 2;
			servo_send_write_command4(EXTERN_OUTPUT_SIGNAL_BLOCK, OUTPUT_SIGNAL_BLOCK, TEST_MODE_BREAK_DATA, 0);
			break;
		case 2:
			servo_jog_functions_cnt[POS_ON] = 2;
			servo_send_write_command4(WRITE_TEST_OPERATING_MODE, SET_TEST_MODE, TEST_MODE_POSITIONING, 0);
			break;
		case 3:
			servo_jog_functions_cnt[POS_ON] = 0;
			servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, POS_MODE_SON_LSP_LSN_ON_DATA, 0);
			servo_mode = nothing_mode;
			servo_timer_enable();
			break;
		default:
			break;
	};
}

void servo_positioning_mode_off(void)
{
	switch (servo_jog_functions_cnt[POS_OFF]) {
		case 0:
			servo_timer_disable();
			servo_mode = pos_mode;
			pos_func = pos_off;
			servo_jog_functions_cnt[POS_OFF] = 1;
			servo_send_write_command4(TEST_MODE, POS_MODE_STOP, TEST_MODE_BREAK_DATA, 0);
			break;
		case 1:
			servo_jog_functions_cnt[POS_OFF] = 2;
			servo_send_write_command4(WRITE_TEST_OPERATING_MODE, SET_TEST_MODE, TEST_MODE_BREAK, 0);
			break;
		case 2:
			servo_jog_functions_cnt[POS_OFF] = 0;
			servo_send_write_command4(EXTERN_OUTPUT_SIGNAL_BLOCK, OUTPUT_SIGNAL_UNLOCK, TEST_MODE_BREAK_DATA, 0);
			servo_mode = nothing_mode;
			break;
		default:
			break;
	};
}

void servo_set_pos_mode_freq(void)
{
	if (servo_pos_functions_cnt[POS_FREQ_SET] == 0) {
		servo_mode = pos_mode;
		pos_func = pos_freq_set;
		servo_pos_functions_cnt[POS_FREQ_SET] = 1;
		servo_send_write_command4(TEST_MODE, POS_MODE_FREQUENCY, servo_freq, 0);
	} else if (servo_pos_functions_cnt[POS_FREQ_SET] == 1) {
		servo_pos_functions_cnt[POS_FREQ_SET] = 0;
		servo_mode = nothing_mode;
	}
}

void servo_set_pos_mode_acceleration_time(void)
{
	if (servo_pos_functions_cnt[POS_ACCELERATION_TIME_SET] == 0) {
		servo_mode = pos_mode;
		pos_func = pos_acceleration_time_set;
		servo_pos_functions_cnt[POS_ACCELERATION_TIME_SET] = 1;
		servo_send_write_command8(TEST_MODE, POS_MODE_FREQUENCY, servo_acceleration_time, 0);
	} else if (servo_pos_functions_cnt[POS_ACCELERATION_TIME_SET] == 1) {
		servo_pos_functions_cnt[POS_ACCELERATION_TIME_SET] = 0;
		servo_mode = nothing_mode;
	}
}

void servo_set_positioning_mode_path_length(void)
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
void servo_jog_mode_on(void)
{
	switch (servo_jog_functions_cnt[JOG_ON]) {
		case 0:
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
			servo_jog_functions_cnt[JOG_ON] = 4;
			servo_send_write_command4(TEST_MODE, POS_MODE_FREQUENCY, servo_freq, 0);
			break;
		case 4:
			servo_jog_functions_cnt[JOG_ON] = 0;
			servo_send_write_command8(TEST_MODE, POS_MODE_FREQUENCY, servo_acceleration_time, 0);
			servo_mode = nothing_mode;
			servo_timer_enable();
			break;	
		default:
			break;
	};
}

void servo_jog_mode_off(void)
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
			break;
		default:
			break;
	};
}

void servo_jog_mode_set_freq(void)
{
	if (servo_jog_functions_cnt[JOG_FREQ_SET] == 0) {
		servo_mode = jog_mode;
		jog_func = jog_freq_set;
		servo_jog_functions_cnt[JOG_FREQ_SET] = 1;
		servo_send_write_command4(TEST_MODE, POS_MODE_FREQUENCY, servo_freq, 0);
	} else if (servo_jog_functions_cnt[JOG_FREQ_SET] == 1) {
		servo_jog_functions_cnt[JOG_FREQ_SET] = 0;
		servo_mode = nothing_mode;
	}
}

void servo_jog_mode_set_acceleration_time(void)
{
	if (servo_jog_functions_cnt[JOG_ACCELERATION_TIME_SET] == 0) {
		servo_mode = jog_mode;
		jog_func = jog_acceleration_time_set;
		servo_jog_functions_cnt[JOG_ACCELERATION_TIME_SET] = 1;
		servo_send_write_command8(TEST_MODE, POS_MODE_FREQUENCY, servo_acceleration_time, 0);
	} else if (servo_jog_functions_cnt[JOG_ACCELERATION_TIME_SET] == 1) {
		servo_jog_functions_cnt[JOG_ACCELERATION_TIME_SET] = 0;
		servo_mode = nothing_mode;
	}
}

void servo_jog_mode_direct_rotation(void)
{
	if (servo_jog_functions_cnt[JOG_DIRECT_ROT] == 0) {
		servo_mode = jog_mode;
		jog_func = jog_direct_rotation;
		servo_jog_functions_cnt[JOG_DIRECT_ROT] = 1;
		servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_DIRECT_ROTATION, 0);
	} else if (servo_jog_functions_cnt[JOG_DIRECT_ROT] == 1) {
		servo_jog_functions_cnt[JOG_DIRECT_ROT] = 0;
		servo_mode = nothing_mode;
	}
}

void servo_jog_mode_revers_rotation(void)
{
	if (servo_jog_functions_cnt[JOG_REVERSE_ROT] == 0) {
		servo_mode = jog_mode;
		jog_func = jog_reverse_rotation;
		servo_jog_functions_cnt[JOG_REVERSE_ROT] = 1;
		servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_REVERSE_ROTATION, 0);
	} else if (servo_jog_functions_cnt[JOG_REVERSE_ROT] == 1) {
		servo_jog_functions_cnt[JOG_REVERSE_ROT] = 0;
		servo_mode = nothing_mode;
	}
}

void servo_jog_mode_stop_rotation(void)
{
	if (servo_jog_functions_cnt[JOG_STOP] == 0) {
		servo_mode = jog_mode;
		jog_func = jog_stop;
		servo_jog_functions_cnt[JOG_STOP] = 1;
		servo_send_write_command8(TEST_MODE_INPUT_SIGNAL, POS_MODE_SON_LSP_LSN_ON, JOG_MODE_STOP_ROTATION, 0);
	} else if (servo_jog_functions_cnt[JOG_STOP] == 1) {
		servo_jog_functions_cnt[JOG_STOP] = 0;
		servo_mode = nothing_mode;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------
void servo_handle_error(void)
{
	if (rxbuffer[2] != 'A') {
		gpio_bit_set(GPIOA, GPIO_PIN_7);
	}
}

uint16_t get_servo_data_length(const char* data)
{
	uint16_t length = 0;
	while (data[length] != '\0') {
		++length;
	}
	return length;
}
