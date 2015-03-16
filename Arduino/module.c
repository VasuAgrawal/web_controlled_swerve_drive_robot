#include <module.h>

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
    m->drive_dir = drive_dir;
}

void set_drive_speed(module *m, int8_t drive_speed) {
    m->drive_speed = drive_speed;
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