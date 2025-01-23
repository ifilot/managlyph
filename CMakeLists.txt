cmake_minimum_required(VERSION 3.16)

# Set the project name
project(managlyph LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# get Git HASH
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_compile_definitions(GIT_HASH="${GIT_HASH}")

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

add_executable(managlyph
    src/main.cpp
    src/data/container.cpp
    src/data/container_loader.cpp
    src/data/frame.cpp
    src/data/orbital_builder.cpp
    src/data/orbitals/factorial.cpp
    src/data/orbitals/integrator.cpp
    src/data/orbitals/isosurface.cpp
    src/data/orbitals/isosurface_mesh.cpp
    src/data/orbitals/laguerre.cpp
    src/data/orbitals/legendre.cpp
    src/data/orbitals/scalar_field.cpp
    src/data/orbitals/wavefunction.cpp
    src/gui/anaglyph_widget.cpp
    src/gui/interface_window.cpp
    src/gui/logwindow.cpp
    src/gui/mainwindow.cpp
    src/gui/orbital_widget.cpp
    src/gui/structure_renderer.cpp
    src/gui/periodic_table.cpp
    src/gui/shader_program.cpp
    src/gui/shader_program_manager.cpp
    src/gui/scene.cpp
    src/data/atom_settings.cpp
    src/data/atom.cpp
    src/data/bond.cpp
    src/data/model.cpp
    src/data/model_loader.cpp
    src/data/structure.cpp
    src/managlyphapplication.cpp
    resources.qrc
)

include_directories("src/vendor/eigen-3.4.0"
                    "src/vendor/glm-1.0.1")

target_link_libraries(managlyph PRIVATE Qt6::Core Qt6::Widgets Qt6::Charts)

set_target_properties(managlyph PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)