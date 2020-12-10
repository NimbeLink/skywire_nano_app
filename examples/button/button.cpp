/**
 * \file
 *
 * \brief A basic 'button' applet that demonstrates communicating with the
 *        button and callbacks
 *
 * (C) NimbeLink Corp. 2020
 *
 * All rights reserved except as explicitly granted in the license agreement
 * between NimbeLink Corp. and the designated licensee.  No other use or
 * disclosure of this software is permitted. Portions of this software may be
 * subject to third party license terms as specified in this software, and such
 * portions are excluded from the preceding copyright notice of NimbeLink Corp.
 */
#include <array>
#include <cstddef>

#include <device.h>
#include <drivers/gpio.h>
#include <kernel.h>

#include "examples/button/button.h"
#include "examples/utils.h"

#define ESC 0x1B

using namespace NimbeLink::Examples;

/**
 * \brief Creates a new button instance
 *
 * \param none
 *
 * \return none
 */
Button::Button(void)
{
    // Try to get our GPIO device
    this->gpioDevice = device_get_binding("GPIO_0");

    // If that didn't work for some reason, don't bother with config settings
    if (this->gpioDevice == nullptr)
    {
    #   if CONFIG_BUTTON_DEBUG
        printk("button returning nullptr gpio device\n");
    #   endif

        return;
    }

    // Configure the button (pin 6) to be input, trigger an interrupt, active
    // low, and pull up the pin
    //
    // see documentation at https://nimbelink.com/products/4g-lte-m-global-nano/
    int8_t ret = gpio_pin_configure(this->gpioDevice, this->GpioPin,
          (GPIO_DIR_IN | GPIO_INT | GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW | GPIO_PUD_PULL_UP));

    // If the configuration failed, stop
    if (ret != 0)
    {
    #   if CONFIG_BUTTON_DEBUG
        printk("button configuration failed: %d\n", ret);
    #   endif

        return;
    }

    // Initialize the gpio_callback struct
    //
    // gpio_button_cb is a private gpio_callback struct that is used to store
    // callbacks. ButtonCallback is the name of the function that we want
    // triggered on each press. BIT(pin) is a bit mask of the pin the button is
    // on
    gpio_init_callback(&gpio_button_cb, ButtonCallback, BIT(this->GpioPin));

    // Add the now-filled out gpio_callback struct to the device
    ret = gpio_add_callback(this->gpioDevice, &gpio_button_cb);

    if (ret != 0)
    {
    #   if CONFIG_BUTTON_DEBUG
        printk("button adding callback failed: %d\n", ret);
    #   endif

        return;
    }

    // Enable the callbacks that work off of pin 6
    ret = gpio_pin_enable_callback(this->gpioDevice, this->GpioPin);

    if (ret != 0)
    {
    #   if CONFIG_BUTTON_DEBUG
        printk("button enabling callback failed: %d\n", ret);
    #   endif

        return;
    }
}

/**
 * \brief Interrupt handler for a button press. Gives a count to the semaphore
 *
 * \param *dev
 *      Pointer to the gpio controller
 * \param *cb
 *      Pointer to the gpio_callback struct - can be used to get private value
 * \param pins
 *      Mask of the gpio pins
 *
 * \return none
 */
void Button::ButtonCallback(struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    Button *button = Utils::GetContainer<
        Button,
        struct gpio_callback,
        &Button::gpio_button_cb
    >(cb);

    button->count++;
}

/**
 * \brief Prints button data to the dashboard window
 *
 * \param window
 *      A unique window within the dashboard
 *
 * \return none
 */
void Button::Display(Dashboard::Window &window)
{
#   if CONFIG_BUTTON_DEBUG
    printk("In Button::Display\n");
#   endif
    window.Print("+---------------+\n");
    window.Print("|    Button     |\n");
    window.Print("|  Count:%4d   |\n", static_cast<uint8_t>(this->count));
    window.Print("+---------------+\n");
#   if CONFIG_BUTTON_DEBUG
    printk("Exiting Button::Display\n");
#   endif
}

