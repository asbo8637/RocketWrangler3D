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

#include "drawScene.h"
#include "init.h"
#include "../../assets/Shapes/master.h"
#include "../../assets/Models/joint_class.h"
#include "../game/rockets.h"

// Lighting 
static float lightPos[4] = {0.f, 10000.f, 0.f, 0.f}; // Just like way up there lol
static float lightAmb[4] = {0.15f, 0.15f, 0.15f, 1.f};
static float lightDif[4] = {1.0f, 1.0f, 1.0f, 1.f};
static float lightSpe[4] = {1.00f, 1.00f, 1.00f, 1.f};
static float groundSeed = -1.0f;

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

#include "camera.h"

// Pseudo-noise based on position and a seed
static float hashNoise(float x, float z, float seed)
{
    float v = sinf((x * 12.9898f + z * 78.233f + seed) * 0.0174533f) * 43758.5453f;
    return v - floorf(v);
}

void setSeed(){
    groundSeed = (float)time(NULL);
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

static void groundColor(float h, float x, float z, float seed)
{

    float ax = fabsf(x);
    if (ax > 80.0f)
    {
        h += 100.0f;
    }
    float lerp = fminf(fmaxf(0.55f + h * 0.012f, 0.0f), 1.0f);
    float green[3] = {0.18f, 0.63f, 0.13f};
    float brown[3] = {0.40f, 0.25f, 0.10f};
    float r = green[0] * (1 - lerp) + brown[0] * lerp;
    float g = green[1] * (1 - lerp) + brown[1] * lerp;
    float b = green[2] * (1 - lerp) + brown[2] * lerp;
    // Tiny spatial jitter snapped to coarse grid
    float jx = floorf(x * 0.05f);
    float jz = floorf(z * 0.05f);
    float jitter = 0.5f * (hashNoise(jx, jz, seed) - 0.5f);
    r += jitter * 0.20f;
    g += jitter * 0.08f;
    b += jitter * -0.10f;
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
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Set up lighting
    lightPos[0] = 0.0f;   // Anchor light to world origin so it doesn't chase the player
    lightPos[2] = camZ;
    setupLighting();

    // Bumpy ground plane
    glPushMatrix();
    const float yGround = -0.1f;
    const float cellSize = 20.0f;        // Grid cell size
    const float rangeX = 800.0f;        // render span on X around camera
    const float rangeZNeg = 5000.0f;  // how far back (-Z)
    const float rangeZPos = 250.0f;      // how far forward (+Z)

    // Align grid to cell size to reduce shimmering as the camera moves
    float startX = -rangeX;
    float endX   = rangeX;
    float startZ = floorf(camZ / cellSize) * cellSize - rangeZNeg;
    float endZ   = startZ + rangeZPos + rangeZNeg;

    glNormal3f(0.0f, 1.0f, 0.0f);
    glDisable(GL_LIGHTING); // use vertex colors directly

    // Sample heights into a grid to avoid long strip diagonals
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

    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Draw the robot and all live rockets
    robot_draw(robot);
    rockets_draw();
}
