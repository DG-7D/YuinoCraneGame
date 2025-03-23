#include <Arduino.h>

const uint8_t motorCount = 4;
const uint8_t outputCount = 2 * motorCount;

#ifdef __AVR_ATtinyX4__
const uint8_t pinArray[outputCount] = {PIN_PB2, PIN_PA4,
                                       PIN_PA7, PIN_PA5,
                                       PIN_PB0, PIN_PA0,
                                       PIN_PB1, PIN_PA3};
                                       
#elif __AVR_ATmega328P__
const uint8_t pinArray[outputCount] = {2, 3,
                                       4, 5,
                                       6, 7,
                                       8, 9};
#endif

const uint16_t pwmWidthMicros = 20000;
const uint16_t pulseWidthMicros = 1000;

byte motorStateFlag = 0b00000000;

void setup() {
    Serial.begin(9600);
    for (uint8_t i = 0; i < outputCount; i++) {
        pinMode(pinArray[i], OUTPUT);
    }
    while(!Serial);
}

void loop() {
    if (Serial.available() > 0) {
        motorStateFlag = Serial.read();
        while(Serial.available() > 0) {
            Serial.read();
        }
    }
    for (uint8_t i = 0; i < outputCount; i++) {
        digitalWrite(
            pinArray[i],
            bitRead(motorStateFlag, i) && (micros() % pwmWidthMicros < pulseWidthMicros)
                ? HIGH
                : LOW);
    }
}