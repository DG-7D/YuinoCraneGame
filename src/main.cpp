#include <Arduino.h>
#include <ESP32Servo.h>
#include <main.h>

// ピン設定
//     ボタン入力
const uint8_t PIN_BUTTON_X = 19;
const uint8_t PIN_BUTTON_Y = 18;
const uint8_t PIN_ACTIVATE = 32;
//     リミットスイッチ入力
const uint8_t PIN_LIMIT_SWITCH_X_START = 33;
const uint8_t PIN_LIMIT_SWITCH_X_END = 25;
const uint8_t PIN_LIMIT_SWITCH_Y_START = 26;
const uint8_t PIN_LIMIT_SWITCH_Y_END = 27;
//     サーボ出力
const uint8_t PIN_SERVO_X = 23;
const uint8_t PIN_SERVO_Y = 22;
const uint8_t PIN_SERVO_Z = 1;
const uint8_t PIN_SERVO_ARM = 3;
//     信号出力
const uint8_t PIN_CONTROL_ENABLED = 21;
// サーボ設定
const uint16_t MICROS_SERVO_XY_STOP = 1500;
const uint16_t MICROS_SERVO_XY_MOVE = 100;
const uint16_t MICROS_SERVO_Z_STOP = 1500;
const uint16_t MICROS_SERVO_Z_MOVE = 100;
const uint8_t DEGREE_SERVO_ARM_CLOSE_MIN = 0;
const uint8_t DEGREE_SERVO_ARM_CLOSE_MAX = 10;
const uint8_t DEGREE_SERVO_ARM_OPEN = 45;
// 時間設定
const uint16_t MILLIS_POLLING_INTERVAL = 1000 / 60;
const uint16_t MILLIS_TIMEOUT = 10000;
const uint16_t MILLIS_Z_MOVE = 2000;
const uint16_t MILLIS_ARM_MOVE = 1000;

// 定数計算
const uint16_t MICROS_SERVO_XY_BACK = MICROS_SERVO_XY_STOP - MICROS_SERVO_XY_MOVE;
const uint16_t MICROS_SERVO_XY_FORWARD = MICROS_SERVO_XY_STOP + MICROS_SERVO_XY_MOVE;
const uint16_t MICROS_SERVO_Z_UP = MICROS_SERVO_Z_STOP - MICROS_SERVO_Z_MOVE;
const uint16_t MICROS_SERVO_Z_DOWN = MICROS_SERVO_Z_STOP + MICROS_SERVO_Z_MOVE;

uint16_t positionX = 0;
uint16_t positionY = 0;

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
    catchObject();
    goHome();
    releaseObject();
}

void waitForActivate() {
    Serial.println("waitForActivate");
    while (digitalRead(PIN_ACTIVATE) == HIGH) {
        delay(MILLIS_POLLING_INTERVAL);
    }
}

void waitForButton() {
    Serial.println("waitForButton");
    while (!digitalRead(PIN_BUTTON_X) && !digitalRead(PIN_BUTTON_Y)) {
        delay(MILLIS_POLLING_INTERVAL);
    }
}

void controlXY() {
    Serial.println("controlXY");
    digitalWrite(PIN_CONTROL_ENABLED, HIGH);
    const unsigned long startMillis = millis();
    while (millis() - startMillis < MILLIS_TIMEOUT) {
        if (digitalRead(PIN_BUTTON_X) == LOW && digitalRead(PIN_LIMIT_SWITCH_X_END) == HIGH) {
            servoX.writeMicroseconds(MICROS_SERVO_XY_FORWARD);
            positionX += 1;
        } else {
            servoX.writeMicroseconds(MICROS_SERVO_XY_STOP);
        }

        if (digitalRead(PIN_BUTTON_Y) == LOW && digitalRead(PIN_LIMIT_SWITCH_Y_END) == HIGH) {
            servoY.writeMicroseconds(MICROS_SERVO_XY_FORWARD);
            positionY += 1;
        } else {
            servoY.writeMicroseconds(MICROS_SERVO_XY_BACK);
        }
        delay(MILLIS_POLLING_INTERVAL);
    }
    digitalWrite(PIN_CONTROL_ENABLED, LOW);
}

void catchObject() {
    Serial.println("catchObject");
    servoZ.writeMicroseconds(MICROS_SERVO_Z_DOWN);
    delay(MILLIS_Z_MOVE);
    servoZ.writeMicroseconds(MICROS_SERVO_Z_STOP);

    uint8_t DEGREE_SERVO_ARM_CLOSE = random(DEGREE_SERVO_ARM_CLOSE_MIN, DEGREE_SERVO_ARM_CLOSE_MAX);
    servoArm.write(DEGREE_SERVO_ARM_CLOSE);
    delay(MILLIS_ARM_MOVE);

    servoZ.writeMicroseconds(MICROS_SERVO_Z_UP);
    delay(MILLIS_Z_MOVE);
    servoZ.writeMicroseconds(MICROS_SERVO_Z_STOP);
}

void goHome() {
    Serial.println("goHome");
    if (positionX > positionY) {
        servoX.writeMicroseconds(MICROS_SERVO_XY_STOP - MICROS_SERVO_XY_MOVE);
        servoY.writeMicroseconds(MICROS_SERVO_XY_STOP - MICROS_SERVO_XY_MOVE * positionY / positionX);
    } else {
        servoX.writeMicroseconds(MICROS_SERVO_XY_STOP - MICROS_SERVO_XY_MOVE * positionX / positionY);
        servoY.writeMicroseconds(MICROS_SERVO_XY_STOP - MICROS_SERVO_XY_MOVE);
    }
    while (positionX != 0 && positionY != 0) {
        if (positionX != 0 && digitalRead(PIN_LIMIT_SWITCH_X_START) == LOW) {
            servoX.writeMicroseconds(MICROS_SERVO_XY_STOP);
            positionX = 0;
        }
        if (positionY != 0 && digitalRead(PIN_LIMIT_SWITCH_Y_START) == LOW) {
            servoY.writeMicroseconds(MICROS_SERVO_XY_STOP);
            positionY = 0;
        }
        delay(MILLIS_POLLING_INTERVAL);
    }
}

void releaseObject() {
    Serial.println("releaseObject");
    servoArm.write(DEGREE_SERVO_ARM_OPEN);
}