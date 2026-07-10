#ifndef MENU_H
#define MENU_H

// Menu states
typedef enum
{
    MENU_MAIN,
    MENU_OPTIONS,
    MENU_NONE // In-game
} MenuState;

// Menu selection
extern int menuSelection;
extern MenuState menuState;

void drawMenu();
void handleMenuInput(unsigned char key);
void handleMenuSpecialInput(int key);

#endif // MENU_H
