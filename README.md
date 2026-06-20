**OpenGL Mesh Displayer**

A C++ / OpenGL (GLUT) application that loads triangle meshes from OBJ files and renders them with real-time Phong lighting, an orbit camera and
smooth color transitions.

Built as practice work for a Computer Graphics tutorial.

**Features**

**Core rendering**
- Custom OBJ parser — reads vertices (v), normals (vn), and triangular faces (f) directly from stdin, with no external mesh-loading library
- Phong-shaded rendering via OpenGL immediate mode, accelerated with display lists for efficient re-rendering of static geometry
- Cycles through a small palette of material colors

**Interaction**
c-Smootly transitions(fades) between material colors, rather than snapping instantly
r-Toggles automatic spinning of the model around the vertical axis
Arrow keys-Moves the scene's point light source
Left-click + drag-Orbits the camera around model
Right-click + drag-Zooms the camera in/out
Esc-Exits the application

**Build & run**
**Requirements**
- A C++ compiler
- OpenGL
- GLUT(freeglut)
- The bundled vecmath library(include/vecmath, vecmath/) for vector/matrix math

**Running**
The program reads an OBJ file via standart input: a0.exe < sphere.obj
**Note**: PowerShell does not support < redirection, so use this command instead: Get-Content sphere.obj | ./a0.exe

**Implementation notes**
- **Display lists** are rebuilt (buildDisplayList()) any time the underlying geometry changes — most notably after a mesh simplification step — since a display list caches a fixed sequence of draw calls and won't reflect updates to vecv/vecf otherwise.

- **Camera control** is implemented as a classic orbit/arcball-style controller: the camera's position is computed each frame from a yaw angle, pitch angle, and distance (spherical coordinates), always looking at the origin. Pitch is clamped to avoid the camera flipping past vertical.

- **Color transitions** are driven by a self-re-arming glutTimerFunc chain that linearly interpolates between the previous and target colors over a short, fixed number of steps, rather than jumping instantly.

- **Automatic spinning** uses the same self-re-arming timer pattern, advancing a rotation angle applied via glRotatef each frame, and stopping cleanly (no dangling timer calls) when toggled off.

**Possible future improvements**
Solve for the analytically optimal vertex position during edge collapse (full QEM).











