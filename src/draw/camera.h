#ifndef CAMERA_H
#define CAMERA_H

/* Camera position and orientation variables */
extern float tpsTargetX, tpsTargetY, tpsTargetZ;  /* Camera target position */
extern float tpsYaw, tpsPitch, tpsDist;           /* Camera orientation and distance */

/* Camera view function */
void applyTPSView(void);

/* Camera control functions (all take deltaTime in seconds) */
void turnLeft(float deltaTime);
void turnRight(float deltaTime);
void lookUp(float deltaTime);
void lookDown(float deltaTime);
void zoomIn(float deltaTime);
void zoomOut(float deltaTime);

#endif /* CAMERA_H */