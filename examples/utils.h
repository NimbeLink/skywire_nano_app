/**
 * \file
 *
 * \brief Provides a few basic utilities
 *
 * © NimbeLink Corp. 2020
 *
 * All rights reserved except as explicitly granted in the license agreement
 * between NimbeLink Corp. and the designated licensee.  No other use or
 * disclosure of this software is permitted. Portions of this software may be
 * subject to third party license terms as specified in this software, and such
 * portions are excluded from the preceding copyright notice of NimbeLink Corp.
 */
#pragma once

#include <string>

#include "nimbelink/sdk/secure_services/at.h"

namespace NimbeLink::Examples::Utils
{
    static inline void PrintError(std::string_view command, int result, NimbeLink::Sdk::SecureServices::At::Error error)
    {
        switch (result)
        {
            case 1:
                printk("%s failed with cme error %d\n", std::data(command), static_cast<int>(error.cmeError));
                break;
            case 2:
                printk("%s failed with cms error %d\n", std::data(command), static_cast<int>(error.cmsError));
                break;
            case 3:
                printk("%s failed with cme error %d\n", std::data(command), static_cast<int>(error.extendedCmeError));
                break;
        }
    }
}
