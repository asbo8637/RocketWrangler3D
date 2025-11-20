#include <stdlib.h>

// POSIX timing headers for Linux/WSL
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
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
static double lastFrameDelta = 0.0;               // Duration of the previous frame (seconds)


static double now_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

static void display(void)
{
    render();
}

static void idle(void)
{
    double currentTime = now_seconds();
    double deltaTime = currentTime - lastTimeSec;

    // Cap delta time to avoid large time steps
    if (deltaTime > 0.1)
        deltaTime = 0.1;
    if (deltaTime < 0.0)
        deltaTime = 0.0;


    // Update last time
    lastTimeSec = currentTime;


    // Update FPS counter
    updateFPS(deltaTime);

    // Process inputs and update game state with accumulated time
    processInputs(deltaTime);
    updateEngine(deltaTime);

    // Store delta for the renderer
    lastFrameDelta = deltaTime;

    // Trigger a new frame
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(1800, 1200);
    win = glutCreateWindow("RocketWrangler - 3D");

    // Initialize all subsystems
    initRenderer();
    initControls(win);
    initEngine();
    initScene(); // Initialize scene objects (robot, etc.)

    // Initialize timing
    lastTimeSec = now_seconds();
    lastFrameDelta = 0.0;

    // Set up GLUT callbacks
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
