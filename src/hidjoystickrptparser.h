#if !defined(__HIDJOYSTICKRPTPARSER_H__)
#define __HIDJOYSTICKRPTPARSER_H__

#include "usbhid.h"

struct JoystickData {
    union {
        uint64_t axes;
        struct {
            uint8_t padding_left: 8;
            uint16_t x : 10;
            uint16_t y : 10;
            uint16_t throttle : 10;
            uint16_t rudder : 10;
            uint16_t padding_right : 16;
        };
    };
};

class JoystickReportParser : public HIDReportParser {
    int *ppm_array;
    bool *ilo;
    int *cc;
public:
    JoystickReportParser(int *ppm, int *cycleCount, bool *is_led_on);

    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

#endif // __HIDJOYSTICKRPTPARSER_H__
