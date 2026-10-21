#include "ConsoleUI.h"

#include <exception>
#include <iostream>

int main() {
    try {
        ConsoleUI ui;
        ui.run();
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
