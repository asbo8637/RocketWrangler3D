#ifndef TEXTURE_H
#define TEXTURE_H

// Loads an image file into an OpenGL 2D texture. Returns 0 on failure.
unsigned int loadTexture2D(const char *filePath);

// Loads a texture and sets white/near-white pixels transparent to kill padded backgrounds.
unsigned int loadTexture2DWhiteToAlpha(const char *filePath);

#endif // TEXTURE_H
