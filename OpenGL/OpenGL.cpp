#include <stdio.h>
#include <math.h>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

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

	float vertices[] =
	{
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	};

	unsigned int VBO;
	glGenBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	shader.use();

	time_t sec = time(NULL), tmp;
	unsigned short fps = 0;
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// now render the triangle
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();

		fps++;

		tmp = time(NULL);
		if(tmp != sec)
		{
			sec = tmp;
			printf("FPS: %d\r", fps);
			fps = 0;
		}

	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
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