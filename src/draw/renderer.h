#ifndef RENDERER_H
#define RENDERER_H

/* Initialize OpenGL rendering settings */
void initRenderer(void);

/* Handle window reshaping */
void handleReshape(int width, int height);

/* Apply projection matrix for current mode */
void setProjection(void);

/* Set up the viewport and projection for HUD elements */
void setupHUD(void);

/* Display debug info (camera position, etc.) */
void displayDebugInfo(void);

/* Update FPS counter with current frame time */
void updateFPS(double deltaTime);

/* Main render function - clears buffers and renders entire scene */
void render(double deltaTime);

#endif /* RENDERER_H */