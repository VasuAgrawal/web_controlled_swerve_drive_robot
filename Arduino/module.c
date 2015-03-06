#include <module.h>

struct module_assembly {
    /**
     * Arduino analog read port for the potentiometer [0, 6). 
     */
    int8_t pot_port;

    /**
     * Factor by which to reduce analog read measurements for consistency.
     */
    int8_t read_reduce_factor;

    /**
     * Inclusive minimum value for analog read by Arduino (ie 0).
     */
    int16_t read_min_value;
    /**
     * Exclusive maximum value for analog read by Arduino (ie 1024).
     */
    int16_t read_max_value;
    
    /**
     * Pointer to Adafruit motor shield where motors are attached.
     * Both drive and steer motors must be atached to the same motor shield.
     */
    Adafruit_MotorShield *shield;

    /**
     * Motor shield port that the steer motor is connected to.
     */
    int8_t steer_port;
    /**
     * Potentiometer reading for steer home value.
     * Must be between [steer_min_value, steer_max_value). This will be the
     * default value that the module will attempt to go initially, and if
     * there is any problem with connection.
     */
    int16_t steer_home_value;
    /** 
     * Minimum safe potentiometer reading for the steering.
     * Given between [read_min_value, read_max_value). Make sure to ignore
     * the read_reduce_factor; this is accounted for automatically. If this
     * value is exceeded, error measures are taken to prevent damage.
     */
    int16_t steer_min_value;
    /** 
     * Maximum safe potentiometer reading for the steering.
     * Given between [read_min_value, read_max_value). Make sure to ignore
     * the read_reduce_factor; this is accounted for automatically. If this
     * value is exceeded, error measures are taken to prevent damage.
     */
    int16_t steer_max_value;
    /**
     * Desired position for the steer potentiometer to be in.
     * Achieved using PID, will attempt to restore to this position whenever
     * possible. Will default to home value at start. Will be home value
     * if a value outside provided safe range is given.
     */
    int8_t steer_pos; 
    /**
     * Adafruit_DCMotor object for the steer motor.
     * Obtained automatically based on the given steer_port and *shield. 
     */
    Adafruit_DCMotor* steer_motor;
    /**
     * PID controller for the steering motor.
     * Ensures that steering motor will arrive at necesssary position.
     */
    PID steer_PID;

    /**
     * Motor shield port that the drive motor is connected to.
     */
    int8_t drive_port;
    /**
     * Direction in which to turn the motor.
     * drive_dir < 0 is BACKWARD.
     * drive_dir = 0 is NEUTRAL.
     * drive_dir > 0 is FORWARD.
     * Initially set to 0.
     */
    int8_t drive_dir;
    /**
     * Speed at which to turn the drive motor.
     * drive_speed < 0 means go abs(drive_speed) BACKWARD.
     * drive_speed = 0 means go abs(drive_speed) NEUTRAL, so don't move.
     * drive_speed > 0 means go abs(drive_Speed) FORWARD.
     * Initially set to 0.
     */
    int8_t drive_speed;
    /**
     * Adafruit_DCMotor object for the drive motor.
     * Obtained automatically based on the given drive port and *shield.
     */
    Adafuit_DCMotor* drive_motor;
    /**
     * PID controller for the drive motor.
     * Ensures that the drive motor will be going at the speed necessary.
     * NOT YET IMPLEMENTED.
     */
    PID drive_PID;
};

typedef struct module_assembly module;

/**
 * Initialization function for a module struct.
 * @param  pot_port           See struct definition.
 * @param  read_reduce_factor See struct definition.
 * @param  read_min_value     See struct definition.
 * @param  read_max_value     See struct definition.
 * @param  shield             See struct definition.
 * @param  steer_port         See struct definition.
 * @param  steer_home_value   See struct definition.
 * @param  steer_min_value    See struct definition.
 * @param  steer_max_value    See struct definition.
 * @param  drive_port         See struct definition.
 * @return                    Returns a module with initialized values.
 */
module module_init(int8_t pot_port, int8_t read_reduce_factor,
    int16_t read_min_value, int16_t read_max_value,
    Adafruit_MotorShield *shield, int8_t steer_port, int16_t steer_home_value,
    int16_t steer_min_value, int16_t steer_max_value, int8_t drive_port) {
    
    module m;
    m.pot_port = pot_port;
    m.read_reduce_factor = read_reduce_factor;
    m.read_min_value = read_min_value;
    m.read_max_value = read_max_value;
    m.shield = shield;
    m.steer_port = steer_port;
    m.steer_home_value = steer_home_value;
    m.steer_min_value = steer_min_value;
    m.steer_max_value = steer_max_value;
    m.steer_pos = m.steer_home_value;
    m.steer_motor = m.shield->getMotor(m.steer_port);
    //m.steer_PID = PID();
    m.drive_port = drive_port;
    m.drive_dir = 0; // Don't move originally.
    m.drive_speed = 0; // Again, let's not move originally.
    m.drive_motor = m.shield->getMotor(m.drive_port);
    //m.drive_PID = PID();
}