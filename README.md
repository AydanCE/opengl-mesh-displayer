A C++ / OpenGL (GLUT) application that loads triangle meshes from OBJ files and renders them with real-time Phong lighting, an orbit camera and
smooth color transitions.

Built as practice work for a Computer Graphics tutorial.

Features

Core rendering


Custom OBJ parser — reads vertices (v), normals (vn), and triangular faces (f) directly from stdin, with no external mesh-loading library
Phong-shaded rendering via OpenGL immediate mode, accelerated with display lists for efficient re-rendering of static geometry
Cycles through a small palette of material colors
