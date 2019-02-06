
#include "SwitchFramFeatherRK.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;
SwitchFramFeatherRK switchFramFeather;

unsigned long lastCounterUpdate = 0;

void setup() {
	Serial.begin();

	// Wait for a USB serial connection for up to 6 seconds to make it easier to monitor the serial output
	waitFor(Serial.isConnected, 6000);


	switchFramFeather.onDipSwitchChange([](SwitchFramFeatherRK&, uint8_t value) {
		char buf[8];
		SwitchFramFeatherRK::onOffString(value, buf);

		Log.info("onDipSwitchChange %s", buf);
	});
	switchFramFeather.onBcdSwitchChange([](SwitchFramFeatherRK&, uint8_t value) {
		Log.info("onBcdSwitchChange %d", value);
	});

	switchFramFeather.setup();
}

void loop() {
	switchFramFeather.loop();

	if (millis() - lastCounterUpdate >= 1000) {
		lastCounterUpdate = millis();

		size_t counter;
		switchFramFeather.get(0, counter);
		counter++;
		switchFramFeather.put(0, counter);

		Log.info("counter=%u", counter);
	}
}
