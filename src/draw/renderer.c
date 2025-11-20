#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "renderer.h"
#include "camera.h"
#include "drawScene.h"
#include "../game/engine.h"

#ifndef RES
#define RES 1
#endif

// FPS tracking 
static double fpsUpdateTime = 0.0;
static int frameCount = 0;
static double currentFPS = 0.0;
static const double FPS_UPDATE_INTERVAL = 0.5; // Update FPS display every 0.5 seconds

// Window dimensions
static int winW = 900, winH = 600;
static float aspect = 900.0f/600.0f;

// Helper function for text rendering
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
    // brighter, less gloomy sky tone 
    glClearColor(0.35f, 0.72f, 0.88f, 1.0f);

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
    // Push far plane out to match long ground range
    gluPerspective(60.0f, aspect, 1.0, 100000.0);
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

    // Show bounce counter near the top-left corner.
    glWindowPos2i(5, winH - 30);
    Print("Score: %d  Bounces: %d / 1", engine_getScore(), engine_getBounceCount());
    
    // Display FPS at top-right corner
    glWindowPos2i(winW - 150, winH - 30);
    Print("FPS: %.1f", currentFPS);
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

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    setProjection();
    applyTPSView();

    // Draw world geometry centered near the camera target
    drawScene(tpsTargetZ, tpsTargetX);
    
    displayDebugInfo();
    
    glutSwapBuffers();
}
