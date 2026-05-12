#include "client_logic.h"

#include <iostream>
#include <stdexcept>

int main() {
    try {
        ClientLogic client;
        client.run();
    }
    catch (const std::exception& error) {
        std::cerr << "Client error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}