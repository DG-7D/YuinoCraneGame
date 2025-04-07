#include <Arduino.h>

constexpr uint8_t motorCount = 4;
constexpr uint8_t outputCount = 2 * motorCount;

constexpr uint8_t portAvailable[2] = {0b11111001, 0b00000111}; // PA2(RXD), PA1(TXD), PB3(RESET)には触れない
constexpr byte portMappingArray[outputCount][2] = {
    {0, 1 << 2}, // PB2
    {1 << 4, 0}, // PA4
    {1 << 7, 0}, // PA7
    {1 << 5, 0}, // PA5
    {0, 1 << 0}, // PB0
    {1 << 0, 0}, // PA0
    {0, 1 << 1}, // PB1
    {1 << 3, 0}  // PA3
};

constexpr uint8_t pulseWidthArray[outputCount] = {31, 31,
                                                  63, 63,
                                                  127, 127,
                                                  255, 255}; // TCNT0(0-255)と比較 2^n - 1のほうが速い

uint8_t portEnabled[2] = {0, 0};

void setup() {
    Serial.begin(9600);
    DDRA |= portAvailable[0];
    DDRB |= portAvailable[1];
}

void loop() {
    while (Serial.available() > 0) {
        uint8_t motorStateFlag = Serial.read();
        portEnabled[0] = 0;
        portEnabled[1] = 0;
        for (uint8_t i = 0; i < outputCount; i++) {
            if (bitRead(motorStateFlag, i)) {
                portEnabled[0] |= portMappingArray[i][0];
                portEnabled[1] |= portMappingArray[i][1];
            }
        }
        Serial.write(motorStateFlag);
    }

    uint8_t portPWM[2] = {0, 0};
    for (uint8_t i = 0; i < outputCount; i++) {
        if (TCNT0 <= pulseWidthArray[i]) {
            portPWM[0] |= portMappingArray[i][0];
            portPWM[1] |= portMappingArray[i][1];
        }
    }

    PORTA = (PORTA & ~portAvailable[0]) | (portEnabled[0] & portPWM[0]);
    PORTB = (PORTB & ~portAvailable[1]) | (portEnabled[1] & portPWM[1]);
}