/*

I2C SENSORS BASE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if SENSOR_SUPPORT && I2C_SUPPORT

#if I2C_USE_BRZO
    #define I2C_TRANS_SUCCESS               0   // All i2c commands were executed without errors
    #define I2C_TRANS_ERROR_BUS_NOT_FREE    1   // Bus not free, i.e. either SDA or SCL is low
    #define I2C_TRANS_ERROR_NACK_WRITE      2   // Not ACK ("NACK") by slave during write:
                                                // Either the slave did not respond to the given slave address; or the slave did not ACK a byte transferred by the master.
    #define I2C_TRANS_ERROR_NACK_READ       4   // Not ACK ("NACK") by slave during read,
                                                // i.e. slave did not respond to the given slave address
    #define I2C_TRANS_ERROR_CLOCK           8   // Clock Stretching by slave exceeded maximum clock stretching time. Most probably, there is a bus stall now!
    #define I2C_TRANS_ERROR_READ_NULL       16  // Read was called with 0 bytes to be read by the master. Command not sent to the slave, since this could yield to a bus stall
    #define I2C_TRANS_ERROR_TIMEOUT         32  // ACK Polling timeout exceeded
#else // Wire
    #define I2C_TRANS_SUCCESS               0   // success
    #define I2C_TRANS_ERROR_BUFFER_OVERLOW  1   // data too long to fit in transmit buffer
    #define I2C_TRANS_ERROR_NACK_ADDRESS    2   // received NACK on transmit of address
    #define I2C_TRANS_ERROR_NACK_DATA       3   // received NACK on transmit of data
    #define I2C_TRANS_ERROR_OTHER           4   // other error
#endif

#pragma once

#include "BaseSensor.h"

class I2CSensor : public BaseSensor {

    public:

        void setAddress(
            uint8_t address
        ) {
            if (_address == address) {
                return;
            }

            _address = address;

            _dirty = true;
        }

        uint8_t getAddress() {
            return _address;
        }

        // Address of the sensor (it could be the GPIO or I2C address)
        String address(
            uint8_t index
        ) {
            char buffer[5];
            
            snprintf(buffer, sizeof(buffer), "0x%02X", _address);
            
            return String(buffer);
        }

    protected:

        // Specific for I2C sensors
        uint8_t _begin_i2c(uint8_t address, size_t size, uint8_t * addresses) {
            // If we have already locked this address for this sensor quit
            if ((address > 0) && (address == _previous_address)) {
                return _previous_address;
            }

            // Check if we should release a previously locked address
            if ((_previous_address > 0) && (_previous_address != address)) {
                i2cReleaseLock(_previous_address);
                _previous_address = 0;
            }

            // If requesting a specific address, try to ger a lock to it
            if ((0 < address) && i2cGetLock(address)) {
                _previous_address = address;
                return _previous_address;
            }

            // If everything else fails, perform an auto-discover
            _previous_address = i2cFindAndLock(size, addresses);

            // Flag error
            if (0 == _previous_address) {
                _error = SENSOR_ERROR_I2C;
            }

            return _previous_address;
        }

        uint8_t _previous_address = 0;
        uint8_t _address = 0;
}

#endif // SENSOR_SUPPORT && I2C_SUPPORT
