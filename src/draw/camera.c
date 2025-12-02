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
#include "camera.h"
#include "../../assets/Models/joint_class.h"

// Camera variables
float tpsTargetX = 0.f, tpsTargetY = 2.0f, tpsTargetZ = -20.f;
float tpsYaw  = 1.57f;     // radians left/right orbit 
float tpsPitch = -0.35f;   // radians up/down (clamped)
float tpsDist  = 10.0f;    // distance from target (kept within clamp range)

// Camera constants 
static const float CAMERA_POS_SMOOTH_RATE = 40.0f;
static const float CAMERA_YAW_SMOOTH_RATE = 10.0f;
static const float CAMERA_DIST_SMOOTH_RATE = 5.0f;

// Helper function for camera direction
static void tpsForward(float yaw, float pitch, float *fx, float *fy, float *fz)
{
    float cy = cosf(yaw), sy = sinf(yaw);
    float cp = cosf(pitch), sp = sinf(pitch);
    if (fx) *fx =  cy * cp;
    if (fy) *fy =  sp;
    if (fz) *fz = -sy * cp;
}

// Apply the camera view transformation
void applyTPSView(void)
{
    float fx, fy, fz;
    tpsForward(tpsYaw, tpsPitch, &fx, &fy, &fz);
    float camX_ = tpsTargetX;
    float camY_ = tpsTargetY - tpsDist * fy + 1.5f; // small lift
    float camZ_ = tpsTargetZ - tpsDist * fz;

    gluLookAt(
        camX_, camY_, camZ_,
        tpsTargetX, tpsTargetY, tpsTargetZ,
        0.f, 1.f, 0.f
    );
}

void camera_update(const Joint *robotCore, float velocityX, float velocityZ, float velocityY, double deltaTime)
{
    if (!robotCore)
        return;

    const float speedMagnitude = sqrtf(velocityX * velocityX + velocityZ * velocityZ + velocityY * velocityY );
    const float dt = (float)deltaTime;
    const float desiredTargetX = robotCore->x;
    const float desiredTargetY = 5.0f + robotCore->y;
    const float desiredTargetZ = robotCore->z + 10.0f;
    const float desiredDist = -3.0f + 0.1f * fabsf(speedMagnitude);
    const float desiredYaw = 1.57f + desiredTargetX * 0.002f;
    const float basePitch = -0.08f;
    const float altitudeInfluence = 0.0015f;
    const float minPitch = -1.0f;
    const float maxPitch = -0.1f;
    
    float altitudeDelta = fmaxf(robotCore->y - 10.0f, 0.0f);
    float desiredPitch = basePitch - altitudeInfluence * altitudeDelta;
    if (desiredPitch < minPitch) desiredPitch = minPitch;
    else if (desiredPitch > maxPitch) desiredPitch = maxPitch;

    const float posAlpha = 1.0f - expf(-CAMERA_POS_SMOOTH_RATE * dt);
    const float distAlpha = 1.0f - expf(-CAMERA_DIST_SMOOTH_RATE * dt);
    const float yawAlpha = 1.0f - expf(-CAMERA_YAW_SMOOTH_RATE * dt * 0.5f);

    tpsTargetX += (desiredTargetX - tpsTargetX) * posAlpha;
    tpsTargetY += (desiredTargetY - tpsTargetY) * posAlpha;
    tpsTargetZ += (desiredTargetZ - tpsTargetZ) * posAlpha;
    tpsDist += (desiredDist - tpsDist) * distAlpha;
    tpsDist = fminf(tpsDist, 30.0f); // maximum distance
    tpsYaw += (desiredYaw - tpsYaw) * yawAlpha;
    tpsPitch += (desiredPitch - tpsPitch) * yawAlpha;
}
