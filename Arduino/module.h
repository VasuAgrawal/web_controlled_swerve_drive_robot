/* A physical module consists of a wheel mounted on a drive motor,
 * which is mounted in a clevice made of acrylic. The clevice has a
 * hex bore and has a gear attached to the top, which also have hex
 * bores. There is a hex shaft coming out of a steering motor; the
 * hex shaft penetrates both the gear and the clevice, thus linking
 * the motons. Attached to the gear is another gear, whose axle is
 * the shaft of a potentiomter. Thus, rotation of the steer motor
 * will turn the potentiometer for feedback, and will turn the
 * clevice for the drive motor.
 *
 * Each module is linked in code to its corresponding motor shield.
 * This means that this code module is completely capable of
 * controlling the steering and driving of a module precisely.
 */

#ifndef MODULE_H
#define MODULE_H

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

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
     * Given between [read_min_value, read_max_value]. Make sure to ignore
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
    int16_t steer_pos; 
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
     * PID controller steer pos output.
     * Holds the output value for the steering position from the PID loop.
     */
    int16_t PID_steer_output;

    /**
     * Motor shield port that the drive motor is connected to.
     */
    int8_t drive_port;
    /**
     * Direction in which to turn the motor.
     * drive_dir < 0 is BACKWARD.
     * drive_dir = 0 is NEUTRAL (RELEASE).
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

    /**
     * PID controller drive speed output.
     * Holds the output value for drive speed from the PID control loop.
     */
    int8_t PID_drive_output;

    /**
     * Function to set the steering position.
     */
    void (*set_steer_pos)(module*, int16_t);

    /**
     * Sets the direction for the drive motor.
     */
    void (*set_drive_dir)(module*, int8_t);

    /**
     * Sets the speed for the drive motor.
     */
    void (*set_drive_speed)(module*, int8_t);

    /**
     * Sets the steering position to be the home position as specified.
     */
    void (*go_home)(module*);

    /**
     *  Sets the drive speed to be zero, and releases the motor.
     */
    void (*stop_driving)(module*);

    /**
     * Called if any unsafe behavior, attempts to arrest all motion.
     */
    void (*error)(module*);

    /**
     * Checks to see whether module is within safe operational limit.
     */
    bool (*safety_check)(module*);

    /**
     * Does a PID update, and sets new speeds of motor for next tick.
     */
    void (*update)(module*);

    /**
     * Gets potentiometer reading, and divides by scale factor.
     */
    int16_t (*read_steer_pot_by_factor)(module*);

    /**
     * Gets raw potentiomter reading.
     */
    int16_t (*read_steer_pot)(module *m);
};

typedef struct module_assembly module;

module module_init(int8_t pot_port, int8_t read_reduce_factor,
    int16_t read_min_value, int16_t read_max_value,
    Adafruit_MotorShield *shield, int8_t steer_port, int16_t steer_home_value,
    int16_t steer_min_value, int16_t steer_max_value, int8_t drive_port);
void set_steer_pos(module *m, int16_t steer_pos);
void set_drive_dir(module *m, int8_t drive_dir);
void set_drive_speed(module *m, int8_t drive_speed);
void go_home(module *m);
void stop_driving(module *m);
void error(module *m);
bool safety_check(module *m);
void update(module *m);
int16_t read_steer_pot_by_factor(module *m);
int16_t read_steer_pot(module *m);

#endif