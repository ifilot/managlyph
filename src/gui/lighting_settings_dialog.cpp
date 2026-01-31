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

#include "lighting_settings_dialog.h"

#include <QSignalBlocker>

namespace {
constexpr int kFloatSliderMax = 100;
constexpr int kShininessMin = 1;
constexpr int kShininessMax = 256;

QString to_percent_label(int value) {
    return QString::number(value / 100.0f, 'f', 2);
}
}  // namespace

LightingSettingsDialog::LightingSettingsDialog(AnaglyphWidget* anaglyph_widget, QWidget* parent)
    : QDialog(parent),
      anaglyph_widget(anaglyph_widget) {
    setWindowTitle(tr("Lighting settings"));
    setModal(false);

    auto *layout = new QVBoxLayout(this);
    auto *atom_group = new QGroupBox(tr("Atoms & bonds"));
    auto *object_group = new QGroupBox(tr("Objects"));

    setup_controls(atom_group, &atom_controls);
    setup_controls(object_group, &object_controls);

    layout->addWidget(atom_group);
    layout->addWidget(object_group);

    refresh_from_widget();

    auto connect_controls = [this](const LightingControls& controls) {
        connect(controls.ambient_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
        connect(controls.specular_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
        connect(controls.shininess_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
    };

    connect_controls(atom_controls);
    connect_controls(object_controls);
}

void LightingSettingsDialog::sync_from_widget() {
    refresh_from_widget();
}

void LightingSettingsDialog::apply_settings() {
    if (!anaglyph_widget) {
        return;
    }

    float atom_ambient_strength = atom_controls.ambient_slider->value() / 100.0f;
    float atom_specular_strength = atom_controls.specular_slider->value() / 100.0f;
    float atom_shininess = static_cast<float>(atom_controls.shininess_slider->value());

    float object_ambient_strength = object_controls.ambient_slider->value() / 100.0f;
    float object_specular_strength = object_controls.specular_slider->value() / 100.0f;
    float object_shininess = static_cast<float>(object_controls.shininess_slider->value());

    anaglyph_widget->set_atom_lighting_settings(atom_ambient_strength, atom_specular_strength, atom_shininess);
    anaglyph_widget->set_object_lighting_settings(object_ambient_strength, object_specular_strength, object_shininess);

    update_labels(atom_controls);
    update_labels(object_controls);
}

void LightingSettingsDialog::refresh_from_widget() {
    if (!anaglyph_widget) {
        return;
    }

    const LightingSettings atom_settings = anaglyph_widget->get_atom_lighting_settings();
    const LightingSettings object_settings = anaglyph_widget->get_object_lighting_settings();

    QSignalBlocker atom_ambient_blocker(atom_controls.ambient_slider);
    QSignalBlocker atom_specular_blocker(atom_controls.specular_slider);
    QSignalBlocker atom_shininess_blocker(atom_controls.shininess_slider);
    atom_controls.ambient_slider->setValue(static_cast<int>(atom_settings.ambient_strength * 100.0f));
    atom_controls.specular_slider->setValue(static_cast<int>(atom_settings.specular_strength * 100.0f));
    atom_controls.shininess_slider->setValue(static_cast<int>(atom_settings.shininess));

    QSignalBlocker object_ambient_blocker(object_controls.ambient_slider);
    QSignalBlocker object_specular_blocker(object_controls.specular_slider);
    QSignalBlocker object_shininess_blocker(object_controls.shininess_slider);
    object_controls.ambient_slider->setValue(static_cast<int>(object_settings.ambient_strength * 100.0f));
    object_controls.specular_slider->setValue(static_cast<int>(object_settings.specular_strength * 100.0f));
    object_controls.shininess_slider->setValue(static_cast<int>(object_settings.shininess));

    update_labels(atom_controls);
    update_labels(object_controls);
}

void LightingSettingsDialog::setup_controls(QGroupBox* group_box, LightingControls* controls) {
    auto *grid = new QGridLayout(group_box);

    controls->ambient_slider = new QSlider(Qt::Horizontal);
    controls->ambient_slider->setRange(0, kFloatSliderMax);
    controls->ambient_slider->setSingleStep(1);
    controls->ambient_value = new QLabel();

    controls->specular_slider = new QSlider(Qt::Horizontal);
    controls->specular_slider->setRange(0, kFloatSliderMax);
    controls->specular_slider->setSingleStep(1);
    controls->specular_value = new QLabel();

    controls->shininess_slider = new QSlider(Qt::Horizontal);
    controls->shininess_slider->setRange(kShininessMin, kShininessMax);
    controls->shininess_slider->setSingleStep(1);
    controls->shininess_value = new QLabel();

    grid->addWidget(new QLabel(tr("Ambient strength")), 0, 0);
    grid->addWidget(controls->ambient_slider, 0, 1);
    grid->addWidget(controls->ambient_value, 0, 2);

    grid->addWidget(new QLabel(tr("Specular strength")), 1, 0);
    grid->addWidget(controls->specular_slider, 1, 1);
    grid->addWidget(controls->specular_value, 1, 2);

    grid->addWidget(new QLabel(tr("Shininess")), 2, 0);
    grid->addWidget(controls->shininess_slider, 2, 1);
    grid->addWidget(controls->shininess_value, 2, 2);
}

void LightingSettingsDialog::update_labels(const LightingControls& controls) {
    controls.ambient_value->setText(to_percent_label(controls.ambient_slider->value()));
    controls.specular_value->setText(to_percent_label(controls.specular_slider->value()));
    controls.shininess_value->setText(QString::number(controls.shininess_slider->value()));
}
