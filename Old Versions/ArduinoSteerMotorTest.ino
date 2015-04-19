/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <PID_v1.h>

Adafruit_MotorShield shield1 = Adafruit_MotorShield(0x60);

double Setpoint, Input, Output;

PID myPID(&Input, &Output, &Setpoint, 2, 0, 0, DIRECT);
bool fucked = false;

Adafruit_DCMotor* motor;

void setup()
{
  //initialize the variables we're linked to
  Serial.begin(115200);
  Setpoint = 150;
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-128, 128); // +/- 6V max
  motor = shield1.getMotor(3);
  shield1.begin();
}

void loop()
{
  if (fucked) {
    motor->setSpeed(0);
    motor->run(RELEASE);
    return;
  }

  if (Serial.available() > 0) {
    int Setpoint = Serial.parseInt();
    Serial.print("New setpoint: ");
    Serial.println(Setpoint);
  }

  Input = analogRead(4) / 4; // 0 - 256ish

  for (int i = 2; i < 6; i++){
    if (analogRead(i) / 4 < 50 || analogRead(i) / 4 > 200) {
      motor->setSpeed(0);
      motor->run(RELEASE);
      Serial.print(i);
      Serial.print(" ");
      Serial.print(analogRead(i));
      Serial.println(" FUCK");  
      fucked = true;
    }
  }
    
  if (Serial.available() > 0) {
    int input = Serial.parseInt();
    motor->setSpeed(50);
    motor->run(BACKWARD);
    delay(input);
    motor->setSpeed(0);
    motor->run(RELEASE);
  }
}