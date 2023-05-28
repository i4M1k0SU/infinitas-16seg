#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include "16seg_htlab.h"

#define DEFAULT_VENDOR_ID 0x4d8
#define DEFAULT_PRODUCT_ID 0x3f
#define NINE_WHITESPACE "         "

bool initSegLed(SegLed* _this) {
    _this->available = false;
    memset(_this->currentString, 0x00, sizeof(_this->currentString));

    if (hid_init() != 0) {
        return false;
    }

    uint16_t vendorId = (uint16_t)GetPrivateProfileInt(
        L"16SEG",
        L"VENDOR_ID",
        DEFAULT_VENDOR_ID,
        L".\\16seg.ini"
    );

    uint16_t productId = (uint16_t)GetPrivateProfileInt(
        L"16SEG",
        L"PRODUCT_ID",
        DEFAULT_PRODUCT_ID,
        L".\\16seg.ini"
    );

    _this->device = hid_open(vendorId, productId, NULL);

    if (_this->device == NULL) {
        return false;
    }

    _this->available = true;

    return true;
}

void closeSegLed(SegLed* _this) {
    if (_this->available) {
        writeSegLed(_this, NINE_WHITESPACE);
        hid_close(_this->device);
        hid_exit();
    }
}

void writeSegLed(SegLed* _this, const char str[10]) {
    // 前回送信した値と同じ値が投げられた時は何もしない
    if (!_this->available || strcmp(str, _this->currentString) == 0) {
        return;
    }

    strcpy_s(_this->currentString, sizeof(_this->currentString), str);

    char output[13] = { 0x00, 0x1F };

    output[2] = str[0];
    output[3] = str[1];
    output[4] = str[2];
    output[5] = str[3];
    output[6] = str[4];
    output[7] = str[5];
    output[8] = str[6];
    output[9] = str[7];
    output[10] = str[8];

    output[11] = 0x00;
    output[12] = 0x00;

    hid_write(_this->device, output, sizeof(output));

    return;
}
