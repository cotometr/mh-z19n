#pragma once

#include <Stream.h>

class Mhz19b
{
public:
    Mhz19b(Stream & stream, bool log = true);

    int getCO2uart();

    const String& get_last_log();
private:
    unsigned char get_crc(char *buff);

    Stream& stream;

    String last_log_str;

    bool is_log_used;
};