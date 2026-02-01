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

#include "mainwindow.h"

/**
 * @brief      Class for main window.
 */
MainWindow::MainWindow(const std::shared_ptr<QStringList> _log_messages,
                       QWidget *parent)
    : QMainWindow(parent),
    log_messages(_log_messages) {

    qDebug() << "Constructing Main Window";

    // log window
    this->log_window = std::make_unique<LogWindow>(this->log_messages);

    // dropdown menu bar
    QMenuBar *menuBar = new QMenuBar;

    // add drop-down menus
    QMenu *menu_file = menuBar->addMenu(tr("&File"));
    QMenu *menu_view = menuBar->addMenu(tr("&View"));
    QMenu *menu_tools = menuBar->addMenu(tr("&Tools"));
    QMenu *menu_help = menuBar->addMenu(tr("&Help"));

    // actions for file menu
    QAction *action_open = new QAction(menu_file);
    QAction *action_quit = new QAction(menu_file);

    // actions for projection menu
    QMenu *menu_camera = new QMenu(tr("Camera"));

    // camera alignment
    QMenu *menu_camera_align = new QMenu(tr("Align"));
    QAction *action_camera_default = new QAction(menu_camera_align);
    QAction *action_camera_top = new QAction(menu_camera_align);
    QAction *action_camera_bottom = new QAction(menu_camera_align);
    QAction *action_camera_left = new QAction(menu_camera_align);
    QAction *action_camera_right = new QAction(menu_camera_align);
    QAction *action_camera_front = new QAction(menu_camera_align);
    QAction *action_camera_back = new QAction(menu_camera_align);

    // camera modes
    QMenu *menu_camera_mode = new QMenu(tr("Mode"));
    QAction *action_camera_perspective = new QAction(menu_camera_mode);
    QAction *action_camera_orthographic = new QAction(menu_camera_mode);

    // camera projections
    QMenu *menu_projection = new QMenu(tr("Projection"));
    QAction *action_projection_two_dimensional = new QAction(menu_projection);
    QAction *action_projection_anaglyph_red_cyan = new QAction(menu_projection);

    // interlaced projections
    QMenu *menu_projection_interlaced = new QMenu(tr("Interlaced"));
    QAction *action_projection_interlaced_rows_lr = new QAction(menu_projection_interlaced);
    QAction *action_projection_interlaced_rows_rl = new QAction(menu_projection_interlaced);
    QAction *action_projection_interlaced_columns_lr = new QAction(menu_projection_interlaced);
    QAction *action_projection_interlaced_columns_rl = new QAction(menu_projection_interlaced);
    QAction *action_projection_interlaced_checkerboard_lr = new QAction(menu_projection_interlaced);
    QAction *action_projection_interlaced_checkerboard_rl = new QAction(menu_projection_interlaced);

    // actions for tools menu
    QAction *action_view_orbitals_menu = new QAction(menu_tools);
    QAction *action_lighting_settings = new QAction(menu_tools);
    action_view_orbitals_menu->setText(tr("Orbitals menu"));
    action_lighting_settings->setText(tr("Lighting settings"));
    menu_tools->addAction(action_view_orbitals_menu);
    menu_tools->addAction(action_lighting_settings);

    // actions for help menu
    QAction *action_about = new QAction(menu_help);

    // debug log
    QAction *action_debug_log = new QAction(menu_help);
    action_debug_log->setText(tr("Debug Log"));
    action_debug_log ->setShortcut(Qt::Key_F2);
    menu_help->addAction(action_debug_log);
    connect(action_debug_log, &QAction::triggered, this, &MainWindow::slot_debug_log);

    // create actions for file menu
    action_open->setText(tr("Open"));
    action_open->setShortcuts(QKeySequence::Open);
    action_quit->setText(tr("Quit"));
    action_quit->setShortcuts(QKeySequence::Quit);
    action_quit->setShortcut(Qt::CTRL | Qt::Key_Q);

    // create actions for projection menu
    action_camera_default->setText(tr("Default"));
    action_camera_default->setData(QVariant((int)CameraAlignment::DEFAULT));
    action_camera_default->setShortcut(Qt::Key_0);
    action_camera_top->setText(tr("Top"));
    action_camera_top->setData(QVariant((int)CameraAlignment::TOP));
    action_camera_top->setShortcut(Qt::Key_7);
    action_camera_bottom->setText(tr("Bottom"));
    action_camera_bottom->setData(QVariant((int)CameraAlignment::BOTTOM));
    action_camera_bottom->setShortcut(Qt::CTRL | Qt::Key_7);
    action_camera_left->setText(tr("Left"));
    action_camera_left->setData(QVariant((int)CameraAlignment::LEFT));
    action_camera_left->setShortcut(Qt::Key_3);
    action_camera_right->setText(tr("Right"));
    action_camera_right->setData(QVariant((int)CameraAlignment::RIGHT));
    action_camera_right->setShortcut(Qt::CTRL | Qt::Key_3);
    action_camera_front->setText(tr("Front"));
    action_camera_front->setData(QVariant((int)CameraAlignment::FRONT));
    action_camera_front->setShortcut(Qt::Key_1);
    action_camera_back->setText(tr("Back"));
    action_camera_back->setData(QVariant((int)CameraAlignment::BACK));
    action_camera_back->setShortcut(Qt::CTRL | Qt::Key_1);
    action_camera_perspective->setText(tr("Perspective"));
    action_camera_perspective->setData(QVariant((int)CameraMode::PERSPECTIVE));
    action_camera_perspective->setShortcut(Qt::Key_5);
    action_camera_orthographic->setText(tr("Orthographic"));
    action_camera_orthographic->setData(QVariant((int)CameraMode::ORTHOGRAPHIC));
    action_camera_orthographic->setShortcut(Qt::CTRL | Qt::Key_5);
    action_projection_two_dimensional->setText(tr("Two-dimensional"));
    action_projection_anaglyph_red_cyan->setText(tr("Anaglyph (red/cyan)"));
    action_projection_interlaced_rows_lr->setText(tr("Interlaced rows (left first)"));
    action_projection_interlaced_rows_rl->setText(tr("Interlaced rows (right first)"));
    action_projection_interlaced_columns_lr->setText(tr("Interlaced columns (left first)"));
    action_projection_interlaced_columns_rl->setText(tr("Interlaced columns (right first)"));
    action_projection_interlaced_checkerboard_lr->setText(tr("Checkerboard (left first)"));
    action_projection_interlaced_checkerboard_rl->setText(tr("Checkerboard (right first)"));
    menu_projection_interlaced->setIcon(QIcon(":/assets/icon/interlaced_rows_lr_32.png"));
    action_projection_two_dimensional->setIcon(QIcon(":/assets/icon/two_dimensional_32.png"));
    action_projection_anaglyph_red_cyan->setIcon(QIcon(":/assets/icon/anaglyph_red_cyan_32.png"));
    action_projection_interlaced_rows_lr->setIcon(QIcon(":/assets/icon/interlaced_rows_lr_32.png"));
    action_projection_interlaced_rows_rl->setIcon(QIcon(":/assets/icon/interlaced_rows_rl_32.png"));
    action_projection_interlaced_columns_lr->setIcon(QIcon(":/assets/icon/interlaced_columns_lr_32.png"));
    action_projection_interlaced_columns_rl->setIcon(QIcon(":/assets/icon/interlaced_columns_rl_32.png"));
    action_projection_interlaced_checkerboard_lr->setIcon(QIcon(":/assets/icon/interlaced_checkerboard_lr_32.png"));
    action_projection_interlaced_checkerboard_rl->setIcon(QIcon(":/assets/icon/interlaced_checkerboard_rl_32.png"));

    // create actions for about menu
    action_about->setText(tr("About"));

    // add actions to file menu
    menu_file->addAction(action_open);
    menu_file->addAction(action_quit);

    // add actions to projection menu
    menu_view->addMenu(menu_projection);
    menu_view->addMenu(menu_camera);
    menu_camera->addMenu(menu_camera_align);
    menu_camera_align->addAction(action_camera_default);
    menu_camera_align->addAction(action_camera_top);
    menu_camera_align->addAction(action_camera_bottom);
    menu_camera_align->addAction(action_camera_left);
    menu_camera_align->addAction(action_camera_right);
    menu_camera_align->addAction(action_camera_front);
    menu_camera_align->addAction(action_camera_back);
    menu_camera->addMenu(menu_camera_mode);
    menu_camera_mode->addAction(action_camera_perspective);
    menu_camera_mode->addAction(action_camera_orthographic);
    menu_projection->addAction(action_projection_two_dimensional);
    menu_projection->addAction(action_projection_anaglyph_red_cyan);
    menu_projection->addMenu(menu_projection_interlaced);
    menu_projection_interlaced->addAction(action_projection_interlaced_rows_lr);
    menu_projection_interlaced->addAction(action_projection_interlaced_rows_rl);
    menu_projection_interlaced->addAction(action_projection_interlaced_columns_lr);
    menu_projection_interlaced->addAction(action_projection_interlaced_columns_rl);
    menu_projection_interlaced->addAction(action_projection_interlaced_checkerboard_lr);
    menu_projection_interlaced->addAction(action_projection_interlaced_checkerboard_rl);

    // add actions to help menu
    menu_help->addAction(action_about);

    // connect actions file menu
    connect(action_open, &QAction::triggered, this, &MainWindow::open);
    connect(action_quit, &QAction::triggered, this, &MainWindow::exit);

    // connect actions projection menu (note; [this]{} is the idiomatic way by providing a functor - "this is the way")
    connect(action_projection_two_dimensional, &QAction::triggered, this, [this]{ MainWindow::set_stereo("no_stereo_flat"); });
    connect(action_projection_anaglyph_red_cyan, &QAction::triggered, this, [this]{ MainWindow::set_stereo("stereo_anaglyph_red_cyan"); });
    connect(action_projection_interlaced_rows_lr, &QAction::triggered, this, [this]{ MainWindow::set_stereo("stereo_interlaced_rows_lr"); });
    connect(action_projection_interlaced_rows_rl, &QAction::triggered, this, [this]{ MainWindow::set_stereo("stereo_interlaced_rows_rl"); });
    connect(action_projection_interlaced_columns_lr, &QAction::triggered, this, [this]{ MainWindow::set_stereo("stereo_interlaced_columns_lr"); });
    connect(action_projection_interlaced_columns_rl, &QAction::triggered, this, [this]{ MainWindow::set_stereo("stereo_interlaced_columns_rl"); });
    connect(action_projection_interlaced_checkerboard_lr, &QAction::triggered, this, [this]{ MainWindow::set_stereo("stereo_interlaced_checkerboard_lr"); });
    connect(action_projection_interlaced_checkerboard_rl, &QAction::triggered, this, [this]{ MainWindow::set_stereo("stereo_interlaced_checkerboard_rl"); });

    // connect actions tools menu
    connect(action_view_orbitals_menu, &QAction::triggered, this, &MainWindow::toggle_orbitals_menu);
    connect(action_lighting_settings, &QAction::triggered, this, &MainWindow::show_lighting_settings);

    // connect actions about menu
    connect(action_about, &QAction::triggered, this, &MainWindow::about);

    setMenuBar(menuBar);
    this->interface_window = new InterfaceWindow(this);
    setCentralWidget(this->interface_window);

    // allow interface and anaglyph window to parse messages to statusbar
    connect(this->interface_window, SIGNAL(signal_message_statusbar(const QString&)), this, SLOT(show_message_statusbar(const QString&)));
    connect(this->interface_window->get_anaglyph_widget(), SIGNAL(signal_message_statusbar(const QString&)), this, SLOT(show_message_statusbar(const QString&)));

    // allow textmessages on the statusbar to disappear after some time
    this->statusbar_timer = new QTimer(this);
    connect(this->statusbar_timer, SIGNAL(timeout()), this, SLOT(statusbar_timeout()));

    // connect actions camera menu
    connect(menu_camera_align, SIGNAL(triggered(QAction*)), this->interface_window, SLOT(set_camera_align(QAction*)));
    connect(menu_camera_mode, SIGNAL(triggered(QAction*)), this->interface_window, SLOT(set_camera_mode(QAction*)));

    // add projection icon to status bar
    this->statusbar_projection_icon = new QLabel();
    this->statusbar_projection_icon->setFixedSize(16, 16);
    this->statusbar_projection_icon->setPixmap(QPixmap(":/assets/icon/two_dimensional_32.png").scaled(16, 16));
    statusBar()->addPermanentWidget(this->statusbar_projection_icon);

    // display status message
    statusBar()->showMessage(QString(PROGRAM_NAME) + " " + QString(PROGRAM_VERSION));
    this->statusbar_timer->start(1000);

    // set icon
    setWindowIcon(QIcon(":/assets/icon/managlyph_256.ico"));

    // allow dropping of data files
    setAcceptDrops(true);

    // set Window properties
    this->setWindowTitle(QString(PROGRAM_NAME) + " " + QString(PROGRAM_VERSION));
    this->resize(1280,960);

    qDebug() << "Done building MainWindow";
}

/**
 * @brief Parse command line arguments
 */
void MainWindow::set_cli_parser(const QCommandLineParser& cli_parser) {
    if(!cli_parser.value("o").isEmpty()) {
        QString filename = cli_parser.value("o");
        qDebug() << "Received CLI '-o': " << filename;
        this->interface_window->open_file(filename);
        statusBar()->showMessage("Loaded " + filename + ".");
        this->setWindowTitle(QFileInfo(filename).fileName() + " - " + QString(PROGRAM_NAME));
    }
}

/**
 * @brief      Open a new object file
 */
void MainWindow::open() {
    QSettings settings;

    // Default to last directory, or user's home directory
    const QString start_dir = settings.value(
        "ui/lastOpenDir",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
    ).toString();

    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open file"),
        start_dir,
        tr("Atom Bond Object files (*abo);;")
    );

    if(filename.isEmpty()) {
        return;
    }

    // Remember directory for next time
    settings.setValue(
        "ui/lastOpenDir",
        QFileInfo(filename).absolutePath()
    );

    // display load time
    this->interface_window->open_file(filename);
    statusBar()->showMessage("Loaded " + filename + ".");

    // set main window title
    this->setWindowTitle(QFileInfo(filename).fileName() + " - " + QString(PROGRAM_NAME));
}

/**
 * @brief      Close the application
 */
void MainWindow::exit() {
    QMessageBox msgBox;
    msgBox.setText("Exit program.");
    msgBox.setStyleSheet("QLabel{min-width: 350px; font-weight: normal;}");
    msgBox.setInformativeText("Are you sure you want to quit?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setWindowIcon(QIcon(":/assets/icon/managlyph_256.ico"));
    int ret = msgBox.exec();

    switch (ret) {
      case QMessageBox::Ok:
          QApplication::quit();
          return;
      case QMessageBox::Cancel:
          return;
      default:
          // should never be reached
          return;
    }
}

/**
 * @brief      Display about menu
 */
void MainWindow::about() {
    QMessageBox message_box;

    message_box.setWindowTitle("About " PROGRAM_NAME);
    message_box.setIcon(QMessageBox::Information);
    message_box.setIconPixmap(QPixmap(":/assets/icon/managlyph_256.ico"));

    message_box.setTextFormat(Qt::RichText);
    message_box.setTextInteractionFlags(Qt::TextBrowserInteraction);
    message_box.setTextInteractionFlags(Qt::TextBrowserInteraction);

    QString text =
        "<b>" PROGRAM_NAME "</b> version " PROGRAM_VERSION "<br><br>"

        "<b>Author</b><br>"
        "Ivo Filot &lt;i.a.w.filot@tue.nl&gt;<br><br>"

        "<b>License</b><br>"
        PROGRAM_NAME " is free software released under the "
        "GNU General Public License v3.0 (GPL-3.0).<br><br>"

        "This software uses the Qt framework, which is available under the "
        "GNU Lesser General Public License v3.0 (LGPL-3.0).<br><br>"

        "<b>Sponsorship</b><br>"
        "This project has been co-financed through the TU/e Boost program.";

    message_box.setText(text);

    QString info =
        "<p align='center'>"
        "<img src=':/assets/icon/tue-logo.png' width='140'><br>"
        "<a href='https://drive.tue.nl/projects/3d-visualization-with-artificial-intelligence-and-projection/'>"
        "Project sponsorship details"
        "</a></p>";

    message_box.setInformativeText(info);

    message_box.setStyleSheet(
        "QLabel { min-width: 320px; font-weight: normal; }"
    );

    message_box.exec();
}

/**
 * @brief      Set stereo projection
 */
void MainWindow::set_stereo(QString stereo_name) {
    this->interface_window->get_anaglyph_widget()->set_stereo(stereo_name);

    // set icon
    QString icon_name;
    if (stereo_name.startsWith("stereo")) {
        icon_name = ":/assets/icon/" + stereo_name.replace("stereo_", "") + "_32.png";
    } else {
        icon_name = ":/assets/icon/two_dimensional_32.png";
    }

    QPixmap pixmap(icon_name);
    this->statusbar_projection_icon->setPixmap(pixmap.scaled(16, 16));
}

/**
 * @brief      Handle windows move event
 *
 * Updates anaglyph window
 *
 * @param      event  MoveEvent
 */
void MainWindow::moveEvent(QMoveEvent* event) {
    this->interface_window->get_anaglyph_widget()->window_move_event();

    // this is just to silence a warning message, we are not using event here, but this is the callback form
    (void)event;
}

/**
 * @brief      Handles drag Enter event
 *
 * @param      event  The event
 */
void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

/**
 * @brief      Handles file drop event
 *
 * @param      event  The event
 */
void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QString text;
        for (int i = 0; i < urlList.size() && i < 32; ++i) {
            QString url = urlList.at(i).path();

            #ifdef Q_OS_WIN
            // remove leading / on Windows path; this sometimes causes problems
            QFileInfo check_file_win(url);
            if(!check_file_win.exists()) {
                if(url[0] == '/') {
                    url = url.remove(0,1);
                }
            }
            #endif

            // check if file exists, else show error message
            QFileInfo check_file(url);
            if(check_file.exists() && check_file.isFile()) {
                this->interface_window->open_file(url);
            } else {
                QMessageBox::critical(this, tr("Failed to load file"), tr("Could not load file. Did you try to load this file from a network drive? This is not supported.") );
                statusBar()->showMessage("Error loading file.");
                return;
            }
        }

        // show message after loading
        if(urlList.size() == 1) {
            statusBar()->showMessage("Loaded " + urlList.at(0).path() + ".");
        } else {
            statusBar()->showMessage("Loaded files.");
        }
    } else {
        statusBar()->showMessage("Could not identify dropped format. Ignoring...");
    }
}

/**
 * @brief      Handles drag move event
 *
 * @param      event  The event
 */
void MainWindow::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

/**
 * @brief      Handles event when object is dragged outside window
 *
 * @param      event  The event
 */
void MainWindow::dragLeaveEvent(QDragLeaveEvent *event) {
    event->accept();
}

/**
 * @brief      Clear statusbar when timer runs out
 */
void MainWindow::statusbar_timeout() {
    statusBar()->showMessage("");
}

/**
 * @brief      Toggle display of orbitals menu
 */
void MainWindow::toggle_orbitals_menu() {
    this->interface_window->toggle_orbital_menu_visible();
}

/**
 * @brief      Show lighting settings window
 */
void MainWindow::show_lighting_settings() {
    if (!this->lighting_settings_dialog) {
        this->lighting_settings_dialog = std::make_unique<LightingSettingsDialog>(
            this->interface_window->get_anaglyph_widget(),
            this
        );
    }

    this->lighting_settings_dialog->sync_from_widget();
    this->lighting_settings_dialog->show();
    this->lighting_settings_dialog->raise();
    this->lighting_settings_dialog->activateWindow();
}

/**
 * @brief      Show a message on the statusbar
 *
 * @param[in]  message  The message
 */
void MainWindow::show_message_statusbar(const QString& message) {
    statusBar()->showMessage(message);
    this->statusbar_timer->start(1000);
}

/**
 * @brief Show an about window
 */
void MainWindow::slot_debug_log() {
    this->log_window->show();
}
