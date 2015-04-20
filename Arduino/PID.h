/* Inspired by https://github.com/br3ttb/Arduino-PID-Library */

#include <Arduino.h>

#ifndef PID_IMPROVED_H
#define PID_IMPROVED_H

class PID_Improved {
    public:
        PID_Improved();
        PID_Improved(double *input, double *output, double *setpoint,
            double Kp, double Ki, double Kd);
        bool compute();
        void set_output_limits(double min, double max);
        void set_output_min_threshold(double min_thresh, double max_thresh);
        void set_error_threshold(double thresh);
        void set_compute_time(double refresh_time);

    private:
        double *input;
        double *output;
        double *setpoint;
        double Kp;
        double Ki;
        double Kd;
        double output_min;
        double output_max;
        double min_thresh;
        double max_thresh;
        double error_thresh;
        double start_time;
        double refresh_time;
};

#endif
