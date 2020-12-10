/**
 * \file
 *
 * \brief A basic 'cell' applet demonstrating sending AT commands
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

#include <atomic>
#include <cstddef>

#include <kernel.h>

#include "examples/utils.h"
#include "examples/dashboard/dashboard.h"
#include "nimbelink/sdk/secure_services/at.h"

namespace NimbeLink::Examples
{
    class Cell;
}

class NimbeLink::Examples::Cell
{
    public:
        /**
         * \brief Available carriers
         */
        enum class Carrier
        {
            VZW,
            ATT,
            TMB,
            UKN,
        };

        /**
         * \brief Cellular information
         */
        struct CellData
        {
            std::atomic<uint8_t> rsrp = 0;
            std::atomic<uint8_t> rsrq = 0;
            std::atomic<enum Carrier> carrier;
        };

        /**
         * \brief Gets the string representing the carrier
         *
         * \param carrier
         *      The carrier whose string to get
         *
         * \return const char *
         *      The string
         */
        static const char *GetCarrierString(enum Carrier carrier)
        {
            switch (carrier)
            {
                case Carrier::VZW:
                    return "VZW";
                case Carrier::ATT:
                    return "ATT";
                case Carrier::TMB:
                    return "TMB";
                case Carrier::UKN:
                    return "UKN";
            }

            return "N/A";
        };

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

        // Struct to store cell data
        struct CellData data = {255, 255, Carrier::UKN};

        // Assertions to check if the values are actually atomic operations and
        // not using locks
        //
        // Another option is to use normal data types (uint8_t, int, etc.) and
        // mutexes.
        static_assert(decltype(data.rsrp)::is_always_lock_free, "Atomic variable rsrp isn't lock-free!");
        static_assert(decltype(data.rsrq)::is_always_lock_free, "Atomic variable rsrq isn't lock-free!");
        static_assert(decltype(data.carrier)::is_always_lock_free, "Atomic variable carrier isn't lock-free!");

    private:
        static void Handler(void *arg1, void *arg2, void *arg3);

        void Rsrpq(void);
        void Carrier(void);

        void Run(void);

    public:
        Cell(void);

        /**
         * \brief Gets our cell data
         *
         * \param none
         *
         * \return struct CellData *
         *      Our cell data
         */
        struct CellData *GetStruct(void)
        {
            return &this->data;
        }
};
