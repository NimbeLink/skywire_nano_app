/**
 * \file
 *
 * \brief A basic 'button' applet
 *
 * © NimbeLink Corp. 2020
 *
 * All rights reserved except as explicitly granted in the license agreement
 * between NimbeLink Corp. and the designated licensee.  No other use or
 * disclosure of this software is permitted. Portions of this software may be
 * subject to third party license terms as specified in this software, and such
 * portions are excluded from the preceding copyright notice of NimbeLink Corp.
 */
#pragma once

#include <atomic>
#include <cstddef>

#include <device.h>
#include <drivers/gpio.h>
#include <kernel.h>

#include "examples/dashboard/dashboard.h"

namespace NimbeLink::Examples
{
    class Button;
}

class NimbeLink::Examples::Button : public Dashboard::Element
{
    private:
        // Our GPIO pin
        static constexpr const std::size_t GpioPin = DT_GPIO_KEYS_BUTTON_0_GPIOS_PIN;

	// number of times the button has been pressed
	std::atomic<uint8_t> count = 0;
	static_assert(decltype(count)::is_always_lock_free, "Atomic variable count isn't lock-free!");

        // Our GPIO device
        struct device *gpioDevice = nullptr;

	// Our GPIO callback
	struct gpio_callback gpio_button_cb;

    private:
	static void ButtonCallback(struct device *, struct gpio_callback *, uint32_t pins);

    public:
        Button(void);
	void Display(Dashboard::Window &window) override;
};
