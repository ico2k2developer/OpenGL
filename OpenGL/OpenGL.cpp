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
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top right
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f // top left
	};

	const GLuint vertexAttribs[] = { 3,3,2 };

	const GLuint indices[] =
	{
		0, 1, 2, // first triangle
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

	const GLuint renderCount = 2;

	GLuint i,*VBO,*VAO,*EBO,*textures;

	VBO = new GLuint[renderCount];
	VAO = new GLuint[renderCount];
	EBO = new GLuint[renderCount];
	textures = new GLuint[renderCount];

	glGenBuffers(renderCount,VBO);
	glGenVertexArrays(renderCount,VAO);
	glGenBuffers(renderCount,EBO);
	glGenTextures(renderCount,textures);

	for (i = 0; i < renderCount; i++)
	{
		setupArrays(VBO[i], VAO[i], EBO[i], vertices, indices + (sizeof(indices) / sizeof(unsigned int)) * i, vertexAttribs,types);
		setupTextures(textures, textureSettings, textureFiles,renderCount);
	}

	shader.use();

	/*time_t sec = time(NULL), tmp;
	GLuint fps = 0;*/
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// now render the triangle
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glBindVertexArray(VAO);
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
	glDeleteVertexArrays(renderCount,VAO);
	glDeleteBuffers(renderCount,VBO);
	glDeleteBuffers(renderCount,EBO);
	glDeleteTextures(renderCount,textures);
	delete[] VBO;
	delete[] VAO;
	delete[] EBO;
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
		printf("Error when loading image file\n\t%s\n",filename);
	return result;
}

bool loadTexture(const GLchar* filename)
{
	int width, height;
	GLboolean result;
	unsigned char* data = loadImage(filename, &width, &height,NULL);
	if (result = (data != NULL))
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		printf("Error when loading texture\n");
	stbi_image_free(data);
	return result;
}

void setupArrays(const GLuint VBO, const GLuint VAO, const GLuint EBO, const GLfloat vertices[], const GLuint indices[], const GLuint vertexAttribs[])
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint i, countRow = 0,columns = ;
	for (i = 0; i < columns; i++)
		countRow += vertexAttribs[i];

	GLuint strive = 0;
	for (i = 0; i < columns; i++)
	{
		glVertexAttribPointer(i, vertexAttribs[i], GL_FLOAT, GL_FALSE, countRow * sizeof(float), (void*)(strive * sizeof(float)));
		glEnableVertexAttribArray(i);
		strive += vertexAttribs[i];
	}
}

void setupTextures(const GLuint textures[],const GLuint textureSettings[],const GLchar* textureFiles[])
{
	GLuint i;

	for (i = 0; i < count; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureSettings[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureSettings[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureSettings[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureSettings[3]);
		loadTexture(textureFiles[i]);
	}

}