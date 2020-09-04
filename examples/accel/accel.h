/**
 * \file
 *
 * \brief A basic 'accel' applet
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
#include <kernel.h>

#include "examples/dashboard/dashboard.h"

namespace NimbeLink::Examples
{
    class Accel;
}

class NimbeLink::Examples::Accel : public Dashboard::Element
{
    private:
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

        // Our I2C device
        struct device *i2cDevice = nullptr;

	// array to store xyz
	struct Coordinates
	{
	    std::atomic<uint8_t> x = 0;
	    std::atomic<uint8_t> y = 0;
	    std::atomic<uint8_t> z = 0;
	} coord;

	static_assert(decltype(coord.x)::is_always_lock_free, "Atomic variables x isn't lock-free!");
	static_assert(decltype(coord.y)::is_always_lock_free, "Atomic variables y isn't lock-free!");
	static_assert(decltype(coord.z)::is_always_lock_free, "Atomic variables z isn't lock-free!");

    private:
        static void Handler(void *arg1, void *arg2, void *arg3);

        void Run(void);

    public:
        Accel(void);
	void Display(Dashboard::Window &window) override;
};
