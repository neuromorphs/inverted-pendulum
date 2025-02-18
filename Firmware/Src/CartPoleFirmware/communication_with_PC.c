#include "communication_with_PC.h"

unsigned char 	crc(const unsigned char * message, unsigned int len);
bool 			crcIsValid(const unsigned char * buff, unsigned int len, unsigned char crcVal);

int get_command_from_PC_message(unsigned char * rxBuffer, unsigned int* rxCnt){
	unsigned int			i;
	unsigned int 			idx;
	unsigned int			pktLen;
	int current_command = CMD_DO_NOTHING;
	// Buffer should have at least 4 bytes
	if (*rxCnt >= 4)
	{
		idx = 0;

		// Message must start with SOF character
    	if (rxBuffer[0] == SERIAL_SOF)
		{
			// Packet length must be less than the max
			pktLen = rxBuffer[2];
			if ((pktLen <= SERIAL_MAX_PKT_LENGTH) && (pktLen >= 4))
			{
				// Receive entire message packet (including CRC)
				if (*rxCnt >= pktLen)
				{
					// Validate message integrity
					if (crcIsValid(rxBuffer, pktLen-1, rxBuffer[pktLen-1]))
					{
						// Process message
						switch (rxBuffer[1])
						{
							case CMD_PING:
							{
								current_command = CMD_PING;
								break;
							}

                            case CMD_STREAM_ON:
                            {
                                if (pktLen == 5)
								{
                                	current_command = CMD_STREAM_ON;
								}
                                break;
                            }

							case CMD_CALIBRATE:
							{
								if (pktLen == 4)
								{
									current_command = CMD_CALIBRATE;
								}
								break;
							}

							case CMD_RUN_HARDWARE_EXPERIMENT:
                            {
                                if (pktLen == 4)
                                {
                                    current_command = CMD_RUN_HARDWARE_EXPERIMENT;
                                }
                                break;
                            }


                            case CMD_TRANSFER_BUFFERS:
                            {
                                if (pktLen == 4)
                                {
                                    current_command = CMD_TRANSFER_BUFFERS;
                                }
                                break;
                            }



							case CMD_CONTROL_MODE:
							{
								if (pktLen == 5)
								{
									current_command = CMD_CONTROL_MODE;
								}
								break;
							}

							case CMD_SET_PID_CONFIG:
							{
								if (pktLen == 34)
								{
									current_command = CMD_SET_PID_CONFIG;
								}
								break;
							}

							case CMD_GET_PID_CONFIG:
							{
								if (pktLen == 4)
								{
									current_command = CMD_GET_PID_CONFIG;
								}
								break;
							}

							case CMD_SET_CONTROL_CONFIG:
							{
								if (pktLen == 14)
								{
									current_command = CMD_SET_CONTROL_CONFIG;
								}
								break;
							}

							case CMD_GET_CONTROL_CONFIG:
							{
								if (pktLen == 4)
								{
									current_command = CMD_GET_CONTROL_CONFIG;
								}
								break;
							}

							case CMD_SET_MOTOR:
							{
								if (pktLen == 8)
								{
									current_command = CMD_SET_MOTOR;
								}
								break;
							}

							case CMD_SET_TARGET_POSITION:
							{
								if (pktLen == 8)
								{
									current_command = CMD_SET_TARGET_POSITION;
								}
								break;
							}

							case CMD_SET_TARGET_EQUILIBRIUM:
							{
								if (pktLen == 8)
								{
									current_command = CMD_SET_TARGET_EQUILIBRIUM;
								}
								break;
							}

							case CMD_COLLECT_RAW_ANGLE:
							{
								if (pktLen == 8)
								{
									current_command = CMD_COLLECT_RAW_ANGLE;
								}
								break;
							}

							default:
							{
								break;
							}
						}

						idx = pktLen;   // Trim message
					}
					else
					{
						idx = 1;  // Trim SOF and start looking for the next packet
					}
				}
			}
			else
			{
				idx = 1;  // Trim SOF and start looking for the next packet
			}
		}
		else
		{
			idx = 1;  // Trim SOF and start looking for the next packet
		}

		// Shift buffer until first character is SOF
		if (idx != 0)
		{
			for (; idx < *rxCnt; idx++)
			{
				if (rxBuffer[idx] == SERIAL_SOF)
				{
					break;
				}
			}

			*rxCnt -= idx;	// This is not correctly modified!
			for (i = 0; i < *rxCnt; i++)
			{
				rxBuffer[i] = rxBuffer[idx+i];
			}
		}
	}

	return current_command;
}

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
		unsigned long time_current_measurement,
		unsigned long latency,
		unsigned short	latency_violation
		){

	buffer[ 0] = SERIAL_SOF;
	buffer[ 1] = CMD_STATE;
	buffer[ 2] = message_len;
	*((short *)&buffer[3]) = angle;
	*((float *)&buffer[5]) = angleD_unprocessed;
	*((short *)&buffer[9]) = position;
	*((float *)&buffer[11]) = target_position;
	*((short *)&buffer[15]) = motor_command;
	*((unsigned char *)&buffer[17]) = invalid_step;
	*((unsigned int *)&buffer[18]) = (unsigned int)time_difference_between_measurement;
	*((unsigned int *)&buffer[22]) = (unsigned int)time_current_measurement;
	*((unsigned short *)&buffer[26]) = (unsigned short)(latency / 10);
	*((unsigned short *)&buffer[28]) = (unsigned short)(latency_violation);
	// latency maximum: 10 * 65'535 Us = 653ms
	buffer[message_len-1] = crc(buffer, message_len-1);
}

void prepare_message_to_PC_calibration(unsigned char * buffer, int encoderDirection){
	buffer[ 0] = SERIAL_SOF;
	buffer[ 1] = CMD_CALIBRATE;
	buffer[ 2] = 5;
	*((signed char *)&buffer[3]) = (signed char)encoderDirection;
	buffer[4] = crc(buffer, 4);
}

void send_information_experiment_done(unsigned char * buffer, unsigned short experiment_length){

	buffer[ 0] = SERIAL_SOF;
	buffer[ 1] = CMD_RUN_HARDWARE_EXPERIMENT;
	buffer[ 2] = 6;
	*((unsigned short *)&buffer[3]) = (unsigned short)experiment_length;
	buffer[5] = crc(buffer, 5);
}

void prepare_message_to_PC_control_config(
		unsigned char * txBuffer,
		unsigned short control_period,
		bool controlSync,
		float angle_hanging,
		unsigned short angle_averageLen,
		bool correct_motor_dynamics
		){

	txBuffer[ 0] = SERIAL_SOF;
	txBuffer[ 1] = CMD_GET_CONTROL_CONFIG;
	txBuffer[ 2] = 14;
	*((unsigned short *)&txBuffer[ 3]) = control_period;
	*((bool           *)&txBuffer[ 5]) = controlSync;
	*((float          *)&txBuffer[6]) = angle_hanging;
	*((unsigned short *)&txBuffer[10]) = angle_averageLen;
	*((bool           *)&txBuffer[ 12]) = controlSync;
	txBuffer[13] = crc(txBuffer, 13);

}


void prepare_message_to_PC_config_PID(
		unsigned char * txBuffer,
		float position_KP,
		float position_KI,
		float position_KD,
		float angle_KP,
		float angle_KI,
		float angle_KD
		){

	txBuffer[ 0] = SERIAL_SOF;
	txBuffer[ 1] = CMD_GET_PID_CONFIG;
	txBuffer[ 2] = 28;

	*((float          *)&txBuffer[3]) = position_KP;
	*((float          *)&txBuffer[7]) = position_KI;
	*((float          *)&txBuffer[11]) = position_KD;

	*((float          *)&txBuffer[15]) = angle_KP;
	*((float          *)&txBuffer[19]) = angle_KI;
	*((float          *)&txBuffer[23]) = angle_KD;

	txBuffer[27] = crc(txBuffer, 27);

}


unsigned char crc(const unsigned char * buff, unsigned int len)
{
    unsigned char crc8 = 0x00;
	unsigned char val;
	unsigned char sum;
	unsigned int  i;

    while (len--)
    {
        val = *buff++;
        for (i = 0; i < 8; i++)
        {
            sum = (crc8 ^ val) & 0x01;
            crc8 >>= 1;
            if (sum > 0)
            {
                crc8 ^= 0x8C;
            }
            val >>= 1;
        }
    }
    return crc8;
}


bool crcIsValid(const unsigned char * buff, unsigned int len, unsigned char crcVal)
{
    return crcVal == crc(buff, len);
}

void prepare_buffer_to_send_long(unsigned char* Buffer, unsigned char CMD, unsigned int message_length) {
    Buffer[0] = SERIAL_SOF; // Assuming SERIAL_SOF is defined elsewhere
    Buffer[1] = CMD;
    *((unsigned int*)&Buffer[2]) = message_length; // Correctly places message_length in the buffer

    // Assuming crc function exists and calculates CRC correctly
    // This assumes the CRC is to be placed at the end of the message and message_length includes the CRC byte
    Buffer[message_length - 1] = crc(Buffer, message_length - 1);
}
