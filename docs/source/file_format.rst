File format
===========

File Variants and Detection
---------------------------

ABO files are stored as **little-endian binary** data. Managlyph supports a
legacy variant and an extended variant (ABOF). The loader distinguishes these
variants using a sentinel value stored in the first two bytes of the file.

.. list-table::
   :header-rows: 1

   * - First field (offset 0)
     - Interpretation
   * - ``uint16 nr_frames`` is non-zero
     - **Legacy ABO**.
       The value is the frame count and parsing continues
       with the first frame record.
   * - ``uint16 nr_frames`` is zero
     - **ABOF**.
       A header follows immediately and the actual frame
       count is read from the payload stream after the
       header has been processed.

ABOF Header
-----------

When the initial ``nr_frames`` value is zero, the loader reads an ABOF header:

.. list-table::
   :widths: 20 15 65
   :header-rows: 1

   * - Offset
     - Size
     - Description
   * - 2
     - 4 bytes
     - Magic identifier ``ABOF`` (ASCII)
   * - 6
     - 1 byte
     - Format version (supported: 1 or 2)
   * - 7
     - 1 byte
     - Flags

The magic identifier must match ``ABOF``. Unsupported versions are rejected.

ABOF Flags
----------

The flags byte controls global properties of the payload:

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Flag
     - Meaning
   * - ``0x01``
     - Payload is compressed using **Zstandard (zstd)**.
   * - ``0x02``
     - Container represents a reaction pathway (NEB); interpolation may be
       performed after loading.

If the compression flag is set, the loader reads the remaining file content
as a zstd frame, decompresses it, and continues parsing from the decompressed
byte stream. If compression is not set, parsing continues directly from the
file stream.

After header processing (and optional decompression), the loader reads the
actual frame count (``uint16 nr_frames``) from the selected input stream.

Legacy vs. ABOF v1 vs. ABOF v2
------------------------------

The following differences are relevant to readers and writers.

.. list-table::
   :widths: 25 25 50
   :header-rows: 1

   * - Variant
     - Detection
     - Behavioral differences in the loader
   * - Legacy ABO
     - First ``uint16 nr_frames`` is non-zero.
     - No header, no global flags. Vertex normals are read as
       3 × ``float32`` per vertex.
   * - ABOF v1
     - First ``uint16`` is zero, followed by ``ABOF`` and version ``1``.
     - Supports global flags, including optional zstd compression and
       reaction-pathway marking. Vertex normals are read using octahedral
       encoding (2 × ``int16`` per vertex).
   * - ABOF v2
     - First ``uint16`` is zero, followed by ``ABOF`` and version ``2``.
     - Same as v1, plus **per-frame flags**. Frames may optionally include a
       unit cell matrix (9 × ``float32``) when the unit-cell flag is set.

Notes:

- The reaction-pathway flag (``0x02``) does not change the on-disk layout of
  frames. It signals post-processing: after loading, Managlyph may generate
  interpolated frames if all frames have compatible atom ordering and identity.
- Normal encoding is selected implicitly by the variant: legacy uses float32
  normals; ABOF uses octahedral-encoded normals.

Frame Record
------------

Each frame is stored sequentially and contains structural data and optional
renderable models.

.. list-table::
   :widths: 30 20 50
   :header-rows: 1

   * - Field
     - Type
     - Description
   * - Frame index
     - ``uint16``
     - Sequential frame identifier.
   * - Description length
     - ``uint16``
     - Length of descriptor string.
   * - Description
     - bytes
     - Optional UTF-8 descriptor.
   * - Frame flags *(v2 only)*
     - ``uint8``
     - Indicates presence of optional frame data.
   * - Unit cell *(optional)*
     - 9 × ``float32``
     - 3×3 unit cell matrix when enabled.
   * - Atom count
     - ``uint16``
     - Number of atoms in the frame.
   * - Atom records
     - variable
     - Atomic structure data.
   * - Model count
     - ``uint16``
     - Number of 3D models.
   * - Model records
     - variable
     - Renderable mesh data.

Frame Flags (Version 2)
-----------------------

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Flag
     - Meaning
   * - ``0x01``
     - Unit cell matrix is present.

If the flag is set, a 3×3 matrix (9 float32 values) immediately follows.

Atomic Structure
----------------

Atomic data defines molecular or crystalline structures.

.. list-table::
   :widths: 30 20 50
   :header-rows: 1

   * - Field
     - Type
     - Description
   * - Atomic number
     - ``uint8``
     - Element identifier.
   * - Position
     - 3 × ``float32``
     - Cartesian coordinates.

Atoms are stored in sequence and must remain consistently ordered across
frames to enable interpolation.

3D Model Records
----------------

Frames may include one or more renderable mesh models.

.. list-table::
   :widths: 30 20 50
   :header-rows: 1

   * - Field
     - Type
     - Description
   * - Model index
     - ``uint16``
     - Identifier for the model.
   * - Color
     - 4 × ``float32``
     - RGBA color.
   * - Vertex count
     - ``uint32``
     - Number of vertices.
   * - Vertex positions
     - 3 × ``float32`` each
     - Vertex coordinates.
   * - Vertex normals
     - encoded
     - Surface normals (encoding depends on format).
   * - Face count
     - ``uint32``
     - Number of triangular faces.
   * - Indices
     - 3 × ``uint32`` per face
     - Triangle vertex indices.

Empty models (zero vertices or faces) may be present and should be ignored.

Normal Encoding
---------------

Two normal encoding schemes are supported:

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Encoding
     - Description
   * - Float32 (legacy)
     - Three 32-bit floats per normal vector.
   * - Octahedral 16-bit (ABOF)
     - Two 16-bit integers decoded into a normalized vector.

Octahedral encoding reduces storage size while maintaining accuracy.

Reaction Pathways & Interpolation
---------------------------------

If the reaction pathway flag is set in the file header, the container is
treated as a reaction sequence.

Interpolation is performed when:

- frames contain the same number of atoms
- atomic ordering is identical
- atomic identities match

If these conditions are met, intermediate frames are generated using
Catmull–Rom cubic interpolation.

Interpolation properties:

.. list-table::
   :widths: 40 60
   :header-rows: 1

   * - Property
     - Value
   * - Steps per segment
     - 10
   * - Method
     - Catmull–Rom spline interpolation
   * - Interpolated data
     - atomic positions
   * - Result
     - smooth reaction trajectory

If compatibility checks fail, interpolation is skipped.

Scene Composition
-----------------

A frame may combine multiple data types:

- atomic structures
- orbital or isosurface meshes
- general 3D geometry
- periodic unit cell definitions

This design allows chemical and geometric data to be rendered together.

Compression
-----------

When enabled in the header, the payload is compressed using **Zstandard
(zstd)**. The entire payload is decompressed before parsing.

Capabilities
------------

The ABO container format supports:

- multi-frame scenes and animations
- reaction pathway visualization with interpolation
- atomic and periodic structures
- orbital and surface meshes
- hybrid scenes combining scientific and geometric data
- compact normal encoding
- optional payload compression
- per-frame metadata and descriptors
