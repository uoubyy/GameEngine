# My Personal Game Engine

## Summary

My personal Game Engine project with a Maya Mesh Exporter plugin. It has well support for both x64 and x32 platforms. The Maya Mesh Exporter plugin is developed based on Maya 2019 but still works well for a higher version (need to include the newer Maya devkit and rebuild). 

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
 -[ ] Screen-space ambient occlusion.

### Maybe

- [ ] PBR.

## Support
uoubyy@gmail.com
