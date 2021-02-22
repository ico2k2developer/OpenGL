#include "OpenGL.h"

void processInput(GLFWwindow* window,shaderp sH, glm::mat4* transform);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
	const GLfloat vertices[] = {
		// positions					// colors           // texture coords
		 POS_VALUE,	 POS_VALUE, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 POS_VALUE,	-POS_VALUE, 0.0f,	0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		 -POS_VALUE,-POS_VALUE, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		 -POS_VALUE,POS_VALUE,	0.0f,	1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	const GLuint vertexAttribs[] = {
		3, 3, 2,
	};

	const GLuint indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	const char* textureFiles[] = {
		TEXTURE0_FILE,
		TEXTURE1_FILE,
	};

	const GLuint textureSettings[] =
	{
		GL_MIRRORED_REPEAT,	GL_MIRRORED_REPEAT,		//GL_TEXTURE_WRAP_S			GL_TEXTURE_WRAP_T
		GL_NEAREST,			GL_NEAREST,				//GL_TEXTURE_MIN_FILTER		GL_TEXTURE_MAG_FILTER
	};

	FT_Library ft;
	GLuint VBO, VAO,EBO,tVBO,tVAO;
	arrp tmp = NULL;
	chr_ttf* font;
	GLFWwindow* window;
	FT_Face face;
	shaderp s, ts;
	tVBO = NULL;
	tVAO = NULL;

	printf("Compiled in %s for %s using %s on %s %s\n%s version\n\n",
		__LANGUAGE__, __OS__, __COMPILER__, __DATE__, __TIME__, __VERSION__);
	printf("This project is compiled using and hence requires support for OpenGL %s\n\n", OPENGL_VERSION);

	printf("Log:\n");
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
	printf("Initialized OpenGL\n");

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	window = glfwCreateWindow(WND_WIDTH, WND_HEIGHT, "OpenGL test", NULL, NULL);
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

	s = shader_new(VERTEX_FILE, FRAGMENT_FILE);
	if (!s)
		printf("Failed to create main shader\n");

	ts = shader_new(TEXT_VERTEX_FILE, TEXT_FRAGMENT_FILE);
	if (!s)
		printf("Failed to create text shader\n");

	shader_use(ts);
	shader_set_mat4(ts,"projection", glm::ortho(0.0f, static_cast<float>(WND_WIDTH), 0.0f, static_cast<float>(WND_HEIGHT)));

	if (FT_Init_FreeType(&ft))
		printf("Failed to initialize FreeType Library\n");
	else
		printf("Initialized FreeType Library\n");

	if (FT_New_Face(ft,FONT_FILE,0,&face))
		printf("Failed to load font %s\n", FONT_FILE);
	else
		printf("Loaded font %s\n", FONT_FILE);

	FT_Set_Pixel_Sizes(face, 0, 25);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	font = (chr_ttf*)malloc(sizeof(chr_ttf) * FONT_COUNT);
	if (font)
	{
		for (VBO = 0; VBO < FONT_COUNT; VBO++)
		{
			if (FT_Load_Char(face, VBO + FONT_START, FT_LOAD_RENDER))
			{
				(font + VBO)->textureId = NULL;
				printf("Failed to load char %c from font %s\n", VBO, FONT_NAME);
			}
			else
			{
				(font + VBO)->sizeX = face->glyph->bitmap.width;
				(font + VBO)->sizeY = face->glyph->bitmap.rows;
				(font + VBO)->bearingX = face->glyph->bitmap_left;
				(font + VBO)->bearingY = face->glyph->bitmap_top;
				(font + VBO)->advance = (unsigned short)face->glyph->advance.x;
				glGenTextures(1, &((font + VBO)->textureId));
				glBindTexture(GL_TEXTURE_2D, (font + VBO)->textureId);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					(font + VBO)->sizeX,
					(font + VBO)->sizeY,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

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


	/*glGenVertexArrays(1, &tVAO);
	glGenBuffers(1, &tVBO);
	glBindVertexArray(tVAO);
	glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);*/


#ifdef FPS
	TIME_TYPE t = 0;
	char* fps = (char*)malloc(sizeof(char) * FPS_STRING);
	unsigned short count = 0;
#endif
	GLfloat f = 0;
#ifndef FRAME_SINGLE
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, s, &trans);
#endif

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		activateTextures(*tmp);


		shader_set_f(s, NAME_VALUE, f > 0 ? (f > 1.0f ? 1.0f : f) : (f > -1.0f ? 0.0f : -f - 1));
		//printf("f: %f result: %f\r", f, f > 1.0f ? 1.0f : (f > 0 ? f : -f));
		f = f >= 2.0f ? -2.0f : f + STEP;

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

#ifdef FPS
		if (fps)
		{
			count++;
			if (TIME_FUN() - t >= 1000)
			{
				t = TIME_FUN();
				sprintf_s(fps, FPS_STRING, "%u FPS", count);
				count = 0;
			}
			renderText(ts, font, fps, &tVAO, &tVBO, 3.0f, 3.0f, glm::vec3(0.5, 0.8f, 0.2f));
			shader_use(s);
		}
#endif

		glfwSwapBuffers(window);
		glfwPollEvents();
#ifndef FRAME_SINGLE
	}
#endif
	deleteArrays(&VBO, &VAO, &EBO);
	deleteTextures(*tmp);
	arr_del(tmp);
	free(font);
	shader_release(s);
	shader_release(ts);

#ifdef	EXIT_WAIT
	system("pause");
#endif

	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window,shaderp sh,glm::mat4* transform)
{
	GLfloat t1, t2, r1, r2;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_ENTER))
	{
		*transform = glm::mat4(1.0f);
	}
	else
	{
		t1 = glfwGetKey(window, GLFW_KEY_W) ? STEP : (glfwGetKey(window, GLFW_KEY_S) ? -STEP : 0.0f);
		t2 = glfwGetKey(window, GLFW_KEY_D) ? STEP : (glfwGetKey(window, GLFW_KEY_A) ? -STEP : 0.0f);
		r1 = glfwGetKey(window, GLFW_KEY_UP) ? STEP : (glfwGetKey(window, GLFW_KEY_DOWN) ? -STEP : 0.0f);
		r2 = glfwGetKey(window, GLFW_KEY_RIGHT) ? STEP : (glfwGetKey(window, GLFW_KEY_LEFT) ? -STEP : 0.0f);
		if (t1 || t2)
			*transform = glm::translate(*transform, glm::vec3(t2, t1, 0.0f));
		if(r1 || r2)
			*transform = glm::rotate(*transform,
				r1 == 0 ? r2 : r1,
				glm::vec3(r1 == 0 ? 0.0f : 1.0f,0.0f,r2 == 0 ? 0.0f : 1.0f));
		printf("t1: %f t2: %f r1: %f r2; %f\r",t1,t2,r1,r2);
	}
	shader_set_mat4(sh, "transform", *transform);
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

void renderText(shaderp s, chr_ttf* font,const char* text, GLuint* VAO, GLuint* VBO,GLfloat x, GLfloat y, glm::vec3 colour)
{
	size_t size = strlen(text);
	GLuint i;
	GLfloat xc, yc, wc, hc;
	GLfloat vertices[6][4];
	chr_ttf* c;
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader_use(s);
	shader_set_vec3(s, "textColour", colour);
#ifdef FRAME_SINGLE
	printf("Going to print \"%s\", length is %zu\n", text, size);
#endif
	if (!(*VAO) || !(*VBO))
	{
		glGenVertexArrays(1, VAO);
		glGenBuffers(1, VBO);
		glBindVertexArray(*VAO);
		glBindBuffer(GL_ARRAY_BUFFER, *VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		printf("Text VAO and/or VBO were NULL, created new\n");
	}
	else
		glBindVertexArray(*VAO);
	glActiveTexture(GL_TEXTURE0);
	for (i = 0; i < size; i++)
	{
		c = font + (text[i] - ((char)FONT_START));
#ifdef FRAME_SINGLE
		printf("Printing char %c, calculated index in font table is %u\t", text[i], text[i] - ((char)FONT_START));
#endif
		if (c != font && c->textureId != 0)
		{
#ifdef FRAME_SINGLE
			printf("Character detected\n");
#endif
			xc = x + (GLfloat)c->bearingX;
			yc = y - (GLfloat)(c->sizeY - c->bearingY);
			wc = (GLfloat)(c->sizeX);
			hc = (GLfloat)(c->sizeY);

			vertices[0][0] = xc;
			vertices[0][1] = yc + hc;
			vertices[0][2] = 0.0f;
			vertices[0][3] = 0.0f;

			vertices[1][0] = xc;
			vertices[1][1] = yc;
			vertices[1][2] = 0.0f;
			vertices[1][3] = 1.0f;

			vertices[2][0] = xc + wc;
			vertices[2][1] = yc;
			vertices[2][2] = 1.0f;
			vertices[2][3] = 1.0f;

			vertices[3][0] = xc;
			vertices[3][1] = yc + hc;
			vertices[3][2] = 0.0f;
			vertices[3][3] = 0.0f;

			vertices[4][0] = xc + wc;
			vertices[4][1] = yc;
			vertices[4][2] = 1.0f;
			vertices[4][3] = 1.0f;

			vertices[5][0] = xc + wc;
			vertices[5][1] = yc + hc;
			vertices[5][2] = 1.0f;
			vertices[5][3] = 0.0f;

			glBindTexture(GL_TEXTURE_2D, c->textureId);
			glBindBuffer(GL_ARRAY_BUFFER, *VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
#ifdef FRAME_SINGLE
		else
			printf("%s detected: skipping\n",c == font ? "Space" : "NULL texture");
#endif
		// advance cursors for next glyph (advance is 1/64 pixels)
		x += (c->advance >> 6); // bitshift by 6 (2^6 = 64)
	}
}