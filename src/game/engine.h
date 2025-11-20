#ifndef ENGINE_H
#define ENGINE_H

// Initialize the game engine
void initEngine(void);

// Update game state. deltaTime is in seconds
void updateEngine(double deltaTime);

// Retrieve the current bounce counter.
int engine_getBounceCount(void);

int engine_getScore(void); 

#endif /* ENGINE_H */