#ifndef CONTROLS_H
#define CONTROLS_H

/* Control state that can be read by engine */
typedef struct {
    float moveX;        // -1 to 1 (left/right)
    float moveY;        // -1 to 1 (down/up)
    float moveZ;        // -1 to 1 (back/forward)
    float jump;         // 1 if space key is pressed, 0 otherwise
    float cameraYaw;    // -1 to 1 (left/right rotation)
    float cameraPitch;  // -1 to 1 (down/up rotation)
    float cameraZoom;   // -1 to 1 (out/in)
    float restart;      // 1 if restart key is pressed, 0 otherwise
} ControlState;

/* Global control state accessible to engine */
extern ControlState controlState;

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