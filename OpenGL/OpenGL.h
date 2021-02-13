#pragma once
#include "ico2k2.h"

unsigned char* loadImage(const GLchar *filename, int *width, int *height, int *channels);
bool loadTexture(const GLchar *filename,bool alpha);
bool loadTexture(const GLchar *filename);
GLuint setupArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO,const arrayp vertices,const arrayp indexes,const arrayp vertexAttribs);
void deleteArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO);
void setupTextures(const arrayp textures, const GLuint *textureSettings, const GLchar **textureFiles);
void activateTextures(const arrayp textures);