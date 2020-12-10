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
#pragma once

#include <cstddef>

#include <device.h>
#include <kernel.h>

namespace NimbeLink::Examples
{
    class Blinky;
}

class NimbeLink::Examples::Blinky
{
    private:
        // The frequency we'll blink at
        static constexpr const std::size_t BlinkFrequency = CONFIG_BLINKY_BLINK_RATE;

        // Our GPIO pin
        static constexpr const std::size_t GpioPin = CONFIG_BLINKY_GPIO_PIN;

        // Our Zephyr stack
        //
        // C++ isn't a big fan of placing a class' member in a section, so
        // we'll reproduce what the Zephyr library does when someone uses
        // K_THREAD_STACK_DEFINE().
        __attribute__((aligned(STACK_ALIGN))) struct _k_thread_stack_element stack[512 + MPU_GUARD_ALIGN_AND_SIZE];

        // Our Zephyr thread
        struct k_thread thread;

        // Our thread's ID
        k_tid_t threadId;

        // Our GPIO device
        struct device *gpioDevice = nullptr;

    private:
        static void Handler(void *arg1, void *arg2, void *arg3);

        void Run(void);

    public:
        Blinky(void);
};
