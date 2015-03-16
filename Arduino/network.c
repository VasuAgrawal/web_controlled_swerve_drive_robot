#include "network.h"

network network_init() {
    network net;
    net.data_available = false;
    net.key = 0;
    net.value = 0;
    net.key_received = false;
    net.value_received = false;
    return net;
}

bool connected(network *this) {
    return this->server.connected();
}

bool available(network *this) {
    return this->data_available;
}

void accept(network *this) {
    this->remote = this->server.accept();
    return;
}

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