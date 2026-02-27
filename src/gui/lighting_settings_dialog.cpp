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
constexpr int kEdgeStrengthMax = 300;
constexpr int kEdgePowerMax = 600;

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
    auto *atom_group = new QGroupBox(tr("Atoms and bonds"));
    auto *object_group = new QGroupBox(tr("Objects (incl. orbitals)"));
    auto *rendering_group = new QGroupBox(tr("Rendering"));

    setup_controls(atom_group, &atom_controls);
    setup_controls(object_group, &object_controls);

    auto *rendering_grid = new QGridLayout(rendering_group);
    this->msaa_combo = new QComboBox();
    this->msaa_combo->addItem("1", 1);
    this->msaa_combo->addItem("2", 2);
    this->msaa_combo->addItem("4", 4);
    this->msaa_combo->addItem("8", 8);

    this->sphere_tesselation_spinbox = new QSpinBox();
    this->sphere_tesselation_spinbox->setRange(0, 6);

    this->reset_lighting_button = new QPushButton(tr("Reset lighting defaults"));

    rendering_grid->addWidget(new QLabel(tr("MSAA samples")), 0, 0);
    rendering_grid->addWidget(this->msaa_combo, 0, 1);
    rendering_grid->addWidget(new QLabel(tr("Sphere tesselation")), 1, 0);
    rendering_grid->addWidget(this->sphere_tesselation_spinbox, 1, 1);
    rendering_grid->addWidget(this->reset_lighting_button, 2, 0, 1, 2);

    layout->addWidget(atom_group);
    layout->addWidget(object_group);
    layout->addWidget(rendering_group);

    refresh_from_widget();

    auto connect_controls = [this](const LightingControls& controls) {
        connect(controls.ambient_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
        connect(controls.diffuse_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
        connect(controls.specular_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
        connect(controls.shininess_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
        connect(controls.edge_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
        connect(controls.edge_power_slider, &QSlider::valueChanged, this, [this]() { apply_settings(); });
    };

    connect_controls(atom_controls);
    connect_controls(object_controls);
    connect(this->msaa_combo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]() { apply_settings(); });
    connect(this->sphere_tesselation_spinbox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this]() { apply_settings(); });
    connect(this->reset_lighting_button, &QPushButton::clicked, this, [this]() {
        if (this->anaglyph_widget) {
            this->anaglyph_widget->reset_lighting_settings_to_defaults();
        }
        refresh_from_widget();
    });

    this->resize(640,480);
}

void LightingSettingsDialog::sync_from_widget() {
    refresh_from_widget();
}

void LightingSettingsDialog::apply_settings() {
    if (!anaglyph_widget) {
        return;
    }

    float atom_ambient_strength = atom_controls.ambient_slider->value() / 100.0f;
    float atom_diffuse_strength = atom_controls.diffuse_slider->value() / 100.0f;
    float atom_specular_strength = atom_controls.specular_slider->value() / 100.0f;
    float atom_shininess = static_cast<float>(atom_controls.shininess_slider->value());
    float atom_edge_strength = atom_controls.edge_slider->value() / 100.0f;
    float atom_edge_power = atom_controls.edge_power_slider->value() / 100.0f;

    float object_ambient_strength = object_controls.ambient_slider->value() / 100.0f;
    float object_diffuse_strength = object_controls.diffuse_slider->value() / 100.0f;
    float object_specular_strength = object_controls.specular_slider->value() / 100.0f;
    float object_shininess = static_cast<float>(object_controls.shininess_slider->value());
    float object_edge_strength = object_controls.edge_slider->value() / 100.0f;
    float object_edge_power = object_controls.edge_power_slider->value() / 100.0f;

    anaglyph_widget->set_atom_lighting_settings(
        atom_ambient_strength,
        atom_diffuse_strength,
        atom_specular_strength,
        atom_shininess,
        atom_edge_strength,
        atom_edge_power
    );

    anaglyph_widget->set_object_lighting_settings(
        object_ambient_strength,
        object_diffuse_strength,
        object_specular_strength,
        object_shininess,
        object_edge_strength,
        object_edge_power
    );

    const int msaa_samples = this->msaa_combo->currentData().toInt();
    anaglyph_widget->set_msaa_samples(msaa_samples);
    anaglyph_widget->set_sphere_tesselation_level(this->sphere_tesselation_spinbox->value());

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
    QSignalBlocker atom_diffuse_blocker(atom_controls.diffuse_slider);
    QSignalBlocker atom_specular_blocker(atom_controls.specular_slider);
    QSignalBlocker atom_shininess_blocker(atom_controls.shininess_slider);
    QSignalBlocker atom_edge_blocker(atom_controls.edge_slider);
    QSignalBlocker atom_edge_power_blocker(atom_controls.edge_power_slider);
    atom_controls.ambient_slider->setValue(static_cast<int>(atom_settings.ambient_strength * 100.0f));
    atom_controls.diffuse_slider->setValue(static_cast<int>(atom_settings.diffuse_strength * 100.0f));
    atom_controls.specular_slider->setValue(static_cast<int>(atom_settings.specular_strength * 100.0f));
    atom_controls.shininess_slider->setValue(static_cast<int>(atom_settings.shininess));
    atom_controls.edge_slider->setValue(static_cast<int>(atom_settings.edge_strength * 100.0f));
    atom_controls.edge_power_slider->setValue(static_cast<int>(atom_settings.edge_power * 100.0f));

    QSignalBlocker object_ambient_blocker(object_controls.ambient_slider);
    QSignalBlocker object_diffuse_blocker(object_controls.diffuse_slider);
    QSignalBlocker object_specular_blocker(object_controls.specular_slider);
    QSignalBlocker object_shininess_blocker(object_controls.shininess_slider);
    QSignalBlocker object_edge_blocker(object_controls.edge_slider);
    QSignalBlocker object_edge_power_blocker(object_controls.edge_power_slider);
    object_controls.ambient_slider->setValue(static_cast<int>(object_settings.ambient_strength * 100.0f));
    object_controls.diffuse_slider->setValue(static_cast<int>(object_settings.diffuse_strength * 100.0f));
    object_controls.specular_slider->setValue(static_cast<int>(object_settings.specular_strength * 100.0f));
    object_controls.shininess_slider->setValue(static_cast<int>(object_settings.shininess));
    object_controls.edge_slider->setValue(static_cast<int>(object_settings.edge_strength * 100.0f));
    object_controls.edge_power_slider->setValue(static_cast<int>(object_settings.edge_power * 100.0f));

    const int configured_msaa = anaglyph_widget->get_msaa_samples();
    const int msaa_index = this->msaa_combo->findData(configured_msaa);
    if (msaa_index >= 0) {
        QSignalBlocker msaa_blocker(this->msaa_combo);
        this->msaa_combo->setCurrentIndex(msaa_index);
    }

    {
        QSignalBlocker tesselation_blocker(this->sphere_tesselation_spinbox);
        this->sphere_tesselation_spinbox->setValue(anaglyph_widget->get_sphere_tesselation_level());
    }

    update_labels(atom_controls);
    update_labels(object_controls);
}

void LightingSettingsDialog::setup_controls(QGroupBox* group_box, LightingControls* controls) {
    auto *grid = new QGridLayout(group_box);

    controls->ambient_slider = new QSlider(Qt::Horizontal);
    controls->ambient_slider->setRange(0, kFloatSliderMax);
    controls->ambient_slider->setSingleStep(1);
    controls->ambient_value = new QLabel();

    controls->diffuse_slider = new QSlider(Qt::Horizontal);
    controls->diffuse_slider->setRange(0, kFloatSliderMax);
    controls->diffuse_slider->setSingleStep(1);
    controls->diffuse_value = new QLabel();

    controls->specular_slider = new QSlider(Qt::Horizontal);
    controls->specular_slider->setRange(0, kFloatSliderMax);
    controls->specular_slider->setSingleStep(1);
    controls->specular_value = new QLabel();

    controls->shininess_slider = new QSlider(Qt::Horizontal);
    controls->shininess_slider->setRange(kShininessMin, kShininessMax);
    controls->shininess_slider->setSingleStep(1);
    controls->shininess_value = new QLabel();

    controls->edge_slider = new QSlider(Qt::Horizontal);
    controls->edge_slider->setRange(0, kEdgeStrengthMax);
    controls->edge_slider->setSingleStep(1);
    controls->edge_value = new QLabel();

    controls->edge_power_slider = new QSlider(Qt::Horizontal);
    controls->edge_power_slider->setRange(0, kEdgePowerMax);
    controls->edge_power_slider->setSingleStep(1);
    controls->edge_power_value = new QLabel();

    grid->addWidget(new QLabel(tr("Ambient strength")), 0, 0);
    grid->addWidget(controls->ambient_slider, 0, 1);
    grid->addWidget(controls->ambient_value, 0, 2);

    grid->addWidget(new QLabel(tr("Diffuse strength")), 1, 0);
    grid->addWidget(controls->diffuse_slider, 1, 1);
    grid->addWidget(controls->diffuse_value, 1, 2);

    grid->addWidget(new QLabel(tr("Specular strength")), 2, 0);
    grid->addWidget(controls->specular_slider, 2, 1);
    grid->addWidget(controls->specular_value, 2, 2);

    grid->addWidget(new QLabel(tr("Shininess")), 3, 0);
    grid->addWidget(controls->shininess_slider, 3, 1);
    grid->addWidget(controls->shininess_value, 3, 2);

    grid->addWidget(new QLabel(tr("Edge darkening")), 4, 0);
    grid->addWidget(controls->edge_slider, 4, 1);
    grid->addWidget(controls->edge_value, 4, 2);

    grid->addWidget(new QLabel(tr("Edge power")), 5, 0);
    grid->addWidget(controls->edge_power_slider, 5, 1);
    grid->addWidget(controls->edge_power_value, 5, 2);
}

void LightingSettingsDialog::update_labels(const LightingControls& controls) {
    controls.ambient_value->setText(to_percent_label(controls.ambient_slider->value()));
    controls.diffuse_value->setText(to_percent_label(controls.diffuse_slider->value()));
    controls.specular_value->setText(to_percent_label(controls.specular_slider->value()));
    controls.shininess_value->setText(QString::number(controls.shininess_slider->value()));
    controls.edge_value->setText(to_percent_label(controls.edge_slider->value()));
    controls.edge_power_value->setText(to_percent_label(controls.edge_power_slider->value()));
}
