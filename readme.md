# Right Now
* multi select verts
* add vertex, delete vertex button

# Todo:
* Touching the screen that causes a scene change doesnt continue to register the touch when the scene changes
* c stick rotation does in place rotation instead of whole model around, just the face
* Use earclipping to generate tris instead of fan so concave shapes
* save data
* home menu, shows last model on home menu? home menu -> editor always starts new file -> load
* UV mapping editor
* sprite editor
* help button

# Maybe:
* Rotate View buttons cause smooth rotation transition, pick good values
* export model
* camera mode? load multi models and scale them? -- how would textures work?
* vert handles depth draw with edges to look cleaner

# Knowledge
* 3dslink -a 192.168.0.102 cad.3dsx
* /opt/devkitpro/devkitARM/bin/arm-none-eabi-addr2line -aipfCe arm -e cad.elf PC
