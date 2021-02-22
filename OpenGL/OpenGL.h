#include "config.h"
#include "shader.h"
#include "ico2k2.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct
{
	GLuint textureId;
	unsigned char sizeX,sizeY;
	char bearingX, bearingY;
	unsigned short advance;
}chr_ttf;

unsigned char* loadImage(const GLchar *filename, int *width, int *height, int *channels);
bool loadTexture(const GLchar *filename,bool alpha);
bool loadTexture(const GLchar *filename);
GLuint setupArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO,arr vertices,arr indexes,arr vertexAttribs);
void deleteArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO);
void setupTextures(arr textures, const GLuint *textureSettings, const GLchar **textureFiles);
void activateTextures(arr textures);
void deleteTextures(arr textures);
void renderText(shaderp s, chr_ttf* font, const char* text, GLuint* VAO, GLuint* VBO, GLfloat x, GLfloat y, glm::vec3 colour);