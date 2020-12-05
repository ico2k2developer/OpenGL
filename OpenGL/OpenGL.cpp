#include <stdio.h>
#include <math.h>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "OpenGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define	WND_WIDTH	800
#define	WND_HEIGHT	600

#define VS_FILENAME	"shader.vs"
#define FS_FILENAME	"shader.fs"

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}
	glViewport(0, 0, WND_WIDTH, WND_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shader(VS_FILENAME, FS_FILENAME);

	const GLfloat vertices[] =
	{
		// positions // colors // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	const GLuint vertexAttribs[] = { 3,3,2 };

	const GLuint indices[] =
	{
		0, 1, 3, // first triangle
		1, 2, 3, // second triangle
	};

	const GLchar* textureFiles[] =
	{
		"image1.jpg",
		"image2.jpg",
	};

	const GLuint textureSettings[] =
	{
		GL_MIRRORED_REPEAT,	GL_MIRRORED_REPEAT,		//GL_TEXTURE_WRAP_S			GL_TEXTURE_WRAP_T
		GL_NEAREST,			GL_NEAREST,				//GL_TEXTURE_MIN_FILTER		GL_TEXTURE_MAG_FILTER
	};

	const GLuint renderCount = sizeof(textureFiles) / sizeof(GLchar*);

	GLuint VBO, VAO, EBO, i;
	setupArrays(&VBO, &VAO, &EBO, vertices, indices, vertexAttribs, sizeof(vertexAttribs) / sizeof(GLuint));
	
	GLuint textures[renderCount] = {};
	setupTextures(textures, textureSettings, textureFiles, renderCount);

	shader.use();

	shader.setInt("skin0", 0);
	shader.setInt("skin1", 1);

	/*time_t sec = time(NULL), tmp;
	GLuint fps = 0;*/
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		activateTextures(textures, renderCount);

		// now render the triangle
		//glBindTexture(GL_TEXTURE_2D, texture);
		shader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();

		/*fps++;

		tmp = time(NULL);
		if(tmp != sec)
		{
			sec = tmp;
			printf("FPS: %d\r", fps);
			fps = 0;
		}*/

	}
	glDeleteBuffers(renderCount,&VBO);
	glDeleteVertexArrays(renderCount,&VAO);
	glDeleteBuffers(renderCount,&EBO);
	for (i = 0; i < renderCount; i++)
	{
		glDeleteTextures(1,&textures[i]);
	}
	shader.release();

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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
	int width, height;
	GLboolean result;
	unsigned char* data = loadImage(filename, &width, &height,NULL);
	GLuint channels = alpha ? GL_RGBA : GL_RGB;
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

GLuint setupArrays(GLuint *VBO, GLuint *VAO, GLuint *EBO, const GLfloat *vertices, const GLuint *indices, const GLuint *vertexAttribs,const GLuint columns)
{
	glGenBuffers(1, VBO);
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, EBO);
	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint i, countRow = 0;
	for (i = 0; i < columns; i++)
		countRow += vertexAttribs[i];

	GLuint strive = 0;
	for (i = 0; i < columns; i++)
	{
		glVertexAttribPointer(i, vertexAttribs[i], GL_FLOAT, GL_FALSE, countRow * sizeof(float), (void*)(strive * sizeof(float)));
		glEnableVertexAttribArray(i);
		strive += vertexAttribs[i];
	}
	return countRow;
}

void setupTextures(GLuint *textures,const GLuint *textureSettings,const GLchar **textureFiles,const GLuint count)
{
	GLuint i;

	for (i = 0; i < count; i++)
	{
		glGenTextures(1,&textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureSettings[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureSettings[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureSettings[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureSettings[3]);
		if (loadTexture(textureFiles[i]))
			printf("Successfully loaded texture %s\n", textureFiles[i]);
		else
			printf("Failed loading texture %s\n", textureFiles[i]);
	}
}

void activateTextures(GLuint* textures, const GLuint count)
{
	GLuint i;
	for (i = 0; i < count; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}