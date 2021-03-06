/*

FIRMWARE GENERAL SETTINGS

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#pragma once

//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#ifndef LOOP_DELAY_TIME
    #define LOOP_DELAY_TIME                 10                                  // Delay for this millis in the main loop [0-250]
#endif

#ifndef ADMIN_PASSWORD
    #define ADMIN_PASSWORD                  "fibonacci"
#endif

//------------------------------------------------------------------------------
// NETWORK
//------------------------------------------------------------------------------

#ifndef NETWORK_SSL_ENABLED
    #define NETWORK_SSL_ENABLED             0                                   // Enable SSL
#endif

//------------------------------------------------------------------------------
// STABILTY CHECK
//------------------------------------------------------------------------------

#ifndef STABILTY_CHECK_ENABLED
    #define STABILTY_CHECK_ENABLED          1                                   // Enable crash check by default
#endif

#ifndef STABILTY_CHECK_TIME
    #define STABILTY_CHECK_TIME             60000                               // The system is considered stable after these many millis
#endif

#ifndef STABILTY_CHECK_MAX
    #define STABILTY_CHECK_MAX              5                                   // After this many crashes on boot
                                                                                // the system is flagged as unstable
#endif

//------------------------------------------------------------------------------
// EEPROM
//------------------------------------------------------------------------------

#define EEPROM_SIZE                         SPI_FLASH_SEC_SIZE                  // EEPROM size in bytes (1 sector = 4096 bytes)

//#define EEPROM_RORATE_SECTORS             2                                   // Number of sectors to use for EEPROM rotation
                                                                                // If not defined the firmware will use a number based
                                                                                // on the number of available sectors

#define EEPROM_CUSTOM_RESET                 0                                   // Address for the reset reason (1 byte)
#define EEPROM_CRASH_COUNTER                1                                   // Address for the crash counter (1 byte)
#define EEPROM_MESSAGE_ID                   2                                   // Address for the MQTT message id (4 bytes)
#define EEPROM_ROTATE_DATA                  6                                   // Reserved for the EEPROM_ROTATE library (3 bytes)
#define EEPROM_RELAY_STATUS                 9                                   // Address for the relay status (1 byte)
#define EEPROM_ENERGY_COUNT                 10                                  // Address for the energy counter (4 bytes)
#define EEPROM_DATA_END                     14                                  // End of custom EEPROM data block

//------------------------------------------------------------------------------
// HEARTBEAT
//------------------------------------------------------------------------------

#ifndef HEARTBEAT_MODE
    #define HEARTBEAT_MODE                  HEARTBEAT_REPEAT
#endif

#ifndef HEARTBEAT_INTERVAL
    #define HEARTBEAT_INTERVAL              300000                              // Interval between heartbeat messages (in ms)
#endif

#define UPTIME_OVERFLOW                     4294967295                          // Uptime overflow value

//------------------------------------------------------------------------------
// LOAD AVERAGE
//------------------------------------------------------------------------------

#ifndef LOADAVG_INTERVAL
    #define LOADAVG_INTERVAL                30000                               // Interval between calculating load average (in ms)
#endif

// -----------------------------------------------------------------------------
// SPIFFS
// -----------------------------------------------------------------------------

#ifndef SPIFFS_SUPPORT
    #define SPIFFS_SUPPORT                  0                                   // Do not add support for SPIFFS by default
#endif

// -----------------------------------------------------------------------------
// ADC
// -----------------------------------------------------------------------------

// Default ADC mode is to monitor internal power supply
#ifndef ADC_MODE_VALUE
    #define ADC_MODE_VALUE                  ADC_VCC
#endif

// =============================================================================
// =============================================================================
//
//                            DEVICE CORE MODULES
//
// =============================================================================
// =============================================================================

//------------------------------------------------------------------------------
// DEBUG MODULE
//------------------------------------------------------------------------------

#ifndef DEBUG_SUPPORT
    #define DEBUG_SUPPORT                   0                                   // Device debug is disabled by default
#endif

#ifndef DEBUG_SERIAL_SUPPORT
    #define DEBUG_SERIAL_SUPPORT            0                                   // Enable serial debug log
#endif

#ifndef DEBUG_PORT
    #define DEBUG_PORT                      Serial                              // Default debugging port
#endif

#ifndef DEBUG_WEB_SUPPORT
    #define DEBUG_WEB_SUPPORT               0                                   // Enable web debug log (will only work if WEB_SUPPORT is also 1)
#endif

#ifndef DEBUG_MQTT_SUPPORT
    #define DEBUG_MQTT_SUPPORT              0                                   // Enable mqtt debug log
#endif

#ifndef SERIAL_BAUDRATE
    #define SERIAL_BAUDRATE                 115200                              // Default baudrate
#endif

#ifndef DEBUG_ADD_TIMESTAMP
    #define DEBUG_ADD_TIMESTAMP             1                                   // Add timestamp to debug messages
                                                                                // (in millis overflowing every 1000 seconds)
#endif

// Second serial port (used for RX)

#ifndef SERIAL_RX_ENABLED
    #define SERIAL_RX_ENABLED               0                                   // Secondary serial port for RX
#endif

#ifndef SERIAL_RX_PORT
    #define SERIAL_RX_PORT                  Serial                              // This setting is usually defined
                                                                                // in the hardware.h file for those
                                                                                // boards that require it
#endif

#ifndef SERIAL_RX_BAUDRATE
    #define SERIAL_RX_BAUDRATE              115200                              // Default baudrate
#endif

// -----------------------------------------------------------------------------
// SETTINGS MODULE
// -----------------------------------------------------------------------------

#ifndef SETTINGS_AUTOSAVE
    #define SETTINGS_AUTOSAVE               1                                   // Autosave settings or force manual commit
#endif

#define SETTINGS_MAX_LIST_COUNT             10                                  // Maximum index for settings lists

// -----------------------------------------------------------------------------
// WIFI MODULE
// -----------------------------------------------------------------------------

#ifndef WIFI_SUPPORT
    #define WIFI_SUPPORT                    1                                   // Enable wifi support
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD                   ADMIN_PASSWORD                      // AP connect password
#endif

#ifndef WIFI_CONNECT_TIMEOUT
    #define WIFI_CONNECT_TIMEOUT            60000                               // Connecting timeout for WIFI in ms
#endif

#ifndef WIFI_RECONNECT_INTERVAL
    #define WIFI_RECONNECT_INTERVAL         180000                              // If could not connect to WIFI, retry after this time in ms
#endif

#ifndef WIFI_MAX_NETWORKS
    #define WIFI_MAX_NETWORKS               5                                   // Max number of WIFI connection configurations
#endif

#ifndef WIFI_FALLBACK_APMODE
    #define WIFI_FALLBACK_APMODE            1                                   // Fallback to AP mode if no STA connection
#endif

#ifndef WIFI_SLEEP_MODE
    #define WIFI_SLEEP_MODE                 WIFI_NONE_SLEEP                     // WIFI_NONE_SLEEP, WIFI_LIGHT_SLEEP or WIFI_MODEM_SLEEP
#endif

#ifndef WIFI_SCAN_NETWORKS
    #define WIFI_SCAN_NETWORKS              1                                   // Perform a network scan before connecting
#endif

#ifndef WIFI_PROPAGATION_CONST
    #define WIFI_PROPAGATION_CONST          4                                   // This is typically something between 2.7 to 4.3 (free space is 2)
#endif

// -----------------------------------------------------------------------------
// WEB MODULE
// -----------------------------------------------------------------------------

#ifndef WEB_SUPPORT
    #define WEB_SUPPORT                     1                                   // Enable web support
#endif

#ifndef WEB_EMBEDDED
    #define WEB_EMBEDDED                    1                                   // Build the firmware with the web interface embedded in
#endif

// This is not working at the moment!!
// Requires NETWORK_SSL_ENABLED to 1 and ESP8266 Arduino Core 2.4.0
#ifndef WEB_SSL_ENABLED
    #define WEB_SSL_ENABLED                 0                                   // Use HTTPS web interface
#endif

#ifndef WEB_USERNAME
    #define WEB_USERNAME                    "admin"                             // HTTP username
#endif

#ifndef WEB_PORT
    #define WEB_PORT                        80                                  // HTTP port
#endif

#ifndef WEB_REMOTE_DOMAIN
    #define WEB_REMOTE_DOMAIN               "*"                                 // Enable CORS for all domains
#endif

#ifndef WEB_API_DISCOVER
    #define WEB_API_DISCOVER                "/discover"                         //
#endif

#ifndef WEB_API_SIGN_IN
    #define WEB_API_SIGN_IN                 "/control/sign-in"                  //
#endif

#ifndef WEB_API_REPORT_CRASH
    #define WEB_API_REPORT_CRASH            "/control/report-crash"             // 
#endif

#ifndef WEB_API_INITIALIZE
    #define WEB_API_INITIALIZE              "/control/initialize"               //
#endif

#ifndef WEB_API_REBOOT
    #define WEB_API_REBOOT                  "/control/reboot"                   //
#endif

#ifndef WEB_API_FACTORY
    #define WEB_API_FACTORY                 "/control/factory-reset"            //
#endif

#ifndef WEB_API_FIRMWARE_UPGRADE
    #define WEB_API_FIRMWARE_UPGRADE        "/control/upgrade-firmware"         //
#endif

#ifndef WEB_API_FIRMWARE_CONFIGURATION
    #define WEB_API_FIRMWARE_CONFIGURATION  "/control/configuration"            //
#endif

#ifndef WEB_API_NETWORK_RECONNECT
    #define WEB_API_NETWORK_RECONNECT       "/control/reconnect"                //
#endif

#ifndef WEB_API_NETWORK_CONFIGURATION
    #define WEB_API_NETWORK_CONFIGURATION   "/control/network-configuration"    //
#endif

#ifndef WEB_API_WS_DATA
    #define WEB_API_WS_DATA                 "/ws/data"                          //
#endif

#ifndef WEB_API_WS_AUTH
    #define WEB_API_WS_AUTH                 "/ws/auth"                          //
#endif

// -----------------------------------------------------------------------------
// WEBSOCKETS MODULE
// -----------------------------------------------------------------------------

#ifndef WS_SUPPORT
    #define WS_SUPPORT                      1                                   // Enable WS support
#endif

#ifndef WS_BUFFER_SIZE
    #define WS_BUFFER_SIZE                  5                                   // Max number of secured websocket connections
#endif

#ifndef WS_TIMEOUT
    #define WS_TIMEOUT                      300000                              // Timeout for secured websocket
#endif

#ifndef WS_UPDATE_INTERVAL
    #define WS_UPDATE_INTERVAL              30000                               // Update clients every x seconds
#endif

// -----------------------------------------------------------------------------
// MQTT MODULE
// -----------------------------------------------------------------------------

#ifndef MQTT_SUPPORT
    #define MQTT_SUPPORT                    1                                   // MQTT support
#endif

// MQTT OVER SSL
// Using MQTT over SSL works pretty well but generates problems with the web interface.
// It could be a good idea to use it in conjuntion with WEB_SUPPORT=0.
// Requires NETWORK_SSL_ENABLED to 1 and ESP8266 Arduino Core 2.4.0.
//
// You will need the fingerprint for your MQTT server, example for CloudMQTT:
// $ echo -n | openssl s_client -connect m11.cloudmqtt.com:24055 > cloudmqtt.pem
// $ openssl x509 -noout -in cloudmqtt.pem -fingerprint -sha1

#ifndef MQTT_SSL_ENABLED
    #define MQTT_SSL_ENABLED                0                                   // By default MQTT over SSL will not be enabled
#endif

#ifndef MQTT_SSL_FINGERPRINT
    #define MQTT_SSL_FINGERPRINT            ""                                  // SSL fingerprint of the server
#endif

#ifndef MQTT_AUTOCONNECT
    #define MQTT_AUTOCONNECT                1                                   // If enabled will perform an autodiscover
#endif

#ifndef MQTT_QOS
    #define MQTT_QOS                        1                                   // MQTT QoS value for all messages
#endif

#ifndef MQTT_KEEPALIVE
    #define MQTT_KEEPALIVE                  300                                 // MQTT keepalive value
#endif

#ifndef MQTT_RECONNECT_DELAY_MIN
    #define MQTT_RECONNECT_DELAY_MIN        5000                                // Try to reconnect in 5 seconds upon disconnection
#endif

#ifndef MQTT_RECONNECT_DELAY_STEP
    #define MQTT_RECONNECT_DELAY_STEP       5000                                // Increase the reconnect delay in 5 seconds after each failed attempt
#endif

#ifndef MQTT_RECONNECT_DELAY_MAX
    #define MQTT_RECONNECT_DELAY_MAX        120000                              // Set reconnect time to 2 minutes at most
#endif

#ifndef MQTT_SKIP_RETAINED
    #define MQTT_SKIP_RETAINED              1                                   // Skip retained messages on connection
#endif

#ifndef MQTT_SKIP_TIME
    #define MQTT_SKIP_TIME                  1000                                // Skip messages for 1 second after connection
#endif

//------------------------------------------------------------------------------
// LED MODULE
//------------------------------------------------------------------------------

#ifndef LED_SUPPORT
    #define LED_SUPPORT                     1
#endif

// =============================================================================
// =============================================================================
//
//                          DEVICE FUNCTIONALS MODULES
//
// =============================================================================
// =============================================================================

//------------------------------------------------------------------------------
// BUTTON MODULE
//------------------------------------------------------------------------------

#ifndef BUTTON_SUPPORT
    #define BUTTON_SUPPORT                  0
#endif

#ifndef BUTTON_DEBOUNCE_DELAY
    #define BUTTON_DEBOUNCE_DELAY           50                                  // Debounce delay (ms)
#endif

#ifndef BUTTON_DBL_CLICK_DELAY
    #define BUTTON_DBL_CLICK_DELAY          500                                 // Time in ms to wait for a second (or third...) click
#endif

#ifndef BUTTON_DEBOUNCE_DBL_CLICK_MIN
    #define BUTTON_DEBOUNCE_DBL_CLICK_MIN   0
#endif

#ifndef BUTTON_DEBOUNCE_DBL_CLICK_MAX
    #define BUTTON_DEBOUNCE_DBL_CLICK_MAX   1000
#endif

#ifndef BUTTON_DEBOUNCE_DBL_CLICK_STEP
    #define BUTTON_DEBOUNCE_DBL_CLICK_STEP  100
#endif

#ifndef BUTTON_LNG_CLICK_DELAY
    #define BUTTON_LNG_CLICK_DELAY          1000                                // Time in ms holding the button down to get a long click
#endif

#ifndef BUTTON_LNG_LNG_CLICK_DELAY
    #define BUTTON_LNG_LNG_CLICK_DELAY      10000                               // Time in ms holding the button down to get a long-long click
#endif

//------------------------------------------------------------------------------
// RELAY MODULE
//------------------------------------------------------------------------------

// Default boot mode: 0 means OFF, 1 ON and 2 whatever was before
#ifndef RELAY_BOOT_MODE
    #define RELAY_BOOT_MODE                 RELAY_BOOT_OFF
#endif

// 0 means ANY, 1 zero or one and 2 one and only one
#ifndef RELAY_SYNC
    #define RELAY_SYNC                      RELAY_SYNC_ANY
#endif

// Default pulse mode: 0 means no pulses, 1 means normally off, 2 normally on
#ifndef RELAY_PULSE_MODE
    #define RELAY_PULSE_MODE                RELAY_PULSE_NONE
#endif

// Default pulse time in seconds
#ifndef RELAY_PULSE_TIME
    #define RELAY_PULSE_TIME                1.0
#endif

// Relay requests flood protection window - in seconds
#ifndef RELAY_FLOOD_WINDOW
    #define RELAY_FLOOD_WINDOW              3
#endif

// Allowed actual relay changes inside requests flood protection window
#ifndef RELAY_FLOOD_CHANGES
    #define RELAY_FLOOD_CHANGES             5
#endif

// Pulse with in milliseconds for a latched relay
#ifndef RELAY_LATCHING_PULSE
    #define RELAY_LATCHING_PULSE            10
#endif

// Do not save relay state after these many milliseconds
#ifndef RELAY_SAVE_DELAY
    #define RELAY_SAVE_DELAY                1000
#endif

// TODO Only single EEPROM address is used to store state, which is 1 byte
// Relay status is stored using bitfield.
// This means that, atm, we are only storing the status of the first 8 relays.
#define RELAY_SAVE_MASK_MAX                 8

// =============================================================================
// =============================================================================
//
//                          DEVICE OTHER MODULES
//
// =============================================================================
// =============================================================================


// -----------------------------------------------------------------------------
// ALEXA MODULE
// -----------------------------------------------------------------------------

// This setting defines whether Alexa support should be built into the firmware
#ifndef ALEXA_SUPPORT
    #define ALEXA_SUPPORT                   0
#endif

// This is default value for the alexaEnabled setting that defines whether
// this device should be discoberable and respond to Alexa commands.
// Both ALEXA_SUPPORT and alexaEnabled should be 1 for Alexa support to work.
#ifndef ALEXA_ENABLED
    #define ALEXA_ENABLED                   1
#endif

// -----------------------------------------------------------------------------
// VIRTUAL BUTTON MODULE
// -----------------------------------------------------------------------------

#ifndef VIRTUAL_BTN_SUPPORT
    #define VIRTUAL_BTN_SUPPORT             0
#endif

#ifndef VIRTUAL_BTN_COUNT
    #define VIRTUAL_BTN_COUNT               5
#endif

#ifndef VIRTUAL_BTN_CLEAR_INTERVAL
    #define VIRTUAL_BTN_CLEAR_INTERVAL      60000
#endif


// -----------------------------------------------------------------------------
// LIGHT MODULE
// -----------------------------------------------------------------------------

// LIGHT_PROVIDER_DIMMER can have from 1 to 5 different channels.
// They have to be defined for each device in the hardware.h file.
// If 3 or more channels first 3 will be considered RGB.
// Usual configurations are:
// 1 channels => W
// 2 channels => WW
// 3 channels => RGB
// 4 channels => RGBW
// 5 channels => RGBWW

#ifndef LIGHT_SAVE_ENABLED
    #define LIGHT_SAVE_ENABLED              1           // Light channel values saved by default after each change
#endif

#ifndef LIGHT_SAVE_DELAY
    #define LIGHT_SAVE_DELAY                5           // Persist color after 5 seconds to avoid wearing out
#endif

#ifndef LIGHT_MAX_PWM

#if LIGHT_PROVIDER == LIGHT_PROVIDER_MY92XX
    #define LIGHT_MAX_PWM                   255
#endif

#if LIGHT_PROVIDER == LIGHT_PROVIDER_DIMMER
    #define LIGHT_MAX_PWM                   10000        // 10000 * 200ns => 2 kHz
#endif

#endif // LIGHT_MAX_PWM

#ifndef LIGHT_LIMIT_PWM
    #define LIGHT_LIMIT_PWM                 LIGHT_MAX_PWM   // Limit PWM to this value (prevent 100% power)
#endif

#ifndef LIGHT_MAX_VALUE
    #define LIGHT_MAX_VALUE                 255         // Maximum light value
#endif

#ifndef LIGHT_MAX_BRIGHTNESS
    #define LIGHT_MAX_BRIGHTNESS            255         // Maximun brightness value
#endif

#define LIGHT_MIN_MIREDS                    153      // Default to the Philips Hue value that HA also use.
#define LIGHT_MAX_MIREDS                    500      // https://developers.meethue.com/documentation/core-concepts

#ifndef LIGHT_STEP
    #define LIGHT_STEP                      32          // Step size
#endif

#ifndef LIGHT_USE_COLOR
    #define LIGHT_USE_COLOR                 1           // Use 3 first channels as RGB
#endif

#ifndef LIGHT_USE_WHITE
    #define LIGHT_USE_WHITE                 0           // Use the 4th channel as (Warm-)White LEDs
#endif

#ifndef LIGHT_USE_CCT
    #define LIGHT_USE_CCT                   0           // Use the 5th channel as Coldwhite LEDs, LIGHT_USE_WHITE must be 1.
#endif

// Used when LIGHT_USE_WHITE AND LIGHT_USE_CCT is 1 - (1000000/Kelvin = MiReds)
// Warning! Don't change this yet, NOT FULLY IMPLEMENTED!
#define LIGHT_COLDWHITE_MIRED               153         // Coldwhite Strip, Value must be __BELOW__ W2!! (Default: 6535 Kelvin/153 MiRed)
#define LIGHT_WARMWHITE_MIRED               500         // Warmwhite Strip, Value must be __ABOVE__ W1!! (Default: 2000 Kelvin/500 MiRed)

#ifndef LIGHT_USE_GAMMA
    #define LIGHT_USE_GAMMA                 0           // Use gamma correction for color channels
#endif

#ifndef LIGHT_USE_CSS
    #define LIGHT_USE_CSS                   1           // Use CSS style to report colors (1=> "#FF0000", 0=> "255,0,0")
#endif

#ifndef LIGHT_USE_RGB
    #define LIGHT_USE_RGB                   0           // Use RGB color selector (1=> RGB, 0=> HSV)
#endif

#ifndef LIGHT_WHITE_FACTOR
    #define LIGHT_WHITE_FACTOR              1           // When using LIGHT_USE_WHITE with uneven brightness LEDs,
                                                        // this factor is used to scale the white channel to match brightness
#endif

#ifndef LIGHT_USE_TRANSITIONS
    #define LIGHT_USE_TRANSITIONS           1           // Transitions between colors
#endif

#ifndef LIGHT_TRANSITION_STEP
    #define LIGHT_TRANSITION_STEP           10          // Time in millis between each transtion step
#endif

#ifndef LIGHT_TRANSITION_TIME
    #define LIGHT_TRANSITION_TIME           500         // Time in millis from color to color
#endif
