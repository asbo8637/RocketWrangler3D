#ifndef CONTROLS_H
#define CONTROLS_H

/* Input handling initialization */
void initControls(void);

/* Input callbacks */
void handleKeyboard(unsigned char k, int x, int y);
void handleSpecialKeys(int k, int x, int y);

#endif /* CONTROLS_H */