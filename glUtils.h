#pragma once
#include <GL\glew.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

GLint getAttributeLocation(GLuint Program, const char* attr_name) {
	auto location = glGetAttribLocation(Program, attr_name);
	if (location == -1)
	{
		std::cout << "could not bind attrib " << attr_name << std::endl;
	}
	return location;
}

void shaderLog(unsigned int shader)
{
	int infologLen = 0;
	int charsWritten = 0;
	char* infoLog;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		infoLog = new char[infologLen];
		if (infoLog == NULL)
		{
			std::cout << "ERROR: Could not allocate InfoLog buffer\n";
			exit(1);
		}
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
		std::cout << "InfoLog: " << infoLog << "\n\n\n";
		delete[] infoLog;
	}
}

void checkOpenGLerror()
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error! - " << gluErrorString(errCode);
}

std::string readShader(const char* path) {
    std::string res = "";
	std::ifstream file(path);
	std::string line;
    getline(file, res, '\0');
    while (getline(file, line))
        res += "\n " + line;
    return res;
}

