/**
 * \file
 *
 * \brief A basic dashboard dial applet that demonstrates communication between
 *        applications
 *
 *  Displays other application information on the serial console. VT100 codes
 *  are used to print to the serial console. Find them here:
 *
 *      http://ascii-table.com/ansi-escape-sequences-vt-100.php
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
#include <vector>

#include <kernel.h>

#define ESC 0x1B

namespace NimbeLink::Examples
{
    class Dashboard;
}

class NimbeLink::Examples::Dashboard
{
    public:
        // Class that represents a portion of the screen
        class Window
        {
            private:
                // Defines the upper left corner
                std::size_t column;
                std::size_t row;

            private:
                static int Print(int c, void *context);
                void Print(int c);

                void MoveCursor(std::size_t count);

            public:
                /**
                 * \brief Creates a new window
                 *
                 * \param c
                 *      The upper-left column
                 * \param r
                 *      The upper-left row
                 */
                constexpr Window(std::size_t column, std::size_t row):
                    column(column),
                    row(row) {}

                void Setup(void);
                void Print(const char *format, ...);
        };

        // Interface class for classes that want to print to the dashboard
        //
        // Must inherit and override the functions below.
        class Element
        {
            public:
                virtual void Display(Window &window) = 0;
        };

    private:
        // Our Zephyr stack
        //
        // C++ isn't a big fan of placing a class' member in a section, so
        // we'll reproduce what the Zephyr library does when someone uses
        // K_THREAD_STACK_DEFINE().
        __attribute__((aligned(STACK_ALIGN))) struct _k_thread_stack_element stack[1024 + MPU_GUARD_ALIGN_AND_SIZE];

        // Our Zephyr thread
        struct k_thread thread;

        // Our thread's ID
        k_tid_t threadId;

        // Number of objects in the elements array
        uint8_t size = 0;
        Element *elements[9] = {0};

        // Defines the grid of windows
        //
        // Ex: 3x3 where each window is 20 characters by 20 characters.
        std::size_t windowColumns;
        std::size_t windowRows;
        std::size_t windowWidth;
        std::size_t windowHeight;

    private:
        static void Handler(void *arg1, void *arg2, void *arg3);

        void Run(void);

    public:
        Dashboard(
            std::size_t gridX,
            std::size_t gridY,
            std::size_t windowWidth,
            std::size_t windowHeight
        );

        void RegisterElement(Element &element);
};
