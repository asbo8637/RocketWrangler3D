#ifndef JOINT_H
#define JOINT_H

// A simple joint that can have child joints attached to it
typedef struct Joint
{
    // position
    float x, y, z;

    // rotation
    float rotX, rotY, rotZ;

    // Animation targets and state
    float targetRotX, targetRotY, targetRotZ;
    float angleSpeed; // degrees per second
    float angleSpeedCof; // speed coefficient multiplier
    int animatingRot; // 1 if animating rotation, 0 if not

    // Parent-child relationships
    struct Joint *parent;
} Joint;

// Create a new joint at position (0,0,0)
Joint *joint_create(void);

// Free a joint
void joint_free(Joint *joint);

// Add a child to a parent joint
void joint_addChild(Joint *parent, Joint *child);

// Move a joint (and all its children)
void joint_move(Joint *joint, float dx, float dy, float dz);

// Rotate a joint (and all its children)
void joint_rotate(Joint *joint, float angleX, float angleY, float angleZ);

// Apply this joint's transform in OpenGL
void joint_applyTransform(Joint *joint);

// Animate joint rotation to target over time
// speed: degrees per second
void joint_animateToAngle(Joint *joint, float targetRotX, float targetRotY, float targetRotZ, float speed);

// Update joint animations (call every frame)
// deltaTime: time elapsed since last frame in seconds
void joint_updateAnimation(Joint *joint, float deltaTime);

#endif
