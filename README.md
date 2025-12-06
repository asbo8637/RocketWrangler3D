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

- Models: Cool models, the character's hat, and cactus (from previous assignment)
- The rocket model and character are made of relatively simple shapes, but look good, and have textures applied

-Game engine: `src/game/engine.c` I think the logic here is fun. It was fun to make. 


## AI acknowledgments
- `src/game/particles.c`: Drafted with AI assistance and adapted from learnopengl.com’s “2D Game Particles”. Exhaust colors/logic are my own, and any use of the particles is also my logic. The only other thing I had help with is rotate_local_and_forward(In `assets/Models/rocket.c`). I could not figure out the math of spawning at the base of the rocket. (Would've worked if I flipped the rocket around the Y, as that would've been really easy as the thrust would be right at the base, but I did not want to refactor all my code.)
- `Makefile`: Made with AI. Based it off previous assignments, but I have no idea how MAKE files really work.
- `textures.c` : AI helped build that out. I did textures before. I wanted a more generally applicable tool. 
- `cone and circle` : I have drawn plenty of shapes; I used AI to make these quickly. 
- `Body Poses` : Some of the poses I had AI initially put together, but required a lot of tweaking. So not really AI
- `Full Screen feature` : I had AI build that out. I found the GL tool for it though, through google. 
- `Hash Function` : I used a hash function in draw scene. I used AI to make that. 
- Everything else is 99% my own work. I might've forgotten to list something here, but most of it is my own logic. 



