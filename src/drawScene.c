#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>

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
    
    /* Draw a colored cube at target position */
    glDisable(GL_LIGHTING);
    glColor3fv(markerColor);
    
    glBegin(GL_LINES);
    /* X axis */
    glVertex3f(-markerSize, 0, 0);
    glVertex3f(markerSize, 0, 0);
    /* Y axis */
    glVertex3f(0, -markerSize, 0);
    glVertex3f(0, markerSize, 0);
    /* Z axis */
    glVertex3f(0, 0, -markerSize);
    glVertex3f(0, 0, markerSize);
    glEnd();
    
    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, markerColor);
    
    /* Draw a small cube */
    glScalef(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    /* Front */
    glNormal3f(0, 0, 1);
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    /* Back */
    glNormal3f(0, 0, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    /* Right */
    glNormal3f(1, 0, 0);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    /* Left */
    glNormal3f(-1, 0, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, 1, -1);
    /* Top */
    glNormal3f(0, 1, 0);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
    /* Bottom */
    glNormal3f(0, -1, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
    glEnd();
    
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

    /* Ready for additional shapes and objects */
}