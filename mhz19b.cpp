//
// Created by yura on 29.03.19.
//

#include <Arduino.h>
#include "mhz19b.h"

#define SENSOR_RETRY_COUNT (30)

#ifdef MHZ19B_DEBUG_LOG

#define CLEAR_LOG()                                         \
do {                                                        \
    if (this->is_log_used)                                  \
    {                                                       \
         this->last_log_str = "";                           \
    }                                                       \
}while(0)

#define ADD_TO_LOG(value)                                   \
do {                                                        \
    if (this->is_log_used)                                  \
    {                                                       \
         this->last_log_str +=                              \
                            String(__PRETTY_FUNCTION__)     \
                            + (value);                      \
    }                                                       \
}while(0)
#else
#define CLEAR_LOG()
#define ADD_TO_LOG(value)
#endif


Mhz19b::Mhz19b(Stream &stream, bool log):stream(stream), is_log_used(log) {
    ADD_TO_LOG("Mhz19b inited!");
}

const String &Mhz19b::get_last_log() {
    return last_log_str;
}

int Mhz19b::get_co2_uart() {
    CLEAR_LOG();

    set_buffer(COMMAND_READ_CO2);
    ADD_TO_LOG("Sending CO2 request...\n");

    int res = send_request();
    if (res != 0)
    {
        return res;
    }

    // print out the response in hexa
    for (int i = 0; i < 9; i++) {
        ADD_TO_LOG(String((int)(unsigned char)buffer[i], HEX));
        ADD_TO_LOG("   ");
    }
    ADD_TO_LOG("\n");

    // ppm
    int ppm_uart = 256 * (int)buffer[2] + (int)buffer[3];
    ADD_TO_LOG("PPM UART: ");
    ADD_TO_LOG(ppm_uart);

    // temp
    int temp = (unsigned char)buffer[4] - 40;
    ADD_TO_LOG("\nTemperature? ");
    ADD_TO_LOG(temp);
    ADD_TO_LOG("\n");

    return ppm_uart;
}

int Mhz19b::set_zero_point_calibration() {
    CLEAR_LOG();

    ADD_TO_LOG("\n");

    set_buffer(COMMAND_CALIBRATE_ZERO);

    int res = send_request();

    if (res != 0)
    {
        ADD_TO_LOG("Failed to send command!\n");
    }

    return res;
}


unsigned char Mhz19b::get_crc(unsigned char *buff) {

    unsigned char checksum = 0;
    for (uint8_t i = 1; i < 8; i++) {
        checksum += buff[i];
    }

    checksum = (uint8_t)0xff - checksum;
    checksum += 1;
    return checksum;
}

int Mhz19b::send_request() {

    size_t io_size;
    io_size = stream.write(buffer, sizeof(buffer));//request PPM CO2
    stream.flush();

    if (io_size != sizeof(buffer))
    {
        clear_serial_cache();

        int write_error = stream.getWriteError();
        ADD_TO_LOG("Could not send the whole request. Only " + String(io_size) +
                   " has been sent, write error = " + String(write_error) + "\n");
        return -1;
    }

    if(!is_available())
    {
        clear_serial_cache();
        ADD_TO_LOG("The sensor doesn't response to receive data\n");
        return -1;
    }

    if ((io_size = (size_t)stream.available()) != sizeof(buffer)){
        clear_serial_cache();
        ADD_TO_LOG("Available less memory than needed" +
                   String(io_size) + io_size + "\n");

        return -1;
    }

    size_t response_size = stream.readBytes((char*)buffer, sizeof(buffer));
    if (response_size != sizeof(buffer)) {
        ADD_TO_LOG("Could not receive the responce. read size = " + String(response_size) + ", error\n");
        return -1;
    }

    unsigned char calculated_crc = get_crc(buffer);
    unsigned char received_crc = buffer[8];

    if (calculated_crc != received_crc)
    {
        ADD_TO_LOG("Checksum not OK! Recv = " + String(received_crc) +
                   " Should be " + String(calculated_crc) + "\n");
        return -1;
    }

    return 0;
}

void Mhz19b::set_buffer(uint8_t byte2, uint8_t byte3, uint8_t byte4) {
    uint8_t cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};

    buffer[0] = 0xFF;
    buffer[1] = 0x01;
    buffer[2] = byte2;
    buffer[3] = byte3;
    buffer[4] = byte4;
    buffer[5] = 0;
    buffer[6] = 0;
    buffer[7] = 0;
    buffer[8] = get_crc(buffer);
}

bool Mhz19b::is_available() {
    int i = 0;
    while (stream.available() == 0 && i < SENSOR_RETRY_COUNT) {
        delay(50);
        i++;
    }

    return i != SENSOR_RETRY_COUNT;
}

void Mhz19b::clear_serial_cache() {
    stream.flush();

    is_available();

    while (stream.read() > 0);
}

int Mhz19b::set_span_point_calibration(int level) {
    CLEAR_LOG();
    ADD_TO_LOG(" is not implemented");
    return -1;
}

int Mhz19b::set_auto_calibrate(bool is_auto_calibrated) {
    CLEAR_LOG();
    ADD_TO_LOG(" is not implemented");
    return -1;
}

int Mhz19b::set_range(int range) {
    CLEAR_LOG();
    ADD_TO_LOG(" is not implemented");
    return -1;
}
