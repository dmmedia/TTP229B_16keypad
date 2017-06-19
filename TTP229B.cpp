/*
  TTP229B.cpp - Library for TTP229B based touchpads.
  Created by Denis Frolov, June 16, 2017.

  This work is licensed under a Creative Commons Attribution 4.0 International License.
  http://creativecommons.org/licenses/by/4.0/
 */
 
// TODO:
// Button callbacks
// Configurable button count
// Configurable button layout
// Callback for every press and release (compare with previous state)

#include "Arduino.h"
#include "TTP229B.h"

TTP229B::TTP229B(int scl_pin, int sda_pin, int extint_pin)
{
	state = Waiting;
	buttonMask = 0;
	for (int i = 0; i < 16; i++) {
		buttonMap[15 - i] = 1 << i;
	}

	pinMode(extint_pin, INPUT);
    pinMode(sda_pin, INPUT);
	pinMode(scl_pin, OUTPUT);
	attachInterrupt(extint_pin, onRise, RISING);
}

void onRise()
{
	state = DataCooking;
	cookingTime = micros();
	detachInterrupt(extint_pin);
	attachInterrupt(extint_pin, onFall, FALLING);
}

void onFall()
{
	state = DataReady;
	detachInterrupt(extint_pin);
}

bool checkKeyState() {
	switch (state) {
		case Waiting:
		{
			break;
		}
		case DataCooking:
		{
			unsigned long checkTime = micros();
			// According to Datasheet, TTP229 should issue Data Valid signal of length 93 uS, but we can make a little over for sure
			if (checkTime - cookingTime > 150)
			{
				detachInterrupt(extint_pin);
				state = Waiting;
				attachInterrupt(extint_pin, onRise, RISING);
			}
			break;
		}
		case DataReady:
		{
			unsigned int first = shiftIn(sda_pin, scl_pin, MSBFIRST);
			unsigned int second = shiftIn(sda_pin, scl_pin, MSBFIRST);
			buttonMask = (first << 8) + second;
			if (buttonMask > 0) {
				for (int i = 0; i < 16; i++)
				{
					if (buttonMask == buttonMap[i])
					{
						Serial.print("Button ");
						Serial.print(i + 1, DEC);
						Serial.println(" pressed");
					}
				}
			} else {
				Serial.println("All buttons released");
			}
			state = Waiting;
			attachInterrupt(extint_pin, onRise, RISING);
			break;
		}
	}
}

unsigned int getKeysPressed() {
	return buttonMask;
}
