/**
 * Network communications handler, as a server.
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
#define MODULE_0_DRIVE_MOTOR_SPD 0xF0
#define MODULE_0_DRIVE_MOTOR_DIR 0xF1
#define MODULE_0_STEER_MOTOR_POS 0xF2
#define MODULE_1_DRIVE_MOTOR_SPD 0xF3
#define MODULE_1_DRIVE_MOTOR_DIR 0xF4
#define MODULE_1_STEER_MOTOR_POS 0xF5
#define MODULE_2_DRIVE_MOTOR_SPD 0xF6
#define MODULE_2_DRIVE_MOTOR_DIR 0xF7
#define MODULE_2_STEER_MOTOR_POS 0xF8
#define MODULE_3_DRIVE_MOTOR_SPD 0xF9
#define MODULE_3_DRIVE_MOTOR_DIR 0xFA
#define MODULE_3_STEER_MOTOR_POS 0xFB

typedef struct network_module network;
struct network_module {
    /**
     * Server object on the Yun's side.
     * Currently the Yun creates its own network, so a client needs to be
     * connected to the local network in order to be able to access the server.
     */
    YunServer server;

    /**
     * Remote client object, on the controller side.
     * Currently the client must be on the same local network as the server
     * for the connection to work.
     */
    YunClient remote;

    /**
     * Value showing whether or not a new data piece is available.
     * This is set to true whenever a complete data pair is received, and 
     * set to false again whenever the server is in the middle of crafting a
     * pair.
     */
    bool data_available;

    /**
     * Key for the received data pair.
     */
    byte key;

    /**
     * Value for the received data pair.
     */
    byte value;

    /**
     * Whether or not a key has been received.
     * If both key and value are received, data_available is marked as true
     * and both key_received and value_received are reset.
     */    
    bool key_received;

    /**
     * Whether or not a value has been received.
     * If both key and value are received, data_available is marked as true
     * and both key_received and value_received are reset.
     */
    bool value_received;

    /**
     * Whether or not a client is connected.
     */
    bool (*connected)(network*);

    /**
     * Whether or not a new piece of data is available.
     */
    bool (*available)(network*);

    /**
     * Accepts a client attempting to connect, if any.
     */
    void (*accept)(network*);

    /**
     * Receives the next piece of data from the stream, if any.
     */
    void (*get_next)(network*);
};

network* network_init();
bool connected(network *net);
bool available(network *net);
void accept(network *net);
void get_next(network *net);

#endif
