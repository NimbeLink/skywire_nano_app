/**
 * \file
 *
 * \brief A basic dashboard applet
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

#include <kernel.h>

#include "examples/dashboard/dashboard.h"
#include "nimbelink/sdk/secure_services/at.h"

using namespace NimbeLink::Examples;

/**
 * \brief Zephyr thread handler
 *
 * \param *arg1
 *      A pointer to our dashboard object
 * \param *arg2
 *      Unused
 * \param *arg3
 *      Unused
 *
 * \return none
 */
void Dashboard::Handler(void *arg1, void *arg2, void *arg3)
{
    (void)arg2;
    (void)arg3;

    Dashboard *dashboard = static_cast<Dashboard *>(arg1);

    if (dashboard == nullptr)
    {
#       if CONFIG_DASHBOARD_DEBUG
	printk("returning nullptr\n");
#       endif
        return;
    }

    dashboard->Run();
}

/**
 * \brief Creates a new dashboard instance
 *
 * \param none
 *
 * \return none
 */
Dashboard::Dashboard(
    std::size_t gridX,
    std::size_t gridY,
    std::size_t windowWidth,
    std::size_t windowHeight
):
    windowColumns(gridX),
    windowRows(gridY),
    windowWidth(windowWidth),
    windowHeight(windowHeight)
{

    // Create our thread
    this->threadId = k_thread_create(
        &this->thread,
        this->stack,
        std::size(this->stack),
        Dashboard::Handler,
        static_cast<void *>(this),
        nullptr,
        nullptr,
        K_LOWEST_APPLICATION_THREAD_PRIO,
        0,
        0
    );
}

void Dashboard::Window::Setup(void) 
{
    printk("%c[%d;%dH", ESC, this->row + 2, this->column + 1);
}

void Dashboard::Window::Print(const char *format, ...) 
{
    va_list list;
    va_start(list, format);

    z_vprintk(this->Print, reinterpret_cast<void *>(this), format, list);

    va_end(list);
}

int Dashboard::Window::Print(int c, void *context)
{
    Window *window = reinterpret_cast<Window *>(context);
    window->Print(c);
    return 0;
}

void Dashboard::Window::Print(int c) 
{
    printk("%c", c);

    switch (c)
    {
        case '\n':
        {
            this->MoveCursor(this->column);
            break;
        }
    }
}

void Dashboard::Window::MoveCursor(std::size_t count) 
{
    if (count > 1)
    {
        printk("%c[%dC", ESC, count);
    }
}
/**
 * \brief Runs our dashboard example
 *
 * \param none
 *
 * \return none
 */
void Dashboard::Run(void)
{
    while (true)
    {
        // Wait until we're ready to sample
        k_sleep(K_SECONDS(CONFIG_DASHBOARD_UPDATE_RATE));

        for (std::size_t i = 0; i < this->size; i++)
        {
	    std::size_t column = (i % this->windowColumns) * this->windowWidth;
	    std::size_t row = (i / this->windowColumns) * this->windowHeight;

	    Window window(column, row);

	    window.Setup();

	    this->elements[i]->Display(window);
        }
    }
}

/**
 * \brief Function to register a device with the dashboard handler
 *
 * \param element is a reference to the element object
 *
 * \return none
 */
void Dashboard::RegisterElement(Element &element)
{
    this->elements[this->size++] = &element;

#   if CONFIG_DASHBOARD_DEBUG
    printk("Registered element %d\n", this->size - 1);
#   endif
}
