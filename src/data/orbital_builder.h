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

#include <QDebug>

#include "orbitals/scalar_field.h"
#include "orbitals/isosurface_mesh.h"
#include "orbitals/wavefunction.h"
#include "orbitals/integrator.h"

class OrbitalBuilder {
private:
    std::unique_ptr<IsoSurfaceMesh> pos;
    std::unique_ptr<IsoSurfaceMesh> neg;

public:
    OrbitalBuilder();

    void build_orbital(int n, int l, int m);

    inline size_t get_num_pos_vertices() const {
        return this->pos->get_num_vertices();
    }

    inline size_t get_num_neg_vertices() const {
        return this->neg->get_num_vertices();
    }

    inline const IsoSurfaceMesh* get_pos() const {
        return this->pos.get();
    }

    inline const IsoSurfaceMesh* get_neg() const {
        return this->neg.get();
    }

private:
};
