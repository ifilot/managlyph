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

    uint16_t nr_frames = 0;
    read_or_throw(reinterpret_cast<char*>(&nr_frames), sizeof(nr_frames));
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
                read_or_throw(reinterpret_cast<char*>(&normals[k][0]), 3 * sizeof(float));
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