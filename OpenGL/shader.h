#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "config.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include "ico2k2.h"

#define	OPENGL_DEF_MAJOR	3
#define	OPENGL_DEF_MINOR	3
#define	OPENGL_DEF_PROFILE	GLFW_OPENGL_CORE_PROFILE

#if defined(OPENGL_MAJOR) && defined(OPENGL_MINOR) && defined(OPENGL_PROFILE)
#define	OPENGL_VERSION_TEXT	"#version " STR(OPENGL_MAJOR) STR(OPENGL_MINOR) "0 " OPENGL_VERSION_PROF "\n"
#else
#define	OPENGL_VERSION_TEXT	"#version " STR(OPENGL_DEF_MAJOR) STR(OPENGL_DEF_MINOR) "0 " OPENGL_PROFILE_TXT(OPENGL_DEF_PROFILE) "\n"
#pragma message("OPENGL_MAJOR and/or OPENGL_MINOR and/or OPENGL_PROFILE are not defined, using default " OPENGL_VERSION(OPENGL_DEF_MAJOR,OPENGL_DEF_MINOR,OPENGL_DEF_PROFILE)
#endif

typedef struct
{
	GLuint ID;
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

