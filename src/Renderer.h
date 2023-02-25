#pragma once
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdexcept>
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "Scene.h"

class Renderer
{
public:
	void init();
	void render(Scene*& scene);
	void close();
private:
	Shader m_shader;
	VAO m_vao;
	GLFWwindow* m_window = nullptr;

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
};

