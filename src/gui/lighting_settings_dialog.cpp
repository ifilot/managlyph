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
    auto *grid = new QGridLayout();

    ambient_slider = new QSlider(Qt::Horizontal);
    ambient_slider->setRange(0, kFloatSliderMax);
    ambient_slider->setSingleStep(1);
    ambient_value = new QLabel();

    specular_slider = new QSlider(Qt::Horizontal);
    specular_slider->setRange(0, kFloatSliderMax);
    specular_slider->setSingleStep(1);
    specular_value = new QLabel();

    shininess_slider = new QSlider(Qt::Horizontal);
    shininess_slider->setRange(kShininessMin, kShininessMax);
    shininess_slider->setSingleStep(1);
    shininess_value = new QLabel();

    grid->addWidget(new QLabel(tr("Ambient strength")), 0, 0);
    grid->addWidget(ambient_slider, 0, 1);
    grid->addWidget(ambient_value, 0, 2);

    grid->addWidget(new QLabel(tr("Specular strength")), 1, 0);
    grid->addWidget(specular_slider, 1, 1);
    grid->addWidget(specular_value, 1, 2);

    grid->addWidget(new QLabel(tr("Shininess")), 2, 0);
    grid->addWidget(shininess_slider, 2, 1);
    grid->addWidget(shininess_value, 2, 2);

    layout->addLayout(grid);

    refresh_from_widget();

    connect(ambient_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
    connect(specular_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
    connect(shininess_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
}

void LightingSettingsDialog::sync_from_widget() {
    refresh_from_widget();
}

void LightingSettingsDialog::apply_settings() {
    if (!anaglyph_widget) {
        return;
    }

    float ambient_strength = ambient_slider->value() / 100.0f;
    float specular_strength = specular_slider->value() / 100.0f;
    float shininess = static_cast<float>(shininess_slider->value());

    anaglyph_widget->set_lighting_settings(ambient_strength, specular_strength, shininess);

    ambient_value->setText(to_percent_label(ambient_slider->value()));
    specular_value->setText(to_percent_label(specular_slider->value()));
    shininess_value->setText(QString::number(shininess_slider->value()));
}

void LightingSettingsDialog::refresh_from_widget() {
    if (!anaglyph_widget) {
        return;
    }

    LightingSettings settings = anaglyph_widget->get_lighting_settings();
    ambient_slider->setValue(static_cast<int>(settings.ambient_strength * 100.0f));
    specular_slider->setValue(static_cast<int>(settings.specular_strength * 100.0f));
    shininess_slider->setValue(static_cast<int>(settings.shininess));

    ambient_value->setText(QString::number(settings.ambient_strength, 'f', 2));
    specular_value->setText(QString::number(settings.specular_strength, 'f', 2));
    shininess_value->setText(QString::number(settings.shininess, 'f', 0));
}
