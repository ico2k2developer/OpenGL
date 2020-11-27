#pragma once

unsigned char* loadImage(const char* filename, int* width, int* height, int* channels);
bool loadTexture(const char* filename);
void setupArrays(const unsigned int VBO, const unsigned int VAO, const unsigned int EBO, const float vertices[], const unsigned int indices[], const unsigned int vertexAttribs[]);
void setupTextures(const unsigned int textures[], const unsigned int textureSettings[], const char* textureFiles[]);