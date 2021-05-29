# measurements from cartpole, controlled by state machine.
# control.py calls update_state() if state is not 'idle'
import time
from globals import *

# from .pendulum import Pendulum
STARTING_POSITION = -(-1)**n*0.17# cart starting position
ENDING_POSITION = (-1) ** n * 300 * TRACK_LENGTH / POSITION_ENCODER_RANGE # position to turn off motor
RESET_SPEED = (-1)**n*2500
SPEED_STEP = (-1)**n*250
STARTING_SPEED = (-1)**n*1000 #doesn't work for 500 for some reason
ENDING_SPEED = (-1)**n*8250
PAUSE_BEFORE_STEP_S = .5 # pause after reset to start position before starting step
FRICTION_SLOWDOWN_TIME_S=1 # time at end to just turn off motor and glide to stop
STEP_TIMEOUT_S=10

class StepResponseMeasurement:
    def __init__(self):
        self.state = 'idle'
        self.speed = RESET_SPEED
        self.motor = 0
        self.time_state_changed = time.time()

    def start(self):
        print('Start measurement!')
        self.state = 'start'
        self.motor = 0

    def stop(self):
        print('Stop measurement!')
        self.motor = 0
        self.state = 'idle'

    def get_state(self):
        return self.state

    def is_idle(self):
        return self.state == 'idle'

    def update_state(self, angle: int, position: int, time: float):
        if self.state == 'idle':
            self.motor = 0
            return
        elif self.state == 'start':  # init measurement
            self.speed = STARTING_SPEED
            self.state = 'resetting'
            self.time_state_changed = time
        elif self.state == 'resetting':  # moving back to start
            if (STARTING_POSITION < 0 and position > STARTING_POSITION) or (
                    STARTING_POSITION > 0 and position < STARTING_POSITION):
                self.motor = -RESET_SPEED
            else:
                self.motor = 0
                self.state = 'pausing_before_step'
                self.time_state_changed = time
            self._check_timeout(time)
        elif self.state == 'pausing_before_step':
            if (time - self.time_state_changed > PAUSE_BEFORE_STEP_S):
                self.state = 'starting_step'
                self.time_state_changed = time
        elif self.state == 'starting_step':
            if (abs(self.speed) < abs(ENDING_SPEED)):
                self.speed += SPEED_STEP
                self.motor = self.speed
                self.state = 'moving'
                self.time_state_changed = time
            else:
                self.speed = 0
                self.motor = (0)
                self.state = 'idle'
                self.time_state_changed = time
        elif self.state == 'moving':
            if (ENDING_POSITION>0 and position>ENDING_POSITION) or (ENDING_POSITION<0 and position<ENDING_POSITION):
                self.motor = 0
                self.state = 'friction_slowdown'
                self.time_state_changed = time
            self._check_timeout(time)
        elif self.state=='friction_slowdown':
            self.motor = 0
            if (time - self.time_state_changed > FRICTION_SLOWDOWN_TIME_S):
                self.state='resetting'
                self.time_state_changed = time
        else:
            raise Exception(f'unknown state {self.state}')

    def _check_timeout(self, time):
        if time - self.time_state_changed > STEP_TIMEOUT_S:
            self.stop()
            raise TimeoutError(f'step took more than {STEP_TIMEOUT_S} to complete')

    def __str__(self):
        return f'{self.state}:sp:{self.speed}:mot:{self.motor}'


