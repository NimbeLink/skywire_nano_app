/**
 * \file
 *
 * \brief A basic 'cell' applet demonstrating using AT commands
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
#include <cstdlib>
#include <cstring>

#include <kernel.h>

#include "examples/cell/cell.h"
#include "nimbelink/sdk/secure_services/at.h"

using namespace NimbeLink::Examples;

/**
 * \brief Zephyr thread handler
 *
 * \param *arg1
 *      A pointer to our cell object
 * \param *arg2
 *      Unused
 * \param *arg3
 *      Unused
 *
 * \return none
 */
void Cell::Handler(void *arg1, void *arg2, void *arg3)
{
    (void)arg2;
    (void)arg3;

    Cell *cell = static_cast<Cell *>(arg1);

    if (cell == nullptr)
    {
    #   if CONFIG_CELL_PRINT
        printk("returning nullptr\n");
    #   endif

        return;
    }

    cell->Run();
}

/**
 * \brief Creates a new cell instance
 *
 * \param none
 *
 * \return none
 */
Cell::Cell(void)
{
    // Create our thread
    this->threadId = k_thread_create(
        &this->thread,
        this->stack,
        std::size(this->stack),
        Cell::Handler,
        static_cast<void *>(this),
        nullptr,
        nullptr,
        K_LOWEST_APPLICATION_THREAD_PRIO,
        0,
        0
    );
}

/**
 * \brief Sends the CESQ command to the modem to get cell status
 *
 * \param none
 *
 * \return none
 */
void Cell::Rsrpq(void)
{
    char resp[100];

    NimbeLink::Sdk::SecureServices::At::Result result;
    NimbeLink::Sdk::SecureServices::At::Error error;

    // Get the cellular status
    //
    // CESQ: 99,99,255,x,y
    int32_t ret = NimbeLink::Sdk::SecureServices::At::RunCommand(&result, &error, "AT+CESQ", 7, resp, 100, nullptr);

    // If the AT command was not successfully processed, stop
    if (ret != 0)
    {
    #   if CONFIG_CELL_DEBUG
        printk("AT+CESQ was not processed");
    #   endif

        return;
    }

    // If AT command failed, stop
    if (result != 0)
    {
    #   if CONFIG_CELL_DEBUG
        PrintError("AT+CESQ", result, error);
    #   endif

        return;
    }

#   if CONFIG_CELL_DEBUG
    printk("resp: %s\n", resp);
#   endif

    const char comma[2] = ",";

    char *tok = strtok(resp, comma);

    char *rsrp = nullptr;
    char *rsrq = nullptr;

    // Ideally:
    //
    // tok = CESQ: 99
    // tok = 99
    // tok = 255
    // tok = x
    // tok = y
    // tok = nullptr
    while (tok != nullptr)
    {
        rsrq = rsrp;
        rsrp = tok;

        tok = strtok(nullptr, comma);
    }

    // Check to make sure that the desired result string was returned before
    // setting the struct values
    if (rsrp != nullptr)
    {
        this->data.rsrp = static_cast<uint8_t>(atoi(rsrp));
    }

    if (rsrq != nullptr)
    {
        this->data.rsrq = static_cast<uint8_t>(atoi(rsrq));
    }
}

/**
 * \brief Sends the COPS command to the modem to get carrier info
 *
 * \param none
 *
 * \return none
 */
void Cell::Carrier(void)
{
    char resp[100];

    NimbeLink::Sdk::SecureServices::At::Result result;
    NimbeLink::Sdk::SecureServices::At::Error error;

    // get the MCC MNC codes
    int32_t ret = NimbeLink::Sdk::SecureServices::At::RunCommand(&result, &error, "AT+COPS?", 8, resp, 100, nullptr);
    // +COPS: x,y,"MCCMNC",z

    // If the AT command was not successfully processed, stop
    if (ret != 0)
    {
    #   if CONFIG_CELL_DEBUG
        printk("AT+COPS? was not processed");
    #   endif

        return;
    }

    // If the AT command failed
    if (result != 0)
    {
    #   if CONFIG_CELL_DEBUG
        PrintError("AT+COPS?", result, error);
    #   endif
    }

#   if CONFIG_CELL_DEBUG
    printk("resp: %s\n", resp);
#   endif

    const char quote[2] = "\"";

    // +COPS: x,y,"
    char *tok = strtok(resp, quote);

    while (tok != nullptr)
    {
    #   if CONFIG_CELL_DEBUG
        printk("tok: %s\n", tok);
    #   endif

        tok = strtok(nullptr, quote);

        break;
    }

    if (tok == nullptr)
    {
        return;
    }

    if (strcmp(tok, "311480") == 0)
    {
        this->data.carrier = Carrier::VZW;
    }
    else if (strcmp(tok, "310410") == 0)
    {
        this->data.carrier = Carrier::ATT;
    }
    else if (strcmp(tok, "311260") == 0)
    {
        this->data.carrier = Carrier::TMB;
    }
}

/**
 * \brief Runs our cell example
 *
 * \param none
 *
 * \return none
 */
void Cell::Run(void)
{
    while (true)
    {
        // Populate the rsrp and rsrq values of the pqr array
        this->Rsrpq();

        // Populate the carrier value of the pqr array
        this->Carrier();

    #   if CONFIG_CELL_DEBUG
        printk("rsrp: %d, rsrq: %d, MCCMNC: %s\n",
            static_cast<uint8_t>(this->data.rsrp),
            static_cast<uint8_t>(this->data.rsrq),
            Cell::GetCarrierString(this->data.carrier)
        );
    #   endif

        // Wait until we're ready to sample again
        k_sleep(K_SECONDS(CONFIG_CELL_POLL_RATE));
    }
}
