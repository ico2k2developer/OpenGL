#include <glad/glad.h> // include glad to get the required OpenGL headers
#include "shader.h"
#include <stdlib.h>
#include "ico2k2.h"

shaderp shader_new(const char* vertexPath, const char* fragmentPath,const char* geometryPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	shaderp result = (shaderp)malloc(sizeof(shader));;
	char *buffer;
	FILE *vShaderFile,*fShaderFile;
	GLuint vertex,fragment,geometry = NULL;
	if (result)
	{
		result->ID = NULL;
		if (!vertexPath || !fragmentPath)
			printf("NULL vertex and/or fragment file path\n");
		else
		{
			fopen_s(&vShaderFile, vertexPath, "r");
			fopen_s(&fShaderFile, fragmentPath, "r");
			if (!vShaderFile || !fShaderFile)
				printf("Invalid vertex and/or fragment file path\n");
			else
			{
				printf("Successfully loaded vertex and fragment shader files\n");
				vertex = flen(vShaderFile);
				buffer = (char*)malloc(sizeof(char) * ((size_t)vertex + 1));
				if (buffer)
				{
					vertex = (GLuint)fread(buffer, sizeof(char), vertex, vShaderFile);
					buffer[vertex] = NULL;
				}
				fclose(vShaderFile);

				//printf("\n\Vertex shader code:\n\n\"%s\"\n\n", buffer ? buffer : NULL);

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

				//printf("\n\nFragment shader code:\n\n\"%s\"\n\n", buffer ? buffer : NULL);

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
						printf("Successfully loaded geometry shader file\n");
						geometry = flen(vShaderFile);
						buffer = (char*)malloc(sizeof(char) * ((size_t)geometry + 1));
						if (buffer)
						{
							geometry = (GLuint)fread(buffer, sizeof(char), geometry, vShaderFile);
							buffer[geometry] = NULL;
						}
						fclose(vShaderFile);

						//printf("Geometry shader code:\n\n\"%s\"\n\n", buffer ? buffer : NULL);

						geometry = glCreateShader(GL_GEOMETRY_SHADER);
						glShaderSource(geometry, 1, &buffer, NULL);
						glCompileShader(geometry);
						shader_chk_err(geometry, "GEOMETRY");
						free(buffer);
					}
					else
						printf("Ignoring geometry shader: invalid file path\n");
				}
				else
					printf("Ignoring geometry shader: NULL file path\n");

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
				printf("Successfully created shader\n");
			}
		}
		printf("Result ID is %d\n", result->ID);
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
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		}
	}
	else
	{
		title = "PROGRAM_LINKING_ERROR";		}
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
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
	printf("Releasing shader\n");
	glDeleteProgram(s->ID);
}

void shader_set_b(shaderp s,const char* name, bool value)
{
	glUniform1i(glGetUniformLocation(s->ID,name), value);
	printf("Set boolean %s, value %s\n",name,value ? "true" : "false");
}

void shader_set_i(shaderp s, const char* name, int value)
{
	glUniform1i(glGetUniformLocation(s->ID,name), value);
	printf("Set integer %s, value %i\n", name, value);
}

void shader_set_f(shaderp s,const char* name, float value)
{
	glUniform1f(glGetUniformLocation(s->ID,name), value);
	printf("Set float %s, value %f\n", name, value);
}

void shader_set_vec2(shaderp s,const char* name, float x, float y)
{
	glUniform2f(glGetUniformLocation(s->ID,name), x,y);
	printf("Set vector2 %s, value %f, %f\n", name, x, y);
}

void shader_set_vec3(shaderp s,const char* name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(s->ID,name), x,y,z);
	printf("Set vector3 %s, value %f, %f, %f\n", name, x, y, z);
}

void shader_set_vec4(shaderp s,const char* name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(s->ID,name), x,y,z,w);
	printf("Set vector4 %s, value %f, %f, %f, %f\n", name, x, y, z, w);
}