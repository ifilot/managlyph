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

#ifndef _ORBITAL_WIDGET
#define _ORBITAL_WIDGET

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QVector>
#include <QPushButton>
#include <QGridLayout>
#include <QSignalMapper>
#include <QScrollArea>

#include <iostream>

class InterfaceWindow; // forward declaration

class OrbitalWidget : public QWidget {
    Q_OBJECT

private:
    QVector<QPushButton*> orbital_build_buttons;
    InterfaceWindow* interface_window;

public:
    /**
     * @brief      Default constructor
     *
     * @param      _interfacewindow  The interfacewindow
     * @param      parent            The parent
     */
    OrbitalWidget(InterfaceWindow *_interfacewindow, QWidget *parent = 0);

private:
    /**
     * @brief      Create a single line
     *
     * @return     pointer to QFrame element
     */
    QFrame* create_line();

};

#endif // _ORBITAL_WIDGET
