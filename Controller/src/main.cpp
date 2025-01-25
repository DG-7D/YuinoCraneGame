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
const uint8_t DEGREE_SERVO_ARM_CLOSE_MIN = 0;
const uint8_t DEGREE_SERVO_ARM_CLOSE_MAX = 5;
const uint8_t DEGREE_SERVO_ARM_OPEN = 45;

// 時間設定
const unsigned long COMMAND_LIFETIME = 500;

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

unsigned long lastCommandTime = 0;
bool isCommandActive = false;

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

    execCommand("0,0,0,0");
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

    if (values[0] == -1 && digitalRead(PIN_LIMIT_SWITCH_X_START) != LOW) {
        servoX.writeMicroseconds(MICROS_SERVO_X_BACKWARD);
        // Serial.print("X-, ");
    } else if (values[0] == 1 && digitalRead(PIN_LIMIT_SWITCH_X_END) != LOW) {
        servoX.writeMicroseconds(MICROS_SERVO_X_FORWARD);
        // Serial.print("X+, ");
    } else {
        servoX.writeMicroseconds(MICROS_SERVO_STOP);
        // Serial.print("X0, ");
    }

    if (values[1] == -1 && digitalRead(PIN_LIMIT_SWITCH_Y_START) != LOW) {
        servoY.writeMicroseconds(MICROS_SERVO_Y_BACKWARD);
        // Serial.print("Y-, ");
    } else if (values[1] == 1 && digitalRead(PIN_LIMIT_SWITCH_Y_END) != LOW) {
        servoY.writeMicroseconds(MICROS_SERVO_Y_FORWARD);
        // Serial.print("Y+, ");
    } else {
        servoY.writeMicroseconds(MICROS_SERVO_STOP);
        // Serial.print("Y0, ");
    }

    if (values[2] == -1) {
        servoZ.writeMicroseconds(MICROS_SERVO_Z_DOWN);
        // Serial.print("Z-, ");
    } else if (values[2] == 1 && digitalRead(PIN_LIMIT_SWITCH_Z_TOP) != LOW) {
        servoZ.writeMicroseconds(MICROS_SERVO_Z_UP);
        // Serial.print("Z+, ");
    } else {
        servoZ.writeMicroseconds(MICROS_SERVO_STOP);
        // Serial.print("Z0, ");
    }

    if (values[3] == 1) {
        servoArm.write(DEGREE_SERVO_ARM_CLOSE_MIN);
        // Serial.print("AO, ");
    } else {
        servoArm.write(DEGREE_SERVO_ARM_OPEN);
        // Serial.print("AC, ");
    }

    // Serial.println();
    while (Serial.available()) Serial.read();
}

void loop() {
    String data = Serial.readString();
    if (data) {
        lastCommandTime = millis();
        isCommandActive = true;
        execCommand(data);
    } else if (isCommandActive && millis() - lastCommandTime > COMMAND_LIFETIME) {
        // Serial.println("Timeout");
        isCommandActive = false;
        execCommand("0,0,0,0");
    }
}