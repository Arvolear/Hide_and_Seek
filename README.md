# Hide and Seek - 3D multiplayer game

## Features

- The game provides multiplayer-only mode to play with friends and get most fun out of it.

- Custom shaders to achieve the best look and feel.
  - Physics based rendering engine.
  - Deferred shading.
  - Atmosphere scattering (day/night shifts).
  - Skyboxes.
  - Exponential shadow mapping.
  - Light shafts.
  - Bloom.
  - SSAO.

- Modern game techniques
  - Model's skeletal animation (feel free to read my [tutorial](https://code-hub.org/tutorials/skeletal-animation-with-assimp-in-opengl/)).
  - Instanced grass.
  - XML configuration files (sent from the server).
  - FPS free gameplay.

## Preview

![preview](./images/preview.png?raw=true)

## Libraries

The game is implemented via OpenGL (mesaGL) and Bullet Physics engines. 
It also includes GLFW3 library to handle windows, ASSIMP library to load models, SOIL and nv_dds libraries to load textures and tinyXML2 library to work with XML.

## Models and textures

All models are taken from [Sketchfab](https://sketchfab.com/).
Some PBR textures are taken from [FreePBR](https://freepbr.com/).
