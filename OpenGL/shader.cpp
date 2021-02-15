#include <glad/glad.h>
#include "shader.h"
#include <stdlib.h>
#include <glm/glm.hpp>
#include "ico2k2.h"

//#define DEBUG

shaderp shader_new(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	shaderp result = (shaderp)malloc(sizeof(shader));
	char* buffer;
	FILE* vShaderFile, * fShaderFile;
	GLuint vertex, fragment, geometry = NULL;
	if (result)
	{
		result->ID = NULL;
		if (vertexPath && fragmentPath)
		{
			fopen_s(&vShaderFile, vertexPath, "r");
			fopen_s(&fShaderFile, fragmentPath, "r");
			if (vShaderFile && fShaderFile)
			{
#ifdef DEBUG
				printf("Successfully loaded vertex and fragment shader files\n");
#endif
				vertex = flen(vShaderFile);
				buffer = (char*)malloc(sizeof(char) * ((size_t)vertex + 1));
				if (buffer)
				{
					vertex = (GLuint)fread(buffer, sizeof(char), vertex, vShaderFile);
					buffer[vertex] = NULL;
				}
				fclose(vShaderFile);

#ifdef DEBUG
				printf("\n\nVertex shader code:\n\n\"%s\"\n\n", buffer ? buffer : NULL);
#endif

				vertex = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(vertex, 1, &buffer, NULL);
				glCompileShader(vertex);
				shader_chk_err(vertex, "VERTEX");
				free(buffer);

				fragment = flen(fShaderFile);
				buffer = (char*)malloc(sizeof(char) * ((size_t)fragment + 1));
				if (buffer)
				{
					fragment = (GLuint)fread(buffer, sizeof(char), fragment, fShaderFile);
					buffer[fragment] = NULL;
				}
				fclose(fShaderFile);

#ifdef DEBUG
				printf("\n\nFragment shader code:\n\n\"%s\"\n\n", buffer ? buffer : NULL);
#endif

				fragment = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fragment, 1, &buffer, NULL);
				glCompileShader(fragment);
				shader_chk_err(fragment, "FRAGMENT");
				free(buffer);

				if (geometryPath)
				{
					fopen_s(&vShaderFile, geometryPath, "r");
					if (vShaderFile)
					{
#ifdef DEBUG
						printf("Successfully loaded geometry shader file\n");
#endif
						geometry = flen(vShaderFile);
						buffer = (char*)malloc(sizeof(char) * ((size_t)geometry + 1));
						if (buffer)
						{
							geometry = (GLuint)fread(buffer, sizeof(char), geometry, vShaderFile);
							buffer[geometry] = NULL;
						}
						fclose(vShaderFile);

#ifdef DEBUG
						printf("\n\nGeometry shader code:\n\n\"%s\"\n\n", buffer ? buffer : NULL);
#endif

						geometry = glCreateShader(GL_GEOMETRY_SHADER);
						glShaderSource(geometry, 1, &buffer, NULL);
						glCompileShader(geometry);
						shader_chk_err(geometry, "GEOMETRY");
						free(buffer);
					}
#ifdef DEBUG
					else
						printf("Ignoring geometry shader: invalid file path\n");
#endif
				}
#ifdef DEBUG
				else
					printf("Ignoring geometry shader: NULL file path\n");
#endif

				result->ID = glCreateProgram();
				glAttachShader(result->ID, vertex);
				glAttachShader(result->ID, fragment);
				if (geometry)
					glAttachShader(result->ID, geometry);
				glLinkProgram(result->ID);
				shader_chk_err(result->ID, "PROGRAM");
				glDeleteShader(vertex);
				glDeleteShader(fragment);
				if (geometry)
					glDeleteShader(geometry);
#ifdef DEBUG
				printf("Successfully created shader\n");
#endif
			}
#ifdef DEBUG
			else
				printf("Invalid vertex and/or fragment file path\n");
#endif
		}
#ifdef DEBUG
		else
			printf("NULL vertex and/or fragment file path\n");
#endif
#ifdef DEBUG
		printf("Result ID is %d\n", result->ID);
#endif
	}
	return result;
}

shaderp shader_new(const char* vertexPath, const char* fragmentPath)
{
	return shader_new(vertexPath, fragmentPath, NULL);
}

void shader_chk_err(GLuint shader, const char* type)
{
	GLint success;
	GLchar infoLog[1024];
	const char *title;
	if (!strcmp("PROGRAM",type))
	{
		title = "SHADER_COMPILATION_ERROR";
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
	}
	else
	{
		title = "PROGRAM_LINKING_ERROR";
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
	}
	if (success)
		printf("SUCCESS: %s\n",type);
	else
		printf("ERROR:%s of type: %s\n%s\n\n", title, type, infoLog);
}

void shader_use(shaderp s)
{
	glUseProgram(s->ID);
}

void shader_release(shaderp s)
{
	glDeleteProgram(s->ID);
#ifdef DEBUG
	printf("Released shader\n");
#endif
}

void shader_set_b(shaderp s,const char* name, GLboolean value)
{
	glUniform1i(glGetUniformLocation(s->ID,name), value);
#ifdef DEBUG
	printf("Set boolean %s, value %s\n",name,value ? "true" : "false");
#endif
}

void shader_set_i(shaderp s, const char* name, GLint value)
{
	glUniform1i(glGetUniformLocation(s->ID,name), value);
#ifdef DEBUG
	printf("Set integer %s, value %i\n", name, value);
#endif
}

void shader_set_f(shaderp s,const char* name, GLfloat value)
{
	glUniform1f(glGetUniformLocation(s->ID,name), value);
#ifdef DEBUG
	printf("Set float %s, value %f\n", name, value);
#endif
}

void shader_get_f(shaderp s,const char* name, GLfloat* dest)
{
	glGetUniformfv(s->ID,glGetUniformLocation(s->ID, name), dest);
#ifdef DEBUG
	printf("Got float %s, value %f\n", name, *dest);
#endif
}

void shader_set_vec2(shaderp s,const char* name, GLfloat x, GLfloat y)
{
	glUniform2f(glGetUniformLocation(s->ID,name), x,y);
#ifdef DEBUG
	printf("Set vector2 %s, value %f, %f\n", name, x, y);
#endif
}

void shader_set_vec3(shaderp s,const char* name, GLfloat x, GLfloat y, GLfloat z)
{
	glUniform3f(glGetUniformLocation(s->ID,name), x,y,z);
#ifdef DEBUG
	printf("Set vector3 %s, value %f, %f, %f\n", name, x, y, z);
#endif
}

void shader_set_vec4(shaderp s,const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glUniform4f(glGetUniformLocation(s->ID,name), x,y,z,w);
#ifdef DEBUG
	printf("Set vector4 %s, value %f, %f, %f, %f\n", name, x, y, z, w);
#endif
}

void shader_set_vec2(shaderp s, const char* name, glm::vec2 v)
{
	glUniform2fv(glGetUniformLocation(s->ID, name), 1,&(v[0]));
#ifdef DEBUG
	printf("Set vector2 %s, value %f, %f\n", name, v.x,v.y);
#endif
}

void shader_set_vec3(shaderp s, const char* name, glm::vec3 v)
{
	glUniform3fv(glGetUniformLocation(s->ID, name), 1, &(v[0]));
#ifdef DEBUG
	printf("Set vector3 %s, value %f, %f, %f\n", name, v.x, v.y, v.z);
#endif
}

void shader_set_vec4(shaderp s, const char* name, glm::vec4 v)
{
	glUniform4fv(glGetUniformLocation(s->ID, name), 1, &(v[0]));
#ifdef DEBUG
	printf("Set vector4 %s, value %f, %f, %f, %f\n", name, v.x, v.y, v.z, v.w);
#endif
}

void shader_set_mat2(shaderp s, const char* name, glm::mat2 v)
{
	glUniformMatrix2fv(glGetUniformLocation(s->ID, name), 1, GL_FALSE, &(v[0][0]));
#ifdef DEBUG
	printf("Set mat2 %s, value %i\n", name, (GLuint)v.length);
#endif
}

void shader_set_mat3(shaderp s, const char* name, glm::mat3 v)
{
	glUniformMatrix3fv(glGetUniformLocation(s->ID, name), 1, GL_FALSE, &(v[0][0]));
#ifdef DEBUG
	printf("Set mat2 %s, value %i\n", name, (GLuint)v.length);
#endif
}

void shader_set_mat4(shaderp s, const char* name, glm::mat4 v)
{
	glUniformMatrix4fv(glGetUniformLocation(s->ID, name), 1, GL_FALSE, &(v[0][0]));
#ifdef DEBUG
	printf("Set mat2 %s, value %i\n", name, (GLuint)v.length);
#endif
}