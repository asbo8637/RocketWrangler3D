#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include "sphere.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void drawSphere(float radius, int segments, int rings) 
{
    int i, j;
    float phi, theta;
    
    //Draw sphere by rings
    for(i = 0; i < rings; i++) {
        phi = M_PI * i / rings;
        
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= segments; j++) {
            theta = 2.0f * M_PI * j / segments;

            float x, y, z;
            
            // First ring 
            x = radius * sinf(phi) * cosf(theta);
            y = radius * cosf(phi);
            z = radius * sinf(phi) * sinf(theta);
            glNormal3f(x/radius, y/radius, z/radius);
            glVertex3f(x, y, z);
            
            // Second ring
            x = radius * sinf(phi + M_PI/rings) * cosf(theta);
            y = radius * cosf(phi + M_PI/rings);
            z = radius * sinf(phi + M_PI/rings) * sinf(theta);
            glNormal3f(x/radius, y/radius, z/radius);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
}
