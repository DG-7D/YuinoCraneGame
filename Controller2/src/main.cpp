#include <Arduino.h>

// HardwareSerial Serial1(2);

// ピン設定
//     スティック入力
const uint8_t PIN_STICK_X = 26;
const uint8_t PIN_STICK_Y = 25;    
//     ボタン入力
const uint8_t PIN_STICK_BUTTON = 27;
//     リミットスイッチ入力
// const uint8_t PIN_LIMIT_SWITCH_X_START = 32;
// const uint8_t PIN_LIMIT_SWITCH_Y_START = 25;
// const uint8_t PIN_LIMIT_SWITCH_Z_TOP = 13;
//     サーボ出力
// const uint8_t PIN_SERVO_ARM = 17;
// //     信号出力
// const uint8_t PIN_CONTROL_ENABLED = 4;
// // サーボ設定
// const uint8_t DEGREE_SERVO_ARM_CLOSE_MIN = 0;
// const uint8_t DEGREE_SERVO_ARM_CLOSE_MAX = 5;
// const uint8_t DEGREE_SERVO_ARM_OPEN = 45;

// 閾値設定 スティックの中心位置128
const uint8_t STICK_X_THRESHOLD = 20;
const uint8_t STICK_Y_THRESHOLD = 20;

byte state = 0b00000000;

void sendSignal(int8_t x_input, int8_t y_input, bool button);

void setup() {
    Serial.begin(9600);
    Serial.println("setup");
    Serial2.begin(9600);
    Serial2.write(state);

    // servoArm.attach(PIN_SERVO_ARM);

    pinMode(PIN_STICK_X, INPUT);
    pinMode(PIN_STICK_Y, INPUT);
    pinMode(PIN_STICK_BUTTON, INPUT_PULLUP);
    analogSetAttenuation(ADC_11db);  // 約3.3Vまでの入力に対応
    // pinMode(PIN_LIMIT_SWITCH_X_START, INPUT_PULLUP);
    // pinMode(PIN_LIMIT_SWITCH_Y_START, INPUT_PULLUP);
    // pinMode(PIN_LIMIT_SWITCH_Z_TOP, INPUT_PULLUP);

    // upArm();
    // goHome();
    // releaseObject();
}

void loop() {
    // Serial.println("loop");
    int8_t xValue = -1 * (analogRead(PIN_STICK_X)/16 - INT8_MAX);
    int8_t yValue = (analogRead(PIN_STICK_Y)/16 - INT8_MAX);
    bool buttonValue = digitalRead(PIN_STICK_BUTTON);
    // Serial.print("X: ");Serial.print(xValue);
    // Serial.print(" Y: ");Serial.print(yValue);  
    // Serial.print(" Button: ");Serial.println(buttonValue);
    sendSignal(xValue, yValue, buttonValue);
    delay(50);
}

void printState(byte b) {
    Serial.print("    1 2 3 4 5 6 7 8\n    ");
    for (int i = 0; i < 8; i++) {
        Serial.print(bitRead(b, i) ? "H " : "L ");
    }
    Serial.println();
}

void sendSignal(int8_t x_input, int8_t y_input, bool button) {

    byte state_now = 0b00000000; // 初期化
    if (abs(x_input) > STICK_X_THRESHOLD) {
        x_input = (x_input > 0) ? 1 : -1;
    } else {
        x_input = 0;
    }
    if (abs(y_input) > STICK_Y_THRESHOLD) {
        y_input = (y_input > 0) ? 1 : -1;
    } else {
        y_input = 0;
    }

    switch(x_input) {
        case 1:
            // Serial.println("X: RIGHT");
            bitSet(state_now, 0);
            break;
        case -1:
            // Serial.println("X: LEFT");
            bitSet(state_now, 1);
            break;
        default:
            break;
            // Serial.println("X: STOP");
    }
    switch(y_input) {
        case 1:
            // Serial.println("Y: UP");
            bitSet(state_now, 2);
            break;
        case -1:
            // Serial.println("Y: DOWN");
            bitSet(state_now, 3);
            break;
        default:
            break;
            // Serial.println("Y: STOP");
    }
    if (state != state_now) {
        printState(state_now);
        Serial2.write(state_now);
        state = state_now;
    }
    
}

// void catchObject() {
//     Serial.println("catchObject");
//     servoZ.writeMicroseconds(MICROS_SERVO_Z_DOWN);
//     delay(MILLIS_Z_DOWN);
//     servoZ.writeMicroseconds(MICROS_SERVO_STOP);

//     uint8_t DEGREE_SERVO_ARM_CLOSE = random(DEGREE_SERVO_ARM_CLOSE_MIN, DEGREE_SERVO_ARM_CLOSE_MAX);
//     delay(MILLIS_ARM_MOVE_INTERVAL);
//     servoArm.write(DEGREE_SERVO_ARM_OPEN / 2);
//     delay(MILLIS_ARM_MOVE_INTERVAL);
//     servoArm.write(DEGREE_SERVO_ARM_CLOSE);
//     delay(MILLIS_ARM_MOVE_INTERVAL);

//     upArm();
// }

// void upArm() {
//     Serial.println("upArm");
//     servoZ.writeMicroseconds(MICROS_SERVO_Z_UP);

//     unsigned long startMillis = millis();
//     while (digitalRead(PIN_LIMIT_SWITCH_Z_TOP) != LOW && millis() - startMillis < MILLIS_Z_UP) {
//     }
//     servoZ.writeMicroseconds(MICROS_SERVO_STOP);
// }

// void goHome() {
//     Serial.println("goHome");
//     servoX.writeMicroseconds(MICROS_SERVO_X_BACKWARD);
//     servoY.writeMicroseconds(MICROS_SERVO_Y_BACKWARD);
//     bool isXHome = false;
//     bool isYHome = false;
//     while (!(isXHome && isYHome)) {
//         if (!isXHome && digitalRead(PIN_LIMIT_SWITCH_X_START) == LOW) {
//             servoX.writeMicroseconds(MICROS_SERVO_STOP);
//             isXHome = true;
//         }
//         if (!isYHome && digitalRead(PIN_LIMIT_SWITCH_Y_START) == LOW) {
//             servoY.writeMicroseconds(MICROS_SERVO_STOP);
//             isYHome = true;
//         }
//     }
// }

// void releaseObject() {
//     Serial.println("releaseObject");
//     servoArm.write(DEGREE_SERVO_ARM_OPEN);
// }