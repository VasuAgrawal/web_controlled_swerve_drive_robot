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
#define DRIVE_MOTOR_SPEED_KEY 0x10
#define DRIVE_MOTOR_DIR_KEY 0x11
#define STEER_MOTOR_POSITION_KEY 0x12
#define STEER_MOTOR_DIR_KEY 0x13

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

typedef struct network_module network;

bool connected(network *this);
bool available(network *this);
void accept(network *this);
void get_next(network *this);

#endif