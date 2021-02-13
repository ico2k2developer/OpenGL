#pragma once
#include "ico2k2.h"

unsigned char* loadImage(const GLchar *filename, int *width, int *height, int *channels);
bool loadTexture(const GLchar *filename,bool alpha);
bool loadTexture(const GLchar *filename);
GLuint setupArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO,const arrp vertices,const arrp indexes,const arrp vertexAttribs);
void deleteArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO);
void setupTextures(const arrp textures, const GLuint *textureSettings, const GLchar **textureFiles);
void activateTextures(const arrp textures);
void deleteTextures(const arrp textures);