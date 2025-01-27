#include <Arduino.h>
#include <ESP32Servo.h>
#include <main.h>

// ピン設定
//     リミットスイッチ入力
const uint8_t PIN_LIMIT_SWITCH_X_START = 32;
const uint8_t PIN_LIMIT_SWITCH_X_END = 33;
const uint8_t PIN_LIMIT_SWITCH_Y_START = 25;
const uint8_t PIN_LIMIT_SWITCH_Y_END = 26;
const uint8_t PIN_LIMIT_SWITCH_Z_TOP = 13;
//     サーボ出力
const uint8_t PIN_SERVO_X = 16;
const uint8_t PIN_SERVO_Y = 18;
const uint8_t PIN_SERVO_Z = 5;
const uint8_t PIN_SERVO_ARM = 17;

// サーボ設定
const uint16_t MICROS_SERVO_STOP = 1500;
const int16_t MICROS_SERVO_X_MOVE = 100;
const int16_t MICROS_SERVO_Y_MOVE = -100;
const uint16_t MICROS_SERVO_Z_MOVE = 100;
const uint8_t DEGREE_SERVO_ARM_CLOSE = 0;
const uint8_t DEGREE_SERVO_ARM_OPEN = 45;

// 定数計算
const uint16_t MICROS_SERVO_X_FORWARD = MICROS_SERVO_STOP + MICROS_SERVO_X_MOVE;
const uint16_t MICROS_SERVO_X_BACKWARD = MICROS_SERVO_STOP - MICROS_SERVO_X_MOVE;
const uint16_t MICROS_SERVO_Y_FORWARD = MICROS_SERVO_STOP + MICROS_SERVO_Y_MOVE;
const uint16_t MICROS_SERVO_Y_BACKWARD = MICROS_SERVO_STOP - MICROS_SERVO_Y_MOVE;
const uint16_t MICROS_SERVO_Z_UP = MICROS_SERVO_STOP - MICROS_SERVO_Z_MOVE;
const uint16_t MICROS_SERVO_Z_DOWN = MICROS_SERVO_STOP + MICROS_SERVO_Z_MOVE;

Servo servoX;
uint16_t servoXMicroseconds = MICROS_SERVO_STOP;
Servo servoY;
uint16_t servoYMicroseconds = MICROS_SERVO_STOP;
Servo servoZ;
uint16_t servoZMicroseconds = MICROS_SERVO_STOP;
Servo servoArm;
uint8_t servoArmDegree = DEGREE_SERVO_ARM_OPEN;

void setup() {
    Serial.begin(9600);
    Serial.println("setup");

    pinMode(PIN_LIMIT_SWITCH_X_START, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_X_END, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_Y_START, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_Y_END, INPUT_PULLUP);
    pinMode(PIN_LIMIT_SWITCH_Z_TOP, INPUT_PULLUP);

    servoX.attach(PIN_SERVO_X);
    servoY.attach(PIN_SERVO_Y);
    servoZ.attach(PIN_SERVO_Z);
    servoArm.attach(PIN_SERVO_ARM);

    Serial.print("> ");
}

String command = "";

void loop() {
    if (Serial.available()) {
        char input = Serial.read();
        if (input == '\r') {
            return;
        }

        Serial.println();

        if (input == '\n') {
            execCommand(command);
            command = "";
        } else if (input == '\b') {
            command = command.substring(0, command.length() - 1);
        } else {
            command += input;
        }

        Serial.print("> ");
        Serial.print(command);
    }

    updateServo();
}

void execCommand(String command) {
    int values[4] = {0, 0, 0, 0};
    int index = 0;
    int startPos = 0;

    for (int i = 0; i < command.length(); i++) {
        if (command.charAt(i) == ',') {
            values[index] = command.substring(startPos, i).toInt();
            index++;
            startPos = i + 1;
        }
    }
    values[index] = command.substring(startPos).toInt();

    if (values[0] == -1) {
        servoXMicroseconds = MICROS_SERVO_X_BACKWARD;
        Serial.print("X-, ");
    } else if (values[0] == 1) {
        servoXMicroseconds = MICROS_SERVO_X_FORWARD;
        Serial.print("X+, ");
    } else {
        servoXMicroseconds = MICROS_SERVO_STOP;
        Serial.print("X0, ");
    }

    if (values[1] == -1) {
        servoYMicroseconds = MICROS_SERVO_Y_BACKWARD;
        Serial.print("Y-, ");
    } else if (values[1] == 1) {
        servoYMicroseconds = MICROS_SERVO_Y_FORWARD;
        Serial.print("Y+, ");
    } else {
        servoYMicroseconds = MICROS_SERVO_STOP;
        Serial.print("Y0, ");
    }

    if (values[2] == -1) {
        servoZMicroseconds = MICROS_SERVO_Z_DOWN;
        Serial.print("Z-, ");
    } else if (values[2] == 1) {
        servoZMicroseconds = MICROS_SERVO_Z_UP;
        Serial.print("Z+, ");
    } else {
        servoZMicroseconds = MICROS_SERVO_STOP;
        Serial.print("Z0, ");
    }

    if (values[3] == 1) {
        servoArmDegree = DEGREE_SERVO_ARM_OPEN;
        Serial.print("AO, ");
    } else {
        servoArmDegree = DEGREE_SERVO_ARM_CLOSE;
        Serial.print("AC, ");
    }

    Serial.println();
}

void updateServo() {
    if (digitalRead(PIN_LIMIT_SWITCH_X_START) == LOW) {
        servoX.writeMicroseconds(MICROS_SERVO_X_FORWARD);
        servoXMicroseconds = MICROS_SERVO_STOP;
    } else if (digitalRead(PIN_LIMIT_SWITCH_X_END) == LOW) {
        servoX.writeMicroseconds(MICROS_SERVO_X_BACKWARD);
        servoXMicroseconds = MICROS_SERVO_STOP;
    } else {
        servoX.writeMicroseconds(servoXMicroseconds);
    }

    if (digitalRead(PIN_LIMIT_SWITCH_Y_START) == LOW) {
        servoY.writeMicroseconds(MICROS_SERVO_Y_FORWARD);
        servoYMicroseconds = MICROS_SERVO_STOP;
    } else if (digitalRead(PIN_LIMIT_SWITCH_Y_END) == LOW) {
        servoY.writeMicroseconds(MICROS_SERVO_Y_BACKWARD);
        servoYMicroseconds = MICROS_SERVO_STOP;
    } else {
        servoY.writeMicroseconds(servoYMicroseconds);
    }

    if (digitalRead(PIN_LIMIT_SWITCH_Z_TOP) == LOW) {
        servoZ.writeMicroseconds(MICROS_SERVO_Z_DOWN);
        servoZMicroseconds = MICROS_SERVO_STOP;
    } else {
        servoZ.writeMicroseconds(servoZMicroseconds);
    }

    servoArm.write(servoArmDegree);
}