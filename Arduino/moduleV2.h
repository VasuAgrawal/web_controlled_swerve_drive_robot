#ifndef MODULE_V2_H
#define MODULE_V2_H

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include "PID.h"

class Module {
    public:
        Module(uint8_t pot_port, Adafruit_MotorShield *shield,
            uint8_t steer_port, uint16_t steer_home_value,
            uint16_t steer_min_value, uint16_t steer_max_value,
            uint8_t drive_port);
        void set_steer_pos(uint16_t steer_pos);
        void set_drive_dir(int8_t drive_dir);
        void set_drive_speed(uint8_t drive_speed);
        uint16_t get_steer_pos();
        int8_t get_drive_dir();
        uint8_t get_drive_speed();
        void update();
        uint8_t read_steer_pot();
        double get_steer_PID_out();

    private:
        void go_home();
        void stop_driving();
        void error();
        bool safety_check();

        /**
         * Arduino analog read port for the potentiometer [0, 6). 
         */
        uint8_t pot_port;

        /**
         * Factor by which to reduce analog read measurements for consistency.
         */
        uint8_t read_reduce_factor;

        /**
         * Inclusive minimum value for analog read by Arduino (ie 0).
         */
        uint16_t read_min_value;
        /**
         * Exclusive maximum value for analog read by Arduino (ie 1024).
         */
        uint16_t read_max_value;
        
        /**
         * Pointer to Adafruit motor shield where motors are attached.
         * Both drive and steer motors must be atached to the same motor shield.
         */
        Adafruit_MotorShield *shield;

        /**
         * Motor shield port that the steer motor is connected to.
         */
        uint8_t steer_port;
        
        /**
         * Potentiometer reading for steer home value.
         * Must be between [steer_min_value, steer_max_value). This will be the
         * default value that the module will attempt to go initially, and if
         * there is any problem with connection.
         */
        uint16_t steer_home_value;
        /** 
         * Minimum safe potentiometer reading for the steering.
         * Given between [read_min_value, read_max_value). Make sure to ignore
         * the read_reduce_factor; this is accounted for automatically. If this
         * value is exceeded, error measures are taken to prevent damage.
         */
        uint16_t steer_min_value;
        /** 
         * Maximum safe potentiometer reading for the steering.
         * Given between [read_min_value, read_max_value]. Make sure to ignore
         * the read_reduce_factor; this is accounted for automatically. If this
         * value is exceeded, error measures are taken to prevent damage.
         */
        uint16_t steer_max_value;
        /**
         * Desired position for the steer potentiometer to be in.
         * Achieved using PID, will attempt to restore to this position whenever
         * possible. Will default to home value at start. Will be home value
         * if a value outside provided safe range is given.
         */
        double steer_pos; 
        /**
         * Adafruit_DCMotor object for the steer motor.
         * Obtained automatically based on the given steer_port and *shield. 
         */
        Adafruit_DCMotor* steer_motor;
        /**
         * PID controller for the steering motor.
         * Ensures that steering motor will arrive at necesssary position.
         */
        PID_Improved steer_PID;

        /**
         * PID controller steer pos input.
         * Holds the input value for the steering position from the PID loop.
         */
        double PID_steer_input;
        /**
         * PID controller steer pos output.
         * Holds the output value for the steering position from the PID loop.
         */
        double PID_steer_output;

        /**
         * Motor shield port that the drive motor is connected to.
         */
        uint8_t drive_port;
        /**
         * Direction in which to turn the motor.
         * drive_dir < 0 is BACKWARD.
         * drive_dir = 0 is NEUTRAL (RELEASE).
         * drive_dir > 0 is FORWARD.
         * Initially set to 0.
         */
        int8_t drive_dir;
        /**
         * Speed at which to turn the drive motor, based on drive_dir
         * drive_speed = 0 means go abs(drive_speed) NEUTRAL, so don't move.
         * drive_speed > 0 means go abs(drive_Speed) FORWARD.
         * Initially set to 0.
         */
        uint8_t drive_speed;
        /**
         * Adafruit_DCMotor object for the drive motor.
         * Obtained automatically based on the given drive port and *shield.
         */
        Adafruit_DCMotor* drive_motor;
        /**
         * PID controller for the drive motor.
         * Ensures that the drive motor will be going at the speed necessary.
         * NOT YET IMPLEMENTED.
         */
    //    PID drive_PID;

        /**
         * PID controller drive speed output.
         * Holds the output value for drive speed from the PID control loop.
         */
        uint8_t PID_drive_output;
};

#endif