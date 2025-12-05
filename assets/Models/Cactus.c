#include "Cactus.h"
#include <math.h>
#include <stdlib.h>
#include "../../src/draw/texture.h"
#include "../Shapes/sphere.h"
#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static unsigned int cactusTex = 0u;



// Analytical normal to help with rings. 
static inline void bentAnalyticNormal(float theta, float t,
                                      float length, float bend, float radius,
                                      float N[3])
{
    float c = cosf(theta), s = sinf(theta);
    float bx = bend * (float)M_PI * cosf((float)M_PI * t); // d(x_c)/dt

    float Nx = radius * c * length;
    float Ny = -radius * c * bx;
    float Nz = radius * s * length;

    float L = sqrtf(Nx*Nx + Ny*Ny + Nz*Nz);
    if (L < 1e-20f) L = 1.0f;
    N[0] = Nx / L; N[1] = Ny / L; N[2] = Nz / L;
}

//Make rings, to fix normals. 
static void makeRing(int slices, float t, float length, float bend, float radius,
                     float (*pos)[3], float (*nor)[3])
{
    float y  = t * length;
    float xc = bend * sinf((float)M_PI * t);

    for (int s = 0; s < slices; ++s)
    {
        float a = (float)s / (float)slices * 2.0f * (float)M_PI;
        float c = cosf(a), si = sinf(a);

        pos[s][0] = xc + radius * c;
        pos[s][1] = y;
        pos[s][2] = radius * si;

        bentAnalyticNormal(a, t, length, bend, radius, nor[s]);
    }
    // duplicate first exactly (no trig) to kill the seam
    pos[slices][0] = pos[0][0];
    pos[slices][1] = pos[0][1];
    pos[slices][2] = pos[0][2];
    nor[slices][0] = nor[0][0];
    nor[slices][1] = nor[0][1];
    nor[slices][2] = nor[0][2];
}

//With graphics!
static void drawTaperedTube(float length, float radius_base, float radius_tip,
                            float bend,
                            int slices, int segments)
{
    float radius = radius_base; // constant radius

    for (int seg = 0; seg < segments; ++seg)
    {
        float tBottom = (float)seg       / (float)segments;
        float tTop    = (float)(seg + 1) / (float)segments;

        float vB[128][3]; 
        float nB[128][3];
        float vT[128][3];
        float nT[128][3];

        makeRing(slices, tBottom, length, bend, radius, vB, nB);
        makeRing(slices, tTop,    length, bend, radius, vT, nT);

        glBegin(GL_TRIANGLES);
        for (int s = 0; s < slices; ++s)
        {
            float s0 = (float)s       / (float)slices;
            float s1 = (float)(s + 1) / (float)slices;

            // Quad split into 2 triangles
            // Tri 1: vB[s] -> vT[s] -> vT[s+1]
            glTexCoord2f(s0, tBottom); glNormal3fv(nB[s]);   glVertex3fv(vB[s]);
            glTexCoord2f(s0, tTop   ); glNormal3fv(nT[s]);   glVertex3fv(vT[s]);
            glTexCoord2f(s1, tTop   ); glNormal3fv(nT[s+1]); glVertex3fv(vT[s+1]);

            // Tri 2: vB[s] -> vT[s+1] -> vB[s+1]
            glTexCoord2f(s0, tBottom); glNormal3fv(nB[s]);   glVertex3fv(vB[s]);
            glTexCoord2f(s1, tTop   ); glNormal3fv(nT[s+1]); glVertex3fv(vT[s+1]);
            glTexCoord2f(s1, tBottom); glNormal3fv(nB[s+1]); glVertex3fv(vB[s+1]);
        }
        glEnd();
    }

    float centerXTop = bend * sinf((float)M_PI);
    float heightTop  = length;
    // Top sphere cap
    glPushMatrix();
    glTranslatef(centerXTop, heightTop, 0.f);
    drawSphere(radius_base*0.9f, slices, slices);
    glPopMatrix();

    // Bottom cap (-Y)
    float heightBottom = 0.f;
    glBegin(GL_TRIANGLES);
    for (int sl = 0; sl < slices; ++sl)
    {
        float a0 = (float)sl / (float)slices * 2.0f * (float)M_PI;
        float a1 = (float)(sl + 1) / (float)slices * 2.0f * (float)M_PI;

        float c0 = cosf(a0), s0 = sinf(a0);
        float c1 = cosf(a1), s1 = sinf(a1);

        float v_center[3] = { 0.f, heightBottom, 0.f };
        float v_rim1  [3] = { radius_base*c1, heightBottom, radius_base*s1 };
        float v_rim0  [3] = { radius_base*c0, heightBottom, radius_base*s0 };

        float tc_center[2] = { 0.5f, 0.5f };
        float tc_rim1  [2] = { 0.5f + 0.5f * c1, 0.5f + 0.5f * s1 };
        float tc_rim0  [2] = { 0.5f + 0.5f * c0, 0.5f + 0.5f * s0 };

        glTexCoord2fv(tc_center); glNormal3f(0.f,-1.f,0.f); glVertex3fv(v_center);
        glTexCoord2fv(tc_rim1);   glNormal3f(0.f,-1.f,0.f); glVertex3fv(v_rim1);
        glTexCoord2fv(tc_rim0);   glNormal3f(0.f,-1.f,0.f); glVertex3fv(v_rim0);
    }
    glEnd();
}


static void drawComplexBranch(float length, float rBase, float rTip,
                              float bend,
                              float forkT,
                              float forkAngleDeg,
                              float forkScale,
                              int slices, int segments)
{
    drawTaperedTube(length, rBase, rBase, bend, slices, segments);

    float yF = forkT * length;
    float xF = bend * sinf((float)M_PI * forkT);

    glPushMatrix();
    glTranslatef(xF, yF, 0.0f);
    glRotatef(forkAngleDeg, 0, 0, 1);
    glScalef(forkScale, forkScale, forkScale);
    drawTaperedTube(length * (1.0f - forkT),
                    rBase * 0.6f, rBase * 0.6f,
                    bend * 0.5f,
                    slices, segments / 2);
    glPopMatrix();
}


void ensure_cactus_texture(void)
{  
    if (cactusTex == 0u)
        cactusTex = loadTexture2D("assets/Textures/cactus.png");
}

void draw_cactus(const CactusBranchConfig *config)
{
    ensure_cactus_texture();

    if (!config)
        return;


    if (cactusTex != 0u)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cactusTex);
    }

    drawComplexBranch(config->length,
                      config->radiusBase,
                      config->radiusTip,
                      config->bend,
                      config->forkT,
                      config->forkAngleDeg,
                      config->forkScale,
                      config->slices,
                      16);

    if (cactusTex != 0u)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}
