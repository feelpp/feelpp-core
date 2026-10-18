<!--
SPDX-FileCopyrightText: 2026 Université de Strasbourg
SPDX-License-Identifier: LGPL-3.0-or-later
-->

# Third-party material

| Material | Terms / treatment |
| --- | --- |
| `src/feel/feelcore/termcolor.hpp` | Ihor Kalnytskyi, 2013. Retains BSD-3-Clause; full upstream terms are in LICENSES/BSD-3-Clause.txt. Imported by commit `6b762ad`. [Upstream](https://github.com/ikalnytskyi/termcolor). |
| `cmake/cgal.patch` | Project patch with CGAL source context, including GeometryFactory's code. The sidecar records LGPL-3.0-or-later AND GPL-3.0-or-later for this distributed patch. It does not grant commercial CGAL rights. [Upstream file](https://github.com/CGAL/cgal/blob/v6.2.1/Constrained_triangulation_3/include/CGAL/Conforming_constrained_Delaunay_triangulation_3.h). |
| `cmake/catch2.patch` | Project patch with Catch2 CMake context. The sidecar records LGPL-3.0-or-later AND BSL-1.0. [Upstream file](https://github.com/catchorg/Catch2/blob/v3.8.1/CMakeLists.txt). |

## Provenance pending

Tracked in [issue #80](https://github.com/feelpp/feelpp-core/issues/80).

| File / portion | Evidence and remaining action |
| --- | --- |
| `src/feel/feelcore/table.cpp`, `utility::centre` | References a [C++ forum example](https://cplusplus.com/forum/general/256212/). Establish the reused snippet's terms or replace that portion. Project additions remain LGPL; no third-party grant is inferred from the URL. |
| `src/feel/feelcore/threadpool.hpp` and `.cpp` | References an [Ishan Tripathi tutorial](https://dev.to/ish4n10/making-a-thread-pool-in-c-from-scratch-bnm). Establish the terms of adapted portions or replace them, distinguishing common ideas from copied expression. |

LicenseRef-Provenance-Pending records those outstanding questions without
granting rights to the external material. Source references are preserved for
follow-up.

## Fetched and system dependencies

Dependencies retain their own licences. The build declares NAPP, fmt, JSON,
spdlog, Eigen, FTXUI, zlib, libzip, Boost and HDF5, with optional CPR, libassert,
MPI and TBB, and Catch2 for tests. Consumer-facing helpers also support CGAL,
BVH and Assimp. The exact build configuration determines what is used or
redistributed; the LGPL project declaration does not replace those terms.
