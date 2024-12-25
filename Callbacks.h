#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <GL/glew.h>
#include "GLFW/glfw3.h"

void windowResizeCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

#endif //CALLBACKS_H
