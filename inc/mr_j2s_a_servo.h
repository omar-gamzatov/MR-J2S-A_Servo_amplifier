#ifndef MR_J2S_A_SERVO_H
#define MR_J2S_A_SERVO_H

#include <stdio.h>
#include "stdint.h"
#include "usart1_dma0.h"
#include "timer.h"

#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define EOT 0x04


//---------------------------------------------------------Команды считывания------------------------------------------------------------
#define READ_STATE																0x01		//	индикация состояния
#define READ_PARAMS 															0x05		//	параметры
#define READ_IO_SIGNALS														0x12		//	внешние входные/выходные сигналы
#define READ_ALARMS 															0x33		//	сигналы тревоги
#define READ_ALARMS_TIME													0x33		//	время сигналов тревоги
#define READ_CURRENT_ALARM_NUMBER 								0x02		//	фактический номер сигнала тревоги 
#define READ_ACTIVE_ALARM 												0x35		//	индикация состояния при возникновении сигнала тревоги
#define READ_GROUP_SETTINGS 											0x1F		//	настройки группы
#define READ_OTHER_COMANDS 												0x02		//	иные команды
#define READ_SOFT_VERSION 												0x02		//	иные команды

// Длина принимаемых данных 
#define RESPONSE_SIZE_STATE												18			// 	в байтах
#define RESPONSE_SIZE_PARAMS 											14
#define RESPONSE_SIZE_IO_SIGNALS									14
#define RESPONSE_SIZE_ALARMS 											10
#define RESPONSE_SIZE_ALARMS_TIME									14
#define RESPONSE_SIZE_ACTUAL_ALARM_NUMBER 				10
#define RESPONSE_SIZE_ACTIVE_ALARM 								18
#define RESPONSE_SIZE_GROUP_SETTINGS 							10
#define RESPONSE_SIZE_OTHER_COMANDS 							14
#define RESPONSE_SIZE_SOFT_VERSION 								22

// Данные для считывания
// Индикация состояния (комнда [0][1])
#define DATA_FEEDBACK_IMPULSES 										0x80	 	//	импульсы обратной связи
#define DATA_ENGINE_SPEED			 										0x81		//	частота вращения двигателя
#define DATA_MISMATCH															0x82		//	рассогласование (в импульсах)
#define DATA_PULSE_SETPOINT												0x83		//	заданное значение импульса
#define DATA_SETPOINT_FREQ												0x84		//	частота заданного значения
#define DATA_FREQ_VAL															0x85		//	значение аналогового задания частоты (ограничения частоты) вращения
#define DATA_TORQUE_VAL														0x86		//	значение аналогового задания крутящего момента (ограничения крутящего момента)
#define DATA_BRAKE_CHAIN_LOAD											0x87		//	загруженность тормозной цепи
#define DATA_EFFICIENT_LOAD_VALUE									0x88		//	эффективное значение нагрузки
#define DATA_MAXIMUM_LOAD													0x89		//	максимальное значение нагрузки
#define DATA_ACTUAL_TORQUE												0x8A		//	фактический крутящий момент
#define DATA_ABS_POS_PER_ROTATION									0x8B		//	абсолютная позиция за один оборот
#define DATA_ABSOLUTE_COUNTER											0x8C		//	счетчик абсолютного значения
#define DATA_MASS_INERTIA_MOMENT_RATIO						0x8D		//	соотношение моментов инерции масс
#define DATA_INTERMEDIATE_CIRCUIT_VOLTAGE					0x8E		//	напряжение промежутночного контура



// Входные/выходные сигналы (команда [1][2])
#define INPUT_SIGNALS															0x40		//	внешние параметры входного сигнала
#define OUTPUT_SIGNALS														0xC0		//	внешние параметры выходного сигнала

// Список сигналов тревоги (команда [3][3])
#define CURRENT_ALARM															0x10		//	действующий сигнал тревоги
#define ALARM_1																		0x11		//	последний сигнал тревоги
#define ALARM_2																		0x12		//	предпоследний сигнал тревоги
#define ALARM_3																		0x13		//	третий с конца сигнал тревоги
#define ALARM_4																		0x14		//	четвертый с конца сигнал тревоги
#define ALARM_5																		0x15		//	пятый с конца сигнал тревоги
#define CURRENT_ALARM_TIME												0x20		//	время начала действующего сигнала тревоги
#define ALARM_1_TIME															0x21		//	время начала последнего сигнала тревоги
#define ALARM_2_TIME															0x22		//	время начала предпоследнего сигнала тревоги
#define ALARM_3_TIME															0x23		//	время начала третьего с конца сигнала тревоги
#define ALARM_4_TIME															0x24		//	время начала четвертого с конца сигнала тревоги
#define ALARM_5_TIME															0x25		//	время начала пятого с конца сигнала тревоги	

// Фактический номер сигнала тревоги (команда [0][2])
#define CURRENT_ALARM_NUMBER 											0x00		//	фактический номер сигнала тревоги 

// Действующий сигнал тревоги (команда [3][5])
#define ALARM_FEEDBACK_IMPULSES 								 	0x80	 	//	импульсы обратной связи при возникновении сигнала тревоги
#define ALARM_ENGINE_SPEED			 								 	0x81		//	частота вращения двигателя при возникновении сигнала тревоги
#define ALARM_MISMATCH													 	0x82		//	рассогласование (в импульсах) при возникновении сигнала тревоги
#define ALARM_PULSE_SETPOINT										 	0x83		//	заданное значение импульса при возникновении сигнала тревоги
#define ALARM_SETPOINT_FREQ											 	0x84		//	частота заданного значения при возникновении сигнала тревоги
#define ALARM_FREQ_VAL													 	0x85		//	значение аналогового задания частоты (ограничения частоты) вращения при возникновении сигнала тревоги
#define ALARM_TORQUE_VAL												 	0x86		//	значение аналогового задания крутящего момента (ограничения крутящего момента) при возникновении сигнала тревоги
#define ALARM_BRAKE_CHAIN_LOAD									 	0x87		//	загруженность тормозной цепи при возникновении сигнала тревоги
#define ALARM_EFFICIENT_LOAD_VALUE							 	0x88		//	эффективное значение нагрузки при возникновении сигнала тревоги
#define ALARM_MAXIMUM_LOAD											 	0x89		//	максимальное значение нагрузки при возникновении сигнала тревоги
#define ALARM_ACTUAL_TORQUE											 	0x8A		//	фактический крутящий момент при возникновении сигнала тревоги
#define ALARM_ABS_POS_PER_ROTATION							 	0x8B		//	абсолютная позиция за один оборот при возникновении сигнала тревоги
#define ALARM_ABSOLUTE_COUNTER									 	0x8C		//	счетчик абсолютного значения при возникновении сигнала тревоги
#define ALARM_MASS_INERTIA_MOMENT_RATIO					 	0x8D		//	соотношение моментов инерции масс при возникновении сигнала тревоги
#define ALARM_INTERMEDIATE_CIRCUIT_VOLTAGE			 	0x8E		//	напряжение промежутночного контура при возникновении сигнала тревоги

// Настройки группы
#define GROUP_SETTINGS														0x00		//	считывание настраиваемого значения для одной группы

// Иные данные для считывания
#define ABS_VALUE_POS_IMP													0x90		//	позиция абсолютного значения относительно блока импульсов сервопривода
#define ABS_VALUE_POS_COMAND											0x91		//	позиция абсолютного значения относительно блока команд
#define SOFT_VERSION															0x70		//	версия программного обеспечения


//--------------------------------------------------------Команды записи------------------------------------------------------------------
#define STATE_INDICATION_RESET										0x81		//	4 байта данных
#define WRITE_PARAMS															0x84		//	8 байт
#define ALARM_LIST_CLEARING												0x82		//	4 байта
#define CURRENT_ALARM_RECOVERY										0x82		//	4 байта
#define WRITE_TEST_OPERATING_MODE									0x8B		//	4 байта
#define EXTERN_OUTPUT_SIGNAL_BLOCK								0x90		//	4 байта
#define TEST_MODE																	0xA0
#define TEST_MODE_INPUT_SIGNAL										0x92

//--------------------------------------------------------Тестовый режим (данныt)--------------------------------------------------
#define TEST_MODE_BREAK														"0000"
#define TEST_MODE_JOG															"0001"
#define TEST_MODE_POSITIONING											"0002"


//--------------------------------------------------------Режим позиционирования (номера данных)------------------------------------------
#define POS_MODE_FREQUENCY												0x10
#define POS_MODE_ACCELERATION_TIME								0x11
#define POS_MODE_STOP															0x12
#define POS_MODE_SET_PATH_LENGTH									0x13
#define POS_MODE_BREAK														0x15
#define POS_MODE_SON_LSP_LSN_ON										0x00
#define OUTPUT_SIGNAL_BLOCK												0x00
#define OUTPUT_SIGNAL_UNLOCK											0x10
#define SET_TEST_MODE															0x00

//--------------------------------------------------------Режим позиционирования (данные)-------------------------------------------------
#define TEST_MODE_BREAK_DATA											"1EA5"
#define POS_MODE_SON_LSP_LSN_ON_DATA							"00000007"

//--------------------------------------------------------Режим JOG-----------------------------------------------------------------------
#define JOG_MODE_DIRECT_ROTATION									"00000807"
#define JOG_MODE_REVERSE_ROTATION									"00001007"
#define JOG_MODE_STOP_ROTATION										"00000007"
#define JOG_MODE_STOP															0x12
#define JOG_MODE_FREQUENCY												0x10
#define JOG_MODE_ACCELERATION_TIME								0x11

//--------------------------------------------------------Параметры-----------------------------------------------------------------------
#define OPERATING_MODE														0x00		//  настройки рабочего режима
#define CN1A_8_IN																	0x45		// 	настройка входного сигнала CN1A-8
#define CN1B_5_IN																	0x43		// 	настройка входного сигнала CN1B-5
#define CN1B_7_IN																	0x46		// 	настройка входного сигнала CN1B-7
#define CN1B_8_IN																	0x47		// 	настройка входного сигнала CN1B-8
#define CN1B_9_IN																	0x48		// 	настройка входного сигнала CN1B-9
#define CN1B_14_IN																0x44		// 	настройка входного сигнала CN1B-14

//--------------------------------------------------------Режим работы (параметр 0)-------------------------------------------------------
#define OP_MODE_POSITION													"0000"
#define OP_MODE_POSITION_FREQUENCY								"0001"
#define OP_MODE_FREQUENCY													"0002"
#define OP_MODE_FREQUENCY_TORQUE									"0003"
#define OP_MODE_TORQUE														"0004"
#define OP_MODE_TORQUE_POSITION										"0005"



#define COMMAND_SIZE															10
#define WRITE_COMMAND_RESPONSE_SIZE								6

typedef enum {READY = 1, BUSY = 0} servo_ready_status;






//__IO FlagStatus g_transfer_complete = SET;

#define ARRAYNUM(arr_nanme)       (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))

uint16_t get_servo_data_length(const char* data);

void servo_init(uint32_t baudrate);

void servo_send_read_command(uint16_t read_command, uint16_t data_to_read, uint16_t response_size, uint8_t servo_number);
void servo_send_write_command(uint16_t write_command, uint16_t data_number, const char* data_to_write, uint8_t servo_number);
void servo_send_eot(void);
	
void servo_emg_stop(void);
void servo_set_operating_mode(const char* op_mode);
void servo_set_input_signal(uint8_t signal);

void servo_positioning_mode_on(const char* freq, const char* acceleration_time);
void servo_set_positioning_mode_path_length(const char* path_length);
void servo_positioning_mode_break(void);
void servo_positioning_mode_off(void);

void servo_jog_mode_on(const char* freq_4_byte, const char* acceleration_time_8_byte);
void servo_jog_mode_off(void);
void servo_jog_mode_freq_set(const char* freq_4_byte);
void servo_jog_mode_direct_rotation(void);
void servo_jog_mode_revers_rotation(void);
void servo_jog_mode_stop_rotation(void);

void servo_handle_error(void);
	
#endif /* MR_J2S_A_SERVO_H */
