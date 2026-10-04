# CG

Classic raster graphics algorithms: DDA and Bresenham lines, midpoint circle, Liang–Barsky clipping, 2D transforms.

**Live in the browser:** open `web/index.html` (no build step). `web/index.html#clip` opens a tab directly.

**Original OpenGL programs:** `make`, then `./dda`, `./bresenham`, `./circle`, `./transform`, `./liang_barsky`
(macOS uses the system GLUT framework; Linux needs `freeglut3-dev`).

**Check the algorithms:** `make test` (needs Node).
