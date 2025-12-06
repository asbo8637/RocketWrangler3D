# RocketWrangler3D

Run make
If you need GLEW Run "make USE_GLEW=1" 
run ./RocketWrangler3D

If that doesn't work. I've attempted to make a dist folder. Just run ./RocketWrangler3D on linux or ./RocketWrangler3D.exe for windows

Controls: 
WASD: For moving around
Space: to jump off a rocket
F: for full screen
Esc: to escape

No camera controls

Your goal is to wrangle some rockets. Jump off the current rocket and use some rudimentary air controls to try and land on another rocket. You have one ground bounce, and any number of wall bounces. 

Cool graphics things, is the joint system I've created. Uses a root parent and a branching tree system. It recursively transforms based on its parents transform(Which is based on that parents transform, and so on)

The Scene has fog and the ground is built off a hashing system to create the squares. 



For the particles.c file I used some AI and this as a guide: https://learnopengl.com/In-Practice/2D-Game/Particles
The directional logic and explosion is all me. 

The make file was AI
Controls where based on previous assignments and some small changes from me

