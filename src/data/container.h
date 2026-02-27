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

#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>
#include <memory>
#include <exception>

#include "frame.h"

class Container
{
private:
    std::vector<std::shared_ptr<Frame>> frames;
    bool flag_is_neb_pathway = false;

public:
    Container();

    inline void add_frame(const std::shared_ptr<Frame> frame) {
        this->frames.push_back(frame);
    }

    inline std::shared_ptr<Frame> frame(unsigned int frame_id) {
        if(frame_id >= frames.size()) {
            throw std::runtime_error("Invalid frame id: " + std::to_string(frame_id)
                                     + "/" + std::to_string(frames.size()));
        }
        return this->frames[frame_id];
    }

    inline size_t get_nr_frames() const {
        return this->frames.size();
    }

    inline void set_is_neb_pathway(bool is_neb_pathway) {
        this->flag_is_neb_pathway = is_neb_pathway;
    }

    inline bool is_neb_pathway() const {
        return this->flag_is_neb_pathway;
    }

    /**
     * @brief Get maximum dimension of all objects
     * @return Maximal dimension
     */
    float get_max_dim() const;
};

#endif // CONTAINER_H
