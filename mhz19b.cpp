//
// Created by yura on 29.03.19.
//

#include <Arduino.h>
#include "mhz19b.h"

#define ADD_TO_LASR_LOG(str)                \
do {                                        \
    if (this->is_log_used)                  \
    {                                       \
         this->last_log_str = (str);        \
    }                                       \
}while(0)

#define INIT_DEBUG_LOCAL_LOG()              \
    String __FUNCTION__local;

#define ADD_DEBUG_LOCAL_LOG(str)            \
do {                                        \
    if (this->is_log_used)                  \
    {                                       \
         __FUNCTION__local += (str);       \
    }                                       \
}while(0)

#define GET_DEBUG_LOCAL_LOG() \
    __FUNCTION__local

Mhz19b::Mhz19b(Stream &stream, bool log):stream(stream), is_log_used(log) {
    ADD_TO_LASR_LOG("Mhz19b inited!");
}

const String &Mhz19b::get_last_log() {
    return last_log_str;
}

int Mhz19b::getCO2uart() {
    // send the CO2 data request
    uint8_t cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
    char response[9]; // for answer

    INIT_DEBUG_LOCAL_LOG();

    ADD_DEBUG_LOCAL_LOG("Sending CO2 request...");

    stream.write(cmd, 9);//request PPM CO2

    // clear the buffer
    memset(response, 0, 9);

#define RETRY_COUNT (20)

    int i = 0;
    while (stream.available() == 0 && i < RETRY_COUNT ) {
        delay(100);
        i++;
    }

    if (i == RETRY_COUNT)
    {
        String &log = GET_DEBUG_LOCAL_LOG();
        ADD_TO_LASR_LOG(log + "\nStream is not available for " + String(RETRY_COUNT) + "times. error");
        return -1;
    }

    size_t response_size = stream.readBytes(response, 9);
    if (response_size != sizeof(response))
    {
        String &log = GET_DEBUG_LOCAL_LOG();
        ADD_TO_LASR_LOG(log + "\nCould not read the sizeof(response), read = " + String(response_size) + ", error");
        return -1;
    }

    // print out the response in hexa
    for ( i = 0; i < 9; i++) {
        ADD_DEBUG_LOCAL_LOG(String((int)(unsigned char)response[i], HEX));
        ADD_DEBUG_LOCAL_LOG("   ");
    }
    ADD_DEBUG_LOCAL_LOG("\n");

//    for (int i = 0; i < 9; i++) {
//        Serial.print(String((int)(unsigned char)response[i]));
//        Serial.print("   ");
//    }
//    Serial.println("");

    // checksum
    unsigned char calculated_crc = get_crc(response);
    unsigned char received_crc = (unsigned char)response[8];

    if (calculated_crc != received_crc) {

        ADD_DEBUG_LOCAL_LOG("Checksum not OK!");
        ADD_DEBUG_LOCAL_LOG("Received: ");
        ADD_DEBUG_LOCAL_LOG((int)(unsigned char)response[8]);
        ADD_DEBUG_LOCAL_LOG("\nShould be: ");
        ADD_DEBUG_LOCAL_LOG(calculated_crc);
        ADD_DEBUG_LOCAL_LOG("\n");
    }

    // ppm
    int ppm_uart = 256 * (int)response[2] + response[3];
    ADD_DEBUG_LOCAL_LOG("PPM UART: ");
    ADD_DEBUG_LOCAL_LOG(ppm_uart);

    // temp
    int temp = (unsigned char)response[4] - 40;
    ADD_DEBUG_LOCAL_LOG("\nTemperature? ");
    ADD_DEBUG_LOCAL_LOG(temp);

    // status
//    byte status = response[5];
//    Serial.print("Status? ");
//    Serial.println(status);
//    if (status == 0x40) {
//        Serial.println("Status OK");
//    }

    ADD_TO_LASR_LOG(GET_DEBUG_LOCAL_LOG());

    return ppm_uart;
}

unsigned char Mhz19b::get_crc(char *buff) {
    byte i;
    unsigned char checksum = 0;
    for (i = 1; i < 8; i++) {
        checksum += (unsigned char)buff[i];
    }

    checksum = 0xff - checksum;
    checksum += 1;
    return checksum;
}
