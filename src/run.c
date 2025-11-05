#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "draw/camera.h"
#include "game/controls.h"
#include "draw/renderer.h"
#include "game/engine.h"

/* Globals */
static GLint win = 0;
static struct timespec lastTime;
static double accumulator = 0.0;       /* Time accumulator for frame limiting */
static const double targetFrameTime = 1.0 / 60.0;  /* Target time per frame in seconds */

/* Get current time in seconds (high precision) */
static double timeDifference(struct timespec *start, struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) + 
           (end->tv_nsec - start->tv_nsec) * 1e-9;
}

static void display(void)
{
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    
    double deltaTime = timeDifference(&lastTime, &currentTime);
    
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
    lastTime = currentTime;
    
    /* Reset accumulator after frame is complete */
    accumulator = 0.0;
}

static void idle(void)
{
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    
    double frameTime = timeDifference(&lastTime, &currentTime);
    
    /* Accumulate time */
    accumulator += frameTime;
    
    /* If we haven't reached our target frame time yet, sleep */
    if (accumulator < targetFrameTime)
    {
        /* Calculate how long to sleep */
        double timeToSleep = (targetFrameTime - accumulator) * 1000000; /* Convert to microseconds */
        if (timeToSleep > 100) /* Only sleep if we have meaningful time to wait */
        {
            usleep((unsigned int)timeToSleep);
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
    clock_gettime(CLOCK_MONOTONIC, &lastTime);
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
