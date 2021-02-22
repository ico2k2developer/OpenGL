#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ico2k2.h"

#define	OPENGL_CORE		GLFW_OPENGL_CORE_PROFILE
#define	OPENGL_COMPAT	GLFW_OPENGL_COMPAT_PROFILE

#define	OPENGL_MAJOR	3
#define	OPENGL_MINOR	3
#define	OPENGL_PROFILE	OPENGL_CORE

#if OPENGL_PROFILE == GLFW_OPENGL_CORE_PROFILE
#define OPENGL_VERSION_PROF	"core"
#elif OPENGL_PROFILE == GLFW_OPENGL_COMPAT_PROFILE
#define OPENGL_VERSION_PROF	"compatibility"
#endif

#define	OPENGL_VERSION_CODE	STR(OPENGL_MAJOR) "." STR(OPENGL_MINOR)
#define	OPENGL_VERSION	OPENGL_VERSION_CODE " " OPENGL_VERSION_PROF

#define	WND_WIDTH	900
#define	WND_HEIGHT	600

#define	NAME_VALUE	"mixValue"
#define	STEP		0.005f
#define	POS_VALUE	0.75f

#define	FPS
#define	FPS_STRING	10

#define	RES_FOLDER	"res"

#define	TEXTURE0_FILE	"R" RES_FOLDER "\\" "image1.jpg"
#define	TEXTURE1_FILE	"R" RES_FOLDER "\\" "image2.jpg"

#define	FONT_NAME		"Roboto"
#define	FONT_FILE		RES_FOLDER "\\" FONT_NAME ".ttf"

#define VERTEX_FILE			RES_FOLDER "\\" "shader.vert"
#define FRAGMENT_FILE		RES_FOLDER "\\" "shader.frag"
#define TEXT_VERTEX_FILE	RES_FOLDER "\\" "text.vert"
#define TEXT_FRAGMENT_FILE	RES_FOLDER "\\" "text.frag"

#define	FONT_START		32
#define	FONT_END		127
#define	FONT_COUNT		FONT_END - FONT_START

#ifdef _WIN64
#define	TIME_FUN	GetTickCount64
#define	TIME_TYPE	ULONGLONG
#elif	_WIN32
#define	TIME_FUN	GetTickCount
#define	TIME_TYPE	DWORD
#else
#error	"Non-Windows target: need to define elapsed ms type and method"
#endif

//#define	FRAME_SINGLE
//#define	EXIT_WAIT

#define STB_IMAGE_IMPLEMENTATION