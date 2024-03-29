/*

HLW8012 SENSOR - Energy meter sensor

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if SENSOR_SUPPORT && HLW8012_SUPPORT

#pragma once

#include <HLW8012.h>

#include "base/BaseSensor.h"

class HLW8012Sensor : public BaseSensor {

    public:

        // -----------------------------------------------------------------------------
        // SENSOR PUBLIC
        // -----------------------------------------------------------------------------

        HLW8012Sensor(): BaseSensor() {
            _count = 7;
            _sensor_id = SENSOR_HLW8012_ID;
            _hlw8012 = new HLW8012();

            _appendMagnitude(MAGNITUDE_CURRENT, 3, MAGNITUDE_UNIT_AMPERES);
            _appendMagnitude(MAGNITUDE_VOLTAGE, 0, MAGNITUDE_UNIT_VOLTS);
            _appendMagnitude(MAGNITUDE_POWER_ACTIVE, 0, MAGNITUDE_UNIT_WATTS);
            _appendMagnitude(MAGNITUDE_POWER_REACTIVE, 0, MAGNITUDE_UNIT_WATTS);
            _appendMagnitude(MAGNITUDE_POWER_APPARENT, 0, MAGNITUDE_UNIT_WATTS);
            _appendMagnitude(MAGNITUDE_POWER_FACTOR, 0, MAGNITUDE_UNIT_PERCENTAGE);
            _appendMagnitude(MAGNITUDE_ENERGY, 0, MAGNITUDE_UNIT_JOULES);
        }

        ~HLW8012Sensor() {
            _enableInterrupts(false);

            delete _hlw8012;
        }

        void expectedCurrent(
            double expected
        ) {
            _hlw8012->expectedCurrent(expected);
        }

        void expectedVoltage(
            unsigned int expected
        ) {
            _hlw8012->expectedVoltage(expected);
        }

        void expectedPower(
            unsigned int expected
        ) {
            _hlw8012->expectedActivePower(expected);
        }

        void resetRatios() {
            _hlw8012->resetMultipliers();
        }

        void resetEnergy(
            double value = 0
        ) {
            _energy_offset = value;

            _hlw8012->resetEnergy();
        }

        // ---------------------------------------------------------------------

        void setSEL(
            uint8_t sel
        ) {
            if (_sel == sel) {
                return;
            }

            _sel = sel;
            _dirty = true;
        }

        void setCF(
            uint8_t cf
        ) {
            if (_cf == cf) {
                return;
            }

            _cf = cf;
            _dirty = true;
        }

        void setCF1(
            uint8_t cf1
        ) {
            if (_cf1 == cf1) {
                return;
            }

            _cf1 = cf1;
            _dirty = true;
        }

        void setSELCurrent(
            bool value
        ) {
            _sel_current = value;
        }

        void setCurrentRatio(
            double value
        ) {
            _hlw8012->setCurrentMultiplier(value);
        }

        void setVoltageRatio(
            double value
        ) {
            _hlw8012->setVoltageMultiplier(value);
        }

        void setPowerRatio(
            double value
        ) {
            _hlw8012->setPowerMultiplier(value);
        }

        // ---------------------------------------------------------------------

        uint8_t getSEL() {
            return _sel;
        }

        uint8_t getCF() {
            return _cf;
        }

        uint8_t getCF1() {
            return _cf1;
        }

        uint8_t getSELCurrent() {
            return _sel_current;
        }

        double getCurrentRatio() {
            return _hlw8012->getCurrentMultiplier();
        }

        double getVoltageRatio() {
            return _hlw8012->getVoltageMultiplier();
        }

        double getPowerRatio() {
            return _hlw8012->getPowerMultiplier();
        }

        // ---------------------------------------------------------------------
        // SENSOR API
        // ---------------------------------------------------------------------

        // Initialization method, must be idempotent
        // Defined outside the class body
        void begin() {
            // Initialize HLW8012
            // void begin(uint8_t cf_pin, uint8_t cf1_pin, uint8_t sel_pin, uint8_t currentWhen = HIGH, bool use_interrupts = false, uint32_t pulse_timeout = PULSE_TIMEOUT);
            // * cf_pin, cf1_pin and sel_pin are GPIOs to the HLW8012 IC
            // * currentWhen is the value in sel_pin to select current sampling
            // * set use_interrupts to true to use interrupts to monitor pulse widths
            // * leave pulse_timeout to the default value, recommended when using interrupts
            #if HLW8012_USE_INTERRUPTS
                _hlw8012->begin(_cf, _cf1, _sel, _sel_current, true);
            #else
                _hlw8012->begin(_cf, _cf1, _sel, _sel_current, false, 1000000);
            #endif

            // These values are used to calculate current, voltage and power factors as per datasheet formula
            // These are the nominal values for the Sonoff POW resistors:
            // * The CURRENT_RESISTOR is the 1milliOhm copper-manganese resistor in series with the main line
            // * The VOLTAGE_RESISTOR_UPSTREAM are the 5 470kOhm resistors in the voltage divider that feeds the V2P pin in the HLW8012
            // * The VOLTAGE_RESISTOR_DOWNSTREAM is the 1kOhm resistor in the voltage divider that feeds the V2P pin in the HLW8012
            _hlw8012->setResistors(HLW8012_CURRENT_R, HLW8012_VOLTAGE_R_UP, HLW8012_VOLTAGE_R_DOWN);

            // Handle interrupts
            #if HLW8012_USE_INTERRUPTS
                #if HLW8012_WAIT_FOR_WIFI == 0
                    _enableInterrupts(false);
                    _enableInterrupts(true);
                #endif
            #endif

            _ready = true;
        }

        // Descriptive name of the sensor
        String description() {
            char buffer[25];

            snprintf(buffer, sizeof(buffer), "HLW8012 @ GPIO(%u,%u,%u)", _sel, _cf, _cf1);

            return String(buffer);
        }

        // Name of the sensor
        uint8_t type() {
            return SENSOR_TYPE_ENERGY;
        }

        // Current value for slot # index
        double _magnitudeCurrentValue(uint8_t index) {
            if (index == 0) return _hlw8012->getCurrent();
            if (index == 1) return _hlw8012->getVoltage();
            if (index == 2) return _hlw8012->getActivePower();
            if (index == 3) return _hlw8012->getReactivePower();
            if (index == 4) return _hlw8012->getApparentPower();
            if (index == 5) return 100 * _hlw8012->getPowerFactor();
            if (index == 6) return (_energy_offset + _hlw8012->getEnergy());

            return 0;
        }

        // Pre-read hook (usually to populate registers with up-to-date data)
        #if HLW8012_USE_INTERRUPTS && HLW8012_WAIT_FOR_WIFI
        void pre() {
            _enableInterrupts(wifiConnected());
        }
        #endif

        // Toggle between current and voltage monitoring
        #if HLW8012_USE_INTERRUPTS == 0
        // Post-read hook (usually to reset things)
        void post() {
            _hlw8012->toggleMode();
        }
        #endif // HLW8012_USE_INTERRUPTS == 0

        // Handle interrupt calls
        void ICACHE_RAM_ATTR handleInterrupt(
            uint8_t gpio
        ) {
            if (gpio == _cf) _hlw8012->cf_interrupt();
            if (gpio == _cf1) _hlw8012->cf1_interrupt();
        }

    protected:

        // ---------------------------------------------------------------------
        // Interrupt management
        // ---------------------------------------------------------------------

        void _attach(HLW8012Sensor * instance, uint8_t gpio, uint8_t mode);
        void _detach(uint8_t gpio);

        void _enableInterrupts(
            bool value
        ) {
            static uint8_t _interrupt_cf = GPIO_NONE;
            static uint8_t _interrupt_cf1 = GPIO_NONE;

            if (value) {
                if (_interrupt_cf != _cf) {
                    if (_interrupt_cf != GPIO_NONE) {
                        _detach(_interrupt_cf);
                    }

                    _attach(this, _cf, HLW8012_INTERRUPT_ON);

                    _interrupt_cf = _cf;
                }

                if (_interrupt_cf1 != _cf1) {
                    if (_interrupt_cf1 != GPIO_NONE) {
                        _detach(_interrupt_cf1);
                    }

                    _attach(this, _cf1, HLW8012_INTERRUPT_ON);

                    _interrupt_cf1 = _cf1;
                }
            } else {
                if (GPIO_NONE != _interrupt_cf) {
                    _detach(_interrupt_cf);

                    _interrupt_cf = GPIO_NONE;
                }

                if (GPIO_NONE != _interrupt_cf1) {
                    _detach(_interrupt_cf1);

                    _interrupt_cf1 = GPIO_NONE;
                }
            }
        }

        // ---------------------------------------------------------------------

        uint8_t _sel = GPIO_NONE;
        uint8_t _cf = GPIO_NONE;
        uint8_t _cf1 = GPIO_NONE;
        bool _sel_current = true;
        double _energy_offset = 0;

        HLW8012 * _hlw8012 = NULL;
};

// -----------------------------------------------------------------------------
// Interrupt helpers
// -----------------------------------------------------------------------------

HLW8012Sensor * _hlw8012_sensor_instance[10] = {NULL};

void ICACHE_RAM_ATTR _hlw8012_sensor_isr(
    uint8_t gpio
) {
    uint8_t index = gpio > 5 ? gpio-6 : gpio;

    if (_hlw8012_sensor_instance[index]) {
        _hlw8012_sensor_instance[index]->handleInterrupt(gpio);
    }
}

void ICACHE_RAM_ATTR _hlw8012_sensor_isr_0() { _hlw8012_sensor_isr(0); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_1() { _hlw8012_sensor_isr(1); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_2() { _hlw8012_sensor_isr(2); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_3() { _hlw8012_sensor_isr(3); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_4() { _hlw8012_sensor_isr(4); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_5() { _hlw8012_sensor_isr(5); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_12() { _hlw8012_sensor_isr(12); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_13() { _hlw8012_sensor_isr(13); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_14() { _hlw8012_sensor_isr(14); }
void ICACHE_RAM_ATTR _hlw8012_sensor_isr_15() { _hlw8012_sensor_isr(15); }

static void (*_hlw8012_sensor_isr_list[10])() = {
    _hlw8012_sensor_isr_0, _hlw8012_sensor_isr_1, _hlw8012_sensor_isr_2,
    _hlw8012_sensor_isr_3, _hlw8012_sensor_isr_4, _hlw8012_sensor_isr_5,
    _hlw8012_sensor_isr_12, _hlw8012_sensor_isr_13, _hlw8012_sensor_isr_14,
    _hlw8012_sensor_isr_15
};

void HLW8012Sensor::_attach(
    HLW8012Sensor * instance,
    uint8_t gpio,
    uint8_t mode
) {
    _detach(gpio);

    uint8_t index = gpio > 5 ? gpio-6 : gpio;

    _hlw8012_sensor_instance[index] = instance;

    attachInterrupt(gpio, _hlw8012_sensor_isr_list[index], mode);

    #if SENSOR_DEBUG
        DEBUG_MSG(PSTR("[SENSOR] GPIO%u interrupt attached to %s\n"), gpio, instance->description().c_str());
    #endif
}

void HLW8012Sensor::_detach(
    uint8_t gpio
) {
    uint8_t index = gpio > 5 ? gpio-6 : gpio;

    if (_hlw8012_sensor_instance[index]) {
        detachInterrupt(gpio);

        #if SENSOR_DEBUG
            DEBUG_MSG(PSTR("[SENSOR] GPIO%u interrupt detached from %s\n"), gpio, _hlw8012_sensor_instance[index]->description().c_str());
        #endif

        _hlw8012_sensor_instance[index] = NULL;
    }
}

#endif // SENSOR_SUPPORT && HLW8012_SUPPORT
