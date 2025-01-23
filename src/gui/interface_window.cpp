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
#include "interface_window.h"

/**
 * @brief      Constructs the object.
 *
 * @param      mw    pointer to MainWindow object
 */
InterfaceWindow::InterfaceWindow(MainWindow *mw)
    : mainWindow(mw) {

    // build main Anaglyph Window
    qDebug() << "Build anaglyph widget";
    this->anaglyph_widget = new AnaglyphWidget();

    // build widget to visualize orbitals
    qDebug() << "Build orbital widget";
    this->orbital_widget = new OrbitalWidget(this);

    // set layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *container = new QHBoxLayout;
    container->addWidget(this->anaglyph_widget);
    this->anaglyph_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->addWidget(this->orbital_widget);
    this->orbital_widget->setVisible(false);

    // add sliders and anaglyph renderer widget
    QWidget *w = new QWidget;
    w->setLayout(container);
    mainLayout->addWidget(w);
    this->setLayout(mainLayout);

    // place buttons to move frame
    this->build_sequence_interface();
    mainLayout->addWidget(this->frame_player);

    // place buttons to
    this->build_control_interface();
    mainLayout->addWidget(this->control_interface);

    // add description button to horizontal buttons
    this->description_button = new QPushButton(tr("Show Description"));
    mainLayout->addWidget(this->description_button);
    this->description_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(this->description_button, SIGNAL (released()), this, SLOT (handle_description_button()));

    // add description screen
    this->description_textfield = new QTextEdit();
    mainLayout->addWidget(this->description_textfield);
    this->description_textfield->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    //this->description_textfield->setText(this->anaglyph_widget->get_model_description());
    this->description_textfield->hide();
    this->description_textfield->setReadOnly(true);

    // connect functions
    connect(this->anaglyph_widget, SIGNAL(opengl_ready()), this, SLOT(load_default_file()));
}

/**
 * @brief Build interface to control the scene: rotate, toggle axis
 */
void InterfaceWindow::build_control_interface() {
    // Initialization and timer for sequence
    this->control_interface = new QWidget();

    // horizontal buttons
    QBoxLayout *horizontalBox = new QHBoxLayout;
    this->control_interface->setLayout(horizontalBox);

    // add toggle axes button to horizontal row
    this->btn_toggle_axis = new QPushButton;
    this->btn_toggle_axis->setIcon(QIcon(":/assets/icon/axes_32.png"));
    this->btn_toggle_axis->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->btn_toggle_axis->setToolTip(QString("Toggle coordination axes in viewport."));
    horizontalBox->addWidget(this->btn_toggle_axis);
    connect(this->btn_toggle_axis, SIGNAL(clicked()), this, SLOT(toggle_axes()));

    // add toggle axes button to horizontal row
    this->btn_rotate_model = new QPushButton;
    this->btn_rotate_model->setIcon(QIcon(":/assets/icon/rotation_gray_32.png"));
    this->btn_rotate_model->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->btn_rotate_model->setToolTip(QString("Toggle rotation of model around z-axis."));
    horizontalBox->addWidget(this->btn_rotate_model);
    connect(this->btn_rotate_model, SIGNAL(clicked()), this, SLOT(toggle_rotation()));

    // rotation timer
    this->rotation_timer = new QTimer(this);
    connect(this->rotation_timer, SIGNAL(timeout()), this, SLOT(rotation_timer_trigger()));

    QFrame *frame = new QFrame;
    frame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    horizontalBox->addWidget(frame);
}

/**
 * @brief Build buttons to move the frame
 */
void InterfaceWindow::build_sequence_interface() {
    // Initialization and timer for sequence
    this->frame_player = new QWidget();

    // horizontal buttons
    QBoxLayout *horizontal_box = new QHBoxLayout;
    this->frame_player->setLayout(horizontal_box);

    // add sequence play button to horizontal row
    this->btn_frame_play = new QPushButton;
    this->btn_frame_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    this->btn_frame_play->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontal_box->addWidget(this->btn_frame_play);

    // add sequence pause button to horizontal row
    this->btn_frame_pause = new QPushButton;
    this->btn_frame_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    this->btn_frame_pause->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontal_box->addWidget(this->btn_frame_pause);

    // add frame duration to horizontal row
    this->frame_label = new QLabel(QString::number(cur_frame) + "/" + QString::number(max_frame));
    horizontal_box->addWidget(this->frame_label);

    // add sequence media slider to horizontal row
    this->frame_slider = new QSlider(Qt::Horizontal);
    horizontal_box->addWidget(this->frame_slider);

    // add prevframe button
    this->btn_frame_prev = new QPushButton;
    this->btn_frame_prev->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    this->btn_frame_prev->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontal_box->addWidget(this->btn_frame_prev);

    // add nextframe button
    this->btn_frame_next = new QPushButton;
    this->btn_frame_next->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    this->btn_frame_next->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontal_box->addWidget(this->btn_frame_next);

    this->frame_timer= new QTimer(this);

    // connect interface events
    connect(this->frame_timer, SIGNAL(timeout()), this, SLOT(frame_timer_trigger()));
    connect(this->btn_frame_play, SIGNAL(clicked()), this, SLOT(frame_timer_play()));
    connect(this->btn_frame_pause, SIGNAL(clicked()), this, SLOT(frame_timer_pause()));
    connect(this->btn_frame_prev, SIGNAL(clicked()), this, SLOT(frame_backward()));
    connect(this->btn_frame_next, SIGNAL(clicked()), this, SLOT(frame_forward()));
    connect(this, SIGNAL(frame_number_update()), this, SLOT(handle_frame_update()));
    connect(this->frame_slider, SIGNAL(valueChanged(int)), this, SLOT(frame_slider_moved(int)));
    connect(this->frame_slider, SIGNAL(sliderPressed()), this, SLOT(frame_slider_clicked()));

    // connect other events
    connect(this, SIGNAL(new_container_loaded()), this, SLOT(handle_new_container()));
}

/**
 * @brief      Button press event
 *
 * @param      event  The event
 */
void InterfaceWindow::keyPressEvent(QKeyEvent *e) {
    QWidget::keyPressEvent(e);
}

/**
 * @brief      Opens a file.
 *
 * @param[in]  filename  The filename
 *
 * @return     loading time of object in seconds
 */
void InterfaceWindow::open_file(const QString& filename) {
    qDebug() << "Opening file: " << filename;

    try {
        this->container = this->conload.load_data_abo(filename.toStdString());
    } catch(const std::exception& e) {
        QMessageBox::critical(this, tr("Exception encountered"), tr(e.what()) );
        return;
    }

    emit new_container_loaded();
}

/**
 * @brief      Sets the camera align.
 *
 * @param      action  The action
 */
void InterfaceWindow::set_camera_align(QAction* action) {
    this->anaglyph_widget->set_camera_alignment(action->data().toInt());
}

/**
 * @brief      Sets the camera mode (orthogonal or perspective).
 *
 * @param      action  The action
 */
void InterfaceWindow::set_camera_mode(QAction* action) {
    this->anaglyph_widget->set_camera_mode(action->data().toInt());
}

/**
 * @brief Build orbital
 * @param Orbital identifier
 */
void InterfaceWindow::build_orbital(int orbid) {
    // deconvolve quantum numbers
    unsigned int n = (orbid >> 8) & (0b1111);
    unsigned int l = (orbid >> 4) & (0b1111);
    int m = ((orbid) & (0b1111)) - l;
    qDebug() << "Building atomic orbital: (" << n << "," << l << "," << m << ")";

    // build orbital models
    this->orbbuilder.build_orbital(n,l,m);
    auto posmodel = std::make_shared<Model>(this->orbbuilder.get_pos()->get_vertices(),
                                            this->orbbuilder.get_pos()->get_normals(),
                                            this->orbbuilder.get_pos()->get_indices());
    auto negmodel = std::make_shared<Model>(this->orbbuilder.get_neg()->get_vertices(),
                                            this->orbbuilder.get_neg()->get_normals(),
                                            this->orbbuilder.get_neg()->get_indices());

    auto orbcon = std::make_shared<Container>();
    auto frame = std::make_shared<Frame>(std::make_shared<Structure>(), "Atomic orbital");
    frame->get_structure()->add_atom(1,0,0,0);

    // colorizing and adding model for positive lobe (if it exists)
    if(this->orbbuilder.get_num_pos_vertices() > 0) {
        posmodel->set_color(QVector4D(59.2f, 79.6f, 36.9f, 100.0f) / 100.0f);
        frame->add_model(posmodel);
    }

    // colorizing and adding model for negative lobe (if it exists)
    if(this->orbbuilder.get_num_neg_vertices() > 0) {
        negmodel->set_color(QVector4D(83.1f, 32.2f, 60.4f, 100.0f) / 100.0f);
        frame->add_model(negmodel);
    }

    // add frame to container
    orbcon->add_frame(frame);

    this->container = orbcon;
    emit new_container_loaded();
}

/**
 * @brief      Handler for the description button
 */
void InterfaceWindow::handle_description_button() {
    if(this->description_textfield->isHidden()) {
        this->description_textfield->show();
        this->description_button->setText("Hide Description");
    } else {
        this->description_textfield->hide();
        this->description_button->setText("Show Description");
    }
}

/**
 * @brief      Action for clicking the slider
 */
void InterfaceWindow::frame_slider_clicked() {
    this->frame_timer->stop();
}

/**
 * @brief      Action for moving the slider
 */
void InterfaceWindow::frame_slider_moved(int val) {
    this->cur_frame= val;
    emit frame_number_update();
}

/**
 * @brief Handle pressing play button
 */
void InterfaceWindow::frame_timer_play() {
    //this->frame_timer->setInterval(1000 / 60.0);
    this->frame_timer->setSingleShot(false);
    this->frame_timer->start(1000 / 60.0);
}

/**
 * @brief Handle pressing pause button
 */
void InterfaceWindow::frame_timer_pause() {
    this->frame_timer->stop();
}

/**
 * @brief Handle timer trigger
 */
void InterfaceWindow::frame_timer_trigger() {
    this->cur_frame++;
    emit frame_number_update();
}

/**
 * @brief handle_frame_update
 */
void InterfaceWindow::handle_frame_update() {
    if(this->cur_frame >= this->max_frame) {
        this->cur_frame = 0;
    }

    if(this->cur_frame < 0) {
        this->cur_frame = 0;
    }

    // update panel
    QString str = QString::number(this->cur_frame + 1) + "/" + QString::number(this->max_frame);
    this->frame_label->setText(str);
    this->frame_slider->setTickInterval(1);
    this->frame_slider->setSliderPosition(this->cur_frame);
    this->frame_slider->setRange(0, this->max_frame - 1);
    this->description_textfield->setText(this->container->frame(this->cur_frame)->get_description().c_str());

    this->anaglyph_widget->set_frame(this->container->frame(this->cur_frame));
}

/**
 * @brief Go one frame backward
 */
void InterfaceWindow::frame_backward() {
    this->cur_frame--;
    qDebug() << "Step frame back: Current frame is now: " << this->cur_frame;
    emit frame_number_update();
}

/**
 * @brief Go one frame forward
 */
void InterfaceWindow::frame_forward() {
    this->cur_frame++;
    qDebug() << "Step frame back: Current frame is now: " << this->cur_frame;
    emit frame_number_update();
}

/**
 * @brief Handle when a new file gets loaded
 */
void InterfaceWindow::handle_new_container() {
    this->cur_frame = 0;
    this->max_frame = this->container->get_nr_frames();
    emit frame_number_update();

    if(this->container->get_nr_frames() > 1) {
        this->btn_frame_play->setEnabled(true);
        this->btn_frame_pause->setEnabled(true);
        this->frame_label->show();
        this->frame_slider->setEnabled(true);
    } else {
        this->btn_frame_play->setEnabled(false);
        this->btn_frame_pause->setEnabled(false);
        this->frame_label->hide();
        this->frame_slider->setEnabled(false);
    }

    float maxdist = this->container->get_max_dim();
    qDebug() << "Parsing new frame to Anaglyph Widget class";
    qDebug() << "Max distance: " << maxdist;
    this->anaglyph_widget->set_camera_zoom(std::max(20.f, maxdist * 2.0f));
    this->anaglyph_widget->set_frame(this->container->frame(this->cur_frame));
}

/**
 * @brief Sets whether or not to rotate the model
 */
void InterfaceWindow::toggle_rotation() {
    this->flag_rotate = !this->flag_rotate;

    if(flag_rotate) {
        qDebug() << "Enabling rotation";
        this->rotation_timer->start(1000 / 60);
    } else {
        qDebug() << "Disabling rotation";
        this->rotation_timer->stop();
    }
}

/**
 * @brief Sets whether to display coordinate axes
 */
void InterfaceWindow::toggle_axes() {
    this->flag_axes = !this->flag_axes;
    this->anaglyph_widget->set_show_axes(this->flag_axes);
}

/**
 * @brief Handles rotation timer
 */
void InterfaceWindow::rotation_timer_trigger() {
    this->anaglyph_widget->rotate_scene(1);
}

/**
 * @brief      Loads a default structure file.
 */
void InterfaceWindow::load_default_file() {
    // do not load default file if a file is already loaded (via CLI)
    if(this->container) {
        return;
    }

    qDebug() << "Creating custom molecule";

    const QString filename = "C8H8.abo";
    QTemporaryDir tmp_dir;
    QFile::copy(":/assets/containers/" + filename, tmp_dir.path() + "/" + filename);
    this->open_file(tmp_dir.path() + "/" + filename);
}
