#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>

#include "drawScene.h"
#include "../../assets/Shapes/master.h"
#include "../../assets/Models/basic_joint.h"
#include "../../assets/Models/joint_class.h"


/* Lighting */
static float lightPos[4] = { 30.f, 25.f, 10.f, 1.f };  /* positional */
static float lightAmb[4] = { 0.1f, 0.1f, 0.1f, 1.f };
static float lightDif[4] = { 1.0f, 1.0f, 0.95f, 1.f };
static float lightSpe[4] = { 1.00f, 1.00f, 1.00f, 1.f };

/* =========================
   Basic scene functions
   ========================= */
static void drawLightGlyph(const float pos[4], float radius)
{
    float x=pos[0], y=pos[1], z=pos[2];
    float savedEmi[4]; glGetMaterialfv(GL_FRONT, GL_EMISSION, savedEmi);
    float glow[4] = {0.9f, 0.8f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glow);

    glPushMatrix();
    glTranslatef(x,y,z);
    glScalef(radius, radius, radius);
    glBegin(GL_TRIANGLES);
      glNormal3f( 1,0,0); glVertex3f(0,0, 1); glVertex3f(0, 1,0); glVertex3f(0,0,-1);
      glNormal3f(-1,0,0); glVertex3f(0,0, 1); glVertex3f(0,-1,0); glVertex3f(0,0,-1);
      glNormal3f(0, 1,0); glVertex3f(0,0, 1); glVertex3f( 1,0,0); glVertex3f(0,0,-1);
      glNormal3f(0,-1,0); glVertex3f(0,0, 1); glVertex3f(-1,0,0); glVertex3f(0,0,-1);
    glEnd();
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, savedEmi);
}

/* Function to set up basic lighting */
void setupLighting(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpe);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

#include "camera.h"

/* Function to draw a simple target marker */
static void drawTargetMarker(void)
{
    float markerSize = 1.0f;
    float markerColor[4] = {1.0f, 0.2f, 0.2f, 1.0f};
    
    glPushMatrix();
    glTranslatef(tpsTargetX, tpsTargetY, tpsTargetZ);    
    
    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, markerColor);
    
    /* Draw a small cube */
    drawBox(0.2f, 0.2f, 0.2f);
    
    glPopMatrix();
}

/* Main scene drawing function */
void drawScene(void)
{
    glEnable(GL_DEPTH_TEST);
    
    /* Set up lighting */
    setupLighting();
    
    /* Draw light glyph */
    drawLightGlyph(lightPos, 0.6f);

    /* Basic ground plane */
    glPushMatrix();
    float groundColor[4] = {0.3f, 0.5f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, groundColor);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-100.0f, -0.1f, -100.0f);
    glVertex3f(-100.0f, -0.1f,  100.0f);
    glVertex3f( 100.0f, -0.1f,  100.0f);
    glVertex3f( 100.0f, -0.1f, -100.0f);
    glEnd();
    glPopMatrix();
    
    /* Draw the target marker */
    drawTargetMarker();

        // Create two independent arms and attach arm2 to the end of arm's first segment
        static int armsInitialized = 0;
        static BasicJointArm* arm = NULL;   // segment one
        static BasicJointArm* arm2 = NULL;  // segment two (child arm)
        if (!armsInitialized) {
            // arm: one segment
            arm = bj_create(0.4f, 0.5f, 2.0f);
            bj_add_segment(arm);
            // place arm base in world space
            arm->head->joint->x = 0.0f;
            arm->head->joint->y = 2.0f;
            arm->head->joint->z = -5.0f;

            // arm2: one segment, will be attached to arm's end
            arm2 = bj_create(0.35f, 0.45f, 1.8f);
            bj_add_segment(arm2);

            // Attach arm2's root joint as a child of arm's joint[1] (end of first segment)
            Joint* parentJ = bj_get_joint(arm, 1);   // end of segment one
            Joint* childRoot = bj_get_joint(arm2, 0); // root of arm2
            if (parentJ && childRoot) {
                joint_addChild(parentJ, childRoot);
            }

            armsInitialized = 1;
        }

        // Per-frame updates
        bj_update(arm, 0.016f);   // spin base of arm slowly
        bj_update(arm2, 0.016f);  // spin base of arm2 (relative to parent)

        // Draw both arms; arm2 will render relative to parentJ
        bj_draw(arm);
        bj_draw(arm2);
}