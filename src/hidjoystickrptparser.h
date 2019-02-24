#if !defined(__HIDJOYSTICKRPTPARSER_H__)
#define __HIDJOYSTICKRPTPARSER_H__

#include "usbhid.h"

#include "joysticks/J1.h"

struct JoystickData {
    union {
        uint64_t axes;
        struct {
            uint8_t package_type: 8;
            uint16_t ailerons : AILERON_DATA_SIZE;
            uint16_t elevator : ELEVATOR_DATA_SIZE;
            uint16_t throttle : THROTTLE_DATA_SIZE;
            uint16_t rudder : RUDDER_DATA_SIZE;
            uint16_t padding_right : 16;
        };
    };
};

class JoystickReportParser : public HIDReportParser {
    int *ppm_array;
public:
    JoystickReportParser(int *ppm);

    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

#endif // __HIDJOYSTICKRPTPARSER_H__