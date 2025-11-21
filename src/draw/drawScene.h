#ifndef DRAWSCENE_H
#define DRAWSCENE_H

// Function to set up basic lighting
void setupLighting(void);

// Main scene drawing function
void drawScene(float camZ);

void setSeed(void);


float get_groundHeight(float x, float z);

#endif // DRAWSCENE_H
