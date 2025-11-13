#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include "camera.h"

/* Camera variables */
float tpsTargetX = 0.f, tpsTargetY = 2.0f, tpsTargetZ = -20.f;
float tpsYaw  = 1.57f;     // radians left/right orbit 
float tpsPitch = -0.35f;   // radians up/down (clamped)
float tpsDist  = 25.0f;   // distance from target

/* Camera constants */
static const float baseTurnSpeed = 2.0f;    /* Radians per second */
static const float baseZoomSpeed = 20.0f;   /* Units per second */
static const float tpsMinDist = 5.0f;
static const float tpsMaxDist = 80.0f;
static const float tpsMinPitch = -1.2f;
static const float tpsMaxPitch = 1.2f;

/* Helper function for camera direction */
static void tpsForward(float yaw, float pitch, float *fx, float *fy, float *fz)
{
    float cy = cosf(yaw), sy = sinf(yaw);
    float cp = cosf(pitch), sp = sinf(pitch);
    if (fx) *fx =  cy * cp;
    if (fy) *fy =  sp;
    if (fz) *fz = -sy * cp;
}

/* Apply the camera view transformation */
void applyTPSView(void)
{
    float fx, fy, fz;
    tpsForward(tpsYaw, tpsPitch, &fx, &fy, &fz);
    float camX_ = tpsTargetX - tpsDist * fx;
    float camY_ = tpsTargetY - tpsDist * fy + 1.5f; /* small lift */
    float camZ_ = tpsTargetZ - tpsDist * fz;

    gluLookAt(
        camX_, camY_, camZ_,
        tpsTargetX, tpsTargetY, tpsTargetZ,
        0.f, 1.f, 0.f
    );
}

/* Camera rotation and zoom */
void turnLeft(float deltaTime) {
    tpsYaw += baseTurnSpeed * deltaTime;
}

void turnRight(float deltaTime) {
    tpsYaw -= baseTurnSpeed * deltaTime;
}

void lookUp(float deltaTime) {
    tpsPitch += baseTurnSpeed * deltaTime;
    if (tpsPitch > tpsMaxPitch) tpsPitch = tpsMaxPitch;
}

void lookDown(float deltaTime) {
    tpsPitch -= baseTurnSpeed * deltaTime;
    if (tpsPitch < tpsMinPitch) tpsPitch = tpsMinPitch;
}

void zoomIn(float deltaTime) {
    tpsDist -= baseZoomSpeed * deltaTime;
    if (tpsDist < tpsMinDist) tpsDist = tpsMinDist;
}

void zoomOut(float deltaTime) {
    tpsDist += baseZoomSpeed * deltaTime;
    if (tpsDist > tpsMaxDist) tpsDist = tpsMaxDist;
}