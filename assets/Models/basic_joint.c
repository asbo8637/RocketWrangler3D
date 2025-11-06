// Basic joint "class" — a simple chain of joints you can extend
#include "basic_joint.h"
#include "joint_class.h"
#include "../Shapes/master.h"
#include <GL/glut.h>
#include <stdlib.h>

// Create a new basic joint arm with visuals
// radius: sphere radius to draw at each joint
// width:  box width/depth for each arm segment
// length: length of each segment along +Y from parent to child
BasicJointArm* bj_create(float radius, float width, float length)
{
    BasicJointArm* arm = (BasicJointArm*)malloc(sizeof(BasicJointArm));
    if (!arm) return NULL;

    arm->radius = radius;
    arm->width  = width;
    arm->length = length;
    arm->segments = 0;
    arm->head = NULL;
    arm->tail = NULL;

    // Create root joint at origin; user can move/rotate arm->base from outside
    BJNode* root = (BJNode*)malloc(sizeof(BJNode));
    if (!root) { free(arm); return NULL; }
    root->joint = joint_create();
    root->next  = NULL;

    arm->head = root;
    arm->tail = root;
    arm->segments = 0; // 0 segments between joints initially

    return arm;
}

// Add one segment/joint to the end of the chain
void bj_add_segment(BasicJointArm* arm)
{
    if (!arm || !arm->tail) return;

    // New joint offset from tail by arm->length in local +Y
    Joint* j = joint_create();
    j->y = arm->length;
    joint_addChild(arm->tail->joint, j);

    // Track in our chain list
    BJNode* node = (BJNode*)malloc(sizeof(BJNode));
    node->joint = j;
    node->next  = NULL;
    arm->tail->next = node;
    arm->tail = node;
    arm->segments++;
}

// Free the arm and all joints
void bj_destroy(BasicJointArm* arm)
{
    if (!arm) return;
    BJNode* n = arm->head;
    while (n) {
        BJNode* nxt = n->next;
        joint_free(n->joint);
        free(n);
        n = nxt;
    }
    free(arm);
}

// Simple update hook (optional animation)
void bj_update(BasicJointArm* arm, float dt)
{
    if (!arm || !arm->head) return;
    // Example: slowly spin the base around Z
    arm->head->joint->rotZ += dt * 20.0f;
}

// Draw the chain: spheres at joints, boxes as segments between joints
void bj_draw(const BasicJointArm* arm)
{
    if (!arm || !arm->head) return;

    const BJNode* n = arm->head;
    while (n) {
        const Joint* j = n->joint;

        // Draw this joint (sphere)
        glPushMatrix();
        joint_applyTransform((Joint*)j);
        drawSphere(arm->radius, 16, 16);

        // If there is a child, draw the segment box halfway to child
        if (n->next) {
            const Joint* child = n->next->joint;
            float dy = child->y; // local offset along +Y from parent to child
            glPushMatrix();
            glTranslatef(0.0f, dy * 0.5f, 0.0f);
            drawBox(arm->width, dy, arm->width);
            glPopMatrix();
        }

        glPopMatrix();
        n = n->next;
    }
}

// Return the Nth node (0 = head). Returns NULL if out of range.
BJNode* bj_get_node(BasicJointArm* arm, int index)
{
    if (!arm || index < 0) return NULL;
    BJNode* n = arm->head;
    int i = 0;
    while (n && i < index) {
        n = n->next;
        ++i;
    }
    return n; // may be NULL if index beyond tail
}

// Return the Joint* for the Nth node, or NULL if not found
Joint* bj_get_joint(BasicJointArm* arm, int index)
{
    BJNode* n = bj_get_node(arm, index);
    return n ? n->joint : NULL;
}