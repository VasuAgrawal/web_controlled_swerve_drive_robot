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