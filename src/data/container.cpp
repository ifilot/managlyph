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

#include "container.h"

Container::Container() {}

/**
 * @brief Get maximum dimension of all objects
 * @return Maximal dimension
 */
float Container::get_max_dim() const {
    float maxval = 0.0;
    for(const auto& frame : this->frames) {
        for(const auto& atom : frame->get_structure()->get_atoms()) {
            maxval = std::max(maxval, (float)atom.get_pos_qtvec().length());
        }

        for(const auto& model : frame->get_models()) {
            maxval = std::max(maxval, glm::length(model->get_max_dim()));
        }
    }

    return maxval;
}
