Building Models
===============

Managlyph renders 3D geometry stored inside ABO containers. These models can
represent molecular orbitals, isosurfaces, or general 3D meshes. Models are
typically prepared using **AboBuilder**, a companion tool for constructing ABO
files compatible with Managlyph.

Using AboBuilder
----------------

AboBuilder provides tools for constructing atomic structures and generating
renderable geometry for use in Managlyph. It supports:

- construction of molecular and atomic structures
- generation of orbital and isosurface meshes
- export of frame data and geometry to the ABO container format

AboBuilder is available at: `https://ifilot.github.io/abobuilder/ <https://ifilot.github.io/abobuilder/>`_

Generated ABO files can be opened directly in Managlyph.

Integration with PyQInt
-----------------------

AboBuilder can integrate with **PyQInt**, a quantum chemistry library used to
compute electronic structure properties such as molecular orbitals and electron
density distributions.

PyQInt project page: `https://github.com/ifilot/pyqint <https://github.com/ifilot/pyqint>`_

This integration enables the generation of orbital surfaces and related data
for visualization.

Typical workflow:

1. Compute molecular orbitals or electron density using PyQInt.
2. Use AboBuilder (in conjunction with PyTessel) to convert computed data into isosurfaces or meshes.
3. Export the result to an ABO container.
4. Open the ABO file in Managlyph for stereographic visualization.