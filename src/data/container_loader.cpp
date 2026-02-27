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
#include <limits>
#include <optional>
#include <sstream>
#include <vector>
#include <zstd.h>

/**
 * @brief ContainerLoader constructor
 */
ContainerLoader::ContainerLoader() {}

namespace {

constexpr unsigned int NEB_INTERPOLATION_STEPS_PER_SEGMENT = 10;
constexpr uint8_t FRAME_UNIT_CELL_FLAG_BIT = 0x01;

using UnitCellMatrix = std::array<float, 9>;

glm::vec3 catmull_rom(const glm::vec3& p0,
                      const glm::vec3& p1,
                      const glm::vec3& p2,
                      const glm::vec3& p3,
                      float t) {
    const float t2 = t * t;
    const float t3 = t2 * t;
    return 0.5f * ((2.0f * p1) +
                   (-p0 + p2) * t +
                   (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}


} // namespace

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

    auto read_or_throw = [&](std::istream& stream, char* data, std::size_t size) {
        if (size == 0) return;
        stream.read(data, size);
        if (!stream)
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
    read_or_throw(file, reinterpret_cast<char*>(&nr_frames), sizeof(nr_frames));

    NormalEncoding normal_encoding = NormalEncoding::Float32;
    std::unique_ptr<std::istringstream> payload_stream;
    std::string payload_data;
    bool is_neb_pathway = false;
    uint8_t abof_version = 0;
    if (nr_frames == 0) {
        std::array<char, 4> magic{};
        read_or_throw(file, magic.data(), magic.size());
        if (std::string(magic.data(), magic.size()) != "ABOF")
            throw std::runtime_error("Unsupported ABO file header: " + path);

        uint8_t version = 0;
        uint8_t flags = 0;
        read_or_throw(file, reinterpret_cast<char*>(&version), sizeof(version));
        read_or_throw(file, reinterpret_cast<char*>(&flags), sizeof(flags));
        if (version != 1 && version != 2)
            throw std::runtime_error("Unsupported ABO format version: " + std::to_string(version));
        abof_version = version;

        const bool is_compressed = (flags & 0x01) != 0;
        is_neb_pathway = (flags & 0x02) != 0;
        if (is_compressed) {
            std::vector<char> compressed((std::istreambuf_iterator<char>(file)),
                                         std::istreambuf_iterator<char>());
            if (compressed.empty())
                throw std::runtime_error("Corrupt ABO file (missing compressed payload): " + path);

            unsigned long long content_size =
                ZSTD_getFrameContentSize(compressed.data(), compressed.size());
            if (content_size == ZSTD_CONTENTSIZE_ERROR)
                throw std::runtime_error("Corrupt ABO file (invalid zstd payload): " + path);

            std::vector<char> decompressed;
            if (content_size != ZSTD_CONTENTSIZE_UNKNOWN) {
                decompressed.resize(static_cast<size_t>(content_size));
                size_t result = ZSTD_decompress(
                    decompressed.data(), decompressed.size(),
                    compressed.data(), compressed.size());
                if (ZSTD_isError(result))
                    throw std::runtime_error(
                        std::string("Failed to decompress ABO payload: ") +
                        ZSTD_getErrorName(result));
                decompressed.resize(result);
            } else {
                ZSTD_DStream* dstream = ZSTD_createDStream();
                if (!dstream)
                    throw std::runtime_error("Failed to allocate zstd decompressor");
                size_t init_result = ZSTD_initDStream(dstream);
                if (ZSTD_isError(init_result)) {
                    ZSTD_freeDStream(dstream);
                    throw std::runtime_error(
                        std::string("Failed to init zstd decompressor: ") +
                        ZSTD_getErrorName(init_result));
                }

                std::vector<char> out_buffer(ZSTD_DStreamOutSize());
                ZSTD_inBuffer input{compressed.data(), compressed.size(), 0};
                while (input.pos < input.size) {
                    ZSTD_outBuffer output{out_buffer.data(), out_buffer.size(), 0};
                    size_t const ret = ZSTD_decompressStream(dstream, &output, &input);
                    if (ZSTD_isError(ret)) {
                        ZSTD_freeDStream(dstream);
                        throw std::runtime_error(
                            std::string("Failed to stream-decompress ABO payload: ") +
                            ZSTD_getErrorName(ret));
                    }
                    decompressed.insert(decompressed.end(),
                                        out_buffer.data(),
                                        out_buffer.data() + output.pos);
                }
                ZSTD_freeDStream(dstream);
            }

            payload_data.assign(decompressed.begin(), decompressed.end());
            payload_stream = std::make_unique<std::istringstream>(payload_data, std::ios::binary);
        }

        normal_encoding = NormalEncoding::Oct16;
        std::istream& input = payload_stream ? static_cast<std::istream&>(*payload_stream)
                                             : static_cast<std::istream&>(file);
        read_or_throw(input, reinterpret_cast<char*>(&nr_frames), sizeof(nr_frames));
        qDebug() << "ABOF header detected. Version:" << version << "flags:" << flags;
    }

    qDebug() << "Number of frames:" << nr_frames;

    std::vector<std::shared_ptr<Frame>> loaded_frames;
    loaded_frames.reserve(nr_frames);

    std::istream& input = payload_stream ? static_cast<std::istream&>(*payload_stream)
                                         : static_cast<std::istream&>(file);
    for (uint16_t f = 0; f < nr_frames; ++f) {
        uint16_t frame_idx = 0;
        read_or_throw(input, reinterpret_cast<char*>(&frame_idx), sizeof(frame_idx));
        qDebug() << "Frame idx:" << frame_idx;

        // ---- Description ----
        uint16_t descriptor_length = 0;
        read_or_throw(input, reinterpret_cast<char*>(&descriptor_length), sizeof(descriptor_length));

        std::string description;
        if (descriptor_length > 0) {
            description.resize(descriptor_length);
            read_or_throw(input, description.data(), descriptor_length);
        }

        std::optional<UnitCellMatrix> frame_unit_cell;
        if (abof_version == 2) {
            uint8_t frame_flags = 0;
            read_or_throw(input, reinterpret_cast<char*>(&frame_flags), sizeof(frame_flags));
            if ((frame_flags & FRAME_UNIT_CELL_FLAG_BIT) != 0) {
                UnitCellMatrix unit_cell{};
                read_or_throw(input,
                              reinterpret_cast<char*>(unit_cell.data()),
                              unit_cell.size() * sizeof(float));
                frame_unit_cell = unit_cell;
            }
        }

        // ---- Atoms ----
        uint16_t nr_atoms = 0;
        read_or_throw(input, reinterpret_cast<char*>(&nr_atoms), sizeof(nr_atoms));
        qDebug() << "Number of atoms:" << nr_atoms;

        std::vector<uint8_t> elements(nr_atoms);
        std::vector<glm::vec3> positions(nr_atoms);

        for (uint16_t j = 0; j < nr_atoms; ++j) {
            read_or_throw(input, reinterpret_cast<char*>(&elements[j]), sizeof(uint8_t));
            read_or_throw(input, reinterpret_cast<char*>(&positions[j][0]), 3 * sizeof(float));
        }

        auto structure = std::make_shared<Structure>();
        for (uint16_t j = 0; j < nr_atoms; ++j)
            structure->add_atom(elements[j], positions[j].x, positions[j].y, positions[j].z);

        structure->update();

        auto frame = std::make_shared<Frame>(structure, description);
        frame->set_unit_cell(frame_unit_cell);

        // ---- Models ----
        uint16_t nr_models = 0;
        read_or_throw(input, reinterpret_cast<char*>(&nr_models), sizeof(nr_models));
        qDebug() << "Number of models:" << nr_models;

        for (uint16_t m = 0; m < nr_models; ++m) {
            uint16_t model_idx = 0;
            read_or_throw(input, reinterpret_cast<char*>(&model_idx), sizeof(model_idx));

            QVector4D color;
            read_or_throw(input, reinterpret_cast<char*>(&color[0]), 4 * sizeof(float));

            uint32_t nr_vertices = 0;
            read_or_throw(input, reinterpret_cast<char*>(&nr_vertices), sizeof(nr_vertices));

            std::vector<glm::vec3> v_positions(nr_vertices);
            std::vector<glm::vec3> normals(nr_vertices);

            for (uint32_t k = 0; k < nr_vertices; ++k) {
                read_or_throw(input, reinterpret_cast<char*>(&v_positions[k][0]), 3 * sizeof(float));
                if (normal_encoding == NormalEncoding::Float32) {
                    read_or_throw(input, reinterpret_cast<char*>(&normals[k][0]), 3 * sizeof(float));
                } else {
                    int16_t nx = 0;
                    int16_t ny = 0;
                    read_or_throw(input, reinterpret_cast<char*>(&nx), sizeof(nx));
                    read_or_throw(input, reinterpret_cast<char*>(&ny), sizeof(ny));
                    normals[k] = decode_octahedral_normal(nx, ny);
                }
            }

            uint32_t nr_faces = 0;
            read_or_throw(input, reinterpret_cast<char*>(&nr_faces), sizeof(nr_faces));

            std::vector<uint32_t> indices(nr_faces * 3);
            if (!indices.empty())
                read_or_throw(input, reinterpret_cast<char*>(indices.data()), indices.size() * sizeof(uint32_t));

            qDebug() << "Model idx:" << model_idx << "faces:" << nr_faces;

            if(nr_vertices == 0 || nr_faces == 0) {
                qDebug() << "Skipping empty model:" << model_idx;
            } else {
                auto model = std::make_shared<Model>(v_positions, normals, indices);
                model->set_color(color);
                frame->add_model(model);
            }
        }

        loaded_frames.push_back(frame);
    }

    container->set_is_neb_pathway(is_neb_pathway);

    if (is_neb_pathway && loaded_frames.size() >= 2) {
        const auto& reference_atoms = loaded_frames.front()->get_structure()->get_atoms();
        const size_t nr_atoms = reference_atoms.size();

        bool can_interpolate = nr_atoms > 0;
        for (size_t frame_idx = 1; frame_idx < loaded_frames.size() && can_interpolate; ++frame_idx) {
            const auto& atoms = loaded_frames[frame_idx]->get_structure()->get_atoms();
            if (atoms.size() != nr_atoms) {
                can_interpolate = false;
                break;
            }
            for (size_t atom_idx = 0; atom_idx < nr_atoms; ++atom_idx) {
                if (atoms[atom_idx].atnr != reference_atoms[atom_idx].atnr) {
                    can_interpolate = false;
                    break;
                }
            }
        }

        if (!can_interpolate) {
            qWarning() << "NEB interpolation skipped due to incompatible atom ordering between frames.";
        } else {
            std::vector<std::shared_ptr<Frame>> interpolated_frames;
            interpolated_frames.reserve((loaded_frames.size() - 1) * NEB_INTERPOLATION_STEPS_PER_SEGMENT + 1);

            const auto make_interpolated_frame = [&](size_t seg_idx, float t) {
                const size_t i0 = (seg_idx == 0) ? seg_idx : seg_idx - 1;
                const size_t i1 = seg_idx;
                const size_t i2 = seg_idx + 1;
                const size_t i3 = (seg_idx + 2 < loaded_frames.size()) ? seg_idx + 2 : loaded_frames.size() - 1;

                const auto& atoms0 = loaded_frames[i0]->get_structure()->get_atoms();
                const auto& atoms1 = loaded_frames[i1]->get_structure()->get_atoms();
                const auto& atoms2 = loaded_frames[i2]->get_structure()->get_atoms();
                const auto& atoms3 = loaded_frames[i3]->get_structure()->get_atoms();

                auto structure = std::make_shared<Structure>();
                for (size_t atom_idx = 0; atom_idx < nr_atoms; ++atom_idx) {
                    const glm::vec3 p0(atoms0[atom_idx].x, atoms0[atom_idx].y, atoms0[atom_idx].z);
                    const glm::vec3 p1(atoms1[atom_idx].x, atoms1[atom_idx].y, atoms1[atom_idx].z);
                    const glm::vec3 p2(atoms2[atom_idx].x, atoms2[atom_idx].y, atoms2[atom_idx].z);
                    const glm::vec3 p3(atoms3[atom_idx].x, atoms3[atom_idx].y, atoms3[atom_idx].z);

                    const glm::vec3 ipos = catmull_rom(p0, p1, p2, p3, t);
                    structure->add_atom(atoms1[atom_idx].atnr, ipos.x, ipos.y, ipos.z);
                }
                structure->update();

                std::ostringstream descriptor;
                descriptor.precision(std::numeric_limits<float>::max_digits10);
                descriptor << "NEB interpolated frame " << seg_idx << " t=" << t;
                return std::make_shared<Frame>(structure, descriptor.str());
            };

            for (size_t seg_idx = 0; seg_idx + 1 < loaded_frames.size(); ++seg_idx) {
                for (unsigned int step = 0; step < NEB_INTERPOLATION_STEPS_PER_SEGMENT; ++step) {
                    const float t = static_cast<float>(step) /
                                    static_cast<float>(NEB_INTERPOLATION_STEPS_PER_SEGMENT);
                    interpolated_frames.push_back(make_interpolated_frame(seg_idx, t));
                }
            }

            interpolated_frames.push_back(loaded_frames.back());
            loaded_frames = std::move(interpolated_frames);
            qDebug() << "Generated interpolated NEB frames:" << loaded_frames.size();
        }
    }

    for (const auto& frame : loaded_frames) {
        container->add_frame(frame);
    }

    return container;
}
