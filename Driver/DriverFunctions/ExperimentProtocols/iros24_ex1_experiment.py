# measurements from cartpole, controlled by state machine.
# control.py calls update_state() if state is not 'idle'
from DriverFunctions.ExperimentProtocols import template_experiment_protocol
from CartPole.state_utilities import (
    ANGLE_IDX,
    ANGLED_IDX,
    POSITION_IDX,
    POSITIOND_IDX,
)

NUMBER_OF_ITERATIONS = 1

TARGET_POSITION_0 = -0.1
TARGET_POSITION_1 = 0.1
TARGET_POSITION_2 = -0.1

TIME_OF_EXPERIMENT = 8.0
TIME_FOR_SWINGUP = 5.0
RECALIBRATE_EVERY_N_SWING_UPS = None

SKIP_RESET = False  # Use for PID to avoid


class swing_up_experiment(template_experiment_protocol):
    def __init__(self, driver):
        super().__init__(
            driver=driver,
            experiment_protocol_name=self.__class__.__name__[:-len('_experiment')], )

        self.counter_iterations = 0

    def set_up_experiment(self, first_iteration=True):
        if first_iteration:
            self.counter_iterations = 0

        if RECALIBRATE_EVERY_N_SWING_UPS is not None and self.counter_iterations % RECALIBRATE_EVERY_N_SWING_UPS == 0:
            print("\nCalibrating motor position.... ")
            self.driver.InterfaceInstance.calibrate()
            print("Done calibrating")

        self.target_position = TARGET_POSITION_0
        self.target_equilibrium = -1
        
        # self.driver.controlEnabled = True  # We are not enabling control automatically in case we want to use hardware controller and only provide target position and equilibrium from PC
        if SKIP_RESET:
            self.start_new_recording(index=self.counter_iterations)
            self.current_experiment_phase = 'swingup'
        else:
            self.current_experiment_phase = 'reset'

    def update_state(self, angle, position, time):
        self.angle = angle
        self.angleD = self.driver.s[ANGLED_IDX]
        self.position = position
        self.positionD = self.driver.s[POSITIOND_IDX]
        self.time = time

        if self.current_experiment_phase == 'idle':
            pass
        elif self.current_experiment_phase == 'reset':
            self.action_reset()
        elif self.current_experiment_phase == 'swingup':
            self.action_swing_up()
        elif self.current_experiment_phase == 'go-to-target':
            self.action_go_to_target()
        else:
            raise Exception(f'unknown experiment phase: {self.current_experiment_phase}')


    def action_reset(self):

        if (abs(self.position - self.target_position) < 0.01
                and
                abs(self.angle) < 0.1
                and abs(self.angleD) < 0.1
        ):
            self.start_new_recording(index=self.counter_iterations)
            self.target_position = TARGET_POSITION_1
            self.target_equilibrium = 1
            self.time_start_stable_down = self.time
            self.current_experiment_phase = 'swingup'


    def action_swing_up(self):
        if self.time - self.time_start_stable_down >= TIME_FOR_SWINGUP:
            self.target_position = TARGET_POSITION_2
            self.current_experiment_phase = 'go-to-target'

    def action_go_to_target(self):
        if self.time - self.time_start_stable_down > TIME_OF_EXPERIMENT:
            self.counter_iterations += 1
            self.finish_recording()
            if self.counter_iterations >= NUMBER_OF_ITERATIONS:
                self.stop()
                self.driver.controller.controller_report()
            else:
                self.set_up_experiment(first_iteration=False)