#ifndef ROCKETS_H
#define ROCKETS_H

#include "../../assets/Models/Rocket.h"
#include <stddef.h>

/* Initialize rocket manager; call once at startup */
void rockets_init(void);

/* Free all rockets and release manager resources */
void rockets_shutdown(void);

/* Spawn a new rocket at the given world position; returns NULL on failure */
Rocket *rockets_spawn(float x, float y, float z, float vx, float vy, float vz);

/* Optional: remove a specific rocket immediately */
void rockets_remove(Rocket *rocket);

/* Number of live rockets */
size_t rockets_count(void);

/* Get rocket by index in [0, rockets_count()) */
Rocket *rockets_get(size_t index);

/* Test for collision against a sphere; returns first intersecting rocket or NULL */
Rocket *rockets_findCollision(float px, float py, float pz, float radius);

/* Update all active rockets */
void rockets_update(double deltaTime, float death_Z_zone);

/* Draw all active rockets */
void rockets_draw(void);

#endif /* ROCKETS_H */
