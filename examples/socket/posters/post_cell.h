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
#pragma once

#include <atomic>
#include <cstddef>

#include <kernel.h>

#include "examples/cell/cell.h"
#include "examples/socket/socket.h"

namespace NimbeLink::Examples
{
    class CellPoster;
}

class NimbeLink::Examples::CellPoster : public Socket::Data
{
    private:
        // Cell application that is getting the rsrp/rsrq/carrier info
        Cell &cell;

    public:
        /**
         * \brief Creates a new cell poster
         *
         * \param &cell
         *      The cell whose data to post
         *
         * \return none
         */
        constexpr CellPoster(Cell &cell):
            cell(cell) {}

        void Retrieve(char *buffer, uint16_t max_length) override;
};
