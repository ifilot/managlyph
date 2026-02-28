Managlyph documentation
=======================

Managlyph is a visualization tool designed for stereographic rendering of
scientific and 3D data. It supports anaglyph and interlaced output for 3D
displays, and can visualize atomic structures, molecular and atomic orbitals,
reaction sequences, and general 3D objects using its native ABO container
format.

Features
========

* **Stereographic rendering**
  Native support for stereographic visualization as the primary rendering mode.

* **3D display compatibility**
  Output for red/cyan anaglyph glasses and interlaced formats for polarized
  3D monitors and televisions.

* **Atomic structure visualization**
  Rendering of molecular and crystalline structures.

* **Atomic and molecular orbitals**
  Visualization of molecular orbitals and a dedicated mode for atomic orbitals.

* **Reaction sequence rendering**
  Smooth interpolation of reaction pathways using cubic interpolation between frames.

* **ABO container format**
  Native ``.abo`` storage format for bundling atomic data, orbitals, reaction
  sequences, and general 3D objects.

* **Hybrid scene composition**
  Combine chemical systems and generic 3D geometry within a single scene.

* **Frame interpolation**
  Automatic generation of smooth transitions for tagged reaction sequences.

* **Scientific visualization focus**
  Designed for chemical and structural visualization workflows.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   installation.rst
   file_format.rst
   building_models.rst
   usage.rst
   community_guidelines.rst