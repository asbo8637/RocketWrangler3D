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
#include "../../src/game/particles.h"

static unsigned int spaceshipTex = 0u;
static const float DEG2RAD = 0.01745329251994329577f; // pi/180

static void ensureRocketTextureLoaded(void)
{
    if (spaceshipTex == 0u)
        spaceshipTex = loadTexture2D("assets/Textures/Spaceship_Texture_by_svenniemannie.jpg");
}

//Helper to make explosions
static float randomFloat(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; // 0.0 to 1.0
    return min + scale * (max - min);
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
    rocket->shell->rotY = 90.0f;
}

// Helper math for transforming rocket local offsets/forward; derived with AI to speed up matrix math.
static void rotate_local_and_forward(const Joint *parent, const float local[3], float outLocal[3], float outForward[3]) {
    float rx = parent->rotX * DEG2RAD;
    float ry = parent->rotY * DEG2RAD;
    float rz = parent->rotZ * DEG2RAD;
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    // Rotate Z, then Y, then X (matches joint_applyTransform after translate)
    float x1 = local[0] * cz - local[1] * sz;
    float y1 = local[0] * sz + local[1] * cz;
    float z1 = local[2];
    float x2 =  x1 * cy + z1 * sy;
    float z2 = -x1 * sy + z1 * cy;
    float y2 =  y1;
    float y3 = y2 * cx - z2 * sx;
    float z3 = y2 * sx + z2 * cx;
    float x3 = x2;
    outLocal[0] = x3; outLocal[1] = y3; outLocal[2] = z3;

    // Rotate base forward (-Z) by same orientation
    const float baseForward[3] = {0.0f, 0.0f, -1.0f};
    x1 = baseForward[0] * cz - baseForward[1] * sz;
    y1 = baseForward[0] * sz + baseForward[1] * cz;
    z1 = baseForward[2];
    x2 =  x1 * cy + z1 * sy;
    z2 = -x1 * sy + z1 * cy;
    y2 =  y1;
    y3 = y2 * cx - z2 * sx;
    z3 = y2 * sx + z2 * cx;
    x3 = x2;
    outForward[0] = x3; outForward[1] = y3; outForward[2] = z3;
}


void rocket_thrust(Rocket *rocket)
{
    if (!rocket)
        return;

    float pos[3] = {0.0f, 0.0f, 0.0f};
    float forward[3] = {0.0f, 0.0f, -1.0f};
    const float exhaustBack = 11.0f; // emit from behind the rocket
    float vel[3] = {0.0f, 0.0f, 0.0f};

    if (rocket->shell->parent) { // If rocket is parented to robot, use parent rot + pos
        const Joint *parent = rocket->shell->parent;
        float local[3] = { rocket->shell->x, rocket->shell->y, rocket->shell->z };
        float rotatedLocal[3];
        float rotatedForward[3];
        rotate_local_and_forward(parent, local, rotatedLocal, rotatedForward);
        float l = sqrtf(rotatedForward[0]*rotatedForward[0] + rotatedForward[1]*rotatedForward[1] + rotatedForward[2]*rotatedForward[2]);
        if (l > 0.0001f) {
            forward[0] = rotatedForward[0] / l;
            forward[1] = rotatedForward[1] / l;
            forward[2] = rotatedForward[2] / l;
        }
        pos[0] = parent->x + rotatedLocal[0] - forward[0] * exhaustBack;
        pos[1] = parent->y + rotatedLocal[1] - forward[1] * exhaustBack;
        pos[2] = parent->z + rotatedLocal[2] - forward[2] * exhaustBack;
    } else {
        pos[0] = rocket->shell->x - forward[0] * exhaustBack;
        pos[1] = rocket->shell->y - forward[1] * exhaustBack;
        pos[2] = rocket->shell->z - forward[2] * exhaustBack;
    }
    //Start with a bright orange flame that fades to dark blue and shrinks over its lifetime    
    const float colorStart[4] = {1.0f, 0.5f, 0.2f, 0.8f};
    const float colorEnd[4] = {0.4f, 0.4f, 0.9f, 0.6f};
    particles_spawn(pos, vel, 0.05f, 1.0f, colorStart, colorEnd);
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

    rocket_thrust(rocket);
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
    const float finThickness = 0.3f;   // thickness
    const float finBackOffset =  bodyLength; // slide toward the tail

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


void rocket_explode(Rocket *rocket)
{
    if (!rocket)
        return;

    float pos[3];
    if(rocket->shell->parent){
        pos[0] = rocket->shell->parent->x + rocket->shell->x;
        pos[1] = rocket->shell->parent->y + rocket->shell->y;
        pos[2] = rocket->shell->parent->z + rocket->shell->z;
    } 
    else {
        pos[0] = rocket->shell->x, pos[1] = rocket->shell->y, pos[2] = rocket->shell->z;
    }
    const int numParticles = 50;
    for (int i = 0; i < numParticles; ++i)
    {
        // Random velocity in all directions
        float speed = randomFloat(40.0f, 80.0f);
        float theta = randomFloat(0.0f, 2.0f * 3.14159265f);
        float phi = acosf(randomFloat(-1.0f, 1.0f));
        float vel[3];
        vel[0] = speed * sinf(phi) * cosf(theta);
        vel[1] = speed * sinf(phi) * sinf(theta);
        vel[2] = speed * cosf(phi) - 100.0f; // add some of rocket's forward velocity

        // Start with an bright orange particle that fades to pink
        const float colorStart[4] = {1.0f, 0.5f, 0.2f, 1.0f};
        const float colorEnd[4] = {0.1f, 0.1f, 0.1f, 0.8f};
        particles_spawn(pos, vel, 0.3f, 2.5f, colorStart, colorEnd);
    }
}

//Remove the rocket and trigger an explosion if needed
void rocket_destroy(Rocket *rocket, int explode)
{
    if (!rocket)
        return;
    
    if (explode)
        rocket_explode(rocket);

    if (rocket->shell)
    {
        joint_free(rocket->shell);
        rocket->shell = NULL;
    }

    free(rocket);
}
