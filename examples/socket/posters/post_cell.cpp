/**
 * \file
 *
 * \brief A basic decoupler and linker of the cell app to the dashboard app
 *
 *  Responsible for printing the cell object.
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

#include "examples/socket/posters/post_cell.h"

using namespace NimbeLink::Examples;

/**
 * \brief Passes along the query string to be included in a POST request
 *
 * \param buffer
 *      The buffer to store the string
 * \param max_length
 *      The max length of the string to store in the buffer
 *
 * \return none
 */
void CellPoster::Retrieve(char *buffer, uint16_t max_length)
{
    Cell::CellData *data = this->cell.GetStruct();

    snprintf(buffer, max_length, "rsrp=%d&rsrq=%d&carrier=%s",
        static_cast<uint8_t>(data->rsrp),
        static_cast<uint8_t>(data->rsrq),
        Cell::GetCarrierString(data->carrier)
    );
}
