# RocketWrangler3D
Run make
If you need GLEW run "make USE_GLEW=1" 
run ./RocketWrangler3D

If that doesn't work. I've attempted to make a dist folder. Just run ./RocketWrangler3D on Linux or ./RocketWrangler3D.exe for Windows. You won't get my textures though. 

Controls: 
WASD: For moving around
Space: to jump off a rocket
F: for full screen
R: to restart
Esc: to escape
O: to get a super birds eye view of the scene(Which I think is cool)
P: to pause and be able to look around. 

No camera controls unpaused
In pause:
WASD: Move around
C and Space: Move up and Down. 
arrows: move camera. Sorry I didn't fix this to be fully FPS. So the camera is a little disjointed


# RocketWrangler3D
Your goal is to wrangle some rockets. Jump off the current rocket and use some rudimentary air controls to try and land on another rocket. You have one ground bounce, and any number of wall bounces. 

I spent a lot of time refining the game aspects, because I know my friends will want to play this. The first RocketWrangler was a huge success among them, but is no longer on iOS. 
I hope I've created a fun tribute, a bit hard to play (mastery of rockets is where the fun lies)
My girlfriend's cousin spent almost an hour playing this game, which I think is a win. 

# Cool things:
- Joints (see `assets/Models/joint_class.c`). This "class" basically allows for the game's animation to work by having transformations recursively run through. I can move specific joints on the character, and they relatively stick together. This is also how the rockets get grabbed. The characters rag doll is a him randomly moving joints around. I added a crunch factor to allow a closed fast spin, and a slower open spi. That with the side controls, is great. 

- Procedural ground: Ground tiles from a hash-based height and color scheme (`src/draw/drawScene.c`)
- Background: The art you see in the background is a large sprite that, with fog, really blends into the scene.

- Rocket gameplay loop: Continuous rocket spawning, collision checks, bounce rules (one ground bounce, unlimited wall bounces), and score tracking (`src/game/engine.c`, `src/game/rockets.c`).

- Particles: Circular particles that allow for rocket thrusts and explosions. Which is fun (`src/game/particles.c`, spawn in `assets/Models/rocket.c`) 

- Models: Cool models, the character's hat, and cactus 
- The rocket model and character are made of relatively simple shapes, but look good, and have textures applied

-Game engine: `src/game/engine.c` I think the logic here is fun. It was fun to make. 



