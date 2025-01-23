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

#ifndef _ISOSURFACE_MESH
#define _ISOSURFACE_MESH

#include <fstream>
#include <set>
#include <vector>
#include <unordered_map>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include "isosurface.h"

/**
 * @brief      structure to put glm::vec3 in a map
 */
struct KeyFuncs
{
    size_t operator()(const glm::vec3& k)const
    {
        return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
    }

    bool operator()(const glm::vec3& a, const glm::vec3& b)const
    {
            return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

/**
 * @brief      Class for iso surface mesh.
 */
class IsoSurfaceMesh{
private:
    std::unordered_map<glm::vec3, uint32_t, KeyFuncs, KeyFuncs> vertices_map;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<uint32_t> indices;

    const ScalarField* sf;
    const IsoSurface* is;

    glm::vec3 center;

public:
    /**
     * @brief      build isosurface mesh object
     *
     * @param[in]  _sf   pointer to scalar field
     * @param[in]  _is   pointer to isosurface
     */
    IsoSurfaceMesh(const ScalarField* _sf, const IsoSurface* _is);

    /**
     * @brief      construct surface mesh
     *
     * @param[in]  center  whether to center structure
     */
    void construct_mesh(bool center);

    inline const auto& get_vertices() const {
        return this->vertices;
    }

    inline const auto& get_normals() const {
        return this->normals;
    }

    inline const auto& get_indices() const {
        return this->indices;
    }

    inline size_t get_num_vertices() const {
        return this->vertices.size();
    }

private:
    /**
     * @brief      get the index of a vertex from unordered map
     *
     * @param[in]  v     vertex coordinates
     *
     * @return     the index
     */
    unsigned int get_index_vertex(const glm::vec3 v);

    /**
     * @brief      Calculates the normals from scalar field.
     */
    void calculate_normals_from_scalar_field();

    /**
     * @brief      Calculates the normals from polygons
     */
    void calculate_normals_from_polygons();

    /**
     * @brief      ensure indices are in CCW order with respect to face normals
     */
    void align_vertices_order_with_normals();
};

#endif //_ISOSURFACE_MESH
