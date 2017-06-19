/*
  TTP229B.h - Library for TTP229B based touchpads.
  Created by Denis Frolov, June 16, 2017.

  This work is licensed under a Creative Commons Attribution 4.0 International License.
  http://creativecommons.org/licenses/by/4.0/
 */

#ifndef TTP229B_h
#define TTP229B_h

#include "Arduino.h"

enum State {
  Waiting = 0, // Waiting for keypress and Data Valid signal from touchpad
  DataCooking, // Got Data Valid signal start, now waiting for Data Valid signal end
  DataReady    // Got properly timed Data Valid signal end, so we can now read which key was pressed
};

class TTP229B {
	public:
		TTP229B(int scl_pin, int sda_pin, int extint_pin);
		void onRise();
		void onFall();
		bool checkKeyState();
		unsigned int getKeysPressed();

	private:
		int _sda;
		int _scl;
		int _extint;

		volatile State state;

		unsigned int buttonMap[16];
		
		volatile unsigned int buttonMask;
}

#endif TTP229B_h
