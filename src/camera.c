#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include "camera.h"

/* Camera variables */
float tpsTargetX = 0.f, tpsTargetY = 2.0f, tpsTargetZ = -20.f; /* "player" anchor */
float tpsYaw  = 0.0f;     /* radians; left/right orbit */
float tpsPitch = -0.35f;   /* radians; up/down (clamped) */
float tpsDist  = 18.0f;   /* boom length (zoom) */

/* Camera constants */
static const float tpsTurnSpeed = 0.05f;
static const float tpsZoomSpeed = 1.0f;
static const float tpsMinDist = 5.0f;
static const float tpsMaxDist = 80.0f;
static const float tpsMinPitch = -1.2f;
static const float tpsMaxPitch = 1.2f;
static const float moveSpeed = 0.8f;

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

/* Camera movement functions */
void moveForward(void) {
    tpsTargetX += moveSpeed * cosf(tpsYaw);
    tpsTargetZ -= moveSpeed * sinf(tpsYaw);
}

void moveBackward(void) {
    tpsTargetX -= moveSpeed * cosf(tpsYaw);
    tpsTargetZ += moveSpeed * sinf(tpsYaw);
}

void moveLeft(void) {
    tpsTargetX -= moveSpeed * sinf(tpsYaw);
    tpsTargetZ -= moveSpeed * cosf(tpsYaw);
}

void moveRight(void) {
    tpsTargetX += moveSpeed * sinf(tpsYaw);
    tpsTargetZ += moveSpeed * cosf(tpsYaw);
}

void moveUp(void) {
    tpsTargetY += moveSpeed;
}

void moveDown(void) {
    tpsTargetY -= moveSpeed;
}

/* Camera rotation and zoom */
void turnLeft(void) {
    tpsYaw += tpsTurnSpeed;
}

void turnRight(void) {
    tpsYaw -= tpsTurnSpeed;
}

void lookUp(void) {
    tpsPitch += tpsTurnSpeed;
    if (tpsPitch > tpsMaxPitch) tpsPitch = tpsMaxPitch;
}

void lookDown(void) {
    tpsPitch -= tpsTurnSpeed;
    if (tpsPitch < tpsMinPitch) tpsPitch = tpsMinPitch;
}

void zoomIn(void) {
    tpsDist -= tpsZoomSpeed;
    if (tpsDist < tpsMinDist) tpsDist = tpsMinDist;
}

void zoomOut(void) {
    tpsDist += tpsZoomSpeed;
    if (tpsDist > tpsMaxDist) tpsDist = tpsMaxDist;
}