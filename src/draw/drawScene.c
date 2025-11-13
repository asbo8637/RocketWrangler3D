#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>

#include "drawScene.h"
#include "init.h"
#include "../../assets/Shapes/master.h"
#include "../../assets/Models/joint_class.h"
#include "../game/rockets.h"

/* Lighting */
static float lightPos[4] = {30.f, 25.f, 10.f, 1.f}; /* positional */
static float lightAmb[4] = {0.1f, 0.1f, 0.1f, 1.f};
static float lightDif[4] = {1.0f, 1.0f, 0.95f, 1.f};
static float lightSpe[4] = {1.00f, 1.00f, 1.00f, 1.f};

static void drawLightGlyph(const float pos[4], float radius)
{
    float x = pos[0], y = pos[1], z = pos[2];
    float savedEmi[4];
    glGetMaterialfv(GL_FRONT, GL_EMISSION, savedEmi);
    float glow[4] = {0.9f, 0.8f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glow);

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, radius, radius);
    glBegin(GL_TRIANGLES);
    glNormal3f(1, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, -1);
    glNormal3f(-1, 0, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(0, -1, 0);
    glVertex3f(0, 0, -1);
    glNormal3f(0, 1, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(1, 0, 0);
    glVertex3f(0, 0, -1);
    glNormal3f(0, -1, 0);
    glVertex3f(0, 0, 1);
    glVertex3f(-1, 0, 0);
    glVertex3f(0, 0, -1);
    glEnd();
    glPopMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, savedEmi);
}

/* Function to set up basic lighting */
void setupLighting(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpe);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

#include "camera.h"

/* Main scene drawing function */
void drawScene(double deltaTime)
{
    glEnable(GL_DEPTH_TEST);

    /* Set up lighting */
    setupLighting();

    /* Draw light glyph */
    drawLightGlyph(lightPos, 0.6f);

    /* Infinite-looking ground plane (large repeated textured quad) */
    glPushMatrix();
    const float yGround = -0.1f;
    const float halfSize = 24000.0f;     /* world half-size of ground quad */
    const float tileWorld = 4.0f;      /* world units per texture repeat */

    glNormal3f(0.0f, 1.0f, 0.0f);

    if (groundTexture != 0u)
    {
        /* Use texture if available */
        float white[4] = {1.f, 1.f, 1.f, 1.f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /* Explicit 0..repeat mapping like hw6 */
        const float repeat = (halfSize * 2.0f) / tileWorld;
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,   0.0f   ); glVertex3f(-halfSize, yGround, -halfSize);
        glTexCoord2f(repeat, 0.0f   ); glVertex3f( halfSize, yGround, -halfSize);
        glTexCoord2f(repeat, repeat ); glVertex3f( halfSize, yGround,  halfSize);
        glTexCoord2f(0.0f,   repeat ); glVertex3f(-halfSize, yGround,  halfSize);
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        /* Fallback flat color if texture not loaded */
        float groundColor[4] = {0.3f, 0.5f, 0.3f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, groundColor);
        glBegin(GL_QUADS);
        glVertex3f(-100.0f, yGround, -100.0f);
        glVertex3f(-100.0f, yGround,  100.0f);
        glVertex3f( 100.0f, yGround,  100.0f);
        glVertex3f( 100.0f, yGround, -100.0f);
        glEnd();
    }
    glPopMatrix();

    /* Draw the robot and all live rockets */
    robot_draw(robot);
    rockets_draw();
}