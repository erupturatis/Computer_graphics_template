//
// Created by eugen on 12/27/2024.
//

#ifndef OTHERS_H
#define OTHERS_H

#include <GL/glew.h>
#include <iostream>

GLenum glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)
void initOpenGLState();

#endif //OTHERS_H
