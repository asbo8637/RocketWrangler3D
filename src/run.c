#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <sys/time.h>
#include "camera.h"
#include "controls.h"
#include "renderer.h"
#include "engine.h"

/* Globals */
static GLint win = 0;
static struct timeval lastTime;    /* Time of last update */
static const double targetFPS = 60.0;
static const double targetFrameTime = 1.0 / 60.0;  /* Target time per frame in seconds */

/* Get current time in seconds (high precision) */
static double getCurrentTime(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec + now.tv_usec / 1000000.0;
}

static void display(void)
{
    /* Calculate delta time */
    struct timeval now;
    gettimeofday(&now, NULL);
    
    double currentTime = getCurrentTime();
    double deltaTime = currentTime - 
                      (lastTime.tv_sec + lastTime.tv_usec / 1000000.0);
    
    /* Cap delta time to avoid large time steps */
    if (deltaTime > 0.1) deltaTime = 0.1;
    
    /* Update game state */
    updateEngine(deltaTime);
    
    /* Render frame */
    render();
    
    /* Store time for next frame */
    lastTime = now;
}

static void idle(void)
{
    /* Simple frame limiting */
    double currentTime = getCurrentTime();
    double deltaTime = currentTime - 
                      (lastTime.tv_sec + lastTime.tv_usec / 1000000.0);
    
    if (deltaTime >= targetFrameTime)
    {
        glutPostRedisplay();
    }
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
    gettimeofday(&lastTime, NULL);

    /* Set up GLUT callbacks */
    glutDisplayFunc(display);
    glutReshapeFunc(handleReshape);
    glutKeyboardFunc(handleKeyboard);
    glutSpecialFunc(handleSpecialKeys);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
