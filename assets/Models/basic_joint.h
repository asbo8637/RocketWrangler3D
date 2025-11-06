#ifndef BASIC_JOINT_H
#define BASIC_JOINT_H

#include "joint_class.h"

// Internal node to keep an ordered chain
typedef struct BJNode {
    Joint* joint;         // underlying transform joint
    struct BJNode* next;  // next in chain
} BJNode;

// Basic joint arm "class"
typedef struct BasicJointArm {
    BJNode* head;     // first joint in chain
    BJNode* tail;     // last joint (end effector)
    int segments;     // number of segments (edges) between joints

    // Visual parameters
    float radius;     // sphere radius for joints
    float width;      // box width/depth for segments
    float length;     // default segment length (child offset in +Y)
} BasicJointArm;

// Create a new arm with given visuals and segment length
BasicJointArm* bj_create(float radius, float width, float length);

// Append one segment at the end of the chain
void bj_add_segment(BasicJointArm* arm);

// Destroy the entire arm and its joints
void bj_destroy(BasicJointArm* arm);

// Optional: animate/update the arm
void bj_update(BasicJointArm* arm, float dt);

// Draw the arm (spheres at joints, boxes for segments)
void bj_draw(const BasicJointArm* arm);

// Helpers: access nodes/joints by index (0 = head/root)
BJNode* bj_get_node(BasicJointArm* arm, int index);
Joint*  bj_get_joint(BasicJointArm* arm, int index);

#endif // BASIC_JOINT_H
