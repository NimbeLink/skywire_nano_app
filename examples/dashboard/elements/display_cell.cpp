/**
 * \file
 *
 * \brief A basic linker and decoupler between the cell and dashboard apps
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

#include "examples/dashboard/elements/display_cell.h"

#define ESC 0x1B

using namespace NimbeLink::Examples;

/**
 * \brief Displays our cell
 *
 * \param &window
 *      The window to display on
 *
 * \return none
 */
void CellDisplay::Display(Dashboard::Window &window)
{
    Cell::CellData *data = this->cell.GetStruct();

    window.Print("+--------------+\n");
    window.Print("|  Networking  |\n");
    window.Print("|  rsrp:%4d   |\n", static_cast<uint8_t>(data->rsrp));
    window.Print("|  rsrq:%4d   |\n", static_cast<uint8_t>(data->rsrq));
    window.Print("| carrier: %s |\n", Cell::GetCarrierString(data->carrier));
    window.Print("+--------------+\n");
}
