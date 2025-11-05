#ifndef CAMERA_H
#define CAMERA_H

/* Camera position and orientation variables */
extern float tpsTargetX, tpsTargetY, tpsTargetZ;  /* Camera target position */
extern float tpsYaw, tpsPitch, tpsDist;           /* Camera orientation and distance */

/* Camera view function */
void applyTPSView(void);

/* Movement functions */
void moveForward(void);
void moveBackward(void);
void moveLeft(void);
void moveRight(void);
void moveUp(void);
void moveDown(void);

/* Camera control functions */
void turnLeft(void);
void turnRight(void);
void lookUp(void);
void lookDown(void);
void zoomIn(void);
void zoomOut(void);

#endif /* CAMERA_H */