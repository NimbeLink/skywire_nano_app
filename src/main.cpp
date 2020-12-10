/**
 * \file
 *
 * \brief A basic widget starting point
 *
 * (C) NimbeLink Corp. 2020
 *
 * All rights reserved except as explicitly granted in the license agreement
 * between NimbeLink Corp. and the designated licensee.  No other use or
 * disclosure of this software is permitted. Portions of this software may be
 * subject to third party license terms as specified in this software, and such
 * portions are excluded from the preceding copyright notice of NimbeLink Corp.
 */
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <zephyr.h>

#if CONFIG_WIDGET_BLINKY_EXAMPLE
#include "examples/blinky/blinky.h"
#endif

#if CONFIG_WIDGET_BUTTON_EXAMPLE
#include "examples/button/button.h"
#endif

#if CONFIG_WIDGET_ACCEL_EXAMPLE
#include "examples/accel/accel.h"
#endif

#if CONFIG_WIDGET_CELL_EXAMPLE
#include "examples/cell/cell.h"
#endif

#if CONFIG_WIDGET_DASHBOARD_EXAMPLE
#include "examples/dashboard/dashboard.h"
#if CONFIG_WIDGET_CELL_EXAMPLE
#include "examples/dashboard/elements/display_cell.h"
#endif
#endif

#if CONFIG_WIDGET_SOCKET_EXAMPLE
#include "examples/socket/socket.h"
#if CONFIG_WIDGET_CELL_EXAMPLE
#include "examples/socket/posters/post_cell.h"
#endif
#endif

#include "examples/utils.h"

#include "nimbelink/sdk/secure_services/at.h"
#include "nimbelink/sdk/secure_services/kernel.h"

using namespace NimbeLink::Examples;
using namespace NimbeLink::Sdk;

void main(void)
{
#   if CONFIG_CELL_CAGE_SIM || CONFIG_WIDGET_SOCKET_EXAMPLE
    // allow the modem to boot up properly before trying to change the sim
    k_sleep(K_SECONDS(20));

    static constexpr const std::string_view commands[] = {
        "AT+CFUN=4",
        "AT#SIMSELECT=1",
        "AT+CFUN=1"
    };

    for (std::size_t i = 0; i < std::size(commands); i++)
    {

        char resp[100];
        SecureServices::At::Result result;
        SecureServices::At::Error error;
        int32_t ret = SecureServices::At::RunCommand(&result, &error, std::data(commands[i]), std::size(commands[i]), resp, 100, nullptr);

        if (ret == 0)
        {
            if (result != 0)
            {
                Utils::PrintError(commands[i], result, error);
            }
        }
    }
#   endif

#   if CONFIG_WIDGET_BLINKY_EXAMPLE
    static Blinky blinky;
#   endif

#   if CONFIG_WIDGET_BUTTON_EXAMPLE
    static Button button;
#   endif


#   if CONFIG_WIDGET_ACCEL_EXAMPLE
    static Accel accel;
#   endif

#   if CONFIG_WIDGET_CELL_EXAMPLE
    static NimbeLink::Examples::Cell cell;
#   endif

#   if CONFIG_WIDGET_DASHBOARD_EXAMPLE
    static Dashboard dashboard(CONFIG_DASHBOARD_X, CONFIG_DASHBOARD_Y, CONFIG_DASHBOARD_W, CONFIG_DASHBOARD_H);
#   if CONFIG_WIDGET_ACCEL_EXAMPLE
    dashboard.RegisterElement(accel);
#   endif
#   if CONFIG_WIDGET_CELL_EXAMPLE
    static CellDisplay display(cell);
    dashboard.RegisterElement(display);
#   endif
#   if CONFIG_WIDGET_BUTTON_EXAMPLE
    dashboard.RegisterElement(button);
#   endif
#   endif

#   if CONFIG_WIDGET_SOCKET_EXAMPLE
    static Socket socket;
#   if CONFIG_WIDGET_CELL_EXAMPLE
    static CellPoster poster(cell);
    socket.RegisterData(poster);
#   endif
#   endif
}
