#include <stdio.h>
#include <math.h>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

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

	float vertices[] = {
		// positions // colors // texture coords
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top right
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f // top left
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBO,VAO,EBO;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int texture0,texture1;
	glGenTextures(1, &texture0);
	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	loadTexture("image.jpg");

	shader.use();

	time_t sec = time(NULL), tmp;
	unsigned short fps = 0;
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		/*glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);*/


		// now render the triangle
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &texture);
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

unsigned char* loadImage(const char *filename,int *width,int *height,int *channels)
{
	unsigned char* result;
	result = stbi_load(filename,width,height,channels, 0);
	if(!result)
		printf("Error when loading image file\n\t%s\n",filename);
	return result;
}

bool loadTexture(const char* filename)
{
	int width, height, channels;
	bool result;
	unsigned char* data = loadImage(filename, &width, &height, &channels);
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