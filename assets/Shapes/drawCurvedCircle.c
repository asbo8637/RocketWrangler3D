#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef struct { float x,y,z; } Vec3;

//Draw the faces of the box
static void drawFace(const Vec3 v[4], const Vec3 n)
{
    glNormal3f(n.x, n.y, n.z);
    glBegin(GL_TRIANGLES);
      // tri 1: v0 -> v1 -> v2   (UVs: (0,0) (1,0) (1,1))
      glTexCoord2f(0.f,0.f); glVertex3f(v[0].x, v[0].y, v[0].z);
      glTexCoord2f(1.f,0.f); glVertex3f(v[1].x, v[1].y, v[1].z);
      glTexCoord2f(1.f,1.f); glVertex3f(v[2].x, v[2].y, v[2].z);
      // tri 2: v0 -> v2 -> v3   (UVs: (0,0) (1,1) (0,1))
      glTexCoord2f(0.f,0.f); glVertex3f(v[0].x, v[0].y, v[0].z);
      glTexCoord2f(1.f,1.f); glVertex3f(v[2].x, v[2].y, v[2].z);
      glTexCoord2f(0.f,1.f); glVertex3f(v[3].x, v[3].y, v[3].z);
    glEnd();
}


void drawCurvedCircle(float radius, float height, int segments)
{
    if (segments < 3)
        segments = 3;

    const float deltaTheta = (2.0f * 3.14159265f) / (float)segments;

    for (int i = 0; i < segments; ++i)
    {
        float theta0 = i * deltaTheta;
        float theta1 = (i + 1) * deltaTheta;

        Vec3 v[4];
        v[0].x = radius * cosf(theta0); v[0].y = 0.0f; v[0].z = radius * sinf(theta0);
        v[1].x = radius * cosf(theta1); v[1].y = 0.0f; v[1].z = radius * sinf(theta1);
        v[2].x = 0.0f;                   v[2].y = radius; v[2].z = 0.0f;
        v[3] = v[0];

        Vec3 normal;
        normal.x = (v[0].x + v[1].x) / 2.0f;
        normal.y = radius / 2.0f;
        normal.z = (v[0].z + v[1].z) / 2.0f;
        float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        normal.x /= length; normal.y /= length; normal.z /= length;

        drawFace(v, normal);
    }
}
