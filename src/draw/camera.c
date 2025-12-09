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
#include "../game/controls.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Camera variables
float tpsTargetX = 0.f, tpsTargetY = 2.0f, tpsTargetZ = -20.f;
float tpsYaw  = 1.57f;     // radians left/right orbit 
float tpsPitch = -0.35f;   // radians up/down (clamped)
float tpsDist  = 10.0f;    // distance from target 

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

// Apply manual control input to the camera
void camera_update_manual(double deltaTime)
{
    const float manualYawSpeed = 1.5f;    // rad/s
    const float manualPitchSpeed = 1.5f;  // rad/s
    const float manualMoveSpeed = 30.0f;  // units/s (translate only when paused)

    tpsYaw += controlState.cameraYaw   * manualYawSpeed   * (float)deltaTime;
    tpsPitch += controlState.cameraPitch * manualPitchSpeed * (float)deltaTime;
    if (tpsPitch > 1.3f) tpsPitch = 1.3f;
    if (tpsPitch < -1.3f) tpsPitch = -1.3f;

    // Move relative to where we are looking (yaw-based forward/strafe)
    const float moveForward = controlState.camMoveZ * manualMoveSpeed * (float)deltaTime;
    const float moveRight = controlState.camMoveX * manualMoveSpeed * (float)deltaTime;
    const float moveUp = controlState.camMoveY * manualMoveSpeed * (float)deltaTime;
    const float sinYaw = sinf(tpsYaw);
    const float cosYaw = cosf(tpsYaw);

    // Forward points where the camera is facing on the XZ plane; right is perpendicular
    tpsTargetX += moveForward * cosYaw + moveRight * sinYaw;
    tpsTargetZ += moveForward * -sinYaw + moveRight * cosYaw;
    tpsTargetY += moveUp;

}

// Apply the camera view transformation
void applyTPSView(void)
{
    float fx, fy, fz;
    tpsForward(tpsYaw, tpsPitch, &fx, &fy, &fz);
    float camX_ = tpsTargetX - tpsDist * fx;
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
    const float desiredYaw = 1.57f + desiredTargetX * 0.001f;
    const float basePitch = -0.08f;
    const float altitudeInfluence = 0.0019f;
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
    tpsDist = fminf(tpsDist, 25.0f); // maximum distance
    tpsYaw += (desiredYaw - tpsYaw) * yawAlpha;
    tpsPitch += (desiredPitch - tpsPitch) * yawAlpha;

    if(controlState.perspectiveToggle != 0.0f){
        tpsTargetY  = 175.0f;
        tpsTargetX = 0.0f;
        tpsPitch = -0.7f;
        tpsYaw = 1.57f;
        tpsTargetZ = robotCore->z+50.0f;
        tpsDist = 60.0f;
    }

}
