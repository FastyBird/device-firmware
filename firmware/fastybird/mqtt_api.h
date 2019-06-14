/*

FASTYBIRD MQTT API MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && MQTT_SUPPORT

#include "./callbacks.h"

String fastybirdThingSN() {
    char buffer[20];

    snprintf_P(buffer, sizeof(buffer), PSTR("%08X"), ESP.getChipId());

    return String(buffer);
}

// -----------------------------------------------------------------------------

String fastybirdMqttApiBuildTopicPrefix(
    const char * thingId
) {
    String topic_prefix;

    if ((fastybirdThingSN()).equals(String(thingId)) == false) {
        // Replace identifier
        topic_prefix = FASTYBIRD_MQTT_NODE_BASE_TOPIC;
        topic_prefix.replace("{cloudThingId}", (fastybirdThingSN()).c_str());
        topic_prefix.replace("{nodeId}", thingId);

    } else {
        // Replace identifier
        topic_prefix = FASTYBIRD_MQTT_BASE_TOPIC;
        topic_prefix.replace("{cloudThingId}", thingId);
    }

    // Check if topic prefix is ending with "/"
    if (!topic_prefix.endsWith("/")) {
        // ...if not, add it
        topic_prefix = topic_prefix + "/";
    }

    return topic_prefix;
}

// -----------------------------------------------------------------------------

bool fastybirdMqttApiIsSameTopic(
    const char * compareTopic,
    const char * topic
) {
    return (String(topic)).endsWith(String(compareTopic));
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiConvertChannelName(
    const char * type
) {
    if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_ANALOG_SENSOR) == 0) {
        return FASTYBIRD_CHANNEL_ANALOG_INPUT;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_ANALOG_ACTOR) == 0) {
        return FASTYBIRD_CHANNEL_ANALOG_OUTPUT;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_BINARY_SENSOR) == 0) {
        return FASTYBIRD_CHANNEL_DIGITAL_INPUT;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_BINARY_ACTOR) == 0) {
        return FASTYBIRD_CHANNEL_DIGITAL_OUTPUT;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_BUTTON) == 0) {
        return FASTYBIRD_CHANNEL_BUTTON;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_ENERGY) == 0) {
        return FASTYBIRD_CHANNEL_ENERGY;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_LED) == 0) {
        return FASTYBIRD_CHANNEL_LED;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_LIGHT) == 0) {
        return FASTYBIRD_CHANNEL_LIGHT;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_SWITCH) == 0) {
        return FASTYBIRD_CHANNEL_RELAY;
    }

    return String(type);
}

// -----------------------------------------------------------------------------

void _fastybirdMQTTOnConnect() {
    // Unsubscribe from all thing topics
    mqttUnsubscribe((fastybirdMqttApiBuildTopicPrefix((fastybirdThingSN()).c_str()) + "#").c_str());

    fastybirdResetThingInitialization();
}

// -----------------------------------------------------------------------------

void _fastybirdMQTTOnDisconnect() {
    fastybirdResetThingInitialization();
}

// -----------------------------------------------------------------------------

bool _fastybirdIsApiReady() {
    return mqttConnected();
}

// -----------------------------------------------------------------------------

void _fastybirdBeforeInitialization() {
    if (mqttConnected()) {
        // Unsubscribe from all thing topics
        mqttUnsubscribe((fastybirdMqttApiBuildTopicPrefix((fastybirdThingSN()).c_str()) + "#").c_str());
    }
}

// -----------------------------------------------------------------------------
// MODULE: INCLUDE REST OF METHODS
// -----------------------------------------------------------------------------

#include "./mqtt_api/thing_api.h"
#include "./mqtt_api/channel_api.h"

// -----------------------------------------------------------------------------

void _fastybirdApiRestore(
    std::vector<fastybird_channel_t> channels
) {
    #if SCHEDULER_SUPPORT
        if (_fastybird_channels_report_scheduler_callbacks.size() > 0) {
            for (uint8_t i = 0; i < _fastybird_channels_report_scheduler_callbacks.size(); i++) {
                if (!(_fastybird_channels_report_scheduler_callbacks[i])()) {
                    return;
                }
            }
        }
    #endif
}

// -----------------------------------------------------------------------------

void _fastybirdApiSetup() {
    mqttOnConnectRegister(_fastybirdMQTTOnConnect);
    mqttOnConnectRegister(_fastybirdMQTTOnDisconnect);

    char buffer[100];
    
    strcpy(buffer, _fastybirdMqttApiCreatePropertyTopicString((fastybirdThingSN()).c_str(), FASTYBIRD_PROPERTY_STATE).c_str());

    mqttSetWill(
        buffer,
        FASTYBIRD_STATUS_LOST
    );
}

// -----------------------------------------------------------------------------

void _fastybirdOnHeartbeat() {
    mqttSend(
        _fastybirdMqttApiCreateStatTopicString((fastybirdThingSN()).c_str(), FASTYBIRD_STAT_FREE_HEAP).c_str(),
        String(getFreeHeap()).c_str()
    );

    mqttSend(
        _fastybirdMqttApiCreateStatTopicString((fastybirdThingSN()).c_str(), FASTYBIRD_STAT_UPTIME).c_str(),
        String(getUptime()).c_str()
    );

    #if WIFI_SUPPORT
        mqttSend(
            _fastybirdMqttApiCreateStatTopicString((fastybirdThingSN()).c_str(), FASTYBIRD_STAT_RSSI).c_str(),
            String(WiFi.RSSI()).c_str()
        );
        mqttSend(
            _fastybirdMqttApiCreateStatTopicString((fastybirdThingSN()).c_str(), FASTYBIRD_STAT_SSID).c_str(),
            getNetwork().c_str()
        );
    #endif

    mqttSend(
        _fastybirdMqttApiCreateStatTopicString((fastybirdThingSN()).c_str(), FASTYBIRD_STAT_CPU_LOAD).c_str(),
        String(systemLoadAverage()).c_str()
    );

    #if ADC_MODE_VALUE == ADC_VCC
        mqttSend(
            _fastybirdMqttApiCreateStatTopicString((fastybirdThingSN()).c_str(), FASTYBIRD_STAT_VCC).c_str(),
            String(ESP.getVcc()).c_str()
        );
    #endif
}

#endif
