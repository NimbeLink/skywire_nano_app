/**
 * \file
 *
 * \brief A basic widget starting point
 *
 * © NimbeLink Corp. 2020
 *
 * All rights reserved except as explicitly granted in the license agreement
 * between NimbeLink Corp. and the designated licensee.  No other use or
 * disclosure of this software is permitted. Portions of this software may be
 * subject to third party license terms as specified in this software, and such
 * portions are excluded from the preceding copyright notice of NimbeLink Corp.
 */
#include <chrono>
#include <cstdint>

#include <stdlib.h>
#include <string.h>
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
#endif

#include "examples/utils.h"

#include "nimbelink/sdk/secure_services/at.h"
#include "nimbelink/sdk/secure_services/kernel.h"

void main(void)
{
#   if CONFIG_CELL_CAGE_SIM
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
        NimbeLink::Sdk::SecureServices::At::Result result;
        NimbeLink::Sdk::SecureServices::At::Error error;
        int32_t ret = NimbeLink::Sdk::SecureServices::At::RunCommand(&result, &error, std::data(commands[i]), std::size(commands[i]), resp, 100);

	if (ret == 0)
	{
            if (result != 0)
            {
                NimbeLink::Examples::Utils::PrintError(commands[i], result, error);
            }
	}
    }
#   endif

#   if CONFIG_WIDGET_BLINKY_EXAMPLE
    static NimbeLink::Examples::Blinky blinky;
#   endif

#   if CONFIG_WIDGET_BUTTON_EXAMPLE
    static NimbeLink::Examples::Button button;
#   endif


#   if CONFIG_WIDGET_ACCEL_EXAMPLE
    static NimbeLink::Examples::Accel accel;
#   endif

#   if CONFIG_WIDGET_CELL_EXAMPLE
    static NimbeLink::Examples::Cell cell;
#   endif

#   if CONFIG_WIDGET_DASHBOARD_EXAMPLE
    static NimbeLink::Examples::Dashboard dashboard(CONFIG_DASHBOARD_X, CONFIG_DASHBOARD_Y, CONFIG_DASHBOARD_W, CONFIG_DASHBOARD_H);
#   if CONFIG_WIDGET_ACCEL_EXAMPLE
    dashboard.RegisterElement(accel);
#   endif
#   if CONFIG_WIDGET_CELL_EXAMPLE
    dashboard.RegisterElement(cell);
#   endif
#   if CONFIG_WIDGET_BUTTON_EXAMPLE
    dashboard.RegisterElement(button);
#   endif
#   endif
}
