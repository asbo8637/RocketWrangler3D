#include "joint_class.h"
#include <GL/glut.h>
#include <stdlib.h>

// Create a new joint at position (0,0,0)
Joint* joint_create(void) {
    Joint* joint = (Joint*)malloc(sizeof(Joint));
    
    //origin set to 0
    joint->x = joint->y = joint->z = 0.0f;
    joint->rotX = joint->rotY = joint->rotZ = 0.0f;
    
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
