#include <Bridge.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <YunClient.h>
#include <YunServer.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include "moduleV2.h"

#define PORT 6666

typedef enum {
    MODULE_0_DRIVE_MOTOR_SPD=0xF0,
    MODULE_0_DRIVE_MOTOR_DIR=0xF1,
    MODULE_0_STEER_MOTOR_POS=0xF2,
    MODULE_1_DRIVE_MOTOR_SPD=0xF3,
    MODULE_1_DRIVE_MOTOR_DIR=0xF4,
    MODULE_1_STEER_MOTOR_POS=0xF5,
    MODULE_2_DRIVE_MOTOR_SPD=0xF6,
    MODULE_2_DRIVE_MOTOR_DIR=0xF7,
    MODULE_2_STEER_MOTOR_POS=0xF8,
    MODULE_3_DRIVE_MOTOR_SPD=0xF9,
    MODULE_3_DRIVE_MOTOR_DIR=0xFA,
    MODULE_3_STEER_MOTOR_POS=0xFB
} net_key;

//Prototypes
void get_next(YunClient remote);

// Network
byte key;
byte value;
bool key_received;
bool value_received; 
bool data_available;
//Shields
Adafruit_MotorShield shield1 = Adafruit_MotorShield(0x60); // bottom shield
Adafruit_MotorShield shield2 = Adafruit_MotorShield(0x61); // top shield
//Server
YunServer server(PORT);

//Modules
Module modules[4] = {
    Module(2, &shield2, 2, 450 / 4, 200 / 4, 800 / 4, 1),
    Module(3, &shield1, 2, 500 / 4, 200 / 4, 800 / 4, 1),
    Module(4, &shield1, 3, 483 / 4, 200 / 4, 800 / 4, 4),
    Module(5, &shield2, 3, 480 / 4, 200 / 4, 800 / 4, 4)
};

void setup(){
    // Lots of magic numbers. Sorry.
    Serial.begin(115200);
    Bridge.begin();

    Serial.printf("Module0 -- DriveSpeed: %u DriveDir: %i SteerPos: %u\n"
        "SteerPot: %u PIDSteerOutput: %d\n", modules[0].get_drive_speed(),
        modules[0].get_drive_dir(), modules[0].get_steer_pos(),
        modules[0].read_steer_pot(), modules[0].get_steer_PID_out());
    delay(10);
    Serial.printf("Module1 -- DriveSpeed: %u DriveDir: %i SteerPos: %u\n"
        "SteerPot: %u PIDSteerOutput: %d\n", modules[1].get_drive_speed(),
        modules[1].get_drive_dir(), modules[1].get_steer_pos(),
        modules[1].read_steer_pot(), modules[1].get_steer_PID_out());
    delay(10);
    Serial.printf("Module2 -- DriveSpeed: %u DriveDir: %i SteerPos: %u\n"
        "SteerPot: %u PIDSteerOutput: %d\n", modules[2].get_drive_speed(),
        modules[2].get_drive_dir(), modules[2].get_steer_pos(),
        modules[2].read_steer_pot(), modules[2].get_steer_PID_out());
    delay(10);
    Serial.printf("Module3 -- DriveSpeed: %u DriveDir: %i SteerPos: %u\n"
        "SteerPot: %u PIDSteerOutput: %d\n", modules[3].get_drive_speed(),
        modules[3].get_drive_dir(), modules[3].get_steer_pos(),
        modules[3].read_steer_pot(), modules[3].get_steer_PID_out());
    delay(10);


    server.noListenOnLocalhost();
    server.begin(); 

    key = 0;
    value = 0;
    key_received = false;
    value_received = false;
    data_available = false;
}

void loop(){
    Serial.println("Accepting client");
    YunClient remote = server.accept();
    if (remote.connected()) {
        Serial.println("Client Connected!");
        while (remote.connected()) {
            get_next(remote);
            if (data_available) {
                Serial.printf("Key: %x Value: %u", key, value);
                switch((net_key)key) {
                
                case MODULE_0_DRIVE_MOTOR_SPD:
                    modules[0].set_drive_speed((uint8_t)value);
                    break;
                
                case MODULE_0_DRIVE_MOTOR_DIR:
                    modules[0].set_drive_dir((signed char)value);
                    break;
                
                case MODULE_0_STEER_MOTOR_POS:
                    modules[0].set_steer_pos((uint16_t)value);
                    break;
                
                case MODULE_1_DRIVE_MOTOR_SPD:
                    modules[1].set_drive_speed((uint8_t)value);
                    break;
                
                case MODULE_1_DRIVE_MOTOR_DIR:
                    modules[1].set_drive_dir((signed char)value);
                    break;
                
                case MODULE_1_STEER_MOTOR_POS:
                    modules[1].set_steer_pos((uint16_t)value);
                    break;
                
                case MODULE_2_DRIVE_MOTOR_SPD:
                    modules[2].set_drive_speed((uint8_t)value);
                    break;
                
                case MODULE_2_DRIVE_MOTOR_DIR:
                    modules[2].set_drive_dir((signed char)value);
                    break;
                
                case MODULE_2_STEER_MOTOR_POS:
                    modules[2].set_steer_pos((uint16_t)value);
                    break;
                
                case MODULE_3_DRIVE_MOTOR_SPD:
                    modules[3].set_drive_speed((uint8_t)value);
                    break;
                
                case MODULE_3_DRIVE_MOTOR_DIR:
                    modules[3].set_drive_dir((signed char)value);
                    break;
                
                case MODULE_3_STEER_MOTOR_POS:
                    modules[3].set_steer_pos((uint16_t)value);
                    break;
                
                default:
                    break;
                }
            }

//            for (int i = 0; i < 4; i++) {
//                modules[i].update();
//            }

            Serial.printf("Module0 -- DriveSpeed: %u DriveDir: %i SteerPos: %u\n"
                "SteerPot: %u PIDSteerOutput: %d\n", modules[0].get_drive_speed(),
                modules[0].get_drive_dir(), modules[0].get_steer_pos(),
                modules[0].read_steer_pot(), modules[0].get_steer_PID_out());
            delay(50);
            Serial.printf("Module1 -- DriveSpeed: %u DriveDir: %i SteerPos: %u\n"
                "SteerPot: %u PIDSteerOutput: %d\n", modules[1].get_drive_speed(),
                modules[1].get_drive_dir(), modules[1].get_steer_pos(),
                modules[1].read_steer_pot(), modules[1].get_steer_PID_out());
            delay(50);
            Serial.printf("Module2 -- DriveSpeed: %u DriveDir: %i SteerPos: %u\n"
                "SteerPot: %u PIDSteerOutput: %d\n", modules[2].get_drive_speed(),
                modules[2].get_drive_dir(), modules[2].get_steer_pos(),
                modules[2].read_steer_pot(), modules[2].get_steer_PID_out());
            delay(50);
            Serial.printf("Module3 -- DriveSpeed: %u DriveDir: %i SteerPos: %u\n"
                "SteerPot: %u PIDSteerOutput: %d\n", modules[3].get_drive_speed(),
                modules[3].get_drive_dir(), modules[3].get_steer_pos(),
                modules[3].read_steer_pot(), modules[3].get_steer_PID_out());
            delay(50);


        }
    }
    delay(1000);
}

void get_next(YunClient remote) {
    if (!remote.available()) {
        data_available = false;
        return;
    }
    while (remote.available()) {
        byte received = remote.read();
        if (received == -1) {
            return;
        } else if (!key_received) {
            Serial.println("Key Received!");
            key_received = true;
            key = received;
        } else if (key_received && !value_received) {
            Serial.println("Value Received!");  
            value_received = true;
            value = received;
        }

        if (key_received && value_received) {
            Serial.println("Data received!");
            data_available = true;
            key_received = false;
            value_received = false;
            return;
        }
    }
}


