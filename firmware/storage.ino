/*

MEMORY STORAGE MODULE

Copyright (C) 2018 FastyBird s.r.o. <code@fastybird.com>

*/

#if SPIFFS_SUPPORT

#include <FS.h>

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

String storageReadConfiguration(
    const char * filename
) {
    if (SPIFFS.exists(filename)) {
        File file = SPIFFS.open(filename, "r");

        if (!file) {
            DEBUG_MSG(PSTR("[INFO][STORAGE] Open file: %s for reading configuration failed\n"), filename);

            return String("");
        }
        
        if (!file.size()) {
            DEBUG_MSG(PSTR("[INFO][STORAGE] Filesize of file: %s is invalid\n"), filename);

            return String("");
        }
        
        String configuration;

        while (file.available())
        {
            configuration += char(file.read());
        }

        file.close();

        configuration.trim();

        return configuration;

    } else {
        DEBUG_MSG(PSTR("[INFO][STORAGE] File: %s for reading configuration does not exists\n"), filename);
    }

    return String("");
}

// -----------------------------------------------------------------------------

bool storageWriteConfiguration(
    const char * filename,
    String configuration
) {
    String saved_configuration = storageReadConfiguration(filename);

    if (strcmp(saved_configuration.c_str(), configuration.c_str()) == 0) {
        DEBUG_MSG(PSTR("[INFO][STORAGE] Saved file: %s has same content\n"), filename);

        return true;
    }

    File file = SPIFFS.open(filename, "w");

    if (!file) {
        DEBUG_MSG(PSTR("[INFO][STORAGE] Open file: %s for storing configuration failed\n"), filename);

        return false;
    }

    DEBUG_MSG(PSTR("[INFO][STORAGE] WRITING: %s\n"), configuration.c_str());

    file.println(configuration.c_str());

    file.close();

    DEBUG_MSG(PSTR("[INFO][STORAGE] Stored configuration to file: %s\n"), filename);

    return true;
}

// -----------------------------------------------------------------------------
// MODULE CORE
// -----------------------------------------------------------------------------

void storageSetup()
{
    SPIFFS.begin();
}

#endif // SPIFFS_SUPPORT