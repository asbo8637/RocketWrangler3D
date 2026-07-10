#include <GL/glut.h>
#include "menu.h"
#include "controls.h"
#include "engine.h"

// Menu state globals
int menuSelection = 0;
MenuState menuState = MENU_MAIN;

static const char *mainMenuItems[] = {
    "Start Game",
    "Full Screen",
    "Quit"};
static const int mainMenuCount = sizeof(mainMenuItems) / sizeof(mainMenuItems[0]);

void drawMenu()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Draw background
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(800, 0);
    glVertex2i(800, 600);
    glVertex2i(0, 600);
    glEnd();

    // Draw menu items
    for (int i = 0; i < mainMenuCount; ++i)
    {
        if (i == menuSelection)
        {
            glColor3f(1.0f, 1.0f, 0.3f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        glRasterPos2i(350, 400 - i * 40);
        const char *c = mainMenuItems[i];
        while (*c)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
            ++c;
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void handleMenuInput(unsigned char key)
{
    if (menuState != MENU_MAIN)
        return;
    // WASD support for menu navigation
    if (key == 'w' || key == 'W')
    {
        menuSelection = (menuSelection - 1 + mainMenuCount) % mainMenuCount;
    }
    else if (key == 's' || key == 'S')
    {
        menuSelection = (menuSelection + 1) % mainMenuCount;
    }
    else if (key == 13 || key == '\r' || key == 32)
    {
        // Enter or Space key: selection handled in main
    }
}

void handleMenuSpecialInput(int key)
{
    if (menuState != MENU_MAIN)
        return;
    if (key == GLUT_KEY_UP)
    {
        menuSelection = (menuSelection - 1 + mainMenuCount) % mainMenuCount;
    }
    else if (key == GLUT_KEY_DOWN)
    {
        menuSelection = (menuSelection + 1) % mainMenuCount;
    }
}

// Helper for menu item bounding box
static int menuItemAt(int mx, int my)
{
    // Menu is drawn at (350, 400 - i*40), font height ~24px, width ~200px
    for (int i = 0; i < mainMenuCount; ++i)
    {
        int x0 = 350, y0 = 400 - i * 40;
        int x1 = x0 + 200, y1 = y0 + 24;
        if (mx >= x0 && mx <= x1 && my >= y0 && my <= y1)
            return i;
    }
    return -1;
}

void handleMenuMouse(int button, int state, int x, int y)
{
    if (menuState != MENU_MAIN || button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;
    // Convert y to OpenGL coordinates (origin bottom-left)
    int oglY = 600 - y;
    int idx = menuItemAt(x, oglY);
    if (idx >= 0)
    {
        menuSelection = idx;
        // Simulate Enter key for selection
        // This logic is duplicated from run.c, keep in sync
        switch (menuSelection)
        {
        case 0: // Start Game
            menuState = MENU_NONE;
            extern ControlState controlState;
            controlState.pause = 0.0f;
            break;
        case 1: // Full Screen
            extern void enterFullScreen();
            enterFullScreen();
            break;
        case 2: // Windowed
            extern void exitFullScreen();
            exitFullScreen();
            break;
        case 3: // Quit
            exit(0);
            break;
        }
    }
}