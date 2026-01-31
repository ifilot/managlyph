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

#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

#include "anaglyph_widget.h"

class LightingSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit LightingSettingsDialog(AnaglyphWidget* anaglyph_widget, QWidget* parent = nullptr);
    void sync_from_widget();

private:
    struct LightingControls {
        QSlider* diffuse_slider;
        QSlider* ambient_slider;
        QSlider* specular_slider;
        QSlider* shininess_slider;
        QLabel* diffuse_value;
        QLabel* ambient_value;
        QLabel* specular_value;
        QLabel* shininess_value;
    };

    void apply_settings();
    void refresh_from_widget();
    void setup_controls(QGroupBox* group_box, LightingControls* controls);
    void update_labels(const LightingControls& controls);

    AnaglyphWidget* anaglyph_widget;

    LightingControls atom_controls;
    LightingControls object_controls;
};
