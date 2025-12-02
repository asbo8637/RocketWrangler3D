#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/glu.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "drawScene.h"
#include "init.h"
#include "../../assets/Shapes/master.h"
#include "../../assets/Models/joint_class.h"
#include "../game/rockets.h"

// Lighting 
static float lightPos[4] = {0.f, 600.f, 0.f, 1.f}; // Just like way up there lol
static float lightAmb[4] = {0.15f, 0.15f, 0.15f, 1.f};
static float lightDif[4] = {1.0f, 1.0f, 1.0f, 1.f};
static float lightSpe[4] = {1.00f, 1.00f, 1.00f, 1.f};
static float groundSeed = -1.0f;
static unsigned int seedBump = 0u;
// Brighter, warmer fog to lift the background
static const float fogColor[4] = {0.70f, 0.36f, 0.30f, 1.0f};

// Function to set up basic lighting
void setupLighting(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpe);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

static void setupFog(void)
{
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 250.0f);
    glFogf(GL_FOG_END, 1010.0f);
    glHint(GL_FOG_HINT, GL_NICEST);
}

#include "camera.h"

// Pseudo-noise based on position and a seed
static float hashNoise(float x, float z, float seed)
{
    float v = sinf((x * 12.9898f + z * 78.233f + seed) * 0.0174533f) * 43758.5453f;
    return v - floorf(v);
}

void setSeed(){
    // Keep seed in a small range so float precision isn't lost with time()
    unsigned int t = (unsigned int)time(NULL) & 0xFFFFu;
    ++seedBump;
    groundSeed = (float)t + 1.234f * (float)seedBump;
}

static void drawBackground(float camZ)
{
    if (backgroundTexture == 0u)
        return;

    // Draw a big quad with a texture far in -Z
    const float bgHalfWidth = 612.0f*0.626f;
    const float bgHeight =  408.0f*1.3f;
    const float bgZ = camZ - 1000.0f;
    const float yBase = -60.0f;

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glColor3f(1.f, 1.f, 1.f);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-bgHalfWidth, yBase + bgHeight, bgZ);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( bgHalfWidth, yBase + bgHeight, bgZ);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( bgHalfWidth, yBase, bgZ);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-bgHalfWidth, yBase, bgZ);
    glEnd();

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

float get_groundHeight(float x, float z)
{
    // Blend of layered sine noise plus hashed noise for nonrepeating bumps
    float h = 0.0f;

    float ax = fabsf(x);
    if (ax > 80.0f)
    {
        h = (ax - 80.0f) * 2.5f;   // climb rate outside center
    }

    return h;
}

// Color bands keyed to world space so movement is visible but stable
static void groundColor(float h, float x, float z, float seed)
{

    float ax = fabsf(x);
    float wallFactor = 0.0f;
    float jx = floorf(x * 0.05f); // coarse world cell
    float jz = floorf(z * 0.05f);
    float jh = floorf(h) * 0.03f;
    seed=seed-floorf(seed);
    float jitter = 0.5f * (hashNoise(jx+seed, jz+seed, seed) - 0.5f);
    if (ax > 80.0f)
    {
        wallFactor = 0.8f;
        jitter = 0.5f * (hashNoise(jh, jx, 1) - 0.5f);
    }
    // world-anchored jitter so colors slide consistently as you move
    float lerp = fmaxf(0.2f + sinf(h*seed*0.2f), wallFactor);
    float Sand[3] = {0.8f, 0.60f, 0.40f};
    float DarkRed[3] = {0.63f, 0.15f, 0.03f};
    float r = Sand[0] * (1 - lerp) + DarkRed[0] * lerp;
    float g = Sand[1] * (1 - lerp) + DarkRed[1] * lerp;
    float b = Sand[2] * (1 - lerp) + DarkRed[2] * lerp;
    // Tiny spatial jitter snapped to coarse grid
    r += jitter * 0.20f;
    g += jitter * 0.08f;
    b += jitter * -0.05f;
    if (r < 0.f) r = 0.f; 
    if (r > 1.f) r = 1.f;
    if (g < 0.f) g = 0.f; 
    if (g > 1.f) g = 1.f;
    if (b < 0.f) b = 0.f; 
    if (b > 1.f) b = 1.f;
    glColor3f(r, g, b);
}


// Main scene drawing function
void drawScene(float camZ)
{
    drawBackground(camZ);
    setupFog();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Set up lighting
    lightPos[2] = camZ-200.0f;
    setupLighting();

    // Bumpy ground plane
    glPushMatrix();
    const float yGround = -0.1f;
    const float cellSize = 8.0f;        // Grid cell size
    const float rangeX = 300.0f;        // render span on X around camera
    const float rangeZNeg = 1000.0f;  // how far back (-Z)
    const float rangeZPos = 250.0f;      // how far forward (+Z)

    // Align grid to cell size to reduce shimmering as the camera moves
    float startX = -rangeX;
    float endX   = rangeX;
    float startZ = floorf(camZ / cellSize) * cellSize - rangeZNeg;
    float endZ   = startZ + rangeZPos + rangeZNeg;

    glNormal3f(0.0f, 1.0f, 0.0f);
    glDisable(GL_LIGHTING); // use vertex colors directly

    int countX = (int)((endX - startX) / cellSize) + 2;
    int countZ = (int)((endZ - startZ) / cellSize) + 2;
    float *heights = (float *)malloc((size_t)countX * (size_t)countZ * sizeof(float));
    if (heights)
    {
        for (int zi = 0; zi < countZ; ++zi)
        {
            float z = startZ + zi * cellSize;
            for (int xi = 0; xi < countX; ++xi)
            {
                float x = startX + xi * cellSize;
                heights[zi * countX + xi] = get_groundHeight(x, z);
            }
        }

        // Render quads as two tris each
        for (int zi = 0; zi < countZ - 1; ++zi)
        {
            float z0 = startZ + zi * cellSize;
            float z1 = z0 + cellSize;
            glBegin(GL_TRIANGLES);
            for (int xi = 0; xi < countX - 1; ++xi)
            {
                float x0 = startX + xi * cellSize;
                float x1 = x0 + cellSize;
                float h00 = heights[zi * countX + xi];
                float h10 = heights[zi * countX + (xi + 1)];
                float h01 = heights[(zi + 1) * countX + xi];
                float h11 = heights[(zi + 1) * countX + (xi + 1)];

                if (((xi + zi) & 1) == 0)
                {
                    groundColor(h00, x0, z0, groundSeed); glVertex3f(x0, yGround + h00, z0);
                    groundColor(h10, x1, z0, groundSeed); glVertex3f(x1, yGround + h10, z0);
                    groundColor(h11, x1, z1, groundSeed); glVertex3f(x1, yGround + h11, z1);

                    groundColor(h00, x0, z0, groundSeed); glVertex3f(x0, yGround + h00, z0);
                    groundColor(h11, x1, z1, groundSeed); glVertex3f(x1, yGround + h11, z1);
                    groundColor(h01, x0, z1, groundSeed); glVertex3f(x0, yGround + h01, z1);
                }
                else
                {
                    groundColor(h00, x0, z0, groundSeed); glVertex3f(x0, yGround + h00, z0);
                    groundColor(h10, x1, z0, groundSeed); glVertex3f(x1, yGround + h10, z0);
                    groundColor(h01, x0, z1, groundSeed); glVertex3f(x0, yGround + h01, z1);

                    groundColor(h10, x1, z0, groundSeed); glVertex3f(x1, yGround + h10, z0);
                    groundColor(h11, x1, z1, groundSeed); glVertex3f(x1, yGround + h11, z1);
                    groundColor(h01, x0, z1, groundSeed); glVertex3f(x0, yGround + h01, z1);
                }
            }
            glEnd();
        }
        free(heights);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Draw the robot and all live rockets
    robot_draw(robot);
    rockets_draw();
}
