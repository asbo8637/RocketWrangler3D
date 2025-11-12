#include "joint_class.h"
#include <GL/glut.h>
#include <stdlib.h>

// Create a new joint at position (0,0,0)
Joint* joint_create(void) {
    Joint* joint = (Joint*)malloc(sizeof(Joint));
    
    //origin set to 0
    joint->x = joint->y = joint->z = 0.0f;
    joint->rotX = joint->rotY = joint->rotZ = 0.0f;
    
    // Initialize animation state
    joint->targetRotX = joint->targetRotY = joint->targetRotZ = 0.0f;
    joint->angleSpeed = 45.0f;
    joint->animatingRot = 1;
    
    //No parent
    joint->parent = NULL;
    return joint;
}

//Free joint
void joint_free(Joint* joint) {
    if (joint) free(joint);
}

//Add child
void joint_addChild(Joint* parent, Joint* child) {
    if (!parent || !child) return;
    
    child->parent = parent;
}

// Move a joint
void joint_move(Joint* joint, float dx, float dy, float dz) {
    if (!joint) return;
    joint->x += dx;
    joint->y += dy;
    joint->z += dz;
}

// Rotate a joint
void joint_rotate(Joint* joint, float angleX, float angleY, float angleZ) {
    if (!joint) return;
    
    joint->rotX += angleX;
    joint->rotY += angleY;
    joint->rotZ += angleZ;
}

// Apply this joint's transform in OpenGL
void joint_applyTransform(Joint* joint) {
    if (!joint) return;
    
    // Apply parent transform first (so we're relative to parent)
    if (joint->parent) {
        joint_applyTransform(joint->parent);
    }
    
    // Then apply our transform
    // Order matters! Translate first, then rotate
    // (OpenGL reads right-to-left, so this rotates around our position)
    glTranslatef(joint->x, joint->y, joint->z);
    glRotatef(joint->rotX, 1, 0, 0);  // Rotate around X axis
    glRotatef(joint->rotY, 0, 1, 0);  // Rotate around Y axis
    glRotatef(joint->rotZ, 0, 0, 1);  // Rotate around Z axis
}

// Helper: linearly interpolate towards target
static float lerp_towards(float current, float target, float maxDelta) {
    float diff = target - current;
    if (diff > maxDelta) return current + maxDelta;
    if (diff < -maxDelta) return current - maxDelta;
    return target; // reached target
}

// Animate joint rotation to target over time
void joint_animateToAngle(Joint* joint, float targetRotX, float targetRotY, float targetRotZ, float speed) {
    if (!joint) return;
    joint->targetRotX = targetRotX;
    joint->targetRotY = targetRotY;
    joint->targetRotZ = targetRotZ;
    joint->angleSpeed = speed;
}

// Update joint animations (call every frame)
void joint_updateAnimation(Joint* joint, float deltaTime) {
    if (!joint) return;
    
    // Update rotation animation
    if (joint->animatingRot) {
        float maxDelta = joint->angleSpeed * deltaTime;
        joint->rotX = lerp_towards(joint->rotX, joint->targetRotX, maxDelta);
        joint->rotY = lerp_towards(joint->rotY, joint->targetRotY, maxDelta);
        joint->rotZ = lerp_towards(joint->rotZ, joint->targetRotZ, maxDelta);
    }
}
