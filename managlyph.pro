greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

HEADERS       = src/gui/anaglyph_widget.h \
                src/data/container.h \
                src/data/container_loader.h \
                src/data/frame.h \
                src/data/orbital_builder.h \
                src/data/orbitals/edgetable.h \
                src/data/orbitals/factorial.h \
                src/data/orbitals/integrator.h \
                src/data/orbitals/isosurface.h \
                src/data/orbitals/isosurface_mesh.h \
                src/data/orbitals/laguerre.h \
                src/data/orbitals/legendre.h \
                src/data/orbitals/scalar_field.h \
                src/data/orbitals/triangletable.h \
                src/data/orbitals/wavefunction.h \
                src/gui/interface_window.h \
                src/gui/logwindow.h \
                src/gui/mainwindow.h \
                src/gui/orbital_widget.h \
                src/gui/shader_program_types.h \
                src/gui/shader_program.h \
                src/gui/shader_program_manager.h \
                src/gui/structure_renderer.h \
                src/gui/periodic_table.h \
                src/gui/scene.h \
                src/data/atom.h \
                src/data/bond.h \
                src/data/atom_settings.h \
                src/data/model.h \
                src/data/model_loader.h \
                src/data/structure.h \
                src/config.h \
                src/managlyphapplication.h

SOURCES       = src/main.cpp \
                src/data/container.cpp \
                src/data/container_loader.cpp \
                src/data/frame.cpp \
                src/data/orbital_builder.cpp \
                src/data/orbitals/factorial.cpp \
                src/data/orbitals/integrator.cpp \
                src/data/orbitals/isosurface.cpp \
                src/data/orbitals/isosurface_mesh.cpp \
                src/data/orbitals/laguerre.cpp \
                src/data/orbitals/legendre.cpp \
                src/data/orbitals/scalar_field.cpp \
                src/data/orbitals/wavefunction.cpp \
                src/gui/anaglyph_widget.cpp \
                src/gui/interface_window.cpp \
                src/gui/logwindow.cpp \
                src/gui/mainwindow.cpp \
                src/gui/orbital_widget.cpp \
                src/gui/structure_renderer.cpp \
                src/gui/periodic_table.cpp \
                src/gui/shader_program.cpp \
                src/gui/shader_program_manager.cpp \
                src/gui/scene.cpp \
                src/data/atom_settings.cpp \
                src/data/atom.cpp \
                src/data/bond.cpp \
                src/data/model.cpp \
                src/data/model_loader.cpp \
                src/data/structure.cpp \
                src/managlyphapplication.cpp

QT           += core gui widgets charts
CONFIG       += force_debug_info

win32 {
    DEFINES += _USE_MATH_DEFINES
    INCLUDEPATH += src/vendor/glm-1.0.1
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

unix {
    DEFINES += _USE_MATH_DEFINES
    INCLUDEPATH += src/vendor/glm-1.0.1
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

RESOURCES += resources.qrc

DISTFILES +=
