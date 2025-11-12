#ifndef INIT_H
#define INIT_H

#include "../../assets/Models/Robot.h"

// Global scene objects (defined in init.c)
extern Robot *robot;

// Initialize all scene objects once at startup
void initScene(void);

// Cleanup scene objects on shutdown (optional)
void cleanupScene(void);

#endif // INIT_H
