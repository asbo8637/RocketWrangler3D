#ifndef HAT_H
#define HAT_H

// Draw a stylized cowboy hat.
// brimWidth/brimDepth: footprint of the flat rectangle section of the brim
// brimSideRadius: outward radius of each upturned semicircle brim wing (width independent of the rectangle)
// brimTiltDeg: upward tilt (deg) applied to the two semicircle sides
// crownRadius/crownHeight: base radius and height of the crown before scaling
// crownScaleX/crownScaleZ: non-uniform scale to make the crown oval
// segments: tessellation for the semicircles/crown
void drawCowboyHat(float brimWidth, float brimDepth, float brimSideRadius, float brimTiltDeg,
                   float crownRadius, float crownHeight,
                   int segments);

#endif // HAT_H
