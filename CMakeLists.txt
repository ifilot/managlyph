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

# Find Qt5 packages
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Use GLM
find_package(glm REQUIRED)

# Use Eigen
find_package(Eigen3 REQUIRED NO_MODULE)

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

target_link_libraries(managlyph PRIVATE 
    Qt5::Core 
    Qt5::Widgets
    Eigen3::Eigen
    glm::glm
)

set_target_properties(managlyph PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

if (WIN32)
    target_sources(managlyph PRIVATE managlyph.rc)
endif()