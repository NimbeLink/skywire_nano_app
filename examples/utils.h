/**
 * \file
 *
 * \brief Provides a few basic utilities
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

#include <cstdint>
#include <string>

#include "nimbelink/sdk/secure_services/at.h"

namespace NimbeLink::Examples::Utils
{
    /**
     * \brief Gets a container from its item
     *
     * \param *item
     *      The item whose container to get
     *
     * \return struct Container *
     *      The container of this item
     */
    template <typename C, typename T, T C::* M>
    static C *GetContainer(T *item)
    {
        uintptr_t _item = (uintptr_t)item;

        uintptr_t _containerStart = reinterpret_cast<uintptr_t>(reinterpret_cast<C *>(0));
        uintptr_t _itemStart = reinterpret_cast<uintptr_t>(&(reinterpret_cast<C *>(0)->*M));

        uintptr_t _container = _item - (_itemStart - _containerStart);

        return reinterpret_cast<C *>(_container);
    }

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
