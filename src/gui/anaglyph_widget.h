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

#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QString>
#include <QGuiApplication>
#include <QScreen>
#include <QSysInfo>
#include <QDebug>
#include <QTimer>
#include <QMenu>
#include <QtGlobal>

#include <QtCore/qmath.h>
#include <QtCore/qvariant.h>

#include <sstream>
#include <fstream>
#include <math.h>
#include <string>
#include <algorithm>

#include "shader_program_manager.h"
#include "shader_program_types.h"
#include "structure_renderer.h"
#include "scene.h"
#include "../data/frame.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

enum FrameBuffer {
    SILHOUETTE_NORMAL,
    SILHOUETTE_LEFT,
    SILHOUETTE_RIGHT,
    STRUCTURE_NORMAL,
    STRUCTURE_LEFT,
    STRUCTURE_RIGHT,
    ANAGLYPH_LEFT,
    ANAGLYPH_RIGHT,
    COORDINATE_AXES,

    NR_FRAMEBUFFERS
};

class AnaglyphWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

private:
    static constexpr int supersample_scale = 2;

    QPoint m_lastPos;

    QString root_path;

    QPoint top_left;

    // default background color
    static constexpr float tint = 235.0f / 255.0f;

    unsigned int framebuffers[FrameBuffer::NR_FRAMEBUFFERS];
    unsigned int texture_color_buffers[FrameBuffer::NR_FRAMEBUFFERS];
    unsigned int rbo[FrameBuffer::NR_FRAMEBUFFERS];

    QOpenGLVertexArrayObject quad_vao;
    QOpenGLVertexArrayObject quad_vao_small;
    QOpenGLBuffer quad_vbo;

    // used for arcball rotation
    bool arcball_rotation_flag = false;             // whether arcball rotation is active

    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo[4];

    bool flag_axis_enabled = true;                  // whether to draw coordinate axes

    // stereographic projections
    bool flag_stereographic_projection = false;     // whether stereographic rendering is used
    QString stereographic_type_name = "NONE";

    std::shared_ptr<Scene> scene;
    std::unique_ptr<StructureRenderer> structure_renderer;
    std::shared_ptr<ShaderProgramManager> shader_manager;
    std::shared_ptr<Frame> frame;

public:
    AnaglyphWidget(QWidget *parent = 0);

    /**
     * @brief      Paint the models in the models vector to the screen
     */
    void draw_structure();

    /**
     * @brief      Set a (new) structure
     *
     * @param[in]  _structure  The structure
     */
    void set_frame(const std::shared_ptr<Frame>& _frame);

    /**
     * @brief      Set a (new) structure
     *
     * Do not modify camera settings
     *
     * @param[in]  _structure  The structure
     */
    void set_frame_conservative(const std::shared_ptr<Frame>& _frame);

    /**
     * @brief      Gets the structure.
     *
     * @return     The structure.
     */
    Structure* get_structure() const {
        return this->frame->get_structure().get();
    }

    /**
     * @brief Release frame which also stops the rendering procedure
     */
    inline void release_frame() {
        this->frame.reset();
    }

    /**
     * @brief Update when moving window
     */
    void window_move_event();

    /**
     * @brief Set stereographic projection type
     * @param stereographic projection name
     */
    void set_stereo(QString stereo_name);

    /**
     * @brief      Set the minimum window size
     *
     * @return     Minimimum window size
     */
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    /**
     * @brief      Clean the widget
     */
    QSize sizeHint() const Q_DECL_OVERRIDE;

    /**
     * @brief Rotate the scene
     * @param angle
     */
    void rotate_scene(float angle);

    /**
     * @brief      Sets the camera alignment.
     *
     * @param[in]  direction  The direction
     */
    void set_camera_alignment(int direction);

    /**
     * @brief      Sets the camera mode.
     *
     * @param[in]  mode  The mode
     */
    void set_camera_mode(int mode);

    /**
     * @brief Sets whether to show axes or not
     * @param flag
     */
    inline void set_show_axes(bool flag) {
        this->flag_axis_enabled = flag;
        this->update();
    }

    /**
     * @brief Set the camera zoom level
     * @param v
     */
    inline void set_camera_zoom(float v) {
        this->scene->camera_position[1] = -v;
    }

    /**
     * @brief Set lighting settings for atoms and bonds.
     *
     * @param ambient
     * @param diffuse
     * @param specular
     * @param shininess
     */
    void set_atom_lighting_settings(float ambient,
                                    float diffuse,
                                    float specular,
                                    float shininess,
                                    float edge_strength,
                                    float edge_power);

    /**
     * @brief Set lighting settings for objects.
     *
     * @param ambient
     * @param diffuse
     * @param specular
     * @param shininess
     */
    void set_object_lighting_settings(float ambient,
                                      float diffuse,
                                      float specular,
                                      float shininess,
                                      float edge_strength,
                                      float edge_power);

    /**
     * @brief Get lighting settings for atoms and bonds.
     *
     * @return lighting settings
     */
    LightingSettings get_atom_lighting_settings() const;

    /**
     * @brief Get lighting settings for objects.
     *
     * @return lighting settings
     */
    LightingSettings get_object_lighting_settings() const;

    ~AnaglyphWidget();

public slots:
    /**
     * @brief      Clean the anaglyph class
     */
    void cleanup();

protected:
    /**
     * @brief      Initialize OpenGL environment
     */
    void initializeGL() Q_DECL_OVERRIDE;

    /**
     * @brief      Render scene
     */
    void paintGL() Q_DECL_OVERRIDE;

    /**
     * @brief      Resize window
     *
     * @param[in]  width   screen width
     * @param[in]  height  screen height
     */
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

    /**
     * @brief      Parse mouse press event
     *
     * @param      event  The event
     */
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief      Parse mouse release event
     *
     * @param      event  The event
     */
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief      Parse mouse move event
     *
     * @param      event  The event
     */
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief      Parse mouse wheel event
     *
     * @param      event  The event
     */
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

    /**
     * @brief      Calculate the arcball vector for mouse rotation
     *
     * @param[in]  x, y  The mouse position
     * @param[out] P  The arcball vector
     */
    QVector3D get_arcball_vector(int x, int y);

    /**
     * @brief      Set the arcball vector rotation (angle and vector) to model and updates
     *
     * @param      arcball_angle, arcball_vector
     */
    void set_arcball_rotation(float arcball_angle, const QVector4D& arcball_vector);

    /**
     * @brief Load lighting settings from persistent storage.
     */
    void load_lighting_settings();

private:
    /**
     * @brief      Build a canvas used for stereographic rendering
     */
    void build_framebuffers();

    /**
     * @brief      Load OpenGL shaders
     */
    void load_shaders();

    /**
     * @brief      Reset rotation matrices
     */
    void reset_matrices();

    /**
     * @brief      Regular draw call
     */
    void paint_regular();

    /**
     * @brief      Stereographic draw call
     */
    void paint_stereographic();

    /**
     * @brief Set viewport for offscreen rendering.
     */
    void set_render_viewport();

    /**
     * @brief Set viewport for onscreen rendering.
     */
    void set_screen_viewport();

    /**
     * @brief Get render target size for supersampling.
     */
    QSize render_size();

signals:
    /**
     * @brief      Send signal that opengl engine is ready
     */
    void opengl_ready();

    /**
     * Transmit message to status bar
     */
    void signal_message_statusbar(const QString& msg);
};
