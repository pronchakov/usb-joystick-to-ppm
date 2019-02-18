#include "hidjoystickrptparser.h"

JoystickReportParser::JoystickReportParser(int *ppm, int *cycleCount, bool *is_led_on) :ppm_array(ppm), cc(cycleCount), ilo(is_led_on) {
    Serial.println("Initialize JoystickReportParser");
}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
    JoystickData *data = (JoystickData*) buf;

    cc++;

    if (*cc >= 1000) {
        if (*ilo) {
            digitalWrite(7, LOW);
            *ilo = false;
        } else {
            digitalWrite(7, HIGH);
            *ilo = true;
        }
        cc = 0;
    }

    ppm_array[0] = map(data->x, 20, 990, 995, 2005);
    ppm_array[1] = map(data->y, 30, 980, 995, 2005);
    ppm_array[2] = map(data->throttle, 0, 995, 970, 2010);
    ppm_array[3] = map(data->rudder, 0, 1010, 995, 2005);

    for (int i = 0; i < 4; i++) {
        if (ppm_array[i] < 1000) {
            ppm_array[i] = 1000;
        } else if (ppm_array[i] > 2000) {
            ppm_array[i] = 2000;
        }
    }


}