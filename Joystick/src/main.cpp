#include <Arduino.h>
#include <ESP32Servo.h>
#include <main.h>
// HardwareSerial Serial1(2);

// ピン設定
//     スティック入力
const uint8_t PIN_STICK_X = 26;
const uint8_t PIN_STICK_Y = 27;    
const uint8_t PIN_STICK_BUTTON = 25;

//     ボタン入力
const uint8_t PIN_BUTTON = 14;
const uint8_t PIN_BUTTON_LED = 12;

//     サーボ出力
const uint8_t PIN_SERVO_Z = 18;
const uint8_t PIN_SERVO_ARM = 19;
//     信号出力
// const uint8_t PIN_CONTROL_ENABLED = 4;

// サーボ設定
const uint8_t DEGREE_SERVO_ARM_CLOSE_MIN = 0;
const uint8_t DEGREE_SERVO_ARM_CLOSE_MAX = 5;
const uint8_t DEGREE_SERVO_ARM_OPEN = 45;

const uint8_t DEGREE_SERVO_Z_UP = 180;
const uint8_t DEGREE_SERVO_Z_DOWN = 20;

Servo servoZ;
Servo servoArm;

// 時間設定
const uint16_t MILLIS_TIMEOUT = 30 * 1000;
const uint16_t MILLIS_Z_DOWN_TIME = 2000;
const uint16_t MILLIS_Z_UP_INTERVAL = 1000;
const uint16_t MILLIS_ARM_MOVE_INTERVAL = 500;
const uint16_t MILLIS_HOMING_INTERVAL = 3000;

// 閾値設定 スティックの中心位置128
const uint8_t STICK_X_THRESHOLD = 20;
const uint8_t STICK_Y_THRESHOLD = 20;

byte state = 0b00000000;

bool active = false;
bool is_control_enabled = false;
volatile bool is_button_pressed = false;

void buttonPressed() {
    is_button_pressed = true;
    // Serial.println("Button Pressed");
}

void setup() {
    Serial.begin(9600);
    Serial.println("setup");
    Serial2.begin(9600);
    Serial2.write(state);

    servoZ.attach(PIN_SERVO_Z);
    servoArm.attach(PIN_SERVO_ARM);

    pinMode(PIN_STICK_X, INPUT);
    pinMode(PIN_STICK_Y, INPUT);
    pinMode(PIN_STICK_BUTTON, INPUT_PULLUP);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_BUTTON_LED, OUTPUT);
    analogSetAttenuation(ADC_11db);  // 約3.3Vまでの入力に対応

    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonPressed, RISING);

    upArm();
    goHome();
    releaseObject();
}

void loop() {
    Serial.println("loop");

    waitForControl();
    control();
    delay(500);
    downArm();
    delay(500);
    catchObject();
    upArm();
    delay(500);
    goHome();
    delay(1000);
    releaseObject();
    delay(1000);

    Serial.println("loop done");
}

void printState(byte b) {
    Serial.print("    1 2 3 4 5 6 7 8\n    ");
    for (int i = 0; i < 8; i++) {
        Serial.print(bitRead(b, i) ? "H " : "L ");
    }
    Serial.println();
}

void sendSignal(int8_t x_input, int8_t y_input) {

    byte state_now = 0b00000000; // 初期化

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

void readController(int8_t &x_input, int8_t &y_input) {
    x_input = -1 * (analogRead(PIN_STICK_X)/16 - INT8_MAX);
    if (abs(x_input) > STICK_X_THRESHOLD) {
        x_input = (x_input > 0) ? 1 : -1;
    } else {
        x_input = 0;
    }

    y_input = -1 * (analogRead(PIN_STICK_Y)/16 - INT8_MAX);
    if (abs(y_input) > STICK_Y_THRESHOLD) {
        y_input = (y_input > 0) ? 1 : -1;
    } else {
        y_input = 0;
    }
}

void enableControl() {
    digitalWrite(PIN_BUTTON_LED, HIGH);
    is_control_enabled = true;
}

void disableControl() {
    digitalWrite(PIN_BUTTON_LED, LOW);
    is_control_enabled = false;
}

void waitForControl() {
    enableControl(); // 仮に常に有効にする
    while (!active) {
        int8_t x_input, y_input;
        readController(x_input, y_input);
        // Serial.print("X Input: "); Serial.print(x_input);
        // Serial.print(" | Y Input: "); Serial.println(y_input);
        if (is_control_enabled && ( x_input != 0 || y_input != 0)) {
            active = true;
        }
    }
}

void control() {
    Serial.println("controlXY");
    is_button_pressed = false;
    const unsigned long startMillis = millis();
    while (millis() - startMillis < MILLIS_TIMEOUT && !is_button_pressed) {
        int8_t x_input, y_input;
        readController(x_input, y_input);
        sendSignal(x_input, y_input);
        delay(50);
    }
    disableControl();
    is_button_pressed = false;
}

void downArm() {
    Serial.println("downArm");
    // UPからDOWNへ5段階で移動し、各段階で遅延を入れる
    const uint8_t STEPS = 6;
    for (uint8_t i = 1; i <= STEPS; ++i) {
        int angle = DEGREE_SERVO_Z_UP + ((int)DEGREE_SERVO_Z_DOWN - (int)DEGREE_SERVO_Z_UP) * i / STEPS;
        servoZ.write(angle);
        delay(MILLIS_Z_DOWN_TIME / STEPS);
    }
}

void catchObject() {
    Serial.println("catchObject");

    uint8_t DEGREE_SERVO_ARM_CLOSE = random(DEGREE_SERVO_ARM_CLOSE_MIN, DEGREE_SERVO_ARM_CLOSE_MAX);
    delay(MILLIS_ARM_MOVE_INTERVAL);
    servoArm.write(DEGREE_SERVO_ARM_OPEN / 2);
    delay(MILLIS_ARM_MOVE_INTERVAL);
    servoArm.write(DEGREE_SERVO_ARM_CLOSE);
    delay(MILLIS_ARM_MOVE_INTERVAL);
}

void upArm() {
    Serial.println("upArm");
    servoZ.write(DEGREE_SERVO_Z_UP);
    delay(MILLIS_Z_UP_INTERVAL);
}

void goHome() {
    Serial.println("goHome");
    sendSignal(-1, -1);
    delay(MILLIS_HOMING_INTERVAL);
    sendSignal(0, 0);
}

void releaseObject() {
    Serial.println("releaseObject");
    servoArm.write(DEGREE_SERVO_ARM_OPEN);
    active = false;
}