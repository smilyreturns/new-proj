#ifndef _UTILITY_H_
#define _UTILITY_H_

typedef unsigned int GLuint;

bool loadShaders(char* a_vertShaderFile, char* a_fragShaderFile, GLuint* a_result);
bool checkProgramError(unsigned int a_program);
#endif