#include "network.h"

/**
 * Creates a new network object.
 * @return An empty network object.
 */
network network_init() {
    network net;
    net.data_available = false;
    net.key = 0;
    net.value = 0;
    net.key_received = false;
    net.value_received = false;

    net.connected = connected;
    net.available = available;
    net.accept = accept;
    net.get_next = get_next;

    return net;
}

/**
 * Method to determine whether there is a client connected.
 * @param  this The network object.
 * @return      Whether or not an object is connected.
 */
bool connected(network *this) {
    return this->server.connected();
}

/**
 * Method to determine whether there is a new key-value pair available for
 * processing.
 * @param  this The network object.
 * @return      Whether or not a new data pair is available to the client.
 */
bool available(network *this) {
    return this->data_available;
}

/**
 * Accepts a new client, if any are attempting to connect.
 * @param this The network object.
 */
void accept(network *this) {
    this->remote = this->server.accept();
    return;
}

/**
 * Function to receive key-value pairs from the network.
 * @param this The network object.
 */
void get_next(network *this) {
    if ((this->remote).connected() && (this->remote).available()) {
        byte received = (this->remote).read();
        if (received == -1) {
            return;
        }

        if (!(this->key_received)) {
            this->key = received;
            this->key_received = true;
            this->data_available = false;
        } else if (this->key_received && !(this->value_received)) {
            this->value = received;
            this->value_received = true;
        }

        if (this->key_received && this->value_received) {
            this->data_available = true;
            this->key_received = false;
            this->value_received = false;
        }
    }
}