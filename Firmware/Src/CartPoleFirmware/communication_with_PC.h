#ifndef __COMMUNICATION_WITH_PC_H_
#define __COMMUNICATION_WITH_PC_H_

#include <stdbool.h>

// Command set
#define SERIAL_MAX_PKT_LENGTH		32
#define SERIAL_SOF					0xAA
#define CMD_PING					0xC0
#define CMD_STREAM_ON               0xC1
#define CMD_CALIBRATE				0xC2
#define CMD_CONTROL_MODE			0xC3
#define CMD_SET_PID_CONFIG		    0xC4
#define CMD_GET_PID_CONFIG			0xC5
#define CMD_SET_CONTROL_CONFIG		0xC6
#define CMD_GET_CONTROL_CONFIG		0xC7
#define CMD_SET_MOTOR				0xC8
#define CMD_SET_TARGET_POSITION		0xC9
#define CMD_COLLECT_RAW_ANGLE		0xCA
#define CMD_STATE					0xCC
#define CMD_SET_TARGET_EQUILIBRIUM  0xCD
#define CMD_RUN_HARDWARE_EXPERIMENT 0xCE
#define CMD_TRANSFER_BUFFERS        0xD1
#define CMD_DO_NOTHING				0x00

int get_command_from_PC_message(unsigned char * rxBuffer, unsigned int* rxCnt);
void prepare_message_to_PC_state(
		unsigned char * buffer,
		unsigned short message_len,
		int angle,
		float angleD_unprocessed,
		short position,
		float target_position,
		int motor_command,
		int invalid_step,
		unsigned long time_difference_between_measurement,
		unsigned long timeMeasured,
		unsigned long latency,
		unsigned short	latency_violation
		);

void prepare_message_to_PC_calibration(unsigned char * buffer, int encoderDirection);
void send_information_experiment_done(unsigned char * buffer, unsigned short experiment_length);

void prepare_message_to_PC_control_config(
		unsigned char * txBuffer,
		unsigned short control_period,
		bool controlSync,
		float angle_hanging,
		unsigned short angle_averageLen,
		bool correct_motor_dynamics
		);

void prepare_message_to_PC_config_PID(
		unsigned char * txBuffer,
		float position_KP,
		float position_KI,
		float position_KD,
		float angle_KP,
		float angle_KI,
		float angle_KD
		);

unsigned char crc(const unsigned char * buff, unsigned int len);

void prepare_buffer_to_send_long(unsigned char* Buffer, unsigned char CMD, unsigned int message_length);

#endif /*__COMMUNICATION_WITH_PC_H_*/
