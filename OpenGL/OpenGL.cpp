#include "shader.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "OpenGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define	WND_WIDTH	900
#define	WND_HEIGHT	600

#define VS_FILENAME	"vertex.gls"
#define FS_FILENAME	"fragment.gls"

//#define	FPS			10

#define	USE_MY_IMAGES

#ifdef USE_MY_IMAGES
#define	TEXTURE0	"Rimage1.jpg"
#define	TEXTURE1	"Rimage2.jpg"
#else
#define	TEXTURE0	"Rcontainer.jpg"
#define	TEXTURE1	"Aawesomeface.png"
#endif

#define	NAME_VALUE	"mixValue"
#define	STEP		0.005f

void processInput(GLFWwindow* window,shaderp s, glm::mat4* transform);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
	const GLfloat vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f,	-0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		 -0.5f,	-0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		 -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	const GLuint vertexAttribs[] = {
		3, 3, 2,
	};

	const GLuint indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	const char* textureFiles[] = {
		TEXTURE0,
		TEXTURE1,
	};

	const GLuint textureSettings[] =
	{
		GL_MIRRORED_REPEAT,	GL_MIRRORED_REPEAT,		//GL_TEXTURE_WRAP_S			GL_TEXTURE_WRAP_T
		GL_NEAREST,			GL_NEAREST,				//GL_TEXTURE_MIN_FILTER		GL_TEXTURE_MAG_FILTER
	};

	GLuint VBO, VAO,EBO;
	arrp tmp = NULL;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	printf("Initialized OpenGL\n");

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	GLFWwindow* window = glfwCreateWindow(WND_WIDTH, WND_HEIGHT, "OpenGL test", NULL, NULL);
	if(window == NULL)
	{
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	else
		printf("Created GLFW window\n");
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}
	else
		printf("Initialized GLAD\n");

	shaderp s = shader_new(VS_FILENAME, FS_FILENAME);
	if (!s)
		printf("Failed to create shader\n");

	setupArrays(&VBO, &VAO, &EBO,
		*arr_snew((void*)vertices, sizeof(vertices),sizeof(GLfloat), &tmp),
		*arr_snew((void*)indices, sizeof(indices),sizeof(GLuint), &tmp),
		*arr_snew((void*)vertexAttribs, sizeof(vertexAttribs), sizeof(GLuint), &tmp));
	arr_snew(NULL, sizeof(textureFiles), sizeof(const char*), &tmp);
	if (!tmp)
		return 1;
	stbi_set_flip_vertically_on_load(true);
	if (tmp)
		setupTextures(*tmp, textureSettings, textureFiles);
	else
		printf("Textures setup failed\n");

	shader_use(s);

	shader_set_i(s,"texture1", 0);
	shader_set_i(s,"texture2", 1);

	glm::mat4 trans = glm::mat4(1.0f);

	shader_set_mat4(s, "transform", trans);
	//trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

	#ifdef FPS
	ULONGLONG t = 0;
	const unsigned short frameT = 1000 / FPS;
	#endif
	GLfloat f = 0;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window,s,&trans);
	#ifdef FPS
		if (GetTickCount64() - t < frameT)
			continue;
		else
			t = GetTickCount64();
	#endif

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		activateTextures(*tmp);
		shader_use(s);


		shader_set_f(s, NAME_VALUE, f > 0 ? f : -f);
		if (f >= 1.0)
			f = -1.0;
		f += STEP;


		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	deleteArrays(&VBO, &VAO, &EBO);
	deleteTextures(*tmp);
	arr_del(tmp);

	shader_release(s);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window,shaderp s,glm::mat4* transform)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		*transform = glm::translate(*transform, glm::vec3(-STEP, 0.0, 0.0));
		shader_set_mat4(s, "transform", *transform);
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		*transform = glm::translate(*transform, glm::vec3(STEP, 0.0, 0.0));
		shader_set_mat4(s, "transform", *transform);
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		*transform = glm::translate(*transform, glm::vec3(0.0, STEP, 0.0));
		shader_set_mat4(s, "transform", *transform);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		*transform = glm::translate(*transform, glm::vec3(0.0, -STEP, 0.0));
		shader_set_mat4(s, "transform", *transform);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		*transform = glm::rotate(*transform, -STEP,glm::vec3(0.0, 0.0, 1.0));
		shader_set_mat4(s, "transform", *transform);
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		*transform = glm::rotate(*transform, STEP, glm::vec3(0.0, 0.0, 1.0));
		shader_set_mat4(s, "transform", *transform);
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		*transform = glm::rotate(*transform, STEP, glm::vec3(1.0, 0.0, 0.0));
		shader_set_mat4(s, "transform", *transform);
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		*transform = glm::rotate(*transform, -STEP, glm::vec3(1.0, 0.0, 0.0));
		shader_set_mat4(s, "transform", *transform);
	}
	else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		*transform = glm::mat4(1.0f);
		shader_set_mat4(s, "transform", *transform);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	printf("Resized! %dx%d\n", width, height);
}

unsigned char* loadImage(const GLchar *filename,int *width,int *height,int *channels)
{
	unsigned char* result;
	result = stbi_load(filename,width,height,channels, 0);
	if(!result)
		printf("Error when loading image file %s\n",filename);
	return result;
}

bool loadTexture(const GLchar* filename,bool alpha)
{
	GLint width, height,channels;
	GLboolean result;
	unsigned char* data = loadImage(filename, &width, &height,&channels);
	channels = alpha ? GL_RGBA : GL_RGB;
	if (result = (data != NULL))
	{
		glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);
	return result;
}

bool loadTexture(const GLchar* filename)
{
	return loadTexture(filename, false);
}

GLuint setupArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO, arr vertices, arr indexes, arr vertexAttribs)
{
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER,arr_size(&vertices),arr_arr(&vertices), GL_STATIC_DRAW);

	if (&indexes)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,arr_size(&indexes),arr_arr(&indexes), GL_STATIC_DRAW);
	}
	else
		printf("Ignoring indexes, array is null\n");

	size_t i;
	GLuint countRow = 0;

	for (i = 0; i < arr_length(&vertexAttribs); i++)
		countRow += ((GLuint*)arr_arr(&vertexAttribs))[i];

	GLuint strive = 0;
	for (i = 0; i < arr_length(&vertexAttribs); i++)
	{
		glVertexAttribPointer((GLuint)i, ((GLuint*)arr_arr(&vertexAttribs))[i], GL_FLOAT, GL_FALSE,(GLsizei)(countRow * sizeof(GLfloat)), (void*)(strive * sizeof(GLfloat)));
		glEnableVertexAttribArray((GLuint)i);
		strive += ((GLuint*)arr_arr(&vertexAttribs))[i];
	}
	return countRow;
}

void deleteArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO)
{
	glDeleteVertexArrays(1,VAO);
	glDeleteBuffers(1,VBO);
	glDeleteBuffers(1,EBO);
}

void setupTextures(arr textures,const GLuint *textureSettings,const GLchar **textureFiles)
{
	GLuint i;
	for (i = 0; i < arr_length(&textures); i++)
	{
		glGenTextures(1,(GLuint*)(arr_arr(&textures)) + i);
		glBindTexture(GL_TEXTURE_2D, ((GLuint*)(arr_arr(&textures)))[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureSettings[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureSettings[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureSettings[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureSettings[3]);
		if (loadTexture(textureFiles[i] + 1,textureFiles[i][0] == 'A'))
			printf("Successfully loaded texture %s\n", textureFiles[i] + 1);
		else
			printf("Failed loading texture %s\n", textureFiles[i] + 1);
	}
}

void activateTextures(arr textures)
{
	GLuint i;
	for (i = 0; i < arr_length(&textures); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, ((GLuint*)(arr_arr(&textures)))[i]);
	}
}

void deleteTextures(arr textures)
{
	glDeleteTextures((GLsizei)arr_length(&textures),(GLuint*)arr_arr(&textures));
}