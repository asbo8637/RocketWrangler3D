#include "rockets.h"

#include <stdlib.h>

//Rocket manager
static Rocket **sRockets = NULL;
static size_t sCount = 0u;
static size_t sCapacity = 0u;

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

Rocket *rockets_spawn(float x, float y, float z)
{
    if (!ensure_capacity(sCount + 1u))
        return NULL;

    Rocket *rocket = rocket_create();
    if (!rocket)
        return NULL;

    rocket_init(rocket, x, y, z);
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

void rockets_update(double deltaTime)
{
    if (sCount == 0u)
        return;

    float dt = (float)deltaTime;
    for (size_t i = 0u; i < sCount; ++i)
    {
        rocket_update(sRockets[i], dt);
    }
}

void rockets_draw(void)
{
    for (size_t i = 0u; i < sCount; ++i)
    {
        rocket_draw(sRockets[i]);
    }
}
