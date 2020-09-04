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
#include <array>
#include <cstddef>

#include <device.h>
#include <drivers/i2c.h>
#include <kernel.h>

#include "examples/accel/accel.h"
#include "nimbelink/sdk/secure_services/at.h"

#define ACCEL_I2C_ADDR 0x32 >> 1
#define ESC 0x1B

using namespace NimbeLink::Examples;

/**
 * \brief Zephyr thread handler
 *
 * \param *arg1
 *      A pointer to our accel object
 * \param *arg2
 *      Unused
 * \param *arg3
 *      Unused
 *
 * \return none
 */
void Accel::Handler(void *arg1, void *arg2, void *arg3)
{
    (void)arg2;
    (void)arg3;

    Accel *accel = static_cast<Accel *>(arg1);

    if (accel == nullptr)
    {
#	if CONFIG_ACCEL_DEBUG
	printk("returning nullptr\n");
#	endif
        return;
    }

    accel->Run();
}

/**
 * \brief Constructor. Creates a new accel instance
 *
 * \param none
 *
 * \return none
 */
Accel::Accel(void)
{
    // Try to get our I2C device
    this->i2cDevice = device_get_binding("I2C_2");

    // If that didn't work for some reason, don't bother with our thread or configuration
    if (this->i2cDevice == nullptr)
    {
#	if CONFIG_ACCEL_DEBUG
	printk("returning i2cnullptr\n");
#	endif
        return;
    }

    // start configuration for the accelerometer

    static constexpr const struct
    {
        uint8_t address;
	uint8_t value;
    } setups[] = {
        {0x20, 0x2F},  // CTRL_REG1 (20h) Enable x,y,z axis, 10 Hz, low power mode (8 bit results)
    };

    for (std::size_t i = 0; i < std::size(setups); i++)
    {
        if (i2c_reg_write_byte(this->i2cDevice, ACCEL_I2C_ADDR, setups[i].address, setups[i].value) != 0)
        {
#	    if CONFIG_ACCEL_DEBUG
            printk("Unable to set 0x%02X to 0x%02X", setups[i].address, setups[i].value);
#	    endif
            return;
        }
    }

    // Create our thread, runs the Handler function, which then just calls Run
    this->threadId = k_thread_create(
        &this->thread,
        this->stack,
        std::size(this->stack),
        Accel::Handler,
        static_cast<void *>(this),
        nullptr,
        nullptr,
        K_LOWEST_APPLICATION_THREAD_PRIO,
        0,
        0
    );
}

/**
 * \brief Runs our accel example
 *
 * \param none
 *
 * \return none
 */
void Accel::Run(void)
{
    // accelerometer is configured for 8 bit numbers - reading X_H, Y_H, Z_H registers
    const uint8_t addr[3] = {0x29, 0x2B, 0x2D};
    while (true)
    {
        // Wait until we're ready to sample
        k_sleep(1000 / CONFIG_ACCEL_SAMPLE_RATE);

    	int result;
	uint8_t xyz[3] = {0};

	for (int i = 0; i < 3; i++) {
	    // read x/y/z accel value and put it in xyz[0/1/2] array
	    // this returns 0 if successful
            result = i2c_reg_read_byte(
                this->i2cDevice,
	        ACCEL_I2C_ADDR,
	        addr[i],
	        &xyz[i]
            );

	    // if the read failed, end the function
	    if (result != 0) {
	        break;
	    }
	}

        // end the function if a read failed
        if (result != 0)
        {
            break;
        }

	this->coord.x = xyz[0];
	this->coord.y = xyz[1];
	this->coord.z = xyz[2];

#	if CONFIG_ACCEL_DEBUG
	printk("x: %d, y: %d, z: %d\n", coord.x, coord., coord,z);
#	endif
    }
}

void Accel::Display(Dashboard::Window &window)
{

#   if CONFIG_ACCEL_DEBUG
    printk("In Accel::Display\n");
#   endif

    window.Print("+---------------+\n");
    window.Print("| Accelerometer |\n");
    window.Print("|    x:%4d     |\n", static_cast<int8_t>(this->coord.x));
    window.Print("|    y:%4d     |\n", static_cast<int8_t>(this->coord.y));
    window.Print("|    z:%4d     |\n", static_cast<int8_t>(this->coord.z));
    window.Print("+---------------+\n");

#   if CONFIG_ACCEL_DEBUG
    printk("Exiting Accel::Display\n");
#   endif
}
