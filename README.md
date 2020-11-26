# Otter, a toy Game Engine

## Broad Goals:
- Implement a basic system for supporting game the development of a 3D game.
- Write everything from scratch, except for certain basic functions from the C Standard Library.

### Note:
  Platform abstraction is done through the use of a 3rd party library (Final Platform Layer, found at: https://libfpl.org/), with extended development features supported only on windows - this was done so I that I have a jump pad to build on to focus on learning other things such as rendering, collisions, asset loading, compression, etc. In the future, I plan on removing this dependancy as well.

  I use Sean Barret's implementation of a dynamically sized array in my code (found at: https://github.com/nothings/stb). I plan on replacing this as well later on.

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
  - - [ ] Normals
  - - [ ] Culling
  - - [ ] Lighting
  
 - [ ] Asset Loading
  - - [ ] .obj files
  
  - [ ] Hardware Rendering
  
- [ ] Collissions


## Known Issues
- The window close events are not being handled properly -> if the game is closed with Alt + F4 or by
  clicking the close button, only the window closes and the game runs in the background. This is due
  to something wrong with my understanding of FPL and how to handle the main window callback function.
- When a triangle outline is drawn on topped of an already filled triangle, the outline breaks

## Images

![Alt text](screenshots/wireframeCube.gif?raw=true "wireframeCube")
