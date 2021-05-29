import math
import logging
LOGGING_LEVEL = logging.INFO

CONTROLLER_NAME = 'PD'

CONTROL_PERIOD_MS = 2 # It was 25 originally, we changed it to 5 - marcin & asude
PATH_TO_CONTROLLERS = './Controllers/'  # Path where controllers are stored
PATH_TO_EXPERIMENT_RECORDINGS = './ExperimentRecordings/'  # Path where the experiments data is stored

MOTOR_FULL_SCALE = 8192  # 7199 # with pololu motor and scaling in firmware #7199 # with original motor
MOTOR_TYPE = 'POLOLU'
# MOTOR_TYPE = 'ORIGINAL'

# Angle unit conversion adc to radians: (ANGLE_TARGET + ANGLE DEVIATION - ANGLE_ADC_RANGE/2)/ANGLE_ADC_RANGE*math.pi
# ANGLE_KP_SOFTWARE = ANGLE_KP_FIRMWARE/ANGLE_NORMALIZATION_FACTOR/MOTOR_FULL_SCALE
ANGLE_AVG_LENGTH = 10  # adc routine in firmware reads ADC this many times quickly in succession to reduce noise
ANGLE_ADC_RANGE = 4096 # Range of angle values #
ANGLE_HANGING = 3195  # Value from sensor when pendulum is at stable equilibrium point
# ANGLE_HANGING = 3126 # Value from sensor when pendulum is at stable equilibrium point
ANGLE_DEVIATION = ANGLE_ADC_RANGE - ANGLE_HANGING # Angle deviation from goal
ANGLE_NORMALIZATION_FACTOR = 2 * math.pi / ANGLE_ADC_RANGE
ANGLE_HANGING_NORMALIZATION = (ANGLE_DEVIATION + ANGLE_HANGING - ANGLE_ADC_RANGE / 2) * ANGLE_NORMALIZATION_FACTOR # Should be equal to pi in radians

# Position unit conversion adc to meters: POSITION_TARGET_SOFTWARE = POSITION_TARGET_FIRMWARE*POSITION_NORMALIZATION_FACTOR
# POSITION_KP_SOFTWARE = POSITION_KP_FIRMWARE/POSITION_NORMALIZATION_FACTOR/MOTOR_FULL_SCALE
POSITION_ENCODER_RANGE = 4660 # This is an empirical approximation
POSITION_OFFSET = 0  # Serves to adjust starting position - position after calibration is 0
POSITION_FULL_SCALE_N = int(POSITION_ENCODER_RANGE) / 2 # Corrected position full scale - cart position should range over +- this value if calibrated for zero at center
TRACK_LENGTH = 0.396 # Total usable track length in meters
POSITION_NORMALIZATION_FACTOR = TRACK_LENGTH/POSITION_ENCODER_RANGE

# Direction for measurement.py - n = 2 for right, n = 1 for left.
n = 1

# SERIAL_PORT = None  # if None, takes first one available
SERIAL_PORT = 1  # index of the port on the list of all serial point (on Marcin's computer name changes, but it is always second port...)
SERIAL_BAUD = 230400  # default 230400, in firmware. Alternatives if compiled and supported by USB serial intervace are are 115200, 128000, 153600, 230400, 460800, 921600, 1500000, 2000000

ratio = 1.05

def inc(param):
    if param < 2:
        param = round(param + 0.1, 1)
    else:
        old = param
        param = round(param * ratio)
        if param == old:
            param += 1
    return param


def dec(param):
    if param < 2:
        param = max(0, round(param - 0.1, 1))
    else:
        old = param
        param = round(param / ratio)
        if param == old:
            param -= 1
    return param