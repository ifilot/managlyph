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

#include "orbital_widget.h"

/**
 * @brief      Default constructor
 *
 * @param      _interfacewindow  The interfacewindow
 * @param      parent            The parent
 */
OrbitalWidget::OrbitalWidget(InterfaceWindow *_interfacewindow, QWidget *parent) :
    QWidget(parent),
    interface_window(_interfacewindow) {

    this->setMinimumWidth(180);
    this->setMaximumWidth(250);

    // create parent layout of this window
    QVBoxLayout *parentLayout = new QVBoxLayout;
    this->setLayout(parentLayout);

    // create scroll area and add it to the parentLayout
    QScrollArea *scrollarea = new QScrollArea;
    parentLayout->addWidget(new QLabel("<b>Atomic orbitals</b>"));
    parentLayout->addWidget(scrollarea);

    QWidget *scrollWidget = new QWidget(scrollarea);
    scrollarea->setWidget(scrollWidget);
    scrollarea->setWidgetResizable(true);
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
    scrollWidget->setLayout(scrollLayout);

    // add grid widget
    QWidget *orbital_selector = new QWidget;
    QGridLayout *orbital_selector_grid = new QGridLayout;
    orbital_selector->setLayout(orbital_selector_grid);
    scrollLayout->addWidget(orbital_selector);

    // create orbital labels
    orbital_selector_grid->addWidget(new QLabel(tr("1s")), 0, 0);
    orbital_selector_grid->addWidget(this->create_line(), 1, 0);

    orbital_selector_grid->addWidget(new QLabel(tr("2s")), 2, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("2p<sub>x</sub>")), 3, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("2p<sub>y</sub>")), 4, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("2p<sub>z</sub>")), 5, 0);
    orbital_selector_grid->addWidget(this->create_line(), 6, 0);

    orbital_selector_grid->addWidget(new QLabel(tr("3s")), 7, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("3p<sub>x</sub>")), 8, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("3p<sub>y</sub>")), 9, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("3p<sub>z</sub>")), 10, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("3d<sub>xy</sub>")), 11, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("3d<sub>xz</sub>")), 12, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("3d<sub>yz</sub>")), 13, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("3d<sub>x2-y2</sub>")), 14, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("3d<sub>z2</sub>")), 15, 0);
    orbital_selector_grid->addWidget(this->create_line(), 16, 0);

    orbital_selector_grid->addWidget(new QLabel(tr("4s")), 17, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4p<sub>x</sub>")), 18, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4p<sub>y</sub>")), 19, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4p<sub>z</sub>")), 20, 0);

    orbital_selector_grid->addWidget(new QLabel(tr("4d<sub>xy</sub>")), 21, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4d<sub>xz</sub>")), 22, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4d<sub>yz</sub>")), 23, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4d<sub>x2-y2</sub>")), 24, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4d<sub>z2</sub>")), 25, 0);

    orbital_selector_grid->addWidget(new QLabel(tr("4f<sub>y(3x2-y2)</sub>")), 26, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4f<sub>xyz</sub>")), 27, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4f<sub>y(5z2-r2)</sub>")), 28, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4f<sub>z(5z2-3r2)</sub>")), 29, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4f<sub>x(5z2-r2)</sub>")), 30, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4f<sub>z(x2-y2)</sub>")), 31, 0);
    orbital_selector_grid->addWidget(new QLabel(tr("4f<sub>x(x2-3y2)</sub>")), 32, 0);

    std::vector<int> magnetic_moments = {0,  0,1,-1,0,  0,1,-1,0,-2,1,-1,2,0,  0,1,-1,0,-2,1,-1,2,0,-3,-2,-1,0,1,2,3};
    std::vector<int> orbital_ids;
    unsigned int ctr = 0;
    for(int n=1; n<5; n++) {
        for(int l=0; l<n; l++) {
            for(int m=-l; m<=l; m++) {
                int orbidx = (n << 8) | (l << 4) | (magnetic_moments[ctr] + l);
                orbital_ids.push_back(orbidx);
                ctr++;
            }
        }
    }

    // the QPushButton needs to be mapped to a function that requires an argument. The best way
    // to do this is using a functor, however, because of a forward declaration, the specification
    // of the InterfaceWindow object is not known. Thus, the other option is to use a
    // QSignalMapper which allows me to pass a single argument to the SLOT function. I thus wrap the orbital data
    // inside a single signed integer using bitshift operations.
    QSignalMapper *m_sigmapper = new QSignalMapper(this);
    ctr = 0;
    unsigned int i =0;
    for(int n=1; n<5; n++) {
        for(int l=0; l<n; l++) {
            for(int m=-l; m<=l; m++) {
                this->orbital_build_buttons.push_back(new QPushButton("+"));
                this->orbital_build_buttons.back()->setFixedSize(QSize(20,20));
                orbital_selector_grid->addWidget(this->orbital_build_buttons[i], ctr, 1);
                m_sigmapper->setMapping(this->orbital_build_buttons[i], orbital_ids[i]);
                connect(this->orbital_build_buttons[i], SIGNAL (released()), m_sigmapper, SLOT(map()));
                ctr++;
                i++;
            }
        }
        ctr++;
    }
    connect(m_sigmapper, SIGNAL(mappedInt(int)), (QWidget*)this->interface_window, SLOT(build_orbital(int)));
}

/**
 * @brief      Create a single line
 *
 * @return     pointer to QFrame element
 */
QFrame* OrbitalWidget::create_line() {
    auto line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}
