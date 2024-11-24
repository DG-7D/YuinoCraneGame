#include <Arduino.h>
#include <ESP32Servo.h>
#include <main.h>

// ピン設定
//     ボタン入力
const uint8_t PIN_BUTTON_X = 27;
const uint8_t PIN_BUTTON_Y = 14;
const uint8_t PIN_ACTIVATE = 12;
//     リミットスイッチ入力
const uint8_t PIN_LIMIT_SWITCH_X_START = 32;
const uint8_t PIN_LIMIT_SWITCH_X_END = 33;
const uint8_t PIN_LIMIT_SWITCH_Y_START = 25;
const uint8_t PIN_LIMIT_SWITCH_Y_END = 26;
const uint8_t PIN_LIMIT_SWITCH_Z_TOP = 13;
//     サーボ出力
const uint8_t PIN_SERVO_X = 19;
const uint8_t PIN_SERVO_Y = 18;
const uint8_t PIN_SERVO_Z = 5;
const uint8_t PIN_SERVO_ARM = 17;
//     信号出力
const uint8_t PIN_CONTROL_ENABLED = 4;
// サーボ設定
const uint16_t MICROS_SERVO_STOP = 1500;
const int16_t MICROS_SERVO_X_MOVE = 100;
const int16_t MICROS_SERVO_Y_MOVE = -100;
const uint16_t MICROS_SERVO_Z_MOVE = 100;
const uint8_t DEGREE_SERVO_ARM_CLOSE_MIN = 0;
const uint8_t DEGREE_SERVO_ARM_CLOSE_MAX = 5;
const uint8_t DEGREE_SERVO_ARM_OPEN = 45;
// 時間設定
const uint16_t MILLIS_TIMEOUT = 10000;
const uint16_t MILLIS_Z_DOWN = 2500;
const uint16_t MILLIS_Z_UP = 3300;
const uint16_t MILLIS_ARM_MOVE_INTERVAL = 500;

// 定数計算
const uint16_t MICROS_SERVO_X_FORWARD = MICROS_SERVO_STOP + MICROS_SERVO_X_MOVE;
const uint16_t MICROS_SERVO_X_BACKWARD = MICROS_SERVO_STOP - MICROS_SERVO_X_MOVE;
const uint16_t MICROS_SERVO_Y_FORWARD = MICROS_SERVO_STOP + MICROS_SERVO_Y_MOVE;
const uint16_t MICROS_SERVO_Y_BACKWARD = MICROS_SERVO_STOP - MICROS_SERVO_Y_MOVE;
const uint16_t MICROS_SERVO_Z_UP = MICROS_SERVO_STOP - MICROS_SERVO_Z_MOVE;
const uint16_t MICROS_SERVO_Z_DOWN = MICROS_SERVO_STOP + MICROS_SERVO_Z_MOVE;

Servo servoX;
Servo servoY;
Servo servoZ;
Servo servoArm;

void setup() {
    Serial.begin(9600);
    Serial.println("setup");

    pinMode(PIN_BUTTON_X, INPUT_PULLUP);
    pinMode(PIN_BUTTON_Y, INPUT_PULLUP);
    pinMode(PIN_ACTIVATE, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_X_START, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_X_END, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_Y_START, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_Y_END, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_Z_TOP, INPUT_PULLUP);

    servoX.attach(PIN_SERVO_X);
    servoY.attach(PIN_SERVO_Y);
    servoZ.attach(PIN_SERVO_Z);
    servoArm.attach(PIN_SERVO_ARM);
    pinMode(PIN_CONTROL_ENABLED, OUTPUT);

    goHome();
    releaseObject();
}

void loop() {
    Serial.println("loop");
    waitForActivate();
    waitForButton();
    controlXY();
    delay(500);
    catchObject(1);
    delay(1000);
    goHome();
    delay(1000);
    releaseObject();
}

void waitForActivate() {
    Serial.println("waitForActivate");
    while (digitalRead(PIN_ACTIVATE) != LOW) {
    }
}

void waitForButton() {
    Serial.println("waitForButton");
    while (digitalRead(PIN_BUTTON_X) != LOW && digitalRead(PIN_BUTTON_Y) != LOW) {
    }
}

void controlXY() {
    Serial.println("controlXY");
    digitalWrite(PIN_CONTROL_ENABLED, HIGH);
    const unsigned long startMillis = millis();
    while (millis() - startMillis < MILLIS_TIMEOUT) {
        if (digitalRead(PIN_BUTTON_X) == LOW && digitalRead(PIN_LIMIT_SWITCH_X_END) != LOW) {
            servoX.writeMicroseconds(MICROS_SERVO_X_FORWARD);
        } else {
            servoX.writeMicroseconds(MICROS_SERVO_STOP);
        }

        if (digitalRead(PIN_BUTTON_Y) == LOW && digitalRead(PIN_LIMIT_SWITCH_Y_END) != LOW) {
            servoY.writeMicroseconds(MICROS_SERVO_Y_FORWARD);
        } else {
            servoY.writeMicroseconds(MICROS_SERVO_STOP);
        }
    }
    servoX.writeMicroseconds(MICROS_SERVO_STOP);
    servoY.writeMicroseconds(MICROS_SERVO_STOP);
    digitalWrite(PIN_CONTROL_ENABLED, LOW);
}

void catchObject(uint8_t mode) {
    Serial.println("catchObject");
    servoZ.writeMicroseconds(MICROS_SERVO_Z_DOWN);
    delay(MILLIS_Z_DOWN);
    servoZ.writeMicroseconds(MICROS_SERVO_STOP);

    uint8_t DEGREE_SERVO_ARM_CLOSE = random(DEGREE_SERVO_ARM_CLOSE_MIN, DEGREE_SERVO_ARM_CLOSE_MAX);
    delay(MILLIS_ARM_MOVE_INTERVAL);
    servoArm.write(DEGREE_SERVO_ARM_OPEN / 2);
    delay(MILLIS_ARM_MOVE_INTERVAL);
    servoArm.write(DEGREE_SERVO_ARM_CLOSE);
    delay(MILLIS_ARM_MOVE_INTERVAL);

    servoZ.writeMicroseconds(MICROS_SERVO_Z_UP);
    if (mode == 0) {
        delay(MILLIS_Z_UP);
    } else if (mode == 1) {
        while (digitalRead(PIN_LIMIT_SWITCH_Z_TOP) != LOW) {
        }
    }
    servoZ.writeMicroseconds(MICROS_SERVO_STOP);
}

void goHome() {
    Serial.println("goHome");
    servoX.writeMicroseconds(MICROS_SERVO_X_BACKWARD);
    servoY.writeMicroseconds(MICROS_SERVO_Y_BACKWARD);
    bool isXHome = false;
    bool isYHome = false;
    while (!(isXHome && isYHome)) {
        if (!isXHome && digitalRead(PIN_LIMIT_SWITCH_X_START) == LOW) {
            servoX.writeMicroseconds(MICROS_SERVO_STOP);
            isXHome = true;
        }
        if (!isYHome && digitalRead(PIN_LIMIT_SWITCH_Y_START) == LOW) {
            servoY.writeMicroseconds(MICROS_SERVO_STOP);
            isYHome = true;
        }
    }
}

void releaseObject() {
    Serial.println("releaseObject");
    servoArm.write(DEGREE_SERVO_ARM_OPEN);
}