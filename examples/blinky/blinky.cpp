/**
 * \file
 *
 * \brief A basic 'blinky' applet that demonstrates communicating with the
 *        onboard LED
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

#include "examples/blinky/blinky.h"

using namespace NimbeLink::Examples;

/**
 * \brief Zephyr thread handler
 *
 * \param *arg1
 *      A pointer to our blinky object
 * \param *arg2
 *      Unused
 * \param *arg3
 *      Unused
 *
 * \return none
 */
void Blinky::Handler(void *arg1, void *arg2, void *arg3)
{
    (void)arg2;
    (void)arg3;

    Blinky *blinky = static_cast<Blinky *>(arg1);

    if (blinky == nullptr)
    {
    #   if CONFIG_BLINKY_DEBUG
        printk("return nullptr blinky handler\n");
    #   endif

        return;
    }

    blinky->Run();
}

/**
 * \brief Creates a new blinky instance
 *
 * \param none
 *
 * \return none
 */
Blinky::Blinky(void)
{
    // Try to get our GPIO device
    this->gpioDevice = device_get_binding("GPIO_0");

    // If that didn't work for some reason, don't bother with our thread
    if (this->gpioDevice == nullptr)
    {
    #   if CONFIG_BLINKY_DEBUG
        printk("return nullptr gpio device\n");
    #   endif

        return;
    }

    // Create our thread
    this->threadId = k_thread_create(
        &this->thread,
        this->stack,
        std::size(this->stack),
        Blinky::Handler,
        static_cast<void *>(this),
        nullptr,
        nullptr,
        K_LOWEST_APPLICATION_THREAD_PRIO,
        0,
        0
    );
}

/**
 * \brief Runs our blinky example
 *
 * \param none
 *
 * \return none
 */
void Blinky::Run(void)
{
    while (true)
    {
        // Wait until we're ready to transition to our next state
        k_sleep(1000 / CONFIG_BLINKY_BLINK_RATE / 2);

        uint32_t currentState;

        // Get the current state of the LED
        int result = gpio_pin_read(
            this->gpioDevice,
            this->GpioPin,
            &currentState
        );

        // If that failed for some reason, don't try changing state
        if (result != 0)
        {
        #   if CONFIG_BLINKY_DEBUG
            printk("unable to read gpio pin\n");
        #   endif

            continue;
        }

        // Set our GPIO to its opposite state
        gpio_pin_write(
            this->gpioDevice,
            this->GpioPin,
            static_cast<uint32_t>(!currentState)
        );
    }
}
