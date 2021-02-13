#pragma once
#include "ico2k2.h"

unsigned char* loadImage(const GLchar *filename, int *width, int *height, int *channels);
bool loadTexture(const GLchar *filename,bool alpha);
bool loadTexture(const GLchar *filename);
GLuint setupArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO,arr vertices,arr indexes,arr vertexAttribs);
void deleteArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO);
void setupTextures(arr textures, const GLuint *textureSettings, const GLchar **textureFiles);
void activateTextures(arr textures);
void deleteTextures(arr textures);