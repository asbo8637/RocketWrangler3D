#include <GL/glut.h>
#include <math.h>
#include "cylinder.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void drawCylinder(float radius, float height, int segments) 
{
    float angle;
    int i;

    /* Draw the top circle */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, height, 0.0f);  /* Center of the top */
    
    for(i = segments; i >= 0; i--) {
        angle = 2.0f * M_PI * i / segments;
        glVertex3f(radius * cosf(angle), height, radius * sinf(angle));
    }
    glEnd();

    /* Draw the bottom circle */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);  /* Center of the bottom */
    
    for(i = 0; i <= segments; i++) {
        angle = 2.0f * M_PI * i / segments;
        glVertex3f(radius * cosf(angle), 0.0f, radius * sinf(angle));
    }
    glEnd();

    /* Draw the cylinder body */
    glBegin(GL_QUAD_STRIP);
    for(i = 0; i <= segments; i++) {
        angle = 2.0f * M_PI * i / segments;
        float x = cosf(angle);
        float z = sinf(angle);
        
        /* Normal for smooth shading */
        glNormal3f(x, 0.0f, z);
        
        /* Top vertex */
        glVertex3f(radius * x, height, radius * z);
        /* Bottom vertex */
        glVertex3f(radius * x, 0.0f, radius * z);
    }
    glEnd();
}