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

#include <QWidget>
#include <QSlider>
#include <QBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QStyle>
#include <QKeyEvent>
#include <QPushButton>
#include <QApplication>
#include <QMessageBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QSplitter>
#include <QInputDialog>
#include <algorithm>

#include "anaglyph_widget.h"
#include "mainwindow.h"
#include "../data/orbital_builder.h"
#include "orbital_widget.h"
#include "../data/model.h"
#include "../data/container_loader.h"

QT_BEGIN_NAMESPACE
class QSlider;
class QPushButton;
QT_END_NAMESPACE

class MainWindow; // forward declaration to avoid circular dependencies

class InterfaceWindow : public QWidget {
    Q_OBJECT

private:
    MainWindow *mainWindow;

    QPushButton *description_button;
    QTextEdit *description_textfield = nullptr;

    // control interface
    QWidget *control_interface;
    QPushButton *btn_toggle_axis;
    QPushButton *btn_rotate_model;
    bool flag_rotate = false;
    bool flag_axes = true;

    AnaglyphWidget *anaglyph_widget;

    // sequence interface
    QWidget *frame_player;
    int cur_frame = 0;
    int max_frame = 1;
    QLabel *frame_label;
    QSlider *frame_slider;
    QPushButton *btn_frame_next;
    QPushButton *btn_frame_prev;
    QPushButton *btn_frame_play;
    QPushButton *btn_frame_pause;
    QTimer *frame_timer;
    QTimer *rotation_timer;


    OrbitalBuilder orbbuilder;
    OrbitalWidget *orbital_widget;

    ContainerLoader conload;

    std::shared_ptr<Container> container;

public:
    /**
     * @brief      Constructs the object.
     *
     * @param      mw    pointer to MainWindow object
     */
    InterfaceWindow(MainWindow *mw);

    /**
     * @brief Toggles visualization of orbital rendering menu
     */
    inline void toggle_orbital_menu_visible() {
        this->orbital_widget->setVisible(!this->orbital_widget->isVisible());
    }

    /**
     * @brief      Get pointer to anaglyph widget
     *
     * @return     The anaglyph widget.
     */
    inline AnaglyphWidget* get_anaglyph_widget() {
        return this->anaglyph_widget;
    }

private:
    void seqFrameChanged();

    /**
     * @brief Build interface to control the scene: rotate, toggle axis
     */
    void build_control_interface();

    /**
     * @brief Build buttons to move the frame
     */
    void build_sequence_interface();

protected:
    /**
     * @brief      Button press event
     *
     * @param      event  The event
     */
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

public slots:
    /**
     * @brief      Opens a file.
     *
     * @param[in]  filename  The filename
     *
     * @return     loading time of object in seconds
     */
    void open_file(const QString& filename);

    /**
     * @brief      Sets the camera align.
     *
     * @param      action  The action
     */
    void set_camera_align(QAction* action);

    /**
     * @brief      Sets the camera mode (orthogonal or perspective).
     *
     * @param      action  The action
     */
    void set_camera_mode(QAction* action);

    /**
     * @brief Build orbital
     * @param Orbital identifier
     */
    void build_orbital(int orbid);

private slots:
    /**
     * @brief      Loads a default structure file.
     */
    void load_default_file();

    /**
     * @brief      Propagate a message to the statusbar
     *
     * @param[in]  message  The message
     */
    void propagate_message_statusbar(const QString& message) {
        emit(signal_message_statusbar(message));
    }

    /**
     * @brief      Handler for the description button
     */
    void handle_description_button();

    /**
     * @brief Handle slider click
     */
    void frame_slider_clicked();

    /**
     * @brief Handle slider movement
     * @param val
     */
    void frame_slider_moved(int val);

    /**
     * @brief Handle pressing play button
     */
    void frame_timer_play();

    /**
     * @brief Handle pressing pause button
     */
    void frame_timer_pause();

    /**
     * @brief Handle timer trigger
     */
    void frame_timer_trigger();

    /**
     * @brief handle_frame_update
     */
    void handle_frame_update();

    /**
     * @brief Go one frame backward
     */
    void frame_backward();

    /**
     * @brief Go one frame forward
     */
    void frame_forward();

    /**
     * @brief Handle when a new file gets loaded
     */
    void handle_new_container();

    /**
     * @brief Sets whether or not to rotate the model
     */
    void toggle_rotation();

    /**
     * @brief Sets whether to display coordinate axes
     */
    void toggle_axes();

    /**
     * @brief Handles rotation timer
     */
    void rotation_timer_trigger();

signals:
    /**
     * @brief Signal to indicate that a new container has been loaded
     */
    void new_container_loaded();

    /**
     * @brief      Message statusbar
     *
     * @param[in]  message  The message
     */
    void signal_message_statusbar(const QString& message);

    /**
     * @brief Triggered when a frame gets updated
     */
    void frame_number_update();
};
