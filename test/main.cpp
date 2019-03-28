#include <iostream>
#include <SoftwareSerial.h>
#include "../mhz19b.h"

int main() {

    Stream stream;
    Mhz19b mhz19b(stream);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}