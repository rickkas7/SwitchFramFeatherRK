#include "SwitchFramFeatherRK.h"


SwitchFramFeatherRK::SwitchFramFeatherRK(uint8_t gpioAddr, uint8_t framAddr) : MB85RC256V(Wire, framAddr), gpio(Wire, gpioAddr) {

}

SwitchFramFeatherRK::~SwitchFramFeatherRK() {

}

void SwitchFramFeatherRK::setup() {
	// Initialize FRAM
	MB85RC256V::begin();

	// Initialize GPIO. Note that the MCP23008 only supports pull-up, not down. And the switches connect to GND,
	// so closed = LOW = 0. This is backwards of what you might think.
	gpio.begin();
	for(size_t ii = 0; ii < 8; ii++) {
		gpio.pinMode(ii, INPUT_PULLUP);
	}
	checkGpio(true);
}

void SwitchFramFeatherRK::loop() {
	checkGpio();
}


void SwitchFramFeatherRK::checkGpio(bool notifyAlways) {

	// GPIO Function
	// 0    BCD 1
	// 1    BCD 2
	// 2    BCD 4
	// 3    BCD 8
	// 4    Switch 0
	// 5    Switch 1
	// 6    Switch 2
	// 7    Switch 3

	// The bitmask that's returned from readAllPins is 1 << pin, in other words switch 4 is mask 0x80 = 0b10000000

	uint8_t pins = gpio.readAllPins();

	// However, since the MCP23008 only support pull-up and a closed switch = GND = 0, flip all of the bits here so
	// they work more naturally as closed = 1 by XORing with 0xff.
	pins ^= 0xff;

	uint8_t curDipSwitch = pins >> 4;
	uint8_t curBcdSwitch = pins & 0xf;

	if (!notifyAlways) {
		if (curDipSwitch != lastDipSwitch) {
			lastDipSwitch = curDipSwitch;
			lastDipSwitchTime = millis();
		}
		if (lastDipSwitchTime != 0 && millis() - lastDipSwitchTime >= dipSwitchDebunceMs) {
			if (dipSwitchChange) {
				dipSwitchChange(*this, curDipSwitch);
			}
			lastDipSwitchTime = 0;
		}


		if (curBcdSwitch != lastBcdSwitch) {
			lastBcdSwitch = curBcdSwitch;
			lastBcdSwitchTime = millis();
		}

		if (lastBcdSwitchTime != 0 && millis() - lastBcdSwitchTime >= bcdSwitchDebunceMs) {
			if (bcdSwitchChange) {
				bcdSwitchChange(*this, curBcdSwitch);
			}
			lastBcdSwitchTime = 0;
		}
	}
	else {
		// notifyAlways is used to read the initial state of the switches at boot
		if (dipSwitchChange) {
			dipSwitchChange(*this, curDipSwitch);
		}
		lastDipSwitch = curDipSwitch;

		if (bcdSwitchChange) {
			bcdSwitchChange(*this, curBcdSwitch);
		}
		lastBcdSwitch = curBcdSwitch;
	}

	if (curDipSwitch != lastDipSwitch) {
		if (dipSwitchChange) {
			dipSwitchChange(*this, curDipSwitch);
		}
		lastDipSwitch = curDipSwitch;
	}
	if (curBcdSwitch != lastBcdSwitch) {
		if (bcdSwitchChange) {
			bcdSwitchChange(*this, curBcdSwitch);
		}
		lastBcdSwitch = curBcdSwitch;
	}

}

// [static]
void SwitchFramFeatherRK::onOffString(uint8_t value, char *buf) {
	for(size_t ii = 0; ii < 4; ii++) {
		if (value & (1 << ii)) {
			buf[ii] = 'o';
		}
		else {
			buf[ii] = '|';
		}
	}
	buf[4] = 0;
}

