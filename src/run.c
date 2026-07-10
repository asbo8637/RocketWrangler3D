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
#include "game/menu.h"

// Globals
static GLint win = 0;
static double lastTimeSec = 0.0;    // Last frame timestamp (seconds)
static double lastFrameDelta = 0.0; // Duration of the previous frame (seconds)

static double now_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

// Forward declarations
void enterFullScreen();
void exitFullScreen();

static void display(void)
{
    if (menuState != MENU_NONE)
    {
        // Draw menu overlay
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawMenu();
        glutSwapBuffers();
    }
    else
    {
        render();
    }
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

    if (menuState == MENU_NONE)
    {
        // In-game
        processInputs(deltaTime);
        if (controlState.pause == 0.0f)
        {
            updateEngine(deltaTime);
        }
        else
        {
            // If paused, update camera with manual controls.
            camera_update_manual(deltaTime);
        }
        // Store delta for the renderer
        lastFrameDelta = deltaTime;
    }
    // Always trigger a new frame
    glutPostRedisplay();
}

// Menu input wrappers
void menuKeyboard(unsigned char key, int x, int y)
{
    if (menuState != MENU_NONE)
    {
        handleMenuInput(key);
        if (key == 13 || key == '\r' || key == 32)
        { // Enter or Space pressed
            if (menuState == MENU_MAIN)
            {
                static int isFullScreen = 0;
                switch (menuSelection)
                {
                case 0: // Start Game
                    menuState = MENU_NONE;
                    controlState.pause = 0.0f;
                    break;
                case 1: // Full Screen (toggle)
                    if (!isFullScreen)
                    {
                        enterFullScreen();
                        isFullScreen = 1;
                    }
                    else
                    {
                        exitFullScreen();
                        isFullScreen = 0;
                    }
                    break;
                case 2: // Quit
                    exit(0);
                    break;
                }
            }
        }
    }
    else
    {
        // If Escape is pressed in-game, pause and go to menu
        if (key == 27)
        {
            controlState.pause = 1.0f;
            menuState = MENU_MAIN;
            menuSelection = 0;
            return;
        }
        handleKeyboard(key, x, y);
    }
}

void menuSpecial(int key, int x, int y)
{
    if (menuState != MENU_NONE)
    {
        handleMenuSpecialInput(key);
    }
    else
    {
        handleSpecialKeys(key, x, y);
    }
}

void menuKeyboardUp(unsigned char key, int x, int y)
{
    if (menuState == MENU_NONE)
    {
        handleKeyboardUp(key, x, y);
    }
}

void menuSpecialUp(int key, int x, int y)
{
    if (menuState == MENU_NONE)
    {
        handleSpecialKeysUp(key, x, y);
    }
}

// Fullscreen helpers
void enterFullScreen()
{
    glutFullScreen();
}
void exitFullScreen()
{
    glutReshapeWindow(1800, 1200);
    glutPositionWindow(50, 50);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(1800, 1200);
    win = glutCreateWindow("Assaf Boneh - RocketWrangler 3D!");

    // Initialize all subsystems
    initRenderer();
    initControls(win);
    initEngine();
    initScene(); // Initialize scene objects (robot, etc.)

    // Initialize timing
    lastTimeSec = now_seconds();
    lastFrameDelta = 0.0;

    // Start in menu
    menuState = MENU_MAIN;
    menuSelection = 0;

    // Set up GLUT callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(handleReshape);
    glutKeyboardFunc(menuKeyboard);
    glutKeyboardUpFunc(menuKeyboardUp);
    glutSpecialFunc(menuSpecial);
    glutSpecialUpFunc(menuSpecialUp);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
