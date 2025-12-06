// Shape helper originally generated with AI and then tweaked (normals/UVs).
#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include "cone.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void drawCone(float baseRadius, float height, int segments) 
{
    float angle;
    int i;

    // Draw the base circle
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);  // Center of the base
    
    for(i = 0; i <= segments; i++) {
        angle = 2.0f * M_PI * i / segments;
        float u = 0.5f + 0.5f * cosf(angle);
        float v = 0.5f + 0.5f * sinf(angle);
        glTexCoord2f(u, v);
        glVertex3f(baseRadius * cosf(angle), 0.0f, baseRadius * sinf(angle));
    }
    glEnd();

    // Draw the cone surface
    glBegin(GL_TRIANGLE_STRIP);
    
    for(i = 0; i <= segments; i++) {
        angle = 2.0f * M_PI * i / segments;
        float x = baseRadius * cosf(angle);
        float z = baseRadius * sinf(angle);

        // Use a constant-length normal so specular lighting is even around the cone
        float invLen = 1.0f / sqrtf(baseRadius * baseRadius + height * height);
        float nx = x * height * invLen / baseRadius;
        float ny = baseRadius * invLen;
        float nz = z * height * invLen / baseRadius;
        glNormal3f(nx, ny, nz);
        glTexCoord2f((float)i / (float)segments, 1.0f);
        glVertex3f(x, 0.0f, z);  // Base vertex
        glTexCoord2f((float)i / (float)segments, 0.0f);
        glVertex3f(0.0f, height, 0.0f);  // Apex
    }
    
    glEnd();
}
