/**************************************************************************
 *   This file is part of MANAGLYPH.                                      *
 *                                                                        *
 *   Author: Ivo Filot <ivo@ivofilot.nl>                                  *
 *                                                                        *
 *   MANAGLYPH is free software:                                          *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   MANAGLYPH is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#ifndef CONTAINER_LOADER_H
#define CONTAINER_LOADER_H

#include <memory>

#include "container.h"
#include "atom_settings.h"

class ContainerLoader
{
private:

public:
    /**
     * @brief ContainerLoader constructor
     */
    ContainerLoader();

    /**
     * @brief      Loads an abo file from hard drive stored as little endian binary
     *
     * @param[in]  path   Path to file
     */
    std::shared_ptr<Container> load_data_abo(const std::string& path);
};

#endif // CONTAINER_LOADER_H
