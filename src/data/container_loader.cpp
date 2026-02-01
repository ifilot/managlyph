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

#include "container_loader.h"

#include <array>
#include <cmath>

/**
 * @brief ContainerLoader constructor
 */
ContainerLoader::ContainerLoader() {}

/**
 * @brief      Loads an abo file from hard drive stored as little endian binary
 *
 * @param[in]  path   Path to file
 */
std::shared_ptr<Container> ContainerLoader::load_data_abo(const std::string& path) {
    qDebug() << "Start reading abo file:" << path.c_str();

    auto container = std::make_shared<Container>();
    std::ifstream file(path, std::ios::binary);

    if (!file)
        throw std::runtime_error("Could not open file: " + path);

    auto read_or_throw = [&](char* data, std::size_t size) {
        if (size == 0) return;
        file.read(data, size);
        if (!file)
            throw std::runtime_error("Corrupt ABO file (unexpected EOF): " + path);
    };

    enum class NormalEncoding {
        Float32,
        Oct16
    };

    auto decode_octahedral_normal = [](int16_t nx, int16_t ny) {
        constexpr float scale = 32767.0f;
        glm::vec3 n(static_cast<float>(nx) / scale,
                    static_cast<float>(ny) / scale,
                    0.0f);
        n.z = 1.0f - std::abs(n.x) - std::abs(n.y);
        if (n.z < 0.0f) {
            const float old_x = n.x;
            n.x = (1.0f - std::abs(n.y)) * (old_x >= 0.0f ? 1.0f : -1.0f);
            n.y = (1.0f - std::abs(old_x)) * (n.y >= 0.0f ? 1.0f : -1.0f);
        }
        const float length = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        if (length > 0.0f) {
            n /= length;
        }
        return n;
    };

    uint16_t nr_frames = 0;
    read_or_throw(reinterpret_cast<char*>(&nr_frames), sizeof(nr_frames));

    NormalEncoding normal_encoding = NormalEncoding::Float32;
    if (nr_frames == 0) {
        std::array<char, 4> magic{};
        read_or_throw(magic.data(), magic.size());
        if (std::string(magic.data(), magic.size()) != "ABOF")
            throw std::runtime_error("Unsupported ABO file header: " + path);

        uint8_t version = 0;
        uint8_t flags = 0;
        read_or_throw(reinterpret_cast<char*>(&version), sizeof(version));
        read_or_throw(reinterpret_cast<char*>(&flags), sizeof(flags));
        if (version != 1)
            throw std::runtime_error("Unsupported ABO format version: " + std::to_string(version));

        normal_encoding = NormalEncoding::Oct16;
        read_or_throw(reinterpret_cast<char*>(&nr_frames), sizeof(nr_frames));
        qDebug() << "ABOF header detected. Version:" << version << "flags:" << flags;
    }

    qDebug() << "Number of frames:" << nr_frames;

    for (uint16_t f = 0; f < nr_frames; ++f) {
        uint16_t frame_idx = 0;
        read_or_throw(reinterpret_cast<char*>(&frame_idx), sizeof(frame_idx));
        qDebug() << "Frame idx:" << frame_idx;

        // ---- Description ----
        uint16_t descriptor_length = 0;
        read_or_throw(reinterpret_cast<char*>(&descriptor_length), sizeof(descriptor_length));

        std::string description;
        if (descriptor_length > 0) {
            description.resize(descriptor_length);
            read_or_throw(description.data(), descriptor_length);
        }

        // ---- Atoms ----
        uint16_t nr_atoms = 0;
        read_or_throw(reinterpret_cast<char*>(&nr_atoms), sizeof(nr_atoms));
        qDebug() << "Number of atoms:" << nr_atoms;

        std::vector<uint8_t> elements(nr_atoms);
        std::vector<glm::vec3> positions(nr_atoms);

        for (uint16_t j = 0; j < nr_atoms; ++j) {
            read_or_throw(reinterpret_cast<char*>(&elements[j]), sizeof(uint8_t));
            read_or_throw(reinterpret_cast<char*>(&positions[j][0]), 3 * sizeof(float));
        }

        auto structure = std::make_shared<Structure>();
        for (uint16_t j = 0; j < nr_atoms; ++j)
            structure->add_atom(elements[j], positions[j].x, positions[j].y, positions[j].z);

        structure->update();

        auto frame = std::make_shared<Frame>(structure, description);

        // ---- Models ----
        uint16_t nr_models = 0;
        read_or_throw(reinterpret_cast<char*>(&nr_models), sizeof(nr_models));
        qDebug() << "Number of models:" << nr_models;

        for (uint16_t m = 0; m < nr_models; ++m) {
            uint16_t model_idx = 0;
            read_or_throw(reinterpret_cast<char*>(&model_idx), sizeof(model_idx));

            QVector4D color;
            read_or_throw(reinterpret_cast<char*>(&color[0]), 4 * sizeof(float));

            uint32_t nr_vertices = 0;
            read_or_throw(reinterpret_cast<char*>(&nr_vertices), sizeof(nr_vertices));

            std::vector<glm::vec3> v_positions(nr_vertices);
            std::vector<glm::vec3> normals(nr_vertices);

            for (uint32_t k = 0; k < nr_vertices; ++k) {
                read_or_throw(reinterpret_cast<char*>(&v_positions[k][0]), 3 * sizeof(float));
                if (normal_encoding == NormalEncoding::Float32) {
                    read_or_throw(reinterpret_cast<char*>(&normals[k][0]), 3 * sizeof(float));
                } else {
                    int16_t nx = 0;
                    int16_t ny = 0;
                    read_or_throw(reinterpret_cast<char*>(&nx), sizeof(nx));
                    read_or_throw(reinterpret_cast<char*>(&ny), sizeof(ny));
                    normals[k] = decode_octahedral_normal(nx, ny);
                }
            }

            uint32_t nr_faces = 0;
            read_or_throw(reinterpret_cast<char*>(&nr_faces), sizeof(nr_faces));

            std::vector<uint32_t> indices(nr_faces * 3);
            if (!indices.empty())
                read_or_throw(reinterpret_cast<char*>(indices.data()), indices.size() * sizeof(uint32_t));

            qDebug() << "Model idx:" << model_idx << "faces:" << nr_faces;

            if(nr_vertices == 0 || nr_faces == 0) {
                qDebug() << "Skipping empty model:" << model_idx;
            } else {
                auto model = std::make_shared<Model>(v_positions, normals, indices);
                model->set_color(color);
                frame->add_model(model);
            }
        }

        container->add_frame(frame);
    }

    return container;
}
