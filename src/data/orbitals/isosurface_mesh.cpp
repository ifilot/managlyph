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

#include "isosurface_mesh.h"

/**
 * @brief      build isosurface mesh object
 *
 * @param[in]  _sf   pointer to scalar field
 * @param[in]  _is   pointer to isosurface
 */
IsoSurfaceMesh::IsoSurfaceMesh(const ScalarField* _sf,
                               const IsoSurface* _is) :
    sf(_sf),
    is(_is) {
}

/**
 * @brief      construct surface mesh
 *
 * @param[in]  center  whether to center structure
 */
void IsoSurfaceMesh::construct_mesh(bool center) {
   // grab center
    this->center = this->sf->get_mat_unitcell() * glm::vec3(0.5, 0.5, 0.5);

    for(unsigned int i=0; i<this->is->get_triangles_ptr()->size(); i++) {
        this->texcoords.push_back(glm::vec2(0,0));
        this->texcoords.push_back(glm::vec2(0,1));
        this->texcoords.push_back(glm::vec2(1,0));

        this->indices.push_back(this->get_index_vertex(is->get_triangles_ptr()->at(i).p1));
        this->indices.push_back(this->get_index_vertex(is->get_triangles_ptr()->at(i).p2));
        this->indices.push_back(this->get_index_vertex(is->get_triangles_ptr()->at(i).p3));
    }

    // build vertex vector from unordered map
    this->vertices.resize(this->vertices_map.size());
    for(auto it : this->vertices_map) {
        this->vertices[it.second] = it.first;
    }

    // calculate vertex normals based on gradient of scalar field
    this->calculate_normals_from_polygons();

    // set order of vertex indices based on face normals
    this->align_vertices_order_with_normals();

    // center structure
    if(center) {
        glm::vec3 sum = this->sf->get_mat_unitcell() * glm::vec3(0.5f, 0.5f, 0.5f);

        #pragma omp parallel for
        for(unsigned int i=0; i<this->vertices.size(); i++) {
           this->vertices[i] -= sum;
        }
    }
}

/**
 * @brief      get the index of a vertex from unordered map
 *
 * @param[in]  v     vertex coordinates
 *
 * @return     the index
 */
unsigned int IsoSurfaceMesh::get_index_vertex(const glm::vec3 v) {
    auto got = this->vertices_map.find(v);
    if(got != this->vertices_map.end()) {
        return got->second;
    } else {
        this->vertices_map.emplace(v, this->vertices_map.size());
        return this->get_index_vertex(v);
    }
}

/**
 * @brief      Calculates the normals from scalar field.
 */
void IsoSurfaceMesh::calculate_normals_from_scalar_field() {
    static const double dev = 0.01;
    this->normals.resize(this->vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

    // calculate normal vectors for each vertex
    #pragma omp parallel for
    for(unsigned int i=0; i<this->vertices.size(); i++) {
        // get derivatives
        double dx0 = sf->get_value_interp(this->vertices[i][0] - 2.0 * dev, this->vertices[i][1], this->vertices[i][2]);
        double dx1 = sf->get_value_interp(this->vertices[i][0] - dev, this->vertices[i][1], this->vertices[i][2]);
        double dx2 = sf->get_value_interp(this->vertices[i][0] + dev, this->vertices[i][1], this->vertices[i][2]);
        double dx3 = sf->get_value_interp(this->vertices[i][0] + 2.0 * dev, this->vertices[i][1], this->vertices[i][2]);

        double dy0 = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1] - 2.0 * dev, this->vertices[i][2]);
        double dy1 = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1] - dev, this->vertices[i][2]);
        double dy2 = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1] + dev, this->vertices[i][2]);
        double dy3 = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1] + 2.0 * dev, this->vertices[i][2]);

        double dz0 = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1], this->vertices[i][2] - 2.0 * dev);
        double dz1 = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1], this->vertices[i][2] - dev);
        double dz2 = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1], this->vertices[i][2] + dev);
        double dz3 = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1], this->vertices[i][2] + 2.0 * dev);

        double cc = sf->get_value_interp(this->vertices[i][0], this->vertices[i][1], this->vertices[i][2]);

        // calculate gradient using 5 point stencil (note that center point is not used when calculating gradient)
        double dx = (dx0 - 8.0 * dx1 + 8.0 * dx2 - dx3) / (12.0 * dev);
        double dy = (dy0 - 8.0 * dy1 + 8.0 * dy2 - dy3) / (12.0 * dev);
        double dz = (dz0 - 8.0 * dz1 + 8.0 * dz2 - dz3) / (12.0 * dev);

        if(cc < 0) { // flip normal vector for negative isovalue
            this->normals[i] = glm::normalize(glm::vec3(dx, dy, dz));
        } else {
            this->normals[i] = glm::normalize(glm::vec3(-dx, -dy, -dz));
        }
    }
}

/**
 * @brief      Calculates the normals from polygons
 */
void IsoSurfaceMesh::calculate_normals_from_polygons() {
    this->normals.resize(this->vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));

    #pragma omp parallel for
    for(unsigned int i=0; i<this->indices.size(); i += 3) {
        const unsigned int idx1 = this->indices[i];
        const unsigned int idx2 = this->indices[i+1];
        const unsigned int idx3 = this->indices[i+2];

        const glm::vec3& v1 = this->vertices[idx1];
        const glm::vec3& v2 = this->vertices[idx2];
        const glm::vec3& v3 = this->vertices[idx3];

        float area = glm::length(glm::cross(v2 - v1, v3 - v1)) / 2.0f;
        glm::vec3 direction = glm::cross(v2 - v1, v3 - v1);

        if(area < 0.0) {
            std::cout << area << std::endl;
        }

        this->normals[idx1] += direction / area;
        this->normals[idx2] += direction / area;
        this->normals[idx3] += direction / area;
    }

    #pragma omp parallel for
    for(unsigned int i=0; i<this->normals.size(); i++) {
        if(this->is->get_isovalue() > 0.0) {
            this->normals[i] = glm::normalize(-this->normals[i]);
        } else {
            this->normals[i] = glm::normalize(this->normals[i]);
        }
    }
}

/**
 * @brief      ensure indices are in CCW order with respect to face normals
 */
void IsoSurfaceMesh::align_vertices_order_with_normals() {
    #pragma omp parallel for
    for(unsigned int i=0; i<this->indices.size(); i+=3) {
        glm::vec3 a = this->vertices[this->indices[i]];
        glm::vec3 b = this->vertices[this->indices[i+1]];
        glm::vec3 c = this->vertices[this->indices[i+2]];

        // calculate normal based on index order
        glm::vec3 norm = glm::normalize(glm::cross(a-b, c-b));

        // calculate face norm
        glm::vec3 facenorm = (this->normals[this->indices[i]] + this->normals[this->indices[i+1]] + this->normals[this->indices[i+2]]) / 3.0f;

        // flip the indices if the normals are not aligned
        if(glm::dot(norm, facenorm) > 0.0) {
            std::swap(this->indices[i+1], this->indices[i+2]);
        }
    }
}
