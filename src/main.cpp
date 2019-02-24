#include "Arduino.h"

#define CHANNEL_NUMBER 5  //set the number of chanels
#define CHANNEL_DEFAULT_VALUE 1500  //set the default servo value
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 8 //set PPM signal output pin on the arduino

#define MODE_BUTTONS_PRESENT 1
#define MODES_COUNT 6

int ledLatchPin = 2;
int ledClockPin = 3;
int ledDataPin = 4;

int buttonsLatchPin = 5;
int buttonsClockPin = 6;
int buttonsDataPin = 7;

byte modes[MODES_COUNT] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
int modesPPM[MODES_COUNT] = {1080, 1240, 1400, 1560, 1740, 1900};

int ppm[CHANNEL_NUMBER];

#include "usbhid.h"
#include "hiduniversal.h"
#include "usbhub.h"
#include "hidjoystickrptparser.h"

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);

JoystickReportParser Joy(ppm);

byte currentButtonsByte = modes[0];

void setup() {
    Serial.begin(115200);

    pinMode(buttonsLatchPin, OUTPUT);
    pinMode(buttonsClockPin, OUTPUT);
    pinMode(buttonsDataPin, INPUT);

    pinMode(ledLatchPin, OUTPUT);
    pinMode(ledClockPin, OUTPUT);
    pinMode(ledDataPin, OUTPUT);


    // PPM Generator setup
    for(int i=0; i<CHANNEL_NUMBER; i++){
        ppm[i]= CHANNEL_DEFAULT_VALUE;
    }

    delay(100);

    ppm[4] = modesPPM[0];
    digitalWrite(ledLatchPin, LOW);
    shiftOut(ledDataPin, ledClockPin, MSBFIRST, currentButtonsByte);
    digitalWrite(ledLatchPin, HIGH);


    pinMode(sigPin, OUTPUT);
    digitalWrite(sigPin, !onState);

    cli();
    TCCR1A = 0; // set entire TCCR1 register to 0
    TCCR1B = 0;

    OCR1A = 100;  // compare match register, change this
    TCCR1B |= (1 << WGM12);  // turn on CTC mode
    TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
    TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
    sei();

    if (Usb.Init() == -1) {
    }

    if (!Hid.SetReportParser(0, &Joy)) {
        ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);
    }
}

uint8_t shiftIn168(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; ++i) {
        digitalWrite(clockPin, LOW);
        if (bitOrder == LSBFIRST)
            value |= digitalRead(dataPin) << i;
        else
            value |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, HIGH);
    }
    return value;
}

ISR(TIMER1_COMPA_vect){
    static boolean state = true;

    TCNT1 = 0;

    if (state) {
        digitalWrite(sigPin, onState);
        OCR1A = PULSE_LENGTH * 2;
        state = false;
    } else{
        static byte cur_chan_numb;
        static unsigned int calc_rest;

        digitalWrite(sigPin, !onState);
        state = true;

        if(cur_chan_numb >= CHANNEL_NUMBER){
            cur_chan_numb = 0;
            calc_rest = calc_rest + PULSE_LENGTH;
            OCR1A = (FRAME_LENGTH - calc_rest) * 2;
            calc_rest = 0;
        }
        else{
            OCR1A = (ppm[cur_chan_numb] - PULSE_LENGTH) * 2;
            calc_rest = calc_rest + ppm[cur_chan_numb];
            cur_chan_numb++;
        }
    }
}



int findMode(byte b) {
    for (int i = 0; i < MODES_COUNT; i++) {
        if (modes[i] == b) {
            return i;
        }
    }
    return -1;
}

void loop() {
    if (MODE_BUTTONS_PRESENT) {
        digitalWrite(buttonsLatchPin, LOW);
        digitalWrite(buttonsLatchPin, HIGH);
        digitalWrite(buttonsClockPin, LOW);
        byte readButtonsByte = shiftIn168(buttonsDataPin, buttonsClockPin, MSBFIRST);

        int modeIndex;
        if (readButtonsByte != currentButtonsByte && readButtonsByte != 0x00 && (modeIndex = findMode(readButtonsByte)) != -1) {
            digitalWrite(ledLatchPin, LOW);
            shiftOut(ledDataPin, ledClockPin, MSBFIRST, readButtonsByte);
            digitalWrite(ledLatchPin, HIGH);
            currentButtonsByte = readButtonsByte;

            ppm[4] = modesPPM[modeIndex];

            Serial.print("Button changed: ");
            Serial.print(currentButtonsByte, HEX);
            Serial.print(", ppm mode: ");
            Serial.println(ppm[4]);
        }
    }

    Usb.Task();
}