#include <stdlib.h>

// Platform-specific timing headers
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "draw/camera.h"
#include "game/controls.h"
#include "draw/renderer.h"
#include "game/engine.h"

// Globals
static GLint win = 0;
static double lastTimeSec = 0.0;       // Last frame timestamp (seconds)
static double accumulator = 0.0;       // Time accumulator for frame limiting 
static const double targetFrameTime = 1.0 / 60.0;  // Target time per frame in seconds 

// High-resolution clock in seconds
static double now_seconds(void)
{
#ifdef _WIN32
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER counter;
    if (freq.QuadPart == 0) {
        QueryPerformanceFrequency(&freq);
    }
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
#endif
}

// Sleep helper (microseconds)
static void sleep_us(unsigned int micros)
{
#ifdef _WIN32
    // Sleep() takes milliseconds; round up to avoid 0ms busy waits
    DWORD ms = (DWORD)((micros + 999) / 1000);
    if (ms > 0) Sleep(ms);
#else
    usleep(micros);
#endif
}

static void display(void)
{
    double currentTime = now_seconds();
    double deltaTime = currentTime - lastTimeSec;
    
    /* Cap delta time to avoid large time steps */
    if (deltaTime > 0.1) deltaTime = 0.1;
    
    /* Process inputs and update game state */
    processInputs(deltaTime);
    updateEngine(deltaTime);
    
    /* Update FPS counter */
    updateFPS(deltaTime);
    
    /* Render frame */
    render();
    
    /* Store time for next frame */
    lastTimeSec = currentTime;
    
    /* Reset accumulator after frame is complete */
    accumulator = 0.0;
}

static void idle(void)
{
    double currentTime = now_seconds();
    double frameTime = currentTime - lastTimeSec;
    
    /* Accumulate time */
    accumulator += frameTime;
    
    /* If we haven't reached our target frame time yet, sleep */
    if (accumulator < targetFrameTime)
    {
        /* Calculate how long to sleep */
        double timeToSleep = (targetFrameTime - accumulator) * 1000000; /* Convert to microseconds */
        if (timeToSleep > 100) /* Only sleep if we have meaningful time to wait */
        {
            sleep_us((unsigned int)timeToSleep);
        }
        return;
    }
    
    /* Always process inputs to prevent input lag */
    processInputs(frameTime);
    
    /* Trigger a new frame */
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(1800, 1200);
    win = glutCreateWindow("RocketWrangler - 3D");

    /* Initialize all subsystems */
    initRenderer();
    initControls(win);
    initEngine();
    
    /* Initialize timing */
    lastTimeSec = now_seconds();
    accumulator = 0.0;

    /* Set up GLUT callbacks */
    glutDisplayFunc(display);
    glutReshapeFunc(handleReshape);
    glutKeyboardFunc(handleKeyboard);
    glutKeyboardUpFunc(handleKeyboardUp);
    glutSpecialFunc(handleSpecialKeys);
    glutSpecialUpFunc(handleSpecialKeysUp);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
