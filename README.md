# Otter, a toy Game Engine

## Broad Goals:
- Implement a basic system for supporting game the development of a 3D game.
- Write everything from scratch, except for certain basic functions from the C Standard Library.

## Progress

## Super early in the process!
- [ ] Software Renderer
- - [x] Bresenham Line Algorithm
- - [x] Render Triangles
- - - [x] Draw Triangles
- - - [x] Fill Triangles
- - [ ] Render Circles
- - - [x] Bresenham Circle Drawing Algorithm
- - - [ ] Fill Circles
- - [x] Render Polygons
- - [x] Project from 3D to 2D
- - [x] Depth Buffering*
- - [x] Normals
- - [x] Culling
- - [ ] Lighting
- - - [x] Basic single point light

- [ ] Asset Loading
- - [x] .obj files

- [ ] Hardware Rendering

- [ ] Collissions


## Known Issues
- * The Depth buffering algorithm is slightly buggy, with artifacts showing up from certain angles,
hope to fix that in the next few days.

## Images

![Alt text](screenshots/wireframeCube.gif?raw=true "wireframeCube")
![Alt text](screenshots/filledAndLitCube.gif?raw=true "filledAndLitCube")
![Alt text](screenshots/filledAndLitCube.gif?raw=true "zBufferedObjFile")