#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
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

// Centered at origin; w=width (X), h=height (Y), d=depth (Z)
void drawBox(float w, float h, float d)
{
    const float hx = 0.5f*w, hy = 0.5f*h, hz = 0.5f*d;

    // Define 6 faces as 4 corners each
    {
        // +X (right)
        Vec3 n = {+1,0,0};
        Vec3 v[4] = {
            {+hx,-hy,-hz}, {+hx,-hy,+hz}, {+hx,+hy,+hz}, {+hx,+hy,-hz}
        };
        drawFace(v,n);
    }
    {
        // -X (left)
        Vec3 n = {-1,0,0};
        Vec3 v[4] = {
            {-hx,-hy,+hz}, {-hx,-hy,-hz}, {-hx,+hy,-hz}, {-hx,+hy,+hz}
        };
        drawFace(v,n);
    }
    {
        // +Y (top)
        Vec3 n = {0,+1,0};
        Vec3 v[4] = {
            {-hx,+hy,-hz}, {+hx,+hy,-hz}, {+hx,+hy,+hz}, {-hx,+hy,+hz}
        };
        drawFace(v,n);
    }
    {
        // -Y (bottom)
        Vec3 n = {0,-1,0};
        Vec3 v[4] = {
            {-hx,-hy,+hz}, {+hx,-hy,+hz}, {+hx,-hy,-hz}, {-hx,-hy,-hz}
        };
        drawFace(v,n);
    }
    {
        // +Z (front)
        Vec3 n = {0,0,+1};
        Vec3 v[4] = {
            {+hx,-hy,+hz}, {-hx,-hy,+hz}, {-hx,+hy,+hz}, {+hx,+hy,+hz}
        };
        drawFace(v,n);
    }
    {
        // -Z (back)
        Vec3 n = {0,0,-1};
        Vec3 v[4] = {
            {-hx,-hy,-hz}, {+hx,-hy,-hz}, {+hx,+hy,-hz}, {-hx,+hy,-hz}
        };
        drawFace(v,n);
    }
}
