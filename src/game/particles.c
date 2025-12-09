// Particle system scaffolding was drafted with AI guidance based on learnopengl.com's 2D particles chapter.
#include <stdlib.h>
#include <string.h>

#include "particles.h"

#ifdef USEGLEW
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#endif
#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include "../../assets/Shapes/circle.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


typedef struct Particle
{
    float pos[3];
    float vel[3];
    float colorStart[4];
    float colorEnd[4];
    float lifetime;
    float age;
    float size;
} Particle;

typedef struct ParticleSystem
{
    Particle *particles;
    int count;
    int maxCount;
} ParticleSystem;

static ParticleSystem gParticles = {0};

int particles_init(int maxParticles)
{   
    if (maxParticles <= 0)
        return 0;

    free(gParticles.particles);
    gParticles.particles = (Particle *)calloc((size_t)maxParticles, sizeof(Particle));
    if (!gParticles.particles)
    {
        gParticles.count = 0;
        gParticles.maxCount = 0;
        return 0;
    }

    gParticles.count = 0;
    gParticles.maxCount = maxParticles;
    return 1;
}

void particles_shutdown(void)
{
    // Clean up
    free(gParticles.particles);
    gParticles.particles = NULL;
    gParticles.count = 0;
    gParticles.maxCount = 0;
}


//Spawn a particle and then slowly transition it from colorStart to colorEnd over its lifetime
int particles_spawn(const float pos[3], const float vel[3],
                    float lifetime, float size,
                    const float colorStart[4], const float colorEnd[4])
{
    if (gParticles.count >= gParticles.maxCount || lifetime <= 0.0f)
        return 0;

    Particle *p = &gParticles.particles[gParticles.count++];
    memcpy(p->pos, pos, 3 * sizeof(float));
    memcpy(p->vel, vel, 3 * sizeof(float));
    p->lifetime = lifetime;
    p->age = 0.0f;
    // Guard against negative/zero sizes; fall back to a small default
    p->size = (size > 0.0f) ? size : 0.25f;

    if (colorStart)
        memcpy(p->colorStart, colorStart, 4 * sizeof(float));
    else
        p->colorStart[0] = p->colorStart[1] = p->colorStart[2] = p->colorStart[3] = 1.0f;

    if (colorEnd)
        memcpy(p->colorEnd, colorEnd, 4 * sizeof(float));
    else
        memcpy(p->colorEnd, p->colorStart, 4 * sizeof(float));

    return 1;
}

void particles_update(float deltaTime)
{
    // Update all our particles. 
    if (deltaTime <= 0.0f || gParticles.count == 0)
        return;

    for (int i = 0; i < gParticles.count; )
    {
        Particle *p = &gParticles.particles[i];
        p->age += deltaTime;

        if (p->age >= p->lifetime)
        {
            gParticles.particles[i] = gParticles.particles[gParticles.count - 1];
            gParticles.count--;
            continue;
        }

        p->pos[0] += p->vel[0] * deltaTime;
        p->pos[1] += p->vel[1] * deltaTime;
        p->pos[2] += p->vel[2] * deltaTime;
        ++i;
    }
}

void particles_render(void)
{
    // circles that shrink/fade over lifetime.
    //Face Camera at all times
    if (gParticles.count == 0)
        return;

    GLfloat mv[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    float right[3] = {mv[0], mv[4], mv[8]};
    float up[3]    = {mv[1], mv[5], mv[9]};

    const int segments = 24; // smoother circle

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < gParticles.count; ++i) {
        const Particle *p = &gParticles.particles[i];
        float t = p->age / p->lifetime;
        float alpha = 1.0f - t; // fade out
        float rCol = p->colorStart[0] + (p->colorEnd[0] - p->colorStart[0]) * t;
        float gCol = p->colorStart[1] + (p->colorEnd[1] - p->colorStart[1]) * t;
        float bCol = p->colorStart[2] + (p->colorEnd[2] - p->colorStart[2]) * t;
        float aCol = (p->colorStart[3] + (p->colorEnd[3] - p->colorStart[3]) * t) * alpha;

        float radius = p->size * (1.0f - t);
        if (radius < 0.02f)
            radius = 0.02f;

        glColor4f(rCol, gCol, bCol, aCol);
        float forward[3] = {
            right[1] * up[2] - right[2] * up[1],
            right[2] * up[0] - right[0] * up[2],
            right[0] * up[1] - right[1] * up[0]
        };
        float billboard[16] = {
            right[0],   right[1],   right[2],   0.0f,
            up[0],      up[1],      up[2],      0.0f,
            forward[0], forward[1], forward[2], 0.0f,
            p->pos[0],  p->pos[1],  p->pos[2],  1.0f
        };
        glPushMatrix();
        glMultMatrixf(billboard);
        drawCircle(radius, segments);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}
