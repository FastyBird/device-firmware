/*

CSE7766 SENSOR - Power monitor sensor

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if SENSOR_SUPPORT && CSE7766_SUPPORT

#pragma once

#include <SoftwareSerial.h>

#include "base/BaseSensor.h"

class CSE7766Sensor : public BaseSensor {

    public:

        // ---------------------------------------------------------------------
        // SENSOR PUBLIC
        // ---------------------------------------------------------------------

        CSE7766Sensor(): BaseSensor(), _data() {
            _count = 6;
            _sensor_id = SENSOR_CSE7766_ID;

            _appendMagnitude(MAGNITUDE_CURRENT, 3, MAGNITUDE_UNIT_AMPERES);
            _appendMagnitude(MAGNITUDE_VOLTAGE, 0, MAGNITUDE_UNIT_VOLTS);
            _appendMagnitude(MAGNITUDE_POWER_ACTIVE, 0, MAGNITUDE_UNIT_WATTS);
            _appendMagnitude(MAGNITUDE_POWER_REACTIVE, 0, MAGNITUDE_UNIT_WATTS);
            _appendMagnitude(MAGNITUDE_POWER_APPARENT, 0, MAGNITUDE_UNIT_WATTS);
            _appendMagnitude(MAGNITUDE_POWER_FACTOR, 0, MAGNITUDE_UNIT_PERCENTAGE);
            _appendMagnitude(MAGNITUDE_ENERGY, 0, MAGNITUDE_UNIT_JOULES);
        }

        ~CSE7766Sensor() {
            if (_serial) {
                delete _serial;
            }
        }

        // ---------------------------------------------------------------------

        void setRX(
            uint8_t pin_rx
        ) {
            if (_pin_rx == pin_rx) {
                return;
            }

            _pin_rx = pin_rx;
            _dirty = true;
        }

        void setInverted(
            bool inverted
        ) {
            if (_inverted == inverted) {
                return;
            }

            _inverted = inverted;
            _dirty = true;
        }

        // ---------------------------------------------------------------------

        uint8_t getRX() {
            return _pin_rx;
        }

        bool getInverted() {
            return _inverted;
        }

        // ---------------------------------------------------------------------

        void expectedCurrent(
            double expected
        ) {
            if ((expected > 0) && (_current > 0)) {
                _ratioC = _ratioC * (expected / _current);
            }
        }

        void expectedVoltage(
            unsigned int expected
        ) {
            if ((expected > 0) && (_voltage > 0)) {
                _ratioV = _ratioV * (expected / _voltage);
            }
        }

        void expectedPower(
            unsigned int expected
        ) {
            if ((expected > 0) && (_active > 0)) {
                _ratioP = _ratioP * (expected / _active);
            }
        }

        void setCurrentRatio(
            double value
        ) {
            _ratioC = value;
        }

        void setVoltageRatio(
            double value
        ) {
            _ratioV = value;
        }

        void setPowerRatio(
            double value
        ) {
            _ratioP = value;
        }

        double getCurrentRatio() {
            return _ratioC;
        }

        double getVoltageRatio() {
            return _ratioV;
        }

        double getPowerRatio() {
            return _ratioP;
        }

        void resetRatios() {
            _ratioC = _ratioV = _ratioP = 1.0;
        }

        void resetEnergy(double value = 0) {
            _energy = value;
        }

        // ---------------------------------------------------------------------
        // SENSOR API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        void begin() {
            if (!_dirty) {
                return;
            }

            if (_serial) {
                delete _serial;
            }

            if (_pin_rx == 1) {
                Serial.begin(CSE7766_BAUDRATE);

            } else {
                _serial = new SoftwareSerial(_pin_rx, SW_SERIAL_UNUSED_PIN, _inverted, 32);
                _serial->enableIntTx(false);
                _serial->begin(CSE7766_BAUDRATE);
            }

            _ready = true;
            _dirty = false;
        }

        // Descriptive name of the sensor
        String description() {
            char buffer[28];

            if (1 == _pin_rx) {
                snprintf(buffer, sizeof(buffer), "CSE7766 @ HwSerial");

            } else {
                snprintf(buffer, sizeof(buffer), "CSE7766 @ SwSerial(%u,NULL)", _pin_rx);
            }

            return String(buffer);
        }

        // Name of the sensor
        uint8_t type() {
            return SENSOR_TYPE_ENERGY;
        }

        // Current value for slot # index
        double _magnitudeCurrentValue(uint8_t index) {
            if (index == 0) return _current;
            if (index == 1) return _voltage;
            if (index == 2) return _active;
            if (index == 3) return _reactive;
            if (index == 4) return _voltage * _current;
            if (index == 5) return ((_voltage > 0) && (_current > 0)) ? 100 * _active / _voltage / _current : 100;
            if (index == 6) return _energy;

            return 0;
        }

        // Loop-like method, call it in your main loop
        void tick() {
            _read();
        }

    protected:

        /**
         * Checksum is the sum of all data
         * except for packet header and packet tail lowering by 8bit (...)
         *
         * @return bool
         */
        bool _checksum() {
            uint8_t checksum = 0;

            for (uint8_t i = 2; i < 23; i++) {
                checksum += _data[i];
            }

            return checksum == _data[23];
        }

        void _process() {
            // Sample data:
            // 55 5A 02 E9 50 00 03 31 00 3E 9E 00 0D 30 4F 44 F8 00 12 65 F1 81 76 72 (w/ load)
            // F2 5A 02 E9 50 00 03 2B 00 3E 9E 02 D7 7C 4F 44 F8 CF A5 5D E1 B3 2A B4 (w/o load)

            #if SENSOR_DEBUG
                DEBUG_MSG("[SENSOR] CSE7766: _process: ");

                for (byte i=0; i<24; i++) {
                    DEBUG_MSG("%02X ", _data[i]);
                }

                DEBUG_MSG("\n");
            #endif

            // Checksum
            if (!_checksum()) {
                _error = SENSOR_ERROR_CRC;

                #if SENSOR_DEBUG
                    DEBUG_MSG("[SENSOR] CSE7766: Checksum error\n");
                #endif

                return;
            }

            // Calibration
            if (0xAA == _data[0]) {
                _error = SENSOR_ERROR_CALIBRATION;

                #if SENSOR_DEBUG
                    DEBUG_MSG("[SENSOR] CSE7766: Chip not calibrated\n");
                #endif

                return;
            }

            if ((_data[0] & 0xFC) > 0xF0) {
                _error = SENSOR_ERROR_OTHER;

                #if SENSOR_DEBUG
                    if (0xF1 == _data[0] & 0xF1) DEBUG_MSG("[SENSOR] CSE7766: Abnormal coefficient storage area\n");
                    if (0xF2 == _data[0] & 0xF2) DEBUG_MSG("[SENSOR] CSE7766: Power cycle exceeded range\n");
                    if (0xF4 == _data[0] & 0xF4) DEBUG_MSG("[SENSOR] CSE7766: Current cycle exceeded range\n");
                    if (0xF8 == _data[0] & 0xF8) DEBUG_MSG("[SENSOR] CSE7766: Voltage cycle exceeded range\n");
                #endif

                return;
            }

            // Calibration coefficients
            unsigned long _coefV = (_data[2]  << 16 | _data[3]  << 8 | _data[4] );              // 190770
            unsigned long _coefC = (_data[8]  << 16 | _data[9]  << 8 | _data[10]);              // 16030
            unsigned long _coefP = (_data[14] << 16 | _data[15] << 8 | _data[16]);              // 5195000

            // Adj: this looks like a sampling report
            uint8_t adj = _data[20];                                                            // F1 11110001

            // Calculate voltage
            _voltage = 0;

            if ((adj & 0x40) == 0x40) {
                unsigned long voltage_cycle = _data[5] << 16 | _data[6] << 8 | _data[7];        // 817

                _voltage = _ratioV * _coefV / voltage_cycle / CSE7766_V2R;                      // 190700 / 817 = 233.41
            }

            // Calculate power
            _active = 0;

            if ((adj & 0x10) == 0x10) {
                if ((_data[0] & 0xF2) != 0xF2) {
                    unsigned long power_cycle = _data[17] << 16 | _data[18] << 8 | _data[19];   // 4709

                    _active = _ratioP * _coefP / power_cycle / CSE7766_V1R / CSE7766_V2R;       // 5195000 / 4709 = 1103.20
                }
            }

            // Calculate current
            _current = 0;

            if ((adj & 0x20) == 0x20) {
                if (_active > 0) {
                    unsigned long current_cycle = _data[11] << 16 | _data[12] << 8 | _data[13]; // 3376
                    _current = _ratioC * _coefC / current_cycle / CSE7766_V1R;                  // 16030 / 3376 = 4.75
                }
            }

			// Calculate reactive power
			_reactive = 0;

			unsigned int active = _active;
			unsigned int apparent = _voltage * _current;

            if (apparent > active) {
                _reactive = sqrt(apparent * apparent - active * active);

            } else {
                _reactive = 0;
            }

            // Calculate energy
            unsigned int difference;
            static unsigned int cf_pulses_last = 0;
            unsigned int cf_pulses = _data[21] << 8 | _data[22];

            if (0 == cf_pulses_last) {
                cf_pulses_last = cf_pulses;
            }

            if (cf_pulses < cf_pulses_last) {
                difference = cf_pulses + (0xFFFF - cf_pulses_last) + 1;

            } else {
                difference = cf_pulses - cf_pulses_last;
            }

            _energy += difference * (float) _coefP / 1000000.0;

            cf_pulses_last = cf_pulses;
        }

        void _read() {
            _error = SENSOR_ERROR_OK;

            static uint8_t index = 0;
            static unsigned long last = millis();

            while (_serial_available()) {
                // A 24 bytes message takes ~55ms to go through at 4800 bps
                // Reset counter if more than 1000ms have passed since last byte.
                if (millis() - last > CSE7766_SYNC_INTERVAL) {
                    index = 0;
                }

                last = millis();

                uint8_t byte = _serial_read();

                // first byte must be 0x55 or 0xF?
                if (index == 0) {
                    if ((0x55 != byte) && (byte < 0xF0)) {
                        continue;
                    }

                // second byte must be 0x5A
                } else if (index == 1) {
                    if (byte != 0x5A) {
                        index = 0;

                        continue;
                    }
                }

                _data[index++] = byte;

                if (index > 23) {
                    _serial_flush();

                    break;
                }
            }

            // Process packet
            if (index == 24) {
                _process();

                index = 0;
            }
        }

        // ---------------------------------------------------------------------

        bool _serial_available() {
            if (1 == _pin_rx) {
                return Serial.available();

            } else {
                return _serial->available();
            }
        }

        void _serial_flush() {
            if (1 == _pin_rx) {
                return Serial.flush();

            } else {
                return _serial->flush();
            }
        }

        uint8_t _serial_read() {
            if (1 == _pin_rx) {
                return Serial.read();

            } else {
                return _serial->read();
            }
        }

        // ---------------------------------------------------------------------

        unsigned int _pin_rx = CSE7766_PIN;
        bool _inverted = CSE7766_PIN_INVERSE;
        SoftwareSerial * _serial = NULL;

        double _active = 0;
        double _reactive = 0;
        double _voltage = 0;
        double _current = 0;
        double _energy = 0;

        double _ratioV = 1.0;
        double _ratioC = 1.0;
        double _ratioP = 1.0;

        uint8_t _data[24];
};

#endif // SENSOR_SUPPORT && CSE7766_SUPPORT