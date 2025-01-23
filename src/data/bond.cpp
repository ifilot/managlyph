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

#include "bond.h"

Bond::Bond(const Atom& _atom1, const Atom& _atom2) :
atom1(_atom1),
atom2(_atom2) {
    auto v = this->atom2.get_pos_qtvec() - this->atom1.get_pos_qtvec();

    this->direction = v.normalized();
    this->length = v.length();

    // avoid gimball locking
    if (fabs(this->direction[2]) > .999) {
        if(this->direction[2] < 0.0) {
            this->axis = QVector3D(0.0, 1.0, 0.0);
            this->angle = -M_PI;
        } else {
            this->axis = QVector3D(0.0, 0.0, 1.0);
            this->angle = 0.0;
        }
    } else {
        this->axis = QVector3D::crossProduct(QVector3D(0.0, 0.0, 1.0), this->direction);
        this->angle = std::acos(this->direction[2]);
    }
}
