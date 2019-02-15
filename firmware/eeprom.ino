/*

EEPROM MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#include <EEPROM_Rotate.h>

// =============================================================================

bool _eeprom_commit = false;
bool _eeprom_last_commit_result = false;
uint32_t _eeprom_commit_count = 0;

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

bool _eepromCommit() {
    _eeprom_commit_count++;
    _eeprom_last_commit_result = EEPROMr.commit();

    return _eeprom_last_commit_result;
}

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

void eepromRotate(bool value) {
    // Enable/disable EEPROM rotation only if we are using more sectors than the
    // reserved by the memory layout
    if (EEPROMr.size() > EEPROMr.reserved()) {
        if (value) {
            DEBUG_MSG(PSTR("[EEPROM] Reenabling EEPROM rotation\n"));

        } else {
            DEBUG_MSG(PSTR("[EEPROM] Disabling EEPROM rotation\n"));
        }

        EEPROMr.rotate(value);
    }
}

// -----------------------------------------------------------------------------

uint32_t eepromCurrent() {
    return EEPROMr.current();
}

// -----------------------------------------------------------------------------

String eepromSectors() {
    String response;

    for (uint32_t i = 0; i < EEPROMr.size(); i++) {
        if (i > 0) response = response + String(", ");
        response = response + String(EEPROMr.base() - i);
    }

    return response;
}

// -----------------------------------------------------------------------------

void eepromSectorsDebug() {
    DEBUG_MSG(PSTR("[MAIN] EEPROM sectors: %s\n"), (char *) eepromSectors().c_str());
    DEBUG_MSG(PSTR("[MAIN] EEPROM current: %lu\n"), eepromCurrent());
}

// -----------------------------------------------------------------------------

void eepromCommit() {
    _eeprom_commit = true;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void eepromSetup() {
    #ifdef EEPROM_ROTATE_SECTORS
        EEPROMr.size(EEPROM_ROTATE_SECTORS);
    #else
        // If the memory layout has more than one sector reserved use those,
        // otherwise calculate pool size based on memory size.
        if (EEPROMr.size() == 1) {
            if (EEPROMr.last() > 1000) { // 4Mb boards
                EEPROMr.size(4);

            } else if (EEPROMr.last() > 250) { // 1Mb boards
                EEPROMr.size(2);
            }
        }
    #endif

    EEPROMr.offset(EEPROM_ROTATE_DATA);
    EEPROMr.begin(EEPROM_SIZE);

    firmwareRegisterLoop(eepromLoop);
}

// -----------------------------------------------------------------------------

void eepromLoop() {
    if (_eeprom_commit) {
        _eepromCommit();
        _eeprom_commit = false;
    }
}
