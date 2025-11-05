#ifndef CONTROLS_H
#define CONTROLS_H

/* Input handling initialization */
void initControls(int windowId);

/* Input callbacks */
void handleKeyboard(unsigned char key, int x, int y);
void handleKeyboardUp(unsigned char key, int x, int y);
void handleSpecialKeys(int key, int x, int y);
void handleSpecialKeysUp(int key, int x, int y);

/* Process all active inputs - called each frame */
void processInputs(double deltaTime);

/* Key state query functions */
int isKeyPressed(unsigned char key);
int isSpecialKeyPressed(int key);

#endif /* CONTROLS_H */