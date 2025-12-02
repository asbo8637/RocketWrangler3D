#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include "hat.h"
#include "cylinder.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Rectangle brim with upturned semicircle sides and an oval crown.
void drawCowboyHat(float brimWidth, float brimDepth, float brimSideRadius, float brimTiltDeg,
                   float crownRadius, float crownHeight,
                   int segments)
{
    if (segments < 6)
        segments = 6;

    const float halfW = brimWidth * 0.5f;
    const float halfD = brimDepth * 0.5f;
    const float sideR = fmaxf(brimSideRadius, 0.0f);

    // Flat center of the brim (lies in XZ plane)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0.0f, -halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfW, 0.0f, -halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfW, 0.0f,  halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfW, 0.0f,  halfD);
    glEnd();

    // Helper to draw one side semicircle, lifted by tilt around Z.
    for (int side = -1; side <= 1; side += 2)
    {
        glPushMatrix();
        glTranslatef((float)side * halfW, 0.0f, 0.0f);
        glRotatef((float)side * brimTiltDeg, 0.0f, 0.0f, 1.0f);

        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, 0.0f, 0.0f); // center at the side edge

        // Arc from back (-Z) to front (+Z) bulging outward along X
        for (int i = 0; i <= segments; ++i)
        {
            float t = (float)i / (float)segments;
            float angle = -0.5f * (float)M_PI + t * (float)M_PI; // -90..+90 deg
            float c = cosf(angle);
            float s = sinf(angle);
            float x = (float)side * sideR * c;
            float z = halfD * s;
            glTexCoord2f(0.5f + 0.5f * c, 0.5f + 0.5f * s);
            glVertex3f(x, 0.0f, z);
        }
        glEnd();
        glPopMatrix();
    }

    // Crown: oval cylinder sitting on the brim
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    drawCylinder(crownRadius, crownHeight, segments);
    glPopMatrix();
}
