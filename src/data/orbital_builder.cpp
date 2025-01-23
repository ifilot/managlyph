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

#include "orbital_builder.h"

OrbitalBuilder::OrbitalBuilder() {

}

void OrbitalBuilder::build_orbital(int n, int l, int m) {
    // Constructing wave function and checking that these are normalized
    qDebug() << "Performing internal integrity checks...";
    double dummy = 0.0;
    WaveFunction wf(n,l,m);
    Integrator in(&wf);

    // perform some sanity checks on internal routines
    double radint = in.integrate_radial_density(0, 200, 1e5, &dummy);   // relative high number of points necessary for high principle quantum number orbitals
    if(!(std::abs(radint - 1.0) < 1e-8)) {
        throw std::runtime_error("Error. Radial wave function does not integrate up to unity. Sum: " + std::to_string(radint));
    }
    double angint = in.integrate_spherical_harmonic(0, M_PI, 5000); // high number of integration points is necessary for higher angular momentum orbitals
    if(!(std::abs(angint - 1.0) < 1e-8)) {
        throw std::runtime_error("Error. Angular wave function does not integrate up to unity. Sum: " + std::to_string(angint));
    }

    double r = 0; // reset r value
    double isovalue = in.find_isosurface_volume(0, 100, 1e5, &r);
    if(isovalue < 0.0) {
        isovalue *= -1.0;
    }

    unsigned int gridsize = 151;
    double resolution = std::ceil(r * 2.0) * 2.0 / (double)gridsize;
    ScalarField sf(gridsize, resolution);
    sf.load_wavefunction(wf, true); // second argument determines whether wavefunction is loaded in a signed fashion

    // analyze the grid and generate the isosurface using the isovalue
    qDebug() << "Constructing isosurface";

    // build isosurfaces
    IsoSurface is_pos(&sf);
    IsoSurface is_neg(&sf);
    is_pos.marching_cubes(isovalue);
    is_neg.marching_cubes(-isovalue);

    this->pos = std::make_unique<IsoSurfaceMesh>(&sf, &is_pos);
    this->pos->construct_mesh(true);
    this->neg = std::make_unique<IsoSurfaceMesh>(&sf, &is_neg);
    this->neg->construct_mesh(true);
}
