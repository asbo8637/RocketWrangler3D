#ifndef JOINT_H
#define JOINT_H

// A simple joint that can have child joints attached to it
typedef struct Joint {
    //position
    float x, y, z;
    
    //rotation
    float rotX, rotY, rotZ;
    
    // Parent-child relationships
    struct Joint* parent;
} Joint;

// Create a new joint at position (0,0,0)
Joint* joint_create(void);

// Free a joint
void joint_free(Joint* joint);

// Add a child to a parent joint
void joint_addChild(Joint* parent, Joint* child);

// Move a joint (and all its children)
void joint_move(Joint* joint, float dx, float dy, float dz);

// Rotate a joint (and all its children)
void joint_rotate(Joint* joint, float angleX, float angleY, float angleZ);

// Apply this joint's transform in OpenGL
void joint_applyTransform(Joint* joint);

#endif
