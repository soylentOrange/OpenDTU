// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <Arduino.h>

class MqttHandleDtuClass {
public:
    void init();
    void loop();

private:
    uint32_t _lastPublish;
};

extern MqttHandleDtuClass MqttHandleDtu;