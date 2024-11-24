#ifndef MAIN_H
#define MAIN_H

#include <iostream>

void setup();
void loop();
void waitForActivate();
void waitForButton();
void controlXY();
void catchObject(uint8_t mode);
void goHome();
void releaseObject();

#endif