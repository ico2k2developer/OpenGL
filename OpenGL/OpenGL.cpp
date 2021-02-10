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

#define	TEXTURE0	"image1.jpg"
#define	TEXTURE1	"image2.jpg"

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
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

	const GLfloat vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
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

	const GLboolean textureCount = sizeof(textureFiles) / sizeof(const char*);

	GLuint VBO, VAO,EBO,*textures;
	setupArrays(&VBO, &VAO, &EBO,
		array_new((void*)vertices, sizeof(vertices) / sizeof(GLfloat), NULL),
		array_new((void*)indices, sizeof(indices) / sizeof(GLuint), NULL),
		array_new((void*)vertexAttribs, sizeof(vertexAttribs) / sizeof(GLuint), NULL));

	textures = (GLuint*)malloc(sizeof(GLuint) * textureCount);
	if (textures)
		setupTextures(textures, textureSettings, textureFiles, textureCount);
	else
		printf("Textures setup failed\n");

	shader_use(s);

	shader_set_i(s,"texture1", 0);
	shader_set_i(s,"texture2", 1);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind textures on corresponding texture units
		activateTextures(textures, textureCount);

		// render container
		shader_use(s);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	shader_release(s);

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

GLuint setupArrays(GLuint* VBO, GLuint* VAO, GLuint* EBO, const arrayp vertices, const arrayp indexes, const arrayp vertexAttribs)
{
	/*
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);*/
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER,vertices->size * sizeof(GLfloat), vertices->a, GL_STATIC_DRAW);

	if (indexes)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes->size * sizeof(GLuint), indexes->a, GL_STATIC_DRAW);
	}
	else
		printf("Ignoring indexes, array is null\n");

	size_t i;
	GLuint countRow = 0;

	for (i = 0; i < vertexAttribs->size; i++)
		countRow += ((GLuint*)vertexAttribs->a)[i];

	GLuint strive = 0;
	for (i = 0; i < vertexAttribs->size; i++)
	{
		glVertexAttribPointer((GLuint)i, ((GLuint*)vertexAttribs->a)[i], GL_FLOAT, GL_FALSE,(GLsizei)(countRow * sizeof(GLfloat)), (void*)(strive * sizeof(GLfloat)));
		glEnableVertexAttribArray((GLuint)i);
		strive += ((GLuint*)vertexAttribs->a)[i];
	}
	return countRow;
}

void setupTextures(GLuint *textures,const GLuint *textureSettings,const GLchar **textureFiles,const GLuint count)
{
	GLuint i;

	for (i = 0; i < count; i++)
	{
		glGenTextures(1,&(textures[i]));
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