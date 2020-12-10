/**
 * \file
 *
 * \brief A basic decoupler and linker of the cell app to the dashboard app. Responsible for printing the cell object
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
#include "examples/dashboard/dashboard.h"

namespace NimbeLink::Examples
{
    class CellDisplay;
}

class NimbeLink::Examples::CellDisplay : public Dashboard::Element
{
    private:
        Cell &cell;

    public:
        /**
         * \brief Creates a new cell display
         *
         * \param &cell
         *      The cell to display
         *
         * \return none
         */
        constexpr CellDisplay(Cell &cell):
            cell(cell) {}

        void Display(Dashboard::Window &window) override;
};
