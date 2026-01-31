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

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QString>
#include <QtWidgets/QApplication>
#include <QFileInfo>
#include <QMimeData>
#include <QTimer>
#include <QStringList>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <QSettings>

#include "interface_window.h"
#include "lighting_settings_dialog.h"
#include "logwindow.h"

#include "../config.h"

class InterfaceWindow; // forward declaration to avoid circular dependencies

/**
 * @brief      Class for main window.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    InterfaceWindow* interface_window;
    QLabel* statusbar_projection_icon;
    QTimer* statusbar_timer;

    // storage for log messages
    std::shared_ptr<QStringList> log_messages;

    // window for log messages
    std::unique_ptr<LogWindow> log_window;
    std::unique_ptr<LightingSettingsDialog> lighting_settings_dialog;

public:
    /**
     * @brief      Constructs the object.
     */
    MainWindow(const std::shared_ptr<QStringList> _log_messages,
               QWidget *parent = nullptr);

    /**
     * @brief Parse command line arguments
     */
    void set_cli_parser(const QCommandLineParser& cli_parser);

protected:
    void moveEvent(QMoveEvent*) Q_DECL_OVERRIDE;

private slots:
    /**
     * @brief      Open a new object file
     */
    void open();

    /**
     * @brief      Close the application
     */
    void exit();

    /**
     * @brief      Display about menu
     */
    void about();

    /**
     * @brief      Set stereo projection
     */
    void set_stereo(QString fragment_shader);

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    /**
     * @brief      Show a message on the statusbar
     *
     * @param[in]  message  The message
     */
    void show_message_statusbar(const QString& message);

    /**
     * @brief      Clear statusbar when timer runs out
     */
    void statusbar_timeout();

    /**
     * @brief      Toggle display of orbitals menu
     */
    void toggle_orbitals_menu();

    /**
     * @brief Show an about window
     */
    void slot_debug_log();

    /**
     * @brief      Show lighting settings window
     */
    void show_lighting_settings();

private:
    /**
     * @brief      Loads a theme.
     */
    void load_theme();
};
