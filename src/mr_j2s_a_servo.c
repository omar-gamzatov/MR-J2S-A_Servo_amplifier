#include "mr_j2s_a_servo.h"

uint16_t rx_size = 50;
char rxbuffer[50];

char tx_write_buffer[20];
uint16_t tx_write_size = 20;

char tx_read_buffer[11];
uint16_t tx_read_size = 11;

servo_ready_status servo_ready = READY;


uint8_t ascii[16] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46
};

void servo_init(uint32_t baudrate)
{
		dma_nvic_config();
		usart1_init(baudrate);
		usart1_dma0_txinit(tx_read_buffer, tx_read_size);
		usart1_dma0_rxinit(rxbuffer, rx_size);
}

//----------------------------------------------------------------------------------------------------------------------
void servo_send_read_command(uint16_t command, uint16_t data, uint16_t response_size, uint8_t servo_number)
{
		servo_ready = BUSY;
		usart1_dma0_rxdata(response_size);

		uint16_t sum = ascii[servo_number] + ascii[(command >> 4)] + ascii[(command & 0x0f)] + STX 
	                + ascii[(data >> 4)] + ascii[(data & 0x0f)] + ETX;
	
    sprintf(tx_read_buffer, "%c%X%02X%c%02X%c%02X", SOH, servo_number, command, STX, data, ETX, sum & 0xff);
		usart1_dma0_send(tx_read_buffer, COMMAND_SIZE + 1);
	
		while (servo_ready == BUSY){
			__NOP();
		}
		__NOP();
}

//----------------------------------------------------------------------------------------------------------------------
void servo_send_write_command(uint16_t write_command, uint16_t data_number, const char* data_to_write, uint8_t servo_number)
{
		servo_ready = BUSY;
		uint16_t data_size = get_servo_data_length(data_to_write);

		usart1_dma0_rxdata(WRITE_COMMAND_RESPONSE_SIZE);
	
		uint16_t sum = 0;
		for (uint16_t i = 0; i < data_size; i++)
		{
				sum += data_to_write[i];
		}
		sum += ascii[servo_number] + ascii[(write_command >> 4)] + ascii[(write_command & 0x0F)]
					+ ascii[(data_number >> 4)] + ascii[(data_number & 0x0f)] + STX + ETX;
		
    sprintf(tx_write_buffer, "%c%X%02X%c%02X%s%c%02X", SOH, servo_number, write_command, STX, data_number, data_to_write, ETX, sum & 0xff);
		
		usart1_dma0_send(tx_write_buffer, COMMAND_SIZE + data_size + 1);
		
		while (servo_ready == BUSY){
			__NOP();
		}
		__NOP();
}

//----------------------------------------------------------------------------------------------------------------------
void servo_emg_stop(void)
{
		
}

void servo_set_operating_mode(const char* op_mode)
{
		servo_send_write_command(WRITE_PARAMS, OPERATING_MODE, op_mode, 0);
}


void servo_set_input_signal(uint8_t signal)
{
		servo_send_write_command(WRITE_PARAMS, CN1B_8_IN, "00000080", 0);
		servo_send_write_command(WRITE_PARAMS, CN1B_9_IN, "00000090", 0);
}

void servo_positioning_mode_on(const char* freq, const char* acceleration_time)
{
		servo_send_write_command(POSITIONING_MODE, POS_MODE_FREQUENCY, freq, 0);
		servo_send_write_command(POSITIONING_MODE, POS_MODE_ACCELERATION_TIME, acceleration_time, 0);
		servo_send_write_command(POSITIONING_MODE, POS_MODE_SON_LSP_LSN_ON, POS_MODE_SON_LSP_LSN_ON_DATA, 0);
	
		servo_send_write_command(POSITIONING_MODE, POS_MODE_SET_PATH_LENGTH, "000003E8", 0);
}

void servo_set_positioning_mode_path_length(const char* path_length)
{
		servo_send_write_command(POSITIONING_MODE, POS_MODE_SET_PATH_LENGTH, path_length, 0);
}

void servo_positioning_mode_break(void)
{
		servo_send_write_command(POSITIONING_MODE, POS_MODE_BREAK, POS_MODE_BREAK_DATA, 0);
}

void servo_positioning_mode_off(void)
{
		
}



//----------------------------------------------------------------------------------------------------------------------
void servo_handle_error(void)
{
		if (rxbuffer[2] != 'A')
		{
				gpio_bit_set(GPIOA, GPIO_PIN_7);
		}
}




uint16_t get_servo_data_length(const char* data)
{
		uint16_t length = 0;
		while (data[length] != '\0')
		{
				++length;
		}
		return length;
}

/*!
    \brief      configure DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
