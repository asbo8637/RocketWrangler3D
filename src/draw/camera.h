#ifndef CAMERA_H
#define CAMERA_H

struct Joint;

// Camera position and orientation variables
extern float tpsTargetX, tpsTargetY, tpsTargetZ;  // Camera target position
extern float tpsYaw, tpsPitch, tpsDist;           // Camera orientation and distance

// Camera view function
void applyTPSView(void);

// Camera control functions (all take deltaTime in seconds)
void turnLeft(float deltaTime);
void turnRight(float deltaTime);
void lookUp(float deltaTime);
void lookDown(float deltaTime);
void zoomIn(float deltaTime);
void zoomOut(float deltaTime);

// Automatic camera update driven by game state
void camera_update(const struct Joint *robotCore, float velocityX, float velocityZ, float velocityY, double deltaTime);

#endif /* CAMERA_H */