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

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

module module_init(int8_t pot_port, int8_t read_reduce_factor,
    int16_t read_min_value, int16_t read_max_value,
    Adafruit_MotorShield *shield, int8_t steer_port, int16_t steer_home_value,
    int16_t steer_min_value, int16_t steer_max_value, int8_t drive_port);
void set_steer_pos(module *m, int16_t steer_pos);
void go_home(module *m);
void stop_driving(module *m);
void error(module *m);
bool safety_check(module *m);
void update(module *m);
int16_t read_steer_pot_by_factor(module *m);
int16_t read_steer_pot(module *m);