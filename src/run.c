#include <stdlib.h>

// POSIX timing headers for Linux/WSL
#include <time.h>
#include <unistd.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "draw/camera.h"
#include "draw/init.h"
#include "game/controls.h"
#include "draw/renderer.h"
#include "game/engine.h"

// Globals
static GLint win = 0;
static double lastTimeSec = 0.0;                  // Last frame timestamp (seconds)
static double accumulator = 0.0;                  // Time accumulator for frame limiting
static const double targetFrameTime = 1.0 / 60.0; // Target time per frame in seconds

// High-resolution clock in seconds (POSIX)
static double now_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

// Sleep helper (microseconds, POSIX)
static void sleep_us(unsigned int micros)
{
    usleep(micros);
}

static void display(void)
{
    double currentTime = now_seconds();
    double deltaTime = currentTime - lastTimeSec;

    /* Cap delta time to avoid large time steps */
    if (deltaTime > 0.1)
        deltaTime = 0.1;

    /* Process inputs and update game state */
    processInputs(deltaTime);
    updateEngine(deltaTime);

    /* Update FPS counter */
    updateFPS(deltaTime);

    /* Render frame */
    render(deltaTime);

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
        if (timeToSleep > 100)                                          /* Only sleep if we have meaningful time to wait */
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
    initScene();  // Initialize scene objects (robot, etc.)

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
