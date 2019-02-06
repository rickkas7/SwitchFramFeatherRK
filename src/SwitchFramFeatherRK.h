#ifndef __SWITCHFRAMFEATHERRK_H
#define __SWITCHFRAMFEATHERRK_H

#include "Particle.h"

// Required library: https://github.com/rickkas7/MCP23008-RK
#include "MCP23008-RK.h"

// Required library: https://github.com/rickkas7/MB85RC256V-FRAM-RK
#include "MB85RC256V-FRAM-RK.h"

/**
 * @brief Class to manage the SwitchFramFeatherRK demo board that includes a MCP23008 GPIO expanded and a MB85RC256V FRAM.
 */
class SwitchFramFeatherRK : public MB85RC256V {
public:
	/**
	 * @brief Constructor. You typically create a global variable with one of these objects.
	 *
	 * @param gpioAddr The address (0-7) of the GPIO expander chip. The 0x20 base is added in automatically. If you
	 * pass in 8 <= address < 128 then that is used directly as the I2C address.
	 *
	 * @param framAddr The address (0-7) of the FRAM. The 0x50 base is added in automatically. If you pass in
	 * a 8 <= address < 128 then that is used directly as the I2C address.
	 */
	SwitchFramFeatherRK(uint8_t gpioAddr = 0, uint8_t framAddr = 0);

	/**
	 * @brief Destructor. You typically make this as a global variable so it won't be destructed.
	 */
	virtual ~SwitchFramFeatherRK();

	/**
	 * @brief Required: Call from your setup() function.
	 */
	void setup();

	/**
	 * @brief Required: Call from your loop() function.
	 *
	 * This handles checking the button GPIO and calling the callbacks if needed (after debouncing)
	 */
	void loop();

	/**
	 * @brief Used internally from setup and loop. You should not call this directly.
	 */
	void checkGpio(bool notifyAlways = false);

	/**
	 * @brief Returns in buf a 4-character string representing the on-off states of the low 4 bits of value
	 *
	 * @param value The value 0-15. This is what's passed to onDipSwitchChanged, not the raw value from the MCP23008.
	 * @param buf A pointer to a 5 byte buffer. It will contain 4 characters and a null terminator.
	 *
	 * The string will have "|" for the on switches and "o" for the off switches in IEC 5008 style.
	 */
	static void onOffString(uint8_t value, char *buf);

	/**
	 * @brief Sets the callback to call when the DIP switch changes. It's also called once during setup.
	 *
	 * The parameter is a C++11 lambda, with the following prototype:
	 *
	 *   void callback(SwitchFramFeatherRK&, uint8_t value);
	 *
	 * The value is a bitmask of the bits that are on, ranging from 0b0000 (0) to 0b1111 (0xf).
	 */
	inline SwitchFramFeatherRK &onDipSwitchChange(std::function<void(SwitchFramFeatherRK&, uint8_t value)> dipSwitchChange) { this->dipSwitchChange = dipSwitchChange; return *this; };

	/**
	 * @brief Sets the callback to call when the BCD switch changes. It's also called once during setup.
	 *
	 * The parameter is a C++11 lambda, with the following prototype:
	 *
	 *   void callback(SwitchFramFeatherRK&, uint8_t value);
	 *
	 * The value is the BCD value 0 - 9. If you used a hex switch instead, the value would be 0 - 15 (0 - 0xf).
	 */
	inline SwitchFramFeatherRK &onBcdSwitchChange(std::function<void(SwitchFramFeatherRK&, uint8_t value)> bcdSwitchChange) { this->bcdSwitchChange = bcdSwitchChange; return *this; };

	/**
	 * @brief Sets the debounce time for the DIP switches (default: 50 ms)
	 *
	 * You probably don't need to change this.
	 */
	inline SwitchFramFeatherRK &withDipSwitchDebounceMs(unsigned long value) { dipSwitchDebunceMs = value; return *this; };

	/**
	 * @brief Sets the debounce time for the BCD switch (default: 500 ms)
	 *
	 * When you turn the BCD switch, it's common to get weird intermediate values in the process of turning. 500 milliseconds
	 * should be enough to eliminate this. However, if the user is turning the knob slowly and you want to wait until
	 * the knob has definitely stopped moving, you could set this even higher, maybe 1000 ms.
	 */
	inline SwitchFramFeatherRK &withBcdSwitchDebounceMs(unsigned long value) { bcdSwitchDebunceMs = value; return *this; };


private:
	std::function<void(SwitchFramFeatherRK&, uint8_t value)> dipSwitchChange = NULL;
	std::function<void(SwitchFramFeatherRK&, uint8_t value)> bcdSwitchChange = NULL;
	MCP23008 gpio;
	uint8_t lastDipSwitch = 0;
	unsigned long lastDipSwitchTime = 0;
	uint8_t lastBcdSwitch = 0;
	unsigned long lastBcdSwitchTime = 0;
	unsigned long dipSwitchDebunceMs = 50;
	unsigned long bcdSwitchDebunceMs = 500;
};

#endif /* __SWITCHFRAMFEATHERRK_H */

