#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include "circle.h"

void drawCircle(float radius, int segments)
{
    if (segments < 3)
        segments = 3;

    const float deltaTheta = (2.0f * 3.14159265f) / (float)segments;

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f); // center

    for (int i = 0; i <= segments; ++i)
    {
        float theta = i * deltaTheta;
        float s = sinf(theta);
        float c = cosf(theta);
        float x = radius * c;
        float y = radius * s;
        glTexCoord2f(0.5f + 0.5f * c, 0.5f + 0.5f * s);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
}
