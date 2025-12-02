#include "Robot.h"
#include "../Shapes/master.h"
#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "./Robot_positions/poses.h"
#include <stdlib.h>
#include <math.h>
#include "Rocket.h"
#include "../../src/draw/texture.h"

static unsigned int spaceshipTex = 0u;

static void ensureRocketTextureLoaded(void)
{
    if (spaceshipTex == 0u)
        spaceshipTex = loadTexture2D("assets/Textures/Spaceship_Texture_by_svenniemannie.jpg");
}

// Create a new rocket instance
Rocket *rocket_create(void)
{
    Rocket *rocket = (Rocket *)malloc(sizeof(Rocket));
    if (!rocket)
        return NULL;

    rocket->shell = NULL;
    rocket->rocket_velocityX = 0.0f;
    rocket->rocket_velocityY = 0.0f;
    rocket->rocket_velocityZ = 0.0f;

    return rocket;
}

void rocket_init(Rocket *rocket, float x, float y, float z, float vx, float vy, float vz)
{
    if (!rocket)
        return;

    // Create joints
    rocket->shell = joint_create();

    // Position shell joint in world (root of entire rocket)
    rocket->shell->x = x;
    rocket->shell->y = y;
    rocket->shell->z = z;
    
    rocket->rocket_velocityX = vx;
    rocket->rocket_velocityY = vy;
    rocket->rocket_velocityZ = vz;

    rocket->shell->rotX = 90.0f;
    rocket->shell->rotY = x*y*z;
}

// Update rocket animation
void rocket_update(Rocket *rocket, float deltaTime)
{
    if (!rocket)
        return;
    // Update rocket position based on velocity
    rocket->shell->x += rocket->rocket_velocityX * deltaTime;
    rocket->shell->y += rocket->rocket_velocityY * deltaTime;
    rocket->shell->z += rocket->rocket_velocityZ * deltaTime;
}

// Draw the rocket
void rocket_draw(const Rocket *rocket)
{
    if (!rocket)
        return;

    const float baseRadius = 1.0f;
    const float bodyLength = 10.0f;

    ensureRocketTextureLoaded();

    // Brushed-metal look
    // Slight green tint on the metal
    float rocketDiffuse[4]  = {0.55f, 0.70f, 0.58f, 1.0f};
    float rocketSpecular[4] = {0.95f, 0.95f, 0.95f, 1.0f};
    float rocketEmission[4] = {0.03f, 0.03f, 0.04f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rocketDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, rocketSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, rocketEmission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96.0f);

    // Draw rocket shell
    glPushMatrix();
    joint_applyTransform(rocket->shell);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, spaceshipTex);
    glColor3f(0.85f, 1.00f, 0.85f); // tint the texture with a light green cast
    drawCylinder(baseRadius, bodyLength, 16);

    glRotatef(180.0f, 1.f, 0.f, 0.f);
    drawCone(baseRadius, 8.0f, 16); // Nose
    glRotatef(180.0f, 1.f, 0.f, 0.f);


    // Three fins around the base using small prisms
    const float finWidth = 2.2f;       // span away from the hull
    const float finHeight = 1.5f;      // rises up the body
    const float finThickness = 0.3f;   // thickness along tangential direction
    const float finBackOffset =  bodyLength; // slide toward the tail along the 10.0f body

    for (int i = 0; i < 3; ++i)
    {
        float angle = 120.0f * (float)i;
        glPushMatrix();
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
        // Position fin so its inner face kisses the hull
        glTranslatef(baseRadius + finWidth * 0.5f - 0.22f, finBackOffset + 0.2f, 0.0f);
        glRotatef(36.0f, 0.0f, 0.0f, 1.0f); // Align fin vertically
        drawPrism(finWidth, finHeight, finThickness);
        glPopMatrix();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
}

void rocket_destroy(Rocket *rocket)
{
    if (!rocket)
        return;

    if (rocket->shell)
    {
        joint_free(rocket->shell);
        rocket->shell = NULL;
    }

    free(rocket);
}
