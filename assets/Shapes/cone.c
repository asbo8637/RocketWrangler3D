#include <GL/glut.h>
#include <math.h>
#include "cone.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void drawCone(float baseRadius, float height, int segments) 
{
    float angle;
    int i;

    /* Draw the base circle */
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);  /* Center of the base */
    
    for(i = 0; i <= segments; i++) {
        angle = 2.0f * M_PI * i / segments;
        glVertex3f(baseRadius * cosf(angle), 0.0f, baseRadius * sinf(angle));
    }
    glEnd();

    /* Draw the cone surface */
    glBegin(GL_TRIANGLE_STRIP);
    
    for(i = 0; i <= segments; i++) {
        angle = 2.0f * M_PI * i / segments;
        float x = baseRadius * cosf(angle);
        float z = baseRadius * sinf(angle);
        
        /* Calculate normal for smooth shading */
        float nx = x / sqrtf(x*x + height*height);
        float ny = height / sqrtf(x*x + height*height);
        float nz = z / sqrtf(z*z + height*height);
        
        glNormal3f(nx, ny, nz);
        glVertex3f(x, 0.0f, z);  /* Base vertex */
        glVertex3f(0.0f, height, 0.0f);  /* Apex */
    }
    
    glEnd();
}