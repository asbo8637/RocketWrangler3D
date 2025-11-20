#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include "prism.h"

void drawPrism(float width, float height, float depth) 
{
    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;
    
    // Calculate vertices for a triangular prism centered at origin 
    float vertices[6][3] = {
        // Front triangle
        {-halfWidth, 0.0f, halfDepth},        // Bottom left
        {halfWidth, 0.0f, halfDepth},         // Bottom right
        {0.0f, height, halfDepth},            // Top
        
        // Back triangle
        {-halfWidth, 0.0f, -halfDepth},       // Bottom left
        {halfWidth, 0.0f, -halfDepth},        // Bottom right
        {0.0f, height, -halfDepth}            // Top
    };

    // Draw front triangle
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glEnd();

   // Draw back triangle
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[3]);
    glEnd();

    // Draw bottom face
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[0]);
    glEnd();

    // Draw right face
    glBegin(GL_QUADS);
    {
        float dx = vertices[1][0] - vertices[2][0];
        float dy = vertices[1][1] - vertices[2][1];
        float dz = vertices[1][2] - vertices[2][2];
        float length = sqrt(dx*dx + dy*dy + dz*dz);
        float nx = dy/length;  /* Cross product with (0,0,1) */
        float ny = -dx/length;
        float nz = 0.0f;
        glNormal3f(nx, ny, nz);
    }
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[2]);
    glEnd();

    // Draw left face
    glBegin(GL_QUADS);
    {
        float dx = vertices[0][0] - vertices[2][0];
        float dy = vertices[0][1] - vertices[2][1];
        float dz = vertices[0][2] - vertices[2][2];
        float length = sqrt(dx*dx + dy*dy + dz*dz);
        float nx = -dy/length;  /* Cross product with (0,0,1) */
        float ny = dx/length;
        float nz = 0.0f;
        glNormal3f(nx, ny, nz);
    }
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[5]);
    glEnd();
}
