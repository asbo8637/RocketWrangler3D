#ifndef PARTICLES_H
#define PARTICLES_H

int  particles_init(int maxParticles);
void particles_shutdown(void);

int  particles_spawn(const float pos[3], const float vel[3],
                     float lifetime, float size,
                     const float colorStart[4], const float colorEnd[4]);

void particles_update(float deltaTime);
void particles_render(void);

#endif /* PARTICLES_H */
