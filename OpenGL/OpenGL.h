#pragma once

unsigned char* loadImage(const GLchar* filename, int* width, int* height, int* channels);
bool loadTexture(const GLchar* filename);
void setupArrays(const GLuint VBO, const GLuint VAO, const GLuint EBO, const GLfloat vertices[], const GLuint indices[], const GLuint vertexAttribs[]);
void setupTextures(const GLuint textures[], const GLuint textureSettings[], const GLchar* textureFiles[]);