#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>

typedef struct
{
	unsigned int ID;
}shader;
typedef shader* shaderp;

shaderp shader_new(const char* vertexPath, const char* fragmentPath,const char* geometryPath);
shaderp shader_new(const char* vertexPath, const char* fragmentPath);
void shader_chk_err(GLuint shader, const char* type);
void shader_use(shaderp s);
void shader_release(shaderp s);
void shader_set_b(shaderp s, const char* name, bool value);
void shader_set_i(shaderp s, const char* name, int value);
void shader_set_f(shaderp s, const char* name, float value);
void shader_set_vec2(shaderp s, const char* name, float x, float y);
void shader_set_vec3(shaderp s, const char* name, float x, float y, float z);
void shader_set_vec4(shaderp s, const char* name, float x, float y, float z, float w);
#endif

