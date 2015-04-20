#include "PID.h"

PID_Improved::PID_Improved() {
    //blank constructor because ...
}

PID_Improved::PID_Improved(double *input, double *output, double *setpoint,
    double Kp, double Ki, double Kd) {

    if (Ki != 0 || Kd != 0) {
        1 / 0; // not yet implemented
    }

    this->input = input;
    this->output = output;
    this->setpoint = setpoint;
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;

    this->output_min = -1;
    this->output_max = 1;
    this->min_thresh = 1;
    this->max_thresh = -1;
    this->error_thresh = 0;

    this->start_time = millis();
    this->refresh_time = 200;
}

void PID_Improved::set_output_limits(double min, double max) {
    output_min = min;
    output_max = max;
}

void PID_Improved::set_output_min_threshold(double min_thresh, double max_thresh) {
    this->min_thresh = min_thresh;
    this->max_thresh = max_thresh;
}

void PID_Improved::set_error_threshold(double thresh) {
    error_thresh = thresh;
}

void PID_Improved::set_compute_time(double refresh_time) {
    this->refresh_time = refresh_time;
}

bool PID_Improved::compute() {
    if (millis() - start_time < refresh_time) {
        return false;
    } else {
        double my_input = *input;
        double my_setpoint = *setpoint;
        double error = my_setpoint - my_input; //higher -> backward,low -> forward

        if (abs(error) <= error_thresh) {
            *output = 0;
            return true; // Arrived at position, no need to change anything
        }
        
        double my_output;
        my_output = Kp * error; // scale the error by Kp

        //cap it at min / max first
        if (my_output < output_min) {
            my_output = output_min;
        } else if (my_output > output_max) {
            my_output = output_max;
        }

        //case on direction:
        if (error <= -error_thresh) {
            if (my_output > min_thresh) {
                my_output = min_thresh;
            }
        } else if (error >= error_thresh) {
            if (my_output < max_thresh) {
                my_output = max_thresh;
            }
        }

        *output = my_output;
        return true;
    }
}