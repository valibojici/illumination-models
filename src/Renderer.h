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
#include "Buffer/VBO.h"
#include "Scene/Scene.h"
#include "Event/EventManager.h"
#include "Scene/SceneMenu.h"

class Renderer : public EventHandler
{
public:
	void init();
	void render();
	void close();
private:
	GLFWwindow* m_window = nullptr;
	unsigned int m_windowWidth = 1280;
	unsigned int m_windowHeight = 720;
	bool m_showImguiWindow = true;
	bool m_setImguiWindowPos = false;
	
	Scene* m_scene;

	void handleEvent(const Event& e) override;

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
};

