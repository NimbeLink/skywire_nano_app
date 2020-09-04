###
 # \file
 #
 # \brief Builds the Skywire Nano SDK
 #
 # © NimbeLink Corp. 2020
 #
 # All rights reserved except as explicitly granted in the license agreement
 # between NimbeLink Corp. and the designated licensee.  No other use or
 # disclosure of this software is permitted. Portions of this software may be
 # subject to third party license terms as specified in this software, and such
 # portions are excluded from the preceding copyright notice of NimbeLink Corp.
 ##

zephyr_sources_ifdef(
    CONFIG_WIDGET_BLINKY_EXAMPLE
        blinky/blinky.cpp
)

zephyr_sources_ifdef(
    CONFIG_WIDGET_ACCEL_EXAMPLE
        accel/accel.cpp
)

zephyr_sources_ifdef(
    CONFIG_WIDGET_DASHBOARD_EXAMPLE
        dashboard/dashboard.cpp
)

zephyr_sources_ifdef(
    CONFIG_WIDGET_CELL_EXAMPLE
        cell/cell.cpp
)

zephyr_sources_ifdef(
    CONFIG_WIDGET_BUTTON_EXAMPLE
        button/button.cpp
)
