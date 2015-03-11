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

    void (*set_drive_dir)(module*, int8_t);

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

    bool (*safety_check)(module*);

    /**
     * Does a PID update, and sets new speeds of motor for next tick.
     */
    void (*update)(module*);

    /**
     * Gets potentiometer reading, and divides by scale factor.
     */
    int16_t (*read_steer_pot_by_factor)(module*);

    int16_t (*read_steer_pot)(module *m);
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
    //should probably add some sort of error checking for all of these values
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
    m.PID_steer_output = m.steer_pos;
    m.drive_port = drive_port;
    m.drive_dir = 0; // Don't move originally.
    m.drive_speed = 0; // Again, let's not move originally.
    m.drive_motor = m.shield->getMotor(m.drive_port);
    //m.drive_PID = PID();
    m.PID_drive_output = m.drive_speed;
    
    //Function pointers
    m.set_steer_pos = set_steer_pos;
    m.go_home = go_home;
    m.stop_driving = stop_driving;
    m.error = error;
    m.update = update;

    return m;
}

/**
 * Method to change the steering position of a module
 * @param m         this
 * @param steer_pos Steering position from [steer_min_value, steer_max_value)
 */
void set_steer_pos(module *m, int16_t steer_pos) {
    if (m->steer_min_value <= steer_pos && steer_pos < m->steer_max_value){
        m->steer_pos = steer_pos;
    } else {
        m->error(m);
    }
}

void set_drive_dir(module *m, int8_t drive_dir) {
    m.drive_dir = drive_dir;
}

void set_drive_speed(module *m, int8_t drive_speed) {
    m.drive_speed = drive_speed;
}

/**
 * Safety method to return the module to home position
 * @param m this
 */
void go_home(module *m) {
    m->steer_pos = m->steer_home_value;
    return;
}

/**
 * Safety method to set the drive speed to 0 and release motor
 * @param m this
 */
void stop_driving(module *m) {
    m->drive_speed = 0;
    m->drive_dir = 0;
    return;
}

void error(module *m) {
    m->go_home(m);
    m->stop_driving(m);
    return;
}

/**
 * Checks to see whether module is at a safe operating point.
 * @param  m this
 * @return   Returns whether module is safe or not.
 */
bool safety_check(module *m) {
    int16_t current = m->read_steer_pot(m);
    return (m->steer_min_value <= current && current < m->steer_max_value)
}

void update(module *m) {
    //Begin with a safety check
    if (m->safety_check(m)) {
        //PID update
        //steer update, assumes negative PID is backward
        if (m->PID_steer_output < 0) {
            (m->steer_motor)->run(BACKWARD);
            (m->steer_motor)->setSpeed(abs(m->PID_steer_output));
        } else if (m->PID_steer_output == 0) {
            (m->steer_motor)->run(RELEASE);
            (m->steer_motor)->setSpeed(0);
        } else {
            (m->steer_motor)->run(FORWARD);
            (m->steer_motor)->setSpeed(m->PID_steer_output);
        }

        //drive update, assumes negative PID is backward
        if (m->PID_drive_output < 0) {
            (m->drive_motor)->run(BACKWARD);
            (m->drive_motor)->setSpeed(abs(m->PID_drive_output));
        } else if (m->PID_drive_output == 0) {
            (m->drive_motor)->run(RELEASE);
            (m->drive_motor)->setSpeed(0);
        } else {
            (m->drive_motor)->run(FORWARD);
            (m->drive_motor)->setSpeed(m->PID_drive_output));
        }
    } else {
        m->error(m);
    }
}

int16_t read_steer_pot_by_factor(module *m) {
    return analogRead(m->pot_port) / m->read_reduce_factor;
}

int16_t read_steer_pot(module *m) {
    return analogRead(m->pot_port);
}