/*

FASTYBIRD CHANNEL MQTT MODULE

Copyright (C) 2018 FastyBird Ltd. <info@fastybird.com>

*/

#if FASTYBIRD_SUPPORT && MQTT_SUPPORT

// -----------------------------------------------------------------------------
// MODULE PRIVATE
// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * thingId,
    const char * channel,
    String topic
) {
    topic.replace("{channel}", channel);

    return fastybirdMqttApiBuildTopicPrefix(thingId) + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * thingId,
    const char * channel,
    String topic,
    String replace,
    String replaceWith
) {
    topic = _fastybirdMqttApiCreateChannelTopicString(thingId, channel, topic);
    topic.replace("{" + replace + "}", replaceWith);

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * thingId,
    const char * channel,
    String topic,
    String replace,
    String replaceWith,
    String secondReplace,
    String secondReplaceWith
) {
    topic = _fastybirdMqttApiCreateChannelTopicString(thingId, channel, topic, replace, replaceWith);
    topic.replace("{" + secondReplace + "}", secondReplaceWith);

    return topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiCreateChannelTopicString(
    const char * thingId,
    const char * channel,
    int channelNo,
    String topic,
    String replace,
    String replaceWith
) {
    // Create channel name with replacable suffix: somechannel_%d
    char channel_with_suffix[strlen(channel)+ 5];
    strcpy(channel_with_suffix, channel);
    strcat(channel_with_suffix, FASTYBIRD_CHANNEL_ARRAY_SUFFIX);

    char channel_topic[strlen(channel_with_suffix) + 3];
    sprintf(channel_topic, channel_with_suffix, channelNo);

    topic.replace("{channel}", channel_topic);
    topic.replace("{" + replace + "}", replaceWith);

    return fastybirdMqttApiBuildTopicPrefix(thingId) + topic;
}

// -----------------------------------------------------------------------------

String _fastybirdMqttApiConvertChannelName(
    const char * type
) {
    if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_ANALOG_SENSOR) == 0) {
        return FASTYBIRD_CHANNEL_ANALOG_INPUT;

    } else if (strcmp(type, FASTYBIRD_CHANNEL_TYPE_BINARY_SENSOR) == 0) {
        return FASTYBIRD_CHANNEL_DIGITAL_INPUT;

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

    return "";
}

// -----------------------------------------------------------------------------

#if DIRECT_CONTROL_SUPPORT
    bool _fastybirdMqttApiChannelSubscribeDirectControls(
        fastybird_channel_t channelStructure
    ) {
        unsigned int packet_id;

        for (unsigned int channel_id = 0; channel_id < channelStructure.length; channel_id++) {
            for (unsigned int i = 0; i < DIRECT_CONTROL_MAX_CONTROLS; i++) {
                if (!hasSetting(directControlCreateSettingsKey("dcControlAction_", channelStructure.type, i), channel_id)) {
                    break;
                }

                if (getSetting(directControlCreateSettingsKey("dcEnabled_", channelStructure.type, i), channel_id, "").toInt() == 1) {
                    String listenAction = getSetting(directControlCreateSettingsKey("dcListenAction_", channelStructure.type, i), channel_id, "");
                    String controlAction = getSetting(directControlCreateSettingsKey("dcControlAction_", channelStructure.type, i), channel_id, "");
                    String expression = getSetting(directControlCreateSettingsKey("dcExpression_", channelStructure.type, i), channel_id, "");
                    String property = getSetting(directControlCreateSettingsKey("dcControlProperty_", channelStructure.type, i), channel_id, "");

                    packet_id = mqttSubscribeRaw(
                        getSetting(directControlCreateSettingsKey("dcListenTopic_", channelStructure.type, i), channel_id, "").c_str(),
                        [channelStructure, channel_id, listenAction, controlAction, property, expression](const char * topic, const char * payload) {
                            if (
                                strcmp(FASTYBIRD_DIRECT_CONTROL_EXPRESSION_EQ, expression.c_str()) == 0
                                && strcmp(payload, listenAction.c_str()) == 0
                            ) {
                                for (unsigned int prop_i = 0; prop_i < channelStructure.properties.size(); prop_i++) {
                                    if (strcmp(channelStructure.properties[prop_i].type, property.c_str()) == 0) {
                                        channelStructure.properties[prop_i].payloadCallback(
                                            channel_id,
                                            controlAction.c_str()
                                        );
                                    }
                                }

                                return;
                            }
                        }
                    );

                    if (packet_id == 0) return false;
                }
            }
        }

        return true;
    }

// -----------------------------------------------------------------------------

    bool _fastybirdMqttApiChannelUnsubscribeDirectControls(
        fastybird_channel_t channelStructure
    ) {
        unsigned int packet_id;

        for (unsigned int channel_id = 0; channel_id < channelStructure.length; channel_id++) {
            for (unsigned int dc_i = 0; dc_i < DIRECT_CONTROL_MAX_CONTROLS; dc_i++) {
                if (hasSetting(directControlCreateSettingsKey("dcControlAction_", channelStructure.type, dc_i), channel_id)) {
                    packet_id = mqttUnsubscribeRaw(getSetting(directControlCreateSettingsKey("dcListenTopic_", channelStructure.type, dc_i), channel_id, "").c_str());

                    if (packet_id == 0) return false;
                }
            }
        }

        return true;
    }

#endif // DIRECT_CONTROL_SUPPORT

// -----------------------------------------------------------------------------
// MODULE API
// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelName(
    const char * thingId,
    fastybird_channel_t channel,
    const char * name
) {
    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_NAME
        ).c_str(),
        name
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelName(
    fastybird_channel_t channel,
    const char * name
) {
    return _fastybirdPropagateChannelName(_fastybird_mqtt_thing_id, channel, name);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelType(
    const char * thingId,
    fastybird_channel_t channel,
    const char * type
) {
    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_TYPE
        ).c_str(),
        type
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelType(
    fastybird_channel_t channel,
    const char * type
) {
    return _fastybirdPropagateChannelType(_fastybird_mqtt_thing_id, channel, type);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelProperties(
    const char * thingId,
    fastybird_channel_t channel,
    std::vector<fastybird_channel_property_t> properties
) {
    char formatted_properties[50];

    if (properties.size() <= 0) {
        return true;
    }

    strcpy(formatted_properties, properties[0].type);

    for (unsigned int i = 1; i < properties.size(); i++) {
        strcat(formatted_properties, ",");
        strcat(formatted_properties, properties[i].type);
    }

    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTIES
        ).c_str(),
        formatted_properties
    );

    if (packet_id == 0) return false;

    String topic;

    for (unsigned int i = 0; i < channel.length; i++) {
        for (unsigned int j = 0; j < properties.size(); j++) {
            fastybird_channel_property_t property = properties[j];

            if (property.settable) {
                if (channel.length > 1) {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        i,
                        FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE,
                        "property",
                        property.type
                    );

                } else {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        FASTYBIRD_TOPIC_CHANNEL_PROPERTY_RECEIVE,
                        "property",
                        property.type
                    );
                }

                packet_id = mqttSubscribeRaw(
                    topic.c_str(),
                    [channel, i, property](const char * topic, const char * payload) {
                        property.payloadCallback(i, payload);
                    }
                );

                if (packet_id == 0) return false;
            }
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelProperties(
    fastybird_channel_t channel,
    std::vector<fastybird_channel_property_t> properties
) {
    return _fastybirdPropagateChannelProperties(_fastybird_mqtt_thing_id, channel, properties);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSize(
    const char * thingId,
    fastybird_channel_t channel,
    unsigned int size
) {
    if (size == 1) {
        return true;
    }

    unsigned int packet_id;

    char array_size[6];
    sprintf(array_size, "0-%d", (size - 1));

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_ARRAY
        ).c_str(),
        array_size
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSize(
    fastybird_channel_t channel,
    unsigned int size
) {
    return _fastybirdPropagateChannelSize(_fastybird_mqtt_thing_id, channel, size);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyName(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_NAME,
            "property",
            property.type
        ).c_str(),
        property.name
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyName(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyName(_fastybird_mqtt_thing_id, channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertySettable(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_SETTABLE,
            "property",
            property.type
        ).c_str(),
        property.settable ? FASTYBIRD_PROPERTY_IS_SETTABLE : FASTYBIRD_PROPERTY_IS_NOT_SETTABLE
    );

    if (packet_id == 0) return false;

    if (property.settable == true) {
        // TODO: subscribe for property
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertySettable(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertySettable(_fastybird_mqtt_thing_id, channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyDataType(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_DATA_TYPE,
            "property",
            property.type
        ).c_str(),
        property.dataType
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyDataType(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyDataType(_fastybird_mqtt_thing_id, channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyFormat(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    char payload[30];

    strcpy(payload, property.format[0].c_str());

    for (unsigned int i = 1; i < property.format.size(); i++) {
        strcat(payload, ",");
        strcat(payload, property.format[i].c_str());
    }

    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_FORMAT,
            "property",
            property.type
        ).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyFormat(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyFormat(_fastybird_mqtt_thing_id, channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyMapping(
    const char * thingId,
    fastybird_channel_t channel,
    const char * property,
    fastybird_channel_property_mapping_t mapping
) {
    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY_MAPPING,
            "property",
            property,
            "mapping",
            mapping.from
        ).c_str(),
        mapping.to
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyMappings(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    if (property.mappings.size() <= 0) {
        return true;
    }

    for (unsigned int i = 0; i < property.mappings.size(); i++) {
        if (!_fastybirdPropagateChannelPropertyMapping(thingId, channel, property.type, property.mappings[i])) {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelPropertyMappings(
    fastybird_channel_t channel,
    fastybird_channel_property_t property
) {
    return _fastybirdPropagateChannelPropertyMappings(_fastybird_mqtt_thing_id, channel, property);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelControlConfiguration(
    const char * thingId,
    fastybird_channel_t channel,
    std::vector<String> controls
) {
    if (controls.size() <= 0) {
        return true;
    }

    char payload[80];

    strcpy(payload, controls[0].c_str());

    for (unsigned int i = 1; i < controls.size(); i++) {
        strcat(payload, ",");
        strcat(payload, controls[i].c_str());
    }

    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL
        ).c_str(),
        payload
    );

    if (packet_id == 0) return false;

    String topic;

    for (unsigned int i = 0; i < channel.length; i++) {
        #if FASTYBIRD_ENABLE_CONFIGURATION
            if (channel.isConfigurable) {
                if (channel.length > 1) {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        i,
                        FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                        "control",
                        FASTYBIRD_CHANNEL_CONTROL_CONFIGURATION
                    );

                } else {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                        "control",
                        FASTYBIRD_CHANNEL_CONTROL_CONFIGURATION
                    );
                }

                packet_id = mqttSubscribeRaw(
                    topic.c_str(),
                    [channel, i](const char * topic, const char * payload) {
                        DynamicJsonBuffer jsonBuffer;

                        // Parse payload
                        JsonObject& root = jsonBuffer.parseObject(payload);

                        if (root.success()) {
                            DEBUG_MSG(PSTR("[FASTYBIRD] Sending configuration to channel: %s:%d\n"), channel.type, i);

                            channel.configureCallback(i, root);

                            DEBUG_MSG(PSTR("[FASTYBIRD] Changes were saved\n"));

                            #if WEB_SUPPORT && WS_SUPPORT
                                wsReportConfiguration();
                            #endif

                            // Reload & cache settings
                            firmwareReload();

                        } else {
                            DEBUG_MSG(PSTR("[FASTYBIRD] Error parsing settings data\n"));
                        }
                    }
                );

                if (packet_id == 0) return false;
            }
        #endif

        #if DIRECT_CONTROL_SUPPORT
            if (channel.hasDirectControl) {
                if (channel.length > 1) {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        i,
                        FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                        "control",
                        FASTYBIRD_CHANNEL_CONTROL_DIRECT_CONTROL
                    );

                } else {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                        "control",
                        FASTYBIRD_CHANNEL_CONTROL_DIRECT_CONTROL
                    );
                }

                packet_id = mqttSubscribeRaw(
                    topic.c_str(),
                    [channel, i](const char * topic, const char * payload) {
                        DynamicJsonBuffer jsonBuffer;

                        // Parse payload
                        JsonArray& root = jsonBuffer.parseArray(payload);

                        if (root.success()) {
                            _fastybirdMqttApiChannelUnsubscribeDirectControls(channel);

                            DEBUG_MSG(PSTR("[FASTYBIRD] Sending direct controls configuration to channel: %s\n"), channel.type);

                            channel.configureDirectControlsCallback(i, root);

                            _fastybirdMqttApiChannelSubscribeDirectControls(channel);

                            DEBUG_MSG(PSTR("[FASTYBIRD] Changes were saved\n"));

                            #if WEB_SUPPORT && WS_SUPPORT
                                wsReportConfiguration();
                            #endif

                            // Reload & cache settings
                            firmwareReload();

                        } else {
                            DEBUG_MSG(PSTR("[FASTYBIRD] Error parsing direct controls configuration data\n"));
                        }
                    }
                );

                if (packet_id == 0) return false;

                if (!_fastybirdMqttApiChannelSubscribeDirectControls(channel)) {
                    return false;
                }
            }
        #endif

        #if SCHEDULER_SUPPORT
            if (channel.hasScheduler) {
                if (channel.length > 1) {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        i,
                        FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                        "control",
                        FASTYBIRD_CHANNEL_CONTROL_SCHEDULER
                    );

                } else {
                    topic = _fastybirdMqttApiCreateChannelTopicString(
                        thingId,
                        _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
                        FASTYBIRD_TOPIC_CHANNEL_CONTROL_RECEIVE,
                        "control",
                        FASTYBIRD_CHANNEL_CONTROL_SCHEDULER
                    );
                }

                packet_id = mqttSubscribeRaw(
                    topic.c_str(),
                    [channel, i](const char * topic, const char * payload) {
                        DynamicJsonBuffer jsonBuffer;

                        // Parse payload
                        JsonArray& root = jsonBuffer.parseArray(payload);

                        if (root.success()) {
                            DEBUG_MSG(PSTR("[FASTYBIRD] Sending scheduler configuration to channel: %s\n"), channel.type);

                            channel.configureSchedulesCallback(i, root);

                            DEBUG_MSG(PSTR("[FASTYBIRD] Changes were saved\n"));

                            #if WEB_SUPPORT && WS_SUPPORT
                                wsReportConfiguration();
                            #endif

                            // Reload & cache settings
                            firmwareReload();

                        } else {
                            DEBUG_MSG(PSTR("[FASTYBIRD] Error parsing scheduler configuration data\n"));
                        }
                    }
                );

                if (packet_id == 0) return false;
            }
        #endif
    }

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelControlConfiguration(
    fastybird_channel_t channel,
    std::vector<String> controls
) {
    return _fastybirdPropagateChannelControlConfiguration(_fastybird_mqtt_thing_id, channel, controls);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfigurationSchema(
    const char * thingId,
    fastybird_channel_t channel,
    String payload
) {
    unsigned int packet_id;

    packet_id = mqttSendRaw(
        _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_SCHEMA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_CONFIGURATION
        ).c_str(),
        payload.c_str()
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfigurationSchema(
    fastybird_channel_t channel,
    String payload
) {
    return _fastybirdPropagateChannelConfigurationSchema(_fastybird_mqtt_thing_id, channel, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfiguration(
    const char * thingId,
    fastybird_channel_t channel,
    unsigned int channelId,
    String payload
) {
    String topic;

    if (channel.length > 1) {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_CONFIGURATION
        );

    } else {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_CONFIGURATION
        );
    }

    unsigned int packet_id;

    packet_id = mqttSendRaw(
        topic.c_str(),
        payload.c_str()
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelConfiguration(
    fastybird_channel_t channel,
    unsigned int channelId,
    String payload
) {
    return _fastybirdPropagateChannelConfiguration(_fastybird_mqtt_thing_id, channel, channelId, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelDirectControlConfiguration(
    const char * thingId,
    fastybird_channel_t channel,
    unsigned int channelId,
    String payload
) {
    String topic;

    if (channel.length > 1) {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_DIRECT_CONTROL
        );

    } else {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_DIRECT_CONTROL
        );
    }

    unsigned int packet_id;

    packet_id = mqttSendRaw(
        topic.c_str(),
        payload.c_str()
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelDirectControlConfiguration(
    fastybird_channel_t channel,
    unsigned int channelId,
    String payload
) {
    return _fastybirdPropagateChannelDirectControlConfiguration(_fastybird_mqtt_thing_id, channel, channelId, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSchedulerConfiguration(
    const char * thingId,
    fastybird_channel_t channel,
    unsigned int channelId,
    String payload
) {
    String topic;

    if (channel.length > 1) {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_SCHEDULER
        );

    } else {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_CONTROL_DATA,
            "control",
            FASTYBIRD_CHANNEL_CONTROL_SCHEDULER
        );
    }

    unsigned int packet_id;

    packet_id = mqttSendRaw(
        topic.c_str(),
        payload.c_str()
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelSchedulerConfiguration(
    fastybird_channel_t channel,
    unsigned int channelId,
    String payload
) {
    return _fastybirdPropagateChannelSchedulerConfiguration(_fastybird_mqtt_thing_id, channel, channelId, payload);
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelValue(
    const char * thingId,
    fastybird_channel_t channel,
    fastybird_channel_property_t property,
    unsigned int channelId,
    const char * payload
) {
    String topic;

    if (channel.length > 1) {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            channelId,
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY,
            "property",
            property.type
        );

    } else {
        topic = _fastybirdMqttApiCreateChannelTopicString(
            thingId,
            _fastybirdMqttApiConvertChannelName(channel.type).c_str(),
            FASTYBIRD_TOPIC_CHANNEL_PROPERTY,
            "property",
            property.type
        );
    }

    unsigned int packet_id;

    packet_id = mqttSendRaw(
        topic.c_str(),
        payload
    );

    if (packet_id == 0) return false;

    return true;
}

// -----------------------------------------------------------------------------

bool _fastybirdPropagateChannelValue(
    fastybird_channel_t channel,
    fastybird_channel_property_t property,
    unsigned int channelId,
    const char * payload
) {
    return _fastybirdPropagateChannelValue(_fastybird_mqtt_thing_id, channel, property, channelId, payload);
}

#endif
