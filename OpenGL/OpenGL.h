#pragma once

unsigned char* loadImage(const GLchar *filename, int *width, int *height, int *channels);
bool loadTexture(const GLchar *filename,bool alpha);
bool loadTexture(const GLchar *filename);
GLuint setupArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO, const GLfloat* vertices, const GLuint* indices, const GLuint* vertexAttribs, const GLuint columns);
void setupTextures(GLuint *textures, const GLuint *textureSettings, const GLchar **textureFiles, const GLuint count);
void activateTextures(GLuint* textures, const GLuint count);