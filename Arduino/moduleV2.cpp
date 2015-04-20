#include "moduleV2.h"

Module::Module(uint8_t pot_port, Adafruit_MotorShield *shield,
    uint8_t steer_port, uint16_t steer_home_value, uint16_t steer_min_value, uint16_t steer_max_value, uint8_t drive_port) {
    this->pot_port = pot_port;
    this->shield = shield;
    this->steer_port = steer_port;
    this->steer_home_value = steer_home_value;
    this->steer_min_value = steer_min_value;
    this->steer_max_value = steer_max_value;
    this->steer_pos = (double)(signed int)(this->steer_home_value);
    this->steer_motor = this->shield->getMotor(this->steer_port);
    this->PID_steer_input = read_steer_pot();
    this->PID_steer_output = this->steer_pos;
    this->steer_PID = PID_Improved(&PID_steer_input, &PID_steer_output,
        &steer_pos, 2, 0, 0); //tuning params
    steer_PID.set_error_threshold(5);
    this->drive_port = drive_port;
    this->drive_dir = 0; // Don't move originally.
    this->drive_speed = 0; // Again, let's not move originally.
    this->drive_motor = this->shield->getMotor(this->drive_port);
    //this->drive_PID = PID();
    //this->PID_drive_output = this->drive_speed;
}

uint8_t Module::read_steer_pot() {
    return analogRead(pot_port) >> 2;
}

void Module::set_steer_pos(uint16_t steer_pos) {
    if (steer_min_value <= steer_pos && steer_pos < steer_max_value){
        this->steer_pos = steer_pos;
    } else {
        error();
    }
}

void Module::set_drive_dir(int8_t drive_dir) {
    this->drive_dir = drive_dir;
}

void Module::set_drive_speed(uint8_t drive_speed) {
    this->drive_speed = drive_speed;
}

uint16_t Module::get_steer_pos() {
    return steer_pos;
}

int8_t Module::get_drive_dir() {
    return drive_dir;
}

uint8_t Module::get_drive_speed() {
    return drive_speed;
}

void Module::stop_driving() {
    drive_speed = 0;
    drive_dir = 0;
    return;
}

void Module::go_home() {
    steer_pos = steer_home_value;
    return;
}

void Module::error() {
    go_home();
    stop_driving();
    Serial.println("WE HAVE AN ERROR! :(");
    return;
}

bool Module::safety_check() {
    int8_t current = read_steer_pot();
    return steer_min_value <= current && current <= steer_max_value;
}

double Module::get_steer_PID_out() {
    return PID_steer_output;
}

void Module::update() {
    //Begin with a safety check
    if (safety_check()) {
        steer_PID.compute();
        //steer update, assumes negative PID is backward
        if (PID_steer_output < 0) {
            steer_motor->run(BACKWARD);
            steer_motor->setSpeed(abs(PID_steer_output));
        } else if (PID_steer_output == 0) {
            steer_motor->run(RELEASE);
            steer_motor->setSpeed(0);
        } else {
            steer_motor->run(FORWARD);
            steer_motor->setSpeed(PID_steer_output);
        }

        //drive update, assumes negative PID is backward
        if (drive_dir < 0) {
            drive_motor->run(BACKWARD);
            drive_motor->setSpeed(abs(drive_speed));
        } else if (drive_dir == 0) {
            drive_motor->run(RELEASE);
            drive_motor->setSpeed(0);
        } else {
            drive_motor->run(FORWARD);
            drive_motor->setSpeed(drive_speed);
        }
    } else {
        error();
    }
}
