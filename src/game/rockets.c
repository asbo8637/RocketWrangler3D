#include "rockets.h"

#include <stdlib.h>
#include <math.h>

//Rocket manager
static Rocket **sRockets = NULL;
static size_t sCount = 0u;
static size_t sCapacity = 0u;
static int maxRockets = 35;

// Capsule approximation of rocket body (matches drawCylinder radius 5, height 10)
static const float ROCKET_RADIUS = 0.8f;
static const float ROCKET_HEIGHT = 6.0f;

static int ensure_capacity(size_t desired)
{
    if (desired <= sCapacity)
        return 1;

    size_t newCapacity = (sCapacity == 0u) ? 4u : sCapacity * 2u;
    while (newCapacity < desired)
        newCapacity *= 2u;

    Rocket **newData = (Rocket **)realloc(sRockets, newCapacity * sizeof(Rocket *));
    if (!newData)
        return 0;

    sRockets = newData;
    sCapacity = newCapacity;
    return 1;
}

void rockets_init(void)
{
    sRockets = NULL;
    sCount = 0u;
    sCapacity = 0u;
}

void rockets_shutdown(void)
{
    if (sRockets)
    {
        for (size_t i = 0u; i < sCount; ++i)
        {
            rocket_destroy(sRockets[i]);
        }
        free(sRockets);
    }

    sRockets = NULL;
    sCount = 0u;
    sCapacity = 0u;
}

Rocket *rockets_spawn(float x, float y, float z, float vx, float vy, float vz)
{
    if (sCount >= maxRockets)
        return NULL;
    if (!ensure_capacity(sCount + 1u))
        return NULL;

    Rocket *rocket = rocket_create();
    if (!rocket)
        return NULL;

    rocket_init(rocket, x, y, z, vx, vy, vz);
    sRockets[sCount++] = rocket;
    return rocket;
}

void rockets_remove(Rocket *rocket)
{
    if (!rocket || sCount == 0u)
        return;

    for (size_t i = 0u; i < sCount; ++i)
    {
        if (sRockets[i] == rocket)
        {
            rocket_destroy(sRockets[i]);
            sRockets[i] = sRockets[sCount - 1u];
            sRockets[sCount - 1u] = NULL;
            --sCount;
            return;
        }
    }
}

size_t rockets_count(void)
{
    return sCount;
}

Rocket *rockets_get(size_t index)
{
    if (index >= sCount)
        return NULL;
    return sRockets[index];
}

//Rockets are made of 3 collision spheres. 
Rocket *rockets_findCollision(float px, float py, float pz, float playerRadius)
{
    if (sCount == 0u)
        return NULL;

    for (size_t i = 0u; i < sCount; ++i)
    {
        Rocket *rocket = sRockets[i];
        if (!rocket || !rocket->shell)
            continue;

        if(px + playerRadius < rocket->shell->x - ROCKET_RADIUS ||
           px - playerRadius > rocket->shell->x + ROCKET_RADIUS ||
           py + playerRadius < rocket->shell->y - ROCKET_RADIUS ||
           py - playerRadius > rocket->shell->y + ROCKET_RADIUS ||
           pz + playerRadius < rocket->shell->z ||
           pz - playerRadius > rocket->shell->z + ROCKET_HEIGHT)
        {
            continue;
        }

        return rocket;
    }

    return NULL;
}

void rockets_update(double deltaTime, float death_Z_zone)
{
    if (sCount == 0u)
        return;

    float dt = (float)deltaTime;
    for (size_t i = 0u; i < sCount; ++i)
    {
        if(sRockets[i]->shell->parent)
            continue;
        rocket_update(sRockets[i], dt);
        if(sRockets[i]->shell->z > death_Z_zone)
        {
            rockets_remove(sRockets[i]);
            --i;
        }
    }
}

void rockets_draw(void)
{
    for (size_t i = 0u; i < sCount; ++i)
    {
        rocket_draw(sRockets[i]);
    }
}
