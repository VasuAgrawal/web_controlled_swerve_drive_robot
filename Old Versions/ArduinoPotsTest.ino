// // #include <Bridge.h>
// // #include <YunServer.h>
// // #include <YunClient.h>
// // #include <Wire.h>
// // #include <Adafruit_MotorShield.h>
// // #include "utility/Adafruit_PWMServoDriver.h"
// // #include <PID_v1.h>

// // #define PORT 6666
// // #define DRIVE_MOTOR_SPEED_KEY 0
// // #define DRIVE_MOTOR_DIR_KEY 1
// // #define STEER_MOTOR_POSITION_KEY 2
// // #define STEER_MOTOR_DIR_KEY 3

// // #define KEY_SIZE 5
// // #define SPEC_SIZE 3
// // #define VALUE_SIZE 8

// // YunServer server(PORT);

// // int8_t driveMotorSpeeds[] = {0, 0, 0, 0};
// // int8_t driveMotorDirs[] = {0, 0, 0, 0};
// // int8_t steerMotorPositions[] = {0, 0, 0, 0}; // this will be the home position
// // int8_t steerMotorDirs[] = {0, 0, 0, 0}; // turning direction for wheels
// // int8_t steerPotPins[] = {10, 15, 20, 25};

// // Adafruit_MotorShield shield1 = Adafruit_MotorShield(0x60); // bottom shield
// // Adafruit_MotorShield shield2 = Adafruit_MotorShield(0x61); // top shield

// // Adafruit_DCMotor *driveMotors[4];
// // Adafruit_DCMotor *steerMotors[4];

// // void setup() {
// //   driveMotors[0] = shield1.getMotor(2);
// //   driveMotors[1] = shield1.getMotor(4);
// //   driveMotors[2] = shield2.getMotor(2);
// //   driveMotors[3] = shield2.getMotor(4);

// //   steerMotors[0] = shield1.getMotor(1);
// //   steerMotors[1] = shield1.getMotor(3);
// //   steerMotors[2] = shield2.getMotor(1);
// //   steerMotors[3] = shield2.getMotor(3);

// //   shield1.begin();
// //   shield2.begin();

// //   // this will act as the server
// //   Serial.begin(115200);
// //   Bridge.begin();
  
// //   server.noListenOnLocalhost();
// //   server.begin();
// // }

// // void loop() {

// //   for (int i = 0; i < 4; i++){
// //     Serial.print(analogRead(i) >> 4);
// //     Serial.print(" ");
// //   }

// //   Serial.println();

// //   delay(200);
// // }

// /********************************************************
//  * PID Adaptive Tuning Example
//  * One of the benefits of the PID library is that you can
//  * change the tuning parameters at any time.  this can be
//  * helpful if we want the controller to be agressive at some
//  * times, and conservative at others.   in the example below
//  * we set the controller to use Conservative Tuning Parameters
//  * when we're near setpoint and more agressive Tuning
//  * Parameters when we're farther away.
//  ********************************************************/

// #include <PID_v1.h>

// //Define Variables we'll be connecting to
// double Setpoint, Input, Output;

// //Define the aggressive and conservative Tuning Parameters
// double aggKp=4, aggKi=0.2, aggKd=1;
// double consKp=1, consKi=0.05, consKd=0.25;

// //Specify the links and initial tuning parameters
// PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

// void setup()
// {
//   //initialize the variables we're linked to
//   Input = analogRead(0);
//   Setpoint = 100;
//   Serial.begin(115200);

//   //turn the PID on
//   myPID.SetMode(AUTOMATIC);
// }

// void loop()
// {
//   Input = analogRead(0);
  
//   double gap = abs(Setpoint-Input); //distance away from setpoint
//   if(gap<10)
//   {  //we're close to setpoint, use conservative tuning parameters
//     myPID.SetTunings(consKp, consKi, consKd);
//   }
//   else
//   {
//      //we're far from setpoint, use aggressive tuning parameters
//      myPID.SetTunings(aggKp, aggKi, aggKd);
//   }
  
//   myPID.Compute();
//   println(Output);
// }



/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,2,0,0, DIRECT);

Adafruit_MotorShield shield1 = Adafruit_MotorShield(0x61);

Adafruit_DCMotor* motor;

void setup()
{
  //initialize the variables we're linked to
  Input = analogRead(0) / 4; // Capacity of (0, 255)
  Setpoint = 25; 
  Serial.begin(115200);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetControllerDirection(DIRECT);
  myPID.SetOutputLimits(-255, 255);

  motor = shield1.getMotor(1);
  shield1.begin();
}

void loop()
{
  Input = analogRead(0) / 4;
  Output = map(Input, 0, 255, -255, 255);

  Serial.print(Input, )

  if (Output < 0){
    motor->run(BACKWARD);
    motor->setSpeed(abs(Output));
    Serial.print("-");
    Serial.print(abs(Output));
    Serial.print(" ");
  }
  else if (Output >= 0){
    motor->run(FORWARD);
    motor->setSpeed(Output);
    Serial.print("+");
    Serial.print(abs(Output));
    Serial.print(" ");
  }

  motor->setSpeed(Output);
  Serial.print(Input);
  Serial.print(" ");
  Serial.print(Output);
  Serial.println();
}