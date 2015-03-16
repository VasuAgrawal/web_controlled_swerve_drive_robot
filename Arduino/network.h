/**
 * Network communications handler.
 * Data will be received as such:
 *     Key   (1 Byte)
 *     Value (1 Byte)
 *     Key   (1 Byte)
 *     Value (1 Byte)
 *     ...
 * No error handling implemented
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

#define PORT 6666
#define DRIVE_MOTOR_SPEED_KEY 0x10
#define DRIVE_MOTOR_DIR_KEY 0x11
#define STEER_MOTOR_POSITION_KEY 0x12
#define STEER_MOTOR_DIR_KEY 0x13

struct network_module {
    YunServer server;
    YunClient remote;
    bool data_available;
    byte key;
    byte value;
    bool key_received;
    bool value_received;
};

typedef struct network_module network;

#endif
