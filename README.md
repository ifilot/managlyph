# Managlyph

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/ifilot/managlyph?label=version)
[![build](https://github.com/ifilot/managlyph/actions/workflows/ci.yml/badge.svg)](https://github.com/ifilot/managlyph/actions/workflows/ci.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Overview
Managlyph is a desktop application for 3D visualization of atomic and molecular
structures.

The application supports stereoscopic rendering via red/cyan anaglyph projection
and interlaced output for compatible 3D displays. Typical use cases include
classroom demonstrations, self-study, and interactive exploration of molecular
models.

## Documentation and Downloads
- Windows 64-bit installer:
  https://github.com/ifilot/managlyph/releases/latest/download/managlyph-installer-win64.exe
- User manual:
  https://ifilot.github.io/managlyph/

## Capabilities

### Stereoscopic Rendering
Managlyph provides red/cyan anaglyph rendering and interlaced stereo output.
These modes are intended for use with either anaglyph glasses or supported 3D
monitors/projectors.

![red/cyan stereographic projection](img/screenshot-anaglyph.png)

### ABO File Support and Molecular Orbitals
Managlyph uses the Atom Bond Object (ABO) format to store atomistic structures
and related metadata. ABO datasets can include molecular geometries, bonding
information, and molecular-orbital isosurfaces for visualization.

![molecular orbitals](img/screenshot-molecular-orbitals.png)

### Atomic Orbitals
The application can generate and render atomic orbitals up to the 4f subshell,
including rotation and inspection in 3D.

![atomic orbitals](img/screenshot-atomic-orbitals.png)

### Reaction Pathway Visualization
Managlyph can visualize elementary reaction pathways by interpolating between
structures (for example: reactants, intermediates, transition states, and
products). This allows stepwise inspection of geometry changes during a
reaction.

![reaction pathway](img/screenshot-reaction.png)

## Project Support
Development of Managlyph received financial support from the TU/e Boost
program:
https://boost.tue.nl/projects/3d-visualization-with-artificial-intelligence-and-projection/
