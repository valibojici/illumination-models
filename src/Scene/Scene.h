#pragma once
#include <memory>
#include "GL/glew.h"
#include <imgui.h>

class Scene
{
protected:
	Scene*& m_currentScene;

	unsigned int m_width = 0;
	unsigned int m_height = 0;
public:
	Scene(Scene*& scene, unsigned int width, unsigned int height) : m_currentScene(scene), m_width(width), m_height(height) {}
	virtual void onRender() {}
	// TODO: add back button rendering here
	virtual void onRenderImGui() {}
	virtual void onUpdate() {}
	virtual void updateWidthHeight(unsigned int width, unsigned int height) {}
	virtual ~Scene() {}
	static void helpPoput(const char* text);
};

