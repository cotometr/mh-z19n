#pragma once

class Stream;

#include <WString.h>
#include "../log/log.h"

#define MHZ19B_DEBUG_LOG

class Mhz19b
{
public:

    Mhz19b(Stream& stream);

    /**
     * @brief Request CO2
     * @return if positive: level of CO2 in the air
     * @return -1 on error or CO2 is not ready
     */
    int get_co2_uart();

    /**
     * @brief ZERO POINT is 400PPM, PLS MAKE SURE THE SENSOR HAD BEEN WORKED UNDER 400PPM FOR OVER 20MINUTES
     * @return 0 is everythin is ok
     */
    int set_zero_point_calibration();

    /**
     * @details Note: Pls do ZERO calibration before span calibration
                Please make sure the sensor worked under a certain level co2 for over 20 minutes.
                Suggest using 2000ppm as span, at least 1000ppm
     * @param level co2 level for calibration
     * @return 0 if everything is ok
     */
    int set_span_point_calibration(int span_level);

    /**
     * @details All Winsen sensor with ABC logic on before delivery if no special request.
     * @param is_auto_calibrated true - for turned on
     * @return
     */
    int set_auto_calibrate(bool is_auto_calibrated);

    /**
     * @brief Set high level of sensor
     * @details Detection range is 2000 or 5000ppm
     * @param range co2 high level
     * @return
     */
    int set_range(int range);

    GET_DEBUG_LOG();
private:

    uint8_t buffer[9];

    uint8_t get_crc(unsigned char *buff);

    /**
     * @brief Send the buffer to the sensor
     * @return Error code
     */
    int send_request();

    int recv_response();

    void set_buffer(uint8_t byte2, uint8_t byte3 = 0, uint8_t byte4 = 0);

    bool is_available();

    void clear_serial_cache();

    Stream& stream;

    CREATE_DEBUG_LOG();

    enum command : uint8_t
    {
        COMMAND_READ_CO2 = 0x86,
        COMMAND_CALIBRATE_ZERO = 0x87,
        COMMAND_CALIBRATE_SPAN = 0x88,
        COMMAND_CALIBRATE_AUTO_ON_OFF = 0x79,
        COMMAND_RANGE_SETTING = 0x99,
    };
};