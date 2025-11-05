#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include "../draw/camera.h"
#include "controls.h"

#define MAX_KEYS 256
#define MAX_SPECIAL_KEYS 256

/* Key state tracking */
static unsigned char normalKeys[MAX_KEYS] = {0};
static unsigned char specialKeys[MAX_SPECIAL_KEYS] = {0};

/* Other global state */
static int windowId = 0;
static int lightRotating = 0;   /* toggled by 'P' */

void handleKeyboard(unsigned char key, int x, int y)
{
    (void)x; (void)y;
    normalKeys[key] = 1;
    
    /* Handle immediate actions */
    switch (key)
    {
    case 27: /* ESC */
        glutDestroyWindow(windowId);
        exit(0);
        break;

    case 'p': case 'P':
        lightRotating = !lightRotating;
        break;
    }
}

void handleKeyboardUp(unsigned char key, int x, int y)
{
    (void)x; (void)y;
    normalKeys[key] = 0;
}

void handleSpecialKeys(int key, int x, int y)
{
    (void)x; (void)y;
    if (key < MAX_SPECIAL_KEYS)
        specialKeys[key] = 1;
}

void handleSpecialKeysUp(int key, int x, int y)
{
    (void)x; (void)y;
    if (key < MAX_SPECIAL_KEYS)
        specialKeys[key] = 0;
}

void processInputs(double deltaTime)
{
    /* Process all currently pressed keys */
    if ((normalKeys['w'] || normalKeys['W']) && !(normalKeys['s'] || normalKeys['S'])) moveForward(deltaTime);
    else if ((normalKeys['s'] || normalKeys['S']) && !(normalKeys['w'] || normalKeys['W'])) moveBackward(deltaTime);

    if ((normalKeys['a'] || normalKeys['A']) && !(normalKeys['d'] || normalKeys['D'])) moveLeft(deltaTime);
    else if ((normalKeys['d'] || normalKeys['D']) && !(normalKeys['a'] || normalKeys['A']) ) moveRight(deltaTime);

    if ((normalKeys['q'] || normalKeys['Q']) && !(normalKeys['e'] || normalKeys['E'])) moveUp(deltaTime);
    else if ((normalKeys['e'] || normalKeys['E']) && !(normalKeys['q'] || normalKeys['Q'])) moveDown(deltaTime);
    
    /* Zoom */
    if (normalKeys['+'] || normalKeys['=']) zoomIn(deltaTime);
    else if (normalKeys['-'] || normalKeys['_']) zoomOut(deltaTime);
    
    /* Camera rotation */
    if (specialKeys[GLUT_KEY_LEFT])  turnLeft(deltaTime);
    if (specialKeys[GLUT_KEY_RIGHT]) turnRight(deltaTime);
    if (specialKeys[GLUT_KEY_UP])    lookUp(deltaTime);
    if (specialKeys[GLUT_KEY_DOWN])  lookDown(deltaTime);
}

int isKeyPressed(unsigned char key)
{
    return normalKeys[key];
}

int isSpecialKeyPressed(int key)
{
    return (key < MAX_SPECIAL_KEYS) ? specialKeys[key] : 0;
}

void initControls(int winId)
{
    windowId = winId;
    
    /* Reset all key states */
    memset(normalKeys, 0, sizeof(normalKeys));
    memset(specialKeys, 0, sizeof(specialKeys));
    
    /* Set up GLUT callbacks */
    glutKeyboardFunc(handleKeyboard);
    glutKeyboardUpFunc(handleKeyboardUp);
    glutSpecialFunc(handleSpecialKeys);
    glutSpecialUpFunc(handleSpecialKeysUp);
}