#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>

#include "drawScene.h"
#include "init.h"
#include "../../assets/Shapes/master.h"
#include "../../assets/Models/joint_class.h"

/* Lighting */
static float lightPos[4] = {30.f, 25.f, 10.f, 1.f}; /* positional */
static float lightAmb[4] = {0.1f, 0.1f, 0.1f, 1.f};
static float lightDif[4] = {1.0f, 1.0f, 0.95f, 1.f};
static float lightSpe[4] = {1.00f, 1.00f, 1.00f, 1.f};

/* =========================
   Basic scene functions
   ========================= */
static void drawLightGlyph(const float pos[4], float radius)
{
    float x = pos[0], y = pos[1], z = pos[2];
    float savedEmi[4];
    glGetMaterialfv(GL_FRONT, GL_EMISSION, savedEmi);
    float glow[4] = {0.9f, 0.8f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glow);

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, radius, radius);
    glBegin(GL_TRIANGLES);
    glNormal3f(1, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, -1);
    glNormal3f(-1, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(0, -1, 0);
    glVertex3f(0, 0, -1);
    glNormal3f(0, 1, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(1, 0, 0);
    glVertex3f(0, 0, -1);
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(-1, 0, 0);
    glVertex3f(0, 0, -1);
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
void drawScene(double deltaTime)
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
    glVertex3f(-100.0f, -0.1f, 100.0f);
    glVertex3f(100.0f, -0.1f, 100.0f);
    glVertex3f(100.0f, -0.1f, -100.0f);
    glEnd();
    glPopMatrix();

    /* Draw the target marker */
    drawTargetMarker();

    /* In-air animation with spinning and random poses */
    robot_inAirAnimation(robot, deltaTime, 270.0f, 0.2f);

    robot_update(robot, deltaTime);
    robot_draw(robot);
}