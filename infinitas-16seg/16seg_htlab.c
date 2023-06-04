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

    char output[13] = { 0x00 };
    output[1] = 0x1f;
    for (int i = 0; i < 9 && str[i] != 0x00; i++)
    {
        output[i + 2] = str[i];
    }

    hid_write(_this->device, output, sizeof(output));
}
