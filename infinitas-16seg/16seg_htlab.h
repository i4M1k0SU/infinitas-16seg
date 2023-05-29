#pragma once

#include <hidapi/hidapi.h>

typedef struct SegLed {
    bool available;
    hid_device* device;
    char currentString[10];
} SegLed;

bool initSegLed(SegLed* _this);
void closeSegLed(SegLed* _this);
void writeSegLed(SegLed* _this, const char str[10]);
