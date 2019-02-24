#define AILERON_REVERSE 0
#define ELEVATOR_REVERSE 0
#define THROTTLE_REERSE 0
#define RUDDER_REVERSE 0

#include "hidjoystickrptparser.h"

JoystickReportParser::JoystickReportParser(int *ppm) :ppm_array(ppm) {
    Serial.println("Initialize JoystickReportParser");
}

void JoystickReportParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
    JoystickData *data = (JoystickData*) buf;

    if (data->package_type == 1) {
        if (AILERON_REVERSE) {
            int16_t delta = AILERON_MID - data->ailerons;
            if (delta >= AILERON_MID) {
                delta = AILERON_MID - 1;
            } else if (delta <= -AILERON_MID) {
                delta = -AILERON_MID + 1;
            }
            data->ailerons = AILERON_MID + delta;
        }

        if (ELEVATOR_REVERSE) {
            int16_t delta = ELEVATOR_MID - data->elevator;
            if (delta >= ELEVATOR_MID) {
                delta = ELEVATOR_MID - 1;
            } else if (delta <= -ELEVATOR_MID) {
                delta = -ELEVATOR_MID + 1;
            }
            data->elevator = ELEVATOR_MID + delta;
        }

        if (THROTTLE_REERSE) {
            int16_t delta = THROTTLE_MID - data->throttle;
            if (delta >= THROTTLE_MID) {
                delta = THROTTLE_MID - 1;
            } else if (delta <= -THROTTLE_MID) {
                delta = -THROTTLE_MID + 1;
            }
            data->throttle = THROTTLE_MID + delta;
        }

        if (RUDDER_REVERSE) {
            int16_t delta = RUDDER_MID - data->rudder;
            if (delta >= RUDDER_MID) {
                delta = RUDDER_MID - 1;
            } else if (delta <= -RUDDER_MID) {
                delta = -RUDDER_MID + 1;
            }
            data->rudder = RUDDER_MID + delta;
        }

        ppm_array[0] = map(data->ailerons, AILERON_MIN, AILERON_MAX, 995, 2005);
        ppm_array[1] = map(data->elevator, ELEVATOR_MIN, ELEVATOR_MAX, 995, 2005);
        ppm_array[2] = map(data->throttle, THROTTLE_MIN, THROTTLE_MAX, 995, 2005);
        ppm_array[3] = map(data->rudder, RUDDER_MIN, RUDDER_MAX, 995, 2005);

        for (int i = 0; i < 4; i++) {
            if (ppm_array[i] < 1000) {
                ppm_array[i] = 1000;
            } else if (ppm_array[i] > 2000) {
                ppm_array[i] = 2000;
            }
        }

        Serial.print("package_type: ");
        Serial.print(data->package_type);
        Serial.print(", x: ");
        Serial.print(ppm_array[0]);
        Serial.print("(");
        Serial.print(data->ailerons);
        Serial.print("), y: ");
        Serial.print(ppm_array[1]);
        Serial.print("(");
        Serial.print(data->elevator);
        Serial.print("), th: ");
        Serial.print(ppm_array[2]);
        Serial.print("(");
        Serial.print(data->throttle);
        Serial.print("), ru: ");
        Serial.print(ppm_array[3]);
        Serial.print("(");
        Serial.print(data->rudder);
        Serial.println(")");
    }
}