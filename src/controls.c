#include <GL/glut.h>
#include <stdlib.h>
#include "camera.h"
#include "controls.h"

/* Global state flags */
static int lightRotating = 0;   /* toggled by 'P' */

void handleKeyboard(unsigned char k, int x, int y)
{
    (void)x; (void)y;
    switch (k)
    {
    case 27: /* ESC */
        glutDestroyWindow(glutGetWindow());
        exit(0);
        break;

    /* TPS movement on XZ; aligns with yaw */
    case 'w': case 'W':
        moveForward();
        break;
    case 's': case 'S':
        moveBackward();
        break;
    case 'a': case 'A':
        moveLeft();
        break;
    case 'd': case 'D':
        moveRight();
        break;
    case 'q': case 'Q':
        moveUp();
        break;
    case 'e': case 'E':
        moveDown();
        break;

    /* zoom */
    case '+': case '=':
        zoomIn();
        break;
    case '-': case '_':
        zoomOut();
        break;

    case 'p': case 'P':
        lightRotating = !lightRotating;
        break;

    default: return;
    }
    glutPostRedisplay();
}

void handleSpecialKeys(int k, int x, int y)
{
    (void)x; (void)y;
    switch (k)
    {
    case GLUT_KEY_LEFT:
        turnLeft();
        break;
    case GLUT_KEY_RIGHT:
        turnRight();
        break;
    case GLUT_KEY_UP:
        lookUp();
        break;
    case GLUT_KEY_DOWN:
        lookDown();
        break;
    default: return;
    }
    glutPostRedisplay();
}

void initControls(void)
{
    /* Set up GLUT callbacks */
    glutKeyboardFunc(handleKeyboard);
    glutSpecialFunc(handleSpecialKeys);
}