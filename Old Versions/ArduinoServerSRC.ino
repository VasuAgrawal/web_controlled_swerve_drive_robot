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

Adafruit_MotorShield shield1 = Adafruit_MotorShield(0x60); // bottom shield
Adafruit_MotorShield shield2 = Adafruit_MotorShield(0x61); // top shield

struct module_assembly {
  int8_t potPort;
  int8_t steerShieldPort;
  int8_t driveShieldPort;
  int homeValue;
  int8_t driveDir;
  int8_t driveSpeed;
  Adafruit_MotorShield shield;
  Adafruit_DCMotor* driveMotor;
  Adafruit_DCMotor* steerMotor;
  double PIDInput;
  double PIDOutput;
  double PIDSetpoint;
  PID steerPID;
};
typedef struct module_assembly module;

module modules[4];

int analogReadShift(port){
  return analogRead(port) / 4;
}

void setup() {
  modules[0].potPort = 0;
  modules[0].steerShieldPort = 2;
  modules[0].driveShieldPort = 1;
  modules[0].homeValue = 450 / 4;
  modules[0].shield = shield2; // 0x61;

  modules[1].potPort = 1;
  modules[1].steerShieldPort = 2;
  modules[1].driveShieldPort = 1;
  modules[1].homeValue = 500 / 4; // experimentally determined
  modules[1].shield = shield1; // 0x60;

  modules[2].potPort = 2;
  modules[2].steerShieldPort = 3;
  modules[2].driveShieldPort = 4;
  modules[2].homeValue = 483 / 4;
  modules[2].shield = shield1; // 0x60;

  modules[3].potPort = 3;
  modules[3].steerShieldPort = 3;
  modules[3].driveShieldPort = 4;
  modules[3].homeValue = 480 / 4;
  modules[3].shield = shield2; // 0x61;

  for(int i = 0; i < 4; i++){
    modules[i].driveMotor = modules[i].shield.getMotor(modules[i].driveShieldPort);
    modules[i].steerMotor = modules[i].shield.getMotor(modules[i].steerShieldPort);
    modules[i].driveDir = 0;
    modules[i].driveSpeed = 0;
    modules[i].PIDInput = analogReadShift(modules[i].potPort);
    modules[i].PIDOutput = 0;
    modules[i].PIDSetpoint = homeValue;
    modules[i].steerPID = PID(&modules[i].PIDInput, &modules[i].PIDOutput, &modules[i].PIDSetpoint,
                              2, 0, 0, DIRECT);
    modules[i].steerPID.SetMode(AUTOMATIC);
    modules[i].steerPID.SetControllerDirection(DIRECT);
    modules[i].SetOutputLimits(-128, 128); // limit to +/- 6V since we're using 12V rail
  }

  shield1.begin();
  shield2.begin();

  // this will act as the server
  Serial.begin(115200);
  Bridge.begin();
  
  server.noListenOnLocalhost();
  server.begin();
}

void loop() {
  Serial.println("Current Version: .50");
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
      modules[spec].driveDir = value;
      modules[spec].driveMotor->setSpeed(modules[spec].driveSpeed);
      modules[spec].driveMotor->run(dir(modules[spec].driveDir));
      break;
    case DRIVE_MOTOR_SPEED_KEY:
      modules[spec].driveSpeed = value;
      modules[spec].driveMotor->setSpeed(modules[spec].driveSpeed);
      modules[spec].driveMotor->run(dir(modules[spec].driveDir));
      break;
    case STEER_MOTOR_DIR_KEY:`
      break;
    case STEER_MOTOR_POSITION_KEY:
      break;
  }
}

int getPotPosition(uint8_t spec){
  return analogRead(modules[spec].potPort);
}
