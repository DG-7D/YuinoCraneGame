#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial1(12, 11);
byte state = 0b00000000;

void printState(byte b) {
    Serial.print("    1 2 3 4 5 6 7 8\n    ");
    for (int i = 0; i < 8; i++) {
        Serial.print(bitRead(b, i) ? "H " : "L ");
    }
    Serial.println();
}

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial.println("Ready");
    Serial1.write(state);
}

void loop() {
    while (Serial.available()) {
        char c = Serial.read();
        if ('1' <= c && c <= '8') {
            uint8_t bit = c - '1';
            bitToggle(state, bit);
        } else if (state) {
            state = 0b00000000;
        } else {
            break;
        }
        Serial.println("Send: ");
        printState(state);
        Serial1.write(state);
    }
    while (Serial1.available()) {
        Serial.println("Receive: ");
        printState(Serial1.read());
    }
}