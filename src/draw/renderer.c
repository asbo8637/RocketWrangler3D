#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "renderer.h"
#include "camera.h"
#include "drawScene.h"

#ifndef RES
#define RES 1
#endif

/* FPS tracking */
static double fpsUpdateTime = 0.0;
static int frameCount = 0;
static double currentFPS = 0.0;
static const double FPS_UPDATE_INTERVAL = 0.5; /* Update FPS display every 0.5 seconds */

/* Window dimensions */
static int winW = 1800, winH = 1200;
static float aspect = 900.0f/600.0f;

/* Helper function for text rendering */
#define LEN 8192
static void Print(const char *format, ...)
{
    char buf[LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, LEN, format, args);
    va_end(args);
    for (const char *ch = buf; *ch; ch++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch);
}

void initRenderer(void)
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);

#ifdef USEGLEW
    if (glewInit() != GLEW_OK) { fprintf(stderr, "Error initializing GLEW\n"); exit(1); }
#endif
}

void handleReshape(int width, int height)
{
    if (height == 0) height = 1;
    winW = width; winH = height;
    aspect = (GLfloat)width / (GLfloat)height;
    glViewport(0, 0, (GLint)RES * width, (GLint)RES * height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}

void setProjection(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, aspect, 0.5, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void setupHUD(void)
{
    glDisable(GL_LIGHTING);
    glColor3f(1,1,1);
    glWindowPos2i(5,5);
}

void displayDebugInfo(void)
{
    setupHUD();
    
    /* Display FPS at top-right corner */
    glWindowPos2i(winW - 150, winH - 30);
    Print("FPS: %.1f", currentFPS);
    
    /* Display other debug info at bottom */
    glWindowPos2i(5, 5);
    Print("Target(%.1f, %.1f, %.1f)  Yaw %.2f Pitch %.2f Dist %.1f",
          tpsTargetX, tpsTargetY, tpsTargetZ, tpsYaw, tpsPitch, tpsDist);
}

void updateFPS(double deltaTime)
{
    frameCount++;
    fpsUpdateTime += deltaTime;
    
    if (fpsUpdateTime >= FPS_UPDATE_INTERVAL)
    {
        currentFPS = frameCount / fpsUpdateTime;
        frameCount = 0;
        fpsUpdateTime = 0.0;
    }
}

void render(double deltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    setProjection();
    applyTPSView();
    drawScene(deltaTime);
    
    displayDebugInfo();
    
    glutSwapBuffers();
}