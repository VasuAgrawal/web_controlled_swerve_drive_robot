#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

#define PORT 6666
#define DRIVE_MOTOR_SPEED_KEY 0
#define DRIVE_MOTOR_DIR_KEY 1
#define STEER_MOTOR_POSITION_KEY 2
#define STEER_MOTOR_DIR_KEY 3

#define KEY_SIZE 5
#define SPEC_SIZE 3
#define VALUE_SIZE 8

YunServer server(PORT);

int8_t driveMotorSpeeds[] = {0, 0, 0, 0};
int8_t driveMotorDirs[] = {0, 0, 0, 0};
int8_t steerMotorPositions[] = {0, 0, 0, 0}; // this will be the home position
int8_t steerMotorDirs[] = {0, 0, 0, 0}; // turning direction for wheels
int8_t steerPotPins[] = {10, 15, 20, 25};

Adafruit_MotorShield shield1 = Adafruit_MotorShield(0x60); // bottom shield
Adafruit_MotorShield shield2 = Adafruit_MotorShield(0x61); // top shield

Adafruit_DCMotor *driveMotors[4];
Adafruit_DCMotor *steerMotors[4];

void setup() {
  driveMotors[0] = shield1.getMotor(2);
  driveMotors[1] = shield1.getMotor(4);
  driveMotors[2] = shield2.getMotor(2);
  driveMotors[3] = shield2.getMotor(4);

  steerMotors[0] = shield1.getMotor(1);
  steerMotors[1] = shield1.getMotor(3);
  steerMotors[2] = shield2.getMotor(1);
  steerMotors[3] = shield2.getMotor(3);

  shield1.begin();
  shield2.begin();

  // this will act as the server
  Serial.begin(115200);
  Bridge.begin();
  
  server.noListenOnLocalhost();
  server.begin();
}

void loop() {
  YunClient remote = server.accept();

  if(remote.connected()){
    Serial.println("REMOTE CONNECTED!");

    String string = "";
    unsigned short data = 0;
    uint8_t numRecv = 0;
    uint8_t dataSize = 2;

    //  once this loop is entered, it should not break out of the loop until
    // the client disconnects
    while(remote.connected()){
      // if there's any new data to be processed, do it
      // what this means that any updates will only happen when we receive
      // a complete data set. Until then, we're  just going to keep reading
      // since that's perhaps more important.
      if(remote.available()){
        byte received = remote.read();
        
        if (received == -1){
          continue;
        }
        
        data = data << 8;
        data += received;

        numRecv++;

        //further, only process if we have enough data
        if (numRecv == dataSize){

          uint8_t key = findKey(data);
          uint8_t spec = findSpec(data);
          uint8_t value = findValue(data);
          
          doUpdate(key, spec, value);
          remoteWrite();

          data = 0; // clears the data to be updated for next time
          numRecv = 0; // resets the numRecv counter
        }

        else if (numRecv < dataSize){
          // do nothing since we're just going to want to read for more imput
        }
      }
    }
    // shouldn't get here until the client terminates
    remote.stop();

  }

  else {
    Serial.println("no remote connected, retrying");
  }

  delay(1000);

}

uint8_t findKey(unsigned short data){
  unsigned short mask = 0xF800; //1111100000000000
  unsigned short masked = data & mask;
  masked = masked >> SPEC_SIZE + VALUE_SIZE;
  return (uint8_t)masked;
}

uint8_t findSpec(unsigned short data){
  unsigned short mask = 0x0700; //0000011100000000
  unsigned short masked = data & mask;
  masked =  masked >> VALUE_SIZE;
  return (uint8_t)masked;
}

uint8_t findValue(unsigned short data){
  unsigned short mask = 0x00FF; //0000000011111111
  unsigned short masked = data & mask;
  return (uint8_t)masked;
}

int dir(uint8_t spec){
  if (spec == 1){
    return FORWARD;
  } else if (spec == -1){
    return BACKWARD;
  } else{
    return RELEASE;
  }
}
  
void doUpdate(uint8_t key, uint8_t spec, uint8_t value){
  Serial.print("Updating key ");
  Serial.print(key);
  Serial.print(" for spec ");
  Serial.print(spec);
  Serial.print(" with value ");
  Serial.print(value);
  Serial.println(".");

  switch(key){
    case DRIVE_MOTOR_DIR_KEY:
    // need to set the value in array, set set speed again, and set dir
      driveMotorDirs[spec] = value;
      driveMotors[spec]->setSpeed(driveMotorSpeeds[spec]);
      driveMotors[spec]->run(dir(value));
      break;
    case DRIVE_MOTOR_SPEED_KEY:
      driveMotorSpeeds[spec] = value;
      driveMotors[spec]->setSpeed(value);
      driveMotors[spec]->run(dir(driveMotorDirs[spec]));
      break;
    case STEER_MOTOR_DIR_KEY:
      steerMotorDirs[spec] = value;
      steerMotors[spec]->setSpeed(steerMotorPositions[spec]);
      steerMotors[spec]->run(dir(value));
      break;
    case STEER_MOTOR_POSITION_KEY:
      steerMotorPositions[spec] = value;
      steerMotors[spec]->setSpeed(value);
      steerMotors[spec]->run(dir(steerMotorDirs[spec]));
      break;
  }

}

void incrementalSteer(uint8_t spec, uint8_t position){
}

void getPotPosition(uint8_t spec){
  int position = map(analogRead(steerPotPins[spec]), 0, 12, -90, 90);
}

void remoteWrite() {}
