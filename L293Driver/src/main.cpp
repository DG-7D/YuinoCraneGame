#include <Arduino.h>

constexpr uint8_t motorCount = 4;
constexpr uint8_t outputCount = 2 * motorCount;

#ifdef __AVR_ATtinyX4__
constexpr uint8_t pinArray[outputCount] = {PIN_PB2, PIN_PA4,
                                           PIN_PA7, PIN_PA5,
                                           PIN_PB0, PIN_PA0,
                                           PIN_PB1, PIN_PA3};

#elif __AVR_ATmega328P__
constexpr uint8_t pinArray[outputCount] = {2, 3,
                                           4, 5,
                                           6, 7,
                                           8, 9};
#endif

constexpr uint8_t pulseWidthArray[outputCount] = {31, 31,
                                                  63, 63,
                                                  127, 127,
                                                  255, 255}; // TCNT0(0-255)と比較 2^n - 1のほうが速い

byte motorStateFlag = 0b00000000;

void setup() {
    Serial.begin(9600);
    for (uint8_t i = 0; i < outputCount; i++) {
        pinMode(pinArray[i], OUTPUT);
    }
}

void loop() {
    while (Serial.available() > 0) {
        motorStateFlag = Serial.read();
        Serial.write(motorStateFlag);
    }

    for (uint8_t i = 0; i < outputCount; i++) {
        digitalWrite(
            pinArray[i],
            bitRead(motorStateFlag, i) && (TCNT0 <= pulseWidthArray[i]) ? HIGH : LOW);
    }
}