#pragma once
#include <memory>
#include "GL/glew.h"
#include <imgui.h>

class Scene
{
protected:
	std::unique_ptr<Scene>& m_currentScene;

	unsigned int m_width = 0;
	unsigned int m_height = 0;
public:
	Scene(std::unique_ptr<Scene>& scene, unsigned int width, unsigned int height) : m_currentScene(scene), m_width(width), m_height(height) {}
	virtual void onRender() {}
	virtual void onRenderImGui() {}
	void setScene(std::unique_ptr<Scene> newScene) { m_currentScene = std::move(newScene); }
	virtual void updateWidthHeight(unsigned int width, unsigned int height) {}
	virtual ~Scene() {}
	static void helpPoput(const char* text);
};

