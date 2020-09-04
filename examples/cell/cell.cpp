/**
 * \file
 *
 * \brief A basic 'cell' applet
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
#include <kernel.h>
#include <stdlib.h>
#include <string.h>

#include "examples/cell/cell.h"
#include "nimbelink/sdk/secure_services/at.h"

#define ESC 0x1B

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
#	if CONFIG_CELL_PRINT
	printk("returning nullptr\n");
#	endif
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
 * \return none, populate the pqr rsrp and rsrq values
 */
void Cell::Rsrpq(void)
{
    char resp[100];

    // pointers for strtok
    char *tok = nullptr;
    char *rsrp = nullptr;
    char *rsrq = nullptr;

    const char comma[2] = ",";
    NimbeLink::Sdk::SecureServices::At::CommandResult result;
    NimbeLink::Sdk::SecureServices::At::CommandError error;

    // get the cellular status
    int32_t ret = NimbeLink::Sdk::SecureServices::At::RunCommand(&result, &error, "AT+CESQ", 7, resp, 100);
    // CESQ: 99,99,255,x,y

    if (ret == 0)
    {
        if (result != 0)
        {
#           if CONFIG_CELL_DEBUG
            PrintError("AT+CESQ", result, error);
#           endif
	    return;
        }
    }

    tok = strtok(resp, comma);
    // tok = CESQ: 99
    // tok = 99
    // tok = 255
    // tok = x 
    // tok = y 
    // tok = NULL
    while (tok != NULL)
    {   
        rsrq = rsrp;
        rsrp = tok;
        tok = strtok(NULL, comma);
    }

    if (rsrp != nullptr)
    {
        data.rsrp = static_cast<uint8_t>(atoi(rsrp));
    }
    if (rsrq != nullptr)
    {
	data.rsrq = static_cast<uint8_t>(atoi(rsrq));
    }
}

/**
 * \brief Sends the COPS command to the modem to get carrier info 
 *
 * \param none
 *
 * \return none, populate the pqr carrier value
 */
void Cell::Carrier(void)
{
    char resp[100];
    char *tok = nullptr;

    const char quote[2] = "\"";

    NimbeLink::Sdk::SecureServices::At::CommandResult result;
    NimbeLink::Sdk::SecureServices::At::CommandError error;

    // get the MCC MNC codes
    int32_t ret = NimbeLink::Sdk::SecureServices::At::RunCommand(&result, &error, "AT+COPS?", 8, resp, 100);
    // +COPS: x,y,"MCCMNC",z

    if (ret == 0)
    {
        if (result != 0)
        {
#           if CONFIG_CELL_DEBUG
            PrintError("AT+COPS?", result, error);
#           endif
        }
    }

    // +COPS: x,y,"
    tok = strtok(resp, quote);
    // "MCCMNC"
    tok = strtok(NULL, quote);

    // this is crude and non exhaustive
    if (tok != nullptr)
    {
        if (strcmp(tok, "311480") == 0)
	{
	    data.carrier = Carrier::VZW;
	}
	else if (strcmp(tok, "310410") == 0)
	{
	    data.carrier = Carrier::ATT;
	}
	else if (strcmp(tok, "311260") == 0)
	{
	    data.carrier = Carrier::TMB;
        }
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
	// populate the rsrp and rsrq values of the pqr array
	Rsrpq();

	// populate the carrier value of the pqr array
	Carrier();

#	if CONFIG_CELL_DEBUG
	printk("rsrp: %d, rsrq: %d, MCCMNC: %d\n", data.rsrp, data.rsrq, GetCarrierString(data.carrier));
#	endif

        // Wait until we're ready to sample again
        k_sleep(K_SECONDS(CONFIG_CELL_POLL_RATE));
    }
}

void Cell::Display(Dashboard::Window &window)
{
    window.Print("+--------------+\n");
    window.Print("|  Networking  |\n");
    window.Print("|  rsrp:%4d   |\n", static_cast<uint8_t>(data.rsrp));
    window.Print("|  rsrq:%4d   |\n", static_cast<uint8_t>(data.rsrq));
    window.Print("| carrier: %s |\n", GetCarrierString(data.carrier));
    window.Print("+--------------+\n");
}
