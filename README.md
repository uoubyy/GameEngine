# My Personal Game Engine

## Summary

My personal Game Engine project with a Maya Mesh Exporter plugin. It has well support for both x64 (d3d11) and x32 (OpenGL) platforms. The Maya Mesh Exporter plugin is developed based on Maya 2019 but still works well for a higher version (need to include the newer Maya devkit and rebuild).
![screenshot](https://imgur.com/a/p2JmO9w)

## Features and dev process

### Finished

- [x] Export Maya mesh with materials to human-readable files.
- [x] Uniform shader interface for GLSL and HLSL.
- [x] Build human-readable mesh files into binary file.
- [x] Load and parse mesh file, extra data into cMesh and cMaterial.
- [x] Generate render commands in the game update thread and submit those commands to the render thread.
- [x] Resources manage, reference count and safety check.
- [x] Decouple in modules and export as dynamic libraries.
- [x] Default shadering model, Lambert and Blinn-Phong.

### In Process

- [ ] Skybox.
- [ ] Default lighting sources, Point Light and Directional Light.
- [ ] Shadows and Soft-Shadows.
- [ ] Deferred Shading.
- [ ] Screen-space ambient occlusion.

### Maybe

- [ ] PBR.

## Setup and install

* Download and install Maya 2019 and Maya 2019 devkit.
* Set environment variable **MAYA_LOCATION** to the location Maya installed (e.g., C:\Program Files\Autodesk\Maya2019).
* Set environment variable **MAYA_PLUG_IN_PATH** to where you want the Maya Mesh Exporter plugin be exported.
* Set environment variable **DEVKIT_LOCATION** to the location where Maya 2019 devkit extracted.
* Generate the MayaMeshExporter, it will be copied to **MAYA_PLUG_IN_PATH**.
* Launch Maya and load the MayaMeshExporter plugin. Select the model and export it.
* Open the mesh (human readable format, end with .mesh) file you exported from Maya, modify the material config (e.g., change the base color texture).
* Copy the mesh file into the "MyGame_\Content\Meshes" dictionary and add it to the build list (AssetsToBuild.lua).
* Build game assets for the target platform (it will convert the human readable format mesh files into binary files which end with .dat and copy textures to the game dictionary).
* Assign your mesh to the target actor.
* Runing the game.


## Support
uoubyy@gmail.com
