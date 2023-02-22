#pragma once
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdexcept>

class Renderer
{
public:
	void init();
	void render();
	void close();
private:
	GLFWwindow* m_window = nullptr;

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
};

