#include "neural_imitator.h"

#include "xparameters.h"

#include "buttons_and_switches.h"


#include "fixed_point.hpp"

#include "TFmicro/TFmicro_Network.h"

#ifdef XPAR_HARDWARE_ACCEL_EDGEDRNN_AXI_DMA_1_DEVICE_ID
#define EdgeDRNN
#endif

#ifdef EdgeDRNN
#include "EdgeDRNN/EdgeDRNN_Network.h"
#endif

#ifdef XPAR_HARDWARE_ACCEL_HLS4ML_AXI_DMA_0_DEVICE_ID
#define HLS4ML
#endif

#ifdef HLS4ML
#include "HLS4ML/HLS4ML_Network.h"
#endif


#define NETWORKS_SWITCH_NUMBER	3

#define MEM_BASE_ADDR		0x01000000

#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)

// Normalization Cartpole hpf_v2024_x3232_12_2_v1
//float hls_normalize_a[] = {0.04595453,1.00000000,1.00000000,5.21186209,0.82011247,1.00000000,6.31313133
//};
//float hls_normalize_b[] = {0.02537525,0.00000000,0.00000000,0.01761615,-0.05823207,0.00000000,0.00000000
//};
//float hls_denormalize_A[] = {1.0};
//float hls_denormalize_B[] = {0.0};


//// Normalization Cartpole hpf_v2024_cpp_x3232_12_2_v2
//float hls_normalize_a[] = {0.05373850,1.00000000,1.00000000,5.44010401,0.86096680,1.00000000,6.31313133
//};
//float hls_normalize_b[] = {-0.07883823,0.00000000,0.00000000,0.01648343,-0.01449436,0.00000000,0.00000000
//};
//float hls_denormalize_A[] = {1.0};
//float hls_denormalize_B[] = {0.0};

// Normalization Cartpole hpf_v2024_cpp_x3232_14_3_short_v1
float hls_normalize_a[] = {0.05373850,1.00000000,1.00000000,5.44010401,0.86096680,1.00000000,6.31313133
};
float hls_normalize_b[] = {-0.07883823,0.00000000,0.00000000,0.01648343,-0.01449436,0.00000000,0.00000000
};
float hls_denormalize_A[] = {1.0};
float hls_denormalize_B[] = {0.0};

// Normalization f1t 31
//float hls_normalize_a[] = {3.10077524,2.03045702,1.03896105,0.62952471,0.43591982,0.32637075,0.25913447,0.21468441,0.18336849,1.39860129,1.39860129,0.68027210,0.44179368,0.32976091,0.26413101,0.22121446,0.19166268,0.16701461,0.14775415,0.14385384,0.14385384,0.14385384,0.14385384,0.14385384,0.14385384,0.14385384,0.14385384,0.12087514,0.14740567,1.26502204,2.23214269
//};
//float hls_normalize_b[] = {-2.16899228,-2.69137049,-1.71376622,-1.14667928,-0.83347863,-0.63315928,-0.50323915,-0.41713184,-0.35711008,0.06713283,0.06713283,0.03877544,0.02628672,0.01665294,0.00449026,0.00940156,0.01102066,0.01244247,0.01019502,-1.24843562,-1.24843562,-1.24843562,-1.24843562,-1.24843562,-1.24843562,-1.24843562,-1.24843562,0.03046048,-1.09772992,-0.05249846,0.00000000
//};
//float hls_denormalize_A[] = {0.78250003,8.06599998
//};
//float hls_denormalize_B[] = {-0.01349998,7.27099991
//};


float tfmicro_normalize_a[] = {0.05373850,1.00000000,1.00000000,5.44010401,0.86096680,1.00000000,6.31313133
};
float tfmicro_normalize_b[] = {-0.07883823,0.00000000,0.00000000,0.01648343,-0.01449436,0.00000000,0.00000000
};
float tfmicro_denormalize_A[] = {1.0};
float tfmicro_denormalize_B[] = {0.0};

// EdgeDRNN input buffer
short* edgedrnn_stim; //[] = {131,256,8,36,2,256,14,0};


void Neural_Imitator_Init()
{

#ifdef HLS4ML
	HLS4ML_Network_Init();
#endif

#ifdef EdgeDRNN
	EdgeDRNN_Network_Init();
#endif

#ifdef TF_MICRO
    TFmicro_Network_Init();
#endif

}


void Neural_Imitator_Evaluate(unsigned char * network_input_buffer, unsigned char * network_output_buffer)
{

	int32_t *TxBufferPtr;
	int32_t *RxBufferPtr;

	TxBufferPtr = (int32_t *) TX_BUFFER_BASE;
	RxBufferPtr = (int32_t *) RX_BUFFER_BASE;

	float actv_floating_point, predic_floating_point;
	int32_t actv_fixed_point_32;
	short actv_fixed_point_16;
	int32_t predic_fixed_point_32;

	NeuralNetworkType active_network;

    // Determine active network based on the switch state
    if (Switch_GetState(NETWORKS_SWITCH_NUMBER)) {
        active_network = SELECTED_NETWORK_UP;
    } else {
        active_network = SELECTED_NETWORK_DOWN;
    }


    switch (active_network) {
        case NETWORK_EDGEDRNN:
    #ifdef EdgeDRNN
            {
                // Use EdgeDRNN accelerator

                for (int neuron_idx = 0; neuron_idx < MLP_ACTIVATION_NEURONS; neuron_idx++) {
                    actv_floating_point = *((float*)&network_input_buffer[neuron_idx * DATA_WORD_BYTES]);
                    actv_fixed_point_16 = float_to_fixed_16(actv_floating_point, 8);
                    edgedrnn_stim[neuron_idx] = actv_fixed_point_16;
                }
                edgedrnn_stim[7] = 0; // FIXME: This is probably just setting the target equilibrium or position to 0

                predic_floating_point = EdgeDRNN_Network_Evaluate((short*)(edgedrnn_stim));

                for (int neuron_idx = 0; neuron_idx < MLP_PREDICTION_NEURONS; neuron_idx++) {
                    *((float*)&network_output_buffer[neuron_idx * DATA_WORD_BYTES]) = predic_floating_point;
                }
            }
    #endif
            break;

        case NETWORK_HLS4ML:
    #ifdef HLS4ML
            {
                // Use MLP accelerator

                for (int neuron_idx = 0; neuron_idx < MLP_ACTIVATION_NEURONS; neuron_idx++) {
                    actv_floating_point = *((float*)&network_input_buffer[neuron_idx * DATA_WORD_BYTES]);
                    actv_floating_point = hls_normalize_a[neuron_idx] * actv_floating_point + hls_normalize_b[neuron_idx];
                    actv_fixed_point_32 = float_to_fixed_32(actv_floating_point, MLP_TOTAL_BITS_PER_VARIABLE - MLP_INTEGER_PLUS_SIGN_BITS_PER_VARIABLE);
                    TxBufferPtr[neuron_idx] = actv_fixed_point_32;
                }

                HLS4ML_Network_Evaluate((UINTPTR)TxBufferPtr, NETWORK_INPUT_SIZE_IN_BYTES, (UINTPTR)RxBufferPtr, NETWORK_OUTPUT_SIZE_IN_BYTES);

                for (int neuron_idx = 0; neuron_idx < MLP_PREDICTION_NEURONS; neuron_idx++) {
                    predic_fixed_point_32 = extend_sign_32(RxBufferPtr[neuron_idx], MLP_TOTAL_BITS_PER_VARIABLE - 1);
                    predic_floating_point = fixed_to_float_32(predic_fixed_point_32, MLP_TOTAL_BITS_PER_VARIABLE - MLP_INTEGER_PLUS_SIGN_BITS_PER_VARIABLE);
                    predic_floating_point = hls_denormalize_A[neuron_idx] * predic_floating_point + hls_denormalize_B[neuron_idx];
                    *((float*)&network_output_buffer[neuron_idx * DATA_WORD_BYTES]) = predic_floating_point;
                }
            }
    #endif
            break;

        case NETWORK_TFMICRO:
    #ifdef TF_MICRO
            {
                float inputs[TF_MICRO_NUMBER_OF_INPUTS];
                float outputs[TF_MICRO_NUMBER_OF_OUTPUTS];

                for (int neuron_idx = 0; neuron_idx < TF_MICRO_NUMBER_OF_INPUTS; neuron_idx++) {
                    actv_floating_point = *((float*)&network_input_buffer[neuron_idx * DATA_WORD_BYTES]);
                    inputs[neuron_idx] = tfmicro_normalize_a[neuron_idx] * actv_floating_point + tfmicro_normalize_b[neuron_idx];
                }

                TFmicro_Network_Evaluate(inputs, outputs);

                for (int neuron_idx = 0; neuron_idx < TF_MICRO_NUMBER_OF_OUTPUTS; neuron_idx++) {
                    predic_floating_point = outputs[neuron_idx];
                    predic_floating_point = tfmicro_denormalize_A[neuron_idx] * predic_floating_point + tfmicro_denormalize_B[neuron_idx];
                    *((float*)&network_output_buffer[neuron_idx * DATA_WORD_BYTES]) = predic_floating_point;
                }
            }
    #endif
            break;

        default:
        {
            for (int neuron_idx = 0; neuron_idx < MLP_PREDICTION_NEURONS; neuron_idx++) {
                *((float*)&network_output_buffer[neuron_idx * DATA_WORD_BYTES]) = 0.0;
            }
        }
            break;
    }
}


void Neural_Imitator_ReleaseResources(){
#ifdef EdgeDRNN
	EdgeDRNN_Network_ReleaseResources();
#endif
}

float neural_imitator_cartpole_step(float angle, float angleD, float angle_cos, float angle_sin, float position, float positionD, float target_equilibrium, float target_position, float time)
{
	// Define input and output buffers
	// Load proper values into input buffer
	static unsigned char network_input_buff[NETWORK_INPUT_SIZE_IN_BYTES];
	static unsigned char network_output_buff[NETWORK_OUTPUT_SIZE_IN_BYTES];

//	float network_input_buff[] = {angleD, angle_cos, angle_sin, position, positionD, target_equilibrium, target_position};

	*((float *)&network_input_buff[0]) = angleD;
	*((float *)&network_input_buff[4]) = angle_cos;
	*((float *)&network_input_buff[8]) = angle_sin;
	*((float *)&network_input_buff[12]) = position;
	*((float *)&network_input_buff[16]) = positionD;
	*((float *)&network_input_buff[20]) = target_equilibrium;
	*((float *)&network_input_buff[24]) = target_position;

	// Evaluate network value
//	Neural_Imitator_Evaluate((unsigned char*)network_input_buff, network_output_buff);
	Neural_Imitator_Evaluate(network_input_buff, network_output_buff);

	// Output buffer for cartpole has just 1 element and this is the requested control input
	float Q = *((float          *)&network_output_buff[ 0]);
	return Q;
}

