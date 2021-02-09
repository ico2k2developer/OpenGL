#pragma once
#include "ico2k2.h"

unsigned char* loadImage(const GLchar *filename, int *width, int *height, int *channels);
bool loadTexture(const GLchar *filename,bool alpha);
bool loadTexture(const GLchar *filename);
GLuint setupArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO,const arrayp vertices,const arrayp indexes,const arrayp vertexAttribs);
void setupTextures(GLuint *textures, const GLuint *textureSettings, const GLchar **textureFiles, const GLuint count);
void activateTextures(GLuint* textures, const GLuint count);