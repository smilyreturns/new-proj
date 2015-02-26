#include "Utility.h"
#include <cstdio>
#include <iostream>
#include "gl_core_4_4.h"

#include <vector>

bool loadShaders(char* a_vertShaderFile, char* a_fragShaderFile, GLuint* a_result)
{
	bool loadSucceeded = false;

	FILE* vertFile = fopen(a_vertShaderFile, "r");
	FILE* fragFile = fopen(a_fragShaderFile, "r");

	if (vertFile == 0 || fragFile == 0)
	{
		fclose(vertFile);
		fclose(fragFile);
	}
	else
	{
		fseek(vertFile, 0, SEEK_END);
		int vertFileLength = ftell(vertFile);
		fseek(vertFile, 0, SEEK_SET);

		fseek(fragFile, 0, SEEK_END);
		int fragFileLength = ftell(fragFile);
		fseek(fragFile, 0, SEEK_SET);

		char* vsSource = new char[vertFileLength];
		char* fsSource = new char[fragFileLength];

		vertFileLength = fread(vsSource, 1, vertFileLength, vertFile);
		fragFileLength = fread(fsSource, 1, fragFileLength, fragFile);

		loadSucceeded = true;

		unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertShader, 1, &vsSource, &vertFileLength);
		glCompileShader(vertShader);

		glShaderSource(fragShader, 1, &fsSource, &fragFileLength);
		glCompileShader(fragShader);

		*a_result = glCreateProgram();
		glAttachShader(*a_result, vertShader);
		glAttachShader(*a_result, fragShader);
		glLinkProgram(*a_result);

		checkProgramError(*a_result);

		glDeleteShader(fragShader);
		glDeleteShader(vertShader);

		delete[] fsSource;
		delete[] vsSource;
	}
	return loadSucceeded;
}

bool checkProgramError(unsigned int a_program)
{
	//Error checking
	int success = GL_FALSE;
	glGetProgramiv(a_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(a_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength <= 0)
		{
			return false;
		}
		std::vector<GLchar> infoLog(infoLogLength);
		glGetProgramInfoLog(a_program, infoLogLength, &infoLogLength, &infoLog[0]);
		std::cout << "Error: Failed to link shader program!" << std::endl;
		for (unsigned int i = 0; i < infoLog.size(); ++i)
		{
			std::cout << infoLog[i];
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}