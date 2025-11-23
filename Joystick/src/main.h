#ifndef MAIN_H
#define MAIN_H

#include <iostream>

void setup();
void loop();
void printState(byte b);
void sendSignal(int8_t x_input = 0, int8_t y_input = 0);
void readController(int8_t &x_input, int8_t &y_input);
void enableControl();
void disableControl();
void waitForControl();
void control();
void downArm();
void catchObject();
void upArm();
void goHome();
void releaseObject();

#endif