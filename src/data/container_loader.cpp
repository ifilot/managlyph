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
    qDebug() << "Start reading abo file: " << path.c_str();

    auto container = std::make_shared<Container>();

    std::ifstream file(path, std::ios::binary);

    if(file.is_open()) {
        uint16_t nr_frames = 0;

        // read number of frames
        file.read((char*)&nr_frames, sizeof(uint16_t));
        qDebug() << "Number of frames: " << nr_frames;
        for(unsigned int i=0; i<nr_frames; i++) {
            // read frame id
            uint16_t frame_idx = 0;
            file.read((char*)&frame_idx, sizeof(uint16_t));
            qDebug() << "Frame idx: " << frame_idx;

            // read description
            uint16_t descriptor_length = 0;
            file.read((char*)&descriptor_length, sizeof(uint16_t));
            std::vector<char> buffer(descriptor_length);
            file.read(&buffer[0], descriptor_length);
            std::string description(buffer.begin(), buffer.end());
            //qDebug() << "Description: " << description.c_str();

            // read nr atoms
            uint16_t nr_atoms = 0;
            file.read((char*)&nr_atoms, sizeof(uint16_t));
            qDebug() << "Number of atoms: " << nr_atoms;

            // read atoms and positions
            std::vector<uint8_t> elements(nr_atoms);
            std::vector<glm::vec3> positions(nr_atoms);
            for(unsigned int j=0; j<nr_atoms; j++) {
                file.read((char*)&elements[j], sizeof(uint8_t));
                file.read((char*)&positions[j][0], 3 * sizeof(float));
            }

            // create structure and populate with atoms
            auto structure = std::make_shared<Structure>();
            for(unsigned int i=0; i<nr_atoms; i++) {
                structure->add_atom(elements[i], positions[i][0], positions[i][1], positions[i][2]);
            }
            structure->update(); // generate bonds

            // create frame
            auto frame = std::make_shared<Frame>(structure, description);

            // read nr_models
            uint16_t nr_models = 0;
            file.read((char*)&nr_models, sizeof(uint16_t));
            qDebug() << "Number of models: " << nr_models;

            // read data models
            for(unsigned int j=0; j<nr_models; j++) {
                // model idx
                uint16_t model_idx = 0;
                file.read((char*)&model_idx, sizeof(uint16_t));

                // model color
                QVector4D color;
                file.read((char*)&color[0], 4 * sizeof(float));

                // number of vertices / normals
                uint32_t nr_vertices = 0;
                file.read((char*)&nr_vertices, sizeof(uint32_t));
                // qDebug() << "Model idx : " << model_idx << " vertices: " << nr_vertices;

                // read positions and normals
                std::vector<glm::vec3> positions(nr_vertices);
                std::vector<glm::vec3> normals(nr_vertices);
                for(unsigned int k=0; k<nr_vertices; k++) {
                    file.read((char*)&positions[k][0], 3 * sizeof(float));
                    file.read((char*)&normals[k][0], 3 * sizeof(float));
                }

                // number of indices
                uint32_t nr_faces = 0;
                file.read((char*)&nr_faces, sizeof(uint32_t));

                // read indices
                std::vector<uint32_t> indices(nr_faces * 3);
                file.read((char*)&indices[0], nr_faces * 3 * sizeof(uint32_t));
                qDebug() << "Model idx : " << model_idx << " faces: " << nr_faces;

                // add to models
                auto model = std::make_shared<Model>(positions, normals, indices);
                model->set_color(color);
                frame->add_model(model);
            } // done looping over models

            // insert frame into container
            container->add_frame(frame);

        } // done looping over frames

        file.close();
    } else {
        throw std::runtime_error("Could not open file: " + path);
    }

    return container;
}
