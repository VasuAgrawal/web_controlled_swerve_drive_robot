#include "network.h"

/**
 * Creates a new network object.
 * @return An empty network object.
 */
network* network_init() {
    network* net;
    net->data_available = false;
    net->key = 0;
    net->value = 0;
    net->key_received = false;
    net->value_received = false;
    net->server = server(PORT);
    (net->server).noListenOnLocalHost();
    (net->server).begin();

    net->connected = connected;
    net->available = available;
    net->accept = accept;
    net->get_next = get_next;

    return net;
}

/**
 * Method to determine whether there is a client connected.
 * @param  net The network object.
 * @return      Whether or not an object is connected.
 */
bool connected(network *net) {
    return (net->remote).connected();
}

/**
 * Method to determine whether there is a new key-value pair available for
 * processing.
 * @param  net The network object.
 * @return      Whether or not a new data pair is available to the client.
 */
bool available(network *net) {
    return net->data_available;
}

/**
 * Accepts a new client, if any are attempting to connect.
 * @param net The network object.
 */
void accept(network *net) {
    net->remote = (net->server).accept();
    return;
}

/**
 * Function to receive key-value pairs from the network.
 * @param net The network object.
 */
void get_next(network *net) {
    if ((net->remote).connected() && (net->remote).available()) {
        byte received = (net->remote).read();
        if (received == -1) {
            return;
        }

        if (!(net->key_received)) {
            net->key = received;
            net->key_received = true;
            net->data_available = false;
        } else if (net->key_received && !(net->value_received)) {
            net->value = received;
            net->value_received = true;
        }

        if (net->key_received && net->value_received) {
            net->data_available = true;
            net->key_received = false;
            net->value_received = false;
        }
    }
}
