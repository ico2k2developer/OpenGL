#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <glm/glm.hpp>

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
void shader_set_b(shaderp s, const char* name, GLboolean value);
void shader_set_i(shaderp s, const char* name, GLint value);
void shader_set_f(shaderp s, const char* name, GLfloat value);
void shader_get_f(shaderp s, const char* name, GLfloat* dest);
void shader_set_vec2(shaderp s, const char* name, GLfloat x, GLfloat y);
void shader_set_vec3(shaderp s, const char* name, GLfloat x, GLfloat y, GLfloat z);
void shader_set_vec4(shaderp s, const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void shader_set_vec2(shaderp s, const char* name, glm::vec2 v);
void shader_set_vec3(shaderp s, const char* name, glm::vec3 v);
void shader_set_vec4(shaderp s, const char* name, glm::vec4 v);
void shader_set_mat2(shaderp s, const char* name, glm::mat2 v);
void shader_set_mat3(shaderp s, const char* name, glm::mat3 v);
void shader_set_mat4(shaderp s, const char* name, glm::mat4 v);
#endif

