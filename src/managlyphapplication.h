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

#include <QApplication>
#include <QMessageBox>
#include <QIcon>

#include "config.h"

class AtomArchitectApplication : public QApplication
{
public:
    /**
     * @brief Default constructor
     * @param argc number of command line argument
     * @param argv command line arguments
     */
    AtomArchitectApplication(int& argc, char** argv);

    /**
     * @brief notify
     * @param receiver
     * @param event
     * @return
     */
    bool notify(QObject* receiver, QEvent* event);

private:
    void throw_message_window(const QString& title, const QString& message);
};
