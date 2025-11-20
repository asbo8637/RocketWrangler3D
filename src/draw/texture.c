#ifdef USEGLEW
#include <GL/glew.h>
#endif
#ifdef _WIN32
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>

// stb_image single-header decoder
// To keep this repo self-contained, we vendor the header locally.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static int isPowerOfTwo(int x) { return (x & (x - 1)) == 0; }
static int nextPowerOfTwo(int x) {
    int p = 1; while (p < x) p <<= 1; return p;
}

unsigned int loadTexture2D(const char *filePath)
{
    int w = 0, h = 0, comp = 0;
    unsigned char *pixels = stbi_load(filePath, &w, &h, &comp, 4); // force RGBA
    if (!pixels)
    {
        fprintf(stderr, "Failed to load texture: %s\n", filePath);
        return 0u;
    }

    // Some old GL paths prefer power-of-two textures for mipmaps; upscale if needed
    int tw = w, th = h;
    if (!isPowerOfTwo(w) || !isPowerOfTwo(h))
    {
        tw = nextPowerOfTwo(w);
        th = nextPowerOfTwo(h);
        unsigned char *resized = (unsigned char *)malloc((size_t)tw * (size_t)th * 4);
        if (!resized)
        {
            stbi_image_free(pixels);
            return 0u;
        }
        // Simple nearest rescale (good enough for a tiling ground texture)
        for (int y = 0; y < th; ++y)
        {
            int sy = (int)((y / (float)th) * h);
            for (int x = 0; x < tw; ++x)
            {
                int sx = (int)((x / (float)tw) * w);
                for (int c = 0; c < 4; ++c)
                    resized[(y * tw + x) * 4 + c] = pixels[(sy * w + sx) * 4 + c];
            }
        }
        stbi_image_free(pixels);
        pixels = resized;
        w = tw; h = th;
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);
    return tex;
}
