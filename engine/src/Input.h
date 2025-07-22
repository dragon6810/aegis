#pragma once

#include <unordered_map>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input
{
public:
	const static std::unordered_map<std::string, int> stringkeycodes;
	static std::unordered_map<int, std::string> keycommands;

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void CharCallback(GLFWwindow* window, unsigned int codepoint);
	static void CursorPosCallback(GLFWwindow* window, double x, double y);
};