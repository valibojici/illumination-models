#pragma once
#include "Scene/Scene.h"
#include "Scene/SceneMenu.h"
#include "Event/EventManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>

class App : public EventHandler
{
private:
	GLFWwindow* m_window = nullptr; // pointer to GLFW window
	unsigned int m_windowWidth = 1280; // current window width
	unsigned int m_windowHeight = 720; // current window height
	
	// flags for hiding/showing imgui UI on right click
	bool m_showImguiWindow = true;
	bool m_setImguiWindowPos = false;

	Scene* m_scene; // current scene to render

	/// <summary>
	/// Handles events (sets window height/width)
	/// </summary>
	void handleEvent(const Event& e) override;

	static void glfw_error_callback(int error, const char* description)
	{
		fprintf(stderr, "GLFW Error %d: %s\n", error, description);
	}
public:
	App();
	~App();
	void run();
};

